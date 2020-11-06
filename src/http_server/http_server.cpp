#include <iostream>
#include <string>

#include <unistd.h>

#include "../include/server.hpp"
#include "../include/http.hpp"


void handler (int clientfd) {
    auto [client_ip_str, client_port] = get_ip_port_from_socket(clientfd);

    http::Request req{clientfd};
    req.receive_and_parse();

    http::Response resp{req.get_http_version()};

    if (req.has_error()) {
        auto [status_code, error_msg] = req.get_error();
        std::cerr << "Error on " << client_ip_str << ":" << client_port << " : " << error_msg << std::endl;

        if (!resp.prepare_status_code(status_code)) {
            std::cerr << "status code " << status_code << " is not implemented" << std::endl;
            status_code = http::HTTP_STATUS_Internal_Server_Error;
            resp.prepare_status_code(status_code);
        }
    }
    else {
        resp.prepare_html("Hello!");
    }

    resp.send(clientfd);
    if (resp.has_error()) {
        std::string error_msg = resp.get_error();
        std::cerr << "Error on " << client_ip_str << ":" << client_port << " : " << error_msg << std::endl;
    }

    close(clientfd);
    std::cout << "Connection from " << client_ip_str << ":" << client_port << " closed." << std::endl;
}


int main (int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./http_server bind_addr port\n"
                  << "    If you don't know what bind_addr is, just leave it '0.0.0.0'\n"
                  << "\n"
                  << "Example:\n"
                  << "    ./http_server 0.0.0.0 8888\n";
        std::exit(EXIT_FAILURE);
    }

    Server server{argv[1], std::atoi(argv[2]), handler};

    server.run();

    return 0;
}
