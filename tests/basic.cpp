#include "testsuite/tests.h"
#include "tlss-tests.h"
#include "tlss.h"

#include <cstddef>

tlssDigit incompleteData[] = {
    0, 6, 0,  4, 2, 0,  0, 0, 1,
    1, 9, 0,  0, 8, 3,  0, 2, 0,
    0, 0, 2,  0, 1, 0,  7, 0, 0,
    
    0, 0, 0,  8, 7, 0,  5, 0, 0,
    0, 5, 1,  3, 4, 9,  0, 0, 2,
    4, 0, 3,  0, 5, 0,  0, 8, 0,
    
    6, 0, 5,  1, 3, 2,  0, 0, 0,
    7, 0, 4,  0, 0, 8,  0, 1, 0,
    0, 1, 0,  0, 6, 0,  8, 5, 0,
};
tlssDigit invalidData[] = {
    1, 2, 0,  0, 0, 0,  0, 0, 0,
    1, 2, 0,  0, 0, 0,  0, 0, 0,
    1, 2, 0,  0, 0, 0,  0, 0, 0,
    
    2, 3, 0,  0, 0, 0,  0, 0, 0,
    2, 3, 0,  0, 0, 0,  0, 0, 0,
    2, 3, 0,  0, 0, 0,  0, 0, 0,
    
    3, 4, 0,  0, 0, 0,  0, 0, 0,
    3, 4, 0,  0, 0, 0,  0, 0, 0,
    3, 4, 0,  0, 0, 0,  0, 0, 0,
};
tlssDigit validData[] = {
    5, 6, 8,  4, 2, 7,  3, 9, 1,
    1, 9, 7,  6, 8, 3,  4, 2, 5,
    3, 4, 2,  9, 1, 5,  7, 6, 8,
    
    9, 2, 6,  8, 7, 1,  5, 3, 4,
    8, 5, 1,  3, 4, 9,  6, 7, 2,
    4, 7, 3,  2, 5, 6,  1, 8, 9,
    
    6, 8, 5,  1, 3, 2,  9, 4, 7,
    7, 3, 4,  5, 9, 8,  2, 1, 6,
    2, 1, 9,  7, 6, 4,  8, 5, 3,
};

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

TEST(ValidateGrid, Basic, 0.0f,
     // initialisation
     {
	 tlssInitContext(&m_data.context);
	 tlssLoad(m_data.context, incompleteData, &m_data.incompleteGrid);
	 tlssLoad(m_data.context, invalidData, &m_data.incorrectGrid);
	 tlssLoad(m_data.context, validData, &m_data.validGrid);
     },
     // cleanup
     {
	 tlssReleaseGrid(m_data.context, &m_data.incompleteGrid);
	 tlssReleaseGrid(m_data.context, &m_data.incorrectGrid);
	 tlssReleaseGrid(m_data.context, &m_data.validGrid);
	 tlssTerminateContext(&m_data.context);
     },
     // test
     {
	 ASSERT(tlssValidateGrid(NULL) == TLSS_NO_GRID);
	 ASSERT(tlssValidateGrid(m_data.incompleteGrid) == TLSS_INCOMPLETE_DATA);
	 ASSERT(tlssValidateGrid(m_data.incorrectGrid) == TLSS_INVALID_DATA);
	 ASSERT(tlssValidateGrid(m_data.validGrid) == TLSS_SUCCESS);
     },
     // data
     {
	 tlssGrid* incompleteGrid;
	 tlssGrid* incorrectGrid;
	 tlssGrid* validGrid;
	 tlssContext* context;
     }
    );


TEST(
    GetValue, Basic, 0.0f,
    // initialisation
    {
      tlssInitContext(&m_data.context);
      tlssLoad(m_data.context, validData, &m_data.grid);
    },
    // cleanup
    { tlssReleaseGrid(m_data.context, &m_data.grid); },
    // test
    {
      tlssDigit val = 99;
      ASSERT(tlssGetValue(NULL, m_data.grid, 0, 0, &val) == TLSS_NO_CONTEXT);
      ASSERT(val == 99);
      ASSERT(tlssGetValue(m_data.context, NULL, 0, 0, &val) == TLSS_NO_GRID);
      ASSERT(val == 99);
      ASSERT(tlssGetValue(m_data.context, m_data.grid, 100, 0, &val) ==
             TLSS_INVALID_DATA);
      ASSERT(val == 99);
      ASSERT(tlssGetValue(m_data.context, m_data.grid, 0, 100, &val) ==
             TLSS_INVALID_DATA);
      ASSERT(val == 99);
      ASSERT(tlssGetValue(m_data.context, m_data.grid, 0, 0, NULL) ==
             TLSS_INVALID_DATA);
      ASSERT(val == 99);

      for (tlssIndex x = 0; x < 9; ++x) {
        for (tlssIndex y = 0; y < 9; ++y) {
          ASSERT(tlssGetValue(m_data.context, m_data.grid, y, x, &val) ==
                 TLSS_SUCCESS);
          ASSERT(val == validData[(y * 9) + x]);
        }
      }
    },
    // data
    {
      tlssGrid *grid;
      tlssContext *context;
    });

