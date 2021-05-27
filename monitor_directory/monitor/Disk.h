#ifndef DISK_H
#define DISK_H

#include <vector>
#include <sys/statvfs.h>
#include "Folder.h"
#include "ThreadBase.h"

class Disk : public ThreadBase
{
private:
    std::vector<Folder> folders;
    unsigned long id;
public:
    Disk(unsigned long id);
    ~Disk();

    Disk (const Disk& other);

    unsigned long getID();

    void pushOrignalFolder(Folder folder);

    // Tra ve tong dung luong cua o cung
    unsigned long getDiskSpace();

    // Tra ve dung luong trong cua o cung
    unsigned long getFreeDiskSpace();

    // Tra ve dung luong kha dung cua o cung
    unsigned long getAvailableDiskSpace();

    //  Tra ve phan tram dung luong con trong
    int getPercentFree();

    // Ham thuc hien viec xoa file
    void run();
};


#endif