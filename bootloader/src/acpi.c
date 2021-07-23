#include <Uefi.h>
#include <Guid/Acpi.h>

#include "acpi.h"
#include "string.h"

EFI_GUID gEfiAcpiTableGuid = EFI_ACPI_TABLE_GUID;

VOID* acpi_get_rsdp_ptr(IN EFI_SYSTEM_TABLE* st) {
    UINTN num = st->NumberOfTableEntries;
    EFI_CONFIGURATION_TABLE* cfg = st->ConfigurationTable;

    for(int i = 0; i < num; i++) {
        // Check if GUID is ok
        if(0 == memcmp((const UINT8*) &cfg[i].VendorGuid, (const UINT8*) &gEfiAcpiTableGuid, sizeof(EFI_GUID))) {
            VOID* ptr = cfg[i].VendorTable;

            // check if ptr isnt somehow null and also check signature
            if(ptr != NULL && 0 == memcmp((const UINT8*) ptr, (const UINT8*) "RSD PTR ", 8))
                return ptr;
        }
    }

    return NULL;
}
