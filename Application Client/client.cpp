#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error: Could not create socket." << std::endl;
        return 1;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(1234);
    serverAddress.sin_addr.s_addr = inet_addr("server_ip_here");

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error: Could not connect to the server." << std::endl;
        close(clientSocket);
        return 1;
    }

    std::string fileName = "file_to_send.txt";
    std::ifstream inputFile(fileName, std::ios::in | std::ios::binary);

    char buffer[1024];
    int bytesRead;
    long long totalBytesSent = 0;
    while (!inputFile.eof()) {
        inputFile.read(buffer, sizeof(buffer));
        bytesRead = inputFile.gcount();
        send(clientSocket, buffer, bytesRead, 0);
        totalBytesSent += bytesRead;
    }

    close(clientSocket);

    return 0;
}