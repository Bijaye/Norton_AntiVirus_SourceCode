//===========================================================================
// 
//                               UTILS.H
// 
// 
// 
//===========================================================================



#ifndef  _UTILS_H_
#define  _UTILS_H_


//---------------------------------------------------------------------------
// Forward declarations.
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Definitions.

#define  _64K_BUFFER                   (64*1024)

#define  HLP_SUCCESS                      ((DWORD)(0))      // No error.
#define  HLP_ERROR                        ((DWORD)(-1))     // Generic error
#define  HLP_ERROR_CANNOT_RENAME_FILE     ((DWORD)(-3))
#define  HLP_ERROR_CANNOT_WIPE_FILE       ((DWORD)(-4))
#define  HLP_ERROR_CANNOT_SET_FILESIZE    ((DWORD)(-5))
#define  HLP_ERROR_CANNOT_SET_FILETIME    ((DWORD)(-6))

#define  HLP_INVALID_PARAMETER            0xFFFFFFF0


#define  CHAR_VAL(x) (bCaseSensitive?(x):toupper(x))
#define  CHAR_QUE    (1)
#define  CHAR_STAR   (2)



// Makes sure "C:\DIR\" will remain in tact and
//            "C:\DIR" will be converted to "C:\DIR\".
inline void StrCatBackSlash (char *str) 
{ 
   if (!str)               return;
   int i = strlen (str);
   if (!i || (str[i-1] != '\\'))   strcat (str, "\\");
}
// All names must be returned without trailing '\'. (C:\ -> C:).
inline void RTrimBackSlash (char *str)
{
   if (!str)               return;
   int i = strlen (str);
   if (!i) return;
   while (str[i-1] == '\\')
      {
      str[i-1] = 0;
      i--;
      }
}

// Structure keeps information about file.
typedef struct _FILE_INFO
{
   char        *pFullFilename;   // Dynamically allocated.

   // These are copied from 'WIN32_FIND_DATA' structure.
   DWORD       dwFileAttributes; 
   FILETIME    ftCreationTime; 
   FILETIME    ftLastAccessTime; 
   FILETIME    ftLastWriteTime; 
   DWORD       nFileSizeHigh; 
   DWORD       nFileSizeLow; 
} FILE_INFO;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Classes.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Function prototypes.

extern BOOL  IsEscPressed           (VOID);
extern BOOL  IsArgument             (
         char  **ppArgs,      // input:  As passed to 'main()'.
         int   ArgsNum,       // input:  As passed to 'main()'.
         char  *pArg,         // input:  "value="        Cannot be NULL.
         char  **ppWholeArg,  // output: "value=1024"    Cannot be NULL.
         char  **ppRestArg,   // output: "1024".         Cannot be NULL.
         BOOL  bStrict = FALSE // TRUE - exact copy required (no partial).
         );
extern VOID  clrscr                 (VOID);
extern BOOL  AddPtrToArray          (void ***pppArray, int *piArraySize, const void *pObject);
extern VOID  RemovePtrFromArray     (void ***pppArray, int *piArraySize, int iIndex);
extern VOID  FreeArrayOfPtrs        (void ***pppArray, int *piArraySize);
extern WORD  textattr               (WORD wAttributes);
extern VOID  SetString              (char **ppStrPtr, const char *pString);
extern VOID  DeleteString           (char **ppStrPtr);
extern BOOL  AddStringToArray       (char ***pppArray, int *piArraySize, const char *pszString);
extern int   FindStringInArray      (
         char        **ppStringArray, 
         int         iElementsNum, 
         const char  *pString,
         BOOL        bCaseSensitive /* = FALSE */
         );
extern VOID  DeleteStringFromArray  (char ***pppArray, int *piArraySize, const char *pszString);
extern VOID  DeleteStringArray      (char ***pppArray, int *piArraySize);
extern int   GetRandomNumber        (int iMin, int iMax, int iSeed = 0);
extern char  *FindBeginningOfLine   (
         const char  *pBuffer, 
         const char  *pStartFrom,
         int         iCharsToSearch
         );
extern char  *FindEndOfLine         (
         const char  *pBuffer,
         int         iCharsToSearch
         );
extern char  *StrStr  (
               const char  *pBuffer, 
               const char  *pString, 
               int         iBufferSize,
               BOOL        bCaseSensitive,
               BOOL        bSpecSearch = FALSE,   // TRUE:'?' and '*' considered as spec, not as 'pString' part.
               int         *piFoundLen = NULL
               );
extern VOID  ConvertPatternStringToSpecString (const char *pPattern, char *pConvertedPattern);
extern int   StrStrNi               (const char *pWhere, const char *pWhat, int iBytes);
extern BOOL  SpecMatch              (const char *pSpec, const char *pFileName);
extern BOOL  LoadValue_Bool   (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
         const char  *pValueName,      // Name of value
         BOOL        *pbValue,         // Where to read the value.
         BOOL        bDefault,         // Default value for the value, if not found.
         const char  *pIniFilename = NULL // Name of INI file, if INI file must be used.
         );
