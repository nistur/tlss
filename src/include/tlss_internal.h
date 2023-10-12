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

/***************************************
 * Library context
 * - holds current state
 ***************************************/
struct _tlssContext    
{
    tlssGrid* m_activeGrids;
    tlssGrid* m_deadGrids;
};

struct _tlssGrid
{
    MANAGED;
    tlssDigit m_data[81];
};

/***************************************
 * Some basic memory management wrappers
 ***************************************/
#include <stdlib.h>
#define tlssMalloc(x) (x*)malloc(sizeof(x))
#define tlssFree(x)   free(x)

/***************************************
 * Error handling
 ***************************************/
extern tlssReturn  g_tlssError;
extern const char* g_tlssErrors[];
#define tlssReturn(x)				\
    {						\
	g_tlssError = TLSS_##x;			\
	return TLSS_##x;			\
    }



#endif/*__TLSS_INTERNAL_H__*/
