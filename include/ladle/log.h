/*
 *   log.h -- Ladle - Chef Bootstrapper
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

#ifndef LADLE_INCLUDE_LOG
#define LADLE_INCLUDE_LOG

#define LOG_USERS "users"
#define LOG_GROUPS "groups"
#define LOG_MEMORY "memory"

enum log_level {
    LOG_INFO,
    LOG_DEBUG,
    LOG_DEBUG_VERBOSE,
    LOG_ERROR,
    LOG_FATAL
};

void writelog(enum log_level level, const char *const module, const char *const message, ...);

#endif
