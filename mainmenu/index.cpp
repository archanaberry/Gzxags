#include "../libarchanaberry/archanaberry_express.hpp"
#include "database.h"
#include <iostream>
#include <sstream>
#include <random>
#include <nlohmann/json.hpp>

std::string generate_random_id() {
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int len = 10;
    std::string id;
    std::default_random_engine rng(std::random_device{}());
    std::uniform_int_distribution<> dist(0, sizeof(alphanum) - 2);

    for (int i = 0; i < len; ++i) {
        id += alphanum[dist(rng)];
    }
    return id;
}

int main() {
    archanaberry::express::AppWeb app;
    Database db("accounts.db");

    app.get("/", [](std::ostream& stream) {
        stream << "HTTP/1.1 200 OK\r\n";
        stream << "Content-Type: text/html\r\n";
        stream << "Connection: close\r\n\r\n";
        stream << R"(
            <!DOCTYPE html>
            <html>
            <head>
                <title>ChatBot API</title>
                <style>
                    body { font-family: Arial, sans-serif; }
                    .container { max-width: 400px; margin: 50px auto; }
                    input[type="text"], input[type="password"] { width: 100%; padding: 10px; margin: 5px 0; }
                    button { width: 100%; padding: 10px; background-color: #4CAF50; color: white; border: none; }
                </style>
            </head>
            <body>
                <div class="container">
                    <h2>Login atau Daftar</h2>
                    <form id="registerForm">
                        <input type="text" id="username" name="username" placeholder="Nama Pengguna (opsional)">
                        <input type="text" id="id" name="id" placeholder="ID akun" readonly>
                        <input type="password" id="password" name="password" placeholder="Password">
                        <button type="button" onclick="register()">Daftar</button>
                    </form>
                    <button type="button" onclick="showLoginForm()">Masuk ke Admin</button>
                    <form id="loginForm" style="display:none;">
                        <input type="text" id="loginId" name="id" placeholder="ID akun">
                        <input type="password" id="loginPassword" name="password" placeholder="Password">
                        <button type="button" onclick="login()">Masuk</button>
                    </form>
                </div>
                <script>
                    document.getElementById('id').value = generateRandomId();
                    function generateRandomId() {
                        return Math.random().toString(36).substring(2, 10);
                    }

                    function showLoginForm() {
                        document.getElementById('registerForm').style.display = 'none';
                        document.getElementById('loginForm').style.display = 'block';
                    }

                    function register() {
                        var username = document.getElementById('username').value;
                        var id = document.getElementById('id').value;
                        var password = document.getElementById('password').value;

                        fetch('/api/register', {
                            method: 'POST',
                            headers: { 'Content-Type': 'application/json' },
                            body: JSON.stringify({ username: username, id: id, password: password })
                        })
                        .then(response => response.json())
                        .then(data => {
                            if (data.success) {
                                alert('Pendaftaran berhasil!');
                            } else {
                                alert('Pendaftaran gagal: ' + data.error);
                            }
                        });
                    }

                    function login() {
                        var id = document.getElementById('loginId').value;
                        var password = document.getElementById('loginPassword').value;

                        fetch('/api/login', {
                            method: 'POST',
                            headers: { 'Content-Type': 'application/json' },
                            body: JSON.stringify({ id: id, password: password })
                        })
                        .then(response => response.json())
                        .then(data => {
                            if (data.success) {
                                alert('Login berhasil!');
                            } else {
                                alert('Login gagal: ' + data.error);
                            }
                        });
                    }
                </script>
            </body>
            </html>
        )";
    });

    app.post("/api/register", [&db](std::ostream& stream, std::istream& request_stream) {
        nlohmann::json body;
        request_stream >> body;
        std::string username = body["username"];
        std::string id = body["id"];
        std::string password = body["password"];

        if (db.userExists(id)) {
            stream << "HTTP/1.1 400 Bad Request\r\n";
            stream << "Content-Type: application/json\r\n";
            stream << "Connection: close\r\n\r\n";
            stream << R"({"success": false, "error": "ID already exists"})";
        } else if (db.usernameExists(username)) {
            stream << "HTTP/1.1 400 Bad Request\r\n";
            stream << "Content-Type: application/json\r\n";
            stream << "Connection: close\r\n\r\n";
            stream << R"({"success": false, "error": "Username already exists"})";
        } else {
            db.addUser(username, id, password);
            stream << "HTTP/1.1 200 OK\r\n";
            stream << "Content-Type: application/json\r\n";
            stream << "Connection: close\r\n\r\n";
            stream << R"({"success": true})";
        }
    });

    app.post("/api/login", [&db](std::ostream& stream, std::istream& request_stream) {
        nlohmann::json body;
        request_stream >> body;
        std::string id = body["id"];
        std::string password = body["password"];

        if (db.validateUser(id, password)) {
            stream << "HTTP/1.1 200 OK\r\n";
            stream << "Content-Type: application/json\r\n";
            stream << "Connection: close\r\n\r\n";
            stream << R"({"success": true})";
        } else {
            stream << "HTTP/1.1 400 Bad Request\r\n";
            stream << "Content-Type: application/json\r\n";
            stream << "Connection: close\r\n\r\n";
            stream << R"({"success": false, "error": "Invalid credentials"})";
        }
    });

    app.listen(8080);

    return 0;
}