// QMigrate1.cpp: implementation of the CQMigrate class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QMigrate.h"
#include "tchar.h"
#include <stdexcept>
#include "QuarantineDllLoader.h"
#include "NavInfo.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQMigrate::CQMigrate()
{
	m_bInitialized = false;
}

CQMigrate::~CQMigrate()
{
}

bool CQMigrate::ConvertFile(char* szFileName)
{
	// Confirm that this file is in the old file format
	if(!IsOldFormatFile(szFileName))
		return false;

	TCHAR szTempPath[MAX_PATH];
	TCHAR szRestoredFile[MAX_PATH];

	GetTempPath(MAX_PATH, szTempPath);

    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    _splitpath( szFileName, drive, dir, fname, ext );

	GetTempFileName(szTempPath, "tmp", 0, szRestoredFile);

    char drive2[_MAX_DRIVE];
    char dir2[_MAX_DIR];
    char fname2[_MAX_FNAME];
    char ext2[_MAX_EXT];
    _splitpath( szRestoredFile, drive2, dir2, fname2, ext2 );
   _makepath( szRestoredFile, drive2, dir2, fname2, ext );

	
	// Read the header information out of the file
	QFILE_HEADER_STRUCT fileHeader;
	if(!ReadHeader(szFileName, fileHeader))
		return false;

	QFILE_INFO_STRUCT fileInfoStruct = fileHeader.FileInfo;
	
	// Copy the infected file out of the old format to a temp file
	if(!RestoreItem(szFileName, szRestoredFile))
		return false;

	// Add the file back into Quarantine file
	// Create quarantine object
	CSymPtr<IQuarantineDLL> pQuarantine;
	CSymPtr<IQuarantineItem> pQuarItem;
	QuarDLL_Loader	QuarDllLoader;

    SYMRESULT symRes = QuarDllLoader.CreateObject(&pQuarantine);
    if( SYM_FAILED( symRes ) || pQuarantine.m_p == NULL )
    {
        g_Log.LogEx(_T("CQMigrate::ConvertFile() - Failed to create the quarantine dll object. Error code = 0x%X"), symRes);
        return false;
    }

    // Initialize the IQuarantineDLL.
    HRESULT hr = pQuarantine->Initialize();
    if( FAILED( hr ) )
    {
        g_Log.LogEx(_T("CQMigrate::ConvertFile() - Failed to initialize the quarantine dll object. Error code = 0x%X"), hr);
        pQuarantine.Release();
        return false;
    }
	
	hr = pQuarantine->CreateNewQuarantineItemFromFile(szRestoredFile, fileInfoStruct.dwFileType, fileInfoStruct.dwFileStatus, &pQuarItem);
    if( FAILED( hr ) || pQuarItem == NULL )
    {
        g_Log.LogEx(_T("CQMigrate::ConvertFile() - Failed to add file to quarantine. Error code = 0x%X"), hr);
        
        pQuarantine.Release();

        if( pQuarItem != NULL )
            pQuarItem.Release();
        return false;
    }

	// Set all needed attrbiutes on the new QSPAK file
    hr = pQuarItem->SetUniqueID(fileInfoStruct.uniqueID);
	hr = pQuarItem->SetFileStatus(fileInfoStruct.dwFileStatus);
    hr = pQuarItem->SetFileType(fileInfoStruct.dwFileType);

    hr = pQuarItem->SetDateQuarantined(&fileInfoStruct.stDateQuarantined);
    hr = pQuarItem->SetOriginalFileDates( &fileInfoStruct.stOriginalFileDateCreated,
                                     &fileInfoStruct.stOriginalFileDateAccessed,
                                     &fileInfoStruct.stOriginalFileDateWritten);

    hr = pQuarItem->SetDateOfLastScan( &fileInfoStruct.stDateOfLastScan);
    hr = pQuarItem->SetDateOfLastScanDefs( &fileInfoStruct.stDateOfLastScanDefs);
    hr = pQuarItem->SetDateSubmittedToSARC( &fileInfoStruct.stDateSubmittedToSARC);

    hr = pQuarItem->SetOriginalFilesize( fileInfoStruct.dwOriginalFileSize);
    hr = pQuarItem->SetOriginalAnsiFilename( fileInfoStruct.szOriginalAnsiFilename);

    hr = pQuarItem->SetOriginalOwnerName( fileInfoStruct.szOriginalOwnerName);
    hr = pQuarItem->SetOriginalMachineName(fileInfoStruct.szOriginalMachineName);
    hr = pQuarItem->SetOriginalMachineDomain( fileInfoStruct.szOriginalMachineDomain);
    hr = pQuarItem->SaveItem();


	// Delete the temporary file we created
	TruncateFile(szRestoredFile);

	// Delete the file we converted
	TruncateFile(szFileName);

    // Explicitly release the objects before the loader frees the dll
    pQuarantine.Release();
    pQuarItem.Release();

	return true;
}

