INCLUDE_HDR = -I bin
INCLUDE_LBR = -L bin

all: gtrm.c
	mkdir bin
	cp *.h bin
	./jobsignal/build/autobuild.sh
	gcc -o gtrm ${INCLUDE_HDR} ${INCLUDE_LBR} gtrm.c cg.o -lcg -lpthread -lm -ljobsignaler -lrt
	mv gtrm bin/.

clean:
	rm -rf bin
