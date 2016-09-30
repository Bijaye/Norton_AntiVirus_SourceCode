// builtins.cpp : implementation file
//

#include "stdafx.h"

#include "ScanTask.h"
#include "Builtins.h"

#include "NAVTasks.h"
#include "NAVTasks_i.c"
#include "winioctl.h"
#include <vector>
#include "osinfo.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Need to declare this stuff, since we don't want to pull in Win9x DDK.

#ifndef VWIN32_DIOC_DOS_IOCTL
#define VWIN32_DIOC_DOS_IOCTL 1

typedef struct _DIOC_REGISTERS {
    DWORD reg_EBX;
    DWORD reg_EDX;
    DWORD reg_ECX;
    DWORD reg_EAX;
    DWORD reg_EDI;
    DWORD reg_ESI;
    DWORD reg_Flags;
}DIOC_REGISTERS, *PDIOC_REGISTERS;

#endif

// Intel x86 processor status fla
#define CARRY_FLAG  0x1

#pragma pack(1)
typedef struct _DOSDPB {
    BYTE    specialFunc;    // 
    BYTE    devType;        // 
    WORD    devAttr;        // 
    WORD    cCyl;           // number of cylinders
    BYTE    mediaType;      // 
    WORD    cbSec;          // Bytes per sector
    BYTE    secPerClus;     // Sectors per cluster
    WORD    cSecRes;        // Reserved sectors
    BYTE    cFAT;           // FATs
    WORD    cDir;           // Root Directory Entries
    WORD    cSec;           // Total number of sectors in image
    BYTE    bMedia;         // Media descriptor
    WORD    secPerFAT;      // Sectors per FAT
    WORD    secPerTrack;    // Sectors per track
    WORD    cHead;          // Heads
    DWORD   cSecHidden;     // Hidden sectors
    DWORD   cTotalSectors;  // Total sectors, if cbSec is zero
    BYTE    reserved[6];    // 
} DOSDPB, *PDOSDPB;
#pragma pack()



/////////////////////////////////////////////////////////////////////////////
// Function descriptions for this file:

// CScanTask Computer functions
bool Computer_Load(IScanTask* pThis)
{
	DWORD       dwDrives = ::GetLogicalDrives();

	// Find all drives on the system, putting the root path of each one in
	// the aszDrivesToScan array.

	for (TCHAR cDrive[] = "A:\\"; cDrive[0] <= 'Z'; cDrive[0]++, dwDrives >>= 1 )
	{
		if ( dwDrives & 1 )
		{
			UINT uType = ::GetDriveType(cDrive);

			// Skip any drives whose type can't be determined, or any
			// disconnected network drives.

			if (DRIVE_UNKNOWN != uType  &&  DRIVE_NO_ROOT_DIR != uType)
			{
				pThis->AddItem(typeDrive, subtypeDriveHD, cDrive[0], NULL, 0);
			}
		}
	}

	return pThis->GetDriveCount() > 0;
}

/////////////////////////////////////////////////////////////////////////////
// CScanTask HardDisks functions
bool HardDisks_Load(IScanTask* pThis)
{
	DWORD       dwDrives = ::GetLogicalDrives();

	// Find all drives on the system, putting the root path of each one in
	// the aszDrivesToScan array.

	for (TCHAR cDrive[] = "A:\\"; cDrive[0] <= 'Z'; cDrive[0]++, dwDrives >>= 1 )
	{
		if ( dwDrives & 1 )
		{
			// Skip any drives whose type can't be determined, or any
			// disconnected network drives.

			if (DRIVE_FIXED == ::GetDriveType(cDrive))
			{
				pThis->AddItem(typeDrive, subtypeDriveHD, cDrive[0], NULL, 0);
			}
		}
	}

	return pThis->GetDriveCount() > 0;
}

/////////////////////////////////////////////////////////////////////////////
// CScanTask RemovableDrives functions
bool RemovableDrives_Load(IScanTask* pThis)
{
	DWORD       dwDrives = ::GetLogicalDrives();

	// Find all drives on the system, putting the root path of each one in
	// the aszDrivesToScan array.

	for (TCHAR cDrive[] = "A:\\"; cDrive[0] <= 'Z'; cDrive[0]++, dwDrives >>= 1 )
	{
		if ( dwDrives & 1 )
		{
			UINT uType = ::GetDriveType(cDrive);

			// Skip any drives whose type can't be determined, or any
			// disconnected network drives.

			if (DRIVE_REMOVABLE == uType || DRIVE_CDROM == uType)
			{
				pThis->AddItem(typeDrive, subtypeDriveHD, cDrive[0], NULL, 0);
			}
		}
	}

	return pThis->GetDriveCount() > 0;
}

