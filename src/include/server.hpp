#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <tuple>
#include <functional>


std::tuple<std::string, int> get_ip_port_from_socket (int sockfd);


typedef std::function<void(int)> Handle_function;


class Server {
private:
    std::string ip_address_;
    int port_;
    Handle_function conn_handler_;

    int sockfd_;

    int accept_connection ();

public:
    Server (std::string ip_address, int port, Handle_function conn_handler);
    ~Server ();

    void run ();
};

#endif
