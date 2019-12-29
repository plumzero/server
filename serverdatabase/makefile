
#################### for databaseserver use ####################

SERVERDIR	= /home/lb/server

SRCS		= start.cpp databaseserver.cpp
PROG		= database
LIBS		= ../support/libsupport.a ../manager/libmanager.a ../message/libmsg.a ../dataparse/libdata.a ../wrapper/libmysql.a
CXX			= g++
CXXFLAGS	= -Wall -O -g -std=c++11 -I$(SERVERDIR)/include/jsoncpp -I$(SERVERDIR)/include/libevent -I$(SERVERDIR)/include -L$(SERVERDIR)/libs -ljsoncpp -levent -lmysqlclient
OBJS		= $(SRCS:.cpp=.o)

$(PROG): $(OBJS) $(LIBS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	
.PHONY: clean
clean:
	rm -rf $(OBJS) $(PROG)
