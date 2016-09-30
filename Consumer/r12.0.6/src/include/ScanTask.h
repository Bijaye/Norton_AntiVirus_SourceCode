
#ifndef SCANTASK_H
#define SCANTASK_H

#include "SymInterface.h"

#ifndef _UNICODE
	#include <string>
	typedef std::string tstring;
#else
	#include <wstring>
	typedef std::wstring tstring;
#endif

#include <list>

const TCHAR szTaskFileExtension [] = _T("sca");
const TCHAR szTaskSubDir [] =        _T("Tasks");

// Maximum allowable # of characters in name of task - "My Computer"
//
#define TASK_MAX_NAME_LENGTH  215 // MAX_PATH - (("Norton AntiVirus - ")x2) - '\0'

//
// Scan item type (drive, folder, or file)
//

enum SCANITEMTYPE
{
    typeNone = 0,
	typeDrive,
	typeFolder,
	typeFile,
    typeFileVID,
    typeFileVIDCompressed
};

//
// Scan item sub-type
//

enum SCANITEMSUBTYPE
{
	subtypeNone = 0,
	subtypeDriveRemovable,
	subtypeDriveHD,
	subtypeDriveCD,
	subtypeDriveNetwork,
	subtypeDriveInactive,
	subtypeFolder,
	subtypeFolderAndSubs,
	subtypeFile
};

//
// Task type
//

enum SCANTASKTYPE
{
	// unset task
	scanUnset = 0,
	scanFirst = scanUnset,
   
	// Scan all drives A - Z : NOT the "My Computer" scan
	scanComputer, 
    
    // Scan all local fixed hard drives A - Z : The "My Computer" scan - See scanMyComputer
    //
    scanHardDrives, 
    
    // All removable devices
    //
    scanRemovable, 
    
    // Floppy
    //
    scanAFloppy,
	
	// Drive(s) - User selected
    //
	scanSelectedDrives,
    
    // Folder(s) - User selected
    //
	scanFolder,
    
    // File(s) - User selected
    //
    scanFiles,

    // custom tasks
    //
	scanCustom,

    // Quick Scan
    //
    scanQuick,
	
	scanLast = scanQuick,

    // Use this for "My Computer"
    scanMyComputer = scanHardDrives
};

class CScanItem
{
public:
	// Constructor.
    CScanItem::CScanItem()
    {
	    m_Type = typeNone;
	    m_SubType = subtypeNone;
        m_ulVID = 0;
    }

public:
	// Sets the item type/subtype.
    void CScanItem::SetItemType(SCANITEMTYPE type, SCANITEMSUBTYPE subtype)
    {
	    m_Type = type;
	    m_SubType = subtype;
    }

	// Retrieves the item type/subtype.
    void CScanItem::GetItemType(SCANITEMTYPE& type, SCANITEMSUBTYPE& subtype) const
    {
	    type = m_Type;
	    subtype = m_SubType;
    }
	
	// Sets the drive letter (if the item is a drive).
    bool CScanItem::SetDrive(const TCHAR chDrive)
    {
	    if (m_Type != typeDrive)
		    return false;

	    m_chDrive = chDrive;

	    return true;
    }

	// Gets the drive letter (if the item is a drive).
    TCHAR CScanItem::GetDrive() const
    {
	    if (m_Type != typeDrive)
		    return (TCHAR) -1;

	    return m_chDrive;
    }
	
	// Sets the items path (if the item is a file or folder).
    bool CScanItem::SetPath(const TCHAR* szPath)
    {
	    if ((m_Type != typeFile) && (m_Type != typeFolder) && (m_Type != typeFileVID) && (m_Type != typeFileVIDCompressed) )
		    return false;

	    m_strPath = szPath;

	    return true;
    }


	// Returns the items path (if the item is a file or folder).
    LPCTSTR CScanItem::GetPath() const
    {
	    if ((m_Type != typeFile) && (m_Type != typeFolder) && (m_Type != typeFileVID) && (m_Type != typeFileVIDCompressed) )
		    return NULL;

	    return m_strPath.c_str();
    }

    // Sets the items VID (if the item is a filename\VID combo).
    bool CScanItem::SetVID(const unsigned long ulVID)
    {
        if ( m_Type != typeFileVID && m_Type != typeFileVIDCompressed )
            return false;

        m_ulVID = ulVID;

        return true;
    }

    // Returns the items VID (if the item is a filename\VID combo).
    unsigned long CScanItem::GetVID() const
    {
        if ( m_Type != typeFileVID && m_Type != typeFileVIDCompressed )
            return 0;

        return m_ulVID;
    }
	
	// for find()
	bool CScanItem::operator==(const CScanItem& rhs) const
    {
	    return this == &rhs
	        || (m_Type == rhs.m_Type && (typeDrive == m_Type && m_chDrive == rhs.m_chDrive
	                                || typeDrive != m_Type && m_strPath == rhs.m_strPath));
    }

protected:
	// Type of item.
    SCANITEMTYPE m_Type;
	
	// Sub-type of item.
    SCANITEMSUBTYPE m_SubType;

	// Uppercase drive letter (if the item is a drive).
    TCHAR m_chDrive;

	// Item path (if the item is a file or folder).
    tstring m_strPath;

    // VID if its a filename\VID pair
    unsigned long m_ulVID;
};

class IScanTask : public ISymBase
{
public:
    // Add a new item to the task.
	virtual bool AddItem(SCANITEMTYPE type, SCANITEMSUBTYPE subtype, TCHAR chDrive, LPCTSTR pcszPath, unsigned long ulVID) = 0;

	// Enumerate through the task items.
	virtual void EnumReset() = 0;

    // This will return the data for the next item in the list
	virtual bool EnumNext(/*out*/SCANITEMTYPE &type, /*out*/SCANITEMSUBTYPE &subtype, /*out*/TCHAR &chDrive, /*out*/LPTSTR pszPath, /*in*/DWORD dwPathSize, /*out*/ unsigned long &ulVID) = 0;

	// Gets/sets the task type.
	virtual SCANTASKTYPE GetType() = 0;
	virtual void SetType(SCANTASKTYPE type) = 0;

	// Returns the number of items in the task.
	virtual UINT GetItemCount()  = 0;

	// Returns the number of drive items in the task.
	virtual UINT GetDriveCount()  = 0;

	// Returns the number of folder items in the task.
	virtual UINT GetFolderCount()  = 0;

	// Returns the number of file items in the task.
	virtual UINT GetFileCount()  = 0;

    // Returns the number of filename\VID items in the task.
    virtual UINT GetFileVIDCount() = 0;

	// Gets/sets the last run time.
    virtual void GetLastRunTime(time_t& time)  = 0;
    virtual void SetLastRunTime(const time_t& time) = 0;

	// Are there any items in the task?
	virtual bool IsEmpty()  = 0;

	// Delete all items.
	virtual void Empty() = 0;

	// Persist the task.
    //
	virtual bool Save(void) = 0;

	// Load the task from the specified file.
	virtual bool Load(LPCTSTR szFileName, bool bItems = true) = 0;

    // Set/get the name of the task - e.g. "My Task"
	virtual bool SetName ( LPTSTR lpszName ) = 0;
    virtual LPCTSTR GetName() = 0;

    // Set the name of the path - e.g. "c:\documents and settings\...\0.scn"
    // Should only be used for temporary tasks. See Save(NULL) above.
    //
	virtual bool SetPath ( LPCTSTR lpszPath ) = 0;
    virtual LPCTSTR GetPath() = 0;

    // Set/get for CanSchedule    
    virtual bool GetCanSchedule () = 0;
    virtual bool SetCanSchedule ( bool bCanSchedule ) = 0;
    
    // Set/get for CanEdit
    virtual bool GetCanEdit () = 0;
    virtual bool SetCanEdit ( bool bCanEdit ) = 0;
};
SYM_DEFINE_INTERFACE_ID(IID_IScanTask,
0x19ef76f8, 0xf733, 0x4a22, 0xb3, 0xcd, 0xcb, 0x40, 0x6b, 0x45, 0x49, 0x9);

typedef CSymPtr<IScanTask> IScanTaskPtr;

class CScanTask : public ISymBaseImpl<CSymThreadSafeRefCount>,
                  public IScanTask
{
public:

    // IScanTask Interface
	SYM_INTERFACE_MAP_BEGIN()                
		SYM_INTERFACE_ENTRY( IID_IScanTask, IScanTask )
	SYM_INTERFACE_MAP_END()

    // Constructor.
	CScanTask();

    ~CScanTask();

	// Add a new item to the task.
	bool AddItem(SCANITEMTYPE type, SCANITEMSUBTYPE subtype, TCHAR chDrive, LPCTSTR pcszPath, unsigned long ulVID);

	// Enumerate through the task items.
	void EnumReset();

    // This will return the data for the next item in the list
	bool EnumNext(/*out*/SCANITEMTYPE &type, /*out*/SCANITEMSUBTYPE &subtype, /*out*/TCHAR &chDrive, /*out*/LPTSTR pszPath, /*in*/DWORD dwPathSize, /*out*/unsigned long &ulVID);

	// Gets/sets the task type.
	SCANTASKTYPE GetType();
	void SetType(SCANTASKTYPE type);

	// Returns the number of items in the task.
	UINT GetItemCount();

	// Returns the number of drive items in the task.
	UINT GetDriveCount();

	// Returns the number of folder items in the task.
	UINT GetFolderCount();

	// Returns the number of file items in the task.
	UINT GetFileCount();

    // Returns the number of file\VID items in the task.
    UINT GetFileVIDCount();

	// Gets/sets the last run time.
    void GetLastRunTime(time_t& time);
    void SetLastRunTime(const time_t& time);

	// Are there any items in the task?
	bool IsEmpty();

	// Delete all items.
	void Empty();

	// Persist the task.
    //
	bool Save(void);

	// Load the task from the specified file.
	bool Load(LPCTSTR szFileName, bool bItems = true);

    // Set/get the name of the task - e.g. "My Task"
	bool SetName ( LPTSTR lpszName );
    LPCTSTR GetName();

    // Set the name of the path - e.g. "c:\documents and settings\...\0.scn"
    // Should only be used for temporary tasks. See Save(NULL) above.
    //
	bool SetPath ( LPCTSTR lpszPath );
    LPCTSTR GetPath();

    // Set/get for CanSchedule    
    bool GetCanSchedule ();
    bool SetCanSchedule ( bool bCanSchedule );
    
    // Set/get for CanEdit
    bool GetCanEdit ();
    bool SetCanEdit ( bool bCanEdit );

protected:
	// List of scan items.
	std::list<CScanItem> m_Items;

	// Iterator used to traverse the scan items list.
	std::list<CScanItem>::iterator m_Iterator;

	// Task type.
	SCANTASKTYPE m_Type;

	// Time the task was created/last modified/last run.
	time_t m_timeLastRun;
	
	// Number of drives, folders, files, and file\VID items.
	DWORD m_uDriveCnt;
	DWORD m_uFolderCnt;
	DWORD m_uFileCnt;
    DWORD m_uFileVIDCnt;

	// Task file version.
    DWORD m_wVersionMajor;
    DWORD m_wVersionMinor;
    DWORD m_wVersionInline;

    // Name of Task - e.g. "My Computer" - translated!
    tstring m_strName;

    // Full path to the task file
    tstring m_strPath;

    // Is the user allowed to edit this?
    bool m_bCanEdit;

    // Is the user allowed to schedule this?
    bool m_bCanSchedule;

protected:
    // Helper function used to reset the task object.
	void Reset();

	// Converts date/time from a SYSTEMTIME structure to text.
	void PackSystemTime(const SYSTEMTIME* pSysTime, std::string& strTime) const;

	// Converts date/time from text to a SYSTEMTIME structure.
	bool UnpackSystemTime(LPCTSTR szPackedTime, SYSTEMTIME* pSysTime) const;

};

#endif

