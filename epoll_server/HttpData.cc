#include "HttpData.h"
#include <functional>
#include <fcntl.h>
#include <unistd.h>
#include "../src/Log/Logging.h"
#include "HttpServer.h"
#include "IOutil.h"
#include "Epoll.h"
using namespace haha_giraffe;
HttpData::HttpData(int port)
    :server_(new HttpServer(port)),
    fd_(server_->getfd()),
    connectionstate_(ConnectionState::H_CONNECTED),
    method_(HttpMethod::METHOD_GET),
    version_(HttpVersion::HTTP_11),
    state_(ProcessState::STATE_PARSE_URI),
    hState_(ParseState::H_START),
    error_(false),
    nowreadpos_(0)
{
    server_->setreadhandle(std::bind(&HttpData::handleRead,this));
    server_->setwritehandle(std::bind(&HttpData::handleWrite,this));
    server_->setconnectionhandle(std::bind(&HttpData::handleConn,this));
}

//对象重置
void HttpData::reset(){

}

//读事件
void HttpData::handleRead(){
    do{
       //先把fd中的内容读取到应用层缓冲去inputbuffer_;
       bool Isreadable=false;//当readn中读取到数据把其置true
       int readnum=readbuffer(fd_,inputbuffer_,Isreadable);
       LOG_INFO<<inputbuffer_;
       if(connectionstate_==ConnectionState::H_DISCONNECTED){
           inputbuffer_.clear();
           break;
       }
       if(readnum<0){
           error_=true;
           handleError(fd_,400,"Bad request");
           break;
       }
       else if(Isreadable){
           //有请求出现，但是没有读取到数据
           connectionstate_=ConnectionState::H_DISCONNECTING;
           if(!readnum){
               break;
           }
       }
       
       //开始解析HTTP请求报文的请求行
       if(state_==ProcessState::STATE_PARSE_URI){
           URIState flag=parseURI();
           if(flag==URIState::PARSE_URI_AGAIN){
               break;
           }
           else if(flag==URIState::PARSE_URI_ERROR){
               error_=true;
               handleError(fd_,400,"Bad request");
               break;
           }
           else{
               state_=ProcessState::STATE_PARSE_HEADERS;
           }
       }
       //解析HTTP请求报文的头部
       if(state_==ProcessState::STATE_PARSE_HEADERS){
           HeaderState flag=parseHeaders();
           if(flag==HeaderState::PARSE_HEADER_AGAIN){
               break;
           }
           else if(flag==HeaderState::PARSE_HEADER_ERROR){
               error_=true;
               handleError(fd_,400,"Bad request");
               break;
           }
           if(method_==METHOD_POST){
               state_=ProcessState::STATE_RECV_BODY;
           }
           else{
               state_=ProcessState::STATE_ANALYSIS;
           }
       }
       //解析HTTP请求报文的主体
       if(state_==ProcessState::STATE_RECV_BODY){
           int content_length = -1;
           if (headers_.find("Content-length") != headers_.end())
           {
               //string to int
               content_length = stoi(headers_["Content-length"]);
           }
           else
           {
                //cout << "(state_ == STATE_RECV_BODY)" << endl;
                error_ = true;
                handleError(fd_, 400, "Bad Request: Lack of argument (Content-length)");
                break;
           }
           if (static_cast<int>(inputbuffer_.size()) < content_length)
                break;
           state_ = STATE_ANALYSIS;
       }
       //解析HTTP
        if(state_==ProcessState::STATE_ANALYSIS){
            AnalysisState flag=analysisRequest();
            if(flag==AnalysisState::ANALYSIS_ERROR){
                error_=true;
                break;
            }
            state_=ProcessState::STATE_FINISH;
       }
    }while(0);
    if(!error_){
        //前方解析没有错
        //如果输出缓冲区有数据则调用写数据
        if(outputbuffer_.size()>0){
            handleWrite();
        }
        if(!error_ && state_==ProcessState::STATE_FINISH){
            reset();
            if(inputbuffer_.size()>0){
                if(connectionstate_!=ConnectionState::H_DISCONNECTING){
                    handleRead();
                }
            }
        }
        else if(!error_ && connectionstate_!=ConnectionState::H_DISCONNECTED){
            //events_ | EPOLLIN
        }
    }
}
    
