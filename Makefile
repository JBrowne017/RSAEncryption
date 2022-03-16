CC = clang
CFLAGS = -Wall -Werror -Wpedantic -Wextra $(shell pkg-config --cflags gmp)
LFLAGS = $(shell pkg-config --libs gmp)
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=*.o)
EXECBIN = keygen encrypt decrypt

KEY_SRC = rsa.c numtheory.c randstate.c keygen.c
KEY_OBJ = $(KEY_SRC:.c=*.o)
ENC_SRC = rsa.c numtheory.c randstate.c encrypt.c
ENC_OBJ = $(ENC_SRC:.c=*.o)
DEC_SRC = rsa.c numtheory.c randstate.c decrypt.c
DEC_OBJ = $(DEC_SRC:.c=*.o)

.PHONY: all clean format debug

all: $(EXECBIN)

debug: CFLAGS += -g
debug: all

clean:
	rm -f $(OBJ) $(EXECBIN)

format:
	clang-format -i -style=file *.[c,h]

keygen: $(KEY_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

decrypt: $(DEC_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)


encrypt: $(ENC_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<
