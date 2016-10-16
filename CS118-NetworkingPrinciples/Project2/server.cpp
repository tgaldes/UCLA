#include <sys/types.h> 
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <string>
#include <cstring>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <cstdio>
#include <ctime>
#include <time.h>
#include "Timer.h"
#include "TCP.h"
#include "constants.h"

using namespace std;
int portnum;
string filename;
string fileContents;
int numChunks;
int sock;


int cwnd = MAX_PACKET_LENGTH;
int unackedBytes = 0;
int nextPacket = 0;
//we start by doubling until a packet is lost
int ssthresh = 2099999999;
vector<Timer> *times;
vector<int> sentPacketNums = vector<int>(0);


//this will be intialized randomly when the connection is established
//TODO: random initialization
int initialSequenceNumber = 0;

//these values were picked so the initial timeout would be computed as 500 miliseconds
double devRTT = 25;
double estimatedRTT = 400;
double timeoutInterval = estimatedRTT + 4 * devRTT;
double alpha = .125;
double beta = .25;



//helper function that sets up the hostname, dir, and portnum
//returns 1 on success and exits with an error otherwise
int configure(int &portnum, string &filename, int argc, char *argv[])
{
	int debug = 1;
	if (argc == 3)
	{
		//portnum
		portnum = atoi((argv[1]));
		if (portnum == 0 && argv[1][0] != '0')
		{
			//incorrectly entered portnum, display error and exit
			cout << "Please enter a port number that is a non zero integer" << endl;
			exit(1);
		}
		//directory name
		filename = argv[2];
	}
	else
	{
		//return an error since we only accept three args or none
		cout << "Incorrect number of arguments. Please enter 0 or 3." << endl;
		exit(1);
	}
	return 1;
}
void printConfigure(int portnum, string filename)
{
	cout << "PortNumber: " << portnum << endl;
	cout << "File name: " << filename << endl;
}


//returns true if file exists, false otherwise
bool fileExists(const char* fileName)
{
	int debug = 1;
	if (FILE *testFile = fopen(fileName, "r"))
	{
		fclose(testFile);
		if (debug)
			//cout << fileName << " exists!" << endl;
			return true;
	}
	else
	{
		if (debug)
			//cout << fileName << " does not exist." << endl;
			return false;
	}

}

//returns the nth chunk of MAX_PACKET_LENGTH bytes from the file
//file chunks start at zero
string getNthChunkFromFile(int n)
{
	return fileContents.substr(n*MAX_PACKET_LENGTH, MAX_PACKET_LENGTH);
}
void testGetNthChunkFromFile()
{
	for (int i = 0; i < numChunks; i++)
	{
		cout << "The " + to_string(i) + "th chunk of the file is:\n";
		cout << getNthChunkFromFile(i);
	}
}

//return true if we're in slow start, false otherwise
bool slowStart()
{
	return cwnd < ssthresh;
}
//return true is we're in congestion avoidance, false otherwise
bool congestionAvoidance()
{
	return !slowStart();
}

//this function is called whenever there is a timeout
//packetNum is the number of the chunk that was in the lost packet
void timeout(int sequenceNum, int packetNum)
{
	int debug = 0;
	
	//clear all timers
	times->clear();

	//update the value of ssthresh and cwnd
	ssthresh = cwnd / 2;
	cwnd = MAX_PACKET_LENGTH;

	//update next packet to send
	nextPacket = packetNum;
	//mark that we have no packets left in our cwnd
	unackedBytes = 0;
	if (debug)
	{
		cout << "In timeout for packetNum: " << packetNum << endl;
		cout << "New ssthresh: " << ssthresh << endl;
		cout << "New cwnd: " << cwnd << endl;
	}
	//double the timeout interval
	timeoutInterval *= 2;
}
bool isRetransmission(int currentPacket)
{
	for (int i = 0; i < sentPacketNums.size(); i++)
	{
		//cout << sentPacketNums.size();
		if (sentPacketNums.at(i) == currentPacket)
		{
			return true;
		}
	}
	//cout << endl;
	sentPacketNums.push_back(currentPacket);
	return false;
}

