#ifndef _PARSER_H_
#define _PARSER_H_

#include <Uefi.h>

#include "gpt_guid.h"

// Config file parser

#define CONFIG_ENTRY_MAX_STR_LENGTH 63

// This structure defines all the possible options for the config file
struct _config {
    CHAR16 kernel_path[CONFIG_ENTRY_MAX_STR_LENGTH + 1];

    GPT_GUID disk_guid;
    
    GPT_GUID part_guid;
};

typedef struct _config CONFIG;

// Tries to parse the config file contents
EFI_STATUS parse_config_file(IN EFI_SYSTEM_TABLE* st, IN const CHAR16* file_contents, OUT CONFIG* cfg);


#endif
