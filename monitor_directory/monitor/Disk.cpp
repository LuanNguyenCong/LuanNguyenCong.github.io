#include "Disk.h"
using namespace edsolabs;
using namespace config;

Disk::Disk(unsigned long id)
{
    this-> id = id;
}

Disk::Disk (const Disk& other)
{
    //this->started = other.started;
    this->id = other.id;
    this->folders = other.folders;
}

unsigned long Disk::getID()
{
    return this->id;
}

void Disk::pushOrignalFolder(Folder folder)
{
    folders.push_back(folder);
}

unsigned long Disk::getDiskSpace()
{
    myMutex.lock();
    struct statvfs info;
    int status = statvfs(folders[0].getPath(), &info);
    unsigned long rs = 0;
    if (status == 0)
        rs =  info.f_bsize * info.f_blocks;
    myMutex.unlock();
    return rs;
}

unsigned long Disk::getFreeDiskSpace()
{
    myMutex.lock();
    struct statvfs info;
    int status = statvfs(folders[0].getPath(), &info);
    unsigned long rs;
    if (status == 0)
        rs = info.f_bsize * info.f_bfree;
    myMutex.unlock();
    return rs;
}

unsigned long Disk::getAvailableDiskSpace()
{
    myMutex.lock();
    struct statvfs info;
    int status = statvfs(folders[0].getPath(), &info);
    unsigned long rs;
    if (status == 0)
        rs = info.f_bsize * info.f_bavail;
    myMutex.unlock();
    return rs;
}

int Disk::getPercentOfUsedDisk()
{
    myMutex.lock();
    struct statvfs info;
    int status = statvfs(folders[0].getPath(), &info);
    int rs;
    if (status == 0)
        rs = 100 - (int)(info.f_bfree*100/info.f_blocks);
    myMutex.unlock();
    return rs;
}

void Disk::findOldestFileInFolder(string path, bool isOriginalFolder)
{
    if (!isOriginalFolder)
    {
        // Neu thu muc rong thi xoa thu muc
        if (rmdir(path.c_str()) == 0)
        {
            // Doc file config
            string exchange = ReadConfig::getInstance()->getStringValue(EXCHANGE);
            string routekey = ReadConfig::getInstance()->getStringValue(ROUTEKEY);

            auto objRabbitmq = RabbitmqHelper::initRabbitMq("");
            objRabbitmq.get()->Publish("Da xoa folder " + path, exchange, routekey);
            return;
        }
    }

    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;

    if ((dp = opendir(path.c_str())) != NULL)
    {
        chdir(path.c_str());
        while ((entry = readdir(dp)) != NULL)
        {
            lstat(entry->d_name, &statbuf);       
            if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0)
                continue;  
            if (entry->d_type == DT_DIR)                    // Neu la thu muc
            {
                findOldestFileInFolder(path + "/" + entry->d_name, false);
            }
            else                                            // Neu la file
            {
                if(difftime(statbuf.st_mtime, t_oldest) < 0)
                {
                    t_oldest = statbuf.st_mtime;
                    pathOldestFile = path + "/" + entry->d_name;
                }
            }
        }
    }
}

void Disk::deleteOldestFile()
{
    // Doc file config
    string exchange = ReadConfig::getInstance()->getStringValue(EXCHANGE);
    string routekey = ReadConfig::getInstance()->getStringValue(ROUTEKEY);

    remove(pathOldestFile.c_str());
    auto objRabbitmq = RabbitmqHelper::initRabbitMq("");
    objRabbitmq.get()->Publish("Da xoa file " + pathOldestFile, exchange, routekey);
}

void Disk::run()
{
    // Doc file config
    string exchange = ReadConfig::getInstance()->getStringValue(EXCHANGE);
    string routekey = ReadConfig::getInstance()->getStringValue(ROUTEKEY);
    int deleteStopThreshold = ReadConfig::getInstance()->getIntValue(DELETE_STOP_THRESHOLD);
    
    auto objRabbitmq = RabbitmqHelper::initRabbitMq("");
    objRabbitmq.get()->Publish("O cung ID = " + to_string(id) + " bat dau xoa", exchange, routekey);

    // Thuc hien xoa file den khi duoi nguong deleteStopThreshold
    while (getPercentOfUsedDisk() > deleteStopThreshold)
    {
        time(&t_oldest);
        
        for (int i = 0; i < folders.size(); i++)
        {
            chdir(folders[i].getPath());
            string path = string(folders[i].getPath());
            findOldestFileInFolder(path, true);
        }
        deleteOldestFile();
    }
}

Disk::~Disk()
{
}