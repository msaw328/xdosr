#ifndef _FILES_H_
#define _FILES_H_

#include <Uefi.h>
#include <Protocol/SimpleFileSystem.h>

// This header offers EFI_FILE_PROTOCOL access to the xdosr vendor dir on the EFI partition and files inside of it

// opens the EFI\XDOSR\ directory on the device that image was loaded from
EFI_STATUS open_xdosr_dir(
    IN EFI_HANDLE image_handle,
    IN EFI_SYSTEM_TABLE* st,
    OUT EFI_FILE_PROTOCOL** xdosr_dir_proto_ret
    );

// reads a file relative to the EFI\XDOSR\ dir into a buffer allocated using AllocatePool()
// the buffer has 2 NULL bytes at the end in case its a text file so it can easily be printed
// buffer_size_ret may also be NULL if one does not care about the size of the file
EFI_STATUS read_file_contents(
    IN EFI_SYSTEM_TABLE* st,
    IN EFI_FILE_PROTOCOL* xdosr_dir_proto,
    IN CHAR16* file_path,
    OUT CHAR8** buffer,
    OUT UINT64* buffer_size_ret
    );

#endif
