all: gtrm.c
	gcc -c dl_syscalls.c
	gcc -o gtrm gtrm.c dl_syscalls.o -lpthread -lm -ljobsignaler -lrt 

clean:
	rm -f gtrm dl_syscalls.o

