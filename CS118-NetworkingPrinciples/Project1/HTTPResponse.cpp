#include <iostream>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include <sstream>
#include <math.h>
#include "HTTPResponse.h"
using namespace std;

  HTTPResponse::HTTPResponse (string buf, string dir) {   
    currBuff = buf;
    currDir = dir;
    respStatus = "404 Not Found";
  }

  bool HTTPResponse::processBuffer() {   
  if (currBuff.length()==0) {
    //cout << "The current buffer is empty!" << endl;
    return false;
  }

  int fileIndex = currBuff.find(" ");
  int respIndex = currBuff.find("HTTP");
  if (fileIndex!=3) {
    string temp = currBuff.substr(fileIndex+1,respIndex-4);
	respStatus = "400 Bad Request";
    //cout << "HTTP/1.0 400 Bad Request" << endl;
    return false;
  }
  respType = currBuff.substr(respIndex);
  //respStatus = "200 OK";
  fileName = currBuff.substr(fileIndex+1,respIndex-5);
  this->fileName = currDir + fileName;
  //cout << "The buffer has been processed!" << endl;
  return true;
  } 

  //need to create getter method getting file name
  string HTTPResponse::getFileContent(string fileName) { 
    /*following code was found on a stack over flow post that explained
      how to get the contents of a file and store it in a string*/
  
   if(fileExists(fileName)) {
      ifstream input (fileName);
      stringstream output;
      output << input.rdbuf(); 
      string fileContents = output.str();
	  //cout << "changed the response status in getFileContent" << endl;
      respStatus = "200 OK";
      return fileContents;
    }    
    else {
      return "";
    } 
  }

  string HTTPResponse::getFileName() { 
	  //cout << "Returning the file name as: " << fileName << endl;
    return fileName;
  }

  string HTTPResponse::getRespStatus() {
    return respStatus;
  }

  string HTTPResponse::getRespType() {
    return respType;
  }

  bool HTTPResponse::fileExists(const string &fileName) {
  int debug = 1;

  if (FILE *check = fopen(fileName.c_str(), "r")) {
    fclose(check);
    if (debug)
      //cout << fileName << " exists!" << endl;
    return true;
  }
   return false;
  }

  //Function was retrieved from a stack over flow post
  long HTTPResponse::getFileSize(string filename){
	  struct stat stat_buf;
	  int rc = stat(filename.c_str(), &stat_buf);
	  return rc == 0 ? stat_buf.st_size : -1;
  /*std::ifstream file;
  file.open(fileName);
  file.seekg(0, ios::end);
  long size = file.tellg();
  file.close();
  return size;*/
  }

  int HTTPResponse::statusLength(string status) {
  return status.length();
  }


  int test() {
    /*HTTPResponse* resp = new HTTPResponse("GET index.html HTTP/1.1", "");
    string content; //will contain all the contents of the exact file
    string httpMessage; //will get the httpMessage (ex. HTTP/1.0 200 OK )
    long fileSize; //will get the numbers of bytes in the file
    if (resp->processBuffer()) {
      content = resp->getFileContent(resp->getFileName());
      fileSize = resp->getFileSize(resp->getFileName());
      httpMessage = "HTTP/1.0 " +resp->getRespStatus()+ (char)(uint8_t)13 + (char)(uint8_t)10 + (char)(uint8_t)13 + (char)(uint8_t)10;
    }
      string finalRespMess = httpMessage + content + "Number of bytes : "+to_string(fileSize);
      const char *respBuff = finalRespMess.c_str();
      //cout << respBuff << endl;*/
    return 0;
  }
