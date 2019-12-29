
### for dataparse use ###

SERVERDIR	= /home/lb/server
JSONCPP64 	= ../include/jsoncpp

SRCS		= dataparse.cpp JSONCPP64/json_reader.cpp JSONCPP64/json_value.cpp JSONCPP64/json_writer.cpp
OBJS		= $(SRCS:.cpp=.o)
LIB			= libdata.a
CXX			= g++
CXXFLAGS	= -Wall -O -g -std=c++11 -I$(SERVERDIR)/include/jsoncpp -I$(SERVERDIR)/include/libevent -I$(SERVERDIR)/include -L$(SERVERDIR)/libs -ljsoncpp

$(LIB): $(OBJS)
	$(AR) cr $(LIB) $(OBJS)
	
.PHONY: clean
clean:
	rm -rf $(OBJS) $(LIB)
