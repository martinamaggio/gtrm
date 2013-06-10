INCLUDE_HDR = -I jobsignal/bin
INCLUDE_LBR = -L jobsignal/bin

all: gtrm.c
	cd jobsignal
	make
	cd ..
	gcc -c dl_syscalls.c
	gcc -o gtrm ${INCLUDE_HDR} ${INCLUDE_LBR} gtrm.c dl_syscalls.o -lpthread -lm -ljobsignaler -lrt 

clean:
	rm -f gtrm dl_syscalls.o

