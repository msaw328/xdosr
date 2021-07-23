#ifndef _BOOTINFO_H_
#define _BOOTINFO_H_

#include <Uefi.h>

#include "parser.h"

// Structure passed to the kernel

struct _boot_info_t {
    EFI_SYSTEM_TABLE* st;
    CONFIG* cfg;
    VOID* acpi_rsdp;
};

typedef struct _boot_info_t BOOTINFO;

#endif
