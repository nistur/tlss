#include "tlss.h"

tlssReturn g_tlssError;
const char* g_tlssErrors[] = 
{
    "Success",
    "Null context",
    "Null grid",
    "Invalid data",
};
