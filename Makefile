# Install
BIN = schack

# Flags
CFLAGS += -g -std=c99 -pedantic -O0

SRC = main.c
OBJ = $(SRC:.c=.o)

$(BIN):
	@mkdir -p bin
	rm -f bin/$(BIN) $(OBJS)
	$(CC) $(SRC) $(CFLAGS) -D_POSIX_C_SOURCE=200809L -o bin/$(BIN) -lX11 -lm



# Windows compile manually with mingw:
# gcc main_win.c -I. -lgdi32 -ld3d9 -o bin/schack_cygwin.exe
