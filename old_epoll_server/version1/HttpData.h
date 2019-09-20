/*
 * @Description: 处理http的请求和解析，每个http对象
 * @Author: hahagiraffe
 * @Date: 2019-07-19 20:48:12
 */
#ifndef HTTPDATA_H
#define HTTPDATA_H
#include <string>
#include <unistd.h>
#include <map>
#include <functional>
#include <memory>

class HttpServer;
enum ProcessState
{
    STATE_PARSE_URI = 1,
    STATE_PARSE_HEADERS,
    STATE_RECV_BODY,
    STATE_ANALYSIS,
    STATE_FINISH
};

enum URIState
{
    PARSE_URI_AGAIN = 1,
    PARSE_URI_ERROR,
    PARSE_URI_SUCCESS,
};

enum HeaderState
{
    PARSE_HEADER_SUCCESS = 1,
    PARSE_HEADER_AGAIN,
    PARSE_HEADER_ERROR
};

enum AnalysisState
{
    ANALYSIS_SUCCESS = 1,
    ANALYSIS_ERROR
};

enum ParseState
{
    H_START = 0,
    H_KEY,
    H_COLON,
    H_SPACES_AFTER_COLON,
    H_VALUE,
    H_CR,
    H_LF,
    H_END_CR,
    H_END_LF
};

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

class HttpData {
public:
    HttpData(int fd);
    ~HttpData(){
        ::close(fd_);
    }
    void reset();
    std::shared_ptr<HttpServer> getserver(){
        return server_;
    }
private:
    std::shared_ptr<HttpServer> server_;
    int fd_;
    std::string inputbuffer_;
    std::string outputbuffer_;
    ConnectionState connectionstate_;
    HttpMethod method_;
    HttpVersion version_;
    std::string filename_;
    std::string path_;
    ProcessState state_;
    ParseState hState_;
    std::map<std::string, std::string> headers_;
    bool error_;
    int nowreadpos_;

    void handleRead(int);
    void handleWrite(int);
    void handleConn(__uint32_t* );
    void handleError(int fd,int errornum,std::string errormessage);
    URIState parseURI();
    HeaderState parseHeaders();
    AnalysisState analysisRequest();
};
#endif