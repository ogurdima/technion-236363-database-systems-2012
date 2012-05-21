CC = gcc
LC = ar rcs
CFLAGS = -I/usr/local/pgsql/include -L/usr/local/pgsql/lib -lpq
DFLAGS = -D MAIN_DBG -D WET_DBG
SUBFLAGS = -U MAIN_DBG -U WET_DBG
OUT = wet
OBJ = wet.c parser.c main.c

all:
	dos2unix *
	$(CC) $(CFLAGS) $(DFLAGS) $(OBJ) -o $(OUT) 

submission:
	dos2unix *
	$(CC) $(CFLAGS) $(SUBFLAGS) $(OBJ) -o $(OUT) 