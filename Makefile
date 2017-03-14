CXXFLAGS=-g -MD -std=c++11
CFLAGS=-g -MD -std=c11

OBJS=js.o eth.o config.o main.o timer.o display.o

js: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) -lpthread -lncurses -lboost_system -lboost_thread

js.o: acnraw.h

acnraw.h: genframe
	./genframe

clean:
	rm -f $(OBJS) $(OBJS:.o=.d) genframe acnraw.h js

-include *.d
