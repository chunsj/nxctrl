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
EXE04=pru-test
SRC04=pru-test.c NXCTRL.c
EXE04-BIN=pru-test.bin
SRC04-PRU=pru-test.p
EXE05=spi-test
SRC05=spi-test.c NXCTRL.c

ARTIFACTS=$(EXE01) $(EXE02) $(EXE03) $(EXE04) $(EXE04-BIN) $(EXE05)

all: $(ARTIFACTS)

$(EXE01): $(SRC01)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(EXE02): $(SRC02)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(EXE03): $(SRC03)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(EXE04): $(SRC04)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) $(PRU_LIBS)

$(EXE04-BIN): $(SRC04-PRU)
	pasm -b $^

$(EXE05): $(SRC05)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	rm -rf $(ARTIFACTS)