TEST(SetValue, Basic, 0.0f,
     //initialisation
     {
	 tlssInitContext(&m_data.context);
	 tlssLoad(m_data.context, incompleteData, &m_data.grid);
     },
     // cleanup
     {
	 tlssReleaseGrid(m_data.context, &m_data.grid);
	 tlssTerminateContext(&m_data.context);
     },
     //test
     {
	 tlssGrid* nextGrid = NULL;
	 ASSERT(tlssSetValue(NULL, m_data.grid, 5, 6, 1, &nextGrid) == TLSS_NO_CONTEXT);
	 ASSERT(nextGrid == NULL);
	 ASSERT(tlssSetValue(m_data.context, NULL, 5, 6, 1, &nextGrid) == TLSS_NO_GRID);
	 ASSERT(nextGrid == NULL);
	 ASSERT(tlssSetValue(m_data.context, m_data.grid, 5, 6, 1, NULL) == TLSS_NO_GRID);
	 ASSERT(nextGrid == NULL);
	 ASSERT(tlssSetValue(m_data.context, m_data.grid, 10, 6, 1, &nextGrid) == TLSS_INVALID_DATA);
	 ASSERT(nextGrid == NULL);
	 ASSERT(tlssSetValue(m_data.context, m_data.grid, 5, 10, 1, &nextGrid) == TLSS_INVALID_DATA);
	 ASSERT(nextGrid == NULL);
	 ASSERT(tlssSetValue(m_data.context, m_data.grid, 5, 6, 10, &nextGrid) == TLSS_INVALID_DATA);
	 ASSERT(nextGrid == NULL);
	 ASSERT(tlssSetValue(m_data.context, m_data.grid, 5, 6, 1, &nextGrid) == TLSS_SUCCESS);
	 ASSERT(nextGrid != NULL);
	 ASSERT(nextGrid != m_data.grid);
	 tlssDigit val = 99;
	 tlssGetValue(m_data.context, nextGrid, 5, 6, &val);
	 ASSERT(val == 1);
	 tlssReleaseGrid(m_data.context, &nextGrid);
     },
     // data
     {
	 tlssGrid* grid;
	 tlssContext* context;
     }
    );


TEST(GridEquals, Basic, 0.0f,
     //initialisation
     {
	 tlssInitContext(&m_data.context);
	 tlssLoad(m_data.context, incompleteData, &m_data.grid);
     },
     // cleanup
     {
	 tlssReleaseGrid(m_data.context, &m_data.grid);
	 tlssTerminateContext(&m_data.context);
     },
     //test
     {
	 tlssGrid* nextGridA = NULL;
	 tlssGrid* nextGridB = NULL;
	 tlssGrid* nextGridC = NULL;
	 tlssSetValue(m_data.context, m_data.grid, 5, 6, 1, &nextGridA);
	 tlssSetValue(m_data.context, m_data.grid, 5, 6, 1, &nextGridB);
	 tlssSetValue(m_data.context, m_data.grid, 5, 6, 2, &nextGridC);

	 ASSERT(tlssGridEquals(NULL, nextGridA) == TLSS_NO_GRID);
	 ASSERT(tlssGridEquals(nextGridA, NULL) == TLSS_NO_GRID);
	 
	 ASSERT(tlssGridEquals(m_data.grid, m_data.grid) == TLSS_SUCCESS);
	 ASSERT(tlssGridEquals(m_data.grid, nextGridA) == TLSS_INVALID_GRID);
	 ASSERT(tlssGridEquals(m_data.grid, nextGridB) == TLSS_INVALID_GRID);
	 ASSERT(tlssGridEquals(m_data.grid, nextGridC) == TLSS_INVALID_GRID);
	 ASSERT(tlssGridEquals(nextGridA, nextGridB) == TLSS_SUCCESS);
	 ASSERT(tlssGridEquals(nextGridA, nextGridC) == TLSS_INVALID_GRID);
	 ASSERT(tlssGridEquals(nextGridB, nextGridC) == TLSS_INVALID_GRID);
	 

	 tlssReleaseGrid(m_data.context, &nextGridC);
	 tlssReleaseGrid(m_data.context, &nextGridB);
	 tlssReleaseGrid(m_data.context, &nextGridA);
     },
     // data
     {
	 tlssGrid* grid;
	 tlssContext* context;
     }
    );


