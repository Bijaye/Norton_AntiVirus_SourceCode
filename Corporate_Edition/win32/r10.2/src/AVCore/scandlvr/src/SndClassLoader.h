// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//
// SND Class Loader
// 
//  This implementation of the loader does not support digital signature
//  validation, and attempts to load the modules from a path relative the
//  running application.
//
//  It is not suitable for a production environment.
//

#pragma once

#include "SymInterface.h"
#define _WINSOCK2API_	//To prevent the nested inclusion of winsock2.h ,which is causing redefintion of LPSOCKET_ADDRESS.
#include "ccSymInterfaceLoader.h" // Has trusted loader
#include "ccSymPathProvider.h"
#include "ccStreamInterface.h"
#include "SDPACK_i.h"
#include "QScommon.h"	//For Registry entry declarations
#include "SymSaferRegistry.h"
#include "ccLib.h"
#include "ccLibStaticLink.h"

using namespace SDPACK;

#ifdef INITEXTERN
#define DEFINE_DLL_NAME(x, y) LPCTSTR x = y;
#else
#define DEFINE_DLL_NAME(x, y) extern LPCTSTR x;
#endif

DEFINE_DLL_NAME(SZ_SNDPACK_DLL, _T("SDPCK32I.DLL"));

extern BOOL g_bUsedByCQC;

class CSNDPACKPathProvider
{
public:
  	static bool GetPath(LPTSTR szPath, size_t& nSize) throw()
  	{
  		if(g_bUsedByCQC)
  		{
  			DWORD dwType = -1;
  			HKEY	hKey = NULL;
  			DWORD	dwBufLen = nSize;
  			DWORD dwErr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGKEY_QSCONSOLE,0, KEY_READ, &hKey);
  			if(ERROR_SUCCESS != dwErr)
  				return false;
  			dwErr = SymSaferRegQueryValueEx(hKey, REGVAL_INSTALL_DIR, NULL, &dwType, (LPBYTE)szPath, &dwBufLen);
  			if(ERROR_SUCCESS == dwErr)
  				return true;
  			else
  				return false;
  		}
  		else
 		{
  			CString cszInstallDir;
 			GetModuleFileName(NULL, cszInstallDir.GetBuffer(MAX_PATH), MAX_PATH);
  			cszInstallDir.ReleaseBuffer();

 			INT iFind = cszInstallDir.ReverseFind('\\');
  			cszInstallDir = cszInstallDir.Left(iFind + 1);

  			_tcscpy(szPath, cszInstallDir);
  			return true;
  		}
  	}
};

typedef CSymInterfaceDLLHelper< &SZ_SNDPACK_DLL,
								CSNDPACKPathProvider, 
								cc::CSymInterfaceTrustedLoader,
								ISNDSubmission, 
								&IID_ISNDSubmission, 
								&IID_ISNDSubmission> SDPACKDLL_Submission_Loader;

typedef CSymInterfaceDLLHelper< &SZ_SNDPACK_DLL,
								CSNDPACKPathProvider, 
								cc::CSymInterfaceTrustedLoader,
								ISNDSample, 
								&IID_ISNDSample, 
								&IID_ISNDSample> SDPACKDLL_Sample_Loader;

