// ScanTask.cpp

#include "Stdafx.h"

#include "ScanTask.h"
#include "Builtins.h"
#include "navopt32.h"
#include <algorithm>
#include "NAVInfo.h"
#include "GlobalEvent.h"
#include "GlobalEvents.h"
#include "NAVTrust.h"
#include <time.h> // for time conversions

#include <stdexcept>
using std::runtime_error;

static CNAVInfo g_NAVInfo;


class CHNAVOPTS32  // Wrapper for the file primitive
{
	HNAVOPTS32 h;
public:
	CHNAVOPTS32(void) : h(NULL) {}
	~CHNAVOPTS32(void) { if (h) ::NavOpts32_Free(h); }
	NAVOPTS32_STATUS Allocate(void) { return NavOpts32_Allocate(&h); }
	NAVOPTS32_STATUS Load(const char* szFileName) { return NavOpts32_Load(szFileName, h, FALSE); }
	NAVOPTS32_STATUS Save(const char* szFileName) { return NavOpts32_Save(szFileName, h); }
	NAVOPTS32_STATUS GetStringValue(const char* szOption, char szBuffer[], int iSize) { return NavOpts32_GetStringValue(h, szOption, szBuffer, iSize, "*"); }
	NAVOPTS32_STATUS GetDwordValue(const char* szOption, DWORD& dwValue, DWORD dwDefault) { return NavOpts32_GetDwordValue(h, szOption, &dwValue, dwDefault); }
	NAVOPTS32_STATUS SetStringValue(const char* szOption, const char szBuffer[]) { return NavOpts32_SetStringValue(h, szOption, szBuffer); }
	NAVOPTS32_STATUS SetDwordValue(const char* szOption, DWORD dwValue) { return NavOpts32_SetDwordValue(h, szOption, dwValue); }
};

// Options names
//
const TCHAR szOptVersionMajor[] =   _T("Version:Major");
const TCHAR szOptVersionMinor[] =   _T("Version:Minor");
const TCHAR szOptVersionInLine[] =  _T("Version:InLine");
const TCHAR szOptName[] =           _T("Name");
const TCHAR szOptType[] =           _T("Type");
const TCHAR szOptLastRun[] =        _T("LastRun");
const TCHAR szOptDriveCount[] =     _T("DriveCount");
const TCHAR szOptFolderCount[] =    _T("FolderCount");
const TCHAR szOptFileCount[] =      _T("FileCount");
const TCHAR szOptFileVIDCount[] =      _T("FileVIDCount");
const TCHAR szOptCanEdit[] =        _T("CanEdit");
const TCHAR szOptCanSchedule[] =    _T("CanSchedule");


CScanTask::CScanTask()
{
    Reset();
}

CScanTask::~CScanTask()
{
    Empty();
}

void CScanTask::Reset()
{
    Empty();

    m_Type = scanUnset;
    m_strName = "";
    m_strPath = "";
    m_bCanEdit = false;
    m_bCanSchedule = false;
    m_uDriveCnt = 0;
    m_uFileCnt = 0;
    m_uFolderCnt = 0;
    m_uFileVIDCnt = 0;
    m_wVersionMajor = 8;
    m_wVersionMinor = 0;
    m_wVersionInline = 0;
    m_timeLastRun = 0;

    EnumReset ();
}

bool CScanTask::AddItem(SCANITEMTYPE type, SCANITEMSUBTYPE subtype, TCHAR chDrive, LPCTSTR pcszPath, unsigned long ulVID)
{
	try
	{
        CScanItem item;

        item.SetItemType(type, subtype);

        if( type == typeDrive )
            item.SetDrive(chDrive);
        else if( pcszPath )
            item.SetPath(pcszPath);

        if( type == typeFileVID || type == typeFileVIDCompressed )
            item.SetVID(ulVID);

		m_Items.push_back(item);

		switch(type)
		{
		case typeDrive:
			m_uDriveCnt++;
			break;
		case typeFolder:
			m_uFolderCnt++;
			break;
		case typeFile:
			m_uFileCnt++;
			break;
        case typeFileVID:
        case typeFileVIDCompressed:
            m_uFileVIDCnt++;
            break;
		}
	}
	catch(...)
	{
		return false;
	}

	return true;
}

