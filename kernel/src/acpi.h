#ifndef _ACPI_H_
#define _ACPI_H_

#include <stdint.h>
#include <stddef.h>

// Parsing ACPI tables

// SDT header
struct acpi_sdt_hdr {
    char sig[4];
    uint32_t length;
    uint8_t rev;
    uint8_t chksum;
    char oemid[6];
    char oemtabid[8];
    uint32_t oemrev;
    uint32_t creator_id;
    uint32_t creator_rev;
};
typedef struct acpi_sdt_hdr acpi_sdt_hdr_t;

// extended RSDP
struct acpi_rsdp {
    char sig[8];
    uint8_t chksum;
    char oemid[6];
    uint8_t rev;
    uint32_t rsdt_addr;
    uint32_t length;
    void* xsdt_addr;
    uint8_t x_chksum;
    uint8_t reserved[3];
} __attribute__ ((packed));
typedef struct acpi_rsdp acpi_rsdp_t;

// extended main system table
struct acpi_xsdt {
    acpi_sdt_hdr_t hdr;
    void* table_ptrs[]; // dynamic size
};
typedef struct acpi_xsdt acpi_xsdt_t;

// ACPI tables struct - can be populated and provide easy access to acpi tables
struct acpi_tables {
    acpi_rsdp_t* rsdp;
};
typedef struct acpi_tables acpi_tables_t;

// validate table checksum and signature
// 1 - table ok
// 0 - table not ok
int acpi_table_ok(void* base, size_t len, char* sig, size_t sig_len);

#endif
