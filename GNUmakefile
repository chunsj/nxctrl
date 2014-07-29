CC=gcc
#CFLAGS=-Wall -O3 -march=armv7-a -mtune=cortex-a8 -mfloat-abi=hard -mfpu=neon -ffast-math -fgnu89-inline -std=c99 -I.
CFLAGS=-Wall -Os -fgnu89-inline -std=c99 -I.
LDFLAGS=
LIBS=
DL_LIBS=-ldl
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
SRC15=ssd1306-test.c NXCTRL.c NXCTRL_oled.c
EXE16=bigbm-test
SRC16=bigbm-test.c NXCTRL.c NXCTRL_oled.c NXCTRL_bitArray.c
EXE17=dpybmp-test
SRC17=dpybmp-test.c NXCTRL.c NXCTRL_oled.c NXCTRL_bitArray.c
EXE18=ctrl-app
SRC18=ctrl-app.c NXCTRL.c NXCTRL_oled.c NXCTRL_bitArray.c
APP18=app-test.app
SRC18-APP=app-test.c NXCTRL.c
EXE18-BIN=ctrl-app.bin
SRC18-PRU=ctrl-app.p
EXE19=pin-info
SRC19=pin-info.c
EXE20=tcdr
SRC20=tcdr.c NXCTRL.c NXCTRL_oled.c NXCTRL_bitArray.c
APP21=app-main.app
SRC21-APP=app-main.c NXCTRL.c
APP22=app-sysutil.app
SRC22-APP=app-sysutil.c NXCTRL.c
APP23=app-blank.app
SRC23-APP=app-blank.c NXCTRL.c
APP24=app-conninfo.app
SRC24-APP=app-conninfo.c NXCTRL.c
APP25=app-sysinfo.app
SRC25-APP=app-sysinfo.c NXCTRL.c
APP26=app-peri.app
SRC26-APP=app-peri.c NXCTRL.c
APP27=app-spark.app
SRC27-APP=app-spark.c NXCTRL.c

ARTIFACTS=$(EXE01) $(EXE02) $(EXE03) $(EXE04) $(EXE04-BIN) $(EXE05) $(EXE06) $(EXE06-BIN) \
	$(EXE07) $(EXE07-BIN) $(EXE08) $(EXE08-BIN) $(EXE09) $(EXE10) $(EXE10-BIN) \
	$(EXE11) $(EXE11-BIN) $(EXE12) $(EXE13) $(EXE14) $(EXE15) $(EXE16) $(EXE19) \
	$(APP18)

AUX_ARTIFACTS=$(EXE17) $(EXE18) $(EXE18-BIN) $(EXE20) $(APP21) $(APP22) $(APP23) $(APP24) \
	$(APP25) $(APP26) $(APP27)

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

$(EXE16): $(SRC16)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(EXE17): $(SRC17)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -lnsbmp

$(EXE18): $(SRC18)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) $(DL_LIBS) $(PRU_LIBS)

$(APP18): $(SRC18-APP)
	$(CC) $(CFLAGS) -fPIC -DPIC -shared -o $@ $^

$(EXE18-BIN): $(SRC18-PRU)
	pasm -b $^

$(EXE19): $(SRC19)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(EXE20): $(SRC20)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) $(DL_LIBS) $(PRU_LIBS) -lm

$(APP21): $(SRC21-APP)
	$(CC) $(CFLAGS) -fPIC -DPIC -shared -o $@ $^

$(APP22): $(SRC22-APP)
	$(CC) $(CFLAGS) -fPIC -DPIC -shared -o $@ $^

$(APP23): $(SRC23-APP)
	$(CC) $(CFLAGS) -fPIC -DPIC -shared -o $@ $^

$(APP24): $(SRC24-APP)
	$(CC) $(CFLAGS) -fPIC -DPIC -shared -o $@ $^

$(APP25): $(SRC25-APP)
	$(CC) $(CFLAGS) -fPIC -DPIC -shared -o $@ $^

$(APP26): $(SRC26-APP)
	$(CC) $(CFLAGS) -fPIC -DPIC -shared -o $@ $^

$(APP27): $(SRC27-APP)
	$(CC) $(CFLAGS) -fPIC -DPIC -shared -o $@ $^

clean:
	rm -rf $(ARTIFACTS) $(AUX_ARTIFACTS)

install-tcdr: tcdr app-main.app app-sysutil.app app-blank.app app-conninfo.app app-sysinfo.app app-peri.app \
	app-spark.app
	strip tcdr
	cp tcdr /usr/bin
	cp app-main.app app-sysutil.app app-blank.app app-conninfo.app app-sysinfo.app app-peri.app /usr/bin
	cp app-spark.app /usr/bin
