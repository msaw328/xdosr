.PHONY: all dir build clean

.DEFAULT_GOAL := all

SOURCES := kmain.c textio.c term.c

all: dir build

dir:
	@mkdir -p build build/obj
	@echo "Creating build directory"

clean:
	@rm -rf build
	@echo "Removing build directory"

build: build/xdosr.elf

# actual compilation stuff below
CC := clang
CFLAGS := \
		-fno-stack-protector \
		-ffreestanding \
		-mno-red-zone

LDFLAGS := \
		-static \
		-nostdlib \
		-Wl,--entry=kmain

OBJECTS := $(SOURCES:.c=.o)

INCLUDE := -I../bootloader/external/edk2/Include -I../bootloader/external/edk2/Include/X64

build/xdosr.elf: $(addprefix build/obj/,$(OBJECTS))
	@$(CC) $(LDFLAGS) -o $@ $^
	@echo -e "\t[LD] $^ -o $@"

build/obj/%.o: src/%.c
	@$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $^
	@echo -e "\t[CC] $^ -o $@"
