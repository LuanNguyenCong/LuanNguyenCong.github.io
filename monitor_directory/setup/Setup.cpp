#include "Setup.h"
using namespace config;
using namespace edsolabs;

Setup::Setup()
{
// Setup ReadConfig-------------------------------------------------------------------------
    ReadConfig::getInstance()->setDataPath("monitor_directory/config/config.txt");
    ReadConfig::getInstance()->read();

// Setup RabbitmqHelper---------------------------------------------------------------------
    std::string strIP = "0.0.0.0";
    int iPort = 5672;
    std::string strUser = "guest";
    std::string strPasswd = "guest";
    std::shared_ptr<RabbitmqHelper> objRabbitmq(new RabbitmqHelper(strIP, iPort, strUser, strPasswd));
    MQErrorInfo infoError = objRabbitmq->Connect();

    std::string strExchange = ReadConfig::getInstance()->getStringValue(EXCHANGE);
    std::string strRoutekey = ReadConfig::getInstance()->getStringValue(ROUTEKEY);

    // declare exchange
    MQExchangeInfo infoExchange;
    infoExchange.strExchangeName = strExchange;
    infoExchange.nType = kMQExchangeTypeBindDirect;
    infoError = objRabbitmq->DeclareExchange(infoExchange);

    // declare queue
    MQQueueInfo infoQueue;
    infoQueue.strQueueName = ReadConfig::getInstance()->getStringValue(QUEUE);
    infoError = objRabbitmq->DeclareQueue(infoQueue);

    // bind queue
    MQQueueInfo infoBindQueue;
    infoBindQueue.strQueueName = infoQueue.strQueueName;
    infoBindQueue.strExchangeName = strExchange;
    infoBindQueue.strRouteKey = strRoutekey;
    infoError = objRabbitmq->QueueBind(infoBindQueue);
}

Setup::~Setup()
{
}