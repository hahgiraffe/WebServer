#include "Address.h"
#include "Epoll.h"
#include "HttpServer.h"

int main(){
    HttpServer server(1234);
    server.start();
}