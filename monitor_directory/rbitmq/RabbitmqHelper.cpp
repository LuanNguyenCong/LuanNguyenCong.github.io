#include "RabbitmqHelper.h"
//#include <io.h>
//#include <process.h>
#include <map>
#include <sstream>
#include <time.h>

namespace edsolabs {
    // MQ action map
    static std::map<MQActionType, std::string> g_mapMQAction
            {
                    { kMQActionTypeLogin, "Logging in" },
                    { kMQActionTypeCloseConnection, "Closing connection" },
                    { kMQActionTypeExchangeDeclare, "exchange_declare" },
                    { kMQActionTypeDeclareQueue, "queue_declare"},
                    { kMQActionTypeBindQueue, "queue_bind" },
                    { kMQActionTypeUnbindQueue, "queue_unbind" },
                    { kMQActionTypeOpenChannel,"open channel" },
                    { kMQActionTypeDeleteQueue, "delete queue" },
                    { kMQActionTypePublishMessage, "amqp_basic_publish" },
                    { kMQActionTypeConsumer, "Consuming" }
            };

// MQ error message
    static std::map<MQErrorCode, std::string> g_mapErrorMsg
            {
                    { kMQErrorCodeOK, "success" },
                    { kMQErrorCodeNewConnFailed, "Create a new connect failed" },
                    { kMQErrorCodeInvalidConn, "Invalid connection instance" },
                    { kMQErrorCodeNewSockError, "Create a new socket failed"},
                    { kMQErrorCodeOpenSockError, "Open socket failed" },
                    { kMQErrorCodeLoginFailed, "Login failed" },
                    { kMQErrorCodeCloseConnFailed, "Close connection failed" },
                    { kMQErrorCodeDestoryConnFailed, "Destory connection failed" },
                    { kMQErrorCodeDeclareExchangeFailed, "Declare exchange failed" },
                    { kMQErrorCodeDeclareQueueFailed, "Declare queue failed" },
                    { kMQErrorCodeBindQueueFailed, "Bind queque failed" },
                    { kMQErrorCodeUnbindQueueFailed, "Unbind queue failed" },
                    { kMQErrorCodeOpenChannelFailed, "Open channel failed" },
                    { kMQErrorCodeDelteQueueFailed, "Delete queue failed" },
                    { kMQErrorCodePublishFailed, "Publish message failed" },
                    { kMQErrorCodeConsumerFailed, "Consume message failed" }
            };

    RabbitmqHelper::RabbitmqHelper()
            : m_pConn(nullptr)
            , m_pSock(nullptr)

    {

    }

    RabbitmqHelper::RabbitmqHelper(const std::string& strIP,
                                   int nPort,
                                   const std::string& strUser,
                                   const std::string& strPasswd,
                                   int nChannel/* = 1*/,
                                   timeval* pTimeout/* = nullptr*/)
            : m_pConn(nullptr)
            , m_pSock(nullptr)
            , m_infoServer(strIP, nPort, strUser, strPasswd, nChannel, pTimeout)
    {

    }

    RabbitmqHelper::~RabbitmqHelper()
    {
        if (nullptr != m_pConn)
        {
            Disconnect();
            m_pConn = nullptr;
        }
    }

    MQErrorInfo RabbitmqHelper::Connect()
    {
        MQErrorInfo infoError;
        // create a connection
        m_pConn = amqp_new_connection();
        if (nullptr == m_pConn)
        {
            infoError.nCode = kMQErrorCodeNewConnFailed;
            infoError.strErrorMsg = get_error_msg(kMQErrorCodeNewConnFailed);
            return infoError;
        }
        // create a socket
        m_pSock = amqp_tcp_socket_new(m_pConn);
        if (nullptr == m_pSock)
        {
            infoError.nCode = kMQErrorCodeNewSockError;
            infoError.strErrorMsg = get_error_msg(kMQErrorCodeNewSockError);
            return infoError;
        }
        // bind ip with port
        int nState = amqp_socket_open_noblock(m_pSock, m_infoServer.strIP.c_str(), m_infoServer.nPort, m_infoServer.pTimeout);
        //int nState = amqp_socket_open(m_pSock, m_infoServer.strIP.c_str(), m_infoServer.nPort);
        if (nState < 0)
        {
            infoError.nCode = kMQErrorCodeOpenSockError;
            infoError.strErrorMsg = amqp_error_string2(nState);
            return infoError;
        }

        // login MQ with user and password
        auto reply = amqp_login(m_pConn,
                                "/",
                                0,
                                kDefaultMaxFrame,
                                kHeartbeat,
                                AMQP_SASL_METHOD_PLAIN,
                                m_infoServer.strUser.c_str(),
                                m_infoServer.strPasswd.c_str());

        if (infoError.strErrorMsg = get_reply_info(reply, kMQActionTypeLogin), !infoError.strErrorMsg.empty())
        {
            infoError.nCode = kMQErrorCodeLoginFailed;
            return infoError;
        }

        return infoError;
    }

