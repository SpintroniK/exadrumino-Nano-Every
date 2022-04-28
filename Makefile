BIN_DIR?=${AVR_TOOLS_DIR}/avr-gcc/7.3.0-atmel3.6.1-arduino5/bin
AVRDUDE_DIR?=${AVR_TOOLS_DIR}/avrdude/6.3.0-arduino17/
AVRDUDE?=${AVRDUDE_DIR}/bin/avrdude
CONFDIR?=${AVRDUDE_DIR}/etc/avrdude.conf

PORT?=/dev/ttyACM0

all:
	${BIN_DIR}/avr-g++ -c -g -Os -w -std=c++17 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -MMD -flto -mmcu=atmega4809 -DF_CPU=16000000L -DARDUINO=10809 -DARDUINO_AVR_NANO_EVERY main.cpp -o main.o
	${BIN_DIR}/avr-g++ -w -Os -g -flto -fuse-linker-plugin -Wl,--gc-sections -Wl,--section-start=.text=0x0 -mmcu=atmega4809 -o main.elf main.o
	${BIN_DIR}/avr-objcopy -O binary -R .eeprom main.elf main.bin
	${BIN_DIR}/avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 main.elf main.eep
	${BIN_DIR}/avr-objcopy -O ihex -R .eeprom main.elf main.hex
	${BIN_DIR}/avr-size -C main.elf


upload:
	python3 ./reset.py ${PORT}
	${AVRDUDE} -C${CONFDIR} -v -patmega4809 -cjtag2updi -P${PORT} -b115200 -e -D -Uflash:w:main.hex:i -Ufuse2:w:0x01:m -Ufuse5:w:0xC9:m -Ufuse8:w:0x00:m


clean:
	rm -f main.bin main.d main.eep main.elf main.hex main.o
