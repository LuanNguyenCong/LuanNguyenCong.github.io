#ifndef DISK_H
#define DISK_H

#include <vector>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <mutex>

#include "Folder.h"
#include "ThreadBase.h"
#include "../rbitmq/RabbitmqHelper.h"
#include "../config/ReadConfig.h"

class Disk : public ThreadBase
{
private:
    std::vector<Folder> folders;    // Danh sach thu muc
    std::mutex myMutex;
    unsigned long id;               // ID o cung

    std::string pathOldestFile;     // Path cua file cu nhat
    std::time_t t_oldest;           // Thoi gian cua file cu nhat

    // Xoa file cu nhat trong cac folder
    void deleteOldestFile();

    // Tim kiem file cu nhat trong thu muc
    // Neu thu muc khong phai thu muc goc va rong thi xoa thu muc
    void findOldestFileInFolder(std::string path, bool isOriginalFolder);
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

    // Tra ve phan tram dung luong da su dung
    int getPercentOfUsedDisk();

    // Luong thuc hien viec xoa file
    void run();
};


#endif