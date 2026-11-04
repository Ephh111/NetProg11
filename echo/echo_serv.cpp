#include <iostream>
#include <cstring>
#include <csignal>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main(int argc, char* argv[]) {
    int port = (argc >= 2) ? std::atoi(argv[1]) : 7;

    std::signal(SIGPIPE, SIG_IGN);

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in self_addr{};
    self_addr.sin_family = AF_INET;
    self_addr.sin_port = htons(port);
    self_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(s, reinterpret_cast<const sockaddr*>(&self_addr),
             sizeof(self_addr)) == -1) {
        perror("bind");
        close(s);
        return 1;
    }

    const int queue_len = 10;
    if (listen(s, queue_len) == -1) {
        perror("listen");
        close(s);
        return 1;
    }
    std::cout << "Echo server listening on port " << port << "\n";

    while (true) {
        sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);

        int work_sock = accept(s, reinterpret_cast<sockaddr*>(&client_addr), &len);
        if (work_sock == -1) {
            perror("accept");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN] = {0};
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        std::cout << "Client connected: " << client_ip
                  << ":" << ntohs(client_addr.sin_port) << "\n";

        char buf[4096];
        ssize_t rc;
        while ((rc = recv(work_sock, buf, sizeof(buf), 0)) > 0) {
            ssize_t sent = 0;
            while (sent < rc) {
                ssize_t part = send(work_sock, buf + sent, rc - sent, 0);
                if (part == -1) {
                    perror("send");
                    break;
                }
                sent += part;
            }
            if (sent != rc) break;
        }
        if (rc == -1) perror("recv");

        close(work_sock);
        std::cout << "Client disconnected\n";
    }

    close(s);
    return 0;
}
