#ifndef _LOADER_H_
#define _LOADER_H_

#include <Uefi.h>

// Loading kernel from an ELF file

// Check buffer containing file contents to check whether its a valid kernel image
EFI_STATUS is_valid_kernel_image(IN EFI_SYSTEM_TABLE* st, IN const UINT8* file_contents);

// Allocate memory for each segment, output entry address
EFI_STATUS load_kernel_image(IN EFI_SYSTEM_TABLE* st, IN const UINT8* image_contents, OUT UINT64* entry_addr);

#endif
