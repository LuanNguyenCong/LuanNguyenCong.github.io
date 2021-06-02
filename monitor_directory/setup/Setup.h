#ifndef SETUP_H
#define SETUP_H

#include <string>
#include "../config/ReadConfig.h"
#include "../rbitmq/RabbitmqHelper.h"

class Setup
{
private:
    /* data */
public:
    Setup();
    Setup (const Setup&) = delete;
    Setup& operator=(const Setup&) = delete;

    ~Setup();
};

#endif