bool CQMigrate::Initialize(char *szQuarantinePath)
{
	_tcscpy(m_szQuarantinePath, szQuarantinePath);
	
	m_bInitialized = true;
	return true;
}

bool CQMigrate::UpdateOldFormatFiles()
{

	// Iterate through the given quarantine directory
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;


	TCHAR szFindName[MAX_PATH];
	_tcscpy(szFindName, m_szQuarantinePath);
	_tcscat(szFindName, "\\*");

	hFind = FindFirstFile(szFindName, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;

	while(hFind != INVALID_HANDLE_VALUE)
	{
		// For each file that we find, check to make sure it is an file int he Old format
		if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			TCHAR szFilePath[MAX_PATH];
			_tcscpy(szFilePath, m_szQuarantinePath);
			_tcscat(szFilePath, "\\");
			_tcscat(szFilePath, FindFileData.cFileName );
			if(IsOldFormatFile(szFilePath))
			{
				// Convert the file over to the new format
				ConvertFile(szFilePath);
			}
		}
		// Iterate
		if(FALSE == FindNextFile(hFind, &FindFileData))
			break;
	}

	FindClose(hFind);
	return true;
}

bool CQMigrate::IsOldFormatFile(char *szFileName)
{
	QFILE_HEADER_STRUCT fileHeader;
	
	if(!ReadHeader(szFileName, fileHeader))
		return false;

	if( QUARANTINE_SIGNATURE == fileHeader.dwSignature )
		return true;

	return false;
}

bool CQMigrate::ReadHeader(char *szFileName, QFILE_HEADER_STRUCT &fileHeader)
{
	BOOL bResult;
    DWORD dwNumBytesRead;
	HANDLE hFileHandle = NULL;

	try
	{
		// initialize the file header structure
		memset( (void*) &fileHeader, '\0', sizeof(fileHeader) );

		// open the file and read in the header
		hFileHandle = CreateFile( szFileName                         // filename
									, GENERIC_READ                      // access
									, FILE_SHARE_READ                   // don't share
									, NULL                              // don't allow handle inheritance
									, OPEN_EXISTING                     // only open if it exists
									, FILE_FLAG_SEQUENTIAL_SCAN         // attributes and flags
									, NULL                              // no template handle
								 );

		if (INVALID_HANDLE_VALUE == hFileHandle)
			throw runtime_error("Could not open given input file");

		// read in the file header
		bResult = ReadFile( hFileHandle                        // file handle
								, (LPVOID) &fileHeader        // address of destination buffer
								, sizeof(fileHeader)          // number of bytes to read
								, (LPDWORD) &dwNumBytesRead     // number of bytes read
								, NULL                          // no overlapped structure used
						  );

		// return if we failed to read the file
		if ( 0 == bResult )
		{
			throw runtime_error("Could not read given input file");
		}
	}

	catch(...)
	{	
		// Cleanup
		if(hFileHandle)
			CloseHandle(hFileHandle);

		return false;
	}

	// Cleanup
	CloseHandle(hFileHandle);

	return true;
}




