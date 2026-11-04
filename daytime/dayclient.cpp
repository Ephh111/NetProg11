#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> [port]\n";
        return 1;
    }
    const char* server_ip = argv[1];
    int port = (argc >= 3) ? std::atoi(argv[2]) : 13;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        return 1;
    }

    sockaddr_in srv_addr{};
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &srv_addr.sin_addr) != 1) {
        std::cerr << "Invalid server IP: " << server_ip << "\n";
        close(sock);
        return 1;
    }

    const char msg[] = "";
    if (sendto(sock, msg, sizeof(msg), 0,
               reinterpret_cast<sockaddr*>(&srv_addr), sizeof(srv_addr)) == -1) {
        perror("sendto");
        close(sock);
        return 1;
    }

    char buf[1024];
    sockaddr_in from_addr{};
    socklen_t len = sizeof(from_addr);
    ssize_t rc = recvfrom(sock, buf, sizeof(buf) - 1, 0,
                          reinterpret_cast<sockaddr*>(&from_addr), &len);
    if (rc == -1) {
        perror("recvfrom");
        close(sock);
        return 1;
    }
    buf[rc] = '\0';

    std::cout << "Daytime from " << server_ip << ": " << buf;
    close(sock);
    return 0;
}
