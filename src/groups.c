/*
 *   groups.c -- Ladle - Chef Bootstrapper
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

#define _XOPEN_SOURCE 600

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ladle/groups.h>
#include <ladle/log.h>
#include <ladle/memory.h>
#include <ladle/utils.h>

static void
add_members_to_group(const char *member_string, struct group_node *group_position)
{
    static const struct member_node EMTPY_MEMBER;

    if (strlen(member_string) == 0)
        return;

    struct member_node *member_position;
    member_position = group_position->member_root;

    char *token, *string, *tofree;
    tofree = string = xstrdup(member_string);

    while ((token = gen_strsep(&string, ",")) != NULL) {
        while (member_position && member_position->next)
            member_position = member_position->next;

        member_position->next = xmalloc(sizeof(struct member_node));
        member_position = member_position->next;
        *member_position = EMTPY_MEMBER;

        member_position->name = xstrdup(token);
        writelog(LOG_DEBUG_VERBOSE, LOG_CHEF, "Added member %s to groupnode %s", member_position->name, group_position->name);
    }

    xfree(tofree);
}

void
get_groups(const char *const file)
{
    FILE *fp;
    char entry[0x40B];
    unsigned ii = 0;
    char *token, *string, *tofree;
    static const struct group_node EMPTY_GROUP;
    static const struct member_node EMPTY_MEMBERS;

    struct group_node *group_position;

    /* Create our groups */
    group_root = xmalloc(sizeof(struct group_node));
    *group_root = EMPTY_GROUP;
    group_root->next = NULL;

    /* And the members */
    group_root->member_root = xmalloc(sizeof(struct member_node));
    *group_root->member_root = EMPTY_MEMBERS;
    group_root->member_root->next = NULL;

    group_position = group_root;

    fp = fopen(file, "r");
    if (fp == NULL)
        writelog(LOG_FATAL, LOG_GROUPS, "Failed to open %s: %s", file, strerror(errno));

    writelog(LOG_DEBUG_VERBOSE, LOG_CHEF, "Opened %s for reading", file);

    while (fgets(entry, sizeof(entry) - 1, fp) != NULL) {
        /* Remove all trailing newlines as we don't care about them */
        unsigned interpoint = 1;

        assert(entry[strlen(entry)] == '\0');
        while (entry[strlen(entry) - interpoint] == '\n')
            entry[strlen(entry) - interpoint--] = '\0';

        assert(entry[strlen(entry) - 1] != '\n');

        /* Alright. Now go ahead and split the group line up
         * so we can understand what's going on. Take a copy
         * of the original string so we don't mess it up. */
        tofree = string = xstrdup(entry);

        /* Index of the section we are in
         * To understand this, we must first understand how
         * the group is layed out in /etc/group.
         * groupname:x:24:a,b,c
         * --------- - -- -----
         *     |     |  |   |
         *     1     2  3   4
         *
         * 1) group name
         * 2) password, usually blank. OK to ignore
         * 3) group ID
         * 4) users who are a member of the group
         */

        /* Create a new group node in our list. */
        while (group_position && group_position->next)
            group_position = group_position->next;

        group_position->next = xmalloc(sizeof(struct group_node));
        group_position = group_position->next;
        *group_position = EMPTY_GROUP;

        group_position->member_root = xmalloc(sizeof(struct member_node));
        *group_position->member_root = EMPTY_MEMBERS;

        unsigned jj = 0;
        while ((token = gen_strsep(&string, ":")) != NULL) {
            switch (jj) {
            case 0:
                group_position->name = xstrdup(token);
                break;
            case 2:
                group_position->id = xstrdup(token);
                break;
            case 3:
                add_members_to_group(token, group_position);
                break;
            default:
                break;
            }
            jj++;
        }

        xfree(tofree);
        ii++;

        writelog(LOG_DEBUG_VERBOSE, LOG_CHEF, "Added groupnode %s", group_position->name);
    }

    fclose(fp);
}

void
clean_groups(void)
{
    struct group_node *group_position, *del_pos;

    group_position = group_root;

    /* Walk through each group */
    while (group_position) {
        struct member_node *member_position, *member_del_pos;
        member_position = group_position->member_root;

        /* Delete the name if it exists, otherwise
         * it's just the anchor node. */
        if (group_position->name)
            xfree(group_position->name);

        if (group_position->id)
            xfree(group_position->id);

        /* Delete members */
        while (member_position) {
            if (member_position->name)
                xfree(member_position->name);

            /* Store the pointer to the structure we want to
             * delete, while keeping the pointer to the next
             * node as our current position. */
            member_del_pos = member_position;
            member_position = member_position->next;
            xfree(member_del_pos);
        }

        /* Same for members */
        del_pos = group_position;
        group_position = group_position->next;
        xfree(del_pos);
    }
}
