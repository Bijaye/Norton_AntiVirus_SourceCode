// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
//////////////////////////////////////////////////////////////////////
// TeeStream.h: interface for the TeeStream class.
//

//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEESTREAM_H__2E7C2D8A_C82B_46F1_98DB_7F6CB6384EA8__INCLUDED_)
#define AFX_TEESTREAM_H__2E7C2D8A_C82B_46F1_98DB_7F6CB6384EA8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <streambuf>

/**
 * TeeStream is a streambuf that redirects output to 2 user-supplied
 * streambufs.  It is intended for use in test tools as a replacement
 * for cout's default streambuf.  This way, everything that goes to
 * cout can be redirected to the screen and a log file automatically.
 */
class TeeStream : public std::streambuf
{
public:
	TeeStream( void );
	TeeStream( std::streambuf* streambuf1, std::streambuf* streambuf2 );
	virtual ~TeeStream( void );
	virtual void SetStreamBufs( std::streambuf* streambuf1, std::streambuf* streambuf2 );

protected:
	virtual int overflow( int c );

private:
	std::streambuf* m_streambuf1;
    std::streambuf* m_streambuf2; 
};

#endif // !defined(AFX_TEESTREAM_H__2E7C2D8A_C82B_46F1_98DB_7F6CB6384EA8__INCLUDED_)