/////////////////////////////////////////////////////////////////////////////
// CScanTask Floppy functions
bool Floppy_Load(IScanTask* pThis)
{
	COSInfo osi;
    bool bRet = false;
    TCHAR  chDrive, chRet = 0;
    DWORD fdwLogicalDrives = GetLogicalDrives();
    DWORD  dwDriveBit;

    if( osi.IsWinNT() )
    {
        TCHAR  szDrive[] = _T("\\\\.\\x:");
        std::vector<DISK_GEOMETRY> vGeometry(20);
	    HANDLE hDrive;
	    DWORD dwBytesReturned;
	    DWORD dwValidCount;
    
        for ( chDrive = _T('A'), dwDriveBit = 1;
		      chDrive <= _T('Z') && chRet == 0; chDrive++, dwDriveBit <<= 1 )
        {
            // If the bit is set, query the drive.
            if ( 0 != ( fdwLogicalDrives & dwDriveBit ) )
            {
			    // Open the drive.
			    szDrive[4] = chDrive;
			    hDrive = CreateFile( szDrive, 0, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL ); 
			    if( hDrive == INVALID_HANDLE_VALUE )
				    continue;

			    // Get drive geometry from drive.
			    if( DeviceIoControl( hDrive, IOCTL_DISK_GET_MEDIA_TYPES, NULL,
						     0, &vGeometry[0], sizeof(DISK_GEOMETRY) * 20, &dwBytesReturned, NULL ) )
			    {
				    // Compute number of valid DISK_GEOMETRY structures returned.
				    dwValidCount = dwBytesReturned / sizeof(DISK_GEOMETRY);

				    // Look for floppy geometry type.
				    for( DWORD i = 0; i < dwValidCount; ++i )
				    {
					    switch ( vGeometry[i].MediaType ) 
					    {
					    case F5_1Pt2_512:	// 5.25, 1.2MB,  512 bytes/sector
					    case F3_1Pt44_512:	// 3.5,  1.44MB, 512 bytes/sector
					    case F3_2Pt88_512:	// 3.5,  2.88MB, 512 bytes/sector
					    case F3_20Pt8_512:	// 3.5,  20.8MB, 512 bytes/sector
					    case F3_720_512:	// 3.5,  720KB,  512 bytes/sector
					    case F5_360_512:	// 5.25, 360KB,  512 bytes/sector
					    case F5_320_512:	// 5.25, 320KB,  512 bytes/sector
					    case F5_320_1024:	// 5.25, 320KB,  1024 bytes/sector
					    case F5_180_512:	// 5.25, 180KB,  512 bytes/sector
					    case F5_160_512:	// 5.25, 160KB,  512 bytes/sector
	                        {
		                        pThis->AddItem(typeDrive, subtypeDriveRemovable, chDrive, NULL, 0);
                                i = dwValidCount;
                                bRet = true;
	                        }
                            break;
					    }
				    }
			    }

			    // Clean up.
			    CloseHandle( hDrive );
            }  
        }
    }
    else
    {
        /*
        On Windows 95, use the technique described in
        the Knowledge Base article Q125712 and in MSDN under
        "Windows 95 Guide to Programming", "Using Windows 95
        features", "Using VWIN32 to Carry Out MS-DOS Functions".
        */ 
        HANDLE hVWin32 = CreateFileA("\\\\.\\VWIN32", 0, 0, 0, 0,
            FILE_FLAG_DELETE_ON_CLOSE, 0);
        
        if (hVWin32 != INVALID_HANDLE_VALUE)
        {
            DWORD          cb;
            DIOC_REGISTERS reg;
            DOSDPB         dpb;
           
            // Peek at each drive in the system.    
            for ( chDrive = _T('A'), dwDriveBit = 1;
		          chDrive <= _T('Z') && chRet == 0; chDrive++, dwDriveBit <<= 1 )
            {
                // If the bit is set, query the drive.
                if ( 0 == ( fdwLogicalDrives & dwDriveBit ) )
                    continue;

                dpb.specialFunc = 0;            // return default type; do not hit disk
                reg.reg_EBX   = chDrive - 'A' + 1; // BL = drive number (1-based)
                reg.reg_EDX   = (DWORD)&dpb;    // DS:EDX -> DPB
                reg.reg_ECX   = 0x0860;         // CX = Get DPB
                reg.reg_EAX   = 0x440D;         // AX = Ioctl
                reg.reg_Flags = CARRY_FLAG;     // assume failure
                
                // Make sure both DeviceIoControl and Int 21h succeeded.
                if (DeviceIoControl (hVWin32, VWIN32_DIOC_DOS_IOCTL, &reg,
                    sizeof(reg), &reg, sizeof(reg),
                    &cb, 0)
                    && !(reg.reg_Flags & CARRY_FLAG))
                {
                    switch (dpb.devType)
                    {
                    case 0: // 5.25 360K floppy
                    case 1: // 5.25 1.2MB floppy
                    case 2: // 3.5  720K floppy
                    case 7: // 3.5  1.44MB floppy
                    case 9: // 3.5  2.88MB floppy
                    case 3: // 8" low-density floppy
                    case 4: // 8" high-density floppy
	                    {
		                    pThis->AddItem(typeDrive, subtypeDriveRemovable, chDrive, NULL, 0);
                            bRet = true;
	                    }
                        break;
                    }
                }
            }
            
            CloseHandle(hVWin32);
        }
    }
    
	return bRet;
}


