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
int readbufferfunc(int fd,std::string&,bool& );
int writebufferfunc(int fd, std::string&,int );
#endif 