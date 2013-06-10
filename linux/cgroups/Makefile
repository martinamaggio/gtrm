INCLUDE_HDR = -I jobsignal/bin
INCLUDE_LBR = -L jobsignal/bin

all: gtrm.c
	cd jobsignal
	make
	cd ..
	gcc -c cg.c
	gcc -o gtrm ${INCLUDE_HDR} ${INCLUDE_LBR} gtrm.c cg.o -lpthread -lm -ljobsignaler -lrt 

clean:
	rm -f gtrm gc.o
