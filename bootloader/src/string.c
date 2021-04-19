#include <Uefi.h>

#include "string.h"

UINTN strnlen(IN const CHAR16* str, UINTN max_len) {
    UINTN counter = 0;

    while(counter < max_len && str[counter] != L'\0') {
        counter++;
    }

    return counter;
}

VOID* memset(IN UINT8* s, IN UINT8 c, IN UINTN n) {
    UINTN i = 0;

    while(i < n) {
        s[i] = c;
        i++;
    }

    return s;
}

VOID* memcpy(OUT UINT8* dst, IN const UINT8* src, IN UINTN n) {
    UINTN i = 0;

    while(i < n) {
        dst[i] = src[i];
        i++;
    }

    return dst;
}

INTN memcmp(IN const UINT8* a, IN const UINT8* b, IN UINTN n) {
    UINTN i = 0;

    while(i < n) {
        if(a[i] > b[i]) return 1;

        if(a[i] < b[i]) return -1;

        i++;
    }

    return 0;
}
