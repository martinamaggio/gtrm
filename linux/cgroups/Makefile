INCLUDE_HDR = -I bin
INCLUDE_LBR = -L bin

all: gtrm.c
	mkdir bin
	mv *.h bin
	./jobsignal/build/autobuild.sh
	gcc -c cg.c
	gcc -o gtrm ${INCLUDE_HDR} ${INCLUDE_LBR} gtrm.c cg.o -lpthread -lm -ljobsignaler -lrt
	mv cg.o bin/.
	mv gtrm bin/.

clean:
	rm -rf bin
