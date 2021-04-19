#ifndef _PRINT_H_
#define _PRINT_H_

#include <Uefi.h>

// Printing various data to stdout, sometimes with formatting

// Print a string to ConOut using OutputString
void print(IN EFI_SYSTEM_TABLE* st, IN CHAR16* str);

// Same as print but with a newline \r\n
void printline(IN EFI_SYSTEM_TABLE* st, IN CHAR16* str);

// Prints a buffer of n bytes to the screen in hexadecimal format
void printbuffer(IN EFI_SYSTEM_TABLE* st, IN UINT8* buffer, IN UINTN n);

#endif