//写事件
void HttpData::handleWrite(int eventfd){
    if (!error_ && connectionstate_ != H_DISCONNECTED)
    {
        struct epoll_event event;
        event.data.fd=eventfd;
        
        //epoll_->epoll_mod(eventfd,event);
        //__uint32_t &events_ = channel_->getEvents();
        if (writebuffer(fd_, outputbuffer_,outputbuffer_.size()) < 0)
        {
            perror("writen");
            //events_ = 0;
            error_ = true;
        }
        if (outputbuffer_.size() > 0){
            //events_ |= EPOLLOUT;
            event.events|=EPOLLIN;
        }
    }
}
    
//连接事件
void HttpData::handleConn(){
    
}

//错误处理事件
void HttpData::handleError(int fd,int errornum,std::string errormessage){
    errormessage = " " + errormessage;
    char send_buff[4096];
    std::string body_buff, header_buff;
    body_buff += "<html><title>哎~出错了</title>";
    body_buff += "<body bgcolor=\"ffffff\">";
    body_buff += std::to_string(errornum) + errormessage;
    body_buff += "<hr><em> LinYa's Web Server</em>\n</body></html>";

    header_buff += "HTTP/1.1 " + std::to_string(errornum) + errormessage + "\r\n";
    header_buff += "Content-Type: text/html\r\n";
    header_buff += "Connection: Close\r\n";
    header_buff += "Content-Length: " + std::to_string(body_buff.size()) + "\r\n";
    header_buff += "Server: hahagiraffe's Web Server\r\n";;
    header_buff += "\r\n";
    // 错误处理不考虑writen不完的情况
    sprintf(send_buff, "%s", header_buff.c_str());
    std::string bodystr(send_buff);
    writebuffer(fd, bodystr, bodystr.size());

    memset(send_buff,0,sizeof(send_buff)/sizeof(char));
    sprintf(send_buff, "%s", body_buff.c_str());
    std::string headerstr(send_buff);
    writebuffer(fd, headerstr, headerstr.size());
}

//解析URI
URIState HttpData::parseURI(){
    std::string& str = inputbuffer_;
    size_t pos=str.find('\r',nowreadpos_);
    if(pos<0){
        return URIState::PARSE_URI_AGAIN;
    }
    //把请求行提取出来
    std::string request_line = str.substr(0, pos);
    if (str.size() > pos + 1)
        str = str.substr(pos + 1);//这里inputbuffer中请求行已被删掉
    else 
        str.clear();
    //请求方法
    int posGet = request_line.find("GET");
    int posPost = request_line.find("POST");
    int posHead = request_line.find("HEAD");
    if (posGet >= 0)
    {
        pos = posGet;
        method_ = METHOD_GET;
    }
    else if (posPost >= 0)
    {
        pos = posPost;
        method_ = METHOD_POST;
    }
    else if (posHead >= 0)
    {
        pos = posHead;
        method_ = METHOD_HEAD;
    }
    else
    {
        return PARSE_URI_ERROR;
    }
    //filename
    pos=request_line.find("/",pos);
    if(pos<0){
        filename_="index.html";
        version_=HttpVersion::HTTP_11;
        return PARSE_URI_SUCCESS;
    }
    else
    {
        //pos与_pos之间
        size_t _pos = request_line.find(' ', pos);
        if (_pos < 0)
            return PARSE_URI_ERROR;
        else
        {
            if (_pos - pos > 1)
            {
                filename_ = request_line.substr(pos + 1, _pos - pos - 1);
                size_t __pos = filename_.find('?');
                if (__pos >= 0)
                {
                    filename_ = filename_.substr(0, __pos);
                }
            }
            else{
                filename_ = "index.html";
            }
        }
        pos = _pos;
    }
    //版本号
    pos = request_line.find("/", pos);
    if (pos < 0)
        return PARSE_URI_ERROR;
    else
    {
        if (request_line.size() - pos <= 3)
            return PARSE_URI_ERROR;
        else
        {
            std::string ver = request_line.substr(pos + 1, 3);//"1.1"
            if (ver == "1.0")
                version_=HttpVersion::HTTP_10;
            else if (ver == "1.1")
                version_=HttpVersion::HTTP_11;
            else
                return PARSE_URI_ERROR;
        }
    }
    return PARSE_URI_SUCCESS;
}
    
