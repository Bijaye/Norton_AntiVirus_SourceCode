// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// uncab.cpp 
//

#include "stdafx.h"
#include "uncab.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <dos.h>
#include <sys/stat.h>
#include <tchar.h>
#include "fdi.h"
#include "resource.h"

//////////////////////////////////////////////////////////////////////////
//
// Function: MsiUnCabRuntimeDlls()
//
// Description: This function pulls the Microsoft Runtime dll's from the
//	installers data1.cab file and puts them in the temp folder
//
// Return Values: 0 if successful.
//
//////////////////////////////////////////////////////////////////////////
// 10/09/2004 - DKOWALYSHYN  
//////////////////////////////////////////////////////////////////////////
UINT __stdcall MsiUnCabRuntimeDlls( MSIHANDLE hInstall )
{
	TCHAR			szCabPath[MAX_PATH]					= {0};
	TCHAR			strCADTemp[MAX_PATH]				= {0};
	TCHAR			szTempFolder[MAX_PATH]				= {0};
	TCHAR			szSOURCEDIR[MAX_PATH]				= {0};
	TCHAR			errorMessage[DOUBLE_MAX_PATH]		= {0};
	TCHAR			szDestDirectory[MAX_PATH]			= {0};
	TCHAR			szInstallsourceDir[MAX_PATH]		= {0};
	TCHAR			szCabsourceDir[MAX_PATH]			= {0};
	TCHAR			szMIDTARGETDIR[MAX_PATH]			= {0};
	TCHAR			szMIDSOURCEDIR[MAX_PATH]			= {0};
	TCHAR			szTARGETDIR[MAX_PATH]				= {0};
	TCHAR			*pdeststr							= NULL;
	CString			strData1CabName;
	CString			szSYSTEM32Path;

	DWORD			strCADTempLen						= sizeof(strCADTemp);
	DWORD			dwLen								= 0;
	UINT			returnVal							= 0;						
	INT				commalocation						= 0;

	// Install Source dir & temp folder
	dwLen = sizeof( szTempFolder );
	MsiGetProperty( hInstall, "TempFolder", szTempFolder, &dwLen );
	dwLen = sizeof( szSOURCEDIR );
	MsiGetProperty( hInstall, "SourceDir", szSOURCEDIR, &dwLen );
	vpstrncpy (szInstallsourceDir, szSOURCEDIR,  sizeof(szInstallsourceDir));
	vpstrncpy (szDestDirectory,    szTempFolder, sizeof(szDestDirectory));

	// Loadstrings
	szSYSTEM32Path.LoadString( IDS_SYSTEM32 );

	vpsnprintf(errorMessage, sizeof (errorMessage), "TempFolder : szInstallsourceDir %s.", szInstallsourceDir);
	MSILogMessage(hInstall, errorMessage);
	OutputDebugString( errorMessage );

	vpsnprintf(errorMessage, sizeof (errorMessage), "TempFolder : szDestDirectory %s.", szDestDirectory);
	MSILogMessage(hInstall, errorMessage);
	OutputDebugString( errorMessage );

	AddSlash( szDestDirectory,    sizeof (szDestDirectory) );
	AddSlash( szInstallsourceDir, sizeof (szInstallsourceDir) );

	// Cat the data1.cab file name
	vpstrncpy (szCabsourceDir, szInstallsourceDir, sizeof(szCabsourceDir));
	strData1CabName.LoadString( IDS_DATA1_CAB );
	vpstrnappend (szCabsourceDir, strData1CabName, sizeof (szCabsourceDir));
	// Set the dest_dir which is used in the CAB lib
	vpstrncpy (dest_dir, szDestDirectory, sizeof(dest_dir));

	vpsnprintf(errorMessage, sizeof (errorMessage), "TempFolder : szCabsourceDir %s.", szCabsourceDir);
	MSILogMessage(hInstall, errorMessage);
	OutputDebugString( errorMessage );

	vpsnprintf(errorMessage, sizeof (errorMessage), "TempFolder : szDestDirectory szFile %s.", szDestDirectory);
	MSILogMessage(hInstall, errorMessage);
	OutputDebugString( errorMessage );

	vpsnprintf(errorMessage, sizeof (errorMessage), "Uncab : strData1CabName %s,  szInstallsourceDir %s.",
		strData1CabName, szInstallsourceDir);
	MSILogMessage(hInstall, errorMessage);
	OutputDebugString( errorMessage );

	// Pass the data1.cab source path to the uncab function to dump out the required files
	if (UnCabRuntimeDlls(hInstall, strData1CabName.GetBuffer(), szInstallsourceDir ) == TRUE)
	{
		strData1CabName.ReleaseBuffer();
		MSILogMessage(hInstall,"UnCabRuntimeDlls was successful\n");
		OutputDebugString( errorMessage );
	}
	else
	{
		// This code is only called if we can't find the data1.cab and we are probably an administrative install
		strData1CabName.ReleaseBuffer();
		// create the sourcdir paths
		vpstrncpy    (szMIDSOURCEDIR, szInstallsourceDir, sizeof(szMIDSOURCEDIR));
		vpstrnappend (szMIDSOURCEDIR, szSYSTEM32Path,     sizeof(szMIDSOURCEDIR));
		AddSlash     (szMIDSOURCEDIR,                     sizeof (szMIDSOURCEDIR));
		// create the destdir path
		vpstrncpy    (szMIDTARGETDIR, szDestDirectory, sizeof(szMIDTARGETDIR));
		// Copy the Data1.cab file from the administrative install location
		for ( int nCount = 0; nCount < NUM_ADMIN_DATA1_DLLS; nCount++ )
		{
			vpstrncpy(szSOURCEDIR, szMIDSOURCEDIR, sizeof (szSOURCEDIR)); // Set the midsource strings
			vpstrncpy(szTARGETDIR, szMIDTARGETDIR, sizeof (szTARGETDIR));

			vpstrnappend(szSOURCEDIR, pszADMINInstallSRCFiles[nCount], sizeof (szSOURCEDIR)); // Append filenames
			vpstrnappend(szTARGETDIR, pszADMINInstallSRCFiles[nCount], sizeof (szTARGETDIR));

			vpsnprintf(errorMessage, sizeof (errorMessage), "TempFolder : szSOURCEDIR %s", szSOURCEDIR);
			MSILogMessage(hInstall, errorMessage);

			vpsnprintf(errorMessage, sizeof (errorMessage), "TempFolder : szTARGETDIR szFile %s", szTARGETDIR);
			MSILogMessage(hInstall, errorMessage);

			if ( CopyFile( szSOURCEDIR, szTARGETDIR, FALSE ) )
			{
				vpsnprintf( errorMessage, sizeof (errorMessage), "CopyFile - copied from %s to %s success", szSOURCEDIR, szTARGETDIR);
				MSILogMessage( hInstall, errorMessage );
			}
			else
			{
				vpsnprintf( errorMessage, sizeof (errorMessage), "CopyFile - copied from %s to %s failure - Get Last Error reports %d", szSOURCEDIR, szTARGETDIR,  GetLastError ());
				MSILogMessage( hInstall, errorMessage );
			}
		}
		// Copy the SSC Shared file from the administrative install location
		// create the sourcdir paths
		vpstrncpy    (szMIDSOURCEDIR, szInstallsourceDir, sizeof(szMIDSOURCEDIR));
		vpstrnappend (szMIDSOURCEDIR, pszSSC_SRC_PATHS,   sizeof(szMIDSOURCEDIR));
		for ( int nCount = 0; nCount < NUM_ADMIN_SSC_DLLS; nCount++ )
		{
			vpstrncpy(szSOURCEDIR, szMIDSOURCEDIR, sizeof (szSOURCEDIR)); // Set the midsource strings
			vpstrncpy(szTARGETDIR, szMIDTARGETDIR, sizeof (szTARGETDIR));

			vpstrnappend(szSOURCEDIR, pszADMINInstallSSCFiles[nCount], sizeof (szSOURCEDIR)); // Append filenames
			vpstrnappend(szTARGETDIR, pszADMINInstallSSCFiles[nCount], sizeof (szTARGETDIR));

			vpsnprintf(errorMessage, sizeof (errorMessage), "TempFolder : szSOURCEDIR %s", szSOURCEDIR);
			MSILogMessage(hInstall, errorMessage);

			vpsnprintf(errorMessage, sizeof (errorMessage), "TempFolder : szTARGETDIR szFile %s", szTARGETDIR);
			MSILogMessage(hInstall, errorMessage);

			if ( CopyFile( szSOURCEDIR, szTARGETDIR, FALSE ) )
			{
				vpsnprintf( errorMessage, sizeof (errorMessage), "CopyFile - copied from %s to %s success", szSOURCEDIR, szTARGETDIR);
				MSILogMessage( hInstall, errorMessage );
			}
			else
			{
				vpsnprintf( errorMessage, sizeof (errorMessage), "CopyFile - copied from %s to %s failure - Get Last Error reports %d", szSOURCEDIR, szTARGETDIR,  GetLastError ());
				MSILogMessage( hInstall, errorMessage );
			}
		}
	}

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: RemoveFile()
//
// Description: This function cleans up files by removing them from
// wherever then maybe on the disk.
//
// Return Values: 0 if successful.
//
//////////////////////////////////////////////////////////////////////////
// 10/27/2005 - DKOWALYSHYN  
//////////////////////////////////////////////////////////////////////////
BOOL RemoveFile(MSIHANDLE hInstall, TCHAR *szTargetFileName )
{
	TCHAR			szBuf[MAX_PATH]						= {0};

	// Set file attribs so we can Deletefile or MoveFileEx them
	if ( !SetFileAttributes ( szTargetFileName, FILE_ATTRIBUTE_NORMAL ))
	{
		vpsnprintf(szBuf, sizeof (szBuf), "MsiCleanCabRuntimeDlls - SetFileAttributes failed %s", szTargetFileName); 
		MSILogMessage( hInstall, szBuf );
	}
	else
	{
		vpsnprintf(szBuf, sizeof (szBuf), "MsiCleanCabRuntimeDlls - SetFileAttributes succeeded %s", szTargetFileName); 
		MSILogMessage( hInstall, szBuf );
	}

	if( !DeleteFile( szTargetFileName ) )
	{
		vpsnprintf(szBuf, sizeof (szBuf), "MsiCleanCabRuntimeDlls - DeleteFile failed %s", szTargetFileName); 
		MSILogMessage( hInstall, szBuf );
		MoveFileEx( szTargetFileName, NULL, MOVEFILE_DELAY_UNTIL_REBOOT );
	}
	else
	{
		vpsnprintf(szBuf, sizeof (szBuf), "MsiCleanCabRuntimeDlls - DeleteFile succeeded %s", szTargetFileName); 
		MSILogMessage( hInstall, szBuf );
	}
	return ( TRUE );
}


//////////////////////////////////////////////////////////////////////////
//
// Function: MsiCleanCabRuntimeDlls()
//
// Description: This function cleans up after MsiUnCabRuntimeDlls() by removing
//	all the MSVCRT.dll's, SCSComms.dll and PDS files the temp folder.
//
// Return Values: 0 if successful.
//
//////////////////////////////////////////////////////////////////////////
// 11/03/2004 - DKOWALYSHYN  
//////////////////////////////////////////////////////////////////////////
UINT __stdcall MsiCleanCabRuntimeDlls( MSIHANDLE hInstall )
{
	TCHAR			strTargetFile[MAX_PATH]				= {0};
	TCHAR			szFile[MAX_PATH]					= {0};
	TCHAR			szTempFolder[MAX_PATH]				= {0};
	TCHAR			szBuf[MAX_PATH]						= {0};
	DWORD			dwLen;

	// get the Install temp folder
	dwLen = sizeof( szTempFolder );
	if ( 0 < GetTempPath ((sizeof(szTempFolder)), szTempFolder))
	{
		vpsnprintf(szBuf, sizeof (szBuf), "MsiCleanCabRuntimeDlls - GetTempPath succeeded %s", szTempFolder); 
		MSILogMessage( hInstall, szBuf );
		for ( int i = 0; i < NUM_VC_RUN_DLLS; i++ )
		{
			vpstrncpy    (szFile, szTempFolder,    sizeof (szFile));
			AddSlash     (szFile,                  sizeof (szFile));
			vpstrnappend (szFile, pszDestFiles[i], sizeof (szFile));
			// Set file attribs so we can Deletefile or MoveFileEx them
			RemoveFile ( hInstall, szFile );
		}
		// Code to remove LUSetup.exe from the temp folder just prior to the install finishing
		vpstrncpy    (szFile, szTempFolder,    sizeof (szFile));
		AddSlash     (szFile,                  sizeof (szFile));
		vpstrnappend (szFile, pszLUSETUPFile, sizeof (szFile));
		RemoveFile ( hInstall, szFile );
	}
	else
	{
		vpsnprintf(szBuf, sizeof (szBuf), "MsiCleanCabRuntimeDlls - GetTempPath Failed %s", szTempFolder); 
		MSILogMessage( hInstall, szBuf );
	}
	return ERROR_SUCCESS;
}



/*
 * Memory allocation function
 */
FNALLOC(mem_alloc)
{
	return malloc(cb);
}


/*
 * Memory free function
 */
FNFREE(mem_free)
{
	free(pv);
}


FNOPEN(file_open)
{
	return _open(pszFile, oflag, pmode);
}


FNREAD(file_read)
{
	return _read(hf, pv, cb);
}


FNWRITE(file_write)
{
	return _write(hf, pv, cb);
}


FNCLOSE(file_close)
{
	return _close(hf);
}


FNSEEK(file_seek)
{
	return _lseek(hf, dist, seektype);
}

FNFDINOTIFY(notification_function)
{
	switch (fdint)
	{
		case fdintCABINET_INFO: // general information about the cabinet
			return 0;

		case fdintPARTIAL_FILE: // first file in cabinet is continuation
			return 0;

		case fdintCOPY_FILE:	// file to be copied
		{
			int		handle;
			char	destination[256];

			for( int count = 0; count < NUM_VC_RUN_DLLS; count++ )
			{
				if (0 == strcmp( pfdin->psz1, pszTargetFiles[count]))
				{
                    vpstrncpy    (destination, dest_dir,            sizeof (destination));
                    vpstrnappend (destination, pszDestFiles[count], sizeof (destination));

					OutputDebugString( destination ); 

					handle = file_open(
						destination,
						_O_BINARY | _O_CREAT | _O_WRONLY | _O_SEQUENTIAL,
						_S_IREAD | _S_IWRITE 
					);

					return handle;
				}
			}
			return 0; /* skip file */
		}

		case fdintCLOSE_FILE_INFO:	// close the file, set relevant info
        {
            HANDLE  handle;
            DWORD   attrs;
            char    destination[256];

/* 			printf(
				"fdintCLOSE_FILE_INFO\n"
				"   file name in cabinet = %s\n"
				"\n",
				pfdin->psz1
			);*/


			// match cab name to the real file name
			for( int count = 0; count < NUM_VC_RUN_DLLS; count++ )
			{
				if (0 == strcmp( pfdin->psz1, pszTargetFiles[count]))
				{
                    vpstrncpy    (destination, dest_dir,            sizeof (destination));
                    vpstrnappend (destination, pszDestFiles[count], sizeof (destination));
				}
			}
			file_close(pfdin->hf);

            /*
             * Set date/time
             *
             * Need Win32 type handle for to set date/time
             */
            handle = CreateFile(
                destination,
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL
            );

            if (handle != INVALID_HANDLE_VALUE)
            {
                FILETIME    datetime;

                if (TRUE == DosDateTimeToFileTime(
                    pfdin->date,
                    pfdin->time,
                    &datetime))
                {
                    FILETIME    local_filetime;

                    if (TRUE == LocalFileTimeToFileTime(
                        &datetime,
                        &local_filetime))
                    {
                        (void) SetFileTime(
                            handle,
                            &local_filetime,
                            NULL,
                            &local_filetime
                        );
                     }
                }

                CloseHandle(handle);
            }

            /*
             * Mask out attribute bits other than readonly,
             * hidden, system, and archive, since the other
             * attribute bits are reserved for use by
             * the cabinet format.
             */
            attrs = pfdin->attribs;

            attrs &= (_A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH);

            (void) SetFileAttributes(
                destination,
                attrs
            );

			return TRUE;
        }

		case fdintNEXT_CABINET:	// file continued to next cabinet
			return 0;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: UnCabRuntimeDlls()
//
// Description: This function opens up the SAV & SCS data1.cab file and
//  dumps the MSRuntime dll's and the SAV PDS Dll's in the our temp folder.
//	These are later used by the installer when doing any managed client or
//	server discovery.
//
// Return Values: 0 if successful.
//
//////////////////////////////////////////////////////////////////////////
// 10/09/2004 - DKOWALYSHYN
// 09/01/2005 - DKOWALYSHYN - Updated and cleaned up some unused code
//////////////////////////////////////////////////////////////////////////
BOOL UnCabRuntimeDlls(MSIHANDLE hInstall, TCHAR *szCabinetFileName, TCHAR *szCabinetPathName)
{
	HFDI			hfdi;
	ERF				erf;
	FDICABINETINFO	fdici;
	int				hf;
	TCHAR			cabinet_fullpath[MAX_PATH]		= {0};
	TCHAR			errorMessage[DOUBLE_MAX_PATH]	= {0};

	// create the fullpath to the data1.cab target file
	vpstrncpy(cabinet_fullpath, szCabinetPathName, sizeof (cabinet_fullpath));
	vpstrnappend(cabinet_fullpath, szCabinetFileName, sizeof (cabinet_fullpath));

	hfdi = FDICreate(
		mem_alloc,
		mem_free,
		file_open,
		file_read,
		file_write,
		file_close,
		file_seek,
		cpu80386,
		&erf
	);

	if (hfdi == NULL)
	{
		vpsnprintf(errorMessage, sizeof (errorMessage), "UnCabRuntimeDlls() failed: code %d [%s]\n",
			 erf.erfOper, return_fdi_error_string(erf.erfOper));
		MSILogMessage(hInstall, errorMessage);

		return FALSE;
	}

	/*
	 * Is this file really a cabinet?
	 */
	hf = file_open(
		cabinet_fullpath,
		_O_BINARY | _O_RDONLY | _O_SEQUENTIAL,
		0
	);

	if (hf == -1)
	{
		(void) FDIDestroy(hfdi);
		vpsnprintf(errorMessage, sizeof (errorMessage), "Unable to open '%s' for input\n", cabinet_fullpath);
		MSILogMessage(hInstall, errorMessage);
		return FALSE;
	}

	if (FALSE == FDIIsCabinet(
			hfdi,
			hf,
			&fdici))
	{
		_close(hf);
		vpsnprintf(errorMessage, sizeof (errorMessage), "UnCabRuntimeDlls IsCabinet() failed: '%s' is not a cabinet\n",
			cabinet_fullpath);
		MSILogMessage(hInstall, errorMessage);

		(void) FDIDestroy(hfdi);
		return FALSE;
	}
	else
	{
		_close(hf);
	}

	if (TRUE != FDICopy(
		hfdi,
		szCabinetFileName,
		szCabinetPathName,
		0,
		notification_function,
		NULL,
		NULL))
	{
		vpsnprintf(errorMessage, sizeof (errorMessage), "UnCabRuntimeDlls Copy() failed: code %d [%s]\n",
			erf.erfOper, return_fdi_error_string(erf.erfOper));
		MSILogMessage(hInstall, errorMessage);

		(void) FDIDestroy(hfdi);
		return FALSE;
	}

	if (FDIDestroy(hfdi) != TRUE)
	{
		vpsnprintf(errorMessage, sizeof (errorMessage), "UnCabRuntimeDlls Destroy() failed: code %d [%s]\n",
			erf.erfOper, return_fdi_error_string(erf.erfOper));
		MSILogMessage(hInstall, errorMessage);

		return FALSE;
	}

	return TRUE;
}

char   *return_fdi_error_string(int err)
{
	switch (err)
	{
		case FDIERROR_NONE:
			return "No error";

		case FDIERROR_CABINET_NOT_FOUND:
			return "Cabinet not found";
			
		case FDIERROR_NOT_A_CABINET:
			return "Not a cabinet";
			
		case FDIERROR_UNKNOWN_CABINET_VERSION:
			return "Unknown cabinet version";
			
		case FDIERROR_CORRUPT_CABINET:
			return "Corrupt cabinet";
			
		case FDIERROR_ALLOC_FAIL:
			return "Memory allocation failed";
			
		case FDIERROR_BAD_COMPR_TYPE:
			return "Unknown compression type";
			
		case FDIERROR_MDI_FAIL:
			return "Failure decompressing data";
			
		case FDIERROR_TARGET_FILE:
			return "Failure writing to target file";
			
		case FDIERROR_RESERVE_MISMATCH:
			return "Cabinets in set have different RESERVE sizes";
			
		case FDIERROR_WRONG_CABINET:
			return "Cabinet returned on fdintNEXT_CABINET is incorrect";
			
		case FDIERROR_USER_ABORT:
			return "User aborted";
			
		default:
			return "Unknown error";
	}
}

