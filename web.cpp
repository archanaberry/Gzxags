#include "libarchanaberry/archanaberry_express.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return buffer;
}

int main() {
    archanaberry::express::AppWeb app;

    app.get("/", [](boost::asio::ip::tcp::iostream& stream) {
        stream << "HTTP/1.1 200 OK\r\n";
        stream << "Content-Type: text/html\r\n";
        stream << "Connection: close\r\n\r\n";
        stream << R"(
            <!DOCTYPE html>
            <html>
            <head>
                <title>Hello, I'am Archana Berry UwU</title>
                <style>
                    body { font-family: Arial, sans-serif; }
                    .archanaberrydev {
                        position: absolute;
                        top: 30%;
                        zoom: 2;
                        left: -4%;
                        transform: rotate(45deg);
                    }
                </style>
            </head>
            <body>
                <h1>Hello, I'am Archana Berry UwU</h1>
                <img class="archanaberrydev" src="berry.png" alt="Archana Berry">
                <script>
                    console.log('Hello from JavaScript');
                </script>
            </body>
            </html>
        )";
        stream.flush();
    });

    app.get("/berry.png", [](boost::asio::ip::tcp::iostream& stream) {
        std::string filename = "berry.png";
        std::vector<char> fileContent = readFile(filename);
        
        if (!fileContent.empty()) {
            stream << "HTTP/1.1 200 OK\r\n";
            stream << "Content-Type: image/jpeg\r\n";
            stream << "Content-Length: " << fileContent.size() << "\r\n";
            stream << "Connection: close\r\n\r\n";
            stream.write(fileContent.data(), fileContent.size());
        } else {
            stream << "HTTP/1.1 404 Not Found\r\n";
            stream << "Content-Type: text/html\r\n";
            stream << "Connection: close\r\n\r\n";
            stream << "<html><body><h1>File not found</h1></body></html>";
        }
        stream.flush();
    });

    app.listen(8080);

    return 0;
}