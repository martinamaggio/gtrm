INCLUDE_HDR = -I bin
INCLUDE_LBR = -L bin

all: gtrm.c
	./jobsignal/build/autobuild.sh
	gcc -c dl_syscalls.c
	gcc -o gtrm ${INCLUDE_HDR} ${INCLUDE_LBR} gtrm.c dl_syscalls.o -lpthread -lm -ljobsignaler -lrt 
	mv cg.o bin/.
	mv gtrm bin/.

clean:
	rm -rf bin

