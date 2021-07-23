#ifndef _ACPI_H_
#define _ACPI_H_

#include <Uefi.h>

// locating of ACPI tables using the EFI_SYSTEM_TABLE

// Finds the pointer to the RSDP, which is the base of the ACPi table tree thingy
VOID* acpi_get_rsdp_ptr(IN EFI_SYSTEM_TABLE*);

#endif
