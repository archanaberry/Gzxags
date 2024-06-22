#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <sqlite3.h>

class Database {
public:
    Database(const std::string& db_name);
    ~Database();

    bool userExists(const std::string& id);
    bool usernameExists(const std::string& username);
    void addUser(const std::string& username, const std::string& id, const std::string& password);
    bool validateUser(const std::string& id, const std::string& password);

private:
    sqlite3* db_;
};

#endif // DATABASE_H