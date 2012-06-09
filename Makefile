OBJS = main.o craps.o
CC = g++
CPPFLAGS = -Wall -O3
CFLAGS = -Wall -O3 -c
LFLAGS = -Wall $(DEBUG)

cardcraps : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o cardcraps