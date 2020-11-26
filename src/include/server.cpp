#include <iostream>
#include <string>
#include <tuple>
#include <thread>
#include <functional>
#include <cstdlib>
#include <cstring>
#include <cerrno>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "server.hpp"


std::tuple<std::string, int> get_ip_port_from_socket (int sockfd) {

    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);
    char ipstr[INET6_ADDRSTRLEN];
    int port;
    if (getpeername(sockfd, (struct sockaddr*)&addr, &len) == -1) {
        std::cerr << "Error on getpeername, errno = " << errno << ": " << std::strerror(errno) << std::endl;
        return std::make_tuple("", -1);
    }

    if (addr.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&addr;
        port = ntohs(s->sin_port);
        inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
    } else { // AF_INET6
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
        port = ntohs(s->sin6_port);
        inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
    }

    return std::make_tuple(ipstr, port);
}


Server::Server (std::string ip_address, int port, Handle_function conn_handler)
: ip_address_(ip_address), port_(port), conn_handler_(conn_handler) {

    addrinfo hints;
    addrinfo *res;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::string port_str = std::to_string(port_);

    if (int status = getaddrinfo(ip_address.c_str(), port_str.c_str(), &hints, &res); status != 0) {
        std::cerr << "Error on getaddrinfo: " << gai_strerror(status) << std::endl;
        exit(EXIT_FAILURE);
    }

    if ((sockfd_ = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1 ) {
        std::cerr << "Error on creating socket, errno = " << errno << ": " << std::strerror(errno) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    if (int optval = 1; setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1 ) {
        std::cerr << "Error on setsockopt, errno = " << errno << ": " << std::strerror(errno) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    if (bind(sockfd_, res->ai_addr, res->ai_addrlen) == -1) {
        std::cerr << "Error on bind, errno = " << errno << ": " << std::strerror(errno) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    if (listen(sockfd_, 128) == -1) {
        std::cerr << "Error on listen, errno = " << errno << ": " << std::strerror(errno) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::cerr << "Listening on " << ip_address << ":" << port << " ... " << std::endl;

    freeaddrinfo(res);
}


Server::~Server () {

}


int Server::accept_connection () {

    sockaddr_storage client_address;
    socklen_t addr_len = sizeof(client_address);

    int clientfd;
    if ((clientfd = accept(sockfd_, (sockaddr *)&client_address, &addr_len)) == -1 ) {
        // TODO: error handling
        std::cerr << "Error on accept, errno = " << errno << ": " << std::strerror(errno) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    auto [client_ip_str, client_port] = get_ip_port_from_socket(clientfd);

    std::cerr << "[info] accept connection from " << client_ip_str << ":" << client_port << std::endl;

    return clientfd;
}


void Server::run () {
    while (1) {
        int clientfd = accept_connection();
        // TODO: timeout a connection
        std::thread{conn_handler_, clientfd}.detach();
    }
}
