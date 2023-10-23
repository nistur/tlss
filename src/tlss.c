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
	    tlssIndex i = TLSS_INDEX(x, y);
	    tlssDigit val = grid->m_data[i];
	    if(val == 0) continue;
	    
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
    for(tlssIndex i = 0; i < 81; ++i)
    {
	tlssDigit val = grid->m_data[i];
	if(val == 0)
	    tlssReturnCode(INCOMPLETE_DATA);
    }
    
    tlssReturnCode(SUCCESS);
}

tlssReturn tlssGetValue(tlssContext* context, tlssGrid* grid, tlssIndex y, tlssIndex x, tlssDigit* v)
{
    TLSS_CHECK(context == 0, NO_CONTEXT);
    TLSS_CHECK(grid == 0, NO_GRID);
    TLSS_CHECK(x >= 9 || y >= 9, INVALID_DATA);
    TLSS_CHECK(v == 0, INVALID_DATA);
    
    *v = grid->m_data[TLSS_INDEX(x, y)];
    tlssReturnCode(SUCCESS);
}

tlssReturn tlssSetValue(tlssContext* context, tlssGrid* in, tlssIndex y, tlssIndex x, tlssDigit v, tlssGrid** out)
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
	// out = 0, b = 0, no change -
	// out = 0, b = *, select b
	// out = *, b = 0, no change -
	// out = A, b = A, no change -
	// out = A, b = B, ERROR
	
	if(((*out)->m_data[i] != b->m_data[i]) && b->m_data[i] != 0)
	{
	    // two grids attempt to change the same cell?
	    if((*out)->m_data[i] != 0)
	    {
		tlssReleaseGrid(context, out);
		tlssReturnCode(INVALID_DATA);
	    }
	    (*out)->m_data[i] = b->m_data[i];
	}
    }
    
    tlssReturnCode(SUCCESS);
}

tlssReturn tlssStep(tlssContext* context, tlssGrid* in, tlssGrid** out)
{
    TLSS_CHECK(context == 0, NO_CONTEXT);
    TLSS_CHECK(in == 0 || out == 0, NO_GRID);
    if(tlssValidateGrid(in) == TLSS_INVALID_DATA)
	tlssReturnCode(INVALID_GRID);
    if(tlssSimpleSolver(context, in, out) != TLSS_SUCCESS)
	tlssReturn();
    tlssReturnCode(SUCCESS);
}

tlssReturn tlssSolve(tlssContext* context, tlssGrid* in, tlssGrid** out)
{
    TLSS_CHECK(context == 0, NO_CONTEXT);
    TLSS_CHECK(in == 0 || out == 0, NO_GRID);
    tlssGrid* currentGrid = 0;
    tlssGrid* nextGrid = 0;

    tlssReturn result = tlssStep(context, in, &currentGrid);
    while(result == TLSS_SUCCESS)
    {
	result = tlssStep(context, currentGrid, &nextGrid);
	if(result != TLSS_SUCCESS)
	{
	    tlssReleaseGrid(context, &currentGrid);
	    tlssReturn();
	}
	if(tlssGridEquals(currentGrid, nextGrid) == TLSS_SUCCESS)
	{
	    *out = nextGrid;
	    tlssReleaseGrid(context, &currentGrid);
	    tlssReturnCode(INVALID_DATA);
	}
	tlssReleaseGrid(context, &currentGrid);
	currentGrid = nextGrid;
	nextGrid = 0;

	if(tlssValidateGrid(currentGrid) == TLSS_SUCCESS)
	{
	    *out = currentGrid;
	    tlssReturnCode(SUCCESS);
	}
    }
    tlssReturn();
}

tlssAllocFunc g_allocFunc = malloc;
tlssFreeFunc g_freeFunc = free;

void tlssSetAlloc(tlssAllocFunc allocfunc)
{
    if(allocfunc == 0)
	g_allocFunc = malloc;
    else
	g_allocFunc = allocfunc;
}

void tlssSetFree(tlssFreeFunc freefunc)
{
    if(freefunc == 0)
	g_freeFunc = free;
    else
	g_freeFunc = freefunc;
}
