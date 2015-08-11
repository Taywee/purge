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

selector paseselector(const char * const option, stype const type)
{
    selector output;
    output.type = type;

    switch (type)
    {
        case monthly:
        case yearly:
            {
                output.specifier = 1;
                break;
            }
        default:
            {
                output.specifier = 0;
                break;
            }
    }

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
        output.specifier = strtoul(colon + 1, &endptr, 10);

        if (endptr == colon + 1)
        {
            fprintf(stderr, "You specified a colon, but have not specified a valid specifier for option string \"%s\", so it has been defaulted to 0 (1 for day of month or month of year).\n", option);
            output.specifier = 0;
        }
    }

    if (slash)
    {
        output.every = strtoul(slash + 1, &endptr, 10);

        if (endptr == slash + 1)
        {
            fprintf(stderr, "You specified a slash, but have not specified a valid every for option string \"%s\", so it has been defaulted to 0 (1 for day of month or month of year).\n", option);
            output.every = 0;
        }
    }

    if (*endptr)
    {
        fprintf(stderr, "There were trailing junk characters that were not used in option string \"%s\", these were \"%s\".\n", option, endptr);
    }

    return output;
}

void addselector(selectorlist list, selector *item)
{
    ++listsize(list);
    if (listsize(list) == listreserved(list))
    {
        listreserved(list) *= 2;

        selectorlist newlist = realloc(list, listreserved(list) * sizeof(selector));
        if (newlist)
        {
            list = newlist;
        } else
        {
            fputs("Realloc failed!  Program can not be trusted to execute properly!  Exiting now with failure!", stderr);
            exit(EXIT_FAILURE);
        }
    }
    list[listsize(list)] = *item;
}

selectorlist makeselectorlist(void)
{
    selectorlist selectors = malloc(sizeof(selector) * 1);
    selectors[0].type = begin;
    selectors[0].count = 0;
    selectors[0].specifier = 1;
    return selectors;
}
