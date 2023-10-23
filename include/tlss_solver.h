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
#pragma once
#ifndef __TLSS_SOLVER_H__
#define __TLSS_SOLVER_H__
#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

#ifndef __TLSS_SOLVER_H__
    #error Do not include "tlss_solver.h" directly. Include "tlss.h"
#endif


typedef unsigned short tlssAvailability;
typedef enum
{
    TLSS_AVAIL_1 = 1<<0,
    TLSS_AVAIL_2 = 1<<1,
    TLSS_AVAIL_3 = 1<<2,
    TLSS_AVAIL_4 = 1<<3,
    TLSS_AVAIL_5 = 1<<4,
    TLSS_AVAIL_6 = 1<<5,
    TLSS_AVAIL_7 = 1<<6,
    TLSS_AVAIL_8 = 1<<7,
    TLSS_AVAIL_9 = 1<<8,
    TLSS_AVAIL_ALL = (TLSS_AVAIL_1 | TLSS_AVAIL_2 | TLSS_AVAIL_3 | TLSS_AVAIL_4 | TLSS_AVAIL_5 | TLSS_AVAIL_6 | TLSS_AVAIL_7 | TLSS_AVAIL_8 | TLSS_AVAIL_9),
} tlssAvailabilityMask;


typedef enum
{
    TLSS_META_AVAILABILITY,

    TLSS_META_USERNEXT, // User defined solvers can add metadata from here
} tlssSolverMetaID;

typedef void* tlssMetaPtr;
    
typedef tlssReturn(*tlssSolverFunc)(tlssContext*context, tlssGrid* in, tlssGrid** out);
typedef tlssReturn(*tlssMetaFreeFunc)(tlssMetaPtr meta);
typedef tlssReturn(*tlssMetaAllocFunc)(tlssMetaPtr* meta);


TLSS_EXPORT tlssReturn   tlssRegisterMetaType(tlssContext* context, tlssSolverMetaID id, tlssMetaAllocFunc allocfunc, tlssMetaFreeFunc freefunc);
TLSS_EXPORT tlssReturn   tlssAddSolver       (tlssContext* context, tlssSolverFunc* solver);


    
TLSS_EXPORT tlssReturn tlssSimpleSolver(tlssContext* context, tlssGrid* in, tlssGrid** out);
#ifdef __cplusplus
}
#endif/*__cplusplus*/
#endif/*__TLSS_H__*/
