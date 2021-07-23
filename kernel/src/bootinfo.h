#ifndef _BOOTINFO_H_
#define _BOOTINFO_H_

#include <Uefi.h>

struct _gpt_guid {
    UINT8 data[16];
};

typedef struct _gpt_guid gpt_guid_t;

struct _bootloader_config {
    CHAR16 kernel_path[64];
    gpt_guid_t disk_guid;
    gpt_guid_t part_guid;
};

typedef struct _bootloader_config bootloader_config_t;

struct _boot_info_t {
    EFI_SYSTEM_TABLE* st;
    bootloader_config_t* cfg;
    void* acpi_rsdp;
};

typedef struct _boot_info_t boot_info_t;

#endif
