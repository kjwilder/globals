#CC = CC
CC = g++ -Wall -std=c++11

GLOBALS_HEADERS = globals.h globlist_example.h
GLOBALS_SOURCES = globals.cc
HEADERS =
SOURCES = main.cc
OBJECTS = $(SOURCES:.cc=.o)
LDLIBS =

program : globlist.h $(HEADERS) $(SOURCES) $(GLOBALS_HEADERS) $(GLOBALS_SOURCES)
	$(CC) $(SOURCES) $(GLOBALS_SOURCES) -o program $(LDLIBS)

globlist.h : globlist_example.h
	cp $< $@

test : test/globals_test
	./test/globals_test

test/globals_test : test/globals_test.cc $(GLOBALS_SOURCES) globals.h test/globlist.h
	rm -f globlist.h
	$(CC) -I test -I . test/globals_test.cc $(GLOBALS_SOURCES) -o test/globals_test

clean :
	rm -f *.o program globlist.h test/globals_test

