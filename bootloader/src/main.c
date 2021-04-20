#include <Uefi.h>
#include <Protocol/SimpleFileSystem.h>

#include "util.h"
#include "files.h"
#include "string.h"
#include "hexconv.h"
#include "print.h"
#include "parser.h"

EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* st) {
        EFI_STATUS status;
        EFI_FILE_PROTOCOL* xdosr_dir;

        status = open_xdosr_dir(image_handle, st, &xdosr_dir);

        if(status != EFI_SUCCESS) {
            printline(st, L"ERROR while opening xdosr directory");
            return status;
        }

        CHAR16* file_contents;
        CHAR16* file_path = L"XDOSR.CFG";
        status = read_file_contents(st, xdosr_dir, file_path, (CHAR8**) &file_contents, NULL);

        if(status != EFI_SUCCESS) {
            print(st, L"ERROR while reading file contents: ");
            printline(st, file_path);
            return status;
        }

        printline(st, L"XDOSR.CFG file contents:");
        printline(st, file_contents);

        CONFIG cfg = { 0 };

        status = parse_config_file(st, file_contents, &cfg);

        if(status != EFI_SUCCESS) {
            printline(st, L"ERROR while parsing config file");
            return status;
        }

        print(st, L"kernel_path: ");
        printline(st, cfg.kernel_path);
        printbuffer(st, cfg.disk_guid.data, 16);
        printline(st, L"");
        printbuffer(st, cfg.part_guid.data, 16);
        printline(st, L"");

        return EFI_SUCCESS;
}
