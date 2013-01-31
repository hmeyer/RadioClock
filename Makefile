# Name:			Makefile
# Author:		Henning Meyer
# Copyright:	2012 Henning Meyer
#
# This Makefile is derived from the MigtyOhm Geiger Counter

# Values you might need to change:
# In particular, check that your programmer is configured correctly.
#
# PROGRAM		The name of the "main" program file, without any suffix.
# OBJECTS		The object files created from your source files. This list is
#                usually the same as the list of source files with suffix ".o".
# DEVICE		The AVR device you are compiling for.
# CLOCK			Target AVR clock rate in Hz (eg. 8000000)
# PROGRAMMER	Programmer hardware used to flash program to target device.
# PORT			The peripheral port on the host PC that the programmer is connected to.	
# LFUSE			Target device configuration fuses, low byte.
# HFUSE			Targer device configuration fuses, high byte.
# EFUSE			Target device configuration fuses (extended).

PROGRAM		= radioclock
WISHIELD_OBJS	= WiShield.o g2100.o stack.o uip.o network.o uip_arp.o socketapp.o psock.o timer.o clock-arch.o
OBJECTS		= radioclock.o display.o myspi.o wiring.o switch.o wifi.o scroller.o $(WISHIELD_OBJS:%=WiShield/%) netcommand.o ClockTime.o
DEVICE		= atmega328p
BAUDRATE	= 115200
CLOCK		= 16000000
#PROGRAMMER	= usbtiny
#PROGRAMMER	= avrispmkII
PROGRAMMER	= arduino
PORT		= /dev/ttyUSB0
TOP := $(shell pwd)

# Fuse configuration:
# For a really nice guide to AVR fuses, see http://www.engbedded.com/fusecalc/
# LFUSE: SUT0, CKSEL0 (Ext Xtal 8+Mhz, 0ms startup time)
#LFUSE		= 0xEE
# HFUSE: SPIEN, BODLEVEL0 (Serial programming enabled, Brownout = 1.8V
#HFUSE		= 0xDD
# EFUSE: no fuses programmed
#EFUSE		= 0xFF

# Tune the lines below only if you know what you are doing:

AVRDUDE = avrdude -c $(PROGRAMMER) -P $(PORT) -p $(DEVICE) -b $(BAUDRATE) -D
CXX = avr-g++
CC = avr-gcc
ALLCFLAGS = -save-temps -Wall -g -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -finline-limit=3 -fno-tree-loop-optimize -I.
CXXFLAGS = $(ALLCFLAGS)
CFLAGS = $(ALLCFLAGS)
COMPILE = $(CXX) $(CFLAGS)
COMPILEC = $(CC) $(CXXFLAGS)

# Linker options
LDFLAGS	= -Wl,-Map=$(PROGRAM).map -Wl,--cref 

# Add size command so we can see how much space we are using on the target device.
SIZE	= avr-size -C --mcu=$(DEVICE)




# symbolic targets:
all:	$(PROGRAM).hex
	$(SIZE) $(PROGRAM).elf

$(PROGRAM):	all	
	
flash: all
	$(AVRDUDE) -U flash:w:$(PROGRAM).hex:i

fuse:
	$(AVRDUDE) -U hfuse:w:$(HFUSE):m -U lfuse:w:$(LFUSE):m -U efuse:w:$(EFUSE):m
	
# Xcode uses the Makefile targets "", "clean" and "install"
install: flash fuse

clean:
	rm -f $(PROGRAM).hex $(PROGRAM).elf $(OBJECTS) $(OBJECTS:.o=.d) $(OBJECTS:.o=.ii) $(OBJECTS:.o=.s) $(PROGRAM).lst $(PROGRAM).map $(WISHIELD_OBJS:%.o=%.ii) $(WISHIELD_OBJS:%.o=%.i) $(WISHIELD_OBJS:%.o=%.s) charset.h dep.make

# file targets:
%.hex: %.elf
	avr-objcopy -j .text -j .data -O ihex $< $@
	
$(PROGRAM).elf: $(OBJECTS)
	$(COMPILE) -o $@ $^ $(LDFLAGS)

dep.make: charset.h
	$(CXX) $(CXXFLAGS) -MM $(OBJECTS:.o=.c*) > $@

include dep.make

charset.h: charset.txt
	./charConv.pl charset.txt > charset.h

#display.o: display.cpp charset.h

%.o: %.cpp
	$(COMPILE) -c $< -o $@

%.o: %.c
	$(COMPILEC) -c $< -o $@

# Targets for code debugging and analysis:
disasm:	$(PROGRAM).elf
	avr-objdump -h -S $(PROGRAM).elf > $(PROGRAM).lst

# Tell make that these targets don't correspond to actual files
.PHONY :	all $(PROGRAM) flash fuse install clean disasm