//this sends the packet containing chunk packetNum to the client
//returns the next packet to send
int sendPacket(int packetNum, sockaddr* client, int fin, int syn, int fin_ack)
{
	int debug = 0;
    string payload = "";
    TCP* headerAndPacket;
    
    if (!fin)
    {
        payload = getNthChunkFromFile(packetNum);
        //increase the number of unacked bytes we have in our cwnd
        unackedBytes += payload.length();
    }
    //FIN requests count as one un-ACKed byte
    else
        unackedBytes += 1;
    
	
	if (debug)
	{
		cout << "The payload is: " << payload;
	}

	//create a tcp header for FIN request
    if (fin)
        headerAndPacket = new TCP((initialSequenceNumber + packetNum*MAX_PACKET_LENGTH+1)%MAX_SEQ_NUM, 0, cwnd, 0, 0, 0, 1, payload); // +1 sequence number for FIN
    else if (fin_ack) //Send this after receives a FIN request from client
        headerAndPacket = new TCP((initialSequenceNumber + packetNum*MAX_PACKET_LENGTH+2)%MAX_SEQ_NUM, 1, cwnd, 0, 0, 0, 1, payload); // +2 to sequence number at set client ACK to 1
    else if (syn) //header for syn request
        headerAndPacket = new TCP((initialSequenceNumber + packetNum*MAX_PACKET_LENGTH+1)%MAX_SEQ_NUM, 0, cwnd, 0, 1, 1, 0, payload);
    else //Regular header
        headerAndPacket = new TCP((initialSequenceNumber + packetNum*MAX_PACKET_LENGTH)%MAX_SEQ_NUM, 0, cwnd, 0, 0, 0, 0, payload);
	//encode the tcp header
	string encoded = headerAndPacket->encode();
    cout << "Sending: " << endl;
    headerAndPacket->print();
	const char *udpData = encoded.c_str();
	
	
	//send it baby!
	socklen_t addrlen = sizeof(*client);
	int n = sendto(sock, encoded.c_str(), encoded.length(), 0, client, addrlen);
	if (debug)
	{
		cout << "Sent packet number: " + to_string(packetNum) << endl;
		cout << "Sequence number: " << headerAndPacket->getSNum() << endl;
		cout << "Sent this many bytes: " << strlen(udpData) << endl;
		//cout << "Currently " << unackedBytes << " unacked bytes in our cwnd of: " << cwnd << " bytes\n";
	}
	//add time we are sending to vector
	times->push_back(Timer(headerAndPacket->getSNum(), DEFAULT_RTO, packetNum, headerAndPacket->fileContent.size()));
	std::cout << "Sending packet " << to_string(headerAndPacket->getSNum()) << " " << to_string(cwnd);
	if (ssthresh == 2099999999)
	{
		std::cout << " Not set";
	}
	else
	{
		std::cout << " " << to_string(ssthresh);
	}
	if (isRetransmission(packetNum))
	{
		std::cout << " Retransmission";
	}
	if (headerAndPacket->getSynFlag() == 1)
	{
		std::cout << " SYN";
	}
	if (headerAndPacket->getFinFlag() == 1)
	{
		std::cout << " FIN";
	}
	std::cout << endl;
	delete headerAndPacket;
	return packetNum + 1;
}
int setTimeout(int socket, int miliSeconds)
{
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = miliSeconds * 1000;
	return setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

//this function handles reading in packets, marking packets as received,
//and changing the number of unacked bytes
//return the lastAckedChunk
int receivePacket(sockaddr* client, int lastAckedChunk)
{
	int debug = 0;
	//read from socket and get the ack numbers (if any)
	char buf[1024];
	memset(&buf, 0, sizeof(buf));
	socklen_t addrlen = sizeof(client);
	int n = recvfrom(sock, &buf, 1024, 0, client, &addrlen);
	int ackNum = -1;
    TCP *response;
	if (n > 0)
	{
		string resp(buf, n);
        
        //print_bytes(buf, n);
		//create TCP object and decode it for the ack number
		response = new TCP(resp);
		ackNum = response->getANum();
        cout << "Receiving: " << endl;
        response->print();
        
//        //If receives FIN-ACK
//        if (response->getFinFlag() && response->getAckFlag())
//        {
//            //Send FIN-ACK TO CLIENT
//            sendPacket(nextPacket-1, client, 1, 0, 1);
//            return -1;
//        }
	}
	if (n > 0 && debug)
	{
		cout << "the ack number is: " << ackNum << endl;
		cout << "this many bytes" << n << endl;
		
		cout << "The size of the timer vector is: " << times->size() << endl;
	}
	

	//update laskAckedChunk if the acked packet is the next packet
	if (n > 0 && ackNum == ((MAX_PACKET_LENGTH * (lastAckedChunk) + getNthChunkFromFile(lastAckedChunk).size() + initialSequenceNumber)) % MAX_SEQ_NUM)
	{
		lastAckedChunk++;
		//remove the corresponding timers from the vector if this is the next packet in order
		for (int i = 0; i < times->size(); i++)
		{
			if ((times->at(i).getSeqNum() + times->at(i).getLength()) % MAX_SEQ_NUM == ackNum)
			{
				if (debug)
				{
					cout << "Found matching ack of: " << ackNum << ". Removing them from vector.\n";
				}
				//update the estimatedRTT, devRTT, andtimeout interval
				int newRTT = times->at(i).getTimeDifference();
				double cacheEst = estimatedRTT;
				estimatedRTT = (1 - alpha) * estimatedRTT + alpha * newRTT;
				devRTT = (1 - beta) * devRTT + beta * abs(newRTT - cacheEst);
				timeoutInterval = estimatedRTT + 4 * devRTT;
				times->erase(times->begin() + i);
			}
		}
		//update the value of cwnd depending on what state we're in
		if (slowStart())
		{
			cwnd += MAX_PACKET_LENGTH;
			if (debug)
			{
				cout << "In slow start phase.\n";
				cout << "New value of cwnd is:" + to_string(cwnd) << endl;
			}
		}
		else
		{
			cwnd += MAX_PACKET_LENGTH;
			if (debug)
			{
				cout << "In congestion control phase.\n";
				cout << "New value of cwnd is:" + to_string(cwnd) << endl;
			}
		}
		//update number of bytes we have in the network still
        if (response->getFinFlag())
            unackedBytes -=1;
        else
            unackedBytes -= MAX_PACKET_LENGTH;
            
		std::cout << "Receiving packet " << to_string(ackNum) << endl;
	}
	
	return lastAckedChunk;
}

int reliableSession(sockaddr* client)
{
	//set up a vector for the times we sent the packets
	times = new vector<Timer>();
	
	int i = 0;
	//this shows us which chunks the client has successfully received in order and acknowledged
	int lastAckedChunk = 0;

	//we have a really low time out for our sock here since if there aren't any segments waiting
	//to be received we just continue and try to send another packet
	int milisecondsToWaitFor = 1;
	if (setTimeout(sock, milisecondsToWaitFor) < 0)
	{
		cout << "Error setting the timeout for the receiving socket.\n";
		exit(1);
	}
	
	while (true)
	{
		
		lastAckedChunk = receivePacket(client, lastAckedChunk);
        
        //Return -1 if connection terminated
        if (lastAckedChunk == -1)
            break;

		//check that no unacked packets have timed out by going through vector
		for (int j = 0; j < times->size(); j++)
		{
			if (times->at(j).hasTimedOut(timeoutInterval))
				//timeout takes care of the nextPacket, unackedbytes
				timeout(times->at(j).getSeqNum(), times->at(j).getPacketNum());
		}
        cout << "unackedBytes " << unackedBytes << ", nextPacket " << nextPacket << endl;
		//check that we have room in our cwnd for another packet
		if (unackedBytes < cwnd && nextPacket <= numChunks)
		{
            cout << "nextPacket " << nextPacket << ", LastAckedChunk " << lastAckedChunk << ", numChunks: " << numChunks << endl;
            //If we sent the entire file, we should send a FIN request to terminate connection
            if (lastAckedChunk == numChunks)
            {
                nextPacket = sendPacket(nextPacket-1, client, 1, 0, 0); //Do not increment nextpacket because we have already sent all data
            }
            else
                nextPacket = sendPacket(nextPacket, client, 0, 0, 0);
		}
		
		i++;
	}
	//call closeConnection() TODO: write that function
	return 0;
}

int main(int argc, char *argv[])
{

	int debug = 1;
	if (debug)
	{
		cout << "there are this many arguments: " << argc << "   " << argv[0] << endl;
	}

	//set up hostname, portnum, and dir
	configure(portnum, filename, argc, argv);
	if (debug)
	{
		printConfigure(portnum, filename);
	}
	//check if file exists
	//if so, read its contents into fileContent
	if (fileExists(filename.c_str()))
	{
		ifstream input(filename);
		stringstream output;
		output << input.rdbuf();
		fileContents = output.str();
		//subtract one since our "chunks" start at index zero
		numChunks = ceil((double)fileContents.length() / MAX_PACKET_LENGTH);
		if (debug)
		{
			cout << "There are: " + to_string(numChunks) + " chunks in the file" << endl;
            cout << "String: " << fileContents << endl;
            cout << "File string size: " << sizeof(fileContents) << endl;
		}
	}
	else
	{
		if (debug)
		{
			cout << "The file doesn't exist." << endl;
		}
		fileContents = "The file named: \"" + filename + "\" does not exist on the server.\n";
		numChunks = 1;
	}
	


	//create socket
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	//let subsequent uses of the program use the same port
	int optval = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	
	if (sock < 0)
	{
		cout << "Error creating our socket." << endl;
		exit(1);
	}
	if (debug)
	{
		cout << "The file descriptor of the socket we made is: " << sock << endl;
	}
	//create sockaddr for our server (which we will cast to sockaddr later
	struct sockaddr_in server;
	memset(&server, 0, sizeof(struct sockaddr_in));

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);// use our ip address
	
	//set up port number with bytes in correct order for the network
	server.sin_port = htons(portnum);
	if (debug)
	{
		cout << "about to bind" << endl;
	}
	//bind socket with the fd, ptr to sockaddr, and length of sockaddr
	int bindReturn = ::bind(sock, (struct sockaddr *) &server, sizeof(server));

	if (bindReturn < 0)
	{
		//error binding our socket
		cout << "Error binding the socket. Better luck next time." << endl;
		exit(1);
	}
	
	while (true)
	{
		//create another sock_addr for the client
		struct sockaddr_in client;
		socklen_t clientSize = sizeof(client);
		//accept a connection with this client
		//create a buffer that the client's socket can write into
		char buf[1024];
		memset(&buf, 0, sizeof(buf));
		socklen_t addrlen = sizeof(client);
		int n = recvfrom(sock, &buf, 255, 0, (sockaddr *)&client, &addrlen);
		//TODO function for three way handshake
		//TODO: intitial sequence number should be initialized in the three way handshake
		if (debug)
		{
			cout << "The message from the client is: " << buf << endl;
		}
		
		reliableSession((sockaddr*)&client);
		//TODO: close connection with client
		//TODO: remove this break so that server can make connections with multiple clients in sequence
		break;
	}
	

	return 0;
}
