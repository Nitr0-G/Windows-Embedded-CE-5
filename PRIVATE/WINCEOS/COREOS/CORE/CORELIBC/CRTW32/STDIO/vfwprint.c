//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// This source code is licensed under Microsoft Shared Source License
// Version 1.0 for Windows CE.
// For a copy of the license visit http://go.microsoft.com/fwlink/?LinkId=3223.
//
/***
*vfwprint.c - fwprintf from variable arg list
*
*
*Purpose:
*   defines vfwprintf() - print formatted output, but take args from
*   a stdargs pointer.
*
*******************************************************************************/

#include <cruntime.h>
#include <stdio.h>
#include <dbgint.h>
#include <stdarg.h>
#include <file2.h>
#include <internal.h>
#include <mtdll.h>

/***
*int vfwprintf(stream, format, ap) - print to file from varargs
*
*Purpose:
*   Performs formatted output to a file.  The arg list is a variable
*   argument list pointer.
*
*Entry:
*   FILEX *stream - stream to write data to
*   wchar_t *format - format string containing data format
*   va_list ap - variable arg list pointer
*
*Exit:
*   returns number of correctly output wide characters
*   returns negative number if error occurred
*
*Exceptions:
*
*******************************************************************************/

int __cdecl vfwprintf (
    FILEX *str,
    const wchar_t *format,
    va_list ap
    )
/*
 * 'V'ariable argument 'F'ile (stream) 'W'char_t 'PRINT', 'F'ormatted
 */
{
    REG1 FILEX *stream;
    REG2 int buffing;
    REG3 int retval;

    _ASSERTE(str != NULL);
    _ASSERTE(format != NULL);

    if(!CheckStdioInit())
        return 0;

    /* Init stream pointer */
    stream = str;

    if (! _lock_validate_str(stream))
        return 0;

    buffing = _stbuf(stream);
    retval = _woutput(stream,format,ap );
    _ftbuf(buffing, stream);
    _unlock_str(stream);

    return(retval);
}

int __cdecl vwprintf(const wchar_t *format, va_list ap)
{
    if(!CheckStdioInit())
        return 0;
    return vfwprintf(stdout, format, ap);
}

int __cdecl fwprintf(FILEX *str, const wchar_t *format, ...)
{
    va_list(arglist);
    va_start(arglist, format);
    return vfwprintf(str, format, arglist);
}

int __cdecl wprintf(const wchar_t *format, ...)
{
    va_list arglist;
    va_start(arglist, format);
    if(!CheckStdioInit())
        return 0;
    return vfwprintf(stdout, format, arglist);
}

