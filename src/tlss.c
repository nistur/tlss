#include "tlss_internal.h"

#include <string.h>

tlssReturn tlssClearContext(tlssContext* context)
{
    context->m_activeGrids = NULL;
    context->m_deadGrids = NULL;
    tlssReturn(SUCCESS);
}

tlssReturn tlssInitContext(tlssContext** context)
{
    if(context == 0)
        tlssReturn(NO_CONTEXT);
    *context = tlssMalloc(tlssContext);
    if(tlssClearContext(*context) != TLSS_SUCCESS)
	   tlssTerminateContext(context);
    tlssReturn(SUCCESS);
}

tlssReturn tlssTerminateContext(tlssContext** context)
{
    if(*context == 0)
	   tlssReturn(NO_CONTEXT);

#define __TLSS_CLEAR_GRIDS(gridlist)			\
    {							\
	tlssGrid* grid = gridlist;			\
	while(grid != NULL)				\
	{						\
	    tlssGrid* next = (tlssGrid*)grid->__next;	\
	    tlssFree(grid);				\
	    grid = next;				\
	}						\
	gridlist = NULL;				\
    }

    __TLSS_CLEAR_GRIDS((*context)->m_activeGrids);
    __TLSS_CLEAR_GRIDS((*context)->m_deadGrids);

#undef __TLSS_CLEAR_GRIDS
    
    tlssClearContext(*context);
    tlssFree(*context);
    *context = 0;
    tlssReturn(SUCCESS);
}

tlssReturn tlssLoad(tlssContext* context, const tlssDigit* data, tlssGrid** grid)
{
    if(context == 0)
	tlssReturn(NO_CONTEXT);

    if(grid == 0)
	tlssReturn(NO_GRID);

    if(data == 0)
	tlssReturn(INVALID_DATA);

    for(tlssIndex i = 0; i < 81; ++i)
	if(data[i] > 9) tlssReturn(INVALID_DATA);

    *grid = context->m_deadGrids;
    if(*grid == NULL)
    {
	*grid = tlssMalloc(tlssGrid);
	(*grid)->__next = NULL;
    }
    context->m_deadGrids = (tlssGrid*)(*grid)->__next;

    (*grid)->__next = context->m_activeGrids;
    context->m_activeGrids = *grid;

    memcpy((*grid)->m_data, data, sizeof(*data) * 81);
    
    tlssReturn(SUCCESS);
}

tlssReturn tlssReleaseGrid(tlssContext *context, tlssGrid **grid)
{
    if(context == 0)
	tlssReturn(NO_CONTEXT);

    if(grid == 0 || *grid == 0)
	tlssReturn(NO_GRID);

    // remove from active grids, add to dead grids
    if(context->m_activeGrids == *grid)
    {
        context->m_activeGrids = (tlssGrid*)(*grid)->__next;
    }
    else
    {
	tlssGrid** prev = &context->m_activeGrids;
	while(*prev && (*prev)->__next  != *grid)
	    prev = (tlssGrid**)&(((*prev)->__next));

	if(*prev == 0)
	    tlssReturn(INVALID_GRID);

	(*prev)->__next = (*grid)->__next;
    }
    (*grid)->__next = context->m_deadGrids;
    context->m_deadGrids = *grid;
    *grid = 0;

    tlssReturn(SUCCESS);
}
const char* tlssError()
{
    return g_tlssErrors[g_tlssError];
}
