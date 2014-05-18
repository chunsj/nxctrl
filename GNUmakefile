CC=gcc
#CFLAGS=-Wall -O3 -march=armv7-a -mtune=cortex-a8 -mfloat-abi=hard -mfpu=neon -ffast-math -fgnu89-inline -std=c99 -I.
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
EXE09=servo-test
SRC09=servo-test.c NXCTRL.c
EXE10=adcpru-test
SRC10=adcpru-test.c NXCTRL.c
EXE10-BIN=adcpru-test.bin
SRC10-PRU=adcpru-test.p
EXE11=hc-sr04-test
SRC11=hc-sr04-test.c NXCTRL.c
EXE11-BIN=hc-sr04-test.bin
SRC11-PRU=hc-sr04-test.p
EXE12=hc-sr04-simple-test
SRC12=hc-sr04-simple-test.c NXCTRL.c
EXE13=lcd-test
SRC13=lcd-test.c NXCTRL.c NXCTRL_lcd.c
EXE14=ak8448-test
SRC14=ak8448-test.c NXCTRL.c
EXE15=ssd1306-test
SRC15=ssd1306-test.c NXCTRL.c

ARTIFACTS=$(EXE01) $(EXE02) $(EXE03) $(EXE04) $(EXE04-BIN) $(EXE05) $(EXE06) $(EXE06-BIN) \
	$(EXE07) $(EXE07-BIN) $(EXE08) $(EXE08-BIN) $(EXE09) $(EXE10) $(EXE10-BIN) \
	$(EXE11) $(EXE11-BIN) $(EXE12) $(EXE13) $(EXE14) $(EXE15)

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

$(EXE09): $(SRC09)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(EXE10): $(SRC10)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) $(PRU_LIBS)

$(EXE10-BIN): $(SRC10-PRU)
	pasm -b $^

$(EXE11): $(SRC11)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) $(PRU_LIBS)

$(EXE11-BIN): $(SRC11-PRU)
	pasm -b $^

$(EXE12): $(SRC12)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(EXE13): $(SRC13)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(EXE14): $(SRC14)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(EXE15): $(SRC15)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	rm -rf $(ARTIFACTS)
