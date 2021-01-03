#include <iostream>
#include <sstream>
#include <stdexcept>
#include <regex>

#include <cstdio>
#include <cstring>

#include "http.hpp"


namespace http {


Request::Request (int clientfd) : clientfd_(clientfd), has_error_(false), error_status_code_(empty_status_code) {
    if ((fp_ = fdopen(clientfd, "r")) == nullptr) {
        set_error(HTTP_STATUS_Internal_Server_Error, std::string{"fdopen: "} + std::strerror(errno));
    }
}


Request::~Request () {
    fclose(fp_);
}


void Request::set_error (http_status_code code, const std::string &msg) {
    has_error_ = true;
    error_status_code_ = code;
    error_msg_ = msg;
}


bool Request::parse_method () {
    char buf[11];
    if (fscanf(fp_, "%10s", buf) != 1) {
        set_error(HTTP_STATUS_Bad_Request, "bad HTTP method");
        return false;
    }

    all_str_ += buf;
    all_str_ += " ";

    try {
        method_ = str2method.at(buf);
    }
    catch (const std::out_of_range &e) {
        set_error(HTTP_STATUS_Bad_Request, "bad HTTP method");
        return false;
    }

    return true;
}


bool Request::parse_path_and_query () {
    // path
    char buf[2010];
    if (fscanf(fp_, "%2000s", buf) != 1) {
        set_error(HTTP_STATUS_Bad_Request, "bad path");
        return false;
    }

    if (strlen(buf) == 2000) {
        set_error(HTTP_STATUS_URI_Too_Long, "request-uri too long");
        return false;
    }

    all_str_ += buf;
    all_str_ += " ";

    std::string tmp = buf;
    size_t pos = tmp.find("?");
    path_ = tmp.substr(0, pos);

    // query
    if (pos == std::string::npos) {
        return true;
    }

    std::istringstream ss{tmp.substr(pos+1)};
    std::string expr;
    while (std::getline(ss, expr, '&')) {
        size_t pos = expr.find("=");
        std::string value = (pos == std::string::npos) ? std::string{""} : expr.substr(pos+1);

        query_.insert({expr.substr(0, pos), value});
    }

    return true;
}


bool Request::parse_version () {
    char buf[30];
    if (fscanf(fp_, "%20s", buf) != 1) {
        set_error(HTTP_STATUS_Bad_Request, "bad HTTP version");
        return false;
    }

    all_str_ += buf;
    all_str_ += "\r\n";

    if (!std::regex_match(std::string{buf}, std::regex{"^HTTP/[0-9]+\\.[0-9]+$"})) {
        set_error(HTTP_STATUS_Bad_Request, "bad HTTP version");
        return false;
    }

    http_version_ = buf;

    return true;
}


bool Request::parse_headers () {
    char buf[1000 + 10];

    // consume the CRLF left in input stream
    if (fgets(buf, 1000, fp_) == nullptr) {
        set_error(HTTP_STATUS_Bad_Request, "bad header");
        return false;
    }

    while (fgets(buf, 1000, fp_) != nullptr) {
        std::string line{buf};
        if (line == "\r\n") {
            all_str_ += "\r\n";
            break;
        }
        if (line.size() < 2) {
            set_error(HTTP_STATUS_Bad_Request, "bad header");
            return false;
        }

        if (line.substr(line.size()-2, 2) != "\r\n") {
            if (line.size() > 998) {
                set_error(HTTP_STATUS_Request_Header_Fields_Too_Large, "header too long");
                return false;
            }
            else {
                set_error(HTTP_STATUS_Bad_Request, "bad header, not end with CRLF");
                return false;
            }
        }

        all_str_ += line;

        size_t pos = line.find(":");
        if (pos == std::string::npos) {
            set_error(HTTP_STATUS_Bad_Request, "bad header");
            return false;
        }

        std::string key = line.substr(0, pos);
        try {
            http_header _header = str2header.at(key);
            std::string value = (pos == std::string::npos) ? std::string{""} : line.substr(pos+1, line.size()-pos-2); // strip the newline at the end
            headers_[_header] = value;
        }
        catch (const std::out_of_range &e) {

        }
    }

    return true;
}


bool Request::parse_body () {
    if (headers_.find(HTTP_HEADER_Content_Length) == headers_.end()) {
        return true;
    }

    size_t len = std::stol(headers_.at(HTTP_HEADER_Content_Length));

    char *buf;
    try {
        buf = new char [len+1];
    }
    catch (std::bad_alloc &e) {
        set_error(HTTP_STATUS_Internal_Server_Error, "error on buf = new char [len+1]");
        return false;
    }

    // TODO: timeout a read
    size_t n = fread(buf, 1, len, fp_);
    buf[n] = '\0';
    body_ += buf;

    all_str_ += body_;
    delete [] buf;

    return true;
}


// return true on success; false on failure
bool Request::receive_and_parse () {
    if (has_error_) {
        return false;
    }

    if (!parse_method()
        || !parse_path_and_query()
        || !parse_version()
        || !parse_headers()
        || !parse_body()) {

        return false;
    }

    return true;
}


std::string Request::get_http_version () {
    return http_version_;
}


std::string Request::get_path () {
    return path_;
}


std::string Request::get_py_handler_string () {
    return all_str_;
}


HTTP_Header& Request::get_http_headers () {
    return headers_;
}


bool Request::has_error () {
    return has_error_;
}


std::tuple<http_status_code, std::string> Request::get_error () {
    return std::make_tuple(error_status_code_, error_msg_);
}


void Request::debug () {
    std::cout << method_ << std::endl
              << path_ << std::endl;

    for (auto const &it : query_) {
        std::cout << "query " << it.first << "=" << it.second << std::endl;
    }

    std::cout << http_version_ << std::endl;

    for (auto const &it : headers_) {
        std::cout << "headers " << it.first << ":" << it.second << std::endl;
    }

    std::cout << body_ << std::endl;

}


Response::Response (std::string http_version) : has_error_(false), http_version_(http_version), http_status_code_(HTTP_STATUS_OK), from_py_handler_(false) {

}


Response::~Response () {

}


// will replace the old header if existing
bool Response::add_header (http_header header, const std::string &value) {
    headers_[header] = value;

    return true;
}


bool Response::set_body (const std::string &body) {
    body_ = body;
    add_header(HTTP_HEADER_Content_Length, std::to_string(body.size()));

    return true;
}


bool Response::set_status_code (http_status_code code) {
    http_status_code_ = code;

    return true;
}


bool Response::prepare_body (const std::string &content, const std::string &content_type) {
    if (set_body(content)
        && add_header(HTTP_HEADER_Content_Type, content_type)) {

        return true;
    }

    return false;
}


bool Response::prepare_status_code (http_status_code code) {
    switch (code) {
    case HTTP_STATUS_Not_Found: {

        http_status_code_ = code;
        headers_.clear();
        set_body("Not Found");

        return true;
    }

    case HTTP_STATUS_Internal_Server_Error: {

        http_status_code_ = code;
        headers_.clear();
        set_body("Internal Server Error");

        return true;
    }

    case HTTP_STATUS_Bad_Request: {

        http_status_code_ = code;
        headers_.clear();
        set_body("Bad Request");

        return true;
    }

    case HTTP_STATUS_Partial_Content: {

        http_status_code_ = code;
        return true;
    }

    default: {
        try {
            std::cerr << "[warn] prepare_status_code " << http_status2str.at(code) << " not implemented" << std::endl;
        }
        catch (const std::out_of_range &e) {
            std::cerr << "status code " << code << " is not implemented" << std::endl;
        }
        return true;
    }

    }
}


bool Response::prepare_from_py_handler (const std::string &py_handler_result) {
    from_py_handler_ = true;
    py_handler_result_ = py_handler_result;
    return true;
}


void Response::set_error (const std::string &msg) {
    has_error_ = true;
    error_msg_ = msg;
}


// return true on success; false on failure
bool Response::send (int clientfd) {
    FILE *fp;
    if ((fp = fdopen(clientfd, "w")) == nullptr) {
        set_error(std::string{"fdopen: "} + std::strerror(errno));
        return false;
    }
    setvbuf(fp, NULL, _IONBF, 0);

    if (from_py_handler_) {
        if (fwrite(py_handler_result_.c_str(), py_handler_result_.size(), 1, fp) != 1) {
            set_error("error on fwrite (py_handler_result_)");
            return false;
        }

        fclose(fp);
        return true;
    }

    std::string SP{" "};
    std::string CRLF{"\r\n"};

    std::string status_code_str = std::to_string(http_status_code_);
    std::string reason_phrase;
    try {
        reason_phrase = http_status2str.at(http_status_code_);
    }
    catch (const std::out_of_range &e) {
        std::cerr << "[warn] http status code " << http_status_code_ << " is not implemented, send \"500 Internal Server Error\" instead" << std::endl;

        prepare_status_code(HTTP_STATUS_Internal_Server_Error);
        status_code_str = std::to_string(HTTP_STATUS_Internal_Server_Error);
        reason_phrase = http_status2str.at(HTTP_STATUS_Internal_Server_Error);
    }

    // Status-Line
    if (fwrite(http_version_.c_str(), http_version_.size(), 1, fp) != 1
        || fwrite(SP.c_str(), SP.size(), 1, fp) != 1
        || fwrite(status_code_str.c_str(), status_code_str.size(), 1, fp) != 1
        || fwrite(SP.c_str(), SP.size(), 1, fp) != 1
        || fwrite(reason_phrase.c_str(), reason_phrase.size(), 1, fp) != 1
        || fwrite(CRLF.c_str(), CRLF.size(), 1, fp) != 1
        ) {

        set_error("error on fwrite (Status-Line)");
        return false;
    }

    // headers
    for (auto const &it : headers_) {
        std::string line{header2str.at(it.first) + ": " + it.second};
        if (fwrite(line.c_str(), line.size(), 1, fp) != 1
            || fwrite(CRLF.c_str(), CRLF.size(), 1, fp) != 1) {

            set_error(std::string{"error on fwrite (header "} + header2str.at(it.first) + ")");
            return false;
        }
    }

    if (fwrite(CRLF.c_str(), CRLF.size(), 1, fp) != 1) {
        set_error("error on fwrite (CRLF after header)");
        return false;
    }

    // message-body
    if (body_.size() != 0 && fwrite(body_.c_str(), body_.size(), 1, fp) != 1) {
        set_error("error on fwrite (body) (if happens on <video> or <audio>, the client possibly closes the connection after obtain the file size");
        return false;
    }

    fclose(fp);
    return true;
}


bool Response::has_error() {
    return has_error_;
}


std::string Response::get_error () {
    return error_msg_;
}


} // namespace http
