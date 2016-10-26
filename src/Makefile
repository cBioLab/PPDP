TOPDIR:=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))/
CFLAGS= -O3 -g
CFLAGS+= -I$(TOPDIR)include -I$(TOPDIR)../../cybozulib/include/ -I$(TOPDIR)../../mcl/include -I$(TOPDIR)../../xbyak/ -I$(TOPDIR)../../PBWT-sec/include -I$(TOPDIR)../include/
LDFLAGS+= -lgmp -lgmpxx -lcrypto -lmcl -L../../mcl/lib/

ROT_OBJ=../../PBWT-sec/src/rot.o
COMM_OBJ=../../PBWT-sec/src/comm.o
SERVER_OBJS=$(ROT_OBJ) ppdpp.o $(COMM_OBJ) server.o
CLIENT_OBJS=$(ROT_OBJ) ppdpp.o $(COMM_OBJ) client.o
PPDPP_OBJS= $(ROT_OBJ) ppdpp.o ppdpp_main.o
MCL_LIB=../../mcl/lib/libmcl.a

all: pbwt_sec ppdpp_main

pbwt_sec:
	make -C ../../PBWT-sec/src/

clean:
	rm -rf ../bin/ppdpp_main $(PPDPP_OBJS) ../bin/server $(SERVER_OBJS) ../bin/client  $(CLIENT_OBJS) *~

.SUFFIXES: .cpp

.cpp.o:
	$(CXX) -c $< -o $@ $(CFLAGS) -fopenmp

ppdpp_main: $(PPDPP_OBJS) $(MCL_LIB)
	mkdir -p ../bin/
	$(CXX) $(PPDPP_OBJS) -o $@ $(LDFLAGS) -fopenmp
	mv ppdpp_main $(TOPDIR)../bin/

client: $(CLIENT_OBJS) $(MCL_LIB) 
	$(CXX) $(CLIENT_OBJS) -o $@ $(LDFLAGS) -fopenmp
	mv $@ $(TOPDIR)../bin/
	mkdir -p ../comm/
	mkdir -p ../comm/client

server: $(SERVER_OBJS) $(MCL_LIB) 
	$(CXX) $(SERVER_OBJS) -o $@ $(LDFLAGS) -fopenmp
	mv $@ $(TOPDIR)../bin/
	mkdir -p ../comm/
	mkdir -p ../comm/server
