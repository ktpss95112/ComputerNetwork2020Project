#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <unistd.h>

#include "../include/server.hpp"
#include "../include/http.hpp"
#include "handler.hpp"


Handler::Handler(std::filesystem::path root_dir) : root_dir_(root_dir) {

}


Handler::~Handler() {

}


void Handler::route (http::Request &req, http::Response &resp) {
    std::string path = req.get_path();
    if (path == "/"
        || path == "/index.html"
        || path == "/css/style.css"
        || path == "/img/avatar.jpg") {

        if (path == "/") path = "/index.html";
        std::filesystem::path file_path = root_dir_ / path.substr(1);

        std::ifstream fin{file_path, std::ios::binary};
        if (!fin.is_open()) {
            std::cerr << "cannot open " << file_path << std::endl;
            resp.prepare_status_code(http::HTTP_STATUS_Internal_Server_Error);
        }
        else {
            std::stringstream buf;
            buf << fin.rdbuf();

            if (path[path.size()-1] == 'g')      resp.prepare_body(buf.str(), "image/jpeg");
            else if (path[path.size()-1] == 's') resp.prepare_body(buf.str(), "text/css");
            else                                 resp.prepare_body(buf.str(), "text/html");
        }

    }
    else {
        std::cerr << "path " << path << " is not found on the server" << std::endl;
        resp.prepare_status_code(http::HTTP_STATUS_Not_Found);
    }
}


void Handler::handler (int clientfd) {
    auto [client_ip_str, client_port] = get_ip_port_from_socket(clientfd);

    http::Request req{clientfd};
    req.receive_and_parse();

    http::Response resp{req.get_http_version()};

    if (req.has_error()) {
        auto [status_code, error_msg] = req.get_error();
        std::cerr << "Error on " << client_ip_str << ":" << client_port << " : " << error_msg << std::endl;

        if (!resp.prepare_status_code(status_code)) {
            std::cerr << "status code " << status_code << " is not implemented" << std::endl;
            resp.prepare_status_code(http::HTTP_STATUS_Internal_Server_Error);
        }
    }
    else {
        route(req, resp);
    }

    resp.send(clientfd);
    if (resp.has_error()) {
        std::string error_msg = resp.get_error();
        std::cerr << "Error on " << client_ip_str << ":" << client_port << " : " << error_msg << std::endl;
    }

    close(clientfd);
    std::cout << "Connection from " << client_ip_str << ":" << client_port << " closed." << std::endl;
}
