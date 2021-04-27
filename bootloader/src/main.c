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

        printline(st, L"XDOSR Bootloader begin");

        status = open_xdosr_dir(image_handle, st, &xdosr_dir);

        if(status != EFI_SUCCESS) {
            printline(st, L"ERROR while opening xdosr directory");
            return status;
        }

        printline(st, L"XDOSR directory opened");

        CHAR16* config_file_contents;
        CHAR16* config_file_path = L"XDOSR.CFG";
        status = read_file_contents(st, xdosr_dir, config_file_path, (CHAR8**) &config_file_contents, NULL);

        if(status != EFI_SUCCESS) {
            print(st, L"ERROR while reading file contents: ");
            printline(st, config_file_path);
            return status;
        }

        printline(st, L"XDOSR.CFG file read");

        CONFIG cfg = { 0 };

        status = parse_config_file(st, config_file_contents, &cfg);

        if(status != EFI_SUCCESS) {
            printline(st, L"ERROR while parsing config file");
            efi_free(st, config_file_contents);
            return status;
        }

        efi_free(st, config_file_contents);

        // Some printing, probably remove after bootloader works
        printline(st, L"XDOSR.CFG file parsed");
        printline(st, L"");

        printline(st, L"Parsed configuration:");
        print(st, L"cfg.kernel_path: ");
        printline(st, cfg.kernel_path);

        print(st, L"cfg.disk_guid: ");
        printbuffer(st, cfg.disk_guid.data, 16);
        printline(st, L"");

        print(st, L"cfg.part_guid: ");
        printbuffer(st, cfg.part_guid.data, 16);
        printline(st, L"");
        printline(st, L"");

        UINT8* kernel_image_contents;
        UINT64 kernel_image_size;
        status = read_file_contents(st, xdosr_dir, cfg.kernel_path, (CHAR8**) &kernel_image_contents, &kernel_image_size);
    
        if(status != EFI_SUCCESS) {
            print(st, L"ERROR while reading kernel image: ");
            printline(st, cfg.kernel_path);
            return status;
        }

        printline(st, L"Kernel image read");

        return EFI_SUCCESS;
}
