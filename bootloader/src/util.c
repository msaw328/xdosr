#include <Uefi.h>

void print(IN EFI_SYSTEM_TABLE* st, IN CHAR16* str) {
    st->ConOut->OutputString(st->ConOut, str);
}

void printline(IN EFI_SYSTEM_TABLE* st, IN CHAR16* str) {
    st->ConOut->OutputString(st->ConOut, str);
    st->ConOut->OutputString(st->ConOut, L"\r\n");
}

EFI_STATUS efi_malloc(IN EFI_SYSTEM_TABLE* st, IN UINTN size, OUT VOID** buffer_ret) {
    VOID* buffer;
    EFI_STATUS status = st->BootServices->AllocatePool(
            EfiLoaderData,
            size,
            &buffer
    );

    *buffer_ret = buffer;

    return status;
}

EFI_STATUS efi_free(IN EFI_SYSTEM_TABLE* st, IN VOID* buffer) {
    EFI_STATUS status = st->BootServices->FreePool(
            buffer
    );

    return status;
}
