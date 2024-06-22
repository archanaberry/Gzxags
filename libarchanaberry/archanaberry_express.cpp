#include "archanaberry_express.hpp"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "archanaberry_mimetypes.hpp"

using boost::asio::ip::tcp;

namespace archanaberry {
    namespace express {

        void AppWeb::get(const std::string& path, std::function<void(std::ostream&)> handler) {
            routes_get_[path] = handler;
        }

        void AppWeb::post(const std::string& path, Handler handler) {
            routes_post_[path] = handler;
        }

        void AppWeb::serveStatic(const std::string& directory) {
            static_directory_ = directory;
        }

        void AppWeb::listen(int port) {
            boost::asio::io_service io_service;
            tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));

            std::cout << "Mendengarkan server di port: " << port << "\n";
            std::cout << "Tekan CTRL + Z untuk keluar!\n";

            while (true) {
                tcp::socket socket(io_service);
                acceptor.accept(socket);

                std::ostringstream response_stream;
                std::string request;

                boost::asio::streambuf buffer;
                boost::asio::read_until(socket, buffer, "\r\n\r\n"); // Baca sampai header selesai
                std::istream request_stream(&buffer);
                std::getline(request_stream, request);

                handle_request(request, response_stream, request_stream);

                boost::asio::write(socket, boost::asio::buffer(response_stream.str()));
            }
        }

        void AppWeb::handle_request(const std::string& request, std::ostream& response_stream, std::istream& request_stream) {
            std::istringstream request_line(request);
            std::string method, path, version;
            request_line >> method >> path >> version;

            if (method == "GET" && routes_get_.find(path) != routes_get_.end()) {
                routes_get_[path](response_stream);
            } else if (method == "POST" && routes_post_.find(path) != routes_post_.end()) {
                routes_post_[path](response_stream, request_stream);
            } else {
                serve_file(path, response_stream);
            }
        }

        void AppWeb::serve_file(const std::string& path, std::ostream& response_stream) {
            std::string full_path = static_directory_ + path;
            std::ifstream file(full_path, std::ios::binary);
            if (file) {
                std::string mime_type = get_mime_type(path);
                response_stream << "HTTP/1.1 200 OK\r\n";
                response_stream << "Content-Type: " << mime_type << "\r\n";

                std::ostringstream buffer;
                buffer << file.rdbuf();
                std::string content = buffer.str();
                response_stream << "Content-Length: " << content.size() << "\r\n";
                response_stream << "Connection: close\r\n\r\n";
                response_stream << content;
            } else {
                response_stream << "HTTP/1.1 404 Not Found\r\n";
                response_stream << "Content-Type: text/plain\r\n";
                response_stream << "Content-Length: 9\r\n";
                response_stream << "Connection: close\r\n\r\n";
                response_stream << "Not Found";
            }
        }

        AppWeb appweb;

    } // namespace express
} // namespace archanaberry