// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Context switch for calling Clib NLMs from libc based ScsComms

#include "libcnlm.h"
#include "DebugPrint.h"
#include "winThreads.h"

#include "nts_legacy.h"

#ifdef SYM_LIBC_NLM
extern SYMNTS_FP_BLOCK SymNTS_FP_Block;
#endif

HANDLE ClibContext::mutex = 0;

void ClibContext::mk() 
{
	mutex = CreateMutex(NULL, false, NULL);
}

void ClibContext::rm() 
{

	CloseHandle(mutex);

	mutex = 0;
}

ClibContext::ClibContext()
{
	DebugPrint(DEBUGFLAG_CLIB_LIBC,"ClibContext::ClibContext\n");

	if ( mutex )
	{
		WaitForSingleObject(mutex, INFINITE);
	}
	else
	{
		DebugPrint(DEBUGFLAG_CLIB_LIBC,"ClibContext::ClibContext thread group wrap failed, shutting down!\n");
	}

	int nRet = SymNTS_FP_Block.CLibThreadGroupWrap( );
	if ( nRet != 0 )
	{
		DebugPrint(DEBUGFLAG_CLIB_LIBC,"ClibContext::ClibContext thread group wrap failed!\n");
	}
}

ClibContext::~ClibContext() 
{
	DebugPrint(DEBUGFLAG_CLIB_LIBC,"ClibContext::~ClibContext\n");

	int nRet = SymNTS_FP_Block.CLibThreadGroupUnwrap( );
	if ( nRet != 0 )
	{
		DebugPrint(DEBUGFLAG_CLIB_LIBC, "ClibContext::~ClibContext thread group unwrap failed!\n");
	}

	ReleaseMutex(mutex);
}
