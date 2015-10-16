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
    daily,
    weekly,
    monthly,
    yearly
} stype;

typedef struct _selector {
    stype type;
    unsigned int count;
    short specifier;
    unsigned short every;
} selector;

/**
 * Parse an option string into a selector struct
 *
 * \param option The option string in [count]{:[specifier]}{/every} form
 * \param type The type of string, as a member from the enum
 * \returns The selector
 */
extern selector selectorparse(const char * const option, stype const type);

/// A dynarray implementation of selectors
struct _selectorlist;
typedef struct _selectorlist selectorlist;

extern selectorlist * selectorlistnew(void);
extern void selectorlistfree(selectorlist *list);

/// Add a *copy* of item to the list
extern void selectorlistadd(selectorlist * list, selector const * const item);
extern selector * selectorlistbegin(selectorlist * const list);
extern selector * selectorlistend(selectorlist * const list);
extern size_t selectorlistsize(selectorlist const * const list);
