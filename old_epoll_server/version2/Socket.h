/*
 * @Description:对套接字的操作bind,listen,accept.等属性操作
 * @Author: haha_giraffe
 * @Date: 2019-07-15 20:03:12
 */

#ifndef SOCKET_H
#define SOCKET_H
#include <string>
using namespace std;
const int MAX_BUFF=1024;
//创建监听文件描述符
int createlistenfd(int port);
//设置描述符为非阻塞
void setlistenfdnonblock(int fd);
//设置为非延时，拒绝nagle
void setnodelay(int fd);
int readn(int fd,void* buf,int size);
ssize_t readn(int fd, string &inBuffer, bool &zero);
int writen(int fd,string& buf,int size);
int writen(int fd,void* buf,int size);
void handle_for_sigpipe();
#endif