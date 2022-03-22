#include <Uefi.h>

#include "bootinfo.h"
#include "hexconv.h"
#include "textio.h"
#include "util.h"

void clear_screen(EFI_SYSTEM_TABLE* st) {
    st->ConOut->ClearScreen(st->ConOut);
}

void print(EFI_SYSTEM_TABLE* st, IN const CHAR16* str) {
    st->ConOut->OutputString(st->ConOut, (CHAR16*) str);
}

void printline(EFI_SYSTEM_TABLE* st, const CHAR16* str) {
    st->ConOut->OutputString(st->ConOut, (CHAR16*) str);
    st->ConOut->OutputString(st->ConOut, u"\r\n");
}

void readkey(EFI_SYSTEM_TABLE* st, EFI_INPUT_KEY* key) {
    EFI_EVENT ev = st->ConIn->WaitForKey;
    unsigned long long idx;
    st->BootServices->WaitForEvent(1, &ev, &idx);
    st->ConIn->ReadKeyStroke(st->ConIn, key);
}

void backspace(EFI_SYSTEM_TABLE* st) {
    st->ConOut->SetCursorPosition(
        st->ConOut,
        st->ConOut->Mode->CursorColumn - 1,
        st->ConOut->Mode->CursorRow
        );

    print(st, u" ");

    st->ConOut->SetCursorPosition(
        st->ConOut,
        st->ConOut->Mode->CursorColumn - 1,
        st->ConOut->Mode->CursorRow
        );
}

void getline_with_echo(EFI_SYSTEM_TABLE* st, CHAR16* input_buffer, UINTN* cmd_len) {
    CHAR16 linebuffer[64];
    UINTN linebuffer_idx = 0;

    for(int i = 0; i < 64; i++) {
        linebuffer[i] = u'\0';
    }

    EFI_INPUT_KEY last_key;

    int not_newline = 1;

    while(not_newline) {
        readkey(st, &last_key);
        if(last_key.UnicodeChar == 0x8) { // backspace
            if(linebuffer_idx > 0) {
                linebuffer_idx--;

                linebuffer[linebuffer_idx] = u'\0';
                
                backspace(st);
            }
        } else if(last_key.UnicodeChar == 0xd) { // enter
            not_newline = 0;
        } else if(last_key.UnicodeChar != u'\0') { // omit unprintable
            if(linebuffer_idx < 63) {
                linebuffer[linebuffer_idx] = last_key.UnicodeChar;

                linebuffer_idx++;

                CHAR16 buf[2];
                buf[0] = last_key.UnicodeChar;
                buf[1] = u'\0';

                print(st, buf);
            }
        }
    }

    printline(st, u"");

    for(UINTN iter = 0; iter <= linebuffer_idx; iter++) {
        input_buffer[iter] = linebuffer[iter];
    }

    *cmd_len = linebuffer_idx;
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

void printpointer(IN EFI_SYSTEM_TABLE* st, IN const VOID* ptr) {
    CHAR16* str;
    EFI_STATUS status = pointer_to_hexstr(st, &str, ptr);

    if(status != EFI_SUCCESS) {
        printline(st, L"Call to pointer_to_hexstr failed");
        return;
    }

    print(st, str);
    efi_free(st, str);
}
