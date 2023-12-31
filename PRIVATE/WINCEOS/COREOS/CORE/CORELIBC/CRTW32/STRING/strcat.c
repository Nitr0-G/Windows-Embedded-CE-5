//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// This source code is licensed under Microsoft Shared Source License
// Version 1.0 for Windows CE.
// For a copy of the license visit http://go.microsoft.com/fwlink/?LinkId=3223.
//

/***
*strcat.c - contains strcat() and strcpy()
*
*Purpose:
*   Strcpy() copies one string onto another.
*
*   Strcat() concatenates (appends) a copy of the source string to the
*   end of the destination string, returning the destination string.
*
*******************************************************************************/

#include <corecrt.h>
#include <string.h>

#ifndef _MBSCAT
#ifdef  _MSC_VER
#pragma function(strcat,strcpy)
#endif
#endif

/***
*char *strcat(dst, src) - concatenate (append) one string to another
*
*Purpose:
*   Concatenates src onto the end of dest.  Assumes enough
*   space in dest.
*
*Entry:
*   char *dst - string to which "src" is to be appended
*   const char *src - string to be appended to the end of "dst"
*
*Exit:
*   The address of "dst"
*
*Exceptions:
*
*******************************************************************************/

char * __cdecl strcat (
    char * dst,
    const char * src
    )
{
    unsigned char * cp = (unsigned char*)dst;

    while( *cp )
        cp++;           /* find end of dst */

    while( *cp++ = *src++ ) ;   /* Copy src to end of dst */

    return( dst );          /* return dst */

}


/***
*char *strcpy(dst, src) - copy one string over another
*
*Purpose:
*   Copies the string src into the spot specified by
*   dest; assumes enough room.
*
*Entry:
*   char * dst - string over which "src" is to be copied
*   const char * src - string to be copied over "dst"
*
*Exit:
*   The address of "dst"
*
*Exceptions:
*******************************************************************************/

char * __cdecl strcpy(char * dst, const char * src)
{
    unsigned char * cp = (unsigned char*)dst;

    while( *cp++ = *src++ )
        ;       /* Copy src over dst */

    return( dst );
}
