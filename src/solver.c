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
#include "tlss.h"
#include "tlss_internal.h"



tlssReturn tlssSimpleSolver(tlssContext* context, tlssGrid* in, tlssGrid** out)
{
    TLSS_CHECK(context == 0, NO_CONTEXT);
    TLSS_CHECK(in == 0 || out == 0, NO_GRID);

    tlssAvailability availabilityGrid[81];
    for(tlssIndex i = 0; i < 81; ++i)
	availabilityGrid[i] = TLSS_AVAIL_ALL;

    
    for(tlssIndex y = 0; y < 9; ++y)
    {
	for(tlssIndex x = 0; x < 9; ++x)
	{
	    tlssIndex i = TLSS_INDEX(x, y);
	    tlssDigit val = 0;
	    if(tlssGetValue(context, in, y, x, &val) != TLSS_SUCCESS)
		tlssReturn();
	    if(val == 0) continue;
	    
	    // check the row, column, and box for matches
	    for(int rc = 0; rc < 9; ++rc)
	    {
		if(rc != x)
		{
		    tlssIndex ri = TLSS_INDEX(rc, y);
		    availabilityGrid[ri] &= ~(1<<(val - 1));
		}
		if(rc != y)
		{
		    tlssIndex ci = TLSS_INDEX(x, rc);
		    availabilityGrid[ci] &= ~(1<<(val - 1));
		}
		tlssIndex bi = TLSS_INDEX(TLSS_BOX_INDEX(x)+(rc%3), TLSS_BOX_INDEX(y)+(rc/3));
		if(bi != i)
		{
		    availabilityGrid[bi] &= ~(1<<(val - 1));
		}
	    }
	}
    }

    // As grids are immutable, we have to do a bit of a dance around this - we set one value
    // and get a new grid returned, then use that to set another value. A much better way would be
    // to resolve the availability grid into a tlssDigit array, and then load that into a new grid.
    tlssGrid* currentGrid = in;
    tlssGrid* nextGrid = 0;

    for(tlssIndex i = 0; i < 81; ++i)
    {
	tlssIndex x = i%9;
	tlssIndex y = i/9;
	tlssDigit d = 0;
	if(tlssGetValue(context, currentGrid, y, x, &d) != TLSS_SUCCESS)
	    tlssReturn();
	if(d == 0)
	{
	    switch(availabilityGrid[i])
	    {
#define SET(n)								\
		case TLSS_AVAIL_##n:					\
		{							\
		    if(tlssSetValue(context, currentGrid, y, x, n, &nextGrid) != TLSS_SUCCESS) \
			tlssReturn();					\
		    if(currentGrid != in && tlssReleaseGrid(context, &currentGrid) != TLSS_SUCCESS) \
			tlssReturn();					\
		    currentGrid = nextGrid;				\
		    nextGrid = 0;					\
		    break;						\
		}
		SET(1);
		SET(2);
		SET(3);
		SET(4);
		SET(5);
		SET(6);
		SET(7);
		SET(8);
		SET(9);
	    }
	}
    }
    *out = currentGrid;
    
    tlssReturnCode(SUCCESS);
}


tlssReturn tlssRegisterMetaType(tlssContext *context, tlssSolverMetaID id, tlssMetaAllocFunc allocfunc, tlssMetaFreeFunc freefunc)
{
    tlssReturnCode(SUCCESS);    
}
