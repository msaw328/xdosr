#include <Uefi.h>

#include "hexconv.h"

#include "string.h"
#include "util.h"
#include "textio.h"

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
UINT8 __byte_from_hexpair(IN const CHAR16* hexpair) {
    return (__val_from_digit(hexpair[0]) << 4) | __val_from_digit(hexpair[1]);
}

EFI_STATUS buffer_to_hexstr(IN EFI_SYSTEM_TABLE* st, OUT CHAR16** str, IN const UINT8* buff, IN UINTN n) {
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

EFI_STATUS pointer_to_hexstr(IN EFI_SYSTEM_TABLE* st, OUT CHAR16** str, IN const VOID* in_ptr1) {
    UINT64 in_ptr = (UINT64) in_ptr1;
    UINTN n = sizeof(in_ptr);
    UINTN hexstrlen = n * 2 + 2; // +2 for "0x"
    EFI_STATUS status = efi_malloc(st, (hexstrlen + 1) * sizeof(CHAR16), (VOID**) str);

    if(status != EFI_SUCCESS) {
        printline(st, L"Error allocating memory while converting a pointer to a string");

        return status;
    }

    CHAR16* ptr = *str; // output string addr

    ptr[0] = L'0';
    ptr[1] = L'x';

    CHAR16* ptr_end = ptr + (n * 2); // theres +2 for "0x" but -2 cause we wanna point at last element

    UINTN i = 0;
    while(i < n) {
        UINT8 lsb = (UINT8) (in_ptr & 0xff);
        __hexpair_from_byte(lsb, ptr_end);
        ptr_end -= 2;
        in_ptr >>= 8;
        i++;
    }

    ptr[hexstrlen] = L'\0';
    return EFI_SUCCESS;
}
