#ifndef ADDRESS_H
#define ADDRESS_H
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include "../src/Log/Logging.h"
using namespace haha_giraffe;

class Address{
public:
    explicit Address(int port){
        bzero(&addr_,sizeof(addr_));
        addr_.sin_family=AF_INET;
        addr_.sin_port=htons(port);
        addr_.sin_addr.s_addr=htonl(INADDR_ANY);
    }
    Address(const std::string& ip,int port){
        bzero(&addr_,sizeof(addr_));
        addr_.sin_family=AF_INET;
        addr_.sin_port=htons(port);
        if (inet_pton(AF_INET,ip.c_str(),&addr_.sin_addr) <= 0)
        {
            LOG_ERROR << "Address::Address";
        }
    }
    Address(const struct sockaddr_in& addr):addr_(addr){ }
    const sockaddr_in getaddress() const { return addr_;}
    void setaddress(const struct sockaddr_in& addr) { addr_ = addr; }
private:
    sockaddr_in addr_;
};
#endif