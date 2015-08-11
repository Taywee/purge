/*
    purge: A drop-in backup retention manager for systems that already take dailies
    Copyright (C) 2015 Taylor C. Richberger <taywee@gmx.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _XOPEN_SOURCE
#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

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
} node;

/**
 * Make a node from a filename and other information.
 *
 * \param filename The filename to pass in.  The node takes ownership of the filename.
 * \param timestamp The timestamp string to match. Used as the "s" argument in strptime.  If NULL, filename is tested for the timestamp.
 * \param timeformat The time formatter to use. Used as the "format" argument in strptime.
 * \returns The node.  Pass into freenode to free it.  If you free it directly, you will leak memory.
 */
static node *makenode(char *filename, const char *timestamp, const char *timeformat);

typedef enum _stype
{
    begin,
    daily,
    weekly,
    monthly,
    yearly
} stype;

typedef struct _selector {
    // If type is "begin", then count is actually the number of stored elements, not including the beginning.
    // If type is "begin", then specifier is actually the reserved number of slots in the list (not bytes).
    stype type;
    size_t count;
    size_t specifier;
    unsigned int every;
} selector;

/// Do not use selectorlist as an array.  You must iterate through it by use of listbegin (returning a pointer to the beginning of the list) and listend (returning the past-the-end pointer), or use the listbegin pointer as an array directly.
typedef selector * selectorlist;

#define listbegin(list) (list + 1)
#define listend(list) (list + 1 + list[0].count)
// listsize and listreserved both return lvalues
#define listsize(list) (list[0].count)
#define listreserved(list) (list[0].specifier)

static selectorlist makeselectorlist(void);
#define freeselectorlist(list) free (list)

/**
 * Parse an option string into a selector struct
 *
 * \param option The option string in [count]{:[specifier]}{/every} form
 * \param type The type of string, as a member from the enum
 * \returns The selector
 */
static selector parseoption(const char * const option, stype const type);

/**
 * Add the selector to the null-terminated selector array, reallocating.
 */
static void addselector(selectorlist list, selector *item);

int main(int argc, char **argv)
{
    // days, weeks, months, years
    selectorlist selectors = makeselectorlist();

    int opt;
    while ((opt = getopt(argc, argv, "d:w:m:y:")) != -1)
    {
        switch (opt)
        {
            case 'd':
                {
                    selector s = parseoption(optarg, daily);
                    addselector(selectors, &s);
                    break;
                }
            case 'w':
                {
                    selector s = parseoption(optarg, weekly);
                    addselector(selectors, &s);
                    break;
                }
            case 'm':
                {
                    selector s = parseoption(optarg, monthly);
                    addselector(selectors, &s);
                    break;
                }
            case 'y':
                {
                    selector s = parseoption(optarg, yearly);
                    addselector(selectors, &s);
                    break;
                }
        }
    }

    for (selector *item = listbegin(selectors); item != listend(selectors); ++item)
    {
        puts("--item--");
        switch (item->type)
        {
            case daily:
                {
                    puts("daily");
                    break;
                }
            case weekly:
                {
                    puts("weekly");
                    break;
                }
            case monthly:
                {
                    puts("monthly");
                    break;
                }
            case yearly:
                {
                    puts("yearly");
                    break;
                }
            default:
                {
                    puts("ERROR");
                    break;
                }
        }
        printf("Count: %zu\n", item->count);
        printf("Specifier: %zu\n", item->specifier);
        printf("Every: %u\n", item->every);
    }

    freeselectorlist(selectors);

    return 0;
}

node *makenode(char *filename, const char *timestamp, const char *timeformat)
{
    struct tm tm;
    if (!timestamp)
    {
        timestamp = filename;
    }
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

        output->name = filename;
        
        return (output);
    } else
    {
        return NULL;
    }
}

static selector parseoption(const char * const option, stype const type)
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
