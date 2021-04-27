.PHONY: all dir build clean

.DEFAULT_GOAL := all

# source files, moved them up so theyre easily accessible
SOURCES := main.c util.c files.c string.c hexconv.c print.c parser.c

all: dir build

dir:
	@mkdir -p build build/obj
	@echo "Creating build directory"

clean:
	@rm -rf build
	@echo "Removing build directory"

build: build/XDOSR.EFI

# actual compilation stuff below
# https://dvdhrm.github.io/2019/01/31/goodbye-gnuefi/
# target x86_64-unknown-windows because UEFI apps are PE32-ish
# -ffreestanding is trivial
# -fshort-wchar because UEFI spec defines CHAR8 and CHAR16, so wide char should have 2 bytes not 4 (Section 2.3.1 Data Types in UEFI spec)
# -mno-red-zone because UEFI does not allow red zone? not sure
CC := clang
CFLAGS := \
		-target x86_64-unknown-windows \
		-ffreestanding \
		-fshort-wchar \
		-mno-red-zone

# -nostdlib because we don't use standard libraries, don't have access to them in UEFI land
# -entry and -subsystem are trivial
# -fuse-ld spcifies which linker backend to use i think, and lld-link produces PE32-ish executables? not sure
LDFLAGS := \
		-target x86_64-unknown-windows \
		-nostdlib \
		-Wl,-entry:efi_main \
		-Wl,-subsystem:efi_application \
		-fuse-ld=lld-link

INCLUDE := -Iexternal/edk2/MdePkg/Include -Iexternal/edk2/MdePkg/Include/X64

OBJECTS := $(SOURCES:.c=.o)

build/XDOSR.EFI: $(addprefix build/obj/,$(OBJECTS))
	@$(CC) $(LDFLAGS) -o $@ $^
	@echo -e "\t[LD] $^ -o $@"

build/obj/%.o: src/%.c
	@$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $^
	@echo -e "\t[CC] $^ -o $@"
