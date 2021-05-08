#ifndef _TEXTIO_H_
#define _TEXTIO_H_

#include <Uefi.h>

void getline_with_echo(EFI_SYSTEM_TABLE* st, CHAR16* input_buffer, UINTN* cmd_len);
void print(EFI_SYSTEM_TABLE* st, IN const CHAR16* str);
void printline(EFI_SYSTEM_TABLE* st, const CHAR16* str);
void clear_screen(EFI_SYSTEM_TABLE* st);

#endif
