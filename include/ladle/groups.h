/*
 *   groups.h -- Ladle - Chef Bootstrapper
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

#ifndef LADLE_INCLUDE_GROUPS
#define LADLE_INCLUDE_GROUPS

struct group_node {
    char *name;
    char *id;
    struct group_node *next;
    struct member_node {
        char *name;
        struct member_node *next;
    } *member_root;
} *group_root;

void get_groups(const char *const file);
void clean_groups(void);

#endif
