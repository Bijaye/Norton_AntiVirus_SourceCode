// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/
// mynew.cpp : Alan Butt
//
// This function implements a global operator new() function to be used in exception safe .DLLs.
// The operator allocates the memory and returns the resultant pointer.  If an out of memory
// condition occurs, the exception CMyMemoryExeception is thrown.
//
// If exception handling (try/catch) is to be used with the new operator in a .DLL several
// problems need to be addressed.  Below is a description of these problems:
//
// Other memory exception standards:
//
// 1) At the time of this writing, ANSI has yet to complete the definition of exception "xalloc".
//
// 2) The STL definition of new (from header file <new> (no .h)) defines yet another exception
//	to be thrown called "bad_alloc".
//
// Both of the above methods work by calling set_new_handler() or _set_new_handler().  The
// new handler simply throws the appropriate exception.  Both of these function save a pointer
// to the new handler in a static variable.  When the underlying memory allocator fails, the
// saved function pointer is called (and the exception is thrown as a result).
//
// Likewise, it is possible to write my own new handler which would throw CMyMemoryException.
// The use of CMyMemoryException as the out of memory exception is simply because I don't use
// the STL in this .DLL and the ANSI version is not yet fully specified.  Either of these exception
// classes could be used.  At some future time I may change the global operator new() defined here
// to use either of the "standard" exception classes.
//
// The PROBLEM here is that the pointer to the handler function MAY NOT BE SPECIFIC TO THE .DLL.
// If the run time .DLLs are used, all modules using the run time library share the single instance
// of the pointer.  So if this .DLL changes the pointer to use either the "standard" exception
// memory exception handlers or a private one, all other modules (.DLLs and the .EXE) that use the
// run time library will also begin throwing exceptions on out of memory conditions.  Since it is
// possible (even likely) that the other modules are not prepared to handle the same kind of exception
// throwing, a variety of nasty bugs can (will) occur.
//
// Possible solutions to the above problem:
//
// 1) Don't use exception handling.  If exception handling isn't used, you always need to check the
//	results of the new operator for NULL and handle the out of memory condition directly.  This is
//	what all good programmers have been doing for years so its really not that bad.
//
// 2) Always link .DLLs with the static libraries.  If done, you get the overhead of the static libraries
//	and you can't share library objects (like file handles, iostreams, etc.) between the .DLL and other
//	modules in the process.  If you use the static libraries, you should put preprocessor statements in the 
//	code (where you set the new handler) that will not allow the code to compile if the run time library is
//	used.  Someday, someone else will change the project options to build the code using the run time library
//	and all of the obsure bugs will start occuring and no one will know why.  This check would look something
//	like:
//
//	#if defined(_DLL)
//	#error This code MUST be compiled with the static libraries--because it sets a new handler
//	#endif
//	set_new_handler(...)
//
// 3) Override the global new operator.  This is the method taken here.  This will work with either the
//	static or the runtime .DLL.  The default global new operator calls _nh_malloc() to actually
//	allocate the memory.  It turns out that malloc() also calls _nh_malloc() to allocate the memory
//	(actually, there are a few more calls involved, but the effect is that both malloc() and the new
//	operator really do the same thing).  As always, you should never leave the memory exception unhandled.
//
//	Any memory allocation that fails will throw the exception and it must be caught (all 'new's
//	must be in a try/catch block).  Using the set_new_handler() you have the flexablity of changing
//	from exception handling to checking the null pointer at run time.  The global new operator is a
//	compile time decision.  It effects ALL calls to global new--calls to new not in a try/catch block
//	will still throw the exception.
//
//	To get around this problem, another global new operator has been created that takes an additional
//	argument (bThrow).  If bThrow is false, an exception will not be thrown.  The prototype to this
//	version of operator new must be declared globally.
//
//	char *x = new(false) char[n];	// will not throw an exception if out of memory.
//

#include <new.h>
#include <malloc.h>

#include "memexcp.h"

#pragma warning(disable: 4100)

//4100: unreferenced formal parameter


unsigned long _g_cNew_ = 0;			// These counts may be useful for tracking down memory leaks
unsigned long _g_cFree_ = 0;

void *operator new(size_t cb)
{
	void
		*rv = malloc(cb);

	if (!rv)
		throw CMyMemoryException(cb);

	_g_cNew_++;

	return rv;
}

void *operator new(size_t cb, int bThrow)	// bThrow == false, won't throw exception.
{
	void
		*rv = malloc(cb);

	if (!rv && bThrow)
		throw CMyMemoryException(cb);

	if (rv)
		_g_cNew_++;

	return rv;
}


void operator delete(void *p)
{
	if (p)
		_g_cFree_++;

    free(p);
}


void operator delete(void *p, int bThrow)
{
	if (p)
		_g_cFree_++;

    free(p);
}

/* end source file */