#include <iostream>
#include <string>
#include <sqlite3.h>
#include <boost/asio.hpp>
#include <sqlite_modern_cpp.h>
#include <bcrypt/BCrypt.hpp>

using namespace boost::asio;

struct User {
    int id;
    std::string username;
    std::string password;
};

void registerUser(sqlite::database& db, const std::string& username, const std::string& password) {
    try {
        auto user = db << "SELECT id, username, password FROM users WHERE username = ?" << username;
        if (user) {
            std::cerr << "Username already exists!" << std::endl;
            return;
        }
        auto salt = bcrypt::generate_salt();
        auto hashedPassword = bcrypt::generate_hash(password, salt);
        db << "INSERT INTO users (username, password) VALUES (?, ?)" << username << hashedPassword;
    } catch (const sqlite::sqlite_exception& e) {
        std::cerr << "Database error: " << e.what() << std::endl;
    }
}

void handleRegistration(sqlite::database& db, const std::string& username, const std::string& password) {
    if (username.empty() || password.empty()) {
        std::cerr << "Username and password are required!" << std::endl;
        return;
    }
    registerUser(db, username, password);
    std::cout << "User registered." << std::endl;
}

int main() {
    io_service io;
    ip::tcp::endpoint endpoint(ip::tcp::v4(), 8080);
    ip::tcp::acceptor acceptor(io, endpoint);
    sqlite::database db("users.db");
    while (true) {
        ip::tcp::socket socket(io);
        acceptor.accept(socket);
        std::istream is(&socket);
        std::string requestString;
        std::string line;
        while (std::getline(is, line)) {
            requestString += line + '\n';
            if (line == "\r" || line == "\r\n") {
                break;
            }
        }
        std::string username = "sample_username";
        std::string password = "sample_password";
        handleRegistration(db, username, password);
        std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 18\r\n\r\nUser registered.";
        std::ostream os(&socket);
        os << response;
    }
    return 0;
}