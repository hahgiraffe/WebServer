/*
 * @Description:用于ET模式下的IO
 * @Author: hahagiraffe
 * @Date: 2019-07-22 15:38:08
 */
#ifndef IOUTIL_H
#define IOUTIL_H
#include <stdio.h>
#include <string>
#include <unistd.h>
#include "../src/Log/Logging.h"
using namespace haha_giraffe;

//非阻塞读
int readbuffer(int fd,std::string& inputbuffer,bool Isreadable){
    ssize_t nread = 0;
    ssize_t readSum = 0;
    while (true)
    {
        char buff[1024];
        if ((nread = read(fd, buff, 1024)) < 0)
        {
            if (errno == EINTR)
                continue;
            else if (errno == EAGAIN)
            {
                return readSum;
            }  
            else
            {
                LOG_ERROR<<"read error";
                return -1;
            }
        }
        else if (nread == 0)
        {
            
            Isreadable = true;
            break;
        }
        readSum += nread;
        inputbuffer += std::string(buff, buff + nread);
    }
    return readSum;
}

//非阻塞写
int writebuffer(int fd, std::string& outputbuffer,int len){
    int writebyte=0;
    int writesum=0;
    int n=len;
    const char *ptr=outputbuffer.c_str();
    while(n>0){
        writebyte=::write(fd,ptr,n);
        if(writebyte<0){
            LOG_ERROR<<"write error";
            break;
        }
        n-=writebyte;
        ptr+=writebyte;
        writesum+=writebyte;
    }
    if (writesum == static_cast<int>(outputbuffer.size()))
        outputbuffer.clear();
    else
        outputbuffer = outputbuffer.substr(writesum);
    return writesum;
}

#endif 