// Always call EnumReset () BEFORE using EnumNext () for the first time!!
//
void CScanTask::EnumReset()
{
	m_Iterator = m_Items.begin();
}

bool CScanTask::EnumNext(SCANITEMTYPE &type, SCANITEMSUBTYPE &subtype, TCHAR &chDrive, LPTSTR pszPath, DWORD dwPathSize, unsigned long &ulVID)
{
	// If the list is at the end, or empty, just return.
    //
    if (m_Iterator == m_Items.end() || m_Items.empty())
		return false;

    m_Iterator->GetItemType(type, subtype);

    if( type == typeDrive )
        chDrive = m_Iterator->GetDrive();
    else if( pszPath )
    {
        _tcsncpy(pszPath, m_Iterator->GetPath(), dwPathSize);
    }

    // If this is a filename\VID item also give the VID back
    if( type == typeFileVID || type == typeFileVIDCompressed )
        ulVID = m_Iterator->GetVID();

	m_Iterator++;

	return true;
}

SCANTASKTYPE CScanTask::GetType()
{
	return m_Type;
}

void CScanTask::SetType(SCANTASKTYPE type)
{
	m_Type = type;
}

UINT CScanTask::GetItemCount()
{
    return m_uDriveCnt + m_uFolderCnt + m_uFileCnt + m_uFileVIDCnt;
}

UINT CScanTask::GetDriveCount()
{
    return m_uDriveCnt;
}

UINT CScanTask::GetFolderCount()
{
    return m_uFolderCnt;
}

UINT CScanTask::GetFileCount()
{
    return m_uFileCnt;
}

UINT CScanTask::GetFileVIDCount()
{
    return m_uFileVIDCnt;
}

    
void CScanTask::GetLastRunTime(time_t& time)
{
	CopyMemory(&time, &m_timeLastRun, sizeof(time_t));
}

void CScanTask::SetLastRunTime(const time_t& time)
{
	CopyMemory(&m_timeLastRun, &time, sizeof(time_t));
}

bool CScanTask::IsEmpty()
{
    return (m_Items.empty());
}

void CScanTask::Empty()
{
	m_Items.clear();

    m_uDriveCnt = m_uFolderCnt = m_uFileCnt = m_uFileVIDCnt = 0;
}

