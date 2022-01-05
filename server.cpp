#include <cstdio>
#include "TcpServer.h"
#include <iostream>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "wzq_msg.h"
#include "DBPool.h"
#include "DUser.h"
using namespace std;

#define DB_HOST_IP "127.0.0.1" // 数据库服务器ip
#define DB_HOST_PORT 3306
#define DB_DATABASE_NAME "wuziqi" // 数据库对应的库名字, 这里需要自己提前用命令创建完毕
#define DB_USERNAME "root"                 // 数据库用户名
#define DB_PASSWORD "000000"               // 数据库密码
#define DB_POOL_NAME "mysql_pool"          // 连接池的名字，便于将多个连接池集中管理
#define DB_POOL_MAX_CON 4                  // 连接池支持的最大连接数量


CDBPool *pDBPool = nullptr;

void* callback(void* arg)
{
	TcpSocket* sockTcp = (TcpSocket*)arg;
	// 通信
	while (1)
	{
		string msg = sockTcp->recvMsg();
		if (msg.empty())
		{
			break; //client fd closed
		}
		cout << "recvMsg: " << msg << endl;
		WZQMessage wmsg;
		wmsg.deserialize(msg);
		switch(wmsg.op) {
			case WZ::ECHO: {
				sockTcp->sendMsg("echo: " + wmsg.s1);
				break;
			}
			case WZ::SIGNUP: {
				string username = wmsg.s1;
				string password = wmsg.s2;
				printf("[%s][%s]\n", username.c_str(), password.c_str());
				CDBConn *pDBConn = pDBPool->GetDBConn(2000);
				if (pDBConn) {
					bool ret = insertUser(pDBConn, username, password);
					pDBPool->RelDBConn(pDBConn);
					if (ret) {
						sockTcp->sendMsg("signup: ok");
					} else {
						sockTcp->sendMsg("signup: failed");
					}
				} else {
					printf("GetDBConn failed\n");
					sockTcp->sendMsg("signup: failed");
				}
				break;
			}
			case WZ::LOGIN: {
				string username = wmsg.s1;
				string password = wmsg.s2;
				printf("[%s][%s]\n", username.c_str(), password.c_str());
				CDBConn *pDBConn = pDBPool->GetDBConn(2000);
				if (pDBConn) {
					DUser user = getUser(pDBConn, username);
					if (user.id == -1) {
						sockTcp->sendMsg("login failed: unknown user name");
					} else if (user.password != password) {
						sockTcp->sendMsg("login failed: password incorrect");
					} else {
						sockTcp->sendMsg("login succeed");
					}
				} else {
					printf("GetDBConn failed\n");
					sockTcp->sendMsg("login failed: retry");
				}
				break;
			}
			default:
				break;
		}
	}
}

void init_dbpool() {
	const char *db_pool_name = DB_POOL_NAME;
    const char *db_host = DB_HOST_IP;
    int db_port = DB_HOST_PORT;
    const char *db_dbname = DB_DATABASE_NAME;
    const char *db_username = DB_USERNAME;
    const char *db_password = DB_PASSWORD;
	int db_maxconnect = DB_POOL_MAX_CON;

    pDBPool = new CDBPool(db_pool_name, db_host, db_port,
                            db_username, db_password, db_dbname, db_maxconnect);

	if (pDBPool->Init())
    {
        printf("init db instance failed: %s", db_pool_name);
        exit(-1);
    }
}

int main()
{
	init_dbpool();
	// 创建对象
	TcpServer *server = new TcpServer;
	// 设置监听
	server->setListen(8888);
	// 等待并接受连接请求
	while (1)
	{
		cout << "开始accept..." << endl;
		TcpSocket* sockTcp = server->acceptConn();
		cout << "接受了一个客户端的连接请求..." << endl;
		if (sockTcp)
		{
			pthread_t tid;
			pthread_create(&tid, NULL, callback, sockTcp);
			pthread_detach(tid);
		}
	}
    return 0;
}