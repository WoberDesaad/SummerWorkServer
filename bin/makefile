#This file builds and compiles the Server for the Summer Workers
#--Robert Appelmann

# 'make depend' uses makedepend to automatically generate dependencies 
# 'make clean'  removes all .o and .~ files

# define the C compiler to use
CC = gcc

# define any compile-time flags
CFLAGS = -Wall -g

# define any directories containing header files other than /usr/include
INCLUDES = -I../includes

# define library paths in addition to /usr/lib
LFLAGS = 

# define any libraries to link into executable:
LIBS = -lpthread

# define the C source files
SRCS = Main.c HandleClientActor.c BlockingQueue.c LogActor.c

#path
VPATH = ../src/

#define the C object files 
OBJS = $(SRCS:.c=.o)

# define the executable file 
MAIN = Serv

.PHONY: depend clean

all: $(MAIN)
	@echo  Work Server named Serv has been Compiled

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)
	$(RM) *.o *~

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@
	
clean:
	$(RM) *.txt *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^
	
# DO NOT DELETE THIS LINE -- make depend needs it
