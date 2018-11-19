TARGET = PA4
GCC = gcc
CFLAGS = -g -std=c99 -Wall -Wshadow -Wvla -pedantic
LIBS += -lm
CC = $(GCC) $(CFLAGS)
SRCS = zst.c zst_main.c
OBJS = $(SRCS:%.c=%.o)
VALGRIND = valgrind --tool=memcheck 

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)

test:
	$(VALGRIND)=./logfile ./PA4

.c.o:
	$(GCC) $(CFLAGS) -c $*.c

clean:
	rm -f $(OBJS) $(TARGET)
