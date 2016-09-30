// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#ifndef __MEMEXCP_H__
#define __MEMEXCP_H__

#include <windows.h>

// A CMyMemoryException is thrown when an out of memory error occurs.
 
class CMyMemoryException
{
public :
	size_t	size;

	CMyMemoryException(size_t s) : size(s)	{ }
	CMyMemoryException() : size(0) { }
};

// proto types for global new operators that throw CMyMemoryException -- from mynew.cpp
void *operator new(size_t cb);
void *operator new(size_t cb, int bThrow);	// bThrow == false, won't throw exception.
void operator delete(void *p);
void operator delete(void *p, int bThrow);

#endif //__MEMEXCP_H__