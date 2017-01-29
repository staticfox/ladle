/*
 *   chef.c -- Ladle - Chef Bootstrapper
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

#include <errno.h>
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <ladle/groups.h>
#include <ladle/ladle.h>
#include <ladle/log.h>
#include <ladle/users.h>

/* File system support... yay! This means that this only
 * ever work on a POSIX compliant platform for a while.
 */
int
unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    (void) sb; (void) ftwbuf; (void) typeflag;

    int rv = remove(fpath);

    if (rv)
        writelog(LOG_FATAL, LOG_CHEF, "Failed to remove directory %s: %s", fpath, strerror(errno));

    writelog(LOG_DEBUG, LOG_CHEF, "Removed %s", fpath);
    return rv;
}

void
setup_directories(void)
{
    struct stat st;
    char c;

    /* Subdirectories */
    const char *directories[] = {
        "attributes", "definitions", "files",
        "libraries", "providers", "recipes",
        "resources", "templates", "files/default",
        "templates/default"
    };

    memset(&st, 0, sizeof(st));

    /* Check if the directory already exists */
    if (stat(options.directory, &st) != -1) {
        printf("%s already exists. Overwrite? [Y/n] ", options.directory);
        c = getchar();
        if (c != 'Y' && c != 'y')
            exit(EXIT_SUCCESS);

        /* File Tree Walk:
         *
         * FTW_DEPTH does a post-order traversal. This calls our
         * unlink callback for the directory itself after handling
         * the contents of the directory and it's subdirectories.
         *
         * FTW_PHYS means don't follow symbolic links.
         */
        nftw(options.directory, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
        writelog(LOG_INFO, LOG_CHEF, "Removed path %s", options.directory);
    }

    /* Create the root directory for the cookbook */
#ifdef _WIN32
    mkdir(options.directory);
#else
    mkdir(options.directory, 0700);
#endif

    /* Create each subdirectory */
    for (size_t ii = 0; ii < sizeof(directories) / sizeof(*directories); ++ii) {
        char buf[1024] = { 0 };
        snprintf(buf, sizeof(buf), "%s/%s", options.directory, directories[ii]);

#ifdef _WIN32
        mkdir(buf);
#else
        mkdir(buf, 0700);
#endif
    }

    writelog(LOG_INFO, LOG_CHEF, "Setup directory structure in %s", options.directory);
}

void
setup_files(void)
{
    FILE *f;
    char buf[1024] = { 0 };
    const char *const file_default_data =
        "#\n"
        "# !! Automatically generated by Ladle !!\n"
        "# Compiled "__DATE__", "__TIME__"\n"
        "#\n"
        "# Cookbook Name:: default\n"
        "# Recipe:: default\n"
        "#\n"
        "# Copyright 2017, YOUR_COMPANY_NAME\n"
        "#\n"
        "# All rights reserved - Do Not Redistribute\n"
        "#\n\n";

    snprintf(buf, sizeof(buf), "%s/recipes/default.rb", options.directory);
    f = fopen(buf, "w+");

    if (f == NULL)
        writelog(LOG_FATAL, LOG_CHEF, "Failed to open %s for writing: %s", buf, strerror(errno));

    fprintf(f, "%s", file_default_data);
    fclose(f);

    writelog(LOG_INFO, LOG_CHEF, "Created %s", buf);
}

void
generate_users(void)
{
    FILE *f;
    char buf[1024] = { 0 };
    struct user_node *user_position;

    snprintf(buf, sizeof(buf), "%s/recipes/users.rb", options.directory);
    f = fopen(buf, "w+");

    user_position = user_root;

    const char *const file_default_data =
        "#\n"
        "# !! Automatically generated by Ladle !!\n"
        "# Compiled "__DATE__", "__TIME__"\n"
        "#\n"
        "# Cookbook Name:: default\n"
        "# Recipe:: users\n"
        "#\n"
        "# Copyright 2017, YOUR_COMPANY_NAME\n"
        "#\n"
        "# All rights reserved - Do Not Redistribute\n"
        "#\n\n";
    const char *noshell[] = {"/usr/bin/nologin", "/sbin/nologin", "/bin/nologin", "/bin/false"};

    fprintf(f, "%s", file_default_data);
    while (user_position) {
        int found = 0;

        if (user_position->name == NULL)
            user_position = user_position->next;

        fprintf(f, "user '%s' do\n", user_position->name);

        for (size_t ii = 0; ii < (sizeof(noshell) / sizeof(*noshell)); ++ii) {
            if (strcmp(user_position->shell, noshell[ii]) == 0) {
                fprintf(f, "  action :lock\n");
                found = 1;
                break;
            }
        }

        if (!found)
            fprintf(f, "  action :create\n");

        fprintf(f, "  comment '%s'\n", user_position->comment);
        fprintf(f, "  uid '%s'\n", user_position->user_id);
        fprintf(f, "  gid '%s'\n", user_position->primary_group_id);
        fprintf(f, "  home '%s'\n", user_position->home_directory);
        fprintf(f, "  shell '%s'\n", user_position->shell);
        fprintf(f, "end\n\n");
        user_position = user_position->next;
    }

    fclose(f);

    writelog(LOG_INFO, LOG_CHEF, "Created users recipe in %s", buf);
}

void
generate_groups(void)
{
    FILE *f;
    char buf[1024] = { 0 };
    struct group_node *group_position;

    snprintf(buf, sizeof(buf), "%s/recipes/groups.rb", options.directory);
    f = fopen(buf, "w+");

    group_position = group_root;

    const char *const file_default_data =
        "#\n"
        "# !! Automatically generated by Ladle !!\n"
        "# Compiled "__DATE__", "__TIME__"\n"
        "#\n"
        "# Cookbook Name:: default\n"
        "# Recipe:: groups\n"
        "#\n"
        "# Copyright 2017, YOUR_COMPANY_NAME\n"
        "#\n"
        "# All rights reserved - Do Not Redistribute\n"
        "#\n\n";

    fprintf(f, "%s", file_default_data);
    while (group_position) {
        if (group_position->name == NULL) {
            group_position = group_position->next;
            continue;
        }

        struct member_node *member_position;
        member_position = group_position->member_root;

        fprintf(f, "group '%s' do\n", group_position->name);

        fprintf(f, "  action :create\n");
        fprintf(f, "  gid '%s'\n", group_position->id);
        fprintf(f, "  group_name '%s'\n", group_position->name);

        /* Generate each member */
        fprintf(f, "  members [\n");
        while (member_position) {
            if (member_position->name == NULL) {
                member_position = member_position->next;
                continue;
            }

            fprintf(f, "    '%s',\n", member_position->name);
            member_position = member_position->next;
        }
        fprintf(f, "  ]\n");

        fprintf(f, "end\n\n");
        group_position = group_position->next;
    }

    fclose(f);

    writelog(LOG_INFO, LOG_CHEF, "Created groups recipe in %s", buf);
}
