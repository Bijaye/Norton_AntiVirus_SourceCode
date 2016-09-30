
#include "Stdafx.h"
#include "FileAPI.h"
#include "OSInfo.h"
#include "DisplayNames.h"

#include "qsfields.h"
#include "resource.h"
#include "Quar32.h"

#define INITGUID
#include "Quarantine.h"

using namespace std;

CQuarantine::CQuarantine() : m_bAddFailedOnExtraction(false)
{
	m_pScanner = NULL;
	m_pQuarServer = NULL;
    m_bHaveSnapShot = false;
}

CQuarantine::~CQuarantine()
{
	Close();
}

bool CQuarantine::Open(IScanner* pScanner)
{
	// Is there already a quarantine server object?

	if (m_pQuarServer != NULL)
	{
        CCTRACEW(_T("CQuarantine::Open() - Already have an open quarantine server"));
		return true;
	}

    if( pScanner == NULL )
    {
        CCTRACEE(_T("CQuarantine::Open() - Invalid scanner object passed in"));
        return false;
    }

	// Create the quarantine server object.
	if(SYM_FAILED(m_QuarLoader.CreateObject(&m_pQuarServer)) ||
        m_pQuarServer.m_p == NULL )
    {
        CCTRACEE(_T("CQuarantine::Open() - Failed to create the qurantine server"));
        return false;
    }

	if( FAILED( m_pQuarServer->Initialize() ) )
	{
        CCTRACEE(_T("CQuarantine::Open() - Failed to initialize the quarantine server"));
		return false;
	}

    m_bHaveSnapShot = false;

	// Save the scanner object pointer.
	m_pScanner = pScanner;


	return true;
}

void CQuarantine::Close()
{
	// Release all quarantine items.
    QUARANTINEMAP::iterator Iter;
	for(Iter = m_ItemMap.begin(); Iter != m_ItemMap.end(); Iter++)
	{
		if ((*Iter).second != NULL)
        {
			unsigned long ulRef = (*Iter).second.Release();

            if( ulRef != 0 )
                CCTRACEE(_T("CQuarantine::Close() - After releasing quarantine item the ref count is still %d."), ulRef);
        }
	}

	m_ItemMap.clear();

	// Clean-up.
	if (m_pQuarServer != NULL)
	{
		m_pQuarServer.Release();
	}

    m_bHaveSnapShot = false;
}

bool CQuarantine::LoadQuarantineItemFromUUID(UUID uuid, IQuarantineItem **pMatchingQuarItem)
{
    if (m_pQuarServer == NULL)
    {
        CCTRACEE("CQuarantine::LoadQuarantineItemFromUUID() - Quarantine server is not valid.");
        return false;
    }

    if( pMatchingQuarItem == NULL )
    {
        CCTRACEE("CQuarantine::LoadQuarantineItemFromUUID() - Quarantine item is not valid.");
        return false;
    }

    // Release existing item
    if( (*pMatchingQuarItem) != NULL )
    {
        (*pMatchingQuarItem)->Release();
        (*pMatchingQuarItem) = NULL;
    }

    if( SUCCEEDED(m_pQuarServer->LoadQuarantineItemFromUUID(uuid, pMatchingQuarItem)) )
        return true;

    return false;
}

bool CQuarantine::LoadQuarantineItemFromFilenameVID(LPCSTR lpszSourceFileName, 
                                                    DWORD dwVirusID, 
                                                    IQuarantineItem **pMatchingQuarItem)
{
    if (m_pQuarServer == NULL)
    {
        CCTRACEE("CQuarantine::LoadQuarantineItemFromFilenameVID() - Quarantine server is not valid.");
        return false;
    }

    if( pMatchingQuarItem == NULL )
    {
        CCTRACEE("CQuarantine::LoadQuarantineItemFromFilenameVID() - Quarantine item is not valid.");
        return false;
    }

    // Release existing item
    if( (*pMatchingQuarItem) != NULL )
    {
        (*pMatchingQuarItem)->Release();
        (*pMatchingQuarItem) = NULL;
    }

    if( SUCCEEDED(m_pQuarServer->LoadQuarantineItemFromFilenameVID(lpszSourceFileName, dwVirusID, pMatchingQuarItem)) )
        return true;

    return false;
}

