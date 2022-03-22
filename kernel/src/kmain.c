#include "bootinfo.h"
#include "textio.h"
#include "term.h"
#include "acpi.h"

int kmain(boot_info_t* binfo) {
    EFI_SYSTEM_TABLE* st = binfo->st;

    clear_screen(st);

    printline(st, u"xdosr kernel loaded succesfully");

    acpi_rsdp_t* rsdp_ptr = binfo->acpi_rsdp;
    int status = acpi_table_ok(rsdp_ptr, rsdp_ptr->length, "RSD PTR ", 8);

    if(status) {
        print(st, u"RSDP ok, XSDT address: ");
        printpointer(st, rsdp_ptr->xsdt_addr);
        printline(st, u"");
    } else {
        printline(st, u"error: bad RSDP");
    }
    printline(st, u"type 'help' for command list");

    terminal_run(st);

    return 0x1234;
}
