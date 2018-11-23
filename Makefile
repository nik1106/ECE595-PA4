TARGET = pa4
GCC = gcc
CFLAGS = -g -std=c99 -Wall -Wshadow -Wvla -pedantic
LIBS += -lm
CC = $(GCC) $(CFLAGS)
SRCS = zst.c bt_to_list.c insert_inv.c zero_skew_adjust.c zst_main.c test_insert.c find_inv_loc.c
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
