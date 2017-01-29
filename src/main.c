/*
 *   main.c -- Ladle - Chef Bootstrapper
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

#include <ladle/chef.h>
#include <ladle/groups.h>
#include <ladle/memory.h>
#include <ladle/opts.h>
#include <ladle/utils.h>
#include <ladle/users.h>

int
main(int argc, char ** argv)
{
    ladle_getops(argc, argv);

    setup_directories();
    setup_files();
    get_groups("/etc/group");
    get_users("/etc/passwd");
    generate_users();
    generate_groups();
    clean_groups();
    clean_users();
    clear_options();

    leakcheck();

    return 0;
}
