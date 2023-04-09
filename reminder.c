// Copyright (C) 2023  Marius Alexandersen <marius_kule@hotmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see https://www.gnu.org/licenses/

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#define SB_IMPLEMENTATION
#include "sb/sb.h"

#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define CLEAR "\033[0m"

#define FLAG_ARGS ":t:ad:i:hf"

typedef struct {
    bool append;
    bool delete;
    bool insert;
    bool text;
} Flags;

void usage() {
    printf( "usage:\treminder <operation> [...]\n"
            "operations:"
            "\n\treminder -f \t\tno colors"
            "\n\treminder -a \t\tappend flag"
            "\n\treminder -i <index>\tinsert flag"
            "\n\treminder -d <index>\tdelete flag"
            "\n\treminder -h \t\tfor help"
            "\n\n use reminder to see reminders\n");
}

int main(int argc, char **argv) {

    // Flags init
    Flags flags = {false};

    // Build string to filepath
    const char *home = getenv("HOME");
    if (home == NULL) {
        fprintf(stderr, "Cannot find the user's home folder.\n");
        return EXIT_FAILURE;
    }
    String_Builder *filepath = sb_create(strlen(home) + 1);
    sb_append(filepath, home);
    sb_append(filepath, "/.config/reminders");
    // Check if file exists, if not create it
    String_Builder *sb = access(sb_get_string(filepath), F_OK) == 0 ? sb_create_from_file(sb_get_string(filepath)) : sb_create(0);

    // Buffer for text
    String_Builder *arg_text = sb_create(64);
    size_t arg_index = 0;

    // Go through all the command line arguments
    for (int opt = getopt(argc, argv, FLAG_ARGS); opt != -1; opt = getopt(argc, argv, FLAG_ARGS)) {
        switch (opt) {
        case 'a': {
        flags.append = true;
        break;
        }
        case 'd': {
        arg_index = strtoul(optarg, NULL, 10);
        flags.delete = true;
        break;
        }
        case 'i': {
        arg_index = strtoul(optarg, NULL, 10);
        flags.insert = true;
        break;
        }
        case 't': {
        sb_append(arg_text, optarg);
        flags.text = true;
        break;
        }
        case 'f': {
        printf("====== Current Reminders ======\n");
        printf(SB_Fmt, SB_Arg(sb));
        break;
        }
        case ':':
        case '?':
        case 'h': {
        usage();
        break;
        }
        default: {
        printf("Invalid command: %c -> \"%s\"\n", (char)opt, optarg);
        break;
        }
        }
    }

    if (flags.append && flags.text) {
        sb_append_format(sb, "\t- %s\n", sb_get_string(arg_text));
        sb_save_to_file(sb, sb_get_string(filepath));
        goto cleanup;
    }

    if (flags.delete) {
        sb_delete_line(sb, arg_index);
        sb_save_to_file(sb, sb_get_string(filepath));
        goto cleanup;
    }

    if (flags.insert && flags.text) {
        sb_insert_line_format(sb, arg_index, "\t- %s", sb_get_string(arg_text));
        sb_save_to_file(sb, sb_get_string(filepath));
        goto cleanup;
    }

    // If no arguments
    if (argc == 1) {
        printf(GREEN "====== Current Reminders ======\n" CLEAR);
        printf(YELLOW SB_Fmt CLEAR, SB_Arg(sb));
    }

    cleanup:
    sb_destroy(arg_text);
    sb_destroy(filepath);
    sb_destroy(sb);

    return EXIT_SUCCESS;
}
