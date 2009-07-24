#
# Makefile for libcsclient
#

PROG		= cscapp
LIB			= csc

BASE_DIR	= ../..
BIN_DIR		= $(BASE_DIR)/bin
LIB_DIR		= $(BASE_DIR)/lib
PROG_FILE	= $(BIN_DIR)/$(PROG)
LIB_FILE	= $(LIB_DIR)/lib$(LIB).a

# Sources
SRCS		= CSApp.cpp
LIB_SRCS	= lib/Client.cpp lib/ChunkSource.cpp lib/ChunkDestination.cpp lib/Image.cpp lib/ImageDirectory.cpp lib/Mask.cpp lib/Stat.cpp lib/Program.cpp

# Objects
OBJS		= $(SRCS:.cpp=.o)
LIB_OBJS	= $(LIB_SRCS:.cpp=.o)

# Include paths
INCLUDE = -I/usr/local/include/Sockets -I/usr/include/GraphicsMagick -Ilib

# Libraries
LIBS  = -lssl -lcrypto -lpthread
LIBS += -L/usr/local/lib -lSockets
LIBS += -L$(LIB_DIR) -l$(LIB)
LIBS += `GraphicsMagick++-config --libs`

# Compiler options
CXX			= g++
CFLAGS		= -Wall -g -O2
CPPFLAGS	= $(CFLAGS)

# Targets
all:			$(PROG_FILE)

$(PROG_FILE):	$(LIB_FILE) $(OBJS)
#		ld -o $@ $(OBJS) $(LIBS)
		$(CXX) $(CPPFLAGS) -o $@ $(OBJS) $(LIBS)

$(LIB_FILE):	$(LIB_OBJS)
		ar cru $@ $(LIB_OBJS)
		ranlib $@

%.o:			%.cpp
		$(CXX) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

#$(LIB_OBJS):	$(LIB_SRCS)
#		$(CXX) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

clean:			clean-lib
		rm -f $(OBJS) *~ *.d $(PROG_FILE)

clean-lib:
		rm -f $(LIB_OBJS) $(LIB_FILE)