//解析header
HeaderState HttpData::parseHeaders(){
    std::string &str = inputbuffer_;
    int key_start = -1, key_end = -1, value_start = -1, value_end = -1;
    int now_read_line_begin = 0;
    bool notFinish = true;
    size_t i = 0;
    for (; i < str.size() && notFinish; ++i)
    {
        switch(hState_)
        {
            case H_START:
            {
                if (str[i] == '\n' || str[i] == '\r')
                    break;
                hState_ = H_KEY;
                key_start = i;
                now_read_line_begin = i;
                break;
            }
            case H_KEY:
            {
                if (str[i] == ':')
                {
                    key_end = i;
                    if (key_end - key_start <= 0)
                        return PARSE_HEADER_ERROR;
                    hState_ = H_COLON;
                }
                else if (str[i] == '\n' || str[i] == '\r')
                    return PARSE_HEADER_ERROR;
                break;  
            }
            case H_COLON:
            {
                if (str[i] == ' ')
                {
                    hState_ = H_SPACES_AFTER_COLON;
                }
                else
                    return PARSE_HEADER_ERROR;
                break;  
            }
            case H_SPACES_AFTER_COLON:
            {
                hState_ = H_VALUE;
                value_start = i;
                break;  
            }
            case H_VALUE:
            {
                if (str[i] == '\r')
                {
                    hState_ = H_CR;
                    value_end = i;
                    if (value_end - value_start <= 0)
                        return PARSE_HEADER_ERROR;
                }
                else if (i - value_start > 255)
                    return PARSE_HEADER_ERROR;
                break;  
            }
            case H_CR:
            {
                if (str[i] == '\n')
                {
                    hState_ = H_LF;
                    std::string key(str.begin() + key_start, str.begin() + key_end);
                    std::string value(str.begin() + value_start, str.begin() + value_end);
                    headers_[key] = value;
                    now_read_line_begin = i;
                }
                else
                    return PARSE_HEADER_ERROR;
                break;  
            }
            case H_LF:
            {
                if (str[i] == '\r')
                {
                    hState_ = H_END_CR;
                }
                else
                {
                    key_start = i;
                    hState_ = H_KEY;
                }
                break;
            }
            case H_END_CR:
            {
                if (str[i] == '\n')
                {
                    hState_ = H_END_LF;
                }
                else
                    return PARSE_HEADER_ERROR;
                break;
            }
            case H_END_LF:
            {
                notFinish = false;
                key_start = i;
                now_read_line_begin = i;
                break;
            }
        }
    }
    if (hState_ == H_END_LF)
    {
        str = str.substr(i);
        return PARSE_HEADER_SUCCESS;
    }
    str = str.substr(now_read_line_begin);
    return PARSE_HEADER_AGAIN;
}
    
