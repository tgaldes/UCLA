#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <cstdbool>
#include <cstring>
using namespace std; 

class HTTPRequest {

public:
	HTTPRequest(string input);
	void createFromEncoding(string input);
	string fileName;
	int portNumber;
	string url;
	string encode();
	string getUrl();
	string getFileName();
	int getPortNumber();
};