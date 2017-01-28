/*
 *   users.c -- Ladle - Chef Bootstrapper
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

#include <ladle/log.h>
#include <ladle/memory.h>
#include <ladle/users.h>
#include <ladle/utils.h>

void
get_users(void)
{
    FILE *fp;
    char entry[0x40B];
    unsigned ii = 0;
    char *token, *string, *tofree;
    static const struct user_node EMPTY_USER;

    struct user_node *user_position;

    /* Create our users */
    user_root = xmalloc(sizeof(*user_root));
    *user_root = EMPTY_USER;
    user_root->next = NULL;

    user_position = user_root;

    fp = fopen("/etc/passwd", "r");
    if (fp == NULL)
        writelog(LOG_FATAL, LOG_USERS, "Failed to open /etc/passwd: %s", strerror(errno));

    while (fgets(entry, sizeof(entry) - 1, fp) != NULL) {
        /* Remove all trailing newlines as we don't care about them */
        unsigned interpoint = 1;

        assert(entry[strlen(entry)] == '\0');
        while (entry[strlen(entry) - interpoint] == '\n')
            entry[strlen(entry) - interpoint--] = '\0';

        assert(entry[strlen(entry) - 1] != '\n');

        tofree = string = xstrdup(entry);

        /* Index of the section we are in
         * To understand this, we must first understand how
         * the users are layed out in /etc/passwd.
         * username:x:24:23:Something Neat:/home/username:/bin/bash
         * -------- - -- -- -------------- -------------- ---------
         *     |    |  |  |       |              |            |
         *     1    2  3  4       5              6            7
         *
         * 1) user name
         * 2) password, usually blank. OK to ignore
         * 3) user id
         * 4) primary group id
         * 5) user info/comment
         * 6) home directory
         * 7) shell
         */

        while (user_position && user_position->next)
            user_position = user_position->next;

        user_position->next = xmalloc(sizeof(struct user_node));
        user_position = user_position->next;
        *user_position = EMPTY_USER;

        unsigned jj = 0;
        while ((token = gen_strsep(&string, ":")) != NULL) {
            switch (jj) {
            case 0:
                user_position->name = xstrdup(token);
                break;
            case 2:
                user_position->user_id = xstrdup(token);
                break;
            case 3:
                user_position->primary_group_id = xstrdup(token);
                break;
            case 4:
                user_position->comment = xstrdup(token);
                break;
            case 5:
                user_position->home_directory = xstrdup(token);
                break;
            case 6:
                user_position->shell = xstrdup(token);
                break;
            default:
                break;
            }
            jj++;
        }

        xfree(tofree);
        ii++;
    }


    pclose(fp);
}

void
clean_users(void)
{
    struct user_node *user_position, *del_pos;

    user_position = user_root;

    while (user_position) {
        if (user_position->name)
            xfree(user_position->name);

        if (user_position->comment)
            xfree(user_position->comment);

        if (user_position->primary_group_id)
            xfree(user_position->primary_group_id);

        if (user_position->user_id)
            xfree(user_position->user_id);

        if (user_position->home_directory)
            xfree(user_position->home_directory);

        if (user_position->shell)
            xfree(user_position->shell);

        del_pos = user_position;
        user_position = user_position->next;
        xfree(del_pos);
    }
}
