#include "StdAfx.h"
#include "vpstrutils.h"
#include "SymSaferRegistry.h"

#include "ScanDlgLoader.h"

#ifdef DEBUG
PSUPERASSERTIONA g_pSuperAssert;
#endif

/** Use:  CALL_FUNCTION(function name)(param1, param2);
  * This will check the function pointer for NULL and call the function.
  */
#define CALL_FUNCTION(pfFunction)		\
	if (NULL == pfFunction)				\
		return ERROR_PROC_NOT_FOUND;	\
	return pfFunction


/** The singleton instance for this class */
CScanDlgLoader CScanDlgLoader::s_objScanDlgLoaderInstance;


/** the default constructor */
CScanDlgLoader::CScanDlgLoader(void):m_pfnCreateResultsView(NULL),
                                     m_pfnAddProgressToResultsView(NULL),
                                     m_pfnAddLogLineToResultsView(NULL),
                                     m_pfnGetViewThreadHandleFromResultsView(NULL),
                                     m_pfnScanDoneToResultsView(NULL),
                                     m_pfnCreateVirusFoundDlg(NULL),
                                     m_pfnAddLogLineToVirusFound(NULL),
                                     m_pfnGetWindowHandle(NULL),
                                     m_pfnGetVirusFoundWindowHandle(NULL),
                                     m_bInitialized(false),
                                     m_hScanDlgDll(NULL)
{
}


/** The destructor is thread-safe. */
CScanDlgLoader::~CScanDlgLoader(void)
{
    CGuard oGuard(g_oScanDlgLock);

    if( false == m_bInitialized )
        return;

    SetFuncsToNull();
    UnloadDll();

    m_bInitialized = false;
}


/** @returns an instance of the singleton class */
CScanDlgLoader& CScanDlgLoader::GetInstance()
{
    if( false == s_objScanDlgLoaderInstance.m_bInitialized )
    {
        s_objScanDlgLoaderInstance.LoadManager();
    }

    return s_objScanDlgLoaderInstance;
}


/** Loads ScanDlgs.dll.
  * @return ERROR_SUCCESS on success, other values on failure
  */
DWORD CScanDlgLoader::LoadDll()
{
    // If the handle has not been set and the library loaded, do so now.
    char szKey[128];
    vpstrncpy(szKey,"CLSID\\{4DEF8DD1-C4D1-11D1-82DA-00A0C9749EEF}\\InProcServer32", sizeof (szKey));

    HKEY hkey = NULL;
    if (RegOpenKey(HKEY_CLASSES_ROOT,szKey,&hkey) == ERROR_SUCCESS)
    {
        SymSaferRegKeyJanitor objKeyJanitor (hkey);
        char                  szPath[IMAX_PATH];
        DWORD                 size = sizeof(szPath);

        if (SymSaferRegQueryValueEx(hkey,NULL,0,NULL,(PBYTE)szPath,&size) == ERROR_SUCCESS)
        {
            m_hScanDlgDll = LoadLibrary(szPath);
            if (m_hScanDlgDll == NULL)
            {
                GetLastError();
                return P_NO_VIRUS_ENGINE;
            }
        }
    }

    return m_hScanDlgDll ? ERROR_SUCCESS : P_NO_VIRUS_ENGINE;
}


/** Unloads ScanDlgs.dll. */
void CScanDlgLoader::UnloadDll()
{
    if (m_hScanDlgDll)
    {
        FreeLibrary(m_hScanDlgDll);
        m_hScanDlgDll = NULL;
    }
}


/** Clears out pointers to functions exported from ScanDlgs.dll. */
void CScanDlgLoader::SetFuncsToNull()
{
    m_pfnCreateResultsView                  = NULL;
    m_pfnAddProgressToResultsView           = NULL;
    m_pfnAddLogLineToResultsView            = NULL;
    m_pfnGetViewThreadHandleFromResultsView = NULL;
    m_pfnScanDoneToResultsView              = NULL;
    m_pfnAddLogLineToVirusFound             = NULL;
    m_pfnCreateVirusFoundDlg                = NULL;
    m_pfnGetWindowHandle                    = NULL;
    m_pfnGetVirusFoundWindowHandle          = NULL;
}


/** Gets pointers to functions exported from ScanDlgs.dll.
  * @return ERROR_SUCCESS on success, other values on failure
  */
