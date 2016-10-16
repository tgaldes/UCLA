#include <cstring>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <vector>
#include <math.h>
#include "constants.h"
#include "TCP.h"

using namespace std;
vector<int> sentAckNums = vector<int>(0);

string* convertCString(unsigned char *p, int len)
{
    string *ret = new string("");
    for (int i=0; i<len;i++)
        *ret+=p[i];
    return ret;
}
bool isRetransmission(int currentAckNum)
{
	for (int i = 0; i < sentAckNums.size(); i++)
	{
		if (sentAckNums.at(i) == currentAckNum)
		{
			return true;
		}
	}
	sentAckNums.push_back(currentAckNum);
	return false;
}

int main(int argc, char **argv)
{
    //Stores the host information
    struct hostent *hp;
	int debug = 1;
    
    //TCP
    TCP *tcp1 = new TCP(0, 0, 15360, 0, 0, 1, 0,"");
    //cout << "HEADER SIZE: " << sizeof(TCP) << " bytes.";
    TCP *recv;
    string ACK1 = tcp1->encode();
    string *msgRecv;
	TCP * msgSend;
    int bytesWritten = 0;
    
    //Socket
    int fd;
    
    //Buffer
    unsigned char buf[MAX_PACKET_LENGTH + HEADER_SIZE];
    
    //Server address
    struct sockaddr_in server;
    socklen_t addrlen = sizeof(server);
    
    //Bytes received
    int recvlen;
    
    //File info
    FILE *fp;

    //GET CWD
    char cpath[1024];
    getcwd(cpath, sizeof(cpath));
    string dir = string(cpath);
    string file = dir + "/received.data";
    
    //Store server info into structure
    memset((char*)&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    
    //SET SOCKET
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        cerr << "Error creating socket.";
        return 0;
    }
    
    char *host  = argv[1];
    int i;
    hp  = gethostbyname(host);
    if (!hp)
        cerr <<  "Could not obtain address of " << host << ".";
    
    //Place the first address of the host server into server address
    memcpy((void *)&server.sin_addr, hp->h_addr_list[0], hp->h_length);

    
    //Initiate handshake
	//TODO: actual handshake
	
    if (sendto(fd, ACK1.c_str(), ACK1.length(), 0, (struct sockaddr *)&server, sizeof(server)) < 0) {
        cerr << "Failed.";
        return 0;
    }
	
    
    //LISTEN FOR MESSAGE FROM SERVER
    for (;;) {
       
        recvlen = recvfrom(fd, buf, MAX_PACKET_LENGTH + HEADER_SIZE, 0, (struct sockaddr *)&server, &addrlen);
		
        //Convert buf to C++ string for use with TCP class
        msgRecv = convertCString(buf, recvlen);
		
        recv = new TCP(*msgRecv);
        cout << "Receiving: " << endl;
        recv->print();
        
        //WRITE BUFFER TO FILE
        fp = fopen(file.c_str(), "a");
        bytesWritten += fwrite(recv->fileContent.c_str(), 1,recv->fileContent.length(),fp);
        cout << "Bytes written: " << bytesWritten << endl;
        cout << "Size of fileContent: " << recv->fileContent.length() << endl;
        fclose(fp);
		
		if (recvlen > 0) {
			buf[recvlen] = 0;
        }
        
        //If server sends back confirming Fin-Ack, end connection
        if (recv->getFinFlag() && recv->getAckFlag())
            break;
        
		//create TCP header to send back
        if (recv->getFinFlag()) //If receives FIN, send FIN-ACK
            msgSend = new TCP(0, (recv->getSNum() + recvlen - HEADER_SIZE) % MAX_SEQ_NUM, 15360, 0, 1, 0, 1, "");
        else//OTHERWISE NORMAL MESSAGE

            msgSend = new TCP(0, (recv->getSNum() + recvlen - HEADER_SIZE) % MAX_SEQ_NUM, 15360, 0, 0, 0, 0, "");
		cout << "Receiving packet " << to_string(recv->getSNum()) << endl;
        
		std::string sendString = msgSend->encode();
        cout << "Sending: " << endl;
        msgSend->print();
		
        //SEND ACK
        if (debug)
        {
            cout << "Sending ack number: " << msgSend->getANum() << endl;
        }
        if (sendto(fd, sendString.c_str(), sendString.length(), 0, (struct sockaddr *)&server, sizeof(server)) < 0) {
            cerr << "Failed.";
            return 0;
        }
        cout << "Sending packet " << to_string(msgSend->getANum());
        if (isRetransmission(msgSend->getANum()))
        {
            cout << " Retransmission";
        }
        if (msgSend->getSynFlag() == 1)
        {
            cout << " SYN";
        }
        if (msgSend->getFinFlag() == 1)
        {
            cout << " FIN";
        }
        cout << endl;
	
        
       
        
        //Garbage collection
        delete msgRecv;
		delete msgSend;
        delete recv;
    }
}

    

