/*
Copyright (c) 2023 Philipp Geyer

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgement in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

Philipp Geyer
nistur@gmail.com
*/
#pragma once
#ifndef __TLSS_INTERNAL_H__
#define __TLSS_INTERNAL_H__

#include "tlss.h"

#define MANAGED \
    void* __next;

#define LAST(type, name, list)			\
    type* name = list;				\
    if(name != NULL)				\
    {						\
	while(name->__next != NULL)		\
	{					\
	    name = (type*)name->__next;		\
	}					\
    }

#define TLSS_INDEX(x, y) (((y) * 9) + (x))
#define TLSS_BOX_INDEX(n) (((n)/3)*3)

/***************************************
 * Library context
 * - holds current state
 ***************************************/
struct _tlssContext    
{
    tlssGrid* m_activeGrids;
    tlssGrid* m_deadGrids;
};

typedef struct _tlssMeta
{
    MANAGED;
    tlssSolverMetaID m_id;
    tlssMetaPtr m_meta;
} tlssMeta;

struct _tlssGrid
{
    MANAGED;
    tlssDigit m_data[81];
    tlssMeta* m_meta;
};

tlssReturn tlssAllocGrid(tlssContext* context, tlssGrid** grid);
tlssReturn tlssGridClone(tlssGrid* src, tlssGrid* dst);

/***************************************
 * Some basic memory management wrappers
 ***************************************/
#include <stdlib.h>
extern tlssAllocFunc g_allocFunc;
extern tlssFreeFunc  g_freeFunc;
#define tlssMalloc(x) (x*)g_allocFunc(sizeof(x))
#define tlssFree(x)   g_freeFunc(x)

/***************************************
 * Error handling
 ***************************************/
extern tlssReturn  g_tlssError;
extern const char* g_tlssErrors[];
#define tlssReturnCode(x)			\
    {						\
	g_tlssError = TLSS_##x;			\
	return TLSS_##x;			\
    }

#define tlssReturn()				\
    {						\
	return g_tlssError;			\
    }

// unsure if this will work, but turn off null checking etc for
// extra speed
#if defined(TLSS_UNSAFE)
#define TLSS_CHECK(rest, ret)
#else
#define TLSS_CHECK(test, ret)						\
    if (test)								\
	tlssReturnCode(ret);
#endif

#endif/*__TLSS_INTERNAL_H__*/
