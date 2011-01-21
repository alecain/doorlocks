#Programming options
AVRDUDE =     avrdude
APROGRAMMER = avr109
APART =       m32u4
APORT =       /dev/ttyACM0
AFLAGS =      -p $(APART) -c $(APROGRAMMER) -e -P $(APORT)
ABURN =       -U flash:w:

#Compilation options
CC =     avr-gcc
CPART =  atmega32u4
CFLAGS = -Wall -mmcu=$(CPART) -o $@

#Object stripping options
OC =      avr-objcopy
OFORMAT = ihex
OMEMORY = .eeprom
OFLAGS =  -O $(OFORMAT) -R $(OMEMORY)


main.hex: main.o onewire.o
	$(CC) $(CFLAGS) $<
	$(OC) $(OFLAGS) $@

main.o: main.c
	$(CC) $(CFLAGS) -c $<

onewire.hex: onewire.o
	$(CC) $(CFLAGS) $<
	$(OC) $(OFLAGS) $@

onewire.o: onewire.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o
	rm -f *.hex

burn: onewire.hex
	$(AVRDUDE) $(AFLAGS) $(ABURN)$<
