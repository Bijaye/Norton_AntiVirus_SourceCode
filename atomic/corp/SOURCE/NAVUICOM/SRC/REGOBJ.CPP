/* Copyright 1997 Symantec, Peter Norton Computing Group                */
/////////////////////////////////////////////////////////////////////////
// RegObj.h
//		Contains class implementation for CNavRegComm Object.  This object
// is use by the NAV'97 UI to communicate with the NAVW.EXE module.
//
// $Header:   S:/NAVUICOM/VCS/REGOBJ.CPv   1.4   28 Mar 1997 17:34:12   DBUCHES  $
//
/////////////////////////////////////////////////////////////////////////
// Created 3/21/97 - DBUCHES
/////////////////////////////////////////////////////////////////////////
/* $Log:   S:/NAVUICOM/VCS/REGOBJ.CPv  $ */
// 
//    Rev 1.4   28 Mar 1997 17:34:12   DBUCHES
// Added m_hWnd Member
//
//    Rev 1.3   25 Mar 1997 16:00:00   DBUCHES
// Fixed bug in Save() code. Forgot array access operator! (doh!)
//
//    Rev 1.2   25 Mar 1997 15:32:46   DBUCHES
// Fixed bug in Save() code. Forgot array access operator! (doh!)
//
//    Rev 1.1   25 Mar 1997 13:43:06   DBUCHES
// Removed reference to MFC include file.
//
//    Rev 1.0   25 Mar 1997 13:17:20   DBUCHES
// Initial revision.

#include "windows.h"
#include "tchar.h"
#include "regobj.h"


// Local constants.  Not to be translated!
static LPTSTR szSizeKey = _T("Size");
static LPTSTR szDataKey = _T("Data");

//////////////////////////////////////////////////////////////////////////////////////
// Constructors/Destructors.
//////////////////////////////////////////////////////////////////////////////////////
CNavRegComm::CNavRegComm()
{
    // Init Data members.
    m_iAction = ACTION_NO_ACTION;
	lstrcpy(m_szRegKey, _T("Software\\Symantec\\Norton AntiVirus\\RegCommData"));
    m_iNumScanObjects = 0;
    m_pDirObjects = NULL;
	m_pRegData = NULL;
	m_bScanMemory = TRUE;
	m_bScanMBR = TRUE;
	m_bScanBootRecords = TRUE;
	m_bScanCompressed = TRUE;
	m_iScanType = ALL_FILES;
	m_iResponse = PROMPT;
    m_hWnd = NULL;

}

//////////////////////////////////////////////////////////////////////////////////////

CNavRegComm::~CNavRegComm()
{
	// Delete all allocated data.
	if(m_pDirObjects)
		delete [] m_pDirObjects;
	if(m_pRegData)
		delete [] m_pRegData;

}

