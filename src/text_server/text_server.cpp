#include <iostream>
#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "../include/server.hpp"


void handler (int clientfd) {
    auto [client_ip_str, client_port] = get_ip_port_from_socket(clientfd);

    int n;
    char buf[4096 + 1] = {0};
    while ((n = read(clientfd, buf, 4096)) > 0) {
        buf[n] = '\0';
        std::cout << client_ip_str << ":" << client_port << " says: " << buf << std::flush;
    }

    close(clientfd);
    std::cout << "Connection from " << client_ip_str << ":" << client_port << " closed." << std::endl;
}


int main (int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./text_server bind_addr port\n"
                  << "    If you don't know what bind_addr is, just leave it '0.0.0.0'\n"
                  << "\n"
                  << "Example:\n"
                  << "    ./text_server 0.0.0.0 8888\n";
        std::exit(EXIT_FAILURE);
    }

    Server server{argv[1], std::atoi(argv[2]), handler};

    server.run();

    return 0;
}
