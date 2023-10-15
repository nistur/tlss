#include "include/tlss_internal.h"
#include "tlss_internal.h"

#include <string.h>

tlssReturn tlssClearContext(tlssContext* context)
{
    TLSS_CHECK(context == 0, NO_CONTEXT);
    context->m_activeGrids = NULL;
    context->m_deadGrids = NULL;
    tlssReturnCode(SUCCESS);
}

tlssReturn tlssInitContext(tlssContext** context)
{
    TLSS_CHECK(context == 0, NO_CONTEXT);
    *context = tlssMalloc(tlssContext);
    if(tlssClearContext(*context) != TLSS_SUCCESS)
	   tlssTerminateContext(context);
    tlssReturnCode(SUCCESS);
}

tlssReturn tlssTerminateContext(tlssContext** context)
{
    TLSS_CHECK(*context == 0, NO_CONTEXT);

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
    tlssReturnCode(SUCCESS);
}

tlssReturn tlssLoad(tlssContext* context, const tlssDigit* data, tlssGrid** grid)
{
    TLSS_CHECK(context == 0, NO_CONTEXT);
    TLSS_CHECK(grid == 0, NO_GRID);
    TLSS_CHECK(data == 0, INVALID_DATA);

    for(tlssIndex i = 0; i < 81; ++i)
	if(data[i] > 9) tlssReturnCode(INVALID_DATA);

    
    if(tlssAllocGrid(context, grid) != TLSS_SUCCESS)
	tlssReturn();
    
    memcpy((*grid)->m_data, data, sizeof(*data) * 81);
    
    tlssReturnCode(SUCCESS);
}

tlssReturn tlssReleaseGrid(tlssContext *context, tlssGrid **grid)
{
    TLSS_CHECK(context == 0, NO_CONTEXT);
    TLSS_CHECK(grid == 0 || *grid == 0, NO_GRID);

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
	    tlssReturnCode(INVALID_GRID);

	(*prev)->__next = (*grid)->__next;
    }
    (*grid)->__next = context->m_deadGrids;
    context->m_deadGrids = *grid;
    *grid = 0;

    tlssReturnCode(SUCCESS);
}

tlssReturn tlssValidateGrid(tlssGrid* grid)
{
    TLSS_CHECK(grid == 0, NO_GRID);

    for(tlssIndex y = 0; y < 9; ++y)
    {
	for(tlssIndex x = 0; x < 9; ++x)
	{
	    tlssIndex i = (y*9)+x;
	    tlssDigit val = grid->m_data[i];
	    if(val == 0)
		tlssReturnCode(INCOMPLETE_DATA);

	    // check the row, column, and box for matches
	    for(int rc = 0; rc < 9; ++rc)
	    {
		if(rc != x)
		{
		    tlssIndex ri = TLSS_INDEX(rc, y);
		    if(grid->m_data[ri] == val)
			tlssReturnCode(INVALID_DATA);
		}
		if(rc != y)
		{
		    tlssIndex ci = TLSS_INDEX(x, rc);
		    if(grid->m_data[ci] == val)
			tlssReturnCode(INVALID_DATA);
		}
		tlssIndex bi = TLSS_INDEX(TLSS_BOX_INDEX(x)+(rc%3), TLSS_BOX_INDEX(y)+(rc/3));
		if(bi != i)
		{
		    if(grid->m_data[bi] == val)
			tlssReturnCode(INVALID_DATA);
		}
	    }
	}
    }
    
    tlssReturnCode(SUCCESS);
}

tlssReturn tlssGetValue(tlssContext* context, tlssGrid* grid, tlssIndex x, tlssIndex y, tlssDigit* v)
{
    TLSS_CHECK(context == 0, NO_CONTEXT);
    TLSS_CHECK(grid == 0, NO_GRID);
    TLSS_CHECK(x >= 9 || y >= 9, INVALID_DATA);
    TLSS_CHECK(v == 0, INVALID_DATA);
    
    *v = grid->m_data[TLSS_INDEX(x, y)];
    tlssReturnCode(SUCCESS);
}

tlssReturn tlssSetValue(tlssContext* context, tlssGrid* in, tlssIndex x, tlssIndex y, tlssDigit v, tlssGrid** out)
{
    TLSS_CHECK(context == 0, NO_CONTEXT);
    TLSS_CHECK(in == 0, NO_GRID);
    TLSS_CHECK(out == 0, NO_GRID);
    TLSS_CHECK(x >= 9 || y >= 9 || v > 9 || v == 0, INVALID_DATA);
    if(tlssAllocGrid(context, out) != TLSS_SUCCESS)
	tlssReturn();
    
    if(tlssGridClone(in, *out) != TLSS_SUCCESS)
    {
	tlssReleaseGrid(context, out);
	tlssReturn();
    }
    (*out)->m_data[TLSS_INDEX(x, y)] = v;

    tlssReturnCode(SUCCESS);
}

const char* tlssError()
{
    return g_tlssErrors[g_tlssError];
}


tlssReturn tlssAllocGrid(tlssContext *context, tlssGrid **grid)
{
    TLSS_CHECK(context == 0, NO_CONTEXT);
    TLSS_CHECK(grid == 0, NO_GRID);
    
    *grid = context->m_deadGrids;
    if(*grid == NULL)
    {
	*grid = tlssMalloc(tlssGrid);
	(*grid)->__next = NULL;
    }
    context->m_deadGrids = (tlssGrid*)(*grid)->__next;

    (*grid)->__next = context->m_activeGrids;
    context->m_activeGrids = *grid;
    memset((*grid)->m_data, 0, sizeof(*(*grid)->m_data) * 81);

    tlssReturnCode(SUCCESS);
}

tlssReturn tlssGridClone(tlssGrid* src, tlssGrid* dst)
{
    TLSS_CHECK(src == 0 || dst == 0, NO_GRID);

    memcpy(dst->m_data, src->m_data, sizeof(*src->m_data) * 81);
    
    tlssReturnCode(SUCCESS);
}

tlssReturn tlssGridEquals(tlssGrid* a, tlssGrid* b)
{
    TLSS_CHECK(a == 0 || b == 0, NO_GRID);
    TLSS_CHECK(a == b, SUCCESS);// if this is the same ptr, then early out
    if(memcmp(a->m_data, b->m_data, sizeof(*a->m_data) * 81) != 0)
	tlssReturnCode(INVALID_GRID);
    tlssReturnCode(SUCCESS);
}

tlssReturn tlssGridMerge(tlssContext* context, tlssGrid* a, tlssGrid* b, tlssGrid** out)
{
    TLSS_CHECK(context == 0, NO_CONTEXT)
    TLSS_CHECK(a == 0 || b == 0 || out == 0, NO_GRID);
    if(tlssAllocGrid(context, out) != TLSS_SUCCESS)
    {
	// shouldn't be allocated in this case, but just ensure it's still 0
	*out = 0;
	tlssReturn();
    }
    if(tlssGridClone(a, *out) != TLSS_SUCCESS)
    {
	tlssReleaseGrid(context, out);
	tlssReturn();
    }
    for(int i = 0; i < 81; ++i)
    {
	if((*out)->m_data[i] != b->m_data[i])
	{
	    // two grids attempt to change the same cell?
	    if((*out)->m_data[i] != 0 && b->m_data[i] != 0)
	    {
		tlssReleaseGrid(context, out);
		tlssReturnCode(INVALID_DATA);
	    }
	    (*out)->m_data[i] = b->m_data[i];
	}
    }
    
    tlssReturnCode(SUCCESS);
}