/////////////////////////////////////////////////////////////////////////////
// CScanTask Drives functions
bool Drives_Load(IScanTask* pThis)
{
    bool bReturn = false;

    if ( FAILED ( CoInitialize (NULL)))
        return false;

	{
		CComVariant vScanTask;
		CComPtr<INAVTasksDlgs> spNAVTasksDlgs;

		vScanTask.vt = VT_BYREF;
		vScanTask.byref = pThis;

		if ( SUCCEEDED(spNAVTasksDlgs.CoCreateInstance(CLSID_NAVTasksDlgs, NULL, CLSCTX_INPROC_SERVER)) &&
			 S_OK == spNAVTasksDlgs->ShowDriveListDlg(vScanTask) &&
			 pThis->GetDriveCount() > 0 )
		{
			bReturn = true;
		}
	}

    CoUninitialize ();
    return bReturn;
}

/////////////////////////////////////////////////////////////////////////////
// CScanTask Folders functions
bool Folders_Load(IScanTask* pThis)
{
    bool bReturn = false;

    if ( FAILED ( CoInitialize (NULL)))
        return false;
	{
		CComVariant vScanTask;
		CComPtr<INAVTasksDlgs> spNAVTasksDlgs;

		vScanTask.vt = VT_BYREF;
		vScanTask.byref = pThis;

		if ( SUCCEEDED ( spNAVTasksDlgs.CoCreateInstance(CLSID_NAVTasksDlgs, NULL, CLSCTX_INPROC_SERVER)) &&
			 S_OK == spNAVTasksDlgs->ShowFolderListDlg(::GetDesktopWindow(), vScanTask, ESFLDO_Scan) && 
			 pThis->GetFolderCount() > 0 )
		{
			bReturn = true;
		}
	}

    CoUninitialize ();
    return bReturn;
}

/////////////////////////////////////////////////////////////////////////////
// CScanTask Files functions
bool Files_Load(IScanTask* pThis)
{
	bool bReturn = false;

    if ( FAILED ( CoInitialize (NULL)))
        return false;
    {
		CComVariant vScanTask;
		CComPtr<INAVTasksDlgs> spNAVTasksDlgs;

		vScanTask.vt = VT_BYREF;
		vScanTask.byref = pThis;

		if (SUCCEEDED (spNAVTasksDlgs.CoCreateInstance(CLSID_NAVTasksDlgs, NULL, CLSCTX_INPROC_SERVER)) &&
    		S_OK == spNAVTasksDlgs->ShowFileListDlg(vScanTask) &&
     		pThis->GetFileCount() > 0 )
		{
			bReturn = true;
		}
	}    
    CoUninitialize ();
    return bReturn;
}
