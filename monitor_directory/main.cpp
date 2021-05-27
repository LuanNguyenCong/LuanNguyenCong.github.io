#include "monitor/Folder.cpp"
#include "monitor/ThreadBase.cpp"
#include "monitor/Disk.cpp"
#include "monitor/MonitorDirectory.cpp"
#include <vector>

int main()
{
    MonitorDirectory::getInstance()->pushPath("/home");
    MonitorDirectory::getInstance()->pushPath("/home/luannc/Documents");
    MonitorDirectory::getInstance()->pushPath("/testdisk1");
    MonitorDirectory::getInstance()->pushPath("/testdisk2");

    std::vector<unsigned long> t;

    t = MonitorDirectory::getInstance()->getDiskSpace();
    t = MonitorDirectory::getInstance()->getFreeDiskSpace();
    t = MonitorDirectory::getInstance()->getAvailableDiskSpace();
    return 0;
}