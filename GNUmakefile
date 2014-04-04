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
EXE06=pru2-test
SRC06=pru2-test.c NXCTRL.c
EXE06-BIN=pru2-test.bin
SRC06-PRU=pru2-test.p
EXE07=ram-test
SRC07=ram-test.c NXCTRL.c
EXE07-BIN=ram-test.bin
SRC07-PRU=ram-test.p
EXE08=sl0-test
SRC08=sl0-test.c NXCTRL.c
EXE08-BIN=sl0-test.bin
SRC08-PRU=sl0-test.p

ARTIFACTS=$(EXE01) $(EXE02) $(EXE03) $(EXE04) $(EXE04-BIN) $(EXE05) $(EXE06) $(EXE06-BIN) \
	$(EXE07) $(EXE07-BIN) $(EXE08) $(EXE08-BIN)

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

$(EXE06): $(SRC06)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) $(PRU_LIBS)

$(EXE06-BIN): $(SRC06-PRU)
	pasm -b $^

$(EXE07): $(SRC07)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) $(PRU_LIBS)

$(EXE07-BIN): $(SRC07-PRU)
	pasm -b $^

$(EXE08): $(SRC08)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) $(PRU_LIBS)

$(EXE08-BIN): $(SRC08-PRU)
	pasm -b $^

clean:
	rm -rf $(ARTIFACTS)
