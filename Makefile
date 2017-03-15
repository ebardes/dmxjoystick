UNAME_S := $(shell uname -s)

CXXFLAGS=-g -MD -std=c++11
CFLAGS=-g -MD -std=c11

OBJS=js.o eth.o config.o main.o timer.o display.o

LIBS= -lboost_system -lboost_thread 

ifeq ($(UNAME_S),SunOS)
LIBS += -lcurses -lsocket
CXXFLAGS += -pthreads
endif
ifeq ($(UNAME_S),Linux)
LIBS += -lncurses
endif

js: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

eth.o: acnraw.h

acnraw.h: genframe
	./genframe

clean:
	rm -f $(OBJS) $(OBJS:.o=.d) genframe genframe.o genframe.d acnraw.h js

-include *.d

test: js
	./js -x
