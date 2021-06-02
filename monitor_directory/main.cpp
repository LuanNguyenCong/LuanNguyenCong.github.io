#include "config/ReadConfig.cpp"
#include "setup/Setup.cpp"
#include "monitor/ThreadBase.cpp"
#include "monitor/Folder.cpp"
#include "monitor/Disk.cpp"
#include "monitor/MonitorDirectory.cpp"
#include "rbitmq/RabbitmqHelper.cpp"
#include <vector>

using namespace edsolabs;
using namespace config;

int main()
{
    Setup();

    // Cai dat cac path
    MonitorDirectory::getInstance()->pushPath("/testdisk1/2");
    MonitorDirectory::getInstance()->pushPath("/testdisk1/1");
    MonitorDirectory::getInstance()->pushPath("/testdisk1/3");
    MonitorDirectory::getInstance()->pushPath("/testdisk2");

    std::vector<unsigned long> t;

    // Lay cac thong so cua o cung
    t = MonitorDirectory::getInstance()->getDiskSpace();
    t = MonitorDirectory::getInstance()->getFreeDiskSpace();
    t = MonitorDirectory::getInstance()->getAvailableDiskSpace();

    MonitorDirectory::getInstance()->start();
    MonitorDirectory::getInstance()->join();
    
    return 0;
}