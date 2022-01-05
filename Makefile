CC=g++
CFLAGS=-I. -std=c++11 
LDFLAGS=-lpthread -lmysqlclient -I/usr/local/include -L/usr/local/lib -lssl -lcrypto

all: server.cpp
	$(CC) -g -o server server.cpp TcpServer.cpp TcpSocket.cpp wzq_msg.cpp DBPool.cpp DUser.cpp utils.cpp $(CFLAGS) $(LDFLAGS)

client: client.cpp
	$(CC) -g -o client client.cpp TcpSocket.cpp wzq_msg.cpp utils.cpp $(CFLAGS) $(LDFLAGS)