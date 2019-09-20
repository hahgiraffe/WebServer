/*
 * @Description: 
 * @Author: hahagiraffe
 * @Date: 2019-08-15 16:53:15
 */
#include "HttpParse.h"
#include "EventLoop.h"
#include "Channel.h"
#include "./Log/Logging.h"
#include "Socket.h"
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <functional>
#include <memory>
using namespace haha_giraffe;

char favicon[555] = {
  '\x89', 'P', 'N', 'G', '\xD', '\xA', '\x1A', '\xA',
  '\x0', '\x0', '\x0', '\xD', 'I', 'H', 'D', 'R',
  '\x0', '\x0', '\x0', '\x10', '\x0', '\x0', '\x0', '\x10',
  '\x8', '\x6', '\x0', '\x0', '\x0', '\x1F', '\xF3', '\xFF',
  'a', '\x0', '\x0', '\x0', '\x19', 't', 'E', 'X',
  't', 'S', 'o', 'f', 't', 'w', 'a', 'r',
  'e', '\x0', 'A', 'd', 'o', 'b', 'e', '\x20',
  'I', 'm', 'a', 'g', 'e', 'R', 'e', 'a',
  'd', 'y', 'q', '\xC9', 'e', '\x3C', '\x0', '\x0',
  '\x1', '\xCD', 'I', 'D', 'A', 'T', 'x', '\xDA',
  '\x94', '\x93', '9', 'H', '\x3', 'A', '\x14', '\x86',
  '\xFF', '\x5D', 'b', '\xA7', '\x4', 'R', '\xC4', 'm',
  '\x22', '\x1E', '\xA0', 'F', '\x24', '\x8', '\x16', '\x16',
  'v', '\xA', '6', '\xBA', 'J', '\x9A', '\x80', '\x8',
  'A', '\xB4', 'q', '\x85', 'X', '\x89', 'G', '\xB0',
  'I', '\xA9', 'Q', '\x24', '\xCD', '\xA6', '\x8', '\xA4',
  'H', 'c', '\x91', 'B', '\xB', '\xAF', 'V', '\xC1',
  'F', '\xB4', '\x15', '\xCF', '\x22', 'X', '\x98', '\xB',
  'T', 'H', '\x8A', 'd', '\x93', '\x8D', '\xFB', 'F',
  'g', '\xC9', '\x1A', '\x14', '\x7D', '\xF0', 'f', 'v',
  'f', '\xDF', '\x7C', '\xEF', '\xE7', 'g', 'F', '\xA8',
  '\xD5', 'j', 'H', '\x24', '\x12', '\x2A', '\x0', '\x5',
  '\xBF', 'G', '\xD4', '\xEF', '\xF7', '\x2F', '6', '\xEC',
  '\x12', '\x20', '\x1E', '\x8F', '\xD7', '\xAA', '\xD5', '\xEA',
  '\xAF', 'I', '5', 'F', '\xAA', 'T', '\x5F', '\x9F',
  '\x22', 'A', '\x2A', '\x95', '\xA', '\x83', '\xE5', 'r',
  '9', 'd', '\xB3', 'Y', '\x96', '\x99', 'L', '\x6',
  '\xE9', 't', '\x9A', '\x25', '\x85', '\x2C', '\xCB', 'T',
  '\xA7', '\xC4', 'b', '1', '\xB5', '\x5E', '\x0', '\x3',
  'h', '\x9A', '\xC6', '\x16', '\x82', '\x20', 'X', 'R',
  '\x14', 'E', '6', 'S', '\x94', '\xCB', 'e', 'x',
  '\xBD', '\x5E', '\xAA', 'U', 'T', '\x23', 'L', '\xC0',
  '\xE0', '\xE2', '\xC1', '\x8F', '\x0', '\x9E', '\xBC', '\x9',
  'A', '\x7C', '\x3E', '\x1F', '\x83', 'D', '\x22', '\x11',
  '\xD5', 'T', '\x40', '\x3F', '8', '\x80', 'w', '\xE5',
  '3', '\x7', '\xB8', '\x5C', '\x2E', 'H', '\x92', '\x4',
  '\x87', '\xC3', '\x81', '\x40', '\x20', '\x40', 'g', '\x98',
  '\xE9', '6', '\x1A', '\xA6', 'g', '\x15', '\x4', '\xE3',
  '\xD7', '\xC8', '\xBD', '\x15', '\xE1', 'i', '\xB7', 'C',
  '\xAB', '\xEA', 'x', '\x2F', 'j', 'X', '\x92', '\xBB',
  '\x18', '\x20', '\x9F', '\xCF', '3', '\xC3', '\xB8', '\xE9',
  'N', '\xA7', '\xD3', 'l', 'J', '\x0', 'i', '6',
  '\x7C', '\x8E', '\xE1', '\xFE', 'V', '\x84', '\xE7', '\x3C',
  '\x9F', 'r', '\x2B', '\x3A', 'B', '\x7B', '7', 'f',
  'w', '\xAE', '\x8E', '\xE', '\xF3', '\xBD', 'R', '\xA9',
  'd', '\x2', 'B', '\xAF', '\x85', '2', 'f', 'F',
  '\xBA', '\xC', '\xD9', '\x9F', '\x1D', '\x9A', 'l', '\x22',
  '\xE6', '\xC7', '\x3A', '\x2C', '\x80', '\xEF', '\xC1', '\x15',
  '\x90', '\x7', '\x93', '\xA2', '\x28', '\xA0', 'S', 'j',
  '\xB1', '\xB8', '\xDF', '\x29', '5', 'C', '\xE', '\x3F',
  'X', '\xFC', '\x98', '\xDA', 'y', 'j', 'P', '\x40',
  '\x0', '\x87', '\xAE', '\x1B', '\x17', 'B', '\xB4', '\x3A',
  '\x3F', '\xBE', 'y', '\xC7', '\xA', '\x26', '\xB6', '\xEE',
  '\xD9', '\x9A', '\x60', '\x14', '\x93', '\xDB', '\x8F', '\xD',
  '\xA', '\x2E', '\xE9', '\x23', '\x95', '\x29', 'X', '\x0',
  '\x27', '\xEB', 'n', 'V', 'p', '\xBC', '\xD6', '\xCB',
  '\xD6', 'G', '\xAB', '\x3D', 'l', '\x7D', '\xB8', '\xD2',
  '\xDD', '\xA0', '\x60', '\x83', '\xBA', '\xEF', '\x5F', '\xA4',
  '\xEA', '\xCC', '\x2', 'N', '\xAE', '\x5E', 'p', '\x1A',
  '\xEC', '\xB3', '\x40', '9', '\xAC', '\xFE', '\xF2', '\x91',
  '\x89', 'g', '\x91', '\x85', '\x21', '\xA8', '\x87', '\xB7',
  'X', '\x7E', '\x7E', '\x85', '\xBB', '\xCD', 'N', 'N',
  'b', 't', '\x40', '\xFA', '\x93', '\x89', '\xEC', '\x1E',
  '\xEC', '\x86', '\x2', 'H', '\x26', '\x93', '\xD0', 'u',
  '\x1D', '\x7F', '\x9', '2', '\x95', '\xBF', '\x1F', '\xDB',
  '\xD7', 'c', '\x8A', '\x1A', '\xF7', '\x5C', '\xC1', '\xFF',
  '\x22', 'J', '\xC3', '\x87', '\x0', '\x3', '\x0', 'K',
  '\xBB', '\xF8', '\xD6', '\x2A', 'v', '\x98', 'I', '\x0',
  '\x0', '\x0', '\x0', 'I', 'E', 'N', 'D', '\xAE',
  'B', '\x60', '\x82',
};

