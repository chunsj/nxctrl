CC=gcc
CFLAGS=-Wall -Os -fgnu89-inline -std=c99 -I.
LDFLAGS=
LIBS=
PRU_LIBS=-lprussdrv -lpthread

EXE01=gpio-test
SRC01=gpio-test.c NXCTRL.c
EXE02=pwm-test
SRC02=pwm-test.c NXCTRL.c
EXE03=adc-test
SRC03=adc-test.c NXCTRL.c

ARTIFACTS=$(EXE01) $(EXE02) $(EXE03)

all: $(ARTIFACTS)

$(EXE01): $(SRC01)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(EXE02): $(SRC02)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(EXE03): $(SRC03)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	rm -rf $(ARTIFACTS)
