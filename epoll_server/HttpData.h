/*
 * @Description: 处理http的请求和解析
 * @Author: hahagiraffe
 * @Date: 2019-07-19 20:48:12
 */
#ifndef HTTPDATA_H
#define HTTPDATA_H
#include <string>

enum ConnectionState
{
    H_CONNECTED = 1,
    H_DISCONNECTING,
    H_DISCONNECTED    
};

enum HttpMethod
{
    METHOD_POST = 1,
    METHOD_GET,
    METHOD_HEAD
};

enum HttpVersion
{
    HTTP_10 = 1,
    HTTP_11
};

class HttpData{
public:
    HttpData();
private:
    int fd_;
    std::string inputbuffer_;
    std::string outputbuffer_;
    ConnectionState connectionstate_;
    HttpMethod method_;
    HttpVersion version_;
    std::string filename_;
    std::string path_;

    void handleRead();
    void handleWrite();
    void handleConn();
};
#endif