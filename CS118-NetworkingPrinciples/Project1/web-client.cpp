#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <iostream>
#include <sstream>
#include <math.h>

#include "HTTPRequest.h"

#define BUF_LENGTH 3000

using namespace std;

int printMSG = 0;


void print_bytes(const void *object, size_t size)
{
    size_t i;
    
    printf("[ ");
    for(i = 0; i < size; i++)
    {
        printf("%02x ", ((const unsigned char *) object)[i] & 0xff);
    }
    printf("]\n");
}

int hostname2ip(char *hostname, char *ip)
{
    int sockfd;
    struct addrinfo hints, *serverinfo, *it;
    struct sockaddr_in *h;
    int ret;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if ((ret = getaddrinfo(hostname , "http" , &hints , &serverinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        return 1;
    }
    
    for(it = serverinfo; it != NULL; it = it->ai_next)
    {
        h = (struct sockaddr_in *) it->ai_addr;
        if (h->sin_addr.s_addr != 0)
        {
            strcpy(ip , inet_ntoa(h->sin_addr));
            break;
        }
    }
    
    freeaddrinfo(serverinfo);
    return 0;
}

int main(int argc, char* argv[])
{
    for (int x=1; x<argc; x++)
    {
    // create a socket using TCP IP
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    HTTPRequest req((string)argv[x]);
    
    //OBTAIN IP ADDRESS FROM HOST NAME
    char ip[100];
    char* url = (char*)malloc(sizeof(char)*req.getUrl().length());
    strcpy(url, req.getUrl().c_str());
    hostname2ip(url, ip);
    
    if (printMSG)
        cout << "URL: " << url << endl << "IP: " << ip << endl;

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(req.getPortNumber());
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));
    
    // connect to the server
    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("connect");
        return 2;
    }
    
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    if (getsockname(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen) == -1) {
        perror("getsockname");
        return 3;
    }
    
    char ipstr[INET_ADDRSTRLEN] = {'\0'};
    inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
    std::cout << "Set up a connection from: " << ipstr << ":" << ntohs(clientAddr.sin_port) << std::endl;
    
    //PUT HTTP REQUEST IN INPUT MESSAGE
    string sendReq = req.encode();
    char *buf = (char*)malloc(BUF_LENGTH*sizeof(char));
    memset(buf, '\0', BUF_LENGTH);
    
    cout << "Sending Request: " << sendReq;

    //SEND TO SOCKET
    if (send(sockfd, sendReq.c_str(), sendReq.size(), 0) == -1) {
        perror("send");
        return 4;
    }
    
    //READ FROM SOCKT INTO BUFFER
    int sockErr = 0;
    int getStatus = 0;
    int statusCode = 0;
    char *p, *c;
    int num_place = 100;
    int bodyFound = 0;
    
    //Download correct content size
    int content_length = 0;
    int amount_downloaded = 0;
    char *length_str = (char*)(malloc(sizeof(char)*20));
    int numdig = 0;
    int bufSize = 0;
    
    //GET CWD
    char cpath[1024];
    getcwd(cpath, sizeof(cpath));
    string dir = string(cpath);
    
    //cout << endl << dir << endl;
    
    //WRITE BUFFER TO FILE
    FILE *fp;
    string file = dir + req.getFileName();
    string extension = file.substr(file.find("."));
    string fileName = file.substr(dir.length(), file.length() - extension.length() - dir.length());
    if (printMSG)
        cout << "Original file name: " << fileName;

    
    int iter = fileName.length()-1;
    string actualFileName = "";
    while (fileName[iter] != '/')
    {
        actualFileName = fileName[iter] + actualFileName;
        iter--;
    }
    actualFileName = "/" + actualFileName;
    fileName = actualFileName;
    file = dir + fileName + extension; //REASSEMBLE FILE NAME
    if (printMSG)
        cout << "DIR: " << dir << endl << "FILE NAME: " << fileName << endl << "EXTENSION: " << extension << endl << file << endl;
    
    int dupCount = 1;
    int shift = 1;
    
    //IF FILE EXISTS, MAKE DUPLICATE NAME
    while (access(file.c_str(), F_OK) != -1)
    {
        if (dupCount==1)
            fileName += "-1";
        else
        {
            shift = int (log(dupCount - 1)/log(10)) + 1;
            fileName = fileName.substr(0, fileName.length() - shift) + to_string(dupCount);
        }
        
        dupCount++;
        
        file = dir + fileName + extension;
        
        if (printMSG)
            cout << file << " " << fileName.substr(0, fileName.length() - shift) << " " << shift << endl;
    }
   
    if (printMSG)
        cout << "file: " << file << endl;
    fp = fopen(file.c_str(), "w+");
   
    
    while ((sockErr = recv(sockfd, buf, BUF_LENGTH, 0)) > 0 )
    {
        //cout << buf;
        if (printMSG)
        {
            cout << sockErr << " bytes: " << endl;
            print_bytes(buf, BUF_LENGTH);
        }
        if (!getStatus)
        {
            getStatus = 1;
            
            //GET STATUS CODE
            p = buf;
            while (*p != ' ')
                p++;
            p++;
            while (*p != ' ')
            {
                statusCode += (*p - '0')*num_place;
                num_place/=10;
                p++;
            }
            
            cout << "Status: " << statusCode << endl;
            if (statusCode == 404 || statusCode == 400 || statusCode == 301)
            {
                if (statusCode == 404)
                    cout << "File not found." << endl;
                if (statusCode == 400)
                    cout << "Bad request." << endl;
                if (statusCode == 301)
                    cout << "Permanently moved." << endl;
                remove(file.c_str());
                break;
            }
            
            if (statusCode == 200)
                cout << "Successful request!" << endl << endl;
        }
        
        if (!bodyFound)
        {
            if (printMSG)
                cout << "PARSE CONTENT-LENGTH" << endl;
            //parse content-length
            if ((c = strstr(buf, "Content-Length")) != NULL)
            {
                c+=16;
                while (c[numdig] != '\n' && c[numdig] != '\r') {
                    length_str[numdig] = c[numdig];
                    numdig++;
                }
                
                //Reallocate to size of string
                length_str = (char*)realloc(length_str, numdig*sizeof(char));
                num_place = pow(10, numdig-1);
                
                int x = 0;
                while (x < numdig)
                {
                    content_length += (length_str[x] - '0')*num_place;
                    num_place/=10;
                    x++;
                }
                
                if (printMSG)
                    cout << "Content-Length: " << content_length << endl;
                
            }
            
            if (printMSG)
                cout << "Find beginning of body" << endl;
            
            //Set buf to point to body
            if ((p = strstr(buf, "\r\n\r\n")) != NULL)
            {
                p+=4;
                bodyFound = 1;
                amount_downloaded += fwrite(p, sizeof(char), sockErr - (int)(p - buf), fp);
                memset(buf, '\0', BUF_LENGTH);
                continue;
            }
        }
        
        bufSize = sockErr;
     
        //Content length not specified
        amount_downloaded += fwrite(buf, sizeof(char), bufSize, fp);
        memset(buf, '\0', BUF_LENGTH);
       
        

    }
    if (printMSG)
        cout << "FP: " << fp << endl;

    cout << amount_downloaded <<  " bytes downloaded." << endl;
    
    if (sockErr == -1)
    {
        perror("recv");
        return 5;
    }
    
    if (statusCode != 404 && statusCode != 400 && statusCode != 301)
        cout << "Saved to: " << file << endl;

    //CLOSE FILE
    fclose(fp);

    //CLOSE SOCKET
    close(sockfd);
    
    }
    return 0;
}
