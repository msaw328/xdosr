#ifndef _HEXCONV_H_
#define _HEXCONV_H_

#include <Uefi.h>

#include "gpt_guid.h"

// Functionality for converting numerical and binary data to hex strings
// and vice versa

// Converts a given string representation of GUID to a EFI_GUID struct
EFI_STATUS guid_from_str(IN EFI_SYSTEM_TABLE* st, OUT GPT_GUID* guid, IN CHAR16* str);

// Covnerts given buffer of size n bytes to a hexstring, allocates a buffer using efi_malloc from util.h
EFI_STATUS buffer_to_hexstr(IN EFI_SYSTEM_TABLE* st, OUT CHAR16** str, IN UINT8* buff, IN UINTN n);

#endif
