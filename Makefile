CXXFLAGS=-g -MD

OBJS=js.o eth.o config.o main.o


js: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) -lpthread -lncurses

js.o: acnraw.h

acnraw.h: genframe
	./genframe

-include *.d