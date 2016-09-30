////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/***********************************************************************************************
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 * and associated documentation files (the "Software"),to redistribution and use in source and binary 
 * forms, with or without modification, are permitted, to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, 
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL STAHL SOFTWARE BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Stahl Software shall not be used in advertising 
 * or otherwise to promote the sale, use or other dealings in this Software without prior written 
 * authorization from Stahl Software.
 * 
 *************************************************************************************************/	
// SRX.h (StahlSoft HRX.h, modified to use SymInterface SYMRESULTs)
//
//////////////////////////////////////////////////////////////////////

#if !defined SRX_H
#define SRX_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <exception>

#include "SymInterface.h"

class srx_exception : public std::exception
{
	public:

	srx_exception(SYMRESULT sr) : m_sr(sr)
	{
	}

	SYMRESULT Error(void)
	{
		return m_sr;
	}

	protected:

	SYMRESULT m_sr;
};

struct SRX
{
	SRX():m_srLastError(SYM_OK),m_bSYM_OK_Is_Only_Success(false){}
	SRX(SYMRESULT sr):m_bSYM_OK_Is_Only_Success(false)
	{
		if(		
				SYM_FAILED(sr) 
			|| ( m_bSYM_OK_Is_Only_Success?(sr != SYM_OK):false )
		  )
		{
			throw srx_exception(sr);
		}
	}

	SRX& operator<<(SYMRESULT sr)
	{
		m_srLastError = sr;
		if(		
				SYM_FAILED(sr) 
			|| ( m_bSYM_OK_Is_Only_Success?(sr != SYM_OK):false )
		  )
		{
			throw srx_exception(sr);
		}
		return *this;
	}
	SYMRESULT GetLastError(){return m_srLastError;}
	bool m_bSYM_OK_Is_Only_Success;
private:
	// DONT ALLOW THIS OPERATOR
	SRX& operator=(SYMRESULT /* sr unused */)
	{
		// DONT ALLOW THIS OPERATOR
		return *this;
	}
	SYMRESULT m_srLastError;
};

#define SYMINTERFACE_SRX_TRY(srVar) \
	SYMRESULT srVar = SYM_OK; \
	SRX srx; \
	try

#define SYMINTERFACE_SRX_CATCH_ALL_NO_RETURN(srVar) \
	catch(srx_exception& e){srVar = e.Error();} \
	catch(...){srVar = SYMERR_UNKNOWN;}

#define SYMINTERFACE_SRX_CATCH_ALL_RETURN(srVar) \
		SYMINTERFACE_SRX_CATCH_ALL_NO_RETURN(srVar)\
		return srVar;


#endif // !defined SRX_H
