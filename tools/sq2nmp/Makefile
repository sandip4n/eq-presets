# SPDX-License-Identifier: MIT
CC = gcc
PROG = sq2nmp
OBJS = $(PROG).o

%.o: %.c
	$(CC) -c $< -o $@

$(PROG): $(OBJS)
	$(CC) -o $@ $(OBJS)

clean:
	rm -f $(OBJS)
	rm -f $(PROG)
