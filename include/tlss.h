#pragma once
#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__
#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

#ifdef TLSS_DYNAMIC
# ifdef WIN32
#  ifdef TLSS_BUILD
#   define TLSS_EXPORT __declspec( dllexport )
#  else
#   define TLSS_EXPORT __declspec( dllimport )
#  endif
# endif
#endif
 
#ifndef TLSS_EXPORT
# define TLSS_EXPORT
#endif

typedef enum
{
    TLSS_SUCCESS,
    TLSS_NO_CONTEXT,
    TLSS_NO_GRID,
    TLSS_INVALID_DATA,
    TLSS_INVALID_GRID,
    TLSS_INCOMPLETE_DATA,
} tlssReturn;

typedef struct _tlssContext tlssContext;
typedef struct _tlssGrid    tlssGrid;
    
typedef unsigned char tlssDigit;
typedef unsigned char tlssIndex;

TLSS_EXPORT tlssReturn   tlssInitContext     (tlssContext** context);
TLSS_EXPORT tlssReturn   tlssTerminateContext(tlssContext** context);

TLSS_EXPORT tlssReturn   tlssLoad            (tlssContext* context, const tlssDigit* data, tlssGrid** grid);
TLSS_EXPORT tlssReturn   tlssReleaseGrid     (tlssContext* context, tlssGrid** grid);
TLSS_EXPORT tlssReturn   tlssValidateGrid    (tlssGrid* grid);

TLSS_EXPORT tlssReturn   tlssGetValue        (tlssContext* context, tlssGrid* grid, tlssIndex y, tlssIndex x, tlssDigit* v);
TLSS_EXPORT tlssReturn   tlssSetValue        (tlssContext* context, tlssGrid* in, tlssIndex y, tlssIndex x, tlssDigit v, tlssGrid** out);

TLSS_EXPORT tlssReturn   tlssGridEquals      (tlssGrid* a, tlssGrid* b);
TLSS_EXPORT tlssReturn   tlssGridMerge       (tlssContext* context, tlssGrid* a, tlssGrid* b, tlssGrid** out);
    
TLSS_EXPORT const char*  tlssError();

#ifdef __cplusplus
}
#endif/*__cplusplus*/
#endif/*__TEMPLATE_H__*/
