

### for message use ###

SERVERDIR	= /home/lb/server

SRCS		= message.cpp
OBJS		= $(SRCS:.cpp=.o)
LIB			= libmsg.a
CXX			= g++
CXXFLAGS	= -Wall -O -g -std=c++11 -I$(SERVERDIR)/include/jsoncpp -I$(SERVERDIR)/include/libevent -I$(SERVERDIR)/include -L$(SERVERDIR)/libs -ljsoncpp -levent

$(LIB): $(OBJS)
	$(AR) cr $(LIB) $(OBJS)
	
.PHONY: clean
clean:
	rm -rf $(OBJS) $(LIB)
