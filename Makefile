FCACTUS_C_SRCS = $(sort $(wildcard *.c) $(wildcard nk/*.c))
FCACTUS_CXX_SRCS = $(sort $(wildcard *.cpp) $(wildcard nk/*.cpp) cfg.cpp)
FCACTUS_OBJS = $(FCACTUS_C_SRCS:.c=.o) $(FCACTUS_CXX_SRCS:.cpp=.o)
FCACTUS_DEP = $(FCACTUS_C_SRCS:.c=.d) $(FCACTUS_CXX_SRCS:.cpp=.d)
INCL = -I.

CFLAGS = -MMD -O2 -s -std=gnu99 -fno-strict-overflow -Wall -Wextra -Wimplicit-fallthrough=0 -Wformat=2 -Wformat-nonliteral -Wformat-security -Wshadow -Wpointer-arith -Wmissing-prototypes -Wcast-qual -Wsign-conversion
CXXFLAGS = -MMD -O2 -s -std=gnu++17 -fno-strict-overflow -fno-rtti -Wall -Wextra -Wimplicit-fallthrough=0 -Wformat-security -Wpointer-arith
CPPFLAGS += $(INCL)

all: ragel fcactus

fcactus: $(FCACTUS_OBJS)
	$(CXX) $(CXXFLAGS) $(INCL) -o $@ $^

-include $(FCACTUS_DEP)

clean:
	rm -f $(FCACTUS_OBJS) $(FCACTUS_DEP) fcactus

cleanragel:
	rm -f cfg.cpp

cfg.cpp:
	ragel -G2 -o cfg.cpp cfg.rl

ragel: cfg.cpp

.PHONY: all clean cleanragel

