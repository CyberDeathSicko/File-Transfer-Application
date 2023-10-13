#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/md5.h> 

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error: Could not create socket." << std::endl;
        return 1;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(1234);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress) == -1) {
        std::cerr << "Error: Could not bind." << std::endl;
        close(serverSocket);
        return 1;
    }

    listen(serverSocket, 5);
    std::cout << "Server listening on port 1234..." << std::endl;

    int clientSocket;
    struct sockaddr_in clientAddress;
    socklen_t clientSize = sizeof(clientAddress);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientSize);
    std::cout << "Client connected." << std::endl;

    std::string fileName = "received_file.txt";
    std::ofstream outputFile(fileName, std::ios::out | std::ios::binary);
    MD5_CTX md5Context;
    MD5_Init(&md5Context);

    char buffer[1024];
    int bytesRead;
    long long totalBytesReceived = 0;
    while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0) > 0)) {
        outputFile.write(buffer, bytesRead);
        MD5_Update(&md5Context, buffer, bytesRead);
        totalBytesReceived += bytesRead;
    }

    MD5_Final(hash, &md5Context);
    std::cout << "File received and saved as " << fileName << std::endl;
    std::cout << "Total bytes received: " << totalBytesReceived << std::endl;
    std::cout << "MD5 Checksum: ";
    for(int i = 0; i < MD5_DIGEST_LENGTH; i++)
        printf("%02x", hash[i]);
    std::cout << std::endl;

    close(clientSocket);
    close(serverSocket);

    return 0;
}