//this holds the sequence number and time sent for packets in 
#include <time.h>
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;
#include "Timer.h"
Timer::Timer(int seqNum, int timeoutseconds, int packetNum, int dataLength)
{
	this->seqNum = seqNum;
	this->packetNum = packetNum;
	//this->sendTime = time(NULL);
	this->t0 = Clock::now();
	this->timeout = timeoutseconds;//in miliseconds
	this->length = dataLength;
}

int Timer::getLength()
{
	return length;
}
int Timer::getSeqNum()
{
	return seqNum;
}
int Timer::getPacketNum()
{
	return packetNum;
}
bool Timer::hasTimedOut(double timeoutVal)
{
	t1 = Clock::now();
	milliseconds diff = std::chrono::duration_cast<milliseconds>(t1 - t0);

	if (diff.count() >= timeoutVal)
		return true;
	return false;
}

int Timer::getTimeDifference()
{
	t1 = Clock::now();
	milliseconds diff = std::chrono::duration_cast<milliseconds>(t1 - t0);
	return diff.count();
}
