CC = /usr/bin/gcc
CFLAGS = -ggdb -O3 -mtune=native -std=gnu99 -Wall
CXX = /usr/bin/g++
CXXFLAGS = -ggdb -O3 -mtune=native -std=gnu++0x -Wall -Wextra -Weffc++
CPPFLAGS = -I include -L .
LDFLAGS = -lcg -lpthread

AR = /usr/bin/ar crs
RANLIB = /usr/bin/ranlib
MKDIR = /bin/mkdir -p
RM = /bin/rm -f

src := $(wildcard src/*.c)
obj := $(patsubst src/%.c,obj/%.o,$(src))
target = libcg.a

CDEFS =
CXXDEFS ?= $(CDEFS)

all: $(target)

$(target): $(obj)
	$(AR) $@ $(obj)
	$(RANLIB) $@

obj/%.o: src/%.c
	@$(MKDIR) obj
	$(CC) $(CFLAGS) $(CPPFLAGS) $(CDEFS) -c -o $@ $<

%: tests/%.cpp | $(target)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(CXXDEFS) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) libcg.a $(obj) $(patsubst tests/%.cpp,%,$(wildcard tests/*.cpp))
