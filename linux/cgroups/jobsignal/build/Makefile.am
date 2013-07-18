lib_LIBRARIES = libjobsignaler.a libcg.a

libjobsignaler_a_SOURCES = src/jobsignaler.h src/jobsignaler.c
libjobsignaler_a_CXXFLAGS = -O2
libjobsignaler_a_LDFLAGS = -lpthread -lrt -lm

libcg_a_SOURCES = src/cg.h src/cg.c
libcg_a_CXXFLAGS = -O2
libcg_a_LDFLAGS = -lpthread -lm

bin_PROGRAMS = application
applicationdir = src/test
application_SOURCES = src/test/application.c
application_HEADERS = src/jobsignaler.h src/cg.h 
application_LDADD = libjobsignaler.a libcg.a
application_LDFLAGS = -lpthread -lrt -lm