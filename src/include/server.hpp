#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <tuple>


std::tuple<std::string, int> get_ip_port_from_socket (int sockfd);


typedef void (*Handler) (int);


class Server {
private:
    std::string ip_address_;
    int port_;
    void (*conn_handler_) (int);

    int sockfd_;

    int accept_connection ();

public:
    Server (std::string ip_address, int port, Handler conn_handler);
    ~Server ();

    void run ();
};

#endif
