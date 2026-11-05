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
    int port = (argc >= 3) ? std::atoi(argv[2]) : 7;

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        perror("socket");
        return 1;
    }

    sockaddr_in srv_addr{};
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &srv_addr.sin_addr) != 1) {
        std::cerr << "Invalid server IP: " << server_ip << "\n";
        close(s);
        return 1;
    }

    if (connect(s, reinterpret_cast<const sockaddr*>(&srv_addr),
                sizeof(srv_addr)) == -1) {
        perror("connect");
        close(s);
        return 1;
    }

    char msg[1024];
    char buf[1024];

    while (true) {
        std::cout << "> " << std::flush;
        if (!std::cin.getline(msg, sizeof(msg))) break;   // Ctrl+D / EOF
        if (std::strcmp(msg, "quit") == 0) break;

        std::strcat(msg, "\n");
        ssize_t rc = send(s, msg, std::strlen(msg), 0);
        if (rc == -1) {
            perror("send");
            break;
        }

        rc = recv(s, buf, sizeof(buf) - 1, 0);
        if (rc == -1) {
            perror("recv");
            break;
        }
        if (rc == 0) break;                             
        buf[rc] = '\0';
        std::cout << "echo: " << buf;
    }

    close(s);
    return 0;
}
