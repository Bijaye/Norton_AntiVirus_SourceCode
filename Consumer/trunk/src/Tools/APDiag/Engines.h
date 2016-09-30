////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(ENGINES_H__INCLUDED_)
#define ENGINES_H__INCLUDED_


// This function will use hawkings to get the current virus defs
// path and return it in the first parameter
bool GetCurrentVirusDefsFolder(LPTSTR,DWORD);

class CCheckNaveng : public CDriverCheck
{
public:
	CCheckNaveng();

	// Installs Naveng registry keys
	bool Install();

private:

	bool VerifyStartType();
	bool ExtraChecks();

};

class CCheckNavex : public CDriverCheck
{
public:
	CCheckNavex();

	// Installs Navex registry keys
	bool Install();

private:

	bool VerifyStartType();
	bool ExtraChecks();

};

#endif // !defined(ENGINES_H__INCLUDED_)