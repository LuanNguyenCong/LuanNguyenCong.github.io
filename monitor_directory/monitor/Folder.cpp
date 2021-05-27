#include "Folder.h"

Folder::Folder(const char* path)
{
    this->path = path;
}

Folder::~Folder()
{
}

const char* Folder::getPath()
{
    return path;
}

char* Folder::getOwner()
{
    char* rs;
    return rs;
}

char* Folder::getPermission()
{
    char* rs;
    return rs;
}