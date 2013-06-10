INCLUDE_HDR = "-I /work/martina/tests/jobsignal/bin"
INCLUDE_LBR = "-L /work/martina/tests/jobsignal/bin"

all: gtrm.c
	gcc -c cg.c
	gcc -o $INCLUDE_HDR $INCLUDE_LBR gtrm gtrm.c cg.o -lpthread -lm -ljobsignaler -lrt 

clean:
	rm -f gtrm gc.o
