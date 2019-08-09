#include "HttpData.h"
#include <functional>
#include "HttpServer.h"
int main(){
    //HttpServer server(1233);
    //server.start();
    HttpData data(1234);
    std::shared_ptr<HttpServer> server=data.getserver();
    server->start();
}