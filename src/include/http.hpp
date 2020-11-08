#ifndef HTTP_HPP
#define HTTP_HPP


#include <string>
#include <tuple>
#include <unordered_map>

#include "definitions.hpp"


namespace http {


class Request {
private:
    int clientfd_;
    FILE *fp_;

    bool has_error_;
    http_status_code error_status_code_;
    std::string error_msg_;

    method method_;
    std::string path_;
    std::unordered_map<std::string, std::string> query_;
    std::string http_version_;
    HTTP_Header headers_;
    std::string body_;

    bool parse_method ();
    bool parse_path_and_query ();
    bool parse_version ();
    bool parse_headers ();
    bool parse_body ();

    void set_error (http_status_code code, const std::string &msg);

public:
    Request (int clientfd);
    ~Request ();

    bool receive_and_parse ();
    std::string get_http_version ();
    std::string get_path ();

    bool has_error ();
    std::tuple<http_status_code, std::string> get_error ();

    void debug ();
};


class Response {
private:
    bool has_error_;
    std::string error_msg_;

    std::string http_version_;
    http_status_code http_status_code_;
    HTTP_Header headers_;
    std::string body_;

    void set_error (const std::string &msg);

public:
    Response (std::string http_version);
    ~Response ();

    // some low level control of the response
    bool add_header (http_header header, const std::string &value);
    bool set_body (const std::string &body);
    bool set_status_code (http_status_code code);

    // some high level operation of the response
    bool prepare_body (const std::string &content, const std::string &content_type);
    bool prepare_status_code(http_status_code code);

    bool send (int clientfd);

    bool has_error ();
    std::string get_error ();
};


} // namespace http


#endif
