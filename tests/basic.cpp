#include "tlss-tests.h"
#include "tlss.h"

#include <cstddef>

TEST(InitTerminate, Basic, 0.0f,
     // initialisation
     {
	 m_data.context = 0;
     },
     // cleanup
     {
	 tlssTerminateContext(&m_data.context);
     },
     // test
     {
	 ASSERT(tlssInitContext(&m_data.context) == TLSS_SUCCESS);
	 ASSERT(m_data.context != 0);
	 ASSERT(tlssTerminateContext(&m_data.context) == TLSS_SUCCESS);
	 ASSERT(m_data.context == 0)
     },
     // data
     {
	 tlssContext* context;
     }
    );


TEST(LoadGrid, Basic, 0.0f,
     // initialisation
     {
	 tlssInitContext(&m_data.context);
	 m_data.smallSet = new tlssDigit[1];
	 m_data.smallSet[0] = 0;
	 m_data.largeSet = new tlssDigit[81];
	 for(int i = 0; i < 81; ++i)
	     m_data.largeSet[i] = 1 + ((i + (i / 9)) % 9);
	 m_data.grid = NULL;
     },
     // cleanup
     {
	 delete[] m_data.largeSet;
	 delete[] m_data.smallSet;
	 tlssReleaseGrid(m_data.context, &m_data.grid);
	 tlssTerminateContext(&m_data.context);
     },
     // test
     {
	 m_data.grid = NULL;
	 ASSERT(tlssLoad(NULL, m_data.largeSet, &m_data.grid) == TLSS_NO_CONTEXT);
	 ASSERT(tlssLoad(m_data.context, NULL, &m_data.grid) == TLSS_INVALID_DATA);
	 ASSERT(tlssLoad(m_data.context, m_data.smallSet, &m_data.grid) == TLSS_INVALID_DATA);
	 ASSERT(m_data.grid == NULL);
	 ASSERT(tlssLoad(m_data.context, m_data.largeSet, NULL) == TLSS_NO_GRID);

	 ASSERT(tlssLoad(m_data.context, m_data.largeSet, &m_data.grid) == TLSS_SUCCESS);
	 ASSERT(m_data.grid != NULL);
	 

	 ASSERT(tlssReleaseGrid(m_data.context, NULL) == TLSS_NO_GRID);
	 ASSERT(tlssReleaseGrid(NULL, &m_data.grid) == TLSS_NO_CONTEXT);
	 ASSERT(m_data.grid != NULL);

	 ASSERT(tlssReleaseGrid(m_data.context, &m_data.grid) == TLSS_SUCCESS);
	 ASSERT(m_data.grid == NULL);
	 
     },	   
     // data
     {
	 tlssContext* context;
	 tlssDigit* smallSet;
	 tlssDigit* largeSet;
	 tlssGrid* grid;
     }
    );
