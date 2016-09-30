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
#pragma once

#include <string>

/*
 * Calculates checksums for files.  It is used to detect changes in files after scanning.
 * Original code is copyright © 1993 Richard Black. All rights are reserved.
 */
class Checksum
{
public:
	Checksum(void);
	virtual ~Checksum(void);
	
	virtual void Initialize();
	virtual unsigned int Calculate( LPCTSTR name );

protected:
	unsigned int m_crctab[256];

// Declared but not implemented to prevent callers from using default implementations
Checksum(const Checksum& oRhs);
Checksum& operator= (const Checksum&);
};
