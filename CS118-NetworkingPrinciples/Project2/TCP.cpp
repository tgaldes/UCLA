#include "TCP.h"
#include <cstring>
#include <bitset>
#include <math.h>
using namespace std;

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

//HELPER FUNCTION TO CONVERT DECIMAL TO BINARY STRING
void ConvertToBinary(int n, int i, int len, char *s) {
	if (n / 2 != 0) {
		ConvertToBinary(n / 2, i + 1, len, s);
	}

	if (n % 2)
		s[len - i - 1] = '1';
}

void clearBinaryString(char *s, int len)
{
	memset(s, '0', sizeof(char)*len);
}

int binary2Decimal(string binary)
{
    int val = 0;
    int num = 0;
    for (int x=0; x<binary.length(); x++)
    {
        num = binary[x] - '0';
        if (num)
            val += pow(2, binary.length()-1-x);
    }
    
    return val;
}

string decimal2Binary(int n, int len)
{
	char* ret = (char*)malloc(sizeof(char)*len);

	//Clear
	clearBinaryString(ret, len);
	ConvertToBinary(n, 0, len, ret);
	string a(ret);
	return a;
}


TCP::TCP(string data) {
    if (sizeof(data)>= 8){
    headerContent = data.substr(0, 8);
    fileContent = data.substr(8);
	decode();
    }
    
    else
    {
        headerContent = "";
        encoding = "";
        fileContent = "";
        sNum = aNum = wNum = nUsed = ackFlag = synFlag = finFlag = 0;
    }
}

TCP::TCP(int s, int a, int w, int n, int ack, int syn, int fin, std::string fileContent) {
	sNum = s;
	aNum = a;
	wNum = w;
	nUsed = 0;
	ackFlag = ack;
	synFlag = syn;
	finFlag = fin;
	this->fileContent = fileContent;
    
    //Set Header content and Decoding Correctly
    encode();
}


string TCP::encode() {
	int debug = 0;
	headerContent = "";
	std::bitset<32> firstHalf = std::bitset<32>((sNum << 16) + (aNum << 0));
	std::bitset<32> secondHalf = std::bitset<32>((wNum << 16) + (ackFlag << 2) + (synFlag << 1) + finFlag);
	for (int i = 3; i >= 0; i--)
	{
		bitset<8> currentChar = bitset<8>(0);
		for (int j = 0; j < 8; j++)
		{
			if (firstHalf.test(j  + i*8))
			{
				currentChar.set(j, 1);
			}
		}
		if (debug)
		{
			cout << currentChar.to_string() << endl;
			cout << (char)currentChar.to_ulong() << endl;
		}
		headerContent += (char)currentChar.to_ulong();
	}
	for (int i = 3; i >= 0; i--)
	{
		bitset<8> currentChar = bitset<8>(0);
		for (int j = 0; j < 8; j++)
		{
			if (secondHalf.test(j + i * 8))
			{
				currentChar.set(j, 1);
			}
		}
		if (debug)
		{
			cout << currentChar.to_string() << endl;
			cout << (char)currentChar.to_ulong() << endl;
		}
		headerContent += (char)currentChar.to_ulong();
	}
    
    encoding = headerContent + fileContent;
    
	return encoding;
}

void TCP::print()
{
    cout << endl << " **** PACKET INFO ***** " << endl;
    cout << "Size: " << encoding.length() << endl;
    cout << "Sequence #: " << sNum << endl;
    cout << "ACK #: " << aNum << endl;
    cout << "Window Size: " << wNum << endl;
    cout << "Ack Flag: " << ackFlag << endl;
    cout << "Syn Flag: " << synFlag << endl;
    cout << "Fin Flag: " << finFlag << endl;
    cout << "Content: " << fileContent << endl;
    cout << "Content Binary: ";
    print_bytes(fileContent.c_str(), fileContent.length());
    cout << endl;
    cout << " *************** " << endl << endl;
}
void TCP::decode() {
	int debug = 0;
	//get seq ack and window
	int nums[3];
	for (int i = 0; i < 6; i+=2)
	{
		char leftHalf = headerContent[i];
		char rightHalf = headerContent[i + 1];
		int currentNum = ((long)leftHalf << 8) + (long)rightHalf;
		nums[i / 2] = currentNum;
	}
	sNum = nums[0];
	aNum = nums[1];
	wNum = nums[2];
	//get the flags
	char flags = headerContent[7];
	int flagNum = (long)flags;
	if (flagNum % 2 == 0)
		finFlag = 0;
	else
		finFlag = 1;
	flagNum -= 1;
	flagNum = flagNum >> 1;
	if (flagNum % 2 == 0)
		synFlag = 0;
	else
		synFlag = 1;
	flagNum -= 1;
	flagNum = flagNum >> 1;
	if (flagNum % 2 == 0)
		ackFlag = 0;
	else
		ackFlag = 1;
	flagNum -= 1;
	flagNum = flagNum >> 1;
	if (debug)
		cout << "Finished decoding." << endl;
}

int TCP::getSNum() {
	return sNum;
}

void TCP::setSNum(int n)
{
	sNum = n;
    encode();
}

int TCP::getANum() {
	return aNum;
}

void TCP::setANum(int n)
{
	aNum = n;
    encode();
}

int TCP::getWNum() {
	return wNum;
}

void TCP::setWNum(int n)
{
	wNum = n;
    encode();
}

int TCP::notUsed() {
	return nUsed;
}

void TCP::setNUsed(int n)
{
	nUsed = n;
    encode();
}

int TCP::getAckFlag() {
	return ackFlag;
}

void TCP::setAckFlag(int n)
{
	ackFlag = n;
    encode();
}

int TCP::getSynFlag() {
	return synFlag;
}

void TCP::setSynFlag(int n)
{
	synFlag = n;
    encode();
}

int TCP::getFinFlag() {
	return finFlag;
}

void TCP::setFinFlag(int n)
{
	finFlag = n;
    encode();
}



//1st arg is value, 2nd arg is length
//int main (int argc, char **argv)
//{
//    cout << decimal2Binary(atoi(argv[1]), atoi(argv[2])) << endl;
//}

//int main () {
//	TCP test = TCP(31001, 25, 31003, 0, 0, 0, 0, "HELLO THERE");
//    cout << test.encode() << endl << endl;
//	TCP test2 = TCP(test.encode());
//    test.print();
//    test2.print();
//	return 0;
//}

//int main(int argc, char** argv)
//{
//    if (argc >= 2)
//    {
//        for (int i=1; i<argc;i++)
//            cout << binary2Decimal(argv[i]) << endl;
//    }
//    
//}