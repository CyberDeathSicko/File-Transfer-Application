#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/md5.h>
#include <thread>
#include <unordered_map>
#include <vector>

std::unordered_map<std::string, std::string> users = {
    {"user1", "password1"},
    {"user2", "password2"}
};

void handleClient(int clientSocket) {
    bool isAuthenticated = false;
    std::string username;

    while (!isAuthenticated) {
        char buffer[256];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesRead <= 0) {
            break;
        }

        std::string credentials(buffer, bytesRead);
        size_t pos = credentials.find(':');
        if (pos != std::string::npos) {
            std::string receivedUsername = credentials.substr(0, pos);
            std::string receivedPassword = credentials.substr(pos + 1);

            if (users.find(receivedUsername) != users.end() && users[receivedUsername] == receivedPassword) {
                isAuthenticated = true;
                username = receivedUsername;
                send(clientSocket, "Authenticated", 13, 0);
            } else {
                send(clientSocket, "Authentication Failed", 21, 0);
            }
        }
    }

    if (!isAuthenticated) {
        close(clientSocket);
        return;
    }
}