HttpParse::HttpParse(EventLoop* loop,int fd)
    :loop_(loop),
    channel_(new Channel(loop,fd)),
    fd_(fd),
    error_(false),
    connectionState_(H_CONNECTED),
    method_(METHOD_GET),
    HTTPVersion_(HTTP_11),
    nowReadPos_(0), 
    state_(STATE_PARSE_URI), 
    hState_(H_START),
    keepAlive_(false)
{
    channel_->setReadCallback(std::bind(&HttpParse::handleRead,this));
    channel_->setConnCallback(std::bind(&HttpParse::handleConn,this));
    channel_->setWriteCallback(std::bind(&HttpParse::handleWrite,this));
}

HttpParse::~HttpParse(){
    close(fd_);
    LOG_INFO<<"httpparse destructor";
}

void HttpParse::newEvent(){
    //多线程一定要EPOLLONESHOT
    channel_->set_events(EPOLLIN|EPOLLET|EPOLLONESHOT);
    loop_->addToPoller(channel_);
}

void HttpParse::reset(){
    fileName_.clear();
    path_.clear();
    nowReadPos_ = 0;
    state_ = STATE_PARSE_URI;
    hState_ = H_START;
    headers_.clear();
}

void HttpParse::handleClose(){
    connectionState_ = H_DISCONNECTED;
    shared_ptr<HttpParse> guard(shared_from_this());
    loop_->removeFromPoller(channel_);
}

