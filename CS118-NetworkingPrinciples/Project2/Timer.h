#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
//#include <omp.h>
#include <time.h>
class Timer {

public:
Timer(int seqNum, int timeoutMiliseconds, int packetNum, int dataLength);
int getSeqNum();
int getPacketNum();
//Clock getTime();
int getTime();
bool hasTimedOut(double timeoutVal);
int getLength();
int getTimeDifference();
//returns true if packet has timed out
//Member variables
int seqNum;
int length;
int packetNum;
//time_t sendTime;
int timeout;
typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::milliseconds milliseconds;
    Clock::time_point t0;
    Clock::time_point t1;
};
