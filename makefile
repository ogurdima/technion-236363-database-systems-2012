CC = gcc
LC = ar rcs
CFLAGS = -I/usr/local/pgsql/include -L/usr/local/pgsql/lib -lpq
OUT = wet
OBJ = wet.c parser.c main.c

all:
	dos2unix *
	$(CC) $(CFLAGS) $(OBJ) -o $(OUT) 

