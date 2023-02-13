C = gcc
CFLAGS = -Wall -g -pedantic 
INCLUDES = -I/usr/include/mariadb -I/usr/include/mariadb/mysql
LFLAGS = -L/usr/lib/x86_64-linux-gnu/
LIBS = -lncurses -lform -lmariadb
SRCS = parts.c
OBJS = $(SRCS:.c=.o)
MAIN = parts

.PHONY: depend clean

all:	$(MAIN)
	@echo parts has been compiled

$(MAIN):	$(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^
