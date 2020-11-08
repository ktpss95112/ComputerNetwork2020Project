#include <iostream>
#include <string>
#include <functional>

#include <unistd.h>

#include "../include/server.hpp"
#include "../include/http.hpp"
#include "./handler.hpp"


int main (int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: ./http_server bind_addr port root_dir\n"
                  << "    If you don't know what bind_addr is, just leave it '0.0.0.0'\n"
                  << "\n"
                  << "Example:\n"
                  << "    ./http_server 0.0.0.0 8888 ../../website\n";
        std::exit(EXIT_FAILURE);
    }

    Handler handler{argv[3]};
    Server server{argv[1], std::atoi(argv[2]), std::bind(&Handler::handler, &handler, std::placeholders::_1)};

    server.run();

    return 0;
}
