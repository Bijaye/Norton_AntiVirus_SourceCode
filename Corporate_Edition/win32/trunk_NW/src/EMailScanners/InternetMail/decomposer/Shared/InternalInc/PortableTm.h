// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//
// Portable time conversion functions
//

#ifndef _PORTABLETM_H_
#define _PORTABLETM_H_

#include "DecPlat.h"
#include <time.h>

void MyUnixTimeToFileTime(time_t t, LPFILETIME pft);
void MyFileTimeToUnixTime(const FILETIME *file_time, time_t *unix_time);
DWORD big_divide(DWORD low, DWORD high, DWORD divisor);
int highest_one_bit(DWORD val);
void shift_subtract(DWORD *low, DWORD *high, int bits, DWORD num);

#endif