bool CScanTask::Save(void)
{
    if ( m_strPath.empty () || scanUnset == m_Type )
            return false;
    
    CHNAVOPTS32 NavOpts;
	SCANITEMTYPE Type;
	SCANITEMSUBTYPE SubType;

	try
	{
		if (NAVOPTS32_OK != NavOpts.Allocate())
			throw runtime_error(_T("Unable to allocate."));

        // Write the task name
		if (NAVOPTS32_OK != NavOpts.SetStringValue("Name", m_strName.c_str ()))
			throw runtime_error(_T("Unable to write name."));


		// Write the version number.
		if (NAVOPTS32_OK != NavOpts.SetDwordValue(szOptVersionMajor, m_wVersionMajor)
		 || NAVOPTS32_OK != NavOpts.SetDwordValue(szOptVersionMinor, m_wVersionMinor)
		 || NAVOPTS32_OK != NavOpts.SetDwordValue(szOptVersionInLine, m_wVersionInline))
			throw runtime_error(_T("Unable to write version."));

		// Write the type.
		if (NAVOPTS32_OK != NavOpts.SetDwordValue(szOptType, m_Type))
			throw runtime_error(_T("Unable to write version."));

		// Write the time the task was last run.
		if (NAVOPTS32_OK != NavOpts.SetDwordValue(szOptLastRun, m_timeLastRun))
			throw runtime_error(_T("Unable to write last run."));

        // Write the bCanEdit
        //
        if (NAVOPTS32_OK != NavOpts.SetDwordValue(szOptCanEdit, m_bCanEdit))
			throw runtime_error(_T("Unable to write CanEdit."));

        // Write the bCanSchedule
        //
        if (NAVOPTS32_OK != NavOpts.SetDwordValue(szOptCanSchedule, m_bCanSchedule))
			throw runtime_error(_T("Unable to write CanSchedule."));

		// If this is a built-in task, we're done.
		if (m_Type != scanCustom)
		{
			if (NAVOPTS32_OK != NavOpts.Save(m_strPath.c_str()))
				throw runtime_error(_T("Unable to save."));


            if ( m_Type == scanMyComputer )
            {
                // If this is the My Computer scan we need to announce that
                // it is being changed for status.
                //
                CGlobalEvent eventMyComputer;

                if ( eventMyComputer.Create ( SYM_REFRESH_FULLSYSTEMSCAN_EVENT ))
                    PulseEvent ( eventMyComputer );

                HKEY hKey;
                DWORD dwType = 0;

                if ( ERROR_SUCCESS == RegCreateKeyEx ( HKEY_LOCAL_MACHINE,
                                                       _T("SOFTWARE\\Symantec\\Norton AntiVirus\\LastScan"),
                                                       0,
                                                       NULL,
                                                       0,
                                                       KEY_QUERY_VALUE | KEY_WRITE,
                                                       NULL, &hKey, &dwType))
                {
                    // Convert from time_t to SYSTEMTIME
                    //
                    SYSTEMTIME stLastScan;

                    ZeroMemory ( &stLastScan, sizeof (SYSTEMTIME ));
                
                    time_t timeCreationTime = m_timeLastRun;

                    struct tm* pTime;

                    pTime = localtime(&timeCreationTime);

                    timeCreationTime = mktime ( pTime );

                    stLastScan.wYear = pTime->tm_year+1900;
                    stLastScan.wMonth = pTime->tm_mon+1;
                    stLastScan.wDay = pTime->tm_mday;
                    stLastScan.wHour = pTime->tm_hour;
                    stLastScan.wMinute = pTime->tm_min;
                    stLastScan.wSecond = pTime->tm_sec;

                
                    DWORD dwBufSize = sizeof ( stLastScan );

                    RegSetValueEx ( hKey,
                                    _T("SystemTime"),
                                    NULL,
                                    REG_BINARY,
                                    (LPBYTE) &stLastScan, 
                                    dwBufSize );
    
                    RegCloseKey ( hKey );
                }
            }

			return true;
		}

		// Write the drives to scan.
		if (NAVOPTS32_OK != NavOpts.SetDwordValue(szOptDriveCount, m_uDriveCnt))
			throw runtime_error(_T("Unable to write version."));

		unsigned int uItemNumber = 1;
		std::list<CScanItem>::const_iterator It;

		char szEntry[64], szBuffer[MAX_PATH*2];

		if (m_uDriveCnt)
		{
			for(It = m_Items.begin(); It != m_Items.end(); It++)
			{
				(*It).GetItemType(Type, SubType);

				if (Type == typeDrive)
				{
					::sprintf(szEntry, "Drive%lu", uItemNumber++);
					::sprintf(szBuffer, "%c", (*It).GetDrive());

					if (NAVOPTS32_OK != NavOpts.SetStringValue(szEntry, szBuffer))
						throw runtime_error(_T("Unable to read drive path."));
				}
			}
		}

		// Write the folders to scan.
		if (NAVOPTS32_OK != NavOpts.SetDwordValue(szOptFolderCount, m_uFolderCnt))
			throw runtime_error(_T("Unable to write folder count."));

		if (m_uFolderCnt)
		{
			uItemNumber = 1;

			for(It = m_Items.begin(); It != m_Items.end(); It++)
			{
				(*It).GetItemType(Type, SubType);

				if (Type == typeFolder)
				{
					::sprintf(szEntry, "Folder%lu", uItemNumber);

					if (NAVOPTS32_OK != NavOpts.SetStringValue(szEntry, (*It).GetPath()))
						throw runtime_error(_T("Unable to write folder path."));

					::sprintf(szEntry, "FolderSubs%lu", uItemNumber++);
					if (NAVOPTS32_OK != NavOpts.SetDwordValue(szEntry, (SubType == subtypeFolderAndSubs) ? 1UL : 0UL))
						throw runtime_error(_T("Unable to write folder path."));
				}
			}
		}

		// Write the files to scan.
		if (NAVOPTS32_OK != NavOpts.SetDwordValue(szOptFileCount, m_uFileCnt))
			throw runtime_error(_T("Unable to write file count."));

		if (m_uFileCnt)
		{
			uItemNumber = 1;

			for(It = m_Items.begin(); It != m_Items.end(); It++)
			{
				(*It).GetItemType(Type, SubType);

				if (Type == typeFile)
				{
					::sprintf(szEntry, "File%lu", uItemNumber++);

					if (NAVOPTS32_OK != NavOpts.SetStringValue(szEntry, (*It).GetPath()))
						throw runtime_error(_T("Unable to write file path."));
				}
			}
		}

        // Write the file\VID items to scan.
        if (NAVOPTS32_OK != NavOpts.SetDwordValue(szOptFileVIDCount, m_uFileVIDCnt))
            throw runtime_error(_T("Unable to write file\\VID count."));

        if (m_uFileVIDCnt)
        {
            uItemNumber = 1;

            for(It = m_Items.begin(); It != m_Items.end(); It++)
            {
                (*It).GetItemType(Type, SubType);

                if (Type == typeFileVID || Type == typeFileVIDCompressed )
                {
                    // If this is a compressed file-VID item write out the compressed flag
                    if( Type == typeFileVIDCompressed )
                    {
                        ::sprintf(szEntry, "FileVIDCompressed%lu", uItemNumber);
                        if (NAVOPTS32_OK != NavOpts.SetDwordValue(szEntry, 1))
                            CCTRACEE(_T("Unable to write the file-vid compressed flag."));
                    }

                    // Write out the path
                    ::sprintf(szEntry, "FileVIDPath%lu", uItemNumber);

                    if (NAVOPTS32_OK != NavOpts.SetStringValue(szEntry, (*It).GetPath()))
                        throw runtime_error(_T("Unable to write file\\VID path."));

                    // Write out the VID
                    ::sprintf(szEntry, "FileVIDId%lu", uItemNumber++);

                    if (NAVOPTS32_OK != NavOpts.SetDwordValue(szEntry, (*It).GetVID()))
                        throw runtime_error(_T("Unable to write file\\VID vid."));
                }
            }
        }

		// We're done!
		if (NAVOPTS32_OK != NavOpts.Save(m_strPath.c_str()))
			throw runtime_error(_T("Unable to save."));
	}
	catch(exception& Ex)
	{
        CCTRACEE(_T("Caught Exception in CScanTask::Save(): %s"),Ex.what());
		return false;
	}

	return true;
}

