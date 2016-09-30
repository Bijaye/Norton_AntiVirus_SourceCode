#include "helper.h"
#include "QuarantineDllLoader.h"
#include "navopt32.h"
#include "NAVInfo.h"
#include "APOptNames.h"
#include "fileaction.h"
#include "Quarantine_const.h"


/////////////////////////////////////////////////////////////////////////////////////////////
// RemoveQuarantineContents():
//
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) UINT __stdcall RemoveQuarantineContents(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug RemoveQuarantineContents()", "Debug Break", NULL);
#endif

	CCTRACEI("RemoveQuarantineContents() Starting.");

	IQuarantineDLL* pIQuar = NULL;
	IEnumQuarantineItems* pIEnum;
	IQuarantineItem* pIQuarItem;
	ULONG ulFetched;
	BOOL bQuarEmpty = TRUE;
	TCHAR szTempDir[MAX_PATH];
	TCHAR szIncomingDir[MAX_PATH];
	TCHAR szQuarantineDir[MAX_PATH];
	DWORD dwBufSize;
	SHFILEOPSTRUCT rFileOp;
	QuarDLL_Loader	QuarDllLoader;

    ZeroMemory(szTempDir, sizeof(szTempDir));
    ZeroMemory(szIncomingDir, sizeof(szIncomingDir));
	ZeroMemory(szQuarantineDir, sizeof(szQuarantineDir));

    // Read in the Quarantine incoming/temp directories.
    CRegKey rkQuar;
    if (ERROR_SUCCESS == rkQuar.Open(HKEY_LOCAL_MACHINE, SZ_REGKEY_QUARANTINE))
	{
        dwBufSize = sizeof(szTempDir);
        if(ERROR_SUCCESS != rkQuar.QueryStringValue(SZ_REGVALUE_QUARANTINE_TEMP_PATH, szTempDir, &dwBufSize))
			CCTRACEE(_T("%s - Failed to query value %s"), __FUNCTION__, SZ_REGVALUE_QUARANTINE_TEMP_PATH);

        dwBufSize = sizeof(szIncomingDir);
        if(ERROR_SUCCESS != rkQuar.QueryStringValue(SZ_REGVALUE_QUARANTINE_INCOMING_PATH, szIncomingDir, &dwBufSize))
			CCTRACEE(_T("%s - Failed to query value %s"), __FUNCTION__, SZ_REGVALUE_QUARANTINE_INCOMING_PATH);

        dwBufSize = sizeof(szQuarantineDir);
        if(ERROR_SUCCESS != rkQuar.QueryStringValue(SZ_REGVALUE_QUARANTINE_PATH, szQuarantineDir, &dwBufSize))
			CCTRACEE(_T("%s - Failed to query value %s"), __FUNCTION__, SZ_REGVALUE_QUARANTINE_PATH);
	}
	else
	{
		CCTRACEE(_T("%s - Failed to open %s for read"), __FUNCTION__, SZ_REGKEY_QUARANTINE);
	}
	
    if (SUCCEEDED(CoInitialize(NULL)))
    {
        SYMRESULT symRes = QuarDllLoader.CreateObject(&pIQuar);
        if (SYM_SUCCEEDED(symRes) && pIQuar != NULL)
	    {
		    if (SUCCEEDED(pIQuar->Initialize()))
		    {
			    if (SUCCEEDED(pIQuar->Enum(&pIEnum)))
			    {
				    if (S_OK == pIEnum->Next(1, &pIQuarItem, &ulFetched))
				    {
					    // Something's in Quarantine.
					    bQuarEmpty = FALSE;
					    pIQuarItem->Release();
				    }

				    pIEnum->Release();
			    }
		    }
	    }
	    else
	    {
		    // Couldn't get IQuarDLL pointer
		    CCTRACEE("RemoveQuarantineContentsHelp: Unable to get IQuarDLL pointer.");
		    pIQuar = NULL;
	    }

        //check to see if the user wants to remove Quarantine
	    TCHAR szProp[MAX_PATH] = {0};
	    dwBufSize = MAX_PATH;
	    MsiGetProperty(hInstall, _T("REMOVEQUARANTINE"), szProp, &dwBufSize);

	    //if they do then let's start deleting files.
	    if(atoi(szProp) == 1)
	    {
		    CCTRACEI(_T("%s - User wants us to remove quarantine files "), __FUNCTION__);

		    // Clean out the Quarantine temp dir.
		    if (_tcslen(szTempDir) > 0)
		    {
			    TCHAR szFilesToDel[MAX_PATH * 2] = {0};

			    ZeroMemory(szFilesToDel, sizeof(szFilesToDel));
			    _tcscpy(szFilesToDel, szTempDir);
			    _tcscat(szFilesToDel, _T("\\*.*"));

			    ZeroMemory(&rFileOp, sizeof(SHFILEOPSTRUCT));

			    rFileOp.hwnd = NULL;
			    rFileOp.wFunc = FO_DELETE;
			    rFileOp.pFrom = szFilesToDel;
			    rFileOp.fFlags = FOF_FILESONLY | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;

			    SHFileOperation(&rFileOp);
		    }

		    // Remove the incoming dir too.
		    if (_tcslen(szIncomingDir) > 0)
		    {
			    TCHAR szFilesToDel[MAX_PATH * 2] = {0};

			    ZeroMemory(szFilesToDel, sizeof(szFilesToDel));
			    _tcscpy(szFilesToDel, szIncomingDir);
			    _tcscat(szFilesToDel, _T("\\*.*"));

			    ZeroMemory(&rFileOp, sizeof(SHFILEOPSTRUCT));

			    rFileOp.hwnd = NULL;
			    rFileOp.wFunc = FO_DELETE;
			    rFileOp.pFrom = szFilesToDel;
			    rFileOp.fFlags = FOF_FILESONLY | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;

			    SHFileOperation(&rFileOp);
		    }

		    if (!bQuarEmpty)
		    {
			    if (SUCCEEDED(pIQuar->Enum(&pIEnum))) 
			    {
				    while (S_OK == pIEnum->Next(1, &pIQuarItem, &ulFetched))
				    {
    					                        
                        // nuke it			
					    pIQuarItem->DeleteItem();

					    // free the interface
					    pIQuarItem->Release();
				    }
                    
                    // quarantine is now empty
                    bQuarEmpty = TRUE;

				    pIEnum->Release();
			    }
		    }

		    if (NULL != pIQuar)
			    pIQuar->Release();
	    }
        
        // delete the keys if theres nothing in Quarantine
        // either it was empty to begin with or we've deleted everythig
        if(bQuarEmpty)
        {
            rkQuar.DeleteValue(SZ_REGVALUE_QUARANTINE_TEMP_PATH);
            rkQuar.DeleteValue(SZ_REGVALUE_QUARANTINE_INCOMING_PATH);
            rkQuar.DeleteValue(SZ_REGVALUE_QUARANTINE_PATH);
        }

        CoUninitialize();
    }
	
	// Always try to remove the directories (if they aren't empty they won't get removed). This ensures 
	// that empty Quarantine dirs always get removed in the case where there was nothing there to begin with.
	if(RemoveDirectory(szTempDir) == 0)
		InstallToolBox::FileInUseHandler(szTempDir);
	if(RemoveDirectory(szIncomingDir) == 0)
		InstallToolBox::FileInUseHandler(szIncomingDir);
	if(RemoveDirectory(szQuarantineDir) == 0)
		InstallToolBox::FileInUseHandler(szQuarantineDir);
	
	CCTRACEI("RemoveQuarantineContents() Finished.");

	return ERROR_SUCCESS;
}