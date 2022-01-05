#include "DUser.h"
#include "DBPool.h"
using namespace std;

bool insertUser(CDBConn *pDBConn, const string &username, const string &password) {
    bool ret = false;
    string strSql = "insert into user(`name`, `password`, `score`) values(?,?,?)";
    CPrepareStatement *stmt = new CPrepareStatement();
    if (stmt->Init(pDBConn->GetMysql(), strSql)) {
        uint32_t index = 0;
        stmt->SetParam(index++, username);
        stmt->SetParam(index++, password);
        stmt->SetParam(index++, "1500");
        ret = stmt->ExecuteUpdate();
        if (!ret) {
            printf("insert user failed: %s\n", strSql.c_str());
        }
    }
    delete stmt;
    return ret;
}
DUser getUser(CDBConn *pDBConn, const std::string &username) {
    DUser user;
    user.id = -1;
    string strSql = "select * from user where `name` = \"" + username + "\"";
    //printf("sql :%s\n", strSql.c_str());
    CResultSet *res = pDBConn->ExecuteQuery(strSql.c_str());
    if (res && res->Next()) {
        user.id = res->GetInt("id");
        user.name = res->GetString("name");
        user.password = res->GetString("password");
        user.score = res->GetInt("score");
        return user;
    }
    return user;
}

