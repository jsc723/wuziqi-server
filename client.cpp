#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include "TcpSocket.h"
#include "wzq_msg.h"
#include "utils.h"


#define BUFFER_LENGTH		4096
#define SERVER_PORT			8888

int main(int argc, char *argv[]) {

	// 创建套接字
	TcpSocket socket;
	// 连接服务器
	cout << "开始连接..." << endl;
	socket.connectToHost("127.0.0.1", SERVER_PORT);
	cout << "连接成功..." << endl;

    string buf, buf2;

    cout << "输入用户名和密码:" << endl;
    cin >> buf >> buf2;
    printf("[%s][%s]\n", buf.c_str(), buf2.c_str());
    WZQMessage wmsg, resp;
    #if 0
    wmsg.op = WZ::SIGNUP;
    #else
    wmsg.op = WZ::LOGIN;
    #endif
    wmsg.s1 = buf;
    wmsg.s2 = buf2;
    socket.sendMsg(wmsg.serialize());
    resp.deserialize(socket.recvMsg());
    if(resp.i1 == 0) {
        cout << "OK: ";
        print_sha256(resp.s1);
    } else {
        cout << "ERR: " << resp.s1 << endl;
    }
    #if 0
	// 通信
	while (1)
	{
        cin >> buf;
        wmsg.op = WZ::ECHO;
        wmsg.s1 = buf;
		socket.sendMsg(wmsg.serialize());
        resp.deserialize(socket.recvMsg());
	    cout << (resp.i1 == 0 ? "OK: ": "ERR: ") << resp.s1 << endl;

	}
    #endif
    return 0;
}