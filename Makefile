#CC = CC
CC = g++ -Wall -std=c++11

GLOBALS_HEADERS = globals.h globlist.h 
GLOBALS_SOURCES = globals.cc
HEADERS = 
SOURCES = main.cc
OBJECTS = $(SOURCES:.cc=.o)
LDLIBS = 

program : $(HEADERS) $(SOURCES) $(GLOBALS_HEADERS) $(GLOBALS_SOURCES) 
	$(CC) $(SOURCES) $(GLOBALS_SOURCES) -o program $(LDLIBS)

clean :
	rm -f *.o program

