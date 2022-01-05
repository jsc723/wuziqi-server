#ifndef DUSER_H
#define DUSER_H
#include <string>
#include "DBPool.h"

struct DUser {
    int id;
    string name;
    string password;
    int score;
};

bool insertUser(CDBConn *pDBConn, const std::string &username, const std::string &password);
DUser getUser(CDBConn *pDBConn, const std::string &username);
#endif