// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(__ASC_EBC_H__)
#define __ASC_EBC_H__

#include <string.h>					
#include "dec_assert.h"
									
/* ASCII to native conversion table. This table correctly
handles all printable characters as well as those control
characters with character-set-neutral escape sequences on
any platform, whether ASCII, EBCDIC, or other. Characters
that are not in our set are translated to dots. */

static char asc_to_native[256] = {
	'\x00', '.', '.', '.', '.', '.', '.', '\a', '\b', '\t',
	'\n', '\v', '\f', '\r', '.', '.', '.', '.', '.', '.',
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.',
	'.', '.', ' ', '!', '\"', '#', '$', '%', '&', '\'',
	'(', ')', '*', '+', ',', '-', '.', '/', '0', '1',
	'2', '3', '4', '5', '6', '7', '8', '9', ':', ';',
	'<', '=', '>', '?', '@', 'A', 'B', 'C', 'D', 'E',
	'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
	'Z', '[', '\\', ']', '^', '_', '`', 'a', 'b', 'c',
	'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
	'x', 'y', 'z', '{', '|', '}', '~', '.', '.', '.',
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.',
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.',
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.',
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.',
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.',
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.',
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.',
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.',
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.',
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.',
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.',
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.',
	'.', '.', '.', '.', '.', '.'
};

static void ascii_to_native(char *lpString)
{
	char *p;

	dec_assert(lpString != NULL);

	for(p = lpString; *p != 0; ++p) {
		*p = asc_to_native[*((unsigned char*)p)];
	}
}

#endif	// __ASC_EBC_H__

