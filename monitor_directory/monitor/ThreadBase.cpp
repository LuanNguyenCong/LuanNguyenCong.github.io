#include "ThreadBase.h"
using namespace std;

ThreadBase::ThreadBase()
{
}

ThreadBase::~ThreadBase()
{
	
}

void ThreadBase::start()
{
	myThread = thread(&ThreadBase::run, this);
	started = true;
}

void ThreadBase::run()
{

}

void ThreadBase::join()
{
	myThread.join();
}