TEST(MergeGrid, Basic, 0.0f,
     //initialisation
     {
	 tlssInitContext(&m_data.context);
	 tlssLoad(m_data.context, incompleteData, &m_data.grid);
     },
     // cleanup
     {
	 tlssReleaseGrid(m_data.context, &m_data.grid);
	 tlssTerminateContext(&m_data.context);
     },
     //test
     {
	 tlssGrid* nextGridA = NULL;
	 tlssGrid* nextGridB = NULL;
	 tlssGrid* nextGridC = NULL;
	 tlssSetValue(m_data.context, m_data.grid, 3, 5, 1, &nextGridA);
	 tlssSetValue(m_data.context, m_data.grid, 4, 0, 8, &nextGridB);
	 tlssSetValue(m_data.context, m_data.grid, 4, 0, 9, &nextGridC);

	 tlssGrid* out = NULL;
	 ASSERT(tlssGridMerge(NULL, nextGridA, nextGridB, &out) == TLSS_NO_CONTEXT);
	 ASSERT(out == NULL);
	 ASSERT(tlssGridMerge(m_data.context, NULL, nextGridB, &out) == TLSS_NO_GRID);
	 ASSERT(out == NULL);
	 ASSERT(tlssGridMerge(m_data.context, nextGridA, NULL, &out) == TLSS_NO_GRID);
	 ASSERT(out == NULL);
	 ASSERT(tlssGridMerge(m_data.context, nextGridA, nextGridB, NULL) == TLSS_NO_GRID);
	 ASSERT(out == NULL);

	 ASSERT(tlssGridMerge(m_data.context, nextGridB, nextGridC, &out) == TLSS_INVALID_DATA);
	 ASSERT(out == NULL);
	 
	 ASSERT(tlssGridMerge(m_data.context, nextGridA, nextGridB, &out) == TLSS_SUCCESS);
	 ASSERT(out != NULL);
	 tlssDigit digitA = 99;
	 tlssDigit digitB = 99;
	 tlssGetValue(m_data.context, out, 3, 5, &digitA);
	 tlssGetValue(m_data.context, out, 4, 0, &digitB);
	 ASSERT(digitA == 1 && digitB == 8);
	 tlssReleaseGrid(m_data.context, &out);
	 tlssReleaseGrid(m_data.context, &nextGridA);
	 tlssReleaseGrid(m_data.context, &nextGridB);
     },
     // data
     {
	 tlssGrid* grid;
	 tlssContext* context;
     }
    );


TEST(Step, Basic, 0.0f,
     //initialisation
     {
	 tlssInitContext(&m_data.context);
	 tlssLoad(m_data.context, invalidData, &m_data.invalidGrid);
	 tlssLoad(m_data.context, incompleteData, &m_data.validGrid);
     },
     // cleanup
     {
	 tlssReleaseGrid(m_data.context, &m_data.invalidGrid);
	 tlssReleaseGrid(m_data.context, &m_data.validGrid);
	 tlssTerminateContext(&m_data.context);
     },
     //test
     {
	 tlssGrid* out = NULL;
	 ASSERT(tlssStep(NULL, m_data.validGrid, &out) == TLSS_NO_CONTEXT);
	 ASSERT(out == NULL);
	 ASSERT(tlssStep(m_data.context, NULL, &out) == TLSS_NO_GRID);
	 ASSERT(out == NULL);
	 ASSERT(tlssStep(m_data.context, m_data.validGrid, NULL) == TLSS_NO_GRID);
	 ASSERT(out == NULL);
	 ASSERT(tlssStep(m_data.context, m_data.invalidGrid, &out) == TLSS_INVALID_GRID);
	 ASSERT(out == NULL);

	 ASSERT(tlssStep(m_data.context, m_data.validGrid, &out) == TLSS_SUCCESS);
	 ASSERT(out != NULL);
	 ASSERT(tlssGridEquals(m_data.validGrid, out) == TLSS_INVALID_GRID);
	 tlssReleaseGrid(m_data.context, &out);
     },
     // data
     {
	 tlssGrid* validGrid;
	 tlssGrid* invalidGrid;
	 tlssContext* context;
     }
    );

