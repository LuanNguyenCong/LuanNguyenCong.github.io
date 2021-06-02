#ifndef MONITOR_DIRECTORY_H
#define MONITOR_DIRECTORY_H

#include <vector>
#include <mutex>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <iostream>

#include "Folder.h"
#include "Disk.h"
#include "ThreadBase.h"
#include "../rbitmq/RabbitmqHelper.h"

class MonitorDirectory : public ThreadBase
{
private:
    /* data */
    MonitorDirectory();
    static MonitorDirectory* instance;
    static mutex myMutex;

    int timeMonitor;
    int warningThreshold_1, warningThreshold_2, warningThreshold_3;

    // Danh sach o cung
    std::vector<Disk> disks;
    // Danh sach thu muc goc
    std::vector<Folder> orignalFolders;

    int percentToThreshold(int percent);

public:
    MonitorDirectory(const MonitorDirectory&) = delete;
    MonitorDirectory& operator= (const MonitorDirectory&) = delete;

    static MonitorDirectory* getInstance();
    ~MonitorDirectory();

    // Them duong dan vao de giam sat.
    // Duong dan dung tra ve true, nguoc lai tra ve false.
    bool pushPath(const char* path);

    // Tra ve tong dung luong cua cac o cung
    std::vector<unsigned long> getDiskSpace();

    // Tra ve dung luong trong cua cac o cung
    std::vector<unsigned long> getFreeDiskSpace();

    // Tra ve dung luong kha dung cua cac o cung
    std::vector<unsigned long> getAvailableDiskSpace();

    // Tra ve quyen truy van cua cac duong dan
    // Vi du: r--r--rwx
    std::vector<char[9]> getPermission();

    // Tra ve chu cua cac duong dan
    std::vector<char*> getOwner();

    // Luong thuc hien viec giam sat
    void run();

};



#endif