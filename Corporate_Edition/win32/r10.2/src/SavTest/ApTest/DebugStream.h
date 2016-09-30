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

#include <windows.h>

/**
 * DebugStream is an ostream that can be enabled and disabled.  It points to cout's streambuf.  It's useful for
 * lines of output that you only want written when a debug flag is set.  To use:
 *		DebugStream debug;
 *		debug.Enable();
 *		debug << "Here is some debug output." << std::endl;
 * Typically you would only call Enable() if a user-supplied debug flag was set.  If Enable() is not called, or
 * Disabled() is called, all output sent to the DebugStream object is ignored.
 */
class DebugStream : public std::ostream
{
public:
	DebugStream(void);
	~DebugStream(void);

	void Enable();
	void Disable();
	void SetLevel();

protected:
	bool m_enabled;

	// Declared but not implemented to prevent callers from using default implementations
	DebugStream(const DebugStream& oRhs);
	DebugStream& operator= (const DebugStream&);
};
