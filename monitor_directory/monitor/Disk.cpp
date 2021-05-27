#include "Disk.h"

Disk::Disk(unsigned long id)
{
    this-> id = id;
}

Disk::Disk (const Disk& other)
{
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
    struct statvfs info;
    int status = statvfs(folders[0].getPath(), &info);

    if (status == 0)
        return info.f_bsize * info.f_blocks;
    return 0;
}

unsigned long Disk::getFreeDiskSpace()
{
    struct statvfs info;
    int status = statvfs(folders[0].getPath(), &info);

    if (status == 0)
        return info.f_bsize * info.f_bfree;
    return 0;
}

unsigned long Disk::getAvailableDiskSpace()
{
    struct statvfs info;
    int status = statvfs(folders[0].getPath(), &info);

    if (status == 0)
        return info.f_bsize * info.f_bavail;
    return 0;
}

int Disk::getPercentFree()
{
    struct statvfs info;
    int status = statvfs(folders[0].getPath(), &info);

    if (status == 0)
        return (int)(info.f_bfree * 100 / info.f_blocks);
    return 0;
}

void Disk::run()
{

}

Disk::~Disk()
{
}