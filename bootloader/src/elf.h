#ifndef _ELF_H_
#define _ELF_H_

#include <Uefi.h>

// ELF structures definitions
// If headers differ bwteeen 32 and 64 bit versions of ELF the structs below represent
// the 64 bit versions

// Main ELF header, starting at offset 0
struct _elf_header {
    UINT8 identity[16];
    UINT16 type;
    UINT16 arch;
    UINT32 version;
    UINT64 entry_addr; // entry point into the executable
    UINT64 pht_off; // program header table offset
    UINT64 sht_off; // section header table offset
    UINT32 flags;
    UINT16 h_size; // header size
    UINT16 pht_entry_size;
    UINT16 pht_entry_count;
    UINT16 sht_entry_size;
    UINT16 sht_entry_count;
    UINT16 sht_str_idx;
};

typedef struct _elf_header ELF_HEADER;

// Indices into the identity array
#define ELF_ID_MAGIC0 0 // first magic byte, should be 7f
#define ELF_ID_MAGIC0_VAL 0x7f
#define ELF_ID_MAGIC1 1 // second magic byte, should be 'E'
#define ELF_ID_MAGIC1_VAL 'E'
#define ELF_ID_MAGIC2 2 // third magic byte, should be 'L'
#define ELF_ID_MAGIC2_VAL 'L'
#define ELF_ID_MAGIC3 3 // fourth magic byte, should be 'F'
#define ELF_ID_MAGIC3_VAL 'F'

#define ELF_ID_BITS 4 // 64 or 32 bit executable
#define ELF_ID_BITS_32 0x1
#define ELF_ID_BITS_64 0x2

#define ELF_ID_ENDIAN 5 // little or big endian
#define ELF_ID_ENDIAN_LE 0x1
#define ELF_ID_ENDIAN_BE 0x2

#define ELF_ID_VERSION 6 // header version

#define ELF_ID_ABI 7 // ABI of the executable
#define ELF_ID_ABI_SYSV 0x0 // SYSV ABI, the only we accept
// bytes from 8 to 15 are padding

// Values for type attribute
#define ELF_TYPE_RELOC 0x1 // relocatable
#define ELF_TYPE_EXEC 0x2 // executable
#define ELF_TYPE_SHARED 0x3 // shared library
#define ELF_TYPE_CORE 0x4 // core dump file

// Values for arch attribute
#define ELF_ARCH_X86_64 0x3e


// ELF Program Header - describes a memory segment in ELF file
struct _elf_program_header {
    UINT32 type;
    UINT32 flags;
    UINT64 p_offset; // offset in file
    UINT64 p_vaddr; // memory address
    UINT64 undefined; // https://wiki.osdev.org/ELF field is undefined for SYS V ABI
    UINT64 p_filesz; // size in file
    UINT64 p_memsz; // size in memory
    UINT64 align; // alignment
};

typedef struct _elf_program_header ELF_PROGRAM_HEADER;

// Types of program headers
#define ELF_PROG_TYPE_LOAD 0x1 // load this segment

// Types of flags
#define ELF_PROG_FLAG_R 0x4
#define ELF_PROG_FLAG_W 0x2
#define ELF_PROG_FLAG_X 0x1

#endif
