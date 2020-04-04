# Author: NStorm
# License: Apache License Version 2.0, January 2004 http://www.apache.org/licenses/

DEVICE     = atmega8
F_CPU      = 16000000
PROGRAMMER = -c arduino -P /dev/ttyUSB* -b 57600
OBJECTS    = m8-test.o

######################################################################
######################################################################

# Tune the lines below only if you know what you are doing:

CC = avr-gcc
SIZE = avr-size
CFLAGS = -Os -g -std=gnu11 -flto -ffunction-sections -fdata-sections -Wl,--gc-sections -Wall -Wl,--warn-common
# -Wl,-u,vfprintf -lprintf_min
AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE) -v
COMPILE = $(CC) $(CFLAGS) -DF_CPU=$(F_CPU) -mmcu=$(DEVICE)

# symbolic targets:
all:	m8-test.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:m8-test.hex:i

install: flash

clean:
	rm -f m8-test.hex m8-test.elf m8-test.lss $(OBJECTS)

# file targets:
m8-test.elf: $(OBJECTS)
	$(COMPILE) -o m8-test.elf $(OBJECTS)

m8-test.hex: m8-test.elf
	rm -f m8-test.hex
	avr-objcopy -j .text -j .data -O ihex m8-test.elf m8-test.hex
	$(SIZE) -C --mcu=$(DEVICE) m8-test.elf
# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.

# Targets for code debugging and analysis:
disasm:	m8-test.elf
	avr-objdump -d -h -S m8-test.elf > m8-test.lss

cpp:
	$(COMPILE) -E m8-test.c

debug: CFLAGS = -DDEBUG -Og -g -std=gnu11 -Wall
debug: clean m8-test.hex disasm
