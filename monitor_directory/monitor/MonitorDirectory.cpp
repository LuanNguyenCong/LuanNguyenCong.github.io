#include "MonitorDirectory.h"
using namespace std;
using namespace config;

MonitorDirectory* MonitorDirectory::instance = nullptr;

mutex MonitorDirectory::myMutex;

MonitorDirectory::MonitorDirectory()
{
}

MonitorDirectory::~MonitorDirectory()
{
}

MonitorDirectory* MonitorDirectory::getInstance()
{
    lock_guard<mutex> myLock(myMutex);
    if (instance == nullptr)
    {
        instance = new MonitorDirectory();
    }

    return instance;
}

bool MonitorDirectory::pushPath(const char* path)
{
    struct statvfs info;
    int status = statvfs(path, &info);

    if (status == 0)
    {
        unsigned long id = info.f_fsid;
        Folder folder = Folder(path);
        bool isExits = false;

        if (!disks.empty())
        {
            for (int i = 0; i < disks.size(); i++)
            {
                if (id == disks[i].getID())
                {
                    disks[i].pushOrignalFolder(folder);
                    isExits = true;
                    break;
                }
            } 
        }        

        if (!isExits)
        {
            Disk newDisk(id);
            newDisk.pushOrignalFolder(folder);
            disks.push_back(newDisk);
        }
        return true;
    }
    else 
    {
        cout << "Path " << path << " khong dung" << endl;
        return false;
    }
}


vector<unsigned long> MonitorDirectory::getDiskSpace()
{
    vector<unsigned long> rs;
    for (int i = 0; i < disks.size(); i++)
    {
        rs.push_back(disks[i].getDiskSpace());
    }
    return rs;
}

vector<unsigned long> MonitorDirectory::getFreeDiskSpace()
{
    vector<unsigned long> rs;
    for (int i = 0; i < disks.size(); i++)
    {
        rs.push_back(disks[i].getFreeDiskSpace());
    }
    return rs;
}


vector<unsigned long> MonitorDirectory::getAvailableDiskSpace()
{
    vector<unsigned long> rs;
    for (int i = 0; i < disks.size(); i++)
    {
        rs.push_back(disks[i].getAvailableDiskSpace());
    }
    return rs;
}

vector<char[9]> MonitorDirectory::getPermission()
{
    vector<char[9]> a;
    return a;
}

vector<char*> MonitorDirectory::getOwner()
{
    vector<char*> a;
    return a;
}

int MonitorDirectory::percentToThreshold(int percent)
{
    if (percent >= warningThreshold_3) return 3;
    if (percent >= warningThreshold_2) return 2;
    if (percent >= warningThreshold_1) return 1;
    return 0;
}

void MonitorDirectory::run()
{
    // Read config
    timeMonitor = ReadConfig::getInstance()->getIntValue(TIME_MONITOR);
    warningThreshold_1 = ReadConfig::getInstance()->getIntValue(WARNING_THRESHOLD_1);
    warningThreshold_2 = ReadConfig::getInstance()->getIntValue(WARNING_THRESHOLD_2);
    warningThreshold_3 = ReadConfig::getInstance()->getIntValue(WARNING_THRESHOLD_3);
    int deleteStartThreshold = ReadConfig::getInstance()->getIntValue(DELETE_START_THRESHOLD);
    string exchange = ReadConfig::getInstance()->getStringValue(EXCHANGE);
    string routekey = ReadConfig::getInstance()->getStringValue(ROUTEKEY);

    // Vector luu muc canh bao cua cac o dia
    vector<int> warningThreshold;

    auto objRabbitmq = RabbitmqHelper::initRabbitMq("");

    // Gui cac thong so ban dau
    int percent, threshold;
    for (int i = 0; i < disks.size(); i++)
    {
        percent = disks[i].getPercentOfUsedDisk();
        warningThreshold.push_back(percentToThreshold(percent));
        objRabbitmq.get()->Publish("O cung ID = " + to_string(disks[i].getID())
                                + " dang su dung " + to_string(percent) + "%", exchange, routekey);
        if (percent >= deleteStartThreshold) disks[i].start();
    }

    // Vong lap kiem tra thong so (chu ky timeMonitor giay)
    while (true)
    {
        for (int i = 0; i < disks.size(); i++)
        {
            percent = disks[i].getPercentOfUsedDisk();
            threshold = percentToThreshold(percent);
            if (warningThreshold[i] < threshold)        // Tang muc canh bao
            {
                objRabbitmq.get()->Publish("O cung ID = " + to_string(disks[i].getID())
                                        + " dang su dung " + to_string(percent) + "%", exchange, routekey);
                warningThreshold[i] = threshold;
            } else
            if (warningThreshold[i] > threshold)        // Giam muc canh bao
            {
                objRabbitmq.get()->Publish("O cung ID = " + to_string(disks[i].getID())
                                        + " dang su dung " + to_string(percent) + "%", exchange, routekey);
                warningThreshold[i] = threshold;
            }
            if (percent >= deleteStartThreshold)    // Bat dau qua trinh xoa file
                disks[i].start();
        }

        // Dung lai timeMonitor giay
        this_thread::sleep_for(chrono::seconds(timeMonitor));
        for (int i = 0; i < disks.size(); i++)
        {
            disks[i].join();
        }
    }
    
}