DWORD CScanDlgLoader::LoadFuncs()
{
    DWORD nRtn = ERROR_SUCCESS;

    try
    {
        if( (m_pfnCreateResultsView = (tCreateResultsView)GetProcAddress(m_hScanDlgDll,(LPCSTR)100)) == NULL )
            throw (DWORD)100;
        if( (m_pfnAddProgressToResultsView = (tAddProgressToResultsView)GetProcAddress(m_hScanDlgDll,(LPCSTR)101)) == NULL )
            throw (DWORD)101;
        if( (m_pfnAddLogLineToResultsView = (tAddLogLineToResultsView)GetProcAddress(m_hScanDlgDll,(LPCSTR)102)) == NULL)
            throw (DWORD)102;
        if( (m_pfnGetViewThreadHandleFromResultsView = (tGetViewThreadHandleFromResultsView)GetProcAddress(m_hScanDlgDll,(LPCSTR)104)) == NULL ) 
            throw (DWORD)104;
        if( (m_pfnScanDoneToResultsView = (tScanDoneToResultsView)GetProcAddress(m_hScanDlgDll,(LPCSTR)105)) == NULL ) 
            throw (DWORD)105;
        if( (m_pfnAddLogLineToVirusFound = (tAddLogLineToVirusFound)GetProcAddress(m_hScanDlgDll,(LPCSTR)113)) == NULL ) 
            throw (DWORD)113;
        if( (m_pfnCreateVirusFoundDlg = (tCreateVirusFoundDlg)GetProcAddress(m_hScanDlgDll,(LPCSTR)112)) == NULL ) 
            throw (DWORD)112;
        if( (m_pfnGetWindowHandle = (tGetWindowHandle)GetProcAddress(m_hScanDlgDll,(LPCSTR)115)) == NULL ) 
            throw (DWORD)115;
        if( (m_pfnGetVirusFoundWindowHandle = (tGetVirusFoundWindowHandle)GetProcAddress(m_hScanDlgDll,(LPCSTR)116)) == NULL ) 
            throw (DWORD)116;
    }
    catch (DWORD nErr)
    {
        nRtn = nErr;

        // Reset the pointers if any one of the functions fails to load.
        SetFuncsToNull();
    }

    return nRtn;
}


/** Manages loading the dll and function pointers.
  * @return ERROR_SUCCESS on success, other values on failure
  */
DWORD CScanDlgLoader::LoadManager(void) 
{
    //make sure this is protected so only one thread can initialize or deinitialize
    CGuard oGuard(g_oScanDlgLock);

    DWORD dwRet = LoadDll();
    if ( ERROR_SUCCESS == dwRet)
    {
        // Load the ScanDlg exported functions.
        if( (dwRet = LoadFuncs()) != ERROR_SUCCESS)
        {
            UnloadDll();
            m_bInitialized = false;

        }
    }

    return dwRet;
}

/** Creates a virus found (notification) dialog.
  * @see CreateVirusFoundDlg() in ScanDlgs
  */
DWORD CScanDlgLoader::CreateVirusFoundDlg(PVIRUSFOUNDDLG pVirusFoundDlg)
{
	CALL_FUNCTION(m_pfnCreateVirusFoundDlg)(pVirusFoundDlg);
}


/** Adds text to notification dialog.
  * @see AddLogLineToVirusFound() in ScanDlgs
  */
DWORD CScanDlgLoader::AddLogLineToVirusFound(CVirusFound *pVirusFoundDlg, char *line, char *Description)
{
	CALL_FUNCTION(m_pfnAddLogLineToVirusFound)(pVirusFoundDlg, line, Description);
}


/** Creates a results view dialog.
  * @see CreateResultsView() in ScanDlgs
  */
DWORD CScanDlgLoader::CreateResultsView(PRESULTSVIEW ScanView)
{
	CALL_FUNCTION(m_pfnCreateResultsView)(ScanView);
}


/** Adds info on a threat to the results view dialog.
  * @see AddLogLineToResultsView() in ScanDlgs
  */
DWORD CScanDlgLoader::AddLogLineToResultsView(CResultsView *pResultsViewDlg, LPARAM Data, char *line)
{
	CALL_FUNCTION(m_pfnAddLogLineToResultsView)(pResultsViewDlg, Data, line);
}


DWORD CScanDlgLoader::AddProgressToResultsView(CResultsView *pResultsViewDlg, PPROGRESSBLOCK pb)
{
    CALL_FUNCTION(m_pfnAddProgressToResultsView)(pResultsViewDlg, pb);
}