////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(SAVRT_H__INCLUDED_)
#define SAVRT_H__INCLUDED_

class CCheckSavrt : public CDriverCheck
{
public:
	CCheckSavrt();

	// Installs Naveng registry keys
	bool Install();

private:

	bool VerifyStartType();
	bool ExtraChecks();
};

class CCheckSavrtPel : public CDriverCheck
{
public:
	CCheckSavrtPel();

	// Installs Naveng registry keys
	bool Install();

private:

	bool VerifyStartType();
	bool ExtraChecks();
};

#endif // !defined(SAVRT_H__INCLUDED_)