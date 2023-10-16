CFLAGS = -std=c11 -O2 -Wall -Wextra -flto -fstrict-aliasing
LDFLAGS = -flto

Sources = $(wildcard *.c)
Objects = $(Sources:.c=.o)

test: $(Objects)
