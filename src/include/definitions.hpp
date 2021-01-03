#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP


#include <string>
#include <unordered_map>


namespace http {


enum method {
    GET,
    POST,
    HEAD,
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
    HTTP_HEADER_Accept_Range,
    HTTP_HEADER_Content_Range,
};
const std::unordered_map<std::string, http_header> str2header{
    {"Accept", HTTP_HEADER_Accept},
    {"Cookie", HTTP_HEADER_Cookie},
    {"Content-Length", HTTP_HEADER_Content_Length},
    {"Content-Type", HTTP_HEADER_Content_Type},
    {"Range", HTTP_HEADER_Range},
    {"Accept-Ranges", HTTP_HEADER_Accept_Range},
    {"Content-Range", HTTP_HEADER_Content_Range},
};
const std::unordered_map<http_header, std::string> header2str{
    {HTTP_HEADER_Accept, "Accept"},
    {HTTP_HEADER_Cookie, "Cookie"},
    {HTTP_HEADER_Content_Length, "Content-Length"},
    {HTTP_HEADER_Content_Type, "Content-Type"},
    {HTTP_HEADER_Range, "Range"},
    {HTTP_HEADER_Accept_Range, "Accept-Ranges"},
    {HTTP_HEADER_Content_Range, "Content-Range"},
};


enum http_status_code {
    // the default value specifying that the status code is not initialized
    empty_status_code = 0,

    HTTP_STATUS_OK = 200,
    HTTP_STATUS_Partial_Content = 206,

    HTTP_STATUS_Bad_Request = 400,
    HTTP_STATUS_Not_Found = 404,
    HTTP_STATUS_URI_Too_Long = 414,
    HTTP_STATUS_Request_Header_Fields_Too_Large = 431,

    HTTP_STATUS_Internal_Server_Error = 500,
};
const std::unordered_map<http_status_code, std::string> http_status2str{
    {HTTP_STATUS_OK, "OK"},
    {HTTP_STATUS_Partial_Content, "Partial Content"},

    {HTTP_STATUS_Bad_Request, "Bad Request"},
    {HTTP_STATUS_Not_Found, "Not Found"},
    {HTTP_STATUS_URI_Too_Long, "URI Too Long"},
    {HTTP_STATUS_Request_Header_Fields_Too_Large, "Request Header Fields Too Large"},

    {HTTP_STATUS_Internal_Server_Error, "Internal Server Error"},
};


const std::unordered_map<std::string, std::string> file_ext2content_type{
    {".html", "text/html"},
    {".css", "text/css"},
    {".js", "text/javascript"},
    {".txt", "text/plain"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".mp4", "video/mp4"},
    {".mp3", "audio/mpeg"},
};


typedef std::unordered_map<http_header, std::string> HTTP_Header;


} // namespace http


#endif
