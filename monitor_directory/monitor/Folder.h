#ifndef FOLDER_H
#define FOLDER_H

class Folder
{
private:
    const char* path;
public:
    Folder(const char* path);
    ~Folder();

    const char* getPath();
    char* getOwner();
    char* getPermission();
};


#endif