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
#include "selector.h"
#include <string.h>
#include <stdio.h>

struct _selectorlist
{
    selector *selectors;
    size_t count;
    size_t reserved;
};

// Used for null-initialization
static const selectorlist EMPTYLIST;

selector selectorparse(const char * const option, stype const type)
{
    selector output;
    output.type = type;
    output.specifier = -1;
    output.every = 1;

    char const * const colon = strchr(option, ':');
    char const * const slash = strchr(option, '/');

    char * endptr;
    output.count = strtoul(option, &endptr, 10);

    if (endptr == option)
    {
        fprintf(stderr, "You have not specified a valid count for option string \"%s\", so it has been defaulted to 0 (which matches nothing).\n", option);
        output.count = 0;
    }

    if (colon)
    {
        output.specifier = strtol(colon + 1, &endptr, 10);

        if (endptr == colon + 1)
        {
            fprintf(stderr, "You specified a colon, but have not specified a valid specifier for option string \"%s\", so it has been defaulted to -1.\n", option);
        }
    }

    if (slash)
    {
        output.every = strtoul(slash + 1, &endptr, 10);

        if (endptr == slash + 1)
        {
            fprintf(stderr, "You specified a slash, but have not specified a valid every for option string \"%s\", so it has been defaulted to 1.\n", option);
        }
    }

    if (*endptr)
    {
        fprintf(stderr, "There were trailing junk characters that were not used in option string \"%s\", these were \"%s\".\n", option, endptr);
    }

    return output;
}

selectorlist *selectorlistnew(void)
{
    selectorlist *output = malloc(sizeof(selectorlist));
    *output = EMPTYLIST;
    // Reserve one spot so that the resize math works without special cases
    output->selectors = malloc(sizeof(selector));
    output->count = 0;
    output->reserved = 1;
    return output;
}

void selectorlistfree(selectorlist *list)
{
    free(list->selectors);
    free(list);
}

void selectorlistadd(selectorlist * list, selector const * const item)
{
    if (list->count == list->reserved)
    {
        list->reserved *= 2;

        selector *newlist = realloc(list->selectors, list->reserved * sizeof(selector));
        if (newlist)
        {
            list->selectors = newlist;
        } else
        {
            fputs("Realloc failed!  Program can not be trusted to execute properly!  Exiting now with failure!", stderr);
            exit(EXIT_FAILURE);
        }
    }
    list->selectors[list->count] = *item;
    ++(list->count);
}

extern selector * selectorlistbegin(selectorlist * const list)
{
    return list->selectors;
}

extern selector * selectorlistend(selectorlist * const list)
{
    return list->selectors + list->count;
}

extern size_t selectorlistsize(selectorlist const * const list)
{
    return list->count;
}