void HttpParse::handleRead(){
    LOG_INFO<<"into handleread";
    int& events=channel_->get_events();
    do{
       //先把fd中的内容读取到应用层缓冲去inputbuffer_;
       bool Isreadable=false;//当readn中读取到数据把其置true
       int readnum=readn(fd_,inBuffer_,Isreadable);
       LOG_INFO<<inBuffer_<<" ----"<<readnum;
       if(connectionState_==ConnectionState::H_DISCONNECTING){
           inBuffer_.clear();
           LOG_ERROR<<"H_DISCONNECTED";
           break;
       }
       if(readnum<0){
           error_=true;
           //handleError(fd_,400,"Bad request");
           LOG_ERROR<<"111";
           LOG_INFO<<"Bad request";
           break;
       }
       else if(Isreadable){
           //有请求出现，但是没有读取到数据,最有可能的情况是对方已经关闭了
           LOG_INFO<<"request is here , but no data";
           connectionState_=ConnectionState::H_DISCONNECTING;
           if(!readnum){
               break;
           }
       }
       
       //开始解析HTTP请求报文的请求行
       if(state_==ProcessState::STATE_PARSE_URI){
           LOG_INFO<<"into parseuri";
           URIState flag=parseURI();
           if(flag==URIState::PARSE_URI_AGAIN){
               break;
           }
           else if(flag==URIState::PARSE_URI_ERROR){
               error_=true;
               //handleError(fd_,400,"Bad request");
               LOG_ERROR<<"222";
               break;
           }
           else{
               state_=ProcessState::STATE_PARSE_HEADERS;
           }
       }
       //解析HTTP请求报文的头部
       if(state_==ProcessState::STATE_PARSE_HEADERS){
           LOG_INFO<<"into parseheader";
           HeaderState flag=parseHeaders();
           if(flag==HeaderState::PARSE_HEADER_AGAIN){
               break;
           }
           else if(flag==HeaderState::PARSE_HEADER_ERROR){
               error_=true;
               //handleError(fd_,400,"Bad request");
               LOG_ERROR<<"333";
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
                //handleError(fd_, 400, "Bad Request: Lack of argument (Content-length)");
                LOG_ERROR<<"444";
                break;
           }
           if (static_cast<int>(inBuffer_.size()) < content_length)
                break;
           state_ = STATE_ANALYSIS;
       }
       //解析HTTP
        if(state_==ProcessState::STATE_ANALYSIS){
            LOG_INFO<<"into analysisrequest";
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
        LOG_INFO<<"into no error";
        if(outBuffer_.size()>0){
            handleWrite();
        }
        if(!error_ && state_==ProcessState::STATE_FINISH){
            LOG_INFO<<"into reset";
            reset();
            if(inBuffer_.size()>0){
                if(connectionState_!=ConnectionState::H_DISCONNECTING){
                    handleRead();
                }
            }
        }
        else if(!error_ && connectionState_!=ConnectionState::H_DISCONNECTED){
            LOG_INFO<<"into H_DISCONNECTING";
            events|=EPOLLIN;
        }
        //struct epoll_event event;
        //event.data.fd=readfd;
        //event.events=EPOLLOUT;
        //server_->setepollmod(readfd,event);
        //epoll_->epoll_mod(readfd,event);
    }
}
    
void HttpParse::handleWrite(){
    LOG_INFO<<"into handlewrite";
    if(!error_ && connectionState_!=H_DISCONNECTED){
        int& event=channel_->get_events();
        if(writen(fd_,outBuffer_,outBuffer_.size())<0){
            perror("write error");
            event=0;
            error_=true;
        }
        if(outBuffer_.size()>0){
            event|=EPOLLOUT;
        }
    }
}

void HttpParse::handleConn(){
    LOG_INFO<<"into handleconn";
    int &events_ = channel_->get_events();
    if (!error_ && connectionState_ == H_CONNECTED)
    {
        if (events_ != 0)
        {
            if ((events_ & EPOLLIN) && (events_ & EPOLLOUT))
            {
                events_ = int(0);
                events_ |= EPOLLOUT;
            }
            //events_ |= (EPOLLET | EPOLLONESHOT);
            events_ |= EPOLLET;
            loop_->updatePoller(channel_);

        }
        else
        {
            //LOG_INFO<< "close normally";
            // loop_->shutdown(channel_);
            // loop_->runInLoop(bind(&HttpData::handleClose, shared_from_this()));
            events_ |= (EPOLLIN | EPOLLET);
            LOG_INFO<<"right in ";
            //events_ |= (EPOLLIN | EPOLLET | EPOLLONESHOT);
            //int timeout = (DEFAULT_KEEP_ALIVE_TIME >> 1);
            loop_->updatePoller(channel_);
        }
    }
    else if (!error_ && connectionState_ == H_DISCONNECTING && (events_ & EPOLLOUT))
    {
        events_ = (EPOLLOUT | EPOLLET);
    }
    else
    {
        LOG_INFO << "close with errors";
        loop_->runInLoop(std::bind(&HttpParse::handleClose, shared_from_this()));
    }
}

void HttpParse::handleError(int fd, int err_num, std::string short_msg){
    LOG_ERROR<<"into handle error";
}
    
URIState HttpParse::parseURI(){
    string &str = inBuffer_;
    string cop = str;
    // 读到完整的请求行再开始解析请求
    size_t pos = str.find('\r', nowReadPos_);
    if (pos < 0)
    {
        return PARSE_URI_AGAIN;
    }
    // 去掉请求行所占的空间，节省空间
    string request_line = str.substr(0, pos);
    if (str.size() > pos + 1)
        str = str.substr(pos + 1);
    else 
        str.clear();
    // Method
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
        LOG_ERROR<<"5555";
        return PARSE_URI_ERROR;
    }

    // filename
    pos = request_line.find("/", pos);
    if (pos < 0)
    {
        fileName_ = "index.html";
        HTTPVersion_ = HTTP_11;
        return PARSE_URI_SUCCESS;
    }
    else
    {
        size_t _pos = request_line.find(' ', pos);
        if (_pos < 0)
            return PARSE_URI_ERROR;
        else
        {
            if (_pos - pos > 1)
            {
                fileName_ = request_line.substr(pos + 1, _pos - pos - 1);
                size_t __pos = fileName_.find('?');
                if (__pos >= 0)
                {
                    fileName_ = fileName_.substr(0, __pos);
                }
            }
                
            else
                fileName_ = "index.html";
        }
        pos = _pos;
    }
    //cout << "fileName_: " << fileName_ << endl;
    // HTTP 版本号
    pos = request_line.find("/", pos);
    if (pos < 0)
        return PARSE_URI_ERROR;
    else
    {
        if (request_line.size() - pos <= 3)
            return PARSE_URI_ERROR;
        else
        {
            string ver = request_line.substr(pos + 1, 3);
            if (ver == "1.0")
                HTTPVersion_ = HTTP_10;
            else if (ver == "1.1")
                HTTPVersion_ = HTTP_11;
            else
                return PARSE_URI_ERROR;
        }
    }
    return PARSE_URI_SUCCESS;
}
    
