#include <Uefi.h>

#include "term.h"
#include "textio.h"

void cmd_help(EFI_SYSTEM_TABLE* st, CHAR16* cmd, UINTN cmd_len) {
    printline(st,   u"Available commands:\r\n" \
                    u"  help - displays this help text\r\n" \
                    u"  echo - displays a string on the screen\r\n" \
                    u"  clear - clears screen\r\n" \
                    u"  shutdown - powers off the machine\r\n");
}

void cmd_echo(EFI_SYSTEM_TABLE* st, CHAR16* cmd, UINTN cmd_len) {
    while(*cmd == u' ' || *cmd == u'\t') cmd++; // skip beginning whitespace

    cmd += 4; // skip "echo"

    while(*cmd == u' ' || *cmd == u'\t') cmd++; // skip whitespace before 1st arg

    printline(st, cmd); // print whatever follows
}

void cmd_clear(EFI_SYSTEM_TABLE* st, CHAR16* cmd, UINTN cmd_len) {
    clear_screen(st);
}
void cmd_shutdown(EFI_SYSTEM_TABLE* st, CHAR16* cmd, UINTN cmd_len) {
    st->RuntimeServices->ResetSystem(
        EfiResetShutdown,
        EFI_SUCCESS,
        0, NULL
    );
}

// check if string starts with another string
int is_cmd(CHAR16* str, CHAR16* cmd, UINTN cmd_len) {
    while(*str == u' ' || *str == u'\t') str++;

    for(int iter = 0; iter < cmd_len; iter++) {
        if(str[iter] != cmd[iter]) return 0;
    }

    return str[cmd_len] == u'\0' || str[cmd_len] == u' ' || str[cmd_len] == u'\t';
}

void print_prompt(EFI_SYSTEM_TABLE* st) {
    print(st, u"[xdosr-cli] >> ");
}

void terminal_run(EFI_SYSTEM_TABLE* st) {
    CHAR16 cmd_buffer[64];
    UINTN cmd_buffer_len = 0;

    while(1) {
        print_prompt(st);

        getline_with_echo(st, cmd_buffer, &cmd_buffer_len);

        if(is_cmd(cmd_buffer, u"help", 4) != 0) {
            cmd_help(st, cmd_buffer, cmd_buffer_len);
        } else if(is_cmd(cmd_buffer, u"clear", 5) != 0) {
            cmd_clear(st, cmd_buffer, cmd_buffer_len);
        } else if(is_cmd(cmd_buffer, u"echo", 4) != 0) {
            cmd_echo(st, cmd_buffer, cmd_buffer_len);
        } else if(is_cmd(cmd_buffer, u"shutdown", 8) != 0) {
            cmd_shutdown(st, cmd_buffer, cmd_buffer_len);
        } else {
            printline(st, u"Command not recognized");
        }
    }
}
