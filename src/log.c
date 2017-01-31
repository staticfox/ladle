/*
 *   log.c -- Ladle - Chef Bootstrapper
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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ladle/ladle.h>
#include <ladle/log.h>
#include <ladle/memory.h>

/* NOTE: You are respondible for freeing this! */
static char *
level_to_string(enum log_level level)
{
    char *word = NULL;

    switch (level) {
    case LOG_INFO:
        word = xstrdup("INFO: ");
        break;
    case LOG_DEBUG:
        word = xstrdup("DEBUG: ");
        break;
    case LOG_DEBUG_VERBOSE:
        word = xstrdup("VERBOSE: ");
        break;
    case LOG_ERROR:
        word = xstrdup("ERROR: ");
        break;
    case LOG_FATAL:
        word = xstrdup("FATAL: ");
        break;
    default:
        word = xstrdup("UNKNOWN: ");
        break;
    }

    return word;
}

void
writelog(enum log_level level, const char *const module, const char *const message, ...)
{
    if (level == LOG_DEBUG && options.verbose < 1)
        return;

    if (level == LOG_DEBUG_VERBOSE && options.verbose < 2)
        return;

    FILE *stream = level == LOG_ERROR || level == LOG_FATAL ? stderr : stdout;
    char buf[1024] = { 0 };
    char *strlevel = level_to_string(level);
    snprintf(buf, sizeof(buf), "[%s] %s", module, strlevel);

    va_list ap;
    va_start(ap, message);
    vsnprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), message, ap);
    va_end(ap);

    buf[strlen(buf)] = '\n';


    fprintf(stream, "%s", buf);


    xfree(strlevel);

    if (level == LOG_FATAL)
        exit(EXIT_FAILURE);
}