bool CQMigrate::RestoreItem(char* szQuarFile, char* szUnpackagedFile)
{
    DWORD           dwBytesRead = 0;
    DWORD           dwBytesWritten = 0;
    DWORD           dwCreationMode = 0;
    DWORD           dwFlagsAndAttributes = 0;
    HANDLE          destFileHandle = INVALID_HANDLE_VALUE;
    HANDLE          inFileHandle = INVALID_HANDLE_VALUE;
	int             nPathLen = 0;
    FILETIME        ftCreation;
    FILETIME        ftAccess;
    FILETIME        ftLastWrite;
    FILETIME        ftZero;
    LPBYTE          lpSecurityDesc = NULL;
    LPBYTE          tempByteBuf = NULL;
	HRESULT			retVal = S_OK;
	BOOL            bResult = TRUE;


    // Allocate transfer buffer.
    tempByteBuf = new BYTE[QUARANTINE_TRANSFER_BUFFER_SIZE];
    if( tempByteBuf == NULL )
        return false;


    // if the caller did not supply a valid destination directory, return
    if ( NULL == szQuarFile || NULL == szUnpackagedFile)
	{
		delete [] tempByteBuf;
		return false;
	}

    // open the source file
   inFileHandle = CreateFile( szQuarFile                // filename
                                , GENERIC_READ          // access
                                , FILE_SHARE_READ       // Allow read access to others
                                , NULL                  // don't allow handle inheritance
                                , OPEN_EXISTING         // only open if it exists
                                , FILE_ATTRIBUTE_NORMAL // attributes and flags
                                , NULL );               // no template handle
    if (INVALID_HANDLE_VALUE == inFileHandle)
	{
		delete [] tempByteBuf;
		return false;
	}

    // Save off security attributes.
    if( FAILED( GetFileSecurityDesc( szQuarFile, &lpSecurityDesc ) ) )
    {
		delete [] tempByteBuf;
        CloseHandle(inFileHandle);
        return false;
    }


    // open the destination file.

    destFileHandle = CreateFile( szUnpackagedFile             // filename
                                        , GENERIC_WRITE         // access
                                        , 0                     // don't share
                                        , NULL                  // don't allow handle inheritance
                                        , CREATE_ALWAYS        // creation mode
                                        , FILE_ATTRIBUTE_TEMPORARY  // attributes and flags
                                        , NULL );               // no template handle
     if ( INVALID_HANDLE_VALUE == destFileHandle )
     {
		delete [] lpSecurityDesc;
		delete [] tempByteBuf;
		CloseHandle(inFileHandle);
		return false;
	 }

    // Set proper security attributes.
    SetFileSecurityDesc( szUnpackagedFile , lpSecurityDesc );

	// Read in the file header
	QFILE_HEADER_STRUCT fileHeader;
	if(!ReadHeader(szQuarFile, fileHeader))
	{
		delete [] lpSecurityDesc;
		delete [] tempByteBuf;
		CloseHandle(inFileHandle);
		CloseHandle(destFileHandle);
		return false;
	}
	QFILE_INFO_STRUCT fileInfoStruct = fileHeader.FileInfo;

    // position the source file so we're pointing at file data (not the header)
    dwBytesRead = SetFilePointer(inFileHandle, fileHeader.dwHeaderBytes, NULL, FILE_BEGIN);
    if (0xFFFFFFFF == dwBytesRead)
    {
		delete [] lpSecurityDesc;
		delete [] tempByteBuf;
		CloseHandle(inFileHandle);
		CloseHandle(destFileHandle);
		return false;
    }

    // Transfer the contents of the Quarantined file to the destination file.
    retVal = TransferFile( szQuarFile, inFileHandle, szUnpackagedFile, destFileHandle );
    if( FAILED( retVal ) )
    {
		delete [] lpSecurityDesc;
		delete [] tempByteBuf;
		CloseHandle(inFileHandle);
		CloseHandle(destFileHandle);
		return false;
    }

    // Create a zero file time
    ftZero.dwLowDateTime = 0;
    ftZero.dwHighDateTime = 0;

    // Write the file date/times
    bResult = SystemTimeToFileTime( &fileInfoStruct.stOriginalFileDateCreated,  &ftCreation );
    bResult = SystemTimeToFileTime( &fileInfoStruct.stOriginalFileDateAccessed, &ftAccess );
    bResult = SystemTimeToFileTime( &fileInfoStruct.stOriginalFileDateWritten,  &ftLastWrite );

    // Set the file time.  If any of the times is 0, then do not set that value.
    bResult = SetFileTime( destFileHandle,
                           ( 0 != CompareFileTime( &ftCreation, &ftZero ) )  ? &ftCreation : NULL,
                           ( 0 != CompareFileTime( &ftAccess, &ftZero ) )    ? &ftAccess : NULL,
                           ( 0 != CompareFileTime( &ftLastWrite, &ftZero ) ) ? &ftLastWrite : NULL );

    // close files
    if (INVALID_HANDLE_VALUE != inFileHandle)
    {
        CloseHandle( inFileHandle );
        inFileHandle = INVALID_HANDLE_VALUE;
    }

    if (INVALID_HANDLE_VALUE != destFileHandle)
    {
        CloseHandle( destFileHandle );
    }

    // Cleanup
    if( lpSecurityDesc )
    {
        delete [] lpSecurityDesc;
        lpSecurityDesc = NULL;
    }

    if( tempByteBuf )
    {
        delete [] tempByteBuf;
        tempByteBuf = NULL;
    }

    return true;

}  


