////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined AVPATHPROVIDER_H
#define AVPATHPROVIDER_H

#include <ccSymInstalledApps.h>

class CAVPathProvider
{
public:
	static bool GetPath(LPTSTR szPath, size_t& nSize) throw()
	{
		ccLib::CString sPath;
		bool bRet = false;
		bRet = ccSym::CInstalledApps::GetNAVDirectory(sPath);

		size_t nLength = sPath.GetLength() + 1;
		if (nLength <= nSize)
			_tcsncpy(szPath, sPath.GetBuffer(), nLength);

		return bRet;
	}
};

#endif // !defined AVPATHPROVIDER_H
