#ifndef _GPT_GUID_H_
#define _GPT_GUID_H_

#include <Uefi.h>

// Definition of GPT_GUID type, which is defined as a byte array, since its main
// use is in GPT formatted drives. GPT_ prefix is there to differentiate between
// this and the EFI_GUID structure, defined in UEFI spec and EDK2 headers, which
// is implemented differently

// Bytes in data[] are in mixed endian format, used in GPT drives

struct _gpt_guid {
    UINT8 data[16];
};

typedef struct _gpt_guid GPT_GUID;

#endif
