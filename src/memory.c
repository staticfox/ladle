/*
 *   memory.c -- Ladle - Chef Bootstrapper
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

#include <stdlib.h>
#include <string.h>

#include <ladle/log.h>

/* Mainly for debugging so we don't leak memory */
static long num_allocs;

static void
outofmemory(void)
{
    writelog(LOG_FATAL, LOG_MEMORY, "FATAL: Out of member! (%ld allocations)\n", num_allocs);
}

void *
xmalloc(size_t bytes)
{
    void *ret = malloc(bytes);

    if (ret == NULL)
        outofmemory();

    num_allocs++;
    return ret;
}

void *
xstrdup(const char *s)
{
    size_t size = strlen(s) + 1;
    void *ret = xmalloc(size);

    if (ret == NULL)
        outofmemory();

    memcpy(ret, s, size);

    return ret;
}

void
xfree(void *p)
{
    /* free() is perfectly safe to call on null pointers
     * but I don't want to break my stats :D
     */
    if (p)
        num_allocs--;

    free(p);
}

void
leakcheck(void)
{
    if (num_allocs != 0)
        writelog(LOG_ERROR, LOG_MEMORY, "Memory warning: Exiting program with non-garbage collected memory in heap: %lu allocations.\n", num_allocs);
}
