////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(SYMEVENT_H__INCLUDED_)
#define SYMEVENT_H__INCLUDED_

class CCheckSymevent : public CDriverCheck
{
public:
	CCheckSymevent();

	// Installs Symevent from the common files\symantec shared directory
	bool Install();

private:

	bool VerifyStartType();
	bool ExtraChecks();

};

#endif // !defined(SYMEVENT_H__INCLUDED_)