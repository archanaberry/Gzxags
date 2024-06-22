#include "database.h"
#include <iostream>
#include <sqlite3.h>

Database::Database(const std::string& db_name) {
    sqlite3_open(db_name.c_str(), &db_);
    std::string sql = "CREATE TABLE IF NOT EXISTS users (username TEXT, id TEXT UNIQUE, password TEXT);";
    sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, nullptr);
}

Database::~Database() {
    sqlite3_close(db_);
}

bool Database::userExists(const std::string& id) {
    std::string sql = "SELECT COUNT(*) FROM users WHERE id = ?";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return count > 0;
}

bool Database::usernameExists(const std::string& username) {
    std::string sql = "SELECT COUNT(*) FROM users WHERE LOWER(username) = LOWER(?)";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return count > 0;
}

void Database::addUser(const std::string& username, const std::string& id, const std::string& password) {
    std::string sql = "INSERT INTO users (username, id, password) VALUES (?, ?, ?)";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, password.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

bool Database::validateUser(const std::string& id, const std::string& password) {
    std::string sql = "SELECT COUNT(*) FROM users WHERE id = ? AND password = ?";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return count > 0;
}