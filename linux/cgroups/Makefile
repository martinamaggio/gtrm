all: gtrm.c
	gcc -c cg.c
	gcc -o gtrm gtrm.c cg.o -lpthread -lm -ljobsignaler -lrt 

clean:
	rm -f gtrm gc.o
