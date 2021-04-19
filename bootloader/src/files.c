#include <Uefi.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>

#include "files.h"
#include "util.h"
#include "print.h"

// GUID structs
EFI_GUID gEfiLoadedImageProtocolGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
EFI_GUID gEfiSimpleFileSystemGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
EFI_GUID gEfiFileInfoGuid = EFI_FILE_INFO_ID;

EFI_STATUS _get_file_size(IN EFI_SYSTEM_TABLE* st, IN EFI_FILE_PROTOCOL* file, OUT UINT64* file_size) {
    EFI_STATUS status;
    EFI_BOOT_SERVICES* bs = st->BootServices;

    // First try to GetInfo without a buffer, should fill buf_size with required size
    UINTN buf_size = 0;
    CHAR8* buffer;
    status = file->GetInfo(
            file,
            &gEfiFileInfoGuid,
            &buf_size,
            NULL
    );

    if(status != EFI_BUFFER_TOO_SMALL || buf_size == 0) {
        printline(st, L"ERROR while retrieving file info buffer size");
        return status;
    }

    // Allocate enough memory
    status = efi_malloc(st, buf_size, (void**) &buffer);

    if(status != EFI_SUCCESS) {
        printline(st, L"ERROR while allocating file info structure");
        return status;
    }

    // Second call with buffer ready should fill i nthe buffer with fileinfo structure
    status = file->GetInfo(
            file,
            &gEfiFileInfoGuid,
            &buf_size,
            buffer
    );

    if(status != EFI_SUCCESS) {
        printline(st, L"ERROR while retrieving file info");
        return status;
    }

    *file_size = ((EFI_FILE_INFO*) buffer)->FileSize;

    // Free the FILE_INFO structure
    status = efi_free(st, buffer);

    if(status != EFI_SUCCESS) {
        printline(st, L"ERROR while freeing the file info structure");
        return status;
    }

    return EFI_SUCCESS;
}

EFI_STATUS open_xdosr_dir(IN EFI_HANDLE image_handle, IN EFI_SYSTEM_TABLE* st, OUT EFI_FILE_PROTOCOL** xdosr_dir_proto_ret) {
    EFI_STATUS status;
    EFI_BOOT_SERVICES* bs = st->BootServices;

    EFI_LOADED_IMAGE_PROTOCOL* loaded_image_proto;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* efi_fs_proto;
    EFI_FILE_PROTOCOL* root_dir_proto;
    EFI_FILE_PROTOCOL* xdosr_dir_proto;

    CHAR16* xdosr_path = L"EFI\\XDOSR";

    // Load information about loaded image, DeviceHandle attribute is of interest for the EFI_SIMPLE_FILESYSTEM_PROTOCOL
    // Section 9.1 in UEFI spec
    status = bs->OpenProtocol(
            image_handle,
            &gEfiLoadedImageProtocolGuid,
            (void**) &loaded_image_proto,
            image_handle,
            NULL,
            EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
    );

    if (status != EFI_SUCCESS) {
        printline(st, L"ERROR while retrieving loaded image protocol");
        return status;
    }

    // Open the device from which the image was loaded from (EFI partition) for file access
    // Section 13.4 in UEFI spec
    status = bs->OpenProtocol(
            loaded_image_proto->DeviceHandle,
            &gEfiSimpleFileSystemGuid,
            (void**) &efi_fs_proto,
            image_handle,
            NULL,
            EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
    );

    if (status != EFI_SUCCESS) {
        printline(st, L"ERROR while retrieving file system access protocol");
        return status;
    }

    // Open file access to the root directory on the EFI partition
    // Section 13.5 in UEFI spec
    status = efi_fs_proto->OpenVolume(
            efi_fs_proto,
            &root_dir_proto
    );

    if (status != EFI_SUCCESS) {
        printline(st, L"ERROR while executing OpenVolume() on EFI partition");
        return status;
    }

    // Open EFI_FILE_PROTOCOL read access to the EFI\XDOSR\ dir where our loader and other files are
    status = root_dir_proto->Open(
            root_dir_proto,
            &xdosr_dir_proto,
            xdosr_path,
            EFI_FILE_MODE_READ,
            0
    );

    if (status != EFI_SUCCESS) {
        printline(st, L"ERROR while executing Open() on the EFI\\XDOSR\\ directory)");
        return status;
    }

    // Return the opened protocol in the OUT argument
    *xdosr_dir_proto_ret = xdosr_dir_proto;

    return EFI_SUCCESS;
}

EFI_STATUS read_file_contents(IN EFI_SYSTEM_TABLE* st, IN EFI_FILE_PROTOCOL* xdosr_dir_proto, IN CHAR16* file_path, OUT CHAR8** buffer_ret, OUT UINT64* buffer_size_ret) {
    EFI_STATUS status;
    EFI_BOOT_SERVICES* bs = st->BootServices;

    EFI_FILE_PROTOCOL* opened_file_proto;

    // Open EFI_FILE_PROTOCOL read access to the file
    status = xdosr_dir_proto->Open(
            xdosr_dir_proto,
            &opened_file_proto,
            file_path,
            EFI_FILE_MODE_READ,
            0
    );

    if (status != EFI_SUCCESS) {
        print(st, L"ERROR while executing Open() on the file: ");
        printline(st, file_path);
        return status;
    }

    UINTN opened_file_size = 0;
    status = _get_file_size(
            st,
            opened_file_proto,
            &opened_file_size
    );

    if (status != EFI_SUCCESS) {
        print(st, L"ERROR while trying to get file size of: ");
        printline(st, file_path);
        return status;
    }

    CHAR8* buffer;
    status = efi_malloc(st, opened_file_size + 2, (void**) &buffer); // allocate 2 more bytes for a NULL string terminator

    if (status != EFI_SUCCESS) {
        print(st, L"ERROR while allocating memory for file: ");
        printline(st, file_path);
        return status;
    }

    status = opened_file_proto->Read(
            opened_file_proto,
            &opened_file_size,
            buffer
    );

    if (status != EFI_SUCCESS) {
        print(st, L"ERROR while reading file into buffer: ");
        printline(st, file_path);
        return status;
    }

    buffer[opened_file_size] = '\0';
    buffer[opened_file_size + 1] = '\0';

    *buffer_ret = buffer;
    if(buffer_size_ret != NULL)
        *buffer_size_ret = opened_file_size + 2;

    return EFI_SUCCESS;
}
