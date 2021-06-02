#include "ThreadBase.h"
using namespace std;

ThreadBase::ThreadBase()
{
}

ThreadBase::~ThreadBase()
{
	if (started) join();
}

void ThreadBase::start()
{
	if (started) return;
	myThread = thread(&ThreadBase::run, this);
	started = true;
}

void ThreadBase::run()
{

}

void ThreadBase::join()
{
	if (started) myThread.join();
	started = false;
}