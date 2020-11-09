#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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
        py_handler(req, resp);
    }
}


void Handler::py_handler (http::Request &req, http::Response &resp) {
    // pipe
    int fd_to_py[2], fd_from_py[2];
    if (pipe(fd_to_py) < 0
        || pipe(fd_from_py) < 0) {

        std::cerr << "Error on creating pipe to py_handler, errno = " << errno << " : " << std::strerror(errno) << std::endl;
        resp.prepare_status_code(http::HTTP_STATUS_Internal_Server_Error);
        return;
    }

    // fork
    pid_t pid;
    if ((pid = fork()) < 0) {
        std::cerr << "Error on forking py_handler, errno = " << errno << " : " << std::strerror(errno) << std::endl;
        resp.prepare_status_code(http::HTTP_STATUS_Internal_Server_Error);
        return;
    }
    else if (pid == 0) {
        close(fd_to_py[1]);
        close(fd_from_py[0]);
        dup2(fd_to_py[0], STDIN_FILENO);
        dup2(fd_from_py[1], STDOUT_FILENO);
        if (execlp("python3", "python3", "./py_handler/main.py", (char *)nullptr) == -1) {
            std::cerr << "error on execlp, errno = " << errno << " : " << std::strerror(errno) << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
    else {
        close(fd_to_py[0]);
        close(fd_from_py[1]);
    }

    // send request
    FILE *fp_to_py;
    if ((fp_to_py = fdopen(fd_to_py[1], "w")) == nullptr) {
        std::cerr << "error on fdopen(fd_to_py), errno = " << errno << " : " << std::strerror(errno) << std::endl;
        resp.prepare_status_code(http::HTTP_STATUS_Internal_Server_Error);
        return;
    }
    setvbuf(fp_to_py, NULL, _IONBF, 0);

    std::string tmp;
    tmp += std::to_string(req.get_py_handler_string().size());
    tmp += "\n";
    tmp += req.get_py_handler_string();

    if (fwrite(tmp.c_str(), tmp.size(), 1, fp_to_py) != 1) {
        std::cerr << "error on fwrite(fp_to_py), errno = " << errno << " : " << std::strerror(errno) << std::endl;
        resp.prepare_status_code(http::HTTP_STATUS_Internal_Server_Error);
        fclose(fp_to_py);
        return;
    }

    fclose(fp_to_py);

    // receive response
    FILE *fp_from_py;
    if ((fp_from_py = fdopen(fd_from_py[0], "r")) == nullptr) {
        std::cerr << "error on fdopen(fd_from_py), errno = " << errno << " : " << std::strerror(errno) << std::endl;
        resp.prepare_status_code(http::HTTP_STATUS_Internal_Server_Error);
        return;
    }

    char buf[20];
    if (fgets(buf, 19, fp_from_py) == nullptr) {
        std::cerr << "error on fread(fp_from_py), errno = " << errno << " : " << std::strerror(errno) << std::endl;
        resp.prepare_status_code(http::HTTP_STATUS_Internal_Server_Error);
        fclose(fp_from_py);
        return;
    }

    size_t len{std::stoul(buf)};
    char *buf2 = new char [len+1];
    if (fread(buf2, 1, len, fp_from_py) != len) {
        std::cerr << "error on fread(fp_from_py), errno = " << errno << " : " << std::strerror(errno) << std::endl;
        resp.prepare_status_code(http::HTTP_STATUS_Internal_Server_Error);
        fclose(fp_from_py);
        return;
    }
    buf2[len] = '\0';

    resp.prepare_from_py_handler(buf2);

    delete [] buf2;
    fclose(fp_from_py);

    int wstatus;
    if (waitpid(pid, &wstatus, 0) == -1) {
        std::cerr << "error on waitpid, errno = " << errno << " : " << std::strerror(errno) << std::endl;
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
