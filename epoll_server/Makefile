#makefile for epoll_server
LIB = -g -lpthread -std=c++11
cc = g++
prom = test
Logdir = ../src/Log/
source = Main.cc Epoll.cc HttpServer.cc HttpData.cc IOutil.cc ${Logdir}LogFile.cc ${Logdir}Logging.cc ${Logdir}Fileutil.cc ${Logdir}AsyncStream.cc ${Logdir}AsyncLog.cc 

${prom} : ${source}
	${cc} -o ${prom} ${source} ${LIB} 

clean :
	rm Mian.o Epoll.o HttpServer.o HttpData.o IOutil.o ${Logdir}LogFile.o ${Logdir}Logging.o ${Logdir}Fileutil.o ${Logdir}AsyncStream.o ${Logdir}AsyncLog.o 