//解析request
AnalysisState HttpData::analysisRequest(){
    if (method_ == METHOD_POST)
    {
        char buf2[1024];
        snprintf(buf2,sizeof(buf2),"HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\nHello World\n", 12);
        outputbuffer_=std::string(buf2);
        return AnalysisState::ANALYSIS_SUCCESS;
        // ------------------------------------------------------
        // My CV stitching handler which requires OpenCV library
        // ------------------------------------------------------
        // string header;
        // header += string("HTTP/1.1 200 OK\r\n");
        // if(headers_.find("Connection") != headers_.end() && headers_["Connection"] == "Keep-Alive")
        // {
        //     keepAlive_ = true;
        //     header += string("Connection: Keep-Alive\r\n") + "Keep-Alive: timeout=" + to_string(DEFAULT_KEEP_ALIVE_TIME) + "\r\n";
        // }
        // int length = stoi(headers_["Content-length"]);
        // vector<char> data(inBuffer_.begin(), inBuffer_.begin() + length);
        // Mat src = imdecode(data, CV_LOAD_IMAGE_ANYDEPTH|CV_LOAD_IMAGE_ANYCOLOR);
        // //imwrite("receive.bmp", src);
        // Mat res = stitch(src);
        // vector<uchar> data_encode;
        // imencode(".png", res, data_encode);
        // header += string("Content-length: ") + to_string(data_encode.size()) + "\r\n\r\n";
        // outBuffer_ += header + string(data_encode.begin(), data_encode.end());
        // inBuffer_ = inBuffer_.substr(length);
        // return ANALYSIS_SUCCESS;
    }
    else if (method_ == METHOD_GET || method_ == METHOD_HEAD)
    {
        char buf2[1024];
        snprintf(buf2,sizeof(buf2),"HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\nHello World222\n", 12);
        outputbuffer_=std::string(buf2);
        return AnalysisState::ANALYSIS_SUCCESS;
        // string header;
        // header += "HTTP/1.1 200 OK\r\n";
        // if(headers_.find("Connection") != headers_.end() && (headers_["Connection"] == "Keep-Alive" || headers_["Connection"] == "keep-alive"))
        // {
        //     keepAlive_ = true;
        //     header += string("Connection: Keep-Alive\r\n") + "Keep-Alive: timeout=" + to_string(DEFAULT_KEEP_ALIVE_TIME) + "\r\n";
        // }
        // int dot_pos = fileName_.find('.');
        // string filetype;
        // if (dot_pos < 0) 
        //     filetype = MimeType::getMime("default");
        // else
        //     filetype = MimeType::getMime(fileName_.substr(dot_pos));
        

        // // echo test
        // if (fileName_ == "hello")
        // {
        //     outBuffer_ = "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\n\r\nHello World";
        //     return ANALYSIS_SUCCESS;
        // }
        // if (fileName_ == "favicon.ico")
        // {
        //     header += "Content-Type: image/png\r\n";
        //     header += "Content-Length: " + to_string(sizeof favicon) + "\r\n";
        //     header += "Server: LinYa's Web Server\r\n";

        //     header += "\r\n";
        //     outBuffer_ += header;
        //     outBuffer_ += string(favicon, favicon + sizeof favicon);;
        //     return ANALYSIS_SUCCESS;
        // }

        // struct stat sbuf;
        // if (stat(fileName_.c_str(), &sbuf) < 0)
        // {
        //     header.clear();
        //     handleError(fd_, 404, "Not Found!");
        //     return ANALYSIS_ERROR;
        // }
        // header += "Content-Type: " + filetype + "\r\n";
        // header += "Content-Length: " + to_string(sbuf.st_size) + "\r\n";
        // header += "Server: LinYa's Web Server\r\n";
        // // 头部结束
        // header += "\r\n";
        // outBuffer_ += header;
        
        // if (method_ == METHOD_HEAD)
        //     return ANALYSIS_SUCCESS;

        // int src_fd = open(fileName_.c_str(), O_RDONLY, 0);
        // if (src_fd < 0)
        // {
        //   outBuffer_.clear();
        //   handleError(fd_, 404, "Not Found!");
        //   return ANALYSIS_ERROR;
        // }
		// void *mmapRet = mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
		// close(src_fd);
		// if (mmapRet == (void *)-1)
		// {
		// 	munmap(mmapRet, sbuf.st_size);
		// 	outBuffer_.clear();
		// 	handleError(fd_, 404, "Not Found!");
		// 	return ANALYSIS_ERROR;
		// }
        // char *src_addr = static_cast<char*>(mmapRet);
        // outBuffer_ += string(src_addr, src_addr + sbuf.st_size);;
        // munmap(mmapRet, sbuf.st_size);
        // return ANALYSIS_SUCCESS;
    }
    return ANALYSIS_ERROR;
}