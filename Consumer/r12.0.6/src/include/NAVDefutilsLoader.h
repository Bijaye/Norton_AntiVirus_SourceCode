// NAVDefutilsLoader.h

#pragma once

#include "ccLibStd.h"
#include "syminterfaceloader.h"
#include "defutilsinterface.h"
#include "ccSymCommonClientInfo.h"

#ifdef MSI_DEFUTILS
#include "msi.h"
#include "msiquery.h"
#endif

extern const LPCTSTR sz_DefUtils_dll;

class CccPathProvider
{
public:
    static bool GetPath(LPTSTR szPath, size_t& dwSize);
};

class CSymSetupPathProvider
{
public:
    static bool GetPath(LPTSTR szPath, size_t& dwSize);
};

// loader for most product component
typedef CSymInterfaceDLLHelper<&sz_DefUtils_dll,
								CccPathProvider, 
								CSymInterfaceLoader, 
								IDefUtils, 
								&IID_DefUtils, 
								&IID_DefUtils> DefUtilsLoader;

// loader for second interface for Def Utils.
typedef CSymInterfaceDLLHelper<&sz_DefUtils_dll,
								CccPathProvider, 
								CSymInterfaceLoader, 
								IDefUtils2, 
								&IID_DefUtils, 
								&IID_DefUtils2> DefUtilsLoader2;

// loader for third interface for Def Utils.
typedef CSymInterfaceDLLHelper<&sz_DefUtils_dll,
								CccPathProvider, 
								CSymInterfaceLoader, 
								IDefUtils3, 
								&IID_DefUtils, 
								&IID_DefUtils3> DefUtilsLoader3;

// loader for symsetup
typedef CSymInterfaceDLLHelper<&sz_DefUtils_dll,
								CSymSetupPathProvider, 
								CSymInterfaceLoader, 
								IDefUtils, 
								&IID_DefUtils, 
								&IID_DefUtils> SymSetupDefUtilsLoader;

#ifdef MSI_DEFUTILS

extern MSIHANDLE g_hDefsInstall;

// class to provide syminterface loader with path to defutils.dll
class CMSIPathProvider
{
public:
    static bool GetPath(LPTSTR szPath, size_t& dwSize);
};

// loader for MSI, be sure to set g_hDefsInstall to your MSIHANDLE before using this
typedef CSymInterfaceDLLHelper<&sz_DefUtils_dll,
								CMSIPathProvider, 
								CSymInterfaceLoader, 
								IDefUtils, 
								&IID_DefUtils, 
								&IID_DefUtils> MSIDefUtilsLoader;
#endif