TEST(Solve, Basic, 0.0f,
     //initialisation
     {
	 tlssInitContext(&m_data.context);
	 tlssLoad(m_data.context, invalidData, &m_data.invalidGrid);
	 tlssLoad(m_data.context, incompleteData, &m_data.validGrid);
	 tlssLoad(m_data.context, validData, &m_data.completeGrid);
     },
     // cleanup
     {
	 tlssReleaseGrid(m_data.context, &m_data.completeGrid);
	 tlssReleaseGrid(m_data.context, &m_data.invalidGrid);
	 tlssReleaseGrid(m_data.context, &m_data.validGrid);
	 tlssTerminateContext(&m_data.context);
     },
     //test
     {
	 tlssGrid* out = NULL;
	 ASSERT(tlssSolve(NULL, m_data.validGrid, &out) == TLSS_NO_CONTEXT);
	 ASSERT(out == NULL);
	 ASSERT(tlssSolve(m_data.context, NULL, &out) == TLSS_NO_GRID);
	 ASSERT(out == NULL);
	 ASSERT(tlssSolve(m_data.context, m_data.validGrid, NULL) == TLSS_NO_GRID);
	 ASSERT(out == NULL);
	 ASSERT(tlssSolve(m_data.context, m_data.invalidGrid, &out) == TLSS_INVALID_GRID);
	 ASSERT(out == NULL);

	 ASSERT(tlssSolve(m_data.context, m_data.validGrid, &out) == TLSS_SUCCESS);
	 ASSERT(out != NULL);
	 ASSERT(tlssGridEquals(m_data.validGrid, out) == TLSS_INVALID_GRID);
	 ASSERT(tlssGridEquals(m_data.completeGrid, out) == TLSS_SUCCESS);
	 tlssReleaseGrid(m_data.context, &out);
     },
     // data
     {
	 tlssGrid* validGrid;
	 tlssGrid* invalidGrid;
	 tlssGrid* completeGrid;
	 tlssContext* context;
     }
    );

size_t totalalloc = 0;
size_t maxalloc = 0;

void* testAlloc(size_t size)
{
    totalalloc += size;
    if(totalalloc > maxalloc) maxalloc = totalalloc;
    void* ret = malloc(size + sizeof(size_t));
    (*(size_t*)ret) = size;
    return &((size_t*)ret)[1];
}
void testFree(void* ptr)
{
    ptr = &((size_t*)ptr)[-1];
    totalalloc -= *(size_t*)ptr;
    free(ptr);
}


TEST(Memory, Basic, 0.0f,
     //initialisation
     {
	 totalalloc = 0;
	 maxalloc = 0;
	 tlssSetAlloc(testAlloc);
	 tlssSetFree(testFree);
     },
     // cleanup
     {
	 printf("\tHigh Water: %zu Bytes\n", maxalloc);
	 tlssSetAlloc(NULL);
	 tlssSetFree(NULL);
     },
     //test
     {
	 ASSERT(totalalloc == 0);
	 size_t start_size = totalalloc;
	 tlssInitContext(&m_data.context);
	 ASSERT(totalalloc != 0);
	 tlssLoad(m_data.context, incompleteData, &m_data.grid);
	 tlssGrid* out;
	 tlssSolve(m_data.context, m_data.grid, &out);
	 tlssReleaseGrid(m_data.context, &out);
	 tlssReleaseGrid(m_data.context, &m_data.grid);
	 tlssTerminateContext(&m_data.context);
	 ASSERT(totalalloc == start_size);
     },
     // data
     {
	 tlssGrid* grid;
	 tlssContext* context;
     }
    );


TEST(Dummy, Basic, 0.0f,
     //initialisation
     {
	 tlssInitContext(&m_data.context);
	 tlssLoad(m_data.context, incompleteData, &m_data.grid);
     },
     // cleanup
     {
	 tlssReleaseGrid(m_data.context, &m_data.grid);
	 tlssTerminateContext(&m_data.context);
     },
     //test
     {
     },
     // data
     {
	 tlssGrid* grid;
	 tlssContext* context;
     }
    );