BOOL CQMigrate::TruncateFile(LPCTSTR lpcszFilePath)
{
    BOOL bReturn = FALSE;

    // Change the file attributes to not be read-only.
    //
    DWORD dwFileAttr = GetFileAttributes( lpcszFilePath );

    if (0xFFFFFFFF != dwFileAttr)
    {
        if( ( dwFileAttr & FILE_ATTRIBUTE_READONLY ) == FILE_ATTRIBUTE_READONLY )
        {
            dwFileAttr -= FILE_ATTRIBUTE_READONLY;
        }

        SetFileAttributes( lpcszFilePath, dwFileAttr );

        // Truncate the file to 0 bytes so the NProtect doesn't
        // put it in the protected Recycle Bin
        //
        HANDLE hFile = 0;

        hFile = CreateFile( lpcszFilePath,         // pointer to name of the file
                            GENERIC_WRITE,      // access (read-write) mode
                            0,                  // share mode
                            NULL,               // pointer to security attributes
                            TRUNCATE_EXISTING,  // how to create
                            NULL,               // file attributes
                            NULL );

        if ( hFile )
        {
            // Looks good
            //        
            bReturn = TRUE;
            CloseHandle ( hFile );
			DeleteFile(lpcszFilePath);
        }
    }

    return bReturn;
}

////////////////////////////////////////////////////////////////////////////
// Function name    : TransferFile
//
// Description      : Append the source file from the current location on,
//                    to the current location of the destination file.
//                    The function assumes valid handles, and correct permissions.
//
// Return type		: HRESULT
//
// Argument         : HANDLE hSource - Source file
// Argument         : HANDLE hDest - Destination file
//
////////////////////////////////////////////////////////////////////////////
// 5/17/98 JTAYLOR - Function created / Header added.
// 7/08/98 JTAYLOR - Updated transfer file to XOR the data by FF
////////////////////////////////////////////////////////////////////////////
HRESULT CQMigrate::TransferFile( LPCTSTR lpszSourceName, HANDLE hSource, LPCTSTR lpszDestName, HANDLE hDest )
{
    LPBYTE              pFileTransferBuff = NULL;
    DWORD               dwBytesWritten;
    DWORD               dwBytesRead;
    BOOL                bResult = FALSE;
    HRESULT             hr = S_OK;

    // Allocate memory for the file transfer buffer
    pFileTransferBuff = new BYTE[QUARANTINE_TRANSFER_BUFFER_SIZE];
    if( NULL == pFileTransferBuff )
        {
        return E_OUTOFMEMORY;
        }

    // Zero out the buffer
    memset(pFileTransferBuff,0,QUARANTINE_TRANSFER_BUFFER_SIZE);

    do
        {
        // Attempt a synchronous read operation.
        bResult = ReadFile(hSource, pFileTransferBuff, QUARANTINE_TRANSFER_BUFFER_SIZE, &dwBytesRead, NULL);

        if( FALSE == bResult )
            {
            hr = E_FAIL;
            break;
            }

        if( dwBytesRead > 0)
            {
				// XOR the bytes that are in memory
				for(unsigned int i = 0; i< dwBytesRead; i++ )
                {
					pFileTransferBuff[i] ^= 0xFF;
                }
            
				bResult = WriteFile(hDest, pFileTransferBuff, dwBytesRead, &dwBytesWritten, NULL);
            if( FALSE == bResult )
                {
                hr = E_FAIL;
                break;
                }

            if(dwBytesRead != dwBytesWritten)
				{
                hr = E_FAIL;
                break;
			}
            }
        } while(dwBytesRead != 0);

    // Free the buffer
    if( NULL != pFileTransferBuff )
        {
        delete []pFileTransferBuff;
        pFileTransferBuff = NULL;
        }
/*
    //
    // Copy over any alternate data streams if running on NT.
    //
    if( hr == S_OK && g_pfnCopyAlternateDataStreams )
        {
        hr = g_pfnCopyAlternateDataStreams( hSource, lpszSourceName, lpszDestName ) ?
                            S_OK : E_FAIL;
        }
*/
    return hr;
}

