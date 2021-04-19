#include <Uefi.h>

#include "hexconv.h"

#include "gpt_guid.h"
#include "string.h"
#include "util.h"
#include "print.h"

// Checks if CHAR16 is a valid hex digit
#define IS_VALID_HEX_DIGIT(digit) ((L'0' <= digit && digit <= L'9') || (L'a' <= digit && digit <= L'f') || (L'A' <= digit && digit <= L'F'))

// Returns digit from value, does not validate input
CHAR16 __digit_from_val(IN UINT8 val) {
    if(0x0 <= val && val <= 0x9)
        return L'0' + val;

    if(0xa <= val && val <= 0xf)
        return L'a' + val - 0xa;

    // so that compiler does not complain
    return L'\0';
}

// Returns value from digit, does not validate input
UINT8 __val_from_digit(IN CHAR16 digit) {
    if(L'0' <= digit && digit <= L'9')
        return (UINT8) (digit - L'0');

    if(L'a' <= digit && digit <= L'f')
        return 0xa + (UINT8) (digit - L'a');

    if(L'A' <= digit && digit <= L'F')
        return 0xa + (UINT8) (digit - L'A');

    // so that compiler does not complain
    return 0x00;
}

// modifies memory at addr hexpair according to val, needs at least 2 CHAR16 allocated
VOID __hexpair_from_byte(IN UINT8 val, OUT CHAR16* hexpair) {
    hexpair[0] = __digit_from_val(val >> 4); // upper half
    hexpair[1] = __digit_from_val(val & 0xf); // lower half
}

// needs two hex digits, returns byte, does not validate input
UINT8 __byte_from_hexpair(IN CHAR16* hexpair) {
    return (__val_from_digit(hexpair[0]) << 4) | __val_from_digit(hexpair[1]);
}

// all GUID strings are in format 12345678-1234-1234-1234-123456789abc
// this len is in CHAR16, size is in bytes
#define GUID_STRING_LEN 36
#define GUID_STRING_SIZE (GUID_STRING_LEN * sizeof(CHAR16))

EFI_STATUS guid_from_str(IN EFI_SYSTEM_TABLE* st, OUT GPT_GUID* g, IN CHAR16* str) {
    // check string length, all guid strings are constant length
    if(strnlen(str, GUID_STRING_LEN + 1) != GUID_STRING_LEN) {
        print(st, L"GUID string has wrong length: ");
        printline(st, str);

        return EFI_INVALID_PARAMETER;
    }

    // check if all characters are ok
    for(UINTN i = 0; i < GUID_STRING_LEN; i++) {
        // hyphens
        if(i == 8 || i == 13 || i == 18 || i == 23) {
            if(str[i] != L'-') {
                print(st, L"Bad char in GUID string, expected hyphen: ");
                printline(st, str);

                return EFI_INVALID_PARAMETER;
            }
        // digits
        } else {
            if(!IS_VALID_HEX_DIGIT(str[i])) {
                print(st, L"Bad char in GUID string, expected hex digit: ");
                printline(st, str);

                return EFI_INVALID_PARAMETER;
            }
        }
    }

    // so we have a valid guid string, convert digits to binary
    // https://code.woboq.org/linux/linux/lib/uuid.c.html
    // we use mixed-endian offsets cause GPT uses mixed endian GUID format
    const UINTN char_offsets[16] = { 6, 4, 2, 0, 11, 9, 16, 14, 19, 21, 24, 26, 28, 30, 32, 34};
    UINT8* ptr = g->data;

    for(UINTN i = 0; i < 16; i++) {
        ptr[i] = __byte_from_hexpair(str + char_offsets[i]);
    }
    
    return EFI_SUCCESS;
}

EFI_STATUS buffer_to_hexstr(IN EFI_SYSTEM_TABLE* st, OUT CHAR16** str, IN UINT8* buff, IN UINTN n) {
    // buffer of n bytes requires n * 2 characters, +1 for NULL char at the end
    UINTN hexstrlen = n * 2;
    EFI_STATUS status = efi_malloc(st, (hexstrlen + 1) * sizeof(CHAR16), (VOID**) str);

    if(status != EFI_SUCCESS) {
        printline(st, L"Error allocating memory while converting a buffer to a string");

        return status;
    }

    CHAR16* ptr = *str; // output string addr

    UINTN i = 0; // byte counter in input
    while(i < n) {
        __hexpair_from_byte(buff[i], ptr + (i * 2));
        i++;
    }

    ptr[hexstrlen] = L'\0';

    return EFI_SUCCESS;
}
