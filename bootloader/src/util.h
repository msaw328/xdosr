#ifndef _UTIL_H_
#define _UTIL_H_

#include <Uefi.h>

// Utilities for the bootloader

// Malloc and free based on AllocatePool() with EfiLoaderData memory type
EFI_STATUS efi_malloc(IN EFI_SYSTEM_TABLE* st, IN UINTN size, OUT VOID** buffer_ret);
EFI_STATUS efi_free(IN EFI_SYSTEM_TABLE* st, IN VOID* buffer);

#endif
