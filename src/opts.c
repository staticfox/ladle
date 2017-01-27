/*
 *   opts.c -- Ladle - Chef Bootstrapper
 *   Copyright 2017 - Matt Ullman <staticfox@staticfox.net>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ladle/ladle.h>

static void
show_help(const char *const program)
{
    printf("Ladle - Chef Cookbook Bootstrapper\n");
    printf("Compiled "__DATE__", "__TIME__"\n");
    printf("\n");
    printf("%s [options]\n", program);
    printf("-h|--help        Shows this dialog\n");
    printf("-d|--directory   The directory to create the cookbook\n");
    printf("-v|--verbose     Enable verbose output\n");
    printf("-vv|--verboser   Enables more debugging\n");
}

/* Right now verbose level is maxed at 2 */
void
ladle_getops(int argc, char **argv)
{
    const char *const command = argv[0];
    const char *const unknown = "Unknown command line arguments: ";
    char buf[512];
    int bufpos;

    bufpos += snprintf(buf + bufpos, sizeof(buf) - bufpos, "%s", unknown);

    if (argc < 2) {
        show_help(command);
        exit(EXIT_SUCCESS);
    }

    for (size_t ii = 1; argv[ii]; ii++) {
        if (strcmp(argv[ii], "--help") == 0) {
            show_help(command);
            exit(EXIT_SUCCESS);
        }

        if (strcmp(argv[ii], "--verbose") == 0) {
            if (options.verbose <= 1)
                options.verbose = 1;
            continue;
        }

        if (strcmp(argv[ii], "--verboser") == 0) {
            options.verbose = 2;
            continue;
        }

        /* Single character options */
        if (strlen(argv[ii]) >= 2 && argv[ii][1] != '-') {
            for (size_t jj = 1; argv[ii][jj]; ++jj) {
                switch (argv[ii][jj]) {
                case 'v':
                    if (options.verbose <= 2)
                        options.verbose++;
                    break;
                case 'h':
                    show_help(command);
                    exit(EXIT_SUCCESS);
                default:
                    bufpos += snprintf(buf + bufpos, sizeof(buf) - bufpos, "-%c ", argv[ii][jj]);
                    break;
                }
            }
            continue;
        }

        bufpos += snprintf(buf + bufpos, sizeof(buf) - bufpos, "%s ", argv[ii]);
    }

    if (strlen(unknown) < bufpos) {
        fprintf(stderr, "%s\n", buf);
        exit(EXIT_SUCCESS);
    }
}
