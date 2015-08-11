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
#include <stdlib.h>

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

extern selectorlist makeselectorlist(void);
#define freeselectorlist(list) free (list)

/**
 * Parse an option string into a selector struct
 *
 * \param option The option string in [count]{:[specifier]}{/every} form
 * \param type The type of string, as a member from the enum
 * \returns The selector
 */
extern selector paseselector(const char * const option, stype const type);

/**
 * Add the selector to the null-terminated selector array, reallocating.
 */
extern void addselector(selectorlist list, selector *item);

