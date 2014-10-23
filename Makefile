all:	bbs server

bbs:	bbs.c common.o
	gcc bbs.c common.o -o bbs -lncurses
server:	server.c common.h common.o server.h
	gcc server.c common.o -o server
depend:
	gcc *.c -MM > .depend
	@echo "Done building dependencies."

clean:
	rm bbs server *.o

.c.o:
	gcc -c -o $@ $<

include .depend