//////////////////////////////////////////////////////////////////////////////////////
//	InitForSave()
//
//	Parameters -
//		iCount - number of items to save
//		pszPaths - array of strings containing paths to scan
//		pbScanSubDirs - array of BOOL flags specifying sub-dir scanning.
//
//	returns TRUE if successful, FALSE otherwise
//
//////////////////////////////////////////////////////////////////////////////////////
BOOL CNavRegComm::InitForSave(int iCount, LPTSTR *pszPaths, BOOL *pbScanSubDirs)
{
	// Save off how many objects we have.
	m_iNumScanObjects = iCount;

	// Allocate array of scan objects and string pointers;
	m_pDirObjects = new SCANOBJECT[m_iNumScanObjects];
	if(m_pDirObjects == NULL)
		return FALSE;

	// Fill buffers with data passed in.
	for(int i=0; i < m_iNumScanObjects; i++)
		{
		m_pDirObjects[i].bScanSubDirs = pbScanSubDirs[i];
		m_pDirObjects[i].szPathName = pszPaths[i];
		}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
// Serialization functions
//////////////////////////////////////////////////////////////////////////////////////

BOOL CNavRegComm::Save()
{
	DWORD dwBufferSize = 0;
	BOOL bRet = FALSE;

	//
	// Compute total size of buffer needed.
	//

	// Get total size of strings in array.
	for(int i=0; i < m_iNumScanObjects; i++)
		{
        dwBufferSize += lstrlen(m_pDirObjects[i].szPathName);
		}

	// Add additional items.
	dwBufferSize += sizeof(int) +	// m_iAction
				    sizeof(int) +	// m_iNumObjects
					sizeof(int) +	// m_iScanType
					sizeof(int) +	// m_iResponse
					sizeof(BOOL) +	// m_bScanMemory
					sizeof(BOOL) +	// m_bScanMBR
					sizeof(BOOL) +	// m_bScanBootRecords
					sizeof(BOOL) +	// m_bScanCompressed
                    sizeof(HWND) +  // m_hWnd
					(sizeof(TCHAR) * m_iNumScanObjects) + // NULL for each path
					(sizeof(BOOL) * m_iNumScanObjects);  // bScanSubDirFlag for each scan object

	// Alloc memory for transfer
	LPBYTE pBuffer = new BYTE[dwBufferSize];
	LPBYTE pPosition = pBuffer;
	if(pBuffer == NULL)
		return bRet;

	// Copy data to buffer.
	*((int*)pPosition) = m_iAction;
	pPosition += sizeof(int);

	*((int*)pPosition) = m_iNumScanObjects;
	pPosition += sizeof(int);

	*((int*)pPosition) = m_iScanType;
	pPosition += sizeof(int);

	*((int*)pPosition) = m_iResponse;
	pPosition += sizeof(int);

	*((BOOL*)pPosition) = m_bScanMemory;
	pPosition += sizeof(BOOL);

	*((BOOL*)pPosition) = m_bScanMBR;
	pPosition += sizeof(BOOL);

	*((BOOL*)pPosition) = m_bScanBootRecords;
	pPosition += sizeof(BOOL);

	*((BOOL*)pPosition) = m_bScanCompressed;
	pPosition += sizeof(BOOL);

    *((HWND*)pPosition) = m_hWnd;
    pPosition += sizeof(HWND);


	// Save off scan objects.  Won't do anything if no scan objects.
	for(i = 0; i < m_iNumScanObjects; i++)
		{
		int iPathLength = lstrlen(m_pDirObjects[i].szPathName);
		// Save off path.
		CopyMemory(pPosition, m_pDirObjects[i].szPathName, iPathLength);
		pPosition += iPathLength;

		// Add trailing NULL
		*((TCHAR*)pPosition) = 0;
		pPosition += sizeof(TCHAR);

		// Save off ScanSubDirFlag
		*((BOOL*)pPosition) = m_pDirObjects[i].bScanSubDirs;
		pPosition += sizeof(BOOL);
		}

	// Save data to registry.
	HKEY hKey;
	DWORD dwStatus;
	if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_LOCAL_MACHINE,	// Handle of open key
						m_szRegKey,				// Sub Key
						0,						// Reserved
						NULL, 					// Class Flag
						REG_OPTION_VOLATILE,	// Special Options
						KEY_WRITE,				// Security Access
						NULL,					// Security descriptor
						&hKey , 				// output.
						&dwStatus))
		{
		// Save off size of data
		if( ERROR_SUCCESS ==  RegSetValueEx( hKey,			// Open key
											szSizeKey,		// Value to set.
											0,				// reserved
											REG_DWORD,		// Data Type
											(BYTE*) &dwBufferSize, // Data
											sizeof(DWORD)))	// Data Size
			{
			// Save off actual data
			if(ERROR_SUCCESS == RegSetValueEx( hKey,		// Open key
											szDataKey,		// Value to set.
											0,				// reserved
											REG_BINARY,		// Data Type
											pBuffer,		// Data
											dwBufferSize))	// Data Size
				{
				bRet = TRUE;
				}
			}
		RegCloseKey(hKey);
		}

	// Cleanup
	delete [] pBuffer;
	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////

