#ifndef _STRING_H_
#define _STRING_H_

// Classic string and memory operations

// Returns length of a string in CHAR16s (2 bytes) with an upper boundary of max_len
UINTN strnlen(IN const CHAR16* str, UINTN max_len);

// Sets memory at s of length n bytes with byte c, returns s
VOID* memset(IN UINT8* s, IN UINT8 c, IN UINTN n);

// Copies memory form src to dst of length n bytes, returns dst
VOID* memcpy(OUT UINT8* dst, IN const UINT8* src, IN UINTN n);

// Compares memory of size n at a and b
// returns less then, equal to or more than zero
// if a is less than, equal or more than b
INTN memcmp(IN const UINT8* a, IN const UINT8* b, IN UINTN n);

#endif
