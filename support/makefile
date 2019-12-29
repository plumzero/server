
SERVERDIR	= /home/lb/server

SRCS		= timer.cpp tserver.cpp
OBJS		= $(SRCS:.cpp=.o)
LIB			= libsupport.a
CXX			= g++
CXXFLAGS	= -Wall -O -g -std=c++11 -I$(SERVERDIR)/include/jsoncpp -I$(SERVERDIR)/include/libevent -I$(SERVERDIR)/include -L$(SERVERDIR)/libs -levent

$(LIB): $(OBJS)
	$(AR) cr $(LIB) $(OBJS)
	
.PHONY: clean
clean:
	rm -rf $(OBJS) $(LIB)
