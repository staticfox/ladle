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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ladle/memory.h>
#include <ladle/utils.h>

/* Some of these id fields can be ints,
 * but then we worry about padding 0s,
 * atoi() and char - '0' removes that so
 * we take the lazy approach.
 */
struct group_node {
    char *name;
    char *id;
    struct group_node *next;
    struct member_node {
        char *name;
        struct member_node *next;
    } *member_root;
} *group_root;

struct user_node {
    char *name;
    char *user_id;
    char *primary_group_id;
    char *comment;
    char *home_directory;
    char *shell;
    struct user_node *next;
} *user_root;

void
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
    }

    xfree(tofree);
}

void
get_groups(void)
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

    fp = popen("cat /etc/group", "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to run command.");
        exit(EXIT_FAILURE);
    }

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
get_users(void)
{
    FILE *fp;
    char entry[0x40B];
    unsigned ii = 0;
    char *token, *string, *tofree;
    static const struct user_node EMPTY_USER;

    struct user_node *user_position;

    /* Create our groups */
    user_root = xmalloc(sizeof(*user_root));
    *user_root = EMPTY_USER;
    user_root->next = NULL;

    user_position = user_root;

    fp = popen("cat /etc/passwd", "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to run command.");
        exit(EXIT_FAILURE);
    }

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
         * the group is layed out in /etc/group.
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
generate_users(void)
{
    struct user_node *user_position;

    user_position = user_root;

    const char *const header_text = "# !! AUTOMATICALLY GENERATED BY Ladle !!\n\n";
    const char *noshell[] = {"/usr/bin/nologin", "/sbin/nologin", "/bin/nologin", "/bin/false"};

    printf(header_text);
    while (user_position) {
        int found = 0;

        if (user_position->name == NULL)
            user_position = user_position->next;

        printf("user '%s' do\n", user_position->name);

        for (size_t ii = 0; ii < (sizeof(noshell) / sizeof(*noshell)); ++ii) {
            if (strcmp(user_position->shell, noshell[ii]) == 0) {
                printf("  action :lock\n");
                found = 1;
                break;
            }
        }

        if (!found)
            printf("  action :create\n");

        printf("  comment '%s'\n", user_position->comment);
        printf("  uid '%s'\n", user_position->user_id);
        printf("  gid '%s'\n", user_position->primary_group_id);
        printf("  home '%s'\n", user_position->home_directory);
        printf("  shell '%s'\n", user_position->shell);
        printf("end\n\n");
        user_position = user_position->next;
    }
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

int
main(int argc, char ** argv)
{
    (void) argc; (void) argv;

    get_groups();
    get_users();
    generate_users();
    clean_groups();
    clean_users();
    leakcheck();

    return 0;
}
