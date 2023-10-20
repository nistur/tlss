#include "testsuite/tests.h"
#include "tlss-tests.h"
#include "tlss.h"

#include <cstddef>

void PrintGrids(tlssContext* context, tlssGrid* init, tlssGrid* result)
{
    if(init == NULL) return;
    printf("\t╔═══════╤═══════╤═══════╗\t");
    if(result != NULL)
	printf("\t╔═══════╤═══════╤═══════╗\n");
    for(int i = 0; i < 9; ++i)
    {
	printf("\t║ ");
	for(int j = 0; j < 9; ++j)
	{
	    tlssDigit d = 0;
	    tlssGetValue(context, init, i, j, &d);
	    if(d == 0) printf("  ");
	    else printf("%d ", d);

	    if(j % 3 == 2 && j != 8) printf("│ ");
	}
	if(result != NULL)
	{
	    printf("║\t\t║ ");
	    for(int j = 0; j < 9; ++j)
	    {
		tlssDigit d = 0;
		tlssGetValue(context, result, i, j, &d);
		if(d == 0) printf("  ");
		else printf("%d ", d);
		if(j % 3 == 2 && j != 8) printf("│ ");
	    }
	}
	printf("║\n");
	if(i % 3 == 2 && i != 8)
	{
	    printf("\t╟───────┼───────┼───────╢\t");
	    if(result != NULL)
		printf("\t╟───────┼───────┼───────╢\n");
	}
    }
    printf("\t╚═══════╧═══════╧═══════╝\t");
    if(result != NULL)
	printf("\t╚═══════╧═══════╧═══════╝\n");
    printf("\n");
}

#define SOLVE(name, x)							\
    tlssDigit __##name##data[] = x;					\
    TEST(name, Smoke, 0.0f,						\
	 {								\
	     tlssInitContext(&m_data.context);				\
	     tlssLoad(m_data.context, __##name##data, &m_data.grid);	\
	     m_data.completedGrid = NULL;				\
	 },								\
	 {								\
	     PrintGrids(m_data.context, m_data.grid, m_data.completedGrid); \
	     tlssReleaseGrid(m_data.context, &m_data.completedGrid);	\
	     tlssReleaseGrid(m_data.context, &m_data.grid);		\
	     tlssTerminateContext(&m_data.context);			\
	 },								\
	 {								\
	     tlssReleaseGrid(m_data.context, &m_data.completedGrid);	\
	     ASSERT(tlssSolve(m_data.context, m_data.grid, &m_data.completedGrid) == TLSS_SUCCESS); \
	 },								\
	 {								\
	     tlssGrid *grid;						\
	     tlssGrid *completedGrid;					\
	     tlssContext *context;					\
	 });

#define GRID(...) { __VA_ARGS__ }

SOLVE(Easy1, GRID(
    0, 0, 0,  0, 0, 3,  0, 2, 7,
    1, 0, 0,  0, 0, 4,  6, 0, 3,
    0, 0, 0,  6, 0, 0,  0, 1, 0,
    
    6, 8, 5,  0, 7, 0,  1, 3, 2,
    7, 0, 0,  1, 6, 0,  5, 0, 8,
    0, 1, 9,  5, 0, 0,  0, 0, 4,

    9, 0, 0,  0, 4, 0,  0, 7, 1,
    0, 0, 0,  7, 2, 6,  0, 0, 0,
    0, 7, 3,  8, 9, 1,  0, 5, 0,
	  ));

SOLVE(Easy2, GRID(
    3, 0, 4,  0, 0, 0,  8, 0, 0,
    1, 0, 0,  3, 0, 0,  0, 0, 6,
    6, 0, 0,  0, 1, 7,  0, 0, 5,
    
    0, 0, 2,  7, 4, 6,  5, 3, 0,
    0, 0, 0,  0, 8, 9,  0, 0, 2,
    0, 0, 0,  1, 0, 3,  4, 7, 9,
    
    8, 6, 5,  0, 0, 2,  0, 1, 3,
    0, 9, 0,  6, 3, 0,  0, 0, 4,
    2, 0, 3,  0, 5, 1,  0, 0, 0,
    ));


SOLVE(Medium1, GRID(
	  
    0, 0, 0,  0, 6, 8,  9, 0, 3,
    0, 1, 0,  3, 0, 2,  0, 8, 0,
    0, 0, 0,  0, 0, 0,  0, 5, 0,

    8, 0, 0,  9, 0, 0,  0, 4, 0,
    3, 0, 0,  8, 0, 0,  7, 0, 0,
    2, 5, 6,  4, 7, 3,  8, 9, 0,
    
    1, 3, 0,  0, 0, 0,  0, 0, 0,
    0, 0, 0,  0, 0, 0,  0, 6, 0,
    7, 0, 4,  0, 1, 9,  0, 0, 0,
	  ));

SOLVE(Medium2, GRID(	  
    5, 0, 0,  0, 0, 0,  2, 7, 0,
    6, 0, 0,  0, 5, 0,  0, 3, 0,
    0, 2, 7,  0, 0, 3,  9, 0, 0,

    0, 0, 2,  3, 7, 8,  0, 1, 0,
    1, 0, 5,  4, 2, 0,  0, 0, 0,
    0, 0, 0,  0, 0, 0,  8, 0, 0,

    0, 9, 0,  0, 3, 0,  0, 5, 0,
    2, 0, 0,  7, 0, 0,  0, 9, 3,
    7, 0, 0,  1, 0, 0,  0, 8, 0,
));
SOLVE(Medium3, GRID(	  
    0, 0, 0,  9, 4, 0,  0, 0, 0,
    0, 0, 8,  0, 7, 0,  1, 0, 0,
    0, 6, 9,  0, 2, 0,  0, 0, 5,

    4, 5, 0,  6, 0, 0,  0, 0, 8,
    0, 3, 6,  8, 0, 5,  0, 2, 0,
    0, 8, 2,  0, 0, 0,  0, 0, 0,

    0, 0, 4,  2, 8, 0,  0, 0, 3,
    0, 1, 0,  0, 0, 0,  0, 5, 7,
    0, 0, 0,  0, 0, 1,  9, 8, 0,
));
