CC=g++
CFLAGS=-I. -std=c++11 
LDFLAGS=-lpthread -lmysqlclient

all: server.cpp
	$(CC) -g -o server server.cpp TcpServer.cpp TcpSocket.cpp wzq_msg.cpp DBPool.cpp DUser.cpp $(CFLAGS) $(LDFLAGS)

client: client.cpp
	$(CC) -g -o client client.cpp TcpSocket.cpp wzq_msg.cpp $(CFLAGS)