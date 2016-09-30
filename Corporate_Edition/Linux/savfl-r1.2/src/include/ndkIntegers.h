// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Linux implementation of NDK Integer Handling functions

#ifndef _ndkIntegers_h_
#define _ndkIntegers_h_

#ifdef __cplusplus
extern "C" {
#endif

unsigned long _lrotl(unsigned long value,int shift);
unsigned long _lrotr(unsigned long value,int shift);

char* itoa(				int  value,char* string,int radix);
char* ltoa(				long value,char* string,int radix);
char* ultoa(unsigned long value,char* string,int radix);

#ifndef max
	#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
	#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifdef __cplusplus
}
#endif
	
#endif // _ndkIntegers_h_
