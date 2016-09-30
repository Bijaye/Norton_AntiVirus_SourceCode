// TeeStream.cpp: implementation of the TeeStream class.
//
// See TeeStream.h for a class description.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "teestream.h"

//////////////////////////////////////////////////////////////////////
// TeeStream( void )
//
// Default constructor.
//////////////////////////////////////////////////////////////////////
TeeStream::TeeStream( void ) :
m_streambuf1( 0 ), 
m_streambuf2( 0 ) 
{
}

//////////////////////////////////////////////////////////////////////
// TeeStream( std::streambuf* streambuf1, std::streambuf* streambuf2 )
//
// Parameters:
//	streambuf1 - streambuf that takes TeeStream's redirected output
//	streambuf2 - streambuf that takes TeeStream's redirected output
//
// Two streambuf constructor.
//////////////////////////////////////////////////////////////////////
TeeStream::TeeStream( std::streambuf* streambuf1, std::streambuf* streambuf2 ) : 
m_streambuf1( streambuf1 ), 
m_streambuf2( streambuf2 ) 
{
}

//////////////////////////////////////////////////////////////////////
// ~TeeStream( void )
//
// Virtual destructor.
//////////////////////////////////////////////////////////////////////
TeeStream::~TeeStream(void)
{
}

//////////////////////////////////////////////////////////////////////
// Overflow( int c )
//
// Parameters:
//	c - character to be redirected to the 2 streambufs
//
// Returns:
//	int - the input character or EOF on an error
//
// This is a virtual override of streambuf::overflow.  It sends the
// specified character to each streambuf.  You do not have to call
// this directly.  It is called naturally when you use the stream
// operators (<< or >>).
//////////////////////////////////////////////////////////////////////
int TeeStream::overflow( int c )
{
	if ( NULL == m_streambuf1 || NULL == m_streambuf2 )
	{
		return EOF;
	}

	if ( m_streambuf1->sputc( c ) == EOF || m_streambuf2->sputc( c ) == EOF )
	{
		return EOF;
	}
	else
	{
		return c;
	} 
}

//////////////////////////////////////////////////////////////////////
// SetStreamBufs( std::streambuf* streambuf1, std::streambuf* streambuf2 )
//
// Parameters:
//	streambuf1 - streambuf that takes TeeStream's redirected output
//	streambuf2 - streambuf that takes TeeStream's redirected output
//
// Set the two streambuf member pointers.
//////////////////////////////////////////////////////////////////////
void TeeStream::SetStreamBufs( std::streambuf* streambuf1, std::streambuf* streambuf2 )
{
	m_streambuf1 = streambuf1;
	m_streambuf2 = streambuf2;
}
