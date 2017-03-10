CFLAGS=-std=c11

OBJS=js.o eth.o


js: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -lpthread -lncurses

js.o: acnraw.h

acnraw.h: genframe
	./genframe
