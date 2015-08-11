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
#include <string.h>

node *makenode(const char * const filename, const char * timestamp, const char * const timeformat)
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

        output->name = strndup(filename, PATH_MAX);
        
        return (output);
    } else
    {
        return NULL;
    }
}
