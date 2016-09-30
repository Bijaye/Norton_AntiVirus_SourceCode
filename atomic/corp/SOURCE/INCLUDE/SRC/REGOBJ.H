/* Copyright 1997 Symantec, Peter Norton Computing Group                */
/////////////////////////////////////////////////////////////////////////
// RegObj.h
//		Contains class definition for CNavRegComm Object.  This object
// is use by the NAV'97 UI to communicate with the NAVW.EXE module.
//
// $Header:   S:/INCLUDE/VCS/REGOBJ.h_v   1.1   28 Mar 1997 17:34:16   DBUCHES  $
//
/////////////////////////////////////////////////////////////////////////
// Created 3/21/97 - DBUCHES
/////////////////////////////////////////////////////////////////////////
/* $Log:   S:/INCLUDE/VCS/REGOBJ.h_v  $ */
// 
//    Rev 1.1   28 Mar 1997 17:34:16   DBUCHES
// Added m_hWnd Member
//
//    Rev 1.0   25 Mar 1997 13:17:50   DBUCHES
// Initial revision.

#ifndef _NAVREGOBJ_H_
#define _NAVREGOBJ_H_

// Scan Object.  This structure contains a null terminated string
// containing the a full pathname.
typedef struct tagSCANOBJECT {
	// Should we scan sub-directories?
	BOOL bScanSubDirs;
	// Full path name (can contain wildcards.)
	TCHAR *szPathName;
} SCANOBJECT, *LPSCANOBJECT;

/*////////////////////////////////////////////////////////////////////////
    CNavRegComm Object - This object is used by NAV32 to communicate
    task data to NAVW.EXE.

    Usage: NAV32 will set the current action to perform (enum) and then
    call the Save() method.  For on-demand scans, InitForSave() must be
    called, specifying paths to scan.

    NAVW simply needs to call the Load() method, which will retrieve the
    data from the registry.  Note that the Load() call removes the data
    from the registry.  Therefore, Load() may only be called once!
    Subsequent calls to Load() will fail.

*/////////////////////////////////////////////////////////////////////////

class CNavRegComm
{
public:
    // Constructors/Destructors.
    CNavRegComm();
    ~CNavRegComm();

    // Serialization.
    BOOL Save();
    BOOL Load();

	// Initialization
	BOOL InitForSave(int iCount, LPTSTR *pszPaths, BOOL *pbScanSubDirs);

    // Data accessors.
    int GetAction();
    int SetAction(int iAction);
	int GetScanType();
	int SetScanType(int iType);
	int GetScanResponse();
	int SetScanResponse(int iResponse);

	// Sets the registry key we're saveing/loading from.
	void SetRegKey(LPTSTR szNewKey);

    // Return total number of drives/directories to scan.
    int GetNumScanObjects();

    // Returns pointer to first scan object.  Can be used in
    // call to GetNextScanObject.
    LPSCANOBJECT GetFirstScanObject();

    // Returns pointer to next object or NULL if no more objects.
    LPSCANOBJECT GetNextScanObject(LPSCANOBJECT pCurrent);

public:
    // Action values.  Add new actions for NAVW to perform here.
    enum ACTION_ENUMS{
        ACTION_NO_ACTION = 0,
		ACTION_SCAN,
        ACTION_VIRUS_LIST,
        ACTION_ACTIVITY_LOG,
		ACTION_LAST_ACTION,
    };

	// Scan types
	enum SCAN_TYPE {
			ALL_FILES = 0,
			PROGRAM_FILES,
			END_SCAN_TYPE,
	};

	// Scan responses
	enum SCAN_RESPONSE {
			PROMPT = 0,
			AUTO_REPAIR,
			LOG,
			DELETE_FILE,
			END_RESPONSE_TYPE,
	};

	// Scan flags
	BOOL m_bScanMemory;
	BOOL m_bScanMBR;
	BOOL m_bScanBootRecords;
	BOOL m_bScanCompressed;

    // Handle of window to send communication message to.
    HWND m_hWnd;

private:
    // Data members.

    // Action type.
    int m_iAction;

	// Type of scan (all files/program files)
	int m_iScanType;

	// User response
	int m_iResponse;

	// Registry key to save/load from.
	TCHAR m_szRegKey[MAX_PATH];

    // Number of directory scan objects.
    int m_iNumScanObjects;

    // Pointer to directory scan objects.
    LPSCANOBJECT m_pDirObjects;

	// Pointer to data loaded from registry
	LPBYTE m_pRegData;

};

#endif

