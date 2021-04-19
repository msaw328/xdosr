#include <Uefi.h>
#include <Protocol/SimpleFileSystem.h>

#include "util.h"
#include "files.h"
#include "string.h"
#include "hexconv.h"
#include "print.h"

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

        efi_free(st, file_contents);

        GPT_GUID g;
        CHAR16* guid_str = L"fc93ab0e-c99e-4b58-975e-9c5e68c53624";
        print(st, L"PARSING GUID STRING: ");
        printline(st, guid_str);
        status = guid_from_str(st, &g, guid_str);

        if(status != EFI_SUCCESS) {
            printline(st, L"ERROR while converting GUID string");
            return status;
        }

        const UINT8 expected_guid[16] = { 0x0e, 0xab, 0x93, 0xfc, 0x9e, 0xc9, 0x58, 0x4b, 0x97, 0x5e, 0x9c, 0x5e, 0x68, 0xc5, 0x36, 0x24 };

        if(memcmp(g.data, expected_guid, 16) == 0) {
            printline(st, L"It's ok");
        }

        print(st, L"GUID struct contents after conversion: ");
        printbuffer(st, g.data, 16);
        printline(st, L"");

        return EFI_SUCCESS;
}
