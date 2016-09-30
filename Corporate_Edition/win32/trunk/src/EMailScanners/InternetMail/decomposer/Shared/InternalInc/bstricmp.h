// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef BSTRICMP_H
#define BSTRICMP_H

#ifndef BSIC_DECLSPEC
#define BSIC_DECLSPEC extern
#endif

/* Returns 0 on a match, any other number on a mismatch. Does
not determine which string is greater or lesser. */

#ifdef __cplusplus
extern "C" {
#endif

BSIC_DECLSPEC int binary_stricmp(const char *var_str, const char *const_str1,
  const char *const_str2);

BSIC_DECLSPEC int binary_strnicmp(const char *var_str, const char *const_str1,
  const char *const_str2, size_t count);

#ifdef __cplusplus
}
#endif

#endif
