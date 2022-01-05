#include <cstdio>
#include "TcpServer.h"
#include <iostream>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <unordered_map>
#include "wzq_msg.h"
#include "DBPool.h"
#include "DUser.h"
#include "utils.h"
using namespace std;

#define DB_HOST_IP "127.0.0.1" // 数据库服务器ip
#define DB_HOST_PORT 3306
#define DB_DATABASE_NAME "wuziqi" // 数据库对应的库名字, 这里需要自己提前用命令创建完毕
#define DB_USERNAME "root"                 // 数据库用户名
#define DB_PASSWORD "000000"               // 数据库密码
#define DB_POOL_NAME "mysql_pool"          // 连接池的名字，便于将多个连接池集中管理
#define DB_POOL_MAX_CON 4                  // 连接池支持的最大连接数量


struct {
	CDBPool *pDBPool = nullptr;
	unordered_map<string, string> tokenToUser;
	unordered_map<string, string> userToToken;
} ctx;

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

		WZQMessage resp;
		resp.op = wmsg.op;

		switch(wmsg.op) {
			case WZ::ECHO: {
				resp.i1 = 0;
				resp.s1 = wmsg.s1;
				sockTcp->sendMsg(resp.serialize());
				break;
			}
			case WZ::SIGNUP: {
				string username = wmsg.s1;
				string password = wmsg.s2;
				printf("[%s][%s]\n", username.c_str(), password.c_str());
				CDBConn *pDBConn = ctx.pDBPool->GetDBConn(2000);
				if (pDBConn) {
					bool ret = insertUser(pDBConn, username, password);
					ctx.pDBPool->RelDBConn(pDBConn);
					if (ret) {
						resp.i1 = 0;
						resp.s1 = "ok";
						sockTcp->sendMsg(resp.serialize());
					} else {
						resp.i1 = 1;
						resp.s1 = "failed";
						sockTcp->sendMsg(resp.serialize());
					}
				} else {
					printf("GetDBConn failed\n");
					resp.i1 = 1;
					resp.s1 = "failed";
					sockTcp->sendMsg(resp.serialize());
				}
				break;
			}
			case WZ::LOGIN: {
				string username = wmsg.s1;
				string password = wmsg.s2;
				printf("[%s][%s]\n", username.c_str(), password.c_str());
				CDBConn *pDBConn = ctx.pDBPool->GetDBConn(2000);
				if (pDBConn) {
					DUser user = getUser(pDBConn, username);
					ctx.pDBPool->RelDBConn(pDBConn);
					if (user.id == -1) {
						resp.i1 = 1; //failed
						resp.s1 = "unknown user name";
						sockTcp->sendMsg(resp.serialize());
					} else if (user.password != password) {
						resp.i1 = 1; //failed
						resp.s1 = "password incorrect";
						sockTcp->sendMsg(resp.serialize());
					} else {
						string token = crypto_hash(username + random_string(8));
						cout << "token: ";
						print_sha256(token);
						if (token.empty()) {
							resp.i1 = 1; //succeed;
							resp.s1 = "server unable to generate token";
							sockTcp->sendMsg(resp.serialize());
						} else {
							ctx.tokenToUser[token] = username;
							ctx.userToToken[username] = token;
							resp.i1 = 0; //succeed;
							resp.s1 = token;
							sockTcp->sendMsg(resp.serialize());
						}
					}
				} else {
					printf("GetDBConn failed\n");
					resp.i1 = 1; //failed
					resp.s1 = "retry";
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

    ctx.pDBPool = new CDBPool(db_pool_name, db_host, db_port,
                            db_username, db_password, db_dbname, db_maxconnect);

	if (ctx.pDBPool->Init())
    {
        printf("init db instance failed: %s", db_pool_name);
        exit(-1);
    }
}

int main()
{
	srand(time(NULL));
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