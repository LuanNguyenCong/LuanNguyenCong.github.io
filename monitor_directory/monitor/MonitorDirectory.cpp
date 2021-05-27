#include "MonitorDirectory.h"
using namespace std;

MonitorDirectory* MonitorDirectory::instance = nullptr;

MonitorDirectory::MonitorDirectory()
{
}

MonitorDirectory::~MonitorDirectory()
{
}

MonitorDirectory* MonitorDirectory::getInstance()
{
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

void MonitorDirectory::run()
{
    while (true)
    {
        
    }
    
}