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

#define _XOPEN_SOURCE 500
#define _DEFAULT_SOURCE 500
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "selector.h"
#include "node.h"

static void usage(const char * const progname);

int main(int argc, char **argv)
{
    selectorlist *selectors = selectorlistnew();

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
                    selector s = selectorparse(optarg, daily);
                    selectorlistadd(selectors, &s);
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
                    selector s = selectorparse(optarg, monthly);
                    selectorlistadd(selectors, &s);
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
                    selector s = selectorparse(optarg, weekly);
                    selectorlistadd(selectors, &s);
                    break;
                }
            case 'y':
                {
                    selector s = selectorparse(optarg, yearly);
                    selectorlistadd(selectors, &s);
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

    // Iterate lines of input
    char *line = NULL;
    ssize_t size = 0;
    size_t bufsize = 0;
    nodelist *nodes = nodelistnew();
    while ((size = getline(&line, &bufsize, stdin)) != -1)
    {
        line[size - 1] = '\0';
        const char * filename;
        const char * timestamp;
        if (splittime)
        {
            char * const slash = strchr(line, '/');
            if (!slash)
            {
                fprintf(stderr, "-s is specified but line has no slash, skipping: %s\n", line);
                continue;
            }

            // Split filename and timestamp into separate contiguous strings
            *slash = '\0';
            filename = slash + 1;
            timestamp = line;
        } else
        {
            filename = line;
            timestamp = line;
        }

        node *item = nodemake(filename, timestamp, formatter);

        // Add the node if the timestamp was properly available
        if (item)
        {
            nodelistadd(nodes, item);
        }
    }
    free(line);

    // Sort the nodes in reverse timestamp order, putting the newest ones at the front
    qsort(nodelistbegin(nodes), nodelistsize(nodes), sizeof(node *), nodecompare);

    for (selector *sel = selectorlistbegin(selectors); sel != selectorlistend(selectors); ++sel)
    {
        unsigned int matches = 0;
        time_t lastmatch = 0;
        for (node **nodp = nodelistbegin(nodes); nodp != nodelistend(nodes); ++nodp)
        {
            // Kick out if we've made our match quota
            if (matches >= sel->count)
            {
                break;
            }

            node * const nod = *nodp;

            // To match the specifier
            time_t nodespec;
            // To do the every and also lastmatch
            time_t nodemoditem;

            switch (sel->type)
            {
                case daily:
                    {
                        nodespec = sel->specifier;
                        nodemoditem = nod->days;
                        break;
                    }
                case weekly:
                    {
                        nodespec = nod->tm.tm_wday;
                        nodemoditem = nod->weeks;
                        break;
                    }
                case monthly:
                    {
                        nodespec = nod->tm.tm_mday;
                        nodemoditem = nod->months;
                        break;
                    }
                case yearly:
                    {
                        nodespec = nod->tm.tm_mon + 1;
                        nodemoditem = nod->years;
                        break;
                    }
                default:
                    {
                        // If the selector can not be recognized, force it to not match any nodes
                        // It will simply match its count in the oldest nodes in this case.
                        nodespec = ~sel->specifier;
                        nodemoditem = lastmatch;
                    }
            }

            // If we've already matched an item in the same period, skip to the next
            if (nodemoditem == lastmatch)
            {
                continue;
            } else if ((nodespec == sel->specifier) && (nodemoditem % sel->every == 0))
            {
                ++matches;
                lastmatch = nodemoditem;

                // We don't check keep first, because different selectors can all match the same node
                nod->keep = true;
            }
        }

        // If we didn't fill enough matches, iterate from the oldest nodes, marking all as keeps until we either hit the right count or run out of nodes
        // The only way we'll run out of nodes is if the count is higher than the total number of nodes in the list.
        node **nodp = nodelistend(nodes);
        while ((matches < sel->count) && (nodp != nodelistbegin(nodes)))
        {
            node * const nod = *(--nodp);
            nod->keep = true;
            ++matches;
        }
    }
    for (node **nodp = nodelistbegin(nodes); nodp != nodelistend(nodes); ++nodp)
    {
        node * const nod = *nodp;

        // If remove is true, we want to show nodes where keep is false, and vice versa
        if (nod->keep != remove)
        {
            puts(nod->name);
        }
    }
    nodelistfree(nodes);
    selectorlistfree(selectors);

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
