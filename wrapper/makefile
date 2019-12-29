
SERVERDIR	= /home/lb/server

SRCS		= mysqlwrapper.cpp
OBJS		= $(SRCS:.cpp=.o)
LIB			= libmysql.a
CXX			= g++
CXXFLAGS	= -Wall -O -g -std=c++11 -I$(SERVERDIR)/include/libevent -I$(SERVERDIR)/include/mysql -L$(SERVERDIR)/libs -lmysqlclient

$(LIB): $(OBJS)
	$(AR) cr $(LIB) $(OBJS)
	
.PHONY: clean
clean:
	rm -rf $(OBJS) $(LIB)
