.PHONY: all bootloader kernel clean

.DEFAULT_GOAL := all

all: bootloader kernel

bootloader:
	$(MAKE) -C bootloader all

kernel:
	$(MAKE) -C kernel all

clean:
	$(MAKE) -C bootloader clean
	$(MAKE) -C kernel clean
