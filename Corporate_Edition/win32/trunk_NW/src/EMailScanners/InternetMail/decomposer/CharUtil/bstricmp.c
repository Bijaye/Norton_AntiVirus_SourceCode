// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/* This file contains our implementation of binary_stricmp, which
is used to compare a variable string with a constant string in a
case-insensitive fashion. The caller provides the variable string,
and both all uppercase and all lowercase versions of the constant
string. A match is made if the variable string matches a character
from either constant string in each position. Also included is a
counted version of the function, binary_strnicmp. */

#include <string.h>
#include "dec_assert.h"

#include "bstricmp.h"

#ifndef BSIC_DECLSPEC
#define BSIC_DECLSPEC
#endif

/* Returns 0 on a match, any other number on a mismatch. Does
not determine which string is greater or lesser. */
BSIC_DECLSPEC int binary_stricmp(const char *var_str,
  const char *const_str1, const char *const_str2)
{
#ifdef _DEBUG
    // This is all just input validation.  The release
    // code below has been optimized for speed.
    size_t var_len;
    size_t const1_len;
    size_t const2_len;

    dec_assert(var_str != NULL);
    dec_assert(const_str1 != NULL);
    dec_assert(const_str2 != NULL);

    var_len = strlen(var_str);
    const1_len = strlen(const_str1);
    const2_len = strlen(const_str2);

    dec_assert(const1_len == const2_len);
    dec_assert(const1_len != 0);
    dec_assert(const2_len != 0);
#endif

    while (*var_str)
    {
        if ((*var_str != *const_str1) &&
            (*var_str != *const_str2))
            return 1;

        var_str++;
        const_str1++;
        const_str2++;
    }

    // Lastly, make sure both strings are of the same length.
    if (*const_str1)
        return 1;  // No, they aren't, so no match.

    return 0;
}

/* Returns 0 on a match, any other number on a mismatch. Does
not determine which string is greater or lesser. */
BSIC_DECLSPEC int binary_strnicmp(const char *var_str,
  const char *const_str1, const char *const_str2, size_t count)
{
#ifdef _DEBUG
    // This is all just input validation.  The release
    // code below has been optimized for speed.
    size_t var_len;
    size_t const1_len;
    size_t const2_len;

    dec_assert(var_str != NULL);
    dec_assert(const_str1 != NULL);
    dec_assert(const_str2 != NULL);

    var_len = strlen(var_str);
    const1_len = strlen(const_str1);
    const2_len = strlen(const_str2);

    dec_assert(const1_len == const2_len);
    dec_assert(const1_len != 0);
    dec_assert(const2_len != 0);
    dec_assert(count != 0);
    dec_assert(count <= const1_len);
#endif

    if (count != 0 && (*var_str != *const_str1) &&
                      (*var_str != *const_str2))
        return 1;

    while (*var_str && count)
    {
        if ((*var_str != *const_str1) &&
            (*var_str != *const_str2))
            return 1;

        var_str++;
        const_str1++;
        const_str2++;
        count--;
    }

    return 0;
}
