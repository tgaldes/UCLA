#include "HTTPRequest.h"

HTTPRequest::HTTPRequest(string input)
{
	//see if we are creating a request from a request that was transmitted over the wire
	if (input[0] == 'G')
	{
		createFromEncoding(input);
		return;
	}
	//here we parse our input string that was specified as argc to web-client (assume all inputs start with http://
	//look for optional port number
	int portNumIndex = -1;
	int i;
	int j = 0;
	for (i = 7; i < input.length(); i++)
	{
		if (input[i] == ':')
		{
			portNumIndex = 0;
			//this is the start of our port number
			for (j = i + 1; j < input.length(); j++)
			{
				if (!isdigit(input[j]))
					break;
			}
			break;
		}
	}
	//there's a ":" but no actual digits, set port number to 80
	if (i + 1 == j )
	{
		portNumber = 80;
	}
	else if (portNumIndex != -1)
	{
		//we have a port number
		portNumber = atoi(input.substr(i + 1, j - i).c_str());
	}
	else
		portNumber = 80;

	//get the url
	int urlStart = 0;
	int urlEnd = 7;
	for (urlEnd = 7; urlEnd < input.length(); urlEnd++)
	{
		if (input[urlEnd] == ':' || input[urlEnd] == '/')
			break;
	}
	url = input.substr(7, urlEnd - 7);

	//get the filename
	int fileNameStart;
	for (fileNameStart = 7; fileNameStart < input.length(); fileNameStart++)
	{
		if (input[fileNameStart] == '/')
		{
			break;
		}
	}
	fileName = input.substr(fileNameStart);
	if (strcmp(fileName.c_str(), "") == 0)
	{
		fileName = "/index.html";
	}
	if (fileName[fileName.length() - 1] == '/')
	{
		fileName += "index.html";
	}
}
void HTTPRequest::createFromEncoding(string input)
{
	
	//grab the filename
	for (int i = 4; i < input.size(); i++)
	{
		if (input[i] != ' ')
		{
			fileName += input[i];
		}
		else
			break;
	}
	//find the host request line and then get the port number and hostname
	int i = 0;
	while (i < input.length() - 6 && input.substr(i, 6).compare("Host: ") != 0)
	{
		i++;
	}
	i += 6;
	//we now have a start index for the beginning of the hostname
	int j = i;
	while (j < input.length())
	{
		if (input[j] == '\n')
		{
			//no port number specified, so use 80 to assume http
			url = input.substr(i, j - i - 1);
			portNumber = 80;
			break;
		}
		else if (input[j] == ':')
		{
			int startOfPortNumber = j + 1;
			j++;
			url = input.substr(i, j - i - 1);
			//now go the end of the line and get the port number
			while (j < input.length() && j != '\n')
				j++;
			portNumber = atoi(input.substr(startOfPortNumber, j - startOfPortNumber).c_str());
			break;
		}
		j++;
	}

}
string HTTPRequest::getUrl()
{
	return url;
}
int HTTPRequest::getPortNumber()
{
	return portNumber;
}
string HTTPRequest::getFileName()
{
	return fileName;
}
//this generates an encoding of the http request in a string
string HTTPRequest::encode()
{
	string encoding = "GET " + fileName + " HTTP/1.0" + (char)(uint8_t)13 + (char)(uint8_t)10 ;
	encoding += "Host: " + url + ":" + to_string(portNumber) + (char)(uint8_t)13 + (char)(uint8_t)10 + (char)(uint8_t)13 + (char)(uint8_t)10;
	return encoding;
}

int test1()
{
	HTTPRequest* test = new HTTPRequest("http://localhost:4000/index.html");
	//cout << test->encode() << endl;
	HTTPRequest* test1 = new HTTPRequest(test->encode());
	//cout << test1->encode() << endl;
	//cout << test1->getUrl() << endl;
	HTTPRequest* test2 = new HTTPRequest("http://web.cs.ucla.edu:69/classes/spring16/cs118/project-1.html");
	//cout << test2->encode() << endl;
	HTTPRequest* test3 = new HTTPRequest("http://web.cs.ucla.edu:/classes/spring16/cs118/project-1.html");
	//cout << test3->encode() << endl;

	return 0;
}
		



