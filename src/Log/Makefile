#makefile for AsyncLog
LIB = -g -lpthread -std=c++11
cc = g++
prom = main
source = Logtest.cc LogFile.cc Logging.cc Fileutil.cc AsyncStream.cc AsyncLog.cc 

${prom} : ${source}
	${cc} -o ${prom} ${source} ${LIB} 

clean :
	rm Logtest.o LogFile.o Logging.o Fileutil.o AsyncStream.o AsyncLog.o 