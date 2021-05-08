#include "bootinfo.h"
#include "textio.h"
#include "term.h"

int kmain(boot_info_t* binfo) {
    EFI_SYSTEM_TABLE* st = binfo->st;

    clear_screen(st);
    printline(st, u"xdosr kernel loaded succesfully");
    printline(st, u"type 'help' for command list");

    terminal_run(st);

    return 0x1234;
}
