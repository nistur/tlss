#include "include/tlss_internal.h"
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
	    tlssDigit val = in->m_data[i];
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

    tlssGrid* currentGrid = in;
    tlssGrid* nextGrid = 0;

    for(tlssIndex i = 0; i < 81; ++i)
    {
	tlssIndex x = i%9;
	tlssIndex y = i/9;
	tlssDigit d = 0;
	if(tlssGetValue(context, currentGrid, y, x, &d) != TLSS_SUCCESS)
	    tlssReturn();
	if((*out)->m_data[i] == 0)
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

    
    tlssReturnCode(SUCCESS);
}


tlssReturn tlssRegisterMetaType(tlssContext *context, tlssSolverMetaID id, tlssMetaAllocFunc allocfunc, tlssMetaFreeFunc freefunc)
{
    tlssReturnCode(SUCCESS);    
}
