CC=gcc
CFLAGS=-Os -I..
LDFLAGS=
LIBS=-lnsbmp

EXE01=gen-bitmap
SRC01=gen-bitmap.c ../NXCTRL_bitArray.c

ARTIFACTS=$(EXE01)

all: $(ARTIFACTS)

$(EXE01): $(SRC01)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	rm -rf $(ARTIFACTS)