bool CQuarantine::Add(char* szFileName,
					  DWORD FileType,
					  bool bDeleteOriginal,
                      bool bOemFileName)
{
	bool bResult = true;
        m_bAddFailedOnExtraction = false;

	try
	{
		// Make sure we have a quarantine server object.
		if (m_pQuarServer == NULL)
		{
			throw runtime_error("CQuarantine::Add() - Quarantine server is not ready.");
		}

		// Add the infected file to quarantine.
		CSymPtr<IQuarantineItem> pItem = NULL;
		HRESULT hResult = m_pQuarServer->CreateNewQuarantineItemFromFile(szFileName, FileType, 
			QFILE_STATUS_QUARANTINED, &pItem );

		// Bail if we weren't able to quarantine the file.

		if (FAILED(hResult))
		{
            CCTRACEE("CQuarantine::Add() - Unable to add %s to quarantine.", szFileName);
			throw runtime_error("CQuarantine::Add() - Unable to add the file to quarantine.");
		}

        // Add to map using the ISymBase value of the quarantine item since there is no
        // associated IScanInfection
        ISymBaseQIPtr pQISymBase = pItem;

        if( pQISymBase == NULL )
        {
            CCTRACEE("CQuarantine::Add() - Unable to QI for ISymBase from the quarantine item. This will not be in the map");
        }
        else
        {
            m_ItemMap[pQISymBase.m_p] = pItem;
        }

		// Delete the original file (if necessary).
		if (bDeleteOriginal == true)
		{
			if (SafeDeleteFile(szFileName) == FALSE)
			{
				// If we can't delete the infection, undo the quarantine operation.

				pItem->DeleteItem();
                CCTRACEE("CQuarantine::Add() - Unable to delete infected file %s after adding to quarantine.", szFileName);
				throw runtime_error("CQuarantine::Add() - Unable to delete the infected file.");
			}
		}
	}
	catch(exception& Ex)
	{
		bResult = false;

		CCTRACEE(Ex.what());
	}
	catch(...)
	{
		bResult = false;

		CCTRACEE("Unknown exception caught in CQuarantine::Add.");
	}

	return bResult;
}

bool CQuarantine::AddWithoutFile(unsigned long ulQuarFileStatus, IQuarantineItem** pItemOut)
{
    m_bAddFailedOnExtraction = false;

    if (m_pQuarServer == NULL)
    {
        CCTRACEE("CQuarantine::AddWithoutFile() - Quarantine server is not valid.");
        return false;
    }

    // Add the infected file to quarantine.
    CSymPtr<IQuarantineItem> pItem = NULL;

    HRESULT hResult = m_pQuarServer->CreateNewQuarantineItemWithoutFile(ulQuarFileStatus, &pItem );

    // Bail if we weren't able to quarantine the file
    if (FAILED(hResult) || pItem == NULL)
    {
        CCTRACEE("CQuarantine::AddWithoutFile() - Unable to create the quarantine backup without file item.");
        return false;
    }
    
    // Add to map using the ISymBase value of the quarantine item since there is no
    // associated IScanInfection
    ISymBaseQIPtr pQISymBase = pItem;

    if( pQISymBase == NULL )
    {
        CCTRACEE("CQuarantine::AddWithoutFile() - Unable to QI for ISymBase from the quarantine item. This will not be in the map");
    }
    else
    {
        m_ItemMap[pQISymBase.m_p] = pItem;
    }

    // If the caller wants a copy assign and ref count it for them now
    if( pItemOut != NULL )
    {
        *pItemOut = pItem;
        (*pItemOut)->AddRef();
    }

    return true;
}

