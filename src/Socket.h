/*
 * @Description:  sockfd的封装类，封装了对套接字的操作bind,listen,accept等
 * @Author: haha_giraffe
 * @Date: 2019-07-15 20:03:12
 */

#ifndef SOCKET_H
#define SOCKET_H
class Address;
class Socket{
public:
    explicit Socket(int sockfd):sockfd_(sockfd){ }
    ~Socket();
    int fd() const { return sockfd_; }
    void setReuseAddr(bool check=true);
    void bindAddress(const Address& addr);
    void listen();
    int accept(const Address& addr);

    void shutdownWrite();
    void setTcpNoDelay(bool on);//禁止Nagle算法，避免发包出现延迟
private:
    const int sockfd_;
};

#endif