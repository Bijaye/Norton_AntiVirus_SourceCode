// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "pscan.h"
//#include "clibnlm.h"
//#include <malloc.h>

#define FAR
#define NULL 0
typedef unsigned int size_t;


/*****************************************************************************/

#undef realloc
#undef malloc
#undef free
#undef calloc

void FAR *MyNLMRealloc(void *old,size_t size, char *file, int line) {

	if(old==NULL)
		return malloc(size);
	return realloc(old,size);

}
void FAR *MyNLMCalloc( size_t count, size_t size, char* file, int line)
{
	return calloc(count, size);
//	char* ptr = NULL;
//	ptr = calloc(count, size);
//	return ptr;
}
// void  FAR *MyNLMMalloc(size_t size, char *file, int line) {  // gdf CW conversion
void  FAR *MyNLMMalloc(int size, char *file, int line) {  // gdf CW conversion
		return malloc(size);
//	char *ptr;

//		ptr = malloc(size);
//		return ptr;

}

void MyNLMFree(void FAR *in, char *file, int line) {

	if (!in) return;
	
		free(in);
		return;
}

void FAR xyzree( void *in )
{
	return MyNLMFree( in, __FILE__, __LINE__ );
}

void FAR *xealloc( void *old, size_t size )
{
	return MyNLMRealloc( old, size, __FILE__, __LINE__ );
}

void FAR *zalloc( size_t size )
{
	return MyNLMMalloc( size, __FILE__, __LINE__ );
}

void FAR *yalloc( size_t count, size_t size )
{
	return MyNLMCalloc(count, size, __FILE__, __LINE__);
}
