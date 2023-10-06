CFLAGS = -std=c11 -O2 -Wall -Wextra -flto -fstrict-aliasing
CXXFLAGS = -std=c++11 -O2 -Wall -Wextra -flto -fstrict-aliasing
LDFLAGS = -flto

CSources = $(wildcard *.c)
CXXSources = $(wildcard *.cc)
Objects = $(CSources:.c=.o) $(CXXSources:.cc=.o)

test: $(Objects)
