#include "acpi.h"

#include <stdint.h>
#include <stddef.h>

#include "string.h"

int acpi_table_ok(void* base, size_t len, char* sig, size_t sig_len) {
    if(memcmp(base, (const UINT8*) sig, sig_len) != 0) return 0;

    uint8_t b;
    for(int i = 0; i < len; i++) {
        b += ((uint8_t*) base)[i];
    }

    return b == 0;
}