BOOL CNavRegComm::Load()
{
	BOOL bRet = FALSE;

	// Clean up any old data from previous Load() calls.
	if(m_pRegData)
		delete [] m_pRegData;

	if(m_pDirObjects)
		delete [] m_pDirObjects;

    //
    // Load data from registry.
    //

	// Open the data transfer key.
	HKEY hKey;
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,
									 m_szRegKey,
									 0,
									 KEY_READ,
									 &hKey))
		{
		DWORD dwType = REG_DWORD;
		DWORD dwBufferSize;
		DWORD dwSize = sizeof(DWORD);
		// Get size of data
		if(ERROR_SUCCESS == RegQueryValueEx(hKey,
											szSizeKey,
											0,
											&dwType,
											(BYTE*)&dwBufferSize,
											&dwSize))
			{
			// Allocate buffer to hold data.
			m_pRegData = new BYTE[dwBufferSize];
			LPBYTE pPosition = m_pRegData;

			// Read data from registry.
			dwType = REG_BINARY;
			if(ERROR_SUCCESS == RegQueryValueEx(hKey,
									szDataKey,
									0,
									&dwType,
									m_pRegData,
									&dwBufferSize))
				{
				// Restore values
				m_iAction = *((int*)pPosition);
				pPosition += sizeof(int);

				m_iNumScanObjects =	*((int*)pPosition);
				pPosition += sizeof(int);

				m_iScanType =	*((int*)pPosition);
				pPosition += sizeof(int);

				m_iResponse =	*((int*)pPosition);
				pPosition += sizeof(int);

				m_bScanMemory =	*((BOOL*)pPosition);
				pPosition += sizeof(BOOL);

				m_bScanMBR = *((BOOL*)pPosition);
				pPosition += sizeof(BOOL);

				m_bScanBootRecords = *((BOOL*)pPosition);
				pPosition += sizeof(BOOL);

				m_bScanCompressed =	*((BOOL*)pPosition);
				pPosition += sizeof(BOOL);

                m_hWnd = *((HWND*)pPosition);
                pPosition += sizeof(HWND);

				// Make sure we have work to do here.
				if(m_iNumScanObjects)
					{
					// Allocate array of scan objects;
					m_pDirObjects = new SCANOBJECT[m_iNumScanObjects];
					if(m_pDirObjects == NULL)
						{
						RegCloseKey(hKey);
						return(FALSE);
						}

					// Init Scan objects;
					for(int i=0; i < m_iNumScanObjects;i++)
						{
						// Get Pathname
						m_pDirObjects[i].szPathName = (LPTSTR)pPosition;

						// Increment pointer, accounting for trailing NULL.
						pPosition += lstrlen(m_pDirObjects[i].szPathName) + sizeof(TCHAR);

						// Get subdir flag
						m_pDirObjects[i].bScanSubDirs = *((BOOL*)pPosition);
						pPosition += sizeof(BOOL);
						}
					}
				bRet = TRUE;
				}
			}

		// Now that we've loaded everything, remove the data from the registry.
		RegDeleteValue(hKey, szDataKey);
		RegDeleteValue(hKey, szSizeKey);
		RegCloseKey(hKey);
		}

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// Data accessors.
//////////////////////////////////////////////////////////////////////////////////////
int CNavRegComm::GetAction()
{
	return m_iAction;
}

int CNavRegComm::GetScanType()
{
	return m_iScanType;
}

int CNavRegComm::SetScanType(int iType)
{
	int iOldType = m_iScanType;

	if(iType >= ALL_FILES && iType <= END_SCAN_TYPE)
		m_iScanType = iType;

	return iOldType;
}

int CNavRegComm::GetScanResponse()
{
	return m_iResponse;
}

int CNavRegComm::SetScanResponse(int iResponse)
{
	int iOldResponse = m_iResponse;

	if(m_iResponse >= ALL_FILES && m_iResponse <= END_SCAN_TYPE)
		m_iResponse = iResponse;

	return iOldResponse;
}

//////////////////////////////////////////////////////////////////////////////////////
// Set the action we're about to perform.  Returns previous action.
int CNavRegComm::SetAction(int iAction)
{
	int iOldAction = m_iAction;

	if(iAction >= ACTION_NO_ACTION && iAction <= ACTION_LAST_ACTION)
		m_iAction = iAction;

	return iOldAction;

}

//////////////////////////////////////////////////////////////////////////////////////
// Sets the registry key we're saveing/loading from.
void CNavRegComm::SetRegKey(LPTSTR szNewKey)
{
	lstrcpy(m_szRegKey, szNewKey);
}

//////////////////////////////////////////////////////////////////////////////////////
// Return total number of drives/directories to scan.
int CNavRegComm::GetNumScanObjects()
{
	return m_iNumScanObjects;
}

//////////////////////////////////////////////////////////////////////////////////////
// Returns pointer to first scan object.  Can be used in
// call to GetNextScanObject.
//
//	This pointer returned by this function may be used with the array[] operator to
// access specific scan objects non-sequentialy, as the opjects are of a fixed size.
//////////////////////////////////////////////////////////////////////////////////////
LPSCANOBJECT CNavRegComm::GetFirstScanObject()
{
	return &m_pDirObjects[0];
}

//////////////////////////////////////////////////////////////////////////////////////
// Returns pointer to next object or NULL if no more objects.
LPSCANOBJECT CNavRegComm::GetNextScanObject(LPSCANOBJECT pCurrent)
{
	// Find address of last object.  Note that we subtract 1 because pointer
	// math is 0 based.
	LPSCANOBJECT pLast = m_pDirObjects + m_iNumScanObjects - 1;

	// Make sure we don't run off our list.
	if(pCurrent == pLast)
		return NULL;
	else
		return pCurrent + 1;
}