////////////////////////////////////////////////////////////////////////////
// Function name    : GetFileSecurityDesc
//
// Description	    : Gets the security descriptor for a given file
//
// Return type		: HRESULT
//
// Argument         : LPSTR lpszSourceFileName - file to get security attributes from
// Argument         : LPBYTE *pDesc - pointer to buffer pointer.  Note that this
//                    function allocates this buffer.  It must be freed by the
//                    calling routine.
//
////////////////////////////////////////////////////////////////////////////
// 5/12/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
HRESULT CQMigrate::GetFileSecurityDesc( LPSTR lpszFileName, LPBYTE *pDesc )
{
    // Do nothing if not on NT.
    if( FALSE == IsWinNT() )
        {
        return S_OK;
        }

    // Get security attributes of original file.
    SECURITY_INFORMATION si = OWNER_SECURITY_INFORMATION |
                              GROUP_SECURITY_INFORMATION |
                              DACL_SECURITY_INFORMATION;

    // This call should fail, returning the length needed for the
    // security descriptor.
    DWORD dwLengthNeeded = 0;

    if (!GetFileSecurity( lpszFileName,
                     si,
                     (PSECURITY_DESCRIPTOR) *pDesc,
                     0,
                     &dwLengthNeeded ))
    {
        // ERROR_NOT_SUPPORTED will be returned when asking for
        // the security descriptor from a file on a non-NT system.
        // Lie about it and tell the caller ewverything is ok.
        if (GetLastError() == ERROR_NOT_SUPPORTED)
            return S_OK;
    }

    // Allocate space needed for call.
    *pDesc = new BYTE[dwLengthNeeded];
    if( *pDesc == NULL )
        {
        return E_OUTOFMEMORY;
        }

    // This should get the actual security descriptor.
    if( FALSE == GetFileSecurity( lpszFileName,
                     si,
                     (PSECURITY_DESCRIPTOR) *pDesc,
                     dwLengthNeeded,
                     &dwLengthNeeded ) )
        {
        return E_FAIL;
        }

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////
// Function name    : SetFileSecurityDesc
//
// Description	    : Set file security for a filesystem object
//
// Return type		: HRESULT
//
// Argument         : LPSTR szFileName - File to modify
// Argument         : LPBYTE pDesc     - descriptor to set
//
////////////////////////////////////////////////////////////////////////////
// 5/12/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
HRESULT CQMigrate::SetFileSecurityDesc( LPSTR lpszFileName, LPBYTE pDesc )
{
    // Do nothing if not on NT.
    if( FALSE == IsWinNT() )
        {
        return S_OK;
        }

    if( pDesc != NULL )
        {
        SECURITY_INFORMATION si = OWNER_SECURITY_INFORMATION |
                                  GROUP_SECURITY_INFORMATION |
                                  DACL_SECURITY_INFORMATION;

        // Ignore return result.
        SetFileSecurity( lpszFileName,
                         si,
                         pDesc );
        }

    return S_OK;
}


BOOL CQMigrate::IsWinNT( void )
{
    OSVERSIONINFO os;
    os.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &os );
    return (BOOL)( os.dwPlatformId == VER_PLATFORM_WIN32_NT );
}

bool CQMigrate::EnableAutoProtect(bool bEnable)
{
	bool bResult = true;

	try
	{	
		if (bEnable == true)
		{
			// Unregiser with the AP Toggle class to protect this process.
			m_APWrapper.EnableAPForThisThread();
		}
		else
		{
			// Regiser with the AP Toggle class to unprotect this process.
			m_APWrapper.DisableAPForThisThread();
		}
	}
	catch(exception& )
	{
		bResult = false;
	}
	
	return bResult;
}