    MQErrorInfo RabbitmqHelper::Disconnect()
    {
        MQErrorInfo infoError;
        if (nullptr != m_pConn)
        {
            if (infoError.strErrorMsg = get_reply_info(amqp_connection_close(m_pConn, AMQP_REPLY_SUCCESS), kMQActionTypeCloseConnection), !infoError.strErrorMsg.empty())
            {
                infoError.nCode = kMQErrorCodeCloseConnFailed;
            }
            else
            {
                if (amqp_destroy_connection(m_pConn) < 0)
                    infoError.nCode = kMQErrorCodeDestoryConnFailed;
            }
            m_pConn = nullptr;
        }

        return infoError;
    }

    MQErrorInfo RabbitmqHelper::OpenChannel()
    {
        MQErrorInfo infoError;
        if (nullptr == m_pConn)
        {
            infoError.nCode = kMQErrorCodeInvalidConn;
            infoError.strErrorMsg = get_error_msg(kMQErrorCodeInvalidConn);
            return infoError;
        }
        // open channel
        amqp_channel_open(m_pConn, m_infoServer.nChannel);
        if (infoError.strErrorMsg = get_reply_info(amqp_get_rpc_reply(m_pConn), kMQActionTypeOpenChannel), !infoError.strErrorMsg.empty())
        {
            amqp_channel_close(m_pConn, m_infoServer.nChannel, AMQP_REPLY_SUCCESS);
            infoError.nCode = kMQErrorCodeOpenChannelFailed;
            return infoError;
        }

        return infoError;
    }

    MQErrorInfo RabbitmqHelper::DeclareExchange(const MQExchangeInfo& infoExchange)
    {
        MQErrorInfo infoError;
        // open channel
        infoError = OpenChannel();
        if (infoError.nCode != kMQErrorCodeOK) return infoError;

        // declare exchange
        auto btExchange = string_2_amqp_bytes(infoExchange.strExchangeName);
        // it needs to store the exchange name in cache, or it destoried
        auto strExchangeName = get_exchange_type_name(infoExchange.nType);
        auto btType = string_2_amqp_bytes(strExchangeName);

        amqp_exchange_declare(m_pConn, m_infoServer.nChannel, btExchange, btType, infoExchange.nPassive, infoExchange.nDurable, infoExchange.nAutoDel, infoExchange.nInternal, amqp_empty_table);
        if (infoError.strErrorMsg = get_reply_info(amqp_get_rpc_reply(m_pConn), kMQActionTypeExchangeDeclare), !infoError.strErrorMsg.empty())
        {
            amqp_channel_close(m_pConn, m_infoServer.nChannel, AMQP_REPLY_SUCCESS);
            infoError.nCode = kMQErrorCodeDeclareExchangeFailed;
            return infoError;
        }

        amqp_channel_close(m_pConn, m_infoServer.nChannel, AMQP_REPLY_SUCCESS);
        return infoError;
    }

    MQErrorInfo RabbitmqHelper::DeclareQueue(const MQQueueInfo& infoQueue)
    {
        MQErrorInfo infoError;
        // open channel
        infoError = OpenChannel();
        if (infoError.nCode != kMQErrorCodeOK) return infoError;

        // declare queue
        auto btQueue = string_2_amqp_bytes(infoQueue.strQueueName);
        amqp_queue_declare(m_pConn, m_infoServer.nChannel, btQueue, infoQueue.nPassive, infoQueue.nDurable, infoQueue.nExclusive, infoQueue.nAutoDel, amqp_empty_table);
        if (infoError.strErrorMsg = get_reply_info(amqp_get_rpc_reply(m_pConn), kMQActionTypeDeclareQueue), !infoError.strErrorMsg.empty())
        {
            amqp_channel_close(m_pConn, m_infoServer.nChannel, AMQP_REPLY_SUCCESS);
            infoError.nCode = kMQErrorCodeDeclareQueueFailed;
            return infoError;
        }

        amqp_channel_close(m_pConn, m_infoServer.nChannel, AMQP_REPLY_SUCCESS);
        return infoError;
    }

    MQErrorInfo RabbitmqHelper::QueueBind(const MQQueueInfo& infoQueue)
    {
        MQErrorInfo infoError;
        // open channel
        infoError = OpenChannel();
        if (infoError.nCode != kMQErrorCodeOK) return infoError;

        // bind queue
        auto btQueue = string_2_amqp_bytes(infoQueue.strQueueName);
        auto btExchange = string_2_amqp_bytes(infoQueue.strExchangeName);
        auto btRouteKey = string_2_amqp_bytes(infoQueue.strRouteKey);
        amqp_queue_bind(m_pConn, m_infoServer.nChannel, btQueue, btExchange, btRouteKey, amqp_empty_table);
        if (infoError.strErrorMsg = get_reply_info(amqp_get_rpc_reply(m_pConn), kMQActionTypeBindQueue), !infoError.strErrorMsg.empty())
        {
            amqp_channel_close(m_pConn, m_infoServer.nChannel, AMQP_REPLY_SUCCESS);
            infoError.nCode = kMQErrorCodeBindQueueFailed;
            return infoError;
        }

        amqp_channel_close(m_pConn, m_infoServer.nChannel, AMQP_REPLY_SUCCESS);
        return infoError;
    }

    MQErrorInfo RabbitmqHelper::QueueUnbind(const MQQueueInfo& infoQueue)
    {
        MQErrorInfo infoError;
        // open channel
        infoError = OpenChannel();
        if (infoError.nCode != kMQErrorCodeOK) return infoError;

        // unbind queue
        auto btQueue = string_2_amqp_bytes(infoQueue.strQueueName);
        auto btExchange = string_2_amqp_bytes(infoQueue.strExchangeName);
        auto btRouteKey = string_2_amqp_bytes(infoQueue.strRouteKey);
        amqp_queue_unbind(m_pConn, m_infoServer.nChannel, btQueue, btExchange, btRouteKey, amqp_empty_table);
        if (infoError.strErrorMsg = get_reply_info(amqp_get_rpc_reply(m_pConn), kMQActionTypeUnbindQueue), !infoError.strErrorMsg.empty())
        {
            amqp_channel_close(m_pConn, m_infoServer.nChannel, AMQP_REPLY_SUCCESS);
            infoError.nCode = kMQErrorCodeUnbindQueueFailed;
            return infoError;
        }

        amqp_channel_close(m_pConn, m_infoServer.nChannel, AMQP_REPLY_SUCCESS);
        return infoError;
    }

    MQErrorInfo RabbitmqHelper::QueueDelete(const std::string& strQueueName, int nUnused, int nEmpty)
    {
        MQErrorInfo infoError;
        // open channel
        infoError = OpenChannel();
        if (infoError.nCode != kMQErrorCodeOK) return infoError;

        // delete queue
        amqp_queue_delete(m_pConn, m_infoServer.nChannel, amqp_cstring_bytes(strQueueName.c_str()), nUnused, nEmpty);
        if (infoError.strErrorMsg = get_reply_info(amqp_get_rpc_reply(m_pConn), kMQActionTypeDeleteQueue), !infoError.strErrorMsg.empty())
        {
            amqp_channel_close(m_pConn, m_infoServer.nChannel, AMQP_REPLY_SUCCESS);
            infoError.nCode = kMQErrorCodeDelteQueueFailed;
            return infoError;
        }

        amqp_channel_close(m_pConn, m_infoServer.nChannel, AMQP_REPLY_SUCCESS);
        return infoError;
    }

    MQErrorInfo RabbitmqHelper::Publish(const std::string& strMessage, const std::string& strExchange, const std::string& strRoutekey)
    {
        MQErrorInfo infoError;
        // open channel
        infoError = OpenChannel();
        if (infoError.nCode != kMQErrorCodeOK) return infoError;

        // publish message
        auto btMessage = string_2_amqp_bytes(strMessage);
        auto btExchangeName = string_2_amqp_bytes(strExchange);
        auto btRouteKey = string_2_amqp_bytes(strRoutekey);

        if (0 != amqp_basic_publish(m_pConn, m_infoServer.nChannel, btExchangeName, btRouteKey, 0, 0, nullptr, btMessage))
        {
            fprintf(stderr, "publish amqp_basic_publish failed\n");
            if (infoError.strErrorMsg = get_reply_info(amqp_get_rpc_reply(m_pConn), kMQActionTypePublishMessage), !infoError.strErrorMsg.empty())
            {
                amqp_channel_close(m_pConn, m_infoServer.nChannel, AMQP_REPLY_SUCCESS);
                infoError.nCode = kMQErrorCodePublishFailed;
                return infoError;
            }
        }

        amqp_channel_close(m_pConn, m_infoServer.nChannel, AMQP_REPLY_SUCCESS);
        return infoError;
    }
    MQErrorInfo RabbitmqHelper::Publish(const char* data,size_t data_size, const std::string& strExchange, const std::string& strRoutekey)
    {
        MQErrorInfo infoError;
        // open channel
        infoError = OpenChannel();
        if (infoError.nCode != kMQErrorCodeOK) return infoError;

        // publish message
        auto btMessage = amqp_bytes_malloc(data_size);
        btMessage.bytes = (void*)data;
        btMessage.len = data_size;
        auto btExchangeName = string_2_amqp_bytes(strExchange);
        auto btRouteKey = string_2_amqp_bytes(strRoutekey);

        if (0 != amqp_basic_publish(m_pConn, m_infoServer.nChannel, btExchangeName, btRouteKey, 0, 0, nullptr, btMessage))
        {
            fprintf(stderr, "publish amqp_basic_publish failed\n");
            if (infoError.strErrorMsg = get_reply_info(amqp_get_rpc_reply(m_pConn), kMQActionTypePublishMessage), !infoError.strErrorMsg.empty())
            {
                amqp_channel_close(m_pConn, m_infoServer.nChannel, AMQP_REPLY_SUCCESS);
                infoError.nCode = kMQErrorCodePublishFailed;
                return infoError;
            }
        }

        amqp_channel_close(m_pConn, m_infoServer.nChannel, AMQP_REPLY_SUCCESS);
        amqp_bytes_free(btMessage);
        return infoError;
    }

    MQErrorInfo RabbitmqHelper::Consumer(const std::string& strQueueName,
                                         std::string& strMessage,
                                         int nPrefetchCnt,
                                         struct timeval* timeout/* = nullptr*/)
    {
        MQErrorInfo infoError;
        // open channel
        infoError = OpenChannel();
        if (infoError.nCode != kMQErrorCodeOK) return infoError;

        amqp_basic_qos(m_pConn, m_infoServer.nChannel, 0, nPrefetchCnt, 0);
        int ack = 1;
        int nAck = 1;// delete message with ack
        auto btQueueName = string_2_amqp_bytes(strQueueName);
        amqp_basic_consume(m_pConn, m_infoServer.nChannel, btQueueName, amqp_empty_bytes, 0, nAck, 0, amqp_empty_table);
        if (infoError.strErrorMsg = get_reply_info(amqp_get_rpc_reply(m_pConn), kMQActionTypeConsumer), !infoError.strErrorMsg.empty())
        {
            amqp_channel_close(m_pConn, m_infoServer.nChannel, AMQP_REPLY_SUCCESS);
            infoError.nCode = kMQErrorCodeConsumerFailed;
            return infoError;
        }
        // consume message
        while (!_shutdown)
        {
            amqp_maybe_release_buffers(m_pConn);

            amqp_envelope_t envelope;
            auto reply = amqp_consume_message(m_pConn, &envelope, timeout, 0);
            switch (reply.reply_type)
            {
                case AMQP_RESPONSE_NONE:
                    //InfoL << "EOF of socket ";
                    continue;
                case AMQP_RESPONSE_NORMAL:
                    break;
                case AMQP_RESPONSE_LIBRARY_EXCEPTION:
                {
                    switch (reply.library_error)
                    {
                        case AMQP_STATUS_TIMEOUT:
                            continue;
                        default:
                            //ErrorL << "Library exception, code: " <<  reply.library_error << " error:" << amqp_error_string2(reply.library_error);
                            break;
                    }
                    break;
                }
                case AMQP_RESPONSE_SERVER_EXCEPTION:
                    //ErrorL <<"Server exception";
                    continue;
                default:
                    break;
            }
            //if (AMQP_RESPONSE_NORMAL != reply.reply_type) continue;

            strMessage = std::string((char*)envelope.message.body.bytes, (char*)envelope.message.body.bytes + envelope.message.body.len);
            //InfoL << "[Consume]Message: "<< strMessage;
            // send ack for a message
            //int rtn = amqp_basic_ack(m_pConn, m_nChannel, envelope.delivery_tag, 1);

            amqp_destroy_envelope(&envelope);
        }

        return infoError;
    }

    std::string RabbitmqHelper::get_action_info(MQActionType nType)
    {
        std::string strInfo;
        auto iterType = g_mapMQAction.find(nType);
        if (iterType != g_mapMQAction.end())
        {
            strInfo = iterType->second;
        }
        return strInfo;
    }

    std::string RabbitmqHelper::get_reply_info(amqp_rpc_reply_t reply, MQActionType nType)
    {
        std::string strErrorMsg;
        switch (reply.reply_type)
        {
            case AMQP_RESPONSE_NORMAL:
                break;
            case AMQP_RESPONSE_NONE:
            {
                std::stringstream ss;
                ss << get_action_info(nType) << " missing RPC reply type";
                strErrorMsg = ss.str();
                break;
            }
            case AMQP_RESPONSE_LIBRARY_EXCEPTION:
            {
                std::stringstream ss;
                ss << get_action_info(nType) << " " << amqp_error_string2(reply.library_error);
                strErrorMsg = ss.str();
                break;
            }
            case AMQP_RESPONSE_SERVER_EXCEPTION:
            {
                std::stringstream ss;
                switch (reply.reply.id)
                {
                    case AMQP_CONNECTION_CLOSE_METHOD:
                    {
                        amqp_connection_close_t* m = (amqp_connection_close_t*)reply.reply.decoded;
                        ss << get_action_info(nType) << ": server connection error"
                           << ", code: %ud" << m->reply_code
                           << ", message: " << std::string((char*)m->reply_text.bytes, m->reply_text.len);
                        strErrorMsg = ss.str();
                        break;
                    }
                    case AMQP_CHANNEL_CLOSE_METHOD:
                    {
                        amqp_channel_close_t* m = (amqp_channel_close_t*)reply.reply.decoded;
                        ss << get_action_info(nType) << ": server channel error"
                           << ", code: %ud" << m->reply_code
                           << ", message: " << std::string((char*)m->reply_text.bytes, m->reply_text.len);
                        break;
                    }
                    default:
                    {
                        ss << get_action_info(nType) << ": unknown server error, method_id: " << reply.reply.id;
                        break;
                    }
                }
                break;
            }
            default:
                printf("Invalid reply type: %d\n", reply.reply_type);
                break;
        }

        return strErrorMsg;
    }

    std::string RabbitmqHelper::get_exchange_type_name(MQExchangeType nType)
    {
        std::string strExchangeName;
        switch (nType)
        {
            case kMQExchangeTypeBroadcast:       // broadcast
            {
                strExchangeName = "fanout";
                break;
            }
            case kMQExchangeTypeBindDirect:      // bind direct
            {
                strExchangeName = "direct";
                break;
            }
            case kMQExchangeTypeTopic:           // match topic with fuzzy
            {
                strExchangeName = "topic";
                break;
            }
            default:
                break;
        }

        return strExchangeName;
    }

    amqp_bytes_t RabbitmqHelper::string_2_amqp_bytes(const std::string& strData)
    {
        return amqp_cstring_bytes(strData.c_str());
    }

    std::string RabbitmqHelper::get_error_msg(MQErrorCode nCode)
    {
        std::string strErrorMsg;
        auto iterMatch = g_mapErrorMsg.find(nCode);
        if (iterMatch != g_mapErrorMsg.end())
        {
            strErrorMsg = iterMatch->second;
        }
        return strErrorMsg;
    }
}
