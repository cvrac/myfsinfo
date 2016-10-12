OBJS	= myfsinfo.o funcs.o
SOURCE	= myfsinfo.c funcs.c
HEADER	= defn.h
OUT		= myfsinfo
CC		= gcc
FLAGS	= -g -c
#-g for debugging
#-c generates object code for seperate files

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

#seperate compilation of files
myfsinfo.o: myfsinfo.c
	$(CC) $(FLAGS) myfsinfo.c

funcs.o: funcs.c
	$(CC) $(FLAGS) funcs.c

#clean 
clean:
	rm -f $(OBJS) $(OUT)

count:
	wc -l $(SOURCE) $(HEADER)
