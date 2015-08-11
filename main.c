/*
    retain: A drop-in backup retention manager for systems that already make dailies
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
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

#include "selector.h"

static void usage(const char * const progname);

int main(int argc, char **argv)
{
    // days, weeks, months, years
    selectorlist selectors = makeselectorlist();

    int opt;
    bool splittime = false;
    bool remove = true;
    const char * formatter = "%FT%T";
    while ((opt = getopt(argc, argv, "d:f:hkm:rsw:y:")) != -1)
    {
        switch (opt)
        {
            case 'd':
                {
                    selector s = paseselector(optarg, daily);
                    addselector(selectors, &s);
                    break;
                }
            case 'f':
                {
                    formatter = optarg;
                    break;
                }
            case 'h':
                {
                    usage(argv[0]);
                    return 0;
                    break;
                }
            case 'k':
                {
                    remove = false;
                    break;
                }
            case 'm':
                {
                    selector s = paseselector(optarg, monthly);
                    addselector(selectors, &s);
                    break;
                }
            case 'r':
                {
                    remove = true;
                    break;
                }
            case 's':
                {
                    splittime = true;
                    break;
                }
            case 'w':
                {
                    selector s = paseselector(optarg, weekly);
                    addselector(selectors, &s);
                    break;
                }
            case 'y':
                {
                    selector s = paseselector(optarg, yearly);
                    addselector(selectors, &s);
                    break;
                }
            default:
                {
                    usage(argv[0]);
                    return 1;
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

void usage(const char * const progname)
{
    printf("%s:\n"
            "\tSmall console program that specifically sorts and selects timestamped\n"
            "\tfile/directory sets based on a retention scheme, in order to remove them\n"
            "\tand stay on a strict and predictable retention schedule.\n\n"

            "\tFilenames are piped in (one per line.  Spaces are interpreted as being\n"
            "\tembedded in the filename. Filenames with embedded newlines are not\n"
            "\taccepted).\n\n"

            "\tFilenames ordinarily are expected to have an embedded timestamp, though\n"
            "\tthis behavior can be changed through the -s option.\n\n"

            "\t-d [selector]   Add a daily selector to the list\n"
            "\t-w [selector]   Add a weekly selector to the list\n"
            "\t-m [selector]   Add a monthly selector to the list\n"
            "\t-y [selector]   Add a yearly selector to the list\n"
            "\t-f [formatter]  Specify the time formatter (normally defaults to\n"
            "\t                \"%%FT%%T\"). If -s is used, this formatter may not have slashes.\n"
            "\t-h              Display this help menu.\n"
            "\t-k              Output files to keep instead of ones to remove.\n"
            "\t-r              Output files to remove instead of ones to keep.\n"
            "\t-s              Take the timestamp as a separate string, specified first on\n"
            "\t                the line and separated from the actual filename by a slash\n"
            "\t                (like timestamp/filename, or timestamp//path/to/file for an\n"
            "\t                absolute path).\n\n"

            "\tretain   Copyright (C) 2015  Taylor C. Richberger <taywee@gmx.com>\n"
            "\tThis program comes with ABSOLUTELY NO WARRANTY.  This is free\n"
            "\tsoftware, and you are welcome to redistribute it under the terms\n"
            "\tof the GNU General Public License as published by the Free Software\n"
            "\tFoundation, either version 3 of the License, or (at your option)\n"
            "\tany later version.\n", progname);
}
