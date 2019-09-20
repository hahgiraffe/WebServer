/*
 * @Description: 
 * @Author: hahagiraffe
 * @Date: 2019-08-16 13:49:08
 */
#include "HttpData.h"
#include <functional>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
 #include <sys/stat.h>
 #include <sys/mman.h>
#include "../src/Log/Logging.h"
#include "HttpServer.h"
#include "IOutil.h"
#include "Epoll.h"
using namespace haha_giraffe;

//表示图标
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
    server_->setreadhandle(std::bind(&HttpData::handleRead,this,std::placeholders::_1));
    server_->setwritehandle(std::bind(&HttpData::handleWrite,this,std::placeholders::_1));
    server_->setconnectionhandle(std::bind(&HttpData::handleConn,this,std::placeholders::_1));
}

//对象重置
void HttpData::reset(){
    filename_.clear();
    path_.clear();
    nowreadpos_ = 0;
    state_ = STATE_PARSE_URI;
    hState_ = H_START;
    headers_.clear();
}

//读事件
void HttpData::handleRead(int readfd){
    do{
       //先把fd中的内容读取到应用层缓冲去inputbuffer_;
       bool Isreadable=false;//当readn中读取到数据把其置true
       int readnum=readbufferfunc(readfd,inputbuffer_,Isreadable);
       LOG_INFO<<inputbuffer_<<" ----"<<readnum;
       if(connectionstate_==ConnectionState::H_DISCONNECTED){
           inputbuffer_.clear();
           LOG_ERROR<<"H_DISCONNECTED";
           break;
       }
       if(readnum<0){
           error_=true;
           handleError(readfd,400,"Bad request");
           LOG_ERROR<<"Bad request";
           break;
       }
       else if(Isreadable){
           //有请求出现，但是没有读取到数据,最有可能的情况是对方已经关闭了
           LOG_INFO<<"request is here , but no data";
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
               handleError(readfd,400,"Bad request");
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
               handleError(readfd,400,"Bad request");
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
                handleError(readfd, 400, "Bad Request: Lack of argument (Content-length)");
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
        LOG_INFO<<"into no error";
        if(outputbuffer_.size()>0){
            handleWrite(readfd);
        }
        if(!error_ && state_==ProcessState::STATE_FINISH){
            LOG_INFO<<"into reset";
            reset();
            if(inputbuffer_.size()>0){
                if(connectionstate_!=ConnectionState::H_DISCONNECTING){
                    handleRead(readfd);
                }
            }
        }
        else if(!error_ && connectionstate_!=ConnectionState::H_DISCONNECTED){
            LOG_INFO<<"into H_DISCONNECTING";
            struct epoll_event event;
            event.data.fd=readfd;
            event.events=EPOLLOUT |EPOLLET;
            server_->setepollmod(readfd,event);
        }
        //struct epoll_event event;
        //event.data.fd=readfd;
        //event.events=EPOLLOUT;
        //server_->setepollmod(readfd,event);
        //epoll_->epoll_mod(readfd,event);
    }
}
    
//写事件
void HttpData::handleWrite(int eventfd){
    LOG_INFO<<"into handlewrite";
    if (!error_ && connectionstate_ != H_DISCONNECTED)
    {
        //epoll_->epoll_mod(eventfd,event);
        //__uint32_t &events_ = channel_->getEvents();
        if (writebufferfunc(eventfd, outputbuffer_,outputbuffer_.size()) < 0)
        {
            perror("writen");
            //events_ = 0;
            error_ = true;
        }
        if (outputbuffer_.size() > 0){
            struct epoll_event event;
            event.data.fd=eventfd;
            event.events=EPOLLOUT |EPOLLET;
            server_->setepollmod(eventfd,event);
        }
        
    }
}
    
//连接事件
void HttpData::handleConn(__uint32_t* event){
    //__uint32_t &events_ = channel_->getEvents();
    if (!error_ && connectionstate_ == H_CONNECTED)
    {
        if (*event != 0)
        {
            if ((*event & EPOLLIN) && (*event & EPOLLOUT))
            {
                *event = __uint32_t(0);
                *event |= EPOLLOUT;
            }
            //events_ |= (EPOLLET | EPOLLONESHOT);
            *event |= EPOLLET;
            //loop_->updatePoller(channel_, timeout);

        }
        else
        {
            //LOG_INFO<< "close normally";
            // loop_->shutdown(channel_);
            // loop_->runInLoop(bind(&HttpData::handleClose, shared_from_this()));
            *event |= (EPOLLIN | EPOLLET);
            LOG_INFO<<"right in ";
            //events_ |= (EPOLLIN | EPOLLET | EPOLLONESHOT);
            //int timeout = (DEFAULT_KEEP_ALIVE_TIME >> 1);
            //loop_->updatePoller(channel_, timeout);
        }
    }
    else if (!error_ && connectionstate_ == H_DISCONNECTING && (*event & EPOLLOUT))
    {
        *event = (EPOLLOUT | EPOLLET);
    }
    else
    {
        LOG_INFO << "close with errors";
        //loop_->runInLoop(bind(&HttpData::handleClose, shared_from_this()));
    }
}

//错误处理事件
void HttpData::handleError(int fd,int errornum,std::string errormessage){
    errormessage = " " + errormessage;
    char send_buff[4096];
    std::string body_buff, header_buff;
    body_buff += "<html><title>哎~出错了</title>";
    body_buff += "<body bgcolor=\"ffffff\">";
    body_buff += std::to_string(errornum) + errormessage;
    body_buff += "<hr><em> haha_giraffe's Web Server</em>\n</body></html>";

    header_buff += "HTTP/1.1 " + std::to_string(errornum) + errormessage + "\r\n";
    header_buff += "Content-Type: text/html\r\n";
    header_buff += "Connection: Close\r\n";
    header_buff += "Content-Length: " + std::to_string(body_buff.size()) + "\r\n";
    header_buff += "Server: hahagiraffe's Web Server\r\n";;
    header_buff += "\r\n";
    // 错误处理不考虑writen不完的情况
    sprintf(send_buff, "%s", header_buff.c_str());
    std::string bodystr(send_buff);
    writebufferfunc(fd, bodystr, bodystr.size());

    memset(send_buff,0,sizeof(send_buff)/sizeof(char));
    sprintf(send_buff, "%s", body_buff.c_str());
    std::string headerstr(send_buff);
    writebufferfunc(fd, headerstr, headerstr.size());
}

//解析请求行URI
URIState HttpData::parseURI(){
    LOG_INFO<<"parseURI";
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
    LOG_INFO<<filename_;
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
    
//解析头部header
HeaderState HttpData::parseHeaders(){
    LOG_INFO<<"parseHeader";
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
    
//根据解析到的request，返回相应的文件页面
AnalysisState HttpData::analysisRequest(){
    if (method_ == METHOD_POST)
    {
        char buf2[1024];
        snprintf(buf2,sizeof(buf2),"HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\nHello World\n", 12);
        outputbuffer_=std::string(buf2);
        return AnalysisState::ANALYSIS_SUCCESS;
    }
    else if (method_ == METHOD_GET || method_ == METHOD_HEAD)
    {
        LOG_INFO<<"come into METHOD_GET";
        //这里根据filename找到文件放到outputbuffer里面
        //char buf2[1024];
        //snprintf(buf2,sizeof(buf2),"HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\nHello World222\n", 12);
        //outputbuffer_=std::string(buf2);
        std::string head="HTTP/1.1 200 OK\r\n";
        if(filename_=="echo"){ //测试echo
            char buf2[1024];
            snprintf(buf2,sizeof(buf2),"HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\necho_test\n", 10);
            outputbuffer_=std::string(buf2);
            return ANALYSIS_SUCCESS;
        }
        else if (filename_ == "favicon.ico")//每个网站的图标
        {
            head += "Content-Type: image/png\r\n";
            head += "Content-Length: " + std::to_string(sizeof(favicon)) + "\r\n";
            head += "Server: haha_giraffe's Web Server\r\n";
            head += "\r\n";
            outputbuffer_ += head;
            outputbuffer_ += std::string(favicon, favicon + sizeof(favicon));;
            return ANALYSIS_SUCCESS;
        }
        else{
            struct stat sbuf;
            LOG_INFO<<"Into Method_GET "<<filename_;
            //通过文件名filename获取文件信息，并保存在buf所指的结构体stat中
            if (stat(filename_.c_str(),&sbuf) < 0)
            {
                head.clear();
                LOG_ERROR<<"NOT_FOUND";
                handleError(fd_, 404, "Not Found!");
                return ANALYSIS_ERROR;
            }
            head += "Content-Type: text \r\n";
            head += "Content-Length: " + std::to_string(sbuf.st_size) + "\r\n";
            head += "Server: haha_giraffe's Web Server\r\n";
            head += "\r\n";
            LOG_INFO<<head;
            outputbuffer_+=head;
            //共享内存映射实现读取文件
            int fd=open(filename_.c_str(),O_RDONLY,0);
            if (fd < 0)
            {
                outputbuffer_.clear();
                handleError(fd_, 404, "Not Found!");
                return ANALYSIS_ERROR;
            }
            void *mmapRet = mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
		    close(fd);
		    if (mmapRet == (void *)-1)
		    {
			    munmap(mmapRet, sbuf.st_size);
			    outputbuffer_.clear();
			    handleError(fd_, 404, "Not Found!");
			    return ANALYSIS_ERROR;
            }
            char *src_addr = static_cast<char*>(mmapRet);
            outputbuffer_ += std::string(src_addr, src_addr + sbuf.st_size);;
            munmap(mmapRet, sbuf.st_size);
            LOG_INFO<<outputbuffer_;
        }
        
        return AnalysisState::ANALYSIS_SUCCESS;
    }
    return ANALYSIS_ERROR;
}