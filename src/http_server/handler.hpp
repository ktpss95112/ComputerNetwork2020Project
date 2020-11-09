#ifndef HANDLER_HPP
#define HANDLER_HPP


#include <filesystem>

#include "../include/http.hpp"


class Handler {
private:
    const std::filesystem::path root_dir_;

    void route (http::Request &req, http::Response &resp);
    void py_handler (http::Request &req, http::Response &resp);

public:
    Handler(std::filesystem::path root_dir);
    ~Handler();

    void handler (int clientfd);
};


#endif