bool CQuarantine::Add(IScanInfection* pInfection,
					  unsigned long ulQuarFileStatus,
					  bool bDeleteOriginal,
					  const char* szOriginalName,
                      IQuarantineItem** pItemOut /*= NULL*/)
{
	bool bResult = true;
    m_bAddFailedOnExtraction = false;

    if( pInfection == NULL )
    {
        CCTRACEE(_T("CQuarantine::Add() - The infection item is null."));
        return false;
    }

	try
	{
		// Make sure we have a quarantine server object.

		if (m_pQuarServer == NULL)
		{
			throw runtime_error("CQuarantine::Add() - Quarantine server is not ready.");
		}
	
		// Is this a file infection?

		CScanPtr<IScanFileInfection2> pFileInfection;
		SYMRESULT Status = pInfection->QueryInterface(IID_ScanFileInfection2,
			(void**) &pFileInfection);

        // The quarantine item being added
        CSymPtr<IQuarantineItem> pItemToAdd = NULL;

		if (SYM_SUCCEEDED(Status))
		{
			HandleFileInfection(pFileInfection, ulQuarFileStatus, bDeleteOriginal,
				szOriginalName, pItemToAdd);
		}
		else
		{
			// Is this a compressed file infection?

			CScanPtr<IScanCompressedFileInfection2> pCompressedInfection;
			SYMRESULT Status = pInfection->QueryInterface(IID_ScanCompressedFileInfection2,
					(void**) &pCompressedInfection);

			if (SYM_SUCCEEDED(Status))
			{
				HandleCompressedInfection(pCompressedInfection, ulQuarFileStatus,
					bDeleteOriginal, szOriginalName, pItemToAdd);
			}
			else
			{
				return false;
			}
		}

        // Validate the quarantine item added
        if( pItemToAdd == NULL )
        {
            CCTRACEE(_T("CQuarantine::Add() - The quarantine item added is not valid."));
            return false;
        }

		// Add the quarantine item to our map using the ISymBase* of the scan infection
        // as the key
        ISymBaseQIPtr pQISymBase = pInfection;

        if( pQISymBase )
            m_ItemMap[pQISymBase.m_p] = pItemToAdd;
        else
            CCTRACEE(_T("CQuarantine::Add() - QI for ISymBase failed, this item will not be in the map."));

        // Does the caller want a copy?
        if( pItemOut != NULL )
        {
            // Release existing instance if it exists
            if( (*pItemOut) != NULL )
            {
                (*pItemOut)->Release();
                *pItemOut = NULL;
            }

            *pItemOut = pItemToAdd;
            (*pItemOut)->AddRef();
        }

	}
	catch(exception& Ex)
	{
		bResult = false;

		CCTRACEE(Ex.what());
	}
	catch(...)
	{
		bResult = false;

		CCTRACEE("Caught by CQuarantine::Add().");
	}
	
	return bResult;
}

bool CQuarantine::Delete(IScanInfection* pInfection)
{
	bool bResult = true;

	try
	{
		// Make sure we have a quarantine server object.
		if (m_pQuarServer == NULL)
		{
			throw runtime_error("CQuarantine::Delete() - Quarantine server is not ready.");
		}

        // Do this in the same file mode that the infection was detected under
        CFileAPI fileAPI;

        IScanCompressedFileInfection2QIPtr pCompressed = pInfection;
        IScanFileInfection2QIPtr pFile = pInfection;

        if( pFile.m_p != NULL )
        {
            if( pFile->AreOEMFileApis() )
                fileAPI.SwitchAPIToOEM();
            else
                fileAPI.SwitchAPIToANSI();
        }
        else if( pCompressed.m_p != NULL )
        {
            if( pCompressed->AreOEMFileApis() )
                fileAPI.SwitchAPIToOEM();
            else
                fileAPI.SwitchAPIToANSI();
        }

		// Get the quarantine item for this infection.
        ISymBaseQIPtr pQISymBase = pInfection;

        if( !pQISymBase )
        {
            throw runtime_error("CQuarantine::Delete() - Failed to QI for ISymBase.");
        }

        QUARANTINEMAP::iterator Iter = m_ItemMap.find(pQISymBase.m_p);

		if (Iter == m_ItemMap.end())
		{
			throw runtime_error("CQuarantine::Delete() - Item is not in the map.");
		}

		// Delete the specified item from quarantine.
		IQuarantineItem* pItem = (*Iter).second;
		if( pItem )
        {
			if( SUCCEEDED(pItem->DeleteItem()) )
                CCTRACEI("CQuarantine::Delete() - Successfully removed item from quarantine. Virus name = %s", pInfection->GetVirusName());
            else
                CCTRACEE("CQuarantine::Delete() - Failed to remove item from quarantine.  Virus name = %s", pInfection->GetVirusName());
        }

        // Remove this item from our map now
        m_ItemMap.erase(Iter);

        // After delete log out map size and vector size for verification
        CCTRACEI(_T("CQuarantine::Delete() - Quarantine item map size now = %d."), m_ItemMap.size());
	}
	catch(exception& Ex)
	{
		bResult = false;

		CCTRACEE(Ex.what());
	}
	catch(...)
	{
		bResult = false;

		CCTRACEE("Caught by CQuarantine::Delete().");
	}

	return bResult;
}

