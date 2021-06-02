#pragma once

#include <string>
#include <thread>
#include <rabbitmq-c/tcp_socket.h>

using namespace std;
namespace edsolabs {
// MQ server info
    struct MQServerInfo {
        std::string strIP = "";
        int nPort = 0;
        std::string strUser = "";
        std::string strPasswd = "";
        int nChannel = 1;    // default channel 1, no different
        timeval *pTimeout = nullptr;

        MQServerInfo() {}

        MQServerInfo(std::string strIP, int nPort, std::string strUser, std::string strPasswd, int nChannel,
                     timeval *pTimeout = nullptr) {
            this->strIP = strIP;
            this->nPort = nPort;
            this->strUser = strUser;
            this->strPasswd = strPasswd;
            this->nChannel = nChannel;
            this->pTimeout = pTimeout;
        }
    };

// MQ error code
    enum MQErrorCode {
        kMQErrorCodeOK,                         // no error
        kMQErrorCodeNewConnFailed,              // create connection failed
        kMQErrorCodeInvalidConn,                // invalid connection
        kMQErrorCodeNewSockError,               // create socket failed
        kMQErrorCodeOpenSockError,              // open socket failed
        kMQErrorCodeLoginFailed,                // login failed
        kMQErrorCodeCloseConnFailed,            // close connection failed
        kMQErrorCodeDestoryConnFailed,          // destory connection failed
        kMQErrorCodeDeclareExchangeFailed,      // declare exchange failed
        kMQErrorCodeDeclareQueueFailed,         // declare queue failed
        kMQErrorCodeBindQueueFailed,            // bind queue failed
        kMQErrorCodeUnbindQueueFailed,          // unbind queue failed
        kMQErrorCodeOpenChannelFailed,          // open channel failed
        kMQErrorCodeDelteQueueFailed,           // delete queue failed
        kMQErrorCodePublishFailed,              // publish message failed
        kMQErrorCodeConsumerFailed,             // consumer message failed
    };

// MQ error message info
    struct MQErrorInfo {
        MQErrorCode nCode = kMQErrorCodeOK;     // error code, kMQErrorCodeOK: success, or failed
        std::string strErrorMsg;                // error message
    };

// MQ exchange type
    enum MQExchangeType {
        kMQExchangeTypeBroadcast,       // broadcast
        kMQExchangeTypeBindDirect,      // bind direct
        kMQExchangeTypeTopic,           // match topic with fuzzy
    };

// MQ exchange info
    struct MQExchangeInfo {
        MQExchangeType nType;           // type
        std::string strExchangeName;    // name
        int nPassive = 0;               // 1: return failed while not exist
        int nDurable = 0;               // 1: durable
        int nAutoDel = 0;               // 1: delete the queue while no exchange binded
        bool nInternal = 0;             // 1: use with binded exchange
    };

// MQ queue info
    struct MQQueueInfo {
        std::string strExchangeName;    // exchange name
        std::string strRouteKey;        // route key
        std::string strQueueName;       // name
        int nPassive = 0;               // 1: return failed while not exist
        int nDurable = 1;               // 1: durable
        int nExclusive = 0;             // 1: delete the queue while curr-connection close
        int nAutoDel = 0;               // 1: delete the queue while no consumer
        bool nInternal = 0;             // 1: use with binded exchange
    };

// MQ action type
    enum MQActionType {
        kMQActionTypeBegin,
        kMQActionTypeLogin,                 // login
        kMQActionTypeCloseConnection,       // close connection
        kMQActionTypeExchangeDeclare,       // declare exchange
        kMQActionTypeDeclareQueue,          // declare queue
        kMQActionTypeBindQueue,             // bind queue
        kMQActionTypeUnbindQueue,           // unbind queue
        kMQActionTypeOpenChannel,           // open channle failed
        kMQActionTypeDeleteQueue,           // delete queue
        kMQActionTypePublishMessage,        // publish message
        kMQActionTypeConsumer,              // consumer message
    };

// MQ client
    class RabbitmqHelper {
        const static int kDefaultMaxFrame = 131072;
        const static int kHeartbeat = 60;
    public:
        RabbitmqHelper();

        RabbitmqHelper(const std::string &strIP,
                       int nPort,
                       const std::string &strUser,
                       const std::string &strPasswd,
                       int nChannel = 1,
                       timeval *pTimeout = nullptr);

        ~RabbitmqHelper();

        MQErrorInfo Connect();

        MQErrorInfo Disconnect();

        /**
        * @brief:       open channel
        * @param[in]:
        * @return:      MQErrorCode, 0: success, or failed
        */
        MQErrorInfo OpenChannel();

        /**
        * @brief:       declare exchange
        * @param[in]:   infoExchange, exchange info
        * @return:      MQErrorCode, 0: success, or failed
        */
        MQErrorInfo DeclareExchange(const MQExchangeInfo &infoExchange);

        /**
        * @brief:       declare queue
        * @param[in]:   infoQueue, info of queue
        * @return:      MQErrorCode, 0: success, or failed
        */
        MQErrorInfo DeclareQueue(const MQQueueInfo &infoQueue);

        /**
        * @brief:       bind queue
        * @param[in]:   infoQueue, info of queue
        * @return:      MQErrorCode, 0: success, or failed
        */
        MQErrorInfo QueueBind(const MQQueueInfo &infoQueue);

        /**
        * @brief:       unbind queue
        * @param[in]:   infoQueue, info of queue
        * @return:      MQErrorCode, 0: success, or failed
        */
        MQErrorInfo QueueUnbind(const MQQueueInfo &infoQueue);

        /**
        * @brief:       delete queue
        * @param[in]:   strQueueName, name of queue
        * @param[in]:   nUnused, force to delete the queue while in use
        * @param[in]:   nEmpty, empty of queue
        * @return:      MQErrorCode, 0: success, or failed
        */
        MQErrorInfo QueueDelete(const std::string &strQueueName, int nUnused, int nEmpty);

        /**
        * @brief:       publish message
        * @param[in]:   strMessage, msaage
        * @param[in]:   strExchangeName, exchange name
        * @param[in]:   strRoutekey, route key
        * @return:      MQErrorCode, 0: success, or failed
        */
        MQErrorInfo
        Publish(const std::string &strMessage, const std::string &strExchangeName, const std::string &strRoutekey);

        /**
        * @brief:       consumer message
        * @param[in]:   strQueueName, queue name
        * @param[in]:   strMessage, message
        * @param[in]:   nPrefetchCnt, pre-fetch count
        * @param[in]:   timeout, timer to fetch data, nullptr: no delay
        * @return:      MQErrorCode, 0: success, or failed
        */

        MQErrorInfo
        Publish(const char *data, size_t data_size, const std::string &strExchange, const std::string &strRoutekey);

        MQErrorInfo Consumer(const std::string &strQueueName,
                             std::string &strMessage,
                             int nPrefetchCnt = 1,
                             struct timeval *pTimeout = nullptr);

    private:
        RabbitmqHelper(const RabbitmqHelper &);

        RabbitmqHelper &operator=(const RabbitmqHelper &);

        RabbitmqHelper(const RabbitmqHelper &&);

        RabbitmqHelper &operator=(const RabbitmqHelper &&);

    private:
        std::string get_action_info(MQActionType nType);

        std::string get_reply_info(amqp_rpc_reply_t reply, MQActionType nType);

        std::string get_exchange_type_name(MQExchangeType nType);

        amqp_bytes_t string_2_amqp_bytes(const std::string &strData);

        std::string get_error_msg(MQErrorCode nCode);

    private:
        MQServerInfo m_infoServer;
        amqp_connection_state_t m_pConn;
        amqp_socket_t *m_pSock;
        bool _shutdown = false;

    public:
        static void ConsumeMessage(std::shared_ptr<RabbitmqHelper> client) {
            std::string strQueuename = "nvr-noti";
            std::string strMessage;
            auto infoError = client->Consumer(strQueuename, strMessage, 1);
            //InfoL << "Rabbitmq Consumer Ret: " << infoError.nCode << " ErrorMsg" << infoError.strErrorMsg.c_str();

            client->Disconnect();
        }

        static std::shared_ptr<RabbitmqHelper> initRabbitMq(const string &rabbitServerUrl) {
            std::string strIP = "0.0.0.0";
            int iPort = 5672;
            std::string strUser = "guest";
            std::string strPasswd = "guest";
            std::shared_ptr<RabbitmqHelper> objRabbitmq(new RabbitmqHelper(strIP, iPort, strUser, strPasswd));

            MQErrorInfo infoError = objRabbitmq->Connect();
            //InfoL << "Connect server finished, code:  " << infoError.nCode << " ErrorMsg"
            //      << infoError.strErrorMsg.c_str();

            std::string strExchange = "ExchangeTest";
            std::string strRoutekey = "routekeyTest";

            // declare exchange
            MQExchangeInfo infoExchange;
            infoExchange.strExchangeName = strExchange;
            infoExchange.nType = kMQExchangeTypeBindDirect;
            infoError = objRabbitmq->DeclareExchange(infoExchange);
            //InfoL << "Declare exchange finished, code:  " << infoError.nCode << " ErrorMsg" << infoError.strErrorMsg;

            // declare queue
            MQQueueInfo infoQueue;
            infoQueue.strQueueName = "nvr-noti";
            infoError = objRabbitmq->DeclareQueue(infoQueue);
            //InfoL << "Declare queue finished, code:  " << infoError.nCode << " ErrorMsg" << infoError.strErrorMsg;

            // bind queue
            MQQueueInfo infoBindQueue;
            infoBindQueue.strQueueName = infoQueue.strQueueName;
            infoBindQueue.strExchangeName = strExchange;
            infoBindQueue.strRouteKey = strRoutekey;
            infoError = objRabbitmq->QueueBind(infoBindQueue);
            //InfoL << "Bind queue[name: " << infoBindQueue.strQueueName << " exchange: "
            //      << infoBindQueue.strExchangeName.c_str()
            //      << " route:" << infoBindQueue.strRouteKey.c_str()
            //      << " code: " << infoError.nCode
            //      << " err" << infoError.strErrorMsg;

            // publish message
            std::string strSendMsg1 = "This is a test";

            for (int i = 0; i < 1000; ++i) {
                auto strSendMsg2 = strSendMsg1 + std::to_string(i);
                infoError = objRabbitmq->Publish(strSendMsg2, strExchange, strRoutekey);
                //InfoL << "Publish message finished, code:  " << infoError.nCode << " ErrorMsg" << infoError.strErrorMsg;
            }
            return objRabbitmq;
        }
    };
}



