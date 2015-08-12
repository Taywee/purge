/*
 * This file is part of retain.
 * 
 * retain is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * retain is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with retain.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once
#define _XOPEN_SOURCE
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif
#include <time.h>
#include <stdbool.h>

typedef struct _node
{
    char *name;

    struct tm tm;
    // Each time_t struct is in the unit since the Unix Epoch.  This is used for "each" calculations.
    time_t seconds; 
    time_t days; 
    time_t weeks; 
    time_t months; 
    time_t years; 
    bool keep;
} node;

/**
 * Make a node from a filename and other information.
 *
 * \param filename The filename to pass in.
 * \param timestamp The timestamp string to match. Used as the "s" argument in strptime.
 * \param timeformat The time formatter to use. Used as the "format" argument in strptime.
 * \returns The node.  Pass into nodefree to free it.  If you free it directly, you will leak memory.
 */
extern node *nodemake(const char * const filename, const char * timestamp, const char *timeformat);
extern void nodefree(node *n);
extern int nodecompare(const void *a, const void *b);

struct _nodelist;
typedef struct _nodelist nodelist;

extern nodelist * nodelistnew(void);
extern void nodelistfree(nodelist *list);

/// Add the item to the list.  The item will be freed with the rest of the list.
extern void nodelistadd(nodelist * list, node * item);
extern node ** nodelistbegin(nodelist * const list);
extern node ** nodelistend(nodelist * const list);
extern size_t nodelistsize(nodelist const * const list);