bool CQuarantine::RetrieveAssociatedQuarantineItem(IScanInfection* pInfection, IQuarantineItem** pItemRetrieved)
{
    if( pInfection == NULL )
    {
        CCTRACEE("CQuarantine::RetrieveAssociatedQuarantineItem() - Infection is null.");
        return false;
    }

    if( pItemRetrieved == NULL )
    {
        CCTRACEE("CQuarantine::RetrieveAssociatedQuarantineItem() - Out item is null.");
        return false;
    }

    if( (*pItemRetrieved) != NULL )
    {
        (*pItemRetrieved)->Release();
        *pItemRetrieved = NULL;
    }

    // Look up the cookie for this infection.
    ISymBaseQIPtr pQISymBase = pInfection;

    if( !pQISymBase )
    {
        CCTRACEE("CQuarantine::RetrieveAssociatedQuarantineItem() - Failed to QI for ISymBase.");
        return false;
    }

    QUARANTINEMAP::iterator Iter = m_ItemMap.find(pQISymBase.m_p);

    if (Iter == m_ItemMap.end())
    {
        CCTRACEE("CQuarantine::RetrieveAssociatedQuarantineItem() - Infection is not in the item map.");
        return false;
    }

    // Retrieve the specified item.
    (*pItemRetrieved) = (*Iter).second;
    if( (*pItemRetrieved) == NULL )
    {
        CCTRACEE("CQuarantine::RetrieveAssociatedQuarantineItem() - Failed to retrieve the item from the map.");
        return false;
    }

    // Reference count the item for the caller
    (*pItemRetrieved)->AddRef();

    return true;
}

bool CQuarantine::CanBeQuarantined(IScanInfection* pInfection)
{
    if( pInfection == NULL )
    {
        CCTRACEE(_T("CQuarantine::CanBeQuarantined() - The infection item is null."));
        return false;
    }

	// Is this a file infection?
	CScanPtr<IScanFileInfection> pFileInfection;
	SYMRESULT Status = pInfection->QueryInterface(IID_ScanFileInfection,
		(void**) &pFileInfection);

	if (SYM_SUCCEEDED(Status))
	{
		return true;
	}

	// Is this a compressed file infection?
	CScanPtr<IScanCompressedFileInfection> pCompressedInfection;
	Status = pInfection->QueryInterface(IID_ScanCompressedFileInfection,
		(void**) &pCompressedInfection);

	if (SYM_SUCCEEDED(Status))
	{
		return true;
	}

	return false;
}

void CQuarantine::HandleFileInfection(IScanFileInfection2* pInfection,
									  unsigned long ulQuarFileStatus,
									  bool bDeleteOriginal,
									  const char* szOriginalName,
                                      IQuarantineItem*& pItemAdded)
{
	// Get the file name
	vector<char> vFileName;
	int iBufferLen = strlen(pInfection->GetLongFileName());
	vFileName.reserve(iBufferLen + 1);

    CFileAPI fileAPI;
    ATL::CAtlString strFileName = pInfection->GetLongFileName();
    if( pInfection->AreOEMFileApis() )
    {
        // Our file paths are in OEM
        if( fileAPI.IsOriginalANSI() )
        {
            CCTRACEI(_T("CQuarantine::HandleFileInfection() - File paths are OEM but file APIs are ANSI, making the file APIs OEM for operation"));
            fileAPI.SwitchAPIToOEM();
        }
    }
    else
    {
        // Our file paths are in ANSI
        if( !fileAPI.IsOriginalANSI() )
        {
            CCTRACEI(_T("CQuarantine::HandleFileInfection() - File paths are ANSI but file APIs are OEM, making the file APIs ANSI for operation"));
            fileAPI.SwitchAPIToANSI();
        }
    }

	_tcsncpy(&vFileName[0], (LPCTSTR)strFileName, iBufferLen + 1);

	// If we have a possible SFN, try to find the LFN	
	if(_tcschr(&vFileName[0], '~') != NULL)
	{
		TCHAR szLongPath[MAX_PATH];
		if(CDisplayNames::GetLongPathName(&vFileName[0], szLongPath))
		{
			vFileName.reserve(strlen(szLongPath) + 1);
			_tcscpy(&vFileName[0], szLongPath);
		}
	}

	// Add the infected file to quarantine.
	HRESULT hResult = m_pQuarServer->CreateNewQuarantineItemFromFile(&vFileName[0], 
									QFILE_TYPE_NORMAL, 
									ulQuarFileStatus, 
									&pItemAdded );


	// Bail if we weren't able to quarantine the file
	if (FAILED(hResult) || pItemAdded == NULL)
	{
        CCTRACEE("CQuarantine::HandleFileInfection() - Failed to add %s to quarantine", &vFileName[0]);
		throw runtime_error("CQuarantine::HandleFileInfection() - Unable to add the file to quarantine.");
	}

    CCTRACEI("CQuarantine::HandleFileInfection() - Successfully added %s to quarantine", &vFileName[0]);

	// Delete the original file (if necessary).
	if (bDeleteOriginal == true)
	{
        if( m_pScanner == NULL )
            throw runtime_error("CQuarantine::HandleFileInfection() - Invalid scanner object.");

		if ( m_pScanner->DeleteInfection(pInfection) != SCAN_OK)
		{
			if( SUCCEEDED(pItemAdded->DeleteItem()) )
                CCTRACEI("CQuarantine::HandleFileInfection() - Successfully removed item from quarantine. Virus name = %s", pInfection->GetVirusName());
            else
                CCTRACEE("CQuarantine::HandleFileInfection() - Failed to remove item from qurantine.  Virus name = %s", pInfection->GetVirusName());

            // Release the output item if the delete fails
            pItemAdded->Release();
            pItemAdded = NULL;

			throw runtime_error("CQuarantine::HandleFileInfection() - Unable to delete the infected file.");
		}
	}
}