bool CScanTask::Load(LPCTSTR szFileName, bool bItems/*= true*/)
{
	// Make sure the file'version number matches.

	CHNAVOPTS32 NavOpts;
	SCANTASKTYPE SType;
	DWORD dw;
	char szBuffer[MAX_PATH*2];

	try
	{
		if (NAVOPTS32_OK != NavOpts.Allocate())
			throw runtime_error(_T("Unable to allocate."));

		if (NAVOPTS32_OK != NavOpts.Load(szFileName))
			throw runtime_error(_T("Unable to load."));

        // Read the task name
        char szTempTaskName[TASK_MAX_NAME_LENGTH+1] = {0};

		if (NAVOPTS32_OK != NavOpts.GetStringValue(szOptName, szTempTaskName, sizeof(szTempTaskName)))
			throw runtime_error(_T("Unable to read name."));

		if (::strcmp(szTempTaskName, _T("*")) != 0)
			m_strName = szTempTaskName;

		// Read the version number.
		if (NAVOPTS32_OK != NavOpts.GetDwordValue(szOptVersionMajor, m_wVersionMajor, 0xFFFFFFFF)
		 || NAVOPTS32_OK != NavOpts.GetDwordValue(szOptVersionMinor, m_wVersionMinor, 0xFFFFFFFF)
		 || NAVOPTS32_OK != NavOpts.GetDwordValue(szOptVersionInLine, m_wVersionInline, 0xFFFFFFFF))
			throw runtime_error(_T("Unable to read version."));

		// See if this is a built-in task.
		if (NAVOPTS32_OK != NavOpts.GetDwordValue(szOptType, dw, scanUnset))
			throw runtime_error(_T("Unable to read type."));

		SType = SCANTASKTYPE(dw);
		if (SType == scanUnset)
			m_Type = scanCustom;
		else
			m_Type = SType;

		// Read the last run time.
		if (NAVOPTS32_OK != NavOpts.GetDwordValue(szOptLastRun, (DWORD&)m_timeLastRun, 0))
			throw runtime_error(_T("Unable to read last run."));

		// Read the CanEdit value
        //        
        if (NAVOPTS32_OK != NavOpts.GetDwordValue(szOptCanEdit, dw, 0))
			throw runtime_error(_T("Unable to read CanEdit."));

        if ( dw == 1 )
            m_bCanEdit = true;

		// Read the CanSchedule value
        //        
        if (NAVOPTS32_OK != NavOpts.GetDwordValue(szOptCanSchedule, dw, 0))
			throw runtime_error(_T("Unable to read CanSchedule."));

        if ( dw == 1 )
            m_bCanSchedule = true;
        
        // Set the internal path name
        //
        m_strPath = szFileName;

		// Only do the _Load if it's a built-in (!custom) and
        // the bItems is on. bItems = UI OK.
        //
        if ( m_Type != scanCustom && !bItems )
            return true;

		if (m_Type != scanCustom)
        {
            // This is the loading code for built-in scans.
            // This will trigger UI, so we only call it if UI
            // has been requested by the bItems flag.
            //
			switch(m_Type)
			{
			case scanCustom:
				break;
			case scanComputer:
				return Computer_Load(this);
			case scanHardDrives:
				return HardDisks_Load(this);
			case scanRemovable:
				return RemovableDrives_Load(this);
			case scanAFloppy:
				return Floppy_Load(this);
			case scanFolder:
				return Folders_Load(this);
			case scanFiles:
				return Files_Load(this);
			case scanSelectedDrives:
				return Drives_Load(this);
            case scanQuick:
                // For the quick scan clients need to just query for the type
                // and then run the quick scan (i.e: navw32)
                return true;
			default:
				return false;
			}
        }   // end built-ins
        else
        {
            // This is the loader for custom scans. Since it never triggers UI
            // we always load them.
            //

		    // Read the drives.
            DWORD dwDriveCount = 0;

		    if (NAVOPTS32_OK != NavOpts.GetDwordValue(szOptDriveCount, dwDriveCount, 0UL))
			    throw runtime_error(_T("Unable to read drive count."));

		    TCHAR szEntry[64];
		    
		    for(unsigned int uIndex = 1; uIndex <= dwDriveCount; uIndex++)
		    {
			    sprintf(szEntry, _T("Drive%lu"), uIndex);

			    if (NAVOPTS32_OK != NavOpts.GetStringValue(szEntry, szBuffer, sizeof(szBuffer)))
				    throw runtime_error(_T("Unable to read drive path."));

			    if (::strcmp(szBuffer, _T("*")) != 0)
			    {
				    AddItem(typeDrive, subtypeDriveHD, szBuffer[0], NULL, 0);
			    }
		    }

		    // Read the folders.
            DWORD dwFolderCount = 0;

		    if (NAVOPTS32_OK != NavOpts.GetDwordValue(szOptFolderCount, dwFolderCount, 0UL))
			    throw runtime_error(_T("Unable to read folder count."));

		    for(uIndex = 1; uIndex <= dwFolderCount; uIndex++)
		    {
			    sprintf(szEntry, _T("Folder%lu"), uIndex);

			    if (NAVOPTS32_OK != NavOpts.GetStringValue(szEntry, szBuffer, sizeof(szBuffer)))
				    throw runtime_error(_T("Unable to read folder path."));

			    if (::strcmp(szBuffer, _T("*")) != 0)
			    {
				    sprintf(szEntry, _T("FolderSubs%lu"), uIndex);

				    if (NAVOPTS32_OK != NavOpts.GetDwordValue(szEntry, dw, 0UL))
					    throw runtime_error(_T("Unable to read type."));

				    AddItem(typeFolder, 0UL == dw ? subtypeFolder : subtypeFolderAndSubs, NULL, szBuffer, 0);
			    }

		    }

		    // Read the files.
            DWORD dwFileCount = 0;

		    if (NAVOPTS32_OK != NavOpts.GetDwordValue(szOptFileCount, dwFileCount, 0UL))
			    throw runtime_error(_T("Unable to read file count."));

		    for(uIndex = 1; uIndex <= dwFileCount; uIndex++)
		    {
			    sprintf(szEntry, _T("File%lu"), uIndex);

			    if (NAVOPTS32_OK != NavOpts.GetStringValue(szEntry, szBuffer, sizeof(szBuffer)))
				    throw runtime_error(_T("Unable to read file path."));

			    if (::strcmp(szBuffer, _T("*")) != 0)
			    {
				    AddItem(typeFile, subtypeFile, NULL, szBuffer, 0);
			    }
		    }

            // Read the filename\VID items.
            DWORD dwFileVIDCount = 0;

            if (NAVOPTS32_OK != NavOpts.GetDwordValue(szOptFileVIDCount, dwFileVIDCount, 0UL))
                throw runtime_error(_T("Unable to read file\\VID count."));

            for(uIndex = 1; uIndex <= dwFileVIDCount; uIndex++)
            {
                // Read the path
                sprintf(szEntry, _T("FileVIDPath%lu"), uIndex);

                if (NAVOPTS32_OK != NavOpts.GetStringValue(szEntry, szBuffer, sizeof(szBuffer)))
                    throw runtime_error(_T("Unable to read file\\VID file path."));

                // Read the VID
                DWORD dwVID = 0;
                sprintf(szEntry, _T("FileVIDId%lu"), uIndex);

                if (NAVOPTS32_OK != NavOpts.GetDwordValue(szEntry, dwVID, 0UL))
                    throw runtime_error(_T("Unable to read file\\VID vid."));

                // See if this is a compressed file VID item
                SCANITEMTYPE itemType = typeFileVID;
                DWORD dwCompressed = 0;
                sprintf(szEntry, _T("FileVIDCompressed%lu"), uIndex);
                NavOpts.GetDwordValue(szEntry, dwCompressed, 0UL);
                if( dwCompressed == 1 )
                {
                    itemType = typeFileVIDCompressed;
                }

                if (::strcmp(szBuffer, _T("*")) != 0)
                {
                    AddItem(itemType, subtypeFile, NULL, szBuffer, dwVID);
                }
            }
        }   // end custom files

    }
	catch(...)
	{
		return false;
	}

	return true;
}

LPCTSTR CScanTask::GetName()
{
    return m_strName.c_str ();
}

bool CScanTask::SetName(LPTSTR lpszName)
{
    if ( lpszName )
    {
        m_strName = lpszName;
        return true;
    }

    return false;
}

bool CScanTask::SetPath ( LPCTSTR lpszPath )
{
    if ( lpszPath == NULL )
        return false;

    m_strPath = lpszPath;
    return true;
}

LPCTSTR CScanTask::GetPath()
{
    return m_strPath.c_str ();
}

bool CScanTask::GetCanSchedule ()
{
    return m_bCanSchedule;
}

bool CScanTask::SetCanSchedule ( bool bCanSchedule )
{
    m_bCanSchedule = bCanSchedule;
    return true;
}
    
bool CScanTask::GetCanEdit ()
{
    return m_bCanEdit;
}

bool CScanTask::SetCanEdit ( bool bCanEdit )
{
    m_bCanEdit = bCanEdit;
    return true;
}