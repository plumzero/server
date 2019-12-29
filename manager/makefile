
SERVERDIR	= /home/lb/server

SRCS		= servermanager.cpp mysqlmanager.cpp
OBJS		= $(SRCS:.cpp=.o)
LIB			= libmanager.a
CXX			= g++
CXXFLAGS	= -Wall -O -g -std=c++11 -I$(SERVERDIR)/include/libevent -I$(SERVERDIR)/include/jsoncpp -I$(SERVERDIR)/include -L$(SERVERDIR)/libs -levent -lpthread -lmysqlclient

$(LIB): $(OBJS)
	$(AR) cr $(LIB) $(OBJS)
	
.PHONY: clean
clean:
	rm -rf $(OBJS) $(LIB)
