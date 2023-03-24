//Copyright (C) 2023  Marius Alexandersen <marius_kule@hotmail.com>
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see https://www.gnu.org/licenses/

#include <stdio.h>
#include <getopt.h>
#include <unistd.h>

#define SB_IMPLEMENTATION
#include "sb/sb.h"

#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define CLEAR "\033[0m"

int main(int argc,char** argv) {

    //Build string to filepath
    const char *home = getenv("HOME");
    if (home == NULL) {
        fprintf(stderr, "Cannot find the user's home folder.\n");
        return EXIT_FAILURE;
    }
    String_Builder* filepath = sb_create(strlen(home)+1);
    sb_append(filepath,home);
    sb_append(filepath,"/.config/reminders");

    //Check if file exists, if not create it
    String_Builder* sb = access(sb_get_string(filepath), F_OK) == 0 ? sb_create_from_file(sb_get_string(filepath)) : sb_create(0);

    //Go through all the command line arguments
    for (int opt = getopt(argc,argv,":a:d:hf"); opt != -1; opt = getopt(argc,argv,":a:d:hf")) {
        switch(opt) {
            case 'a': {
                // TODO: Add a sb_concat function to sort this mess...
                String_Builder* add = sb_create(4);
                sb_append(add,"\t- ");
                sb_append(add,optarg);
                sb_append(add,"\n");
                sb_append(sb,sb_get_string(add));
                sb_destroy(add);
                sb_save_to_file(sb_get_string(filepath),sb);
                break;
            }
            case 'd': {
                size_t index = strtoul(optarg, NULL, 10);
                sb_delete_line(sb,index);
                //printf(SB_Fmt,SB_Arg(sb));
                sb_save_to_file(sb_get_string(filepath),sb);

                return EXIT_SUCCESS;
                break;
            }
            case 'f': {
                printf("====== Current Reminders ======\n");
                printf(SB_Fmt,SB_Arg(sb));
                break;
            }
            case ':':
            case '?':
            case 'h': {
                printf("usage:\treminder <operation> [...]\noperations:\n\treminder -f no formatting\n\treminder -a <string>\n\treminder -d <index>\n\treminder -h for help\n\n use reminder to see reminders\n");
                break;
            }
            default: {
                printf("Invalid command: %c -> \"%s\"\n",(char)opt,optarg);
                break;
            }
        }
    }

    //If no arguments 
    if(argc == 1) {
        printf(GREEN"====== Current Reminders ======\n"CLEAR);
        printf(YELLOW SB_Fmt CLEAR,SB_Arg(sb));
    }
    sb_destroy(filepath);
    sb_destroy(sb);

    return EXIT_SUCCESS;
}