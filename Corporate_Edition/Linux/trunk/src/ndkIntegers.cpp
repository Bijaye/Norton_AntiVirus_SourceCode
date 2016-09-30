// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Linux implementation of NDK Integer Handling functions

// ================== ndkIntegers =======================================================

extern "C" {
	#include <stdlib.h>
	#include <limits.h>
	#include <bits/wordsize.h>

	#include <ndkIntegers.h>
}

// --------------------- Rotation -------------------------------------------------------

// The `_lrotl' and _lrotr functions rotate `value' by `shift' bits. `_lrotl' rotates the
// value left,. `_lrotr' rotates the value right. Both functions "wrap" bits rotated off
// one end of value to the other end. Note that the `shift' is normalized to be less than
// the size of the value as rotating a multiple of the size gets you back to where you
// started.

unsigned long _lrotl(unsigned long value,int shift) {
	shift &= __WORDSIZE - 1; return value << shift | value >> __WORDSIZE - shift;
}
unsigned long _lrotr(unsigned long value,int shift) {
	shift &= __WORDSIZE - 1; return value >> shift | value << __WORDSIZE - shift;
}

// --------------------- Radix Converison -----------------------------------------------

// The `itoa', `ltoa', and `ultoa' functions convert the integer `value' into the
// equivalent string in base radix notation, storing the result in the character array
// pointed to by `string'. A null-terminating character is appended to the result. The
// `radix' may be between 2 and 36. Radix 1 can only repesent one number, 0. We stop at
// 36 in order to stay within the English alphabet to represent numbers.
//
// The size of `string' must be at least (8 * sizeof(value) + 1) bytes when converting
// values in base 2.
//
// If the value of `radix' is 10 and `value' is negative for calls to `itoa' and `ltoa',,
// the first character stored in buffer is a minus sign.

static char int2char(int digit) {
	return digit < 10 ? '0' + digit : 'a' + digit - 10;
}
static char* reverse(char* string,size_t length) {
	char c; size_t middle = length / 2;

	for (size_t lower = 0,upper = length - 1; lower < middle; lower++,upper--) {
		c = string[upper]; string[upper] = string[lower]; string[lower] = c;
	}
	return string;
}

char* itoa(int value,char* string,int radix) {
	int v = abs(value); size_t i = 0;

	while (v > 0) {
		string[i++] = int2char(v % radix); v = v / radix;
	}
	if (value < 0 && radix == 10) string[i++] = '-';
	if (! value || v) string[i++] = int2char(v); string[i] = '\0';

	return reverse(string,i);
}
char* ltoa(long value,char* string,int radix) {
	long v = abs(value); size_t i = 0;

	while (v > 0) {
		string[i++] = int2char(v % radix); v = v / radix;
	}
	if (value < 0 && radix == 10) string[i++] = '-';
	if (! value || v) string[i++] = int2char(v); string[i] = '\0';

	return reverse(string,i);
}
char* ultoa(unsigned long value,char* string,int radix) {
	unsigned long v = value; size_t i = 0;

	while (v > 0) {
		string[i++] = int2char(v % radix); v = v / radix;
	}
	if (! value || v) string[i++] = int2char(v); string[i] = '\0';

	return reverse(string,i);
}
