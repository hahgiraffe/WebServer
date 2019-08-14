/*
 * @Description: 工具头文件
 * @Author: hahagiraffe
 * @Date: 2019-08-13 17:33:41
 */
#ifndef UTIL_H
#define UTIL_H

//创建监听文件描述符
int createlistenfd(int port);
//设置描述符为非阻塞
void setlistenfdnonblock(int fd);

int readn(int fd,void* buf,int size);
int writen(int fd,void* buf,int size);
#endif
