#include <iostream>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include <sstream>
using namespace std;

class HTTPResponse {
private:
	string currBuff;
	string respType;
	string fileName;
	string respStatus;
	string currDir;

public:
	HTTPResponse(string buf, string dir);
	bool processBuffer();
	string getFileContent(string fileName);
	int statusLength(string status);


	  
	//getters
	string getFileName();
	string getRespStatus(); //this is for the errors (200,400,404)
	string getRespType();  //used to determine what version of HTTP is being used
	long getCLHeader(long num);
	long getFileSize(string filename);
	bool fileExists(const string &fileName);
};