HeaderState HttpParse::parseHeaders(){
    string &str = inBuffer_;
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
                    string key(str.begin() + key_start, str.begin() + key_end);
                    string value(str.begin() + value_start, str.begin() + value_end);
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
    
AnalysisState HttpParse::analysisRequest(){
    if (method_ == METHOD_POST)
    {
        LOG_INFO<<"into method_post";
    }
    else if (method_ == METHOD_GET || method_ == METHOD_HEAD)
    {
        LOG_INFO<<"into method_get";
        string header;
        header += "HTTP/1.1 200 OK\r\n";
        // if(headers_.find("Connection") != headers_.end() && (headers_["Connection"] == "Keep-Alive" || headers_["Connection"] == "keep-alive"))
        // {
        //     keepAlive_ = true;
        //     header += string("Connection: Keep-Alive\r\n") + "Keep-Alive: timeout=" + to_string(DEFAULT_KEEP_ALIVE_TIME) + "\r\n";
        // }
        int dot_pos = fileName_.find('.');
        string filetype;
        // if (dot_pos < 0) 
        //     filetype = MimeType::getMime("default");
        // else
        //     filetype = MimeType::getMime(fileName_.substr(dot_pos));

        // echo test
        if (fileName_ == "hello")
        {
            outBuffer_ = "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\n\r\nHello World";
            return ANALYSIS_SUCCESS;
        }
        if (fileName_ == "favicon.ico")
        {
            header += "Content-Type: image/png\r\n";
            header += "Content-Length: " + to_string(sizeof favicon) + "\r\n";
            header += "Server: LinYa's Web Server\r\n";

            header += "\r\n";
            outBuffer_ += header;
            outBuffer_ += string(favicon, favicon + sizeof favicon);;
            return ANALYSIS_SUCCESS;
        }
        LOG_INFO<<"file name is "<<fileName_;
        struct stat sbuf;
        if (stat(fileName_.c_str(), &sbuf) < 0)
        {
            header.clear();
            handleError(fd_, 404, "Not Found!");
            return ANALYSIS_ERROR;
        }
        header += "Content-Type: " + filetype + "\r\n";
        header += "Content-Length: " + to_string(sbuf.st_size) + "\r\n";
        header += "Server: hahagiraffe's Web Server\r\n";
        // 头部结束
        header += "\r\n";
        outBuffer_ += header;
        
        if (method_ == METHOD_HEAD)
            return ANALYSIS_SUCCESS;

        int src_fd = open(fileName_.c_str(), O_RDONLY, 0);
        if (src_fd < 0)
        {
          outBuffer_.clear();
          handleError(fd_, 404, "Not Found!");
          return ANALYSIS_ERROR;
        }
		void *mmapRet = mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
		close(src_fd);
		if (mmapRet == (void *)-1)
		{
			munmap(mmapRet, sbuf.st_size);
			outBuffer_.clear();
			handleError(fd_, 404, "Not Found!");
			return ANALYSIS_ERROR;
		}
        char *src_addr = static_cast<char*>(mmapRet);
        outBuffer_ += string(src_addr, src_addr + sbuf.st_size);;
        munmap(mmapRet, sbuf.st_size);
        return ANALYSIS_SUCCESS;
    }
    return ANALYSIS_ERROR;
}