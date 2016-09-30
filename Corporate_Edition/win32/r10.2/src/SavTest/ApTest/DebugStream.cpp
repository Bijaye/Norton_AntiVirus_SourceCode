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
#include "StdAfx.h"
#include ".\DebugStream.h"

//The base class (ostream) constructor is called with null in the
//initialization list
DebugStream::DebugStream(void):
std::ostream( 0 ),
m_enabled( false )
{
}

DebugStream::~DebugStream(void)
{
	rdbuf( 0 );
}

/**
 * Enables the stream.
 */
void DebugStream::Enable()
{
	m_enabled = true;
	rdbuf( std::cout.rdbuf() );
}

/**
 * Disables the stream.
 */
void DebugStream::Disable()
{
	m_enabled = false;
	rdbuf( 0 );
}

/**
 * Set the severity of debug messages to include.
 */
void DebugStream::SetLevel()
{
	//Not implemented
}