void CQuarantine::HandleCompressedInfection(IScanCompressedFileInfection2* pInfection,
											unsigned long ulQuarFileStatus,
											bool bDeleteOriginal,
											const char* szOriginalName,
                                            IQuarantineItem*& pItemAdded)
{
	// First extract the infected file.

    // Make sure the current file APIs match that of the infection object
    CFileAPI fileAPI;
    if( pInfection->AreOEMFileApis() )
    {
        // The infection paths are in OEM
        if( fileAPI.IsOriginalANSI() )
        {
            CCTRACEI("CQuarantine::HandleCompressedInfection() - Switching the file APIs to OEM since the compressed infection path is in OEM and the file APIs are ANSI.");
            fileAPI.SwitchAPIToOEM();
        }
    }
    else
    {
        // The infection file paths are in ANSI
        if( !fileAPI.IsOriginalANSI() )
        {
            CCTRACEI("CQuarantine::HandleCompressedInfection() - Switching the file APIs to ANSI since the compressed infection path is in ANSI and the file APIs are OEM.");
            fileAPI.SwitchAPIToANSI();
        }
    }

    CCTRACEI("CQuarantine::HandleCompressedInfection() - Extracting infected file");
    ATL::CAtlString strFileName;
	SCANSTATUS Status = m_pScanner->ExtractInfectedFile(pInfection,
		strFileName.GetBuffer(MAX_PATH+1), MAX_PATH);
    CCTRACEI("CQuarantine::HandleCompressedInfection() - Extracted infected file");

    strFileName.ReleaseBuffer();

	if (Status != SCAN_OK)
	{
            m_bAddFailedOnExtraction = true;
		throw runtime_error("CQuarantine::HandleCompressedInfection() - Unable to extract the infected file.");
	}

	// Make sure the file is not 0 bytes.
	// This can occur if another instance of Scan Manager has quarantined
	// the infection before we get here.
	HANDLE hFile = CreateFile(strFileName, 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
        CCTRACEE("CQuarantine::HandleCompressedInfection() - Unable to CreateFile on the extracted file = %s", (LPCTSTR)strFileName);
		throw runtime_error("CQuarantine::HandleCompressedInfection() - Unable to CreateFile on the extracted file.");
	}

	DWORD dwSizeHigh, dwSizeLow;
	dwSizeLow = GetFileSize(hFile, &dwSizeHigh);
	
	CloseHandle(hFile);

	if (dwSizeLow == 0 && dwSizeHigh == 0)
	{
        CCTRACEE("CQuarantine::HandleCompressedInfection() - The infected file is 0 bytes. File = %s", (LPCTSTR)strFileName);
		throw runtime_error("CQuarantine::HandleCompressedInfection() - The infected file is 0 bytes.");
	}

	// Add the infected file to quarantine.
    CCTRACEI("CQuarantine::HandleCompressedInfection() - Quarantining the extracted infected file");
    int nChars = strFileName.GetLength();
	HRESULT hResult = m_pQuarServer->CreateNewQuarantineItemFromFile(strFileName.GetBuffer(nChars+1), 
									QFILE_TYPE_NORMAL, 
									ulQuarFileStatus, 
									&pItemAdded );
    CCTRACEI("CQuarantine::HandleCompressedInfection() - Quarantined the extracted infected file. HRESULT = 0x%X", hResult);

    strFileName.ReleaseBuffer();

	// Safe delete the extracted file.
	SafeDeleteFile(strFileName);

	// Bail if we weren't able to quarantine the file.
	if (FAILED(hResult) || pItemAdded == NULL)
	{
        CCTRACEE("CQuarantine::HandleCompressedInfection() - Failed to add %s to quarantine", (LPCTSTR)strFileName);
		throw runtime_error("CQuarantine::HandleCompressedInfection() - Unable to add the file to quarantine.");
	}

	// Delete the infected file from the container (if necessary).
	if (bDeleteOriginal == true)
	{
		if( SCAN_OK != (Status = m_pScanner->DeleteInfection(pInfection)) )
        {
            // Undo the quarantine operation if weren't able to delete the infected file.
            pItemAdded->DeleteItem();
            pItemAdded->Release();
            pItemAdded = NULL;

            throw runtime_error("CQuarantine::HandleCompressedInfection() - Unable to delete the infected file.");
        }
	}
	
	// Otherwise set the original file name.
	pItemAdded->SetOriginalAnsiFilename( const_cast<char*>(szOriginalName) );
	pItemAdded->SaveItem();
}

BOOL CQuarantine::SafeDeleteFile(const char* szFileName)
{
	// Truncate the file and delete it.
	HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL,
		TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		COSInfo osi;
		if(osi.IsWinNT())
		{
			// Try again with unicode version for super long paths
			vector<WCHAR> vwFileName;
			CFileAPI::ConvertLongFileToUnicodeBuffer(szFileName, vwFileName);
			hFile = CreateFileW(&vwFileName[0], GENERIC_WRITE, 0, NULL,
			TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

            if (hFile == INVALID_HANDLE_VALUE)
            {
                CCTRACEE(_T("CQuarantine::SafeDeleteFile() - Failed to open handle to %s for the safe delete"), szFileName);
		        return FALSE;
            }

            // Close the handle to the file
            CloseHandle(hFile);

            // Delete the file using the unicode version
            return (DeleteFileW(&vwFileName[0]));
		}
        else
        {
            CCTRACEE(_T("CQuarantine::SafeDeleteFile() - Failed to open handle to %s for the safe delete"), szFileName);
		    return FALSE;
        }
	}
    else
    {
        // Close the handle
        CloseHandle(hFile);

        // Delete the file
        return(DeleteFile(szFileName));
    }
}

long CQuarantine::GetQuarantineCount()
{
    return m_ItemMap.size();
}

bool CQuarantine::TakeSystemSnapShot()
{
    if( m_bHaveSnapShot )
        return true;

    if (m_pQuarServer == NULL)
    {
        CCTRACEE(_T("CQuarantine::TakeSystemSnapShot() - Do not have a valid quarantine DLL object."));
        return false;
    }

    CFileAPI fileAPI;
    fileAPI.SwitchAPIToANSI();
    HRESULT hr = S_OK;
    if( FAILED(hr = m_pQuarServer->TakeSystemSnapshot()) )
    {
        CCTRACEE(_T("CQuarantine::TakeSystemSnapShot() - Failed with result 0x%X."), hr);
        return false;
    }
    fileAPI.RestoreOriginalAPI();
    
    m_bHaveSnapShot = true;
    return true;
}

bool CQuarantine::AddSnapShotToQuarantineItem(IQuarantineItem* pItem)
{
    if (pItem == NULL)
    {
        CCTRACEE(_T("CQuarantine::AddSnapShotToQuarantineItem() - Quarantine item is null."));
        return false;
    }

    // Make sure we have the snap shot
    if( !TakeSystemSnapShot() )
    {
        return false;
    }

    HRESULT hr = S_OK;
    if( FAILED(hr = m_pQuarServer->AddSnapshotToQuarantineItem(pItem)) )
    {
        CCTRACEE(_T("CQuarantine::AddSnapshotToQuarantineItem() - Failed with result 0x%X."), hr);
        return false;
    }

    return true;
}

bool CQuarantine::AddFailedOnExtraction() const
{
    return m_bAddFailedOnExtraction;
}