extern BOOL  SaveValue_Bool   (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'= NULL) or name of section in INI file.
         const char  *pValueName,      // Name of value
         BOOL        bValue,           // Value.
         const char  *pIniFilename = NULL // Name of INI file, if INI file must be used.
         );
extern BOOL  LoadValue_Int    (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'= NULL) or name of section in INI file.
         const char  *pValueName,      // Name of value
         int         *piValue,         // Where to read the value.
         int         iDefault,         // Default value for the value, if not found.
         const char  *pIniFilename = NULL // Name of INI file, if INI file must be used.
         );
extern BOOL  SaveValue_Int    (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'= NULL) or name of section in INI file.
         const char  *pValueName,      // Name of value
         int         iValue,           // Value.
         const char  *pIniFilename = NULL // Name of INI file, if INI file must be used.
         );
extern BOOL  LoadValue_Dword  (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'= NULL) or name of section in INI file.
         const char  *pValueName,      // Name of value
         DWORD       *pdwValue,        // Where to read the value.
         DWORD       dwDefault,        // Default value for the value, if not found.
         const char  *pIniFilename = NULL // Name of INI file, if INI file must be used.
         );
extern BOOL  SaveValue_Dword  (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'= NULL) or name of section in INI file.
         const char  *pValueName,      // Name of value
         DWORD       dwValue,          // Value.
         const char  *pIniFilename = NULL // Name of INI file, if INI file must be used.
         );
extern BOOL  LoadValue_Filetime   (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'= NULL) or name of section in INI file.
         const char  *pValueName,            // Name of value
         FILETIME    *pftToLoad,             // Where to read the value.
         FILETIME    *pftDefault,            // Default value for the value, if not found.
         const char  *pIniFilename = NULL // Name of INI file, if INI file must be used.
         );
extern BOOL  SaveValue_Filetime  (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName,
         const char  *pValueName, 
         FILETIME    *pftValue, 
         const char  *pIniFilename = NULL // Name of INI file, if INI file must be used.
         );
extern BOOL  LoadValue_Systemtime   (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'= NULL) or name of section in INI file.
         const char  *pValueName, 
         SYSTEMTIME  *pstToLoad, 
         SYSTEMTIME  *pstDefault, 
         const char  *pIniFilename = NULL // Name of INI file, if INI file must be used.
         );
extern BOOL  SaveValue_Systemtime (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName,
         const char  *pValueName, 
         SYSTEMTIME  *pftValue, 
         const char  *pIniFilename = NULL // Name of INI file, if INI file must be used.
         );
extern BOOL  LoadValue_String       (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'= NULL) or name of section in INI file.
         const char  *pValueName, 
         char        *pDestString, 
         int         iDestStrSize, 
         char        *pStrDefault,
         const char  *pIniFilename = NULL // Name of INI file, if INI file must be used.
         );
extern BOOL  SaveValue_String       (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'= NULL) or name of section in INI file.
         const char  *pValueName, 
         char        *pString, 
         const char  *pIniFilename = NULL // Name of INI file, if INI file must be used.
         );
extern BOOL  LoadValue_StringArray  (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'= NULL) or name of section in INI file.
         const char  *pValueName, 
         char        ***pppStrArray, 
         int         *piStrArraySize, 
         const char  *pIniFilename = NULL // Name of INI file, if INI file must be used.
         );
extern BOOL  SaveValue_StringArray  (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'= NULL) or name of section in INI file.
         const char  *pValueName, 
         char        **ppString, 
         int         iStringsNum, 
         const char  *pIniFilename = NULL // Name of INI file, if INI file must be used.
         );
extern BOOL HLP_RegLoadValue (
               HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
               const char  *pKeyName,     // "Software\MENSK Technologies Inc\BXCopy\1.0\Source Directories\E:/PROJECTS/MYPRJ"
               const char  *pValueName,   // "Include Subdirectories"
               char        *pData,        // Where to load data or NULL if data size query.
               DWORD       *pdwDataSize,  // Address of DWORD where: size of 'pData'/bytes copied. if pData=NULL => to receive size of data required.
               char        *pDefaultData     = NULL,  // Default data buffer if key is not in registry.
               DWORD       dwDefaultDataSize = 0      // Size of default data buffer. 'pData' must be large enough to get 'dwDefaultDataSize' bytes.
               );
