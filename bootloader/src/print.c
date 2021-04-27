#include <Uefi.h>

#include "hexconv.h"
#include "util.h"

void print(IN EFI_SYSTEM_TABLE* st, IN const CHAR16* str) {
    st->ConOut->OutputString(st->ConOut, (CHAR16*) str);
}

void printline(IN EFI_SYSTEM_TABLE* st, IN const CHAR16* str) {
    st->ConOut->OutputString(st->ConOut, (CHAR16*) str);
    st->ConOut->OutputString(st->ConOut, L"\r\n");
}

void printbuffer(IN EFI_SYSTEM_TABLE* st, IN const UINT8* buffer, IN UINTN n) {
    CHAR16* str;
    EFI_STATUS status = buffer_to_hexstr(st, &str, buffer, n);

    if(status != EFI_SUCCESS) {
        printline(st, L"Call to buffer_to_hexstr failed");
        return;
    }

    print(st, str);
    efi_free(st, str);
}
