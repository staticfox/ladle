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

int
ladle_getops(int argc, char **argv)
{
    static char *usage = "usage: %s [-hv] -d directory\n";

    if (argc < 2) {
        show_help(argv[0]);
        exit(EXIT_SUCCESS);
    }

    for (size_t ii = 0; argv[ii]; ii++) {
        if (strcmp(argv[ii], "-h") == 0) {
            show_help(argv[0]);
            exit(EXIT_SUCCESS);
        }

        if (strcmp(argv[ii], "--help") == 0) {
            show_help(argv[0]);
            exit(EXIT_SUCCESS);
        }

        if (strcmp(argv[ii], "-v") == 0) {
            options.verbose = 1;
            continue;
        }

        if (strcmp(argv[ii], "--verbose") == 0) {
            options.verbose = 1;
            continue;
        }

        if (strcmp(argv[ii], "-vv") == 0) {
            options.verbose = 2;
            continue;
        }

        if (strcmp(argv[ii], "--verboser") == 0) {
            options.verbose = 2;
            continue;
        }
    }
}
