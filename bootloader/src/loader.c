#include <Uefi.h>

#include "elf.h"
#include "print.h"
#include "string.h"
#include "util.h"

EFI_STATUS is_valid_kernel_image(IN EFI_SYSTEM_TABLE* st, IN const UINT8* file_contents) {
    ELF_HEADER* eh = (ELF_HEADER*) file_contents;

    // check magic bytes
    if( eh->identity[ELF_ID_MAGIC0] != ELF_ID_MAGIC0_VAL ||
        eh->identity[ELF_ID_MAGIC1] != ELF_ID_MAGIC1_VAL ||
        eh->identity[ELF_ID_MAGIC2] != ELF_ID_MAGIC2_VAL ||
        eh->identity[ELF_ID_MAGIC3] != ELF_ID_MAGIC3_VAL) {

        printline(st, L"ERROR: Invalid magic bytes");
        return EFI_INVALID_PARAMETER;
    }

    // check if its 64 bit
    if(eh->identity[ELF_ID_BITS] != ELF_ID_BITS_64) {
        printline(st, L"ERROR: Image has to be a 64 bit ELF file");
        return EFI_INVALID_PARAMETER;
    }

    // check endian
    if(eh->identity[ELF_ID_ENDIAN] != ELF_ID_ENDIAN_LE) {
        printline(st, L"ERROR: Image has to be litte endian");
        return EFI_INVALID_PARAMETER;
    }

    // check ABI
    if(eh->identity[ELF_ID_ABI] != ELF_ID_ABI_SYSV) {
        printline(st, L"ERROR: Image has to use SYSV ABI");
        return EFI_INVALID_PARAMETER;
    }

    // check type
    if(eh->type != ELF_TYPE_EXEC) {
        printline(st, L"ERROR: Image has to be executable");
        return EFI_INVALID_PARAMETER;
    }

    // check arch
    if(eh->arch != ELF_ARCH_X86_64) {
        printline(st, L"ERROR: Image has to be for x86-64 architecture");
        return EFI_INVALID_PARAMETER;
    }

    // check program entry size
    if(eh->pht_entry_size != sizeof(ELF_PROGRAM_HEADER)) {
        printline(st, L"ERROR: Image has wrong program entry size");
        return EFI_INVALID_PARAMETER;
    }

    return EFI_SUCCESS;
}

EFI_STATUS _load_segment(EFI_SYSTEM_TABLE* st, IN const UINT8* image_contents, IN const ELF_PROGRAM_HEADER* ph, OUT EFI_PHYSICAL_ADDRESS* phys_addr, OUT UINTN* num_pages) {
    EFI_PHYSICAL_ADDRESS memaddr = ph->p_vaddr;

    // If execute flag is set allocate EfiLoaderCode, otherwise EfiLoaderData
    EFI_MEMORY_TYPE memtype = (ph->flags & ELF_PROG_FLAG_X) ? EfiLoaderCode : EfiLoaderData;

    UINT64 pages = EFI_SIZE_TO_PAGES(ph->p_memsz);
    EFI_STATUS status = st->BootServices->AllocatePages(
        AllocateAddress, // allocate page at specific address
        memtype,
        pages,
        &memaddr
    );

    if(status != EFI_SUCCESS) {
        print(st, L"ERROR: Cannot allocate pages: ");

        if(status == EFI_OUT_OF_RESOURCES)
            printline(st, L"Out of resources");
        else
            printline(st, L"Other error");
    
        return status;
    }

    if((UINT64) memaddr != ph->p_vaddr) {
        printline(st, L"ERROR: Address changed");
        st->BootServices->FreePages(memaddr, pages);
        return EFI_NOT_FOUND;
    }

    memcpy((UINT8*) memaddr, image_contents + ph->p_offset, ph->p_filesz);

    // Difference in size in file and memory
    UINT64 size_difference = ph->p_memsz - ph->p_filesz;

    if(size_difference > 0) {
        memset((UINT8*) memaddr + ph->p_filesz, 0x0, size_difference);
    }

    *phys_addr = memaddr;
    *num_pages = pages;

    return EFI_SUCCESS;
}

EFI_STATUS load_kernel_image(IN EFI_SYSTEM_TABLE* st, IN const UINT8* image_contents, OUT UINT64* entry_addr) {
    ELF_HEADER* eh = (ELF_HEADER*) image_contents;
    EFI_STATUS status;

    UINT16 segment_count = eh->pht_entry_count;

    // In case page fails to allocate keep track of already allocated memory
    // and it's size to free it
    EFI_PHYSICAL_ADDRESS* allocs;
    status = efi_malloc(st, sizeof(EFI_PHYSICAL_ADDRESS) * segment_count, (VOID**) &allocs);
    if(status != EFI_SUCCESS) {
        printline(st, L"ERROR: after malloc() - allocs");
        return status;
    }

    UINTN* allocs_sizes;
    status = efi_malloc(st, sizeof(UINTN) * segment_count, (VOID**) &allocs_sizes);
    if(status != EFI_SUCCESS) {
        printline(st, L"ERROR: after malloc() - allocs_sizes");
        efi_free(st, allocs);
        return status;
    }

    ELF_PROGRAM_HEADER* ph = (ELF_PROGRAM_HEADER*) (image_contents + eh->pht_off);

    UINT16 i = 0;

    while(i < segment_count) {
        if(ph[i].type != ELF_PROG_TYPE_LOAD) {
            i++;
            continue;
        }

        EFI_STATUS status = _load_segment(st, image_contents, ph + i, allocs + i, allocs_sizes + i);
        if(status != EFI_SUCCESS) {
            printline(st, L"ERROR: Can't load segment");

            for(UINT16 j = 0; j < i; j++) {
                st->BootServices->FreePages(allocs[j], allocs_sizes[j]);
            }

            return status;
        }

        i++;
    }

    *entry_addr = eh->entry_addr;

    return EFI_SUCCESS;
}
