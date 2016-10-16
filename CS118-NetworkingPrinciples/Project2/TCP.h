#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

void print_bytes(const void *object, size_t size);

class TCP {

public:
	TCP(std::string data);
	TCP(int s = 0, int a = 0, int w = 0, int n = 0, int ack = 0, int syn = 0, int fin = 0, std::string fileContent = "");
	std::string encode();
	void decode();
	int getSNum();
	void setSNum(int n);
	int getANum();
	void setANum(int n);
	int getWNum();
	void setWNum(int n);
	int notUsed();
	void setNUsed(int n);
	int getAckFlag();
	void setAckFlag(int n);
	int getSynFlag();
	void setSynFlag(int n);
	int getFinFlag();
	void setFinFlag(int n);
	void print();
	//Member variables
	std::string headerContent;
	std::string fileContent;
    std::string encoding;
	int sNum;
	int aNum;
	int wNum;
	int nUsed;
	int ackFlag;
	int synFlag;
	int finFlag;
};