// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/findall.hpv   1.8   09 Dec 1996 15:07:32   MZAREMB  $
//
// Description:
//      Constants, typedefs, and prototypes for FindAll class.
//
// Contains:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/findall.hpv  $
// 
//    Rev 1.8   09 Dec 1996 15:07:32   MZAREMB
// Changed an (#ifdef SYM_WIN16) to be (#ifndef SYM_WIN32) to be consistent
// with the associated CPP file.
// 
//    Rev 1.7   09 Sep 1996 15:51:18   JBRENNA
// Add NON_TRANS_OEM_CHARS_*. I do not really like putting these defines
// here. But, there does not seem to be a better place.
// 
//************************************************************************
#ifndef _ITERATE_
#define _ITERATE_

typedef WORD ENUMSTATUS;

#define ERR_ENUM_OK             ((WORD)0)
#define ERR_ENUM_INITIALIZATION ((WORD)1)
#define ERR_ENUM_INTERNAL       ((WORD)2)
#ifndef SYM_WIN32
#define ERR_ENUM_NO_MEMORY      ((WORD)3)
#endif

#define MAX_LEVEL   ((WORD)130)

// These characters appear in a OEM filename when the original ANSI character
// cannot be translated to OEM. On Win95, the ANSI character becomes "_" in
// OEM. On WinNT, the ANSI character becomes either "*" or "?" in OEM.
#define NON_TRANS_OEM_CHARS_95    (_T("_"))
#define NON_TRANS_OEM_CHARS_NT    (_T("*?"))


// --------------------------------------------------------------------------
// A pointer to tagFINDALL structure is passed the the LPFNFINDALL function
// Note that the dwFileAttributes field should be masked with the attribute
// defines that are being used by the Win32 APIs.
// --------------------------------------------------------------------------

typedef struct tagFINDALL
{
    DWORD       dwStructSize;                       // This struct size
    DWORD       dwCookie;                           // User defined
    DWORD       dwFileSize;                         // Size of this file
    DWORD       dwFileAttributes;                   // Attribs of this file
    char        szFullPath      [ SYM_MAX_PATH + 1 ];// Full path where file is
    char        szShortFileName [ SYM_MAX_PATH + 1 ];// Short file name
    char        szLongFileName  [ SYM_MAX_PATH + 1 ];// Long file name
    FILETIME    ftCreationTime;                     // Creation time
    FILETIME    ftLastAccessTime;                   // Last access time
    FILETIME    ftLastWriteTime;                    // Last write time

} FINDALL, FAR * LPFINDALL;

// --------------------------------------------------------------------------
// The application provided callback function should be of this format. The
// return value should be TRUE if the file iteration should continue or FALSE
// if the iteration should be halted.
// --------------------------------------------------------------------------

typedef BOOL (WINAPI * LPFNFILEITERATE) ( LPFINDALL );

// --------------------------------------------------------------------------
// Before starting iterating the application must provide the following info
// to the CFileIterate::StartIterate() member function.
// --------------------------------------------------------------------------

typedef struct tagFINDALLINFO
{
    char            szStartPath [ SYM_MAX_PATH + 1 ]; // Start at this path
    BOOL            bSubDirs;                       // enter dub directories
    LPFNFILEITERATE lpfn;                           // callback func pointer
    DWORD           dwCookie;                       // User defined data

} FINDALLINFO, FAR * LPFINDALLINFO;

// --------------------------------------------------------------------------
// This class definition contains all the functions that are required to
// iterate through directory trees.
// --------------------------------------------------------------------------

class FAR CFindAll
{
public:
    SYM_EXPORT CFindAll();              // Constructor
    SYM_EXPORT ~CFindAll();             // Destructor

    ENUMSTATUS SYM_EXPORT WINAPI StartIterate (// Start iterate function
        LPFINDALLINFO );                // [in] initialization info

private:
    VOID WINAPI FreeAllVars (           // Free all the variables
        BOOL bFreeHandle );             // [in] if handles should be freed

    BOOL WINAPI IsDirectory(
        VOID );

    BOOL WINAPI EnterDirectories (
        VOID );

    BOOL WINAPI FixupNewPath (
        VOID );

    BOOL WINAPI ValidateStartInfo ( 
        LPFINDALLINFO lpStartInfo );    // [in] initialization info

    BOOL WINAPI InitializeObject (
        LPFINDALLINFO lpStartInfo );    // [in] initialization info

    BOOL WINAPI IndicateBackOneDir (
        VOID );

    BOOL WINAPI IsValidItem (
        VOID );

    BOOL WINAPI ShowItemFound (
        VOID );

    BOOL WINAPI WrapperForFileFindNext32 (
        HFIND hFind,
        LPFINDDATA lpFindData );

private:
    FINDDATA        m_FindData;                 // Info on found file
    HFIND           m_hHandles[ MAX_LEVEL ];    // Array of handles
#ifndef SYM_WIN32
    FINDDATA        *m_pFindData[ MAX_LEVEL ];  // Array of ptrs to file data
#endif
    UINT            m_uCurrLevel;               // Current level
    FINDALLINFO     m_StartInfo;                // Start info from app
    int             m_iLenFullPath;             // Store current len to save
                                                // on STRLEN() evals.
    char            m_szFullPath [ SYM_MAX_PATH ];
    char            m_szFileName [ SYM_MAX_PATH ];
};

#endif // _ITERATE_
