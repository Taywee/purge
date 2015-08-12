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
#include "node.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct _nodelist
{
    node **nodes;
    size_t count;
    size_t reserved;
};

// static initialization sets everything to zero.
static const node EMPTYNODE;
static const nodelist EMPTYLIST;

node *nodemake(const char * const filename, const char * timestamp, const char * const timeformat)
{
    // zero-initialize tm
    struct tm tm = EMPTYNODE.tm;

    if (strptime(timestamp, timeformat, &tm))
    {
        node *output = malloc(sizeof(node));
        output->tm = tm;
        // Ignore timezones.  If the timestamp expresses a sunday at midnight, and you did local time, it could actually think the file is a saturday, based on gmtime.
        output->seconds = timegm(&output->tm);
        output->days = output->seconds / 86400;
        output->weeks = output->days / 7;
        output->years = output->tm.tm_year - 70;
        output->months = (output->years * 12) + output->tm.tm_mon;

        output->name = strndup(filename, PATH_MAX);
        
        return (output);
    } else
    {
        return NULL;
    }
}

extern void nodefree(node *n)
{
    free(n->name);
    free(n);
}

nodelist *nodelistnew(void)
{
    nodelist *output = malloc(sizeof(nodelist));
    *output = EMPTYLIST;
    // Reserve one spot so that the resize math works without special cases
    output->nodes = malloc(sizeof(node));
    output->count = 0;
    output->reserved = 1;
    return output;
}

void nodelistfree(nodelist *list)
{
    for (node **item = nodelistbegin(list); item != nodelistend(list); ++item)
    {
        nodefree(*item);
    }
    free(list->nodes);
    free(list);
}

void nodelistadd(nodelist * list, node * item)
{
    if (list->count == list->reserved)
    {
        list->reserved *= 2;

        node **newlist = realloc(list->nodes, list->reserved * sizeof(node *));
        if (newlist)
        {
            list->nodes = newlist;
        } else
        {
            fputs("Realloc failed!  Program can not be trusted to execute properly!  Exiting now with failure!", stderr);
            exit(EXIT_FAILURE);
        }
    }
    list->nodes[list->count] = item;
    ++(list->count);
}

extern node ** nodelistbegin(nodelist * const list)
{
    return list->nodes;
}

extern node ** nodelistend(nodelist * const list)
{
    return list->nodes + list->count;
}

extern size_t nodelistsize(nodelist const * const list)
{
    return list->count;
}
