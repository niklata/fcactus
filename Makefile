FCACTUS_C_SRCS = $(sort $(wildcard *.c) $(wildcard nk/*.c))
FCACTUS_CXX_SRCS = $(sort $(wildcard *.cpp) $(wildcard nk/*.cpp)) cfg.cpp
FCACTUS_OBJS = $(FCACTUS_C_SRCS:.c=.o) $(FCACTUS_CXX_SRCS:.cpp=.o)
INCL = -I.

CC ?= gcc
CCX ?= g++
CFLAGS = -O2 -s -std=gnu99 -fno-strict-overflow -Wall -Wextra -Wimplicit-fallthrough=0 -Wformat=2 -Wformat-nonliteral -Wformat-security -Wshadow -Wpointer-arith -Wmissing-prototypes -Wcast-qual -Wsign-conversion
CXXFLAGS = -O2 -s -std=gnu++17 -fno-strict-overflow -fno-rtti -Wall -Wextra -Wimplicit-fallthrough=0 -Wformat-security -Wpointer-arith

all: ragel fcactus

clean:
	rm -f fcactus *.o nk/*.o

cfg.cpp:
	ragel -G2 -o cfg.cpp cfg.rl

ragel: cfg.cpp

%.o: %.c
	$(CC) $(CFLAGS) $(INCL) -c -o $@ $^

%.o: %.cpp
	$(CCX) $(CXXFLAGS) $(INCL) -c -o $@ $^

fcactus: $(FCACTUS_OBJS)
	$(CCX) $(CXXFLAGS) $(INCL) -o $@ $^

.PHONY: all clean

