#pragma once
#ifndef _THREAD_BASE_H_
#define _THREAD_BASE_H_

#include <thread>
#include <iostream>

class ThreadBase
{
private:
	std::thread myThread;
	bool started = false;
public:
	ThreadBase(/* args */);

	virtual ~ThreadBase();

	void start();
	void join();

	virtual void run();
};



#endif

