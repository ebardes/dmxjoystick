UNAME_S := $(shell uname -s)

OPTIMIZE=-O3 -march=native -fno-strict-aliasing
CXXFLAGS=-g ${OPTIMIZE} -MMD -std=c++11 -Wall
CFLAGS=-g -MMD -std=c11 -Wall

OBJS=js.o eth.o config.o main.o timer.o display.o bres.o

LIBS= -lboost_system -lboost_thread

ifeq ($(UNAME_S),SunOS)
LIBS += -lcurses -lsocket
CXXFLAGS += -pthreads
CXX=g++
CC=gcc
endif
ifeq ($(UNAME_S),Linux)
LIBS += -lncurses
endif

all: js olatest
js: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

eth.o: acnraw.h

acnraw.h: genframe
	./genframe

clean:
	rm -f $(OBJS) $(OBJS:.o=.d) genframe genframe.o genframe.d acnraw.h js olatest ola.o

-include *.d

test: js
	./js -x

docs: README.html

README.html: README.md
	./node_modules/markdown-it/bin/markdown-it.js README.md  > README.html

olatest: ola.o
	$(CXX) $(CXXFLAGS) -o $@ ola.o $$(pkg-config --cflags --libs libola)
