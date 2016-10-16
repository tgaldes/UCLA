all: server client

server: web-server

web-server: web-server.o HTTPRequest.o HTTPResponse.o
	g++ -std=c++11 -pthread -o web-server web-server.o HTTPRequest.o HTTPResponse.o
	
web-server.o: web-server.cpp HTTPRequest.h  HTTPResponse.h
	g++ -std=c++11 -c -pthread -o web-server.o web-server.cpp 

HTTPRequest.o: HTTPRequest.cpp HTTPRequest.h
	g++ -std=c++11 -c -o HTTPRequest.o HTTPRequest.cpp

HTTPResponse.o: HTTPResponse.cpp HTTPResponse.h
	g++ -std=c++11 -c -o HTTPResponse.o HTTPResponse.cpp
client: web-client

web-client: web-client.o HTTPRequest.o
	g++ -std=c++11 -o web-client web-client.o HTTPRequest.o

web-client.o: web-client.cpp HTTPRequest.h
	g++ -std=c++11 -c -o web-client.o web-client.cpp


clean:
	rm -f *.o
	rm -f *.exe
	rm web-server web-client
