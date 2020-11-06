#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP


#include <string>
#include <unordered_map>


namespace http {


enum method {
    GET,
    POST
};
const std::unordered_map<std::string, method> str2method{
    {"GET", GET},
    {"POST", POST},
};


enum http_header {
    HTTP_HEADER_Accept,
    HTTP_HEADER_Cookie,
    HTTP_HEADER_Content_Length,
    HTTP_HEADER_Content_Type,
    HTTP_HEADER_Range,
};
const std::unordered_map<std::string, http_header> str2header{
    {"Accept", HTTP_HEADER_Accept},
    {"Cookie", HTTP_HEADER_Cookie},
    {"Content-Length", HTTP_HEADER_Content_Length},
    {"Content-Type", HTTP_HEADER_Content_Type},
    {"Range", HTTP_HEADER_Range},
};
const std::unordered_map<http_header, std::string> header2str{
    {HTTP_HEADER_Accept, "Accept"},
    {HTTP_HEADER_Cookie, "Cookie"},
    {HTTP_HEADER_Content_Length, "Content-Length"},
    {HTTP_HEADER_Content_Type, "Content-Type"},
    {HTTP_HEADER_Range, "Range"},
};


enum http_status_code {
    // the default value specifying that the status code is not initialized
    empty_status_code = 0,

    HTTP_STATUS_OK = 200,

    HTTP_STATUS_Bad_Request = 400,
    HTTP_STATUS_Not_Found = 404,
    HTTP_STATUS_URI_Too_Long = 414,
    HTTP_STATUS_Request_Header_Fields_Too_Large = 431,

    HTTP_STATUS_Internal_Server_Error = 500,
};
const std::unordered_map<http_status_code, std::string> http_status2str{
    {HTTP_STATUS_OK, "200 OK"},

    {HTTP_STATUS_Bad_Request, "400 Bad Request"},
    {HTTP_STATUS_Not_Found, "404 Not Found"},
    {HTTP_STATUS_URI_Too_Long, "414 URI Too Long"},
    {HTTP_STATUS_Request_Header_Fields_Too_Large, "431 Request Header Fields Too Large"},

    {HTTP_STATUS_Internal_Server_Error, "500 Internal Server Error"},
};


typedef std::unordered_map<http_header, std::string> HTTP_Header;


} // namespace http


#endif
