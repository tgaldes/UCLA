#include <sys/types.h> 
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <cstdio>
#include <ctime>
#include "HTTPRequest.h"
#include "HTTPResponse.h"

using namespace std;
string hostname;
int portnum;
string dir;
const int TIMEOUT = 500;
//helper function that sets up the hostname, dir, and portnum
//returns 1 on success and exits with an error otherwise
int configure(string &hostname, int &portnum, string &dir, int argc, char *argv[])
{
	int debug = 1;
	if (argc == 1)
	{
		//use default values
		hostname = "localhost";
		portnum = 4000;
		dir = ".";
	}
	else if (argc == 4)
	{
		//set up hostname
		hostname = argv[1];
		//change the hostname of our machine
		if (sethostname(hostname.c_str(), (size_t)hostname.length()) != 0)
		{
			//cout << "error changing the hostname" << endl;
			//cout << "the error is " << strerror(errno) << endl;
		}
		if (debug)
		{
			char* n = new char[hostname.length()];
			gethostname(n, (size_t)hostname.length());
			//cout << "Our new hostname is: " << n << endl;
		}
		//portnum
		portnum = atoi((argv[2]));
		if (portnum == 0 && argv[2][0] != '0')
		{
			//incorrectly entered portnum, display error and exit
			//cout << "Please enter a port number that is a non zero integer" << endl;
			exit(1);
		}
		//directory name
		dir = argv[3];
	}
	else
	{
		//return an error since we only accept three args or none
		//cout << "Incorrect number of arguments. Please enter 0 or 3." << endl;
		exit(1);
	}
	return 1;
}
void printConfigure(string hostname, int portnum, string dir)
{
	//cout << "Hostname: " << hostname << endl;
	//cout << "PortNumber: " << portnum << endl;
	//cout << "Directory: " << dir << endl;
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
//check that the hsotname and port number are right, return true if so
bool correctHostNumber(HTTPRequest* input)
{
	if (portnum == input->getPortNumber() && hostname.compare(input->getUrl()) == 0)
	{
		return true;
	}
	return false;
}
void *httpSession(void * clientFdInput)
{
	int debug = 1;
	//create a buffer that the client's socket can write into
	char buf[1024];
	int i = 0;
	long clientFd = (long)clientFdInput;
	do
	{
		memset(&buf, 0, sizeof(buf));
		int readReturn = read(clientFd, buf, sizeof(buf));
		if (readReturn < 0)
		{
			//cout << "Error reading from the buffer." << endl;
		}

		//create an HTTP request with the information in the buffer
		HTTPRequest *http = new HTTPRequest((string)buf);
		if (!correctHostNumber(http))
		{
			break;
		}
		if (debug) {
			//cout << "The filename that was requested is: " << http->getFileName() << endl;
		}
		//get data from the file in the proper directory
		
		HTTPResponse* resp = new HTTPResponse(http->encode(),dir);
		string content; //will contain all the contents of the exact file
		string httpMessage; //will get the httpMessage (ex. HTTP/1.0 200 OK )
		long fileSize; //will get the numbers of bytes in the file
		if (resp->processBuffer()) {
			//get the contents of the file
			content = resp->getFileContent(resp->getFileName());
			if (debug)
			{
				//cout << "The contents of the file is:\n" << content << endl;
			}

			//add header with status code
			fileSize = resp->getFileSize(resp->getFileName());
			httpMessage = "HTTP/1.0 " + resp->getRespStatus() + (char)(uint8_t)13 + (char)(uint8_t)10 + "Content-Length: " + to_string(fileSize) + (char)(uint8_t)13 + (char)(uint8_t)10 + (char)(uint8_t)13 + (char)(uint8_t)10;
			//connection closing test response httpMessage = "HTTP/1.0 " + resp->getRespStatus() + (char)(uint8_t)13 + (char)(uint8_t)10 + "Connection: close" + (char)(uint8_t)13 + (char)(uint8_t)10 + (char)(uint8_t)13 + (char)(uint8_t)10;
		}
 	
		string finalRespMess = httpMessage + content;
		const char *respBuff = finalRespMess.c_str();
		if (debug)
		{
			//cout << "We are going to write this to the buffer:\n" << respBuff << endl;
		}

		long bufferWriteLength = resp->getFileSize(resp->getFileName()) + resp->statusLength(resp->getRespStatus()) + 9  + 2 + 16 + to_string(resp->getFileSize(resp->getFileName())).length() + 4;
		//cout << "bufferWriteLength is : " << bufferWriteLength << endl;
		//will get the status length with the file size 

		int writeReturn = write(clientFd,respBuff,bufferWriteLength);
		//cout << "write return " << endl;
		i++;
	} while (false);//only respond once for now
	close(clientFd);
	pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
	int debug = 1;
	if (debug)
	{
		//cout << "there are this many arguments: " << argc << "   " << argv[0] << endl;
	}
	
	//set up hostname, portnum, and dir
	configure(hostname, portnum, dir, argc, argv);
	if (debug)
	{
		printConfigure(hostname, portnum, dir);
	}

	//create socket
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	//let subsequent uses of the program use the same port
	int optval = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	//timeout variables
	fd_set readFds;
	fd_set errFds;
	fd_set watchFds;
	FD_ZERO(&readFds);
	FD_ZERO(&watchFds);
	FD_ZERO(&errFds);
	int maxSockfd = sock;
	FD_SET(sock, &watchFds);
	if (sock < 0)
	{
		//cout << "Error creating our socket." << endl;
		exit(1);
	}
	if (debug)
	{
		//cout << "The file descriptor of the socket we made is: " << sock << endl;
	}
	//create sockaddr for our server (which we will cast to sockaddr later
	struct sockaddr_in server;
	memset(&server, 0, sizeof(struct sockaddr_in));
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;// use our ip address
	//set up port number with bytes in correct order for the network
	server.sin_port = htons(portnum);
	if (debug)
	{
		//cout << "about to bind" << endl;
	}
	//bind socket with the fd, ptr to sockaddr, and length of sockaddr
	int bindReturn = ::bind(sock, (struct sockaddr *) &server, sizeof(server));
	if (bindReturn < 0)
	{
		//error binding our socket
		//cout << "Error binding the socket. Better luck next time." << endl;
		exit(1);
	}
	if (debug)
	{
		//cout << "about to listen" << endl;
	}
	//now we need to listen for a client that wants to connect
	//we'll let up to ten clients be in the queue in case our server goes viral
	
	int listenReturn = listen(sock, 10);
	if(listenReturn < 0)
	{
		//cout << "Error on listen()" << endl;
		exit(1);
	}
	//start a timer for ten seconds
	struct timeval timer;
	while(true)
	{
		readFds = watchFds;
		errFds = watchFds;
		int availableFds = 0;
		timer.tv_usec = 0;
		timer.tv_sec = 10;
		if((availableFds = select(maxSockfd+1, &readFds, NULL, &errFds, &timer)) == -1)
		{
			//cout << "error selecting a maximum socket." << endl;
			close(sock);
			exit(1);
		}
		if(availableFds == 0)
		{
			//cout << "We didn't receive a request for five seconds. Exiting." << endl;
			close(sock);
			close(maxSockfd);
			exit(1);
		}
	
	//int to keep track of how many clients we've connected to so far
	int threadNumber = 0;


	const int NUMBER_OF_THREADS = 10;
	pthread_t threads[NUMBER_OF_THREADS];
		

	
	



		//start time variable at zero seconds
		//clock_t startTime;
		//startTime = clock();
		//create another sock_addr for the client
		struct sockaddr_in client;
		socklen_t clientSize = sizeof(client);
		//accept a connection with this client
		long clientFd = accept(sock, (struct sockaddr *) &client, &clientSize);
		//if time is over ten seconds, close the server, client, and exit
		//if (debug)
		//{
		//	//cout << "start: " << startTime << endl;
	  //		//cout << "end(ish):" << clock() << endl;
		//}
		//if ((clock() - startTime) > TIMEOUT)
	//	{
	//		close(clientFd);
		//	close(sock);
	//		exit(1);
		//}
		
		if (clientFd < 0)
		{
			//cout << "Error accepting connection from the client." << endl;
			exit(1);
		}

		int threadStatusCode;
		if (debug)
		{
			//cout << "creating thread number: " << threadNumber << endl;
		}
		threadStatusCode = pthread_create(&threads[threadNumber], NULL,
			httpSession, (void *)clientFd);
		if (threadStatusCode)
		{
			//cout << "Error:unable to create thread," << threadStatusCode << endl;
			exit(-1);
		}
		threadNumber++;
	}
	pthread_exit(NULL);
     return 0; 
}
