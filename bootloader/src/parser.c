#include <Uefi.h>

#include "parser.h"

#include "string.h"
#include "print.h"

#include "gpt_guid.h"
#include "hexconv.h"

struct _config_entry {
    CHAR16 key[CONFIG_ENTRY_MAX_STR_LENGTH + 1];
    CHAR16 value[CONFIG_ENTRY_MAX_STR_LENGTH + 1];
};
typedef struct _config_entry CONFIG_ENTRY;

#define IS_WHITESPACE(c) (c == L'\t' || c == L' ')

// This function uses labels and gotos because it parses the lines using a FSM of sorts,
// and that is a reasonable use case for gotos
UINTN __get_next_config_entry(IN EFI_SYSTEM_TABLE* st, IN const CHAR16** cursor_ptr, OUT CONFIG_ENTRY* cfg_entry) {
    const CHAR16* cursor = *cursor_ptr;

    UINTN i = 0; // arbitrary counter

    // we start at the beginning of a new line or ending of the last line
    // or sometimes at the end of a file, if it does not end with an empty line

    looking_for_a_key:
        if(cursor[0] == L'\0') { // if it's null character we return
            return 0;
        }

        if(IS_WHITESPACE(cursor[0])) { // if it's whitespace we skip a char
            cursor++;
            goto looking_for_a_key;
        }

        if(cursor[0] == L'\n') { // if it's a lone LF return
            printline(st, L"ERROR new line: line feed withotu carriage return");
            return 0;
        }

        if(cursor[0] == L'\r') { // if it's CR we have an empty line, go to the next one
            if(cursor[1] == L'\n') {
                cursor++; // skip \r
                cursor++; // skip \n
                goto looking_for_a_key;
            }

            // else
            printline(st, L"ERROR new line: carriage return without line feed");
            return 0;
        }

        // in other cases, we have found a start of a key probably
        i = 0;
        goto reading_the_key;

    reading_the_key:
        if(cursor[0] == L'\0') { // if it's null character we return
            printline(st, L"ERROR end of file: expected a character or whitespace");
            return 0;
        }

        if(IS_WHITESPACE(cursor[0])) { // if it's whitespace we have finished the key, find value
            cfg_entry->key[i] = L'\0';
            goto looking_for_a_value;
        }

        if(cursor[0] == L'\r' || cursor[0] == L'\n') { // if it's new line we return
            printline(st, L"ERROR end of line: expected a character or whitespace");
            return 0;
        }

        // in other cases, it's another key character, append it if it's not too much
        if(i >= CONFIG_ENTRY_MAX_STR_LENGTH) {
            printline(st, L"ERROR token length: maximum key length exceeded");
            return 0;
        }

        cfg_entry->key[i] = cursor[0];
        i++;
        cursor++;

        goto reading_the_key;

    looking_for_a_value:
        if(cursor[0] == L'\0') { // if it's null character we return
            printline(st, L"ERROR end of file: expected value");
            return 0;
        }

        if(IS_WHITESPACE(cursor[0])) { // if it's whitespace we skip a char
            cursor++;
            goto looking_for_a_value;
        }

        if(cursor[0] == L'\r' || cursor[0] == L'\n') { // if it's new line we return
            printline(st, L"ERROR end of line: expected a character or whitespace");
            return 0;
        }

        // in other case, beginning of the value
        i = 0;
        goto reading_the_value;

    reading_the_value:
        if(cursor[0] == L'\0') { // if it's null character we return
            cfg_entry->value[i] = L'\0';
            *cursor_ptr = cursor;
            return 1;
        }

        if(IS_WHITESPACE(cursor[0])) { // if it's whitespace, skip the char
            goto reading_the_value;
        }

        if(cursor[0] == L'\n') {
            printline(st, L"WARNING end of line: a lone line feed");
            cfg_entry->value[i] = L'\0';
            *cursor_ptr = cursor + 1;
            return 1;
        }

        if(cursor[0] == L'\r') { // if it's new line we return
            cfg_entry->value[i] = L'\0';
            if(cursor[1] == L'\n') {
                *cursor_ptr = cursor + 2;
                return 1;
            }

            printline(st, L"WARNING end of line: a lone carriage return");
            *cursor_ptr = cursor + 1;
            return 1;
        }

        // in other cases, it's another value character, append it if it's not too much
        if(i >= CONFIG_ENTRY_MAX_STR_LENGTH) {
            printline(st, L"ERROR token length: maximum value length exceeded");
            return 0;
        }

        cfg_entry->value[i] = cursor[0];
        i++;
        cursor++;

        goto reading_the_value;

}


EFI_STATUS parse_config_file(IN EFI_SYSTEM_TABLE* st, IN const CHAR16* file_contents, OUT CONFIG* cfg) {
    const CHAR16* cursor = file_contents;
    EFI_STATUS status;

    // handle BOM if present
    if(cursor[0] == 0xfeff)
        cursor++;

    if(cursor[0] == 0xfffe) {
        printline(st, L"ERROR wrong BOM: this probably means file is UTF16 Big endian, aborting");
        return EFI_INVALID_PARAMETER;
    }

    BOOLEAN kernel_path_set = 0, disk_guid_set = 0, part_guid_set = 0;

    CONFIG_ENTRY cfg_entry = { 0 };

    UINTN result;
    while(1) {
        result = __get_next_config_entry(st, &cursor, &cfg_entry);

        if(result > 0) {
            if(strncmp(cfg_entry.key, L"kernel_path", CONFIG_ENTRY_MAX_STR_LENGTH) == 0) {
                memcpy((UINT8*) cfg->kernel_path, (UINT8*) cfg_entry.value, CONFIG_ENTRY_MAX_STR_LENGTH + 1);
                kernel_path_set = 1;
            } else if(strncmp(cfg_entry.key, L"disk_guid", CONFIG_ENTRY_MAX_STR_LENGTH) == 0) {
                GPT_GUID guid = { 0 };

                status = guid_from_str(st, &guid, cfg_entry.value);

                if(status != EFI_SUCCESS) {
                    printline(st, L"ERROR parsing disk_guid: invalid GUID string");
                    return status;
                }

                memcpy((UINT8*) &(cfg->disk_guid), (UINT8*) &guid, sizeof(GPT_GUID));
                disk_guid_set = 1;
            } else if(strncmp(cfg_entry.key, L"part_guid", CONFIG_ENTRY_MAX_STR_LENGTH) == 0) {
                GPT_GUID guid = { 0 };

                status = guid_from_str(st, &guid, cfg_entry.value);

                if(status != EFI_SUCCESS) {
                    printline(st, L"ERROR parsing part_guid: invalid GUID string");
                    return status;
                }

                memcpy((UINT8*) &(cfg->part_guid), (UINT8*) &guid, sizeof(GPT_GUID));
                part_guid_set = 1;
            } else {
                print(st, L"WARNING unknown option: ");
                printline(st, cfg_entry.key);
                printbuffer(st, (UINT8*) cfg_entry.key, CONFIG_ENTRY_MAX_STR_LENGTH + 1);
                printline(st, L" < the key");
            }
        } else break;
    }

    if(kernel_path_set == 0) {
        printline(st, L"ERROR missing setting: kernel_path");
        return EFI_INVALID_PARAMETER;
    }

    if(disk_guid_set == 0) {
        printline(st, L"ERROR missing setting: disk_guid");
        return EFI_INVALID_PARAMETER;
    }

    if(part_guid_set == 0) {
        printline(st, L"ERROR missing setting: part_guid");
        return EFI_INVALID_PARAMETER;
    }

    return EFI_SUCCESS;
}
