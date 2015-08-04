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

// 
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

unsigned long dbj2(const unsigned char *str)
{
    unsigned long hash = 5381;
    unsigned long c;

    while ((c = *str++) != 0)
    {
        hash = ((hash << 5) + hash) ^ c;
    }

    return hash;
}

typedef struct _node
{
    time_t seconds;
    time_t days; 
    time_t weeks; 
    time_t months; 
    time_t years; 
    char *name;
    unsigned long hash;
} node;

int main(int argc, char **argv)
{
    // days, weeks, months, years
    unsigned long matches[4] = {0, 0, 0, 0};

    int opt;
    while ((opt = getopt(argc, argv, "d:w:m:y:")) != -1)
    {
        switch (opt)
        {
            case 'd':
                {
                    ++matches[0];
                    break;
                }
            case 'w':
                {
                    ++matches[1];
                    break;
                }
            case 'm':
                {
                    ++matches[2];
                    break;
                }
            case 'y':
                {
                    ++matches[3];
                    break;
                }
        }
    }
    return 0;
}
