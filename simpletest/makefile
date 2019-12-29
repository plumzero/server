
#################### for simple test use ####################

SERVERDIR	= /home/lb/server

SRCS		= test_start.cpp test_dataparse.cpp test_client.cpp
PROG		= test
LIBS		= ../message/libmsg.a ../manager/libmanager.a ../support/libsupport.a ../dataparse/libdata.a ../wrapper/libmysql.a
CXX			= g++
CXXFLAGS	= -Wall -O -g -std=c++11 -I$(SERVERDIR)/include/jsoncpp -I$(SERVERDIR)/include/libevent -I$(SERVERDIR)/include -L$(SERVERDIR)/libs -ljsoncpp -levent -lmysqlclient
OBJS		= $(SRCS:.cpp=.o)

$(PROG): $(OBJS) $(LIBS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	
.PHONY: clean
clean:
	rm -rf $(OBJS) $(PROG)