extern BOOL HLP_RegSaveValue (
               HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
               const char  *pKeyName,     // "Software\MENSK Technologies Inc\BXCopy\1.0\Source Directories\E:/PROJECTS/MYPRJ"
               const char  *pValueName,   // "Include Subdirectories"
               DWORD       dwValueType,   // REG_[DWORD|SZ|MULTI_SZ|BINARY...].
               const char  *pData, 
               DWORD       dwDataSize     // size of data [incl. \0]
               );
extern VOID GetLocalSystemTime      (SYSTEMTIME *pst);
extern VOID StringToSystemTime      (const char *pString, SYSTEMTIME *pst);
extern VOID SystemTimeToString      (const SYSTEMTIME *pst, char *pString);
extern VOID SystemTimeToString2     (const SYSTEMTIME *pst, char *pString);
extern VOID CurrentDateTimeToString2(char *pRetStr);
extern VOID SystemTimeToString3     (const SYSTEMTIME *pst, char *pString);
extern VOID FileTimeToString3       (const FILETIME *pft, char *pString);
extern VOID CurrentDateTimeToString3 (char *pRetStr);
extern VOID DayMonthYearToString    (int iDay, int iMonth, int iYear, char *pString);
extern VOID ConvertPathToRealPath   (char *pszPath);
extern VOID ConvertWeirdNameToFullPathName (
               const char  *pWeirdName,         // "..\..\.\.\dir1\..\..\dir2\file3
               char        *pFullPathName,      // "c:\dir1\dir2\file3"
               DWORD       dwFullPathNameSize,
               char        **ppFilenamePart = NULL
               );
extern VOID WipeTree  (
               const char *pszDirname,
               BOOL        bWipeContents=FALSE, // TRUE:wipe file contents first.
               BOOL        bUseDoDWipe =FALSE   // =FALSE. TRUE:Use DoD to wipe file('bWipeContents' must be TRUE).
               );
extern DWORD   AssembleRootDirName     (const char *pInputName, char *pRootDirName, DWORD dwRootDirSize);
extern char    *JustFilename           (const char *pszFullName, char cDividerChar = '\\');
extern char    *JustDirname            (const char *pszFullName);
extern char    *FileExtension          (char *pFilename);
extern BOOL    DeleteFileEx   (
                  const char  *pFilename,       // Name of file to delete.
                  BOOL        bWipeContents = FALSE,    // =FALSE. TRUE:wipe file contents first.
                  BOOL        bUseDoDWipe = FALSE,      // =FALSE. TRUE:Use DoD to wipe file('bWipeContents' must be TRUE).
                  DWORD       dwClusterSize = 0,    // =0.     Cluster size of the filesystem, where file is located.
                                                //         Required to wipe unused file slack space('bWipeContents' must be TRUE).
                  DWORD       *pdwLastErrorW32=NULL,  // =NULL.  Ptr to where return WIN32 last error.
                  DWORD       *pdwLastErrorHLP=NULL  // =NULL.  Ptr to where to return HELPERS last error (HLP_ERROR_WRITE_FAILURE)
                  );
extern BOOL    TakeOwnership     (char *pFilename);
extern BOOL    SetFileContents   (
                  HANDLE         hFile,            // Handle of opened file.
                  unsigned char  ucByte,           // Set contents of file to this value.
                  DWORD          dwFileSize,       // Number of bytes to write.
                  BOOL           bVerify = FALSE,          // =FALSE. TRUE:verify write operation.
                  DWORD          *pdwLastErrorW32 = NULL  // =NULL.  Ptr to where return WIN32 last error.
                  );
extern DWORD   GetDirnameIndexFromName    (const char *pDirOrFilename, int *piDirNameIdx);
extern VOID    GetFilenameIndexFromName   (const char *pDirOrFilename, int *piFileNameIdx);
extern DWORD   HLP_GetFileAttributes      (const char *pszObjectName, WIN32_FIND_DATA *pW32Data = NULL);
extern LONG    HLP_DeleteRegistryKey      (HKEY hOpenedParentKey, const char *pszSubKeyNameToDelete);
extern VOID    GetLastErrorText           (char *pBuffer, int iBuffSize, DWORD dwLastError);
extern DWORD   StartTimer                 (VOID);
extern DWORD   QueryTimer                 (DWORD dwStartingValue);
extern DWORD   StopTimer                  (DWORD dwStartingValue);
extern BOOL    FindKey (
                  const char  *pBuffer,         // Where to search for.
                  int         iBufferSize,      // Size of buffer.
                  const char  *pKeyName,        // ASCIIZ name of key.
                  char        cSeparator,
                  BOOL        bCaseSensitive,   // TRUE:case sensitive
                  char        *pKeyValueStr,    // Where to return key value.
                  int         iKeyValueStrSize  // Size of destination buffer
                  );
extern DWORD   GetRandomDword (VOID);
//---------------------------------------------------------------------------


#endif   // _UTILS_H_
