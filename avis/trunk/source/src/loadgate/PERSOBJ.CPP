//===========================================================================
// 
//                               PERSOBJ.CPP
// 
// Copyright (c) 1997  MENSK Technologies Corporation. All Rights Reserved.
// 
// 
// All LoadValue.../SaveValue...() function use flag, that can be either of 
// or combination ob both
//    USE_STORAGE_REGISTRY
//    USE_STORAGE_INIFILE
// USE_STORAGE_REGISTRY - dictates usage of registry to save/load.
// USE_STORAGE_INIFILE  - dictates usage of ini file to save/load.
// Default value of this flag is the combination of both flags.
// Following table gives an idea of which actions are being taken for any combination
// of current storage/flags:
//
// ------------+---------------------------------------------------------------------------
// Persistent  |              action flags requested:
// Storage     | --------------------------------------------------------------------------
// set to:v    |  USE_STORAGE_REGISTRY    USE_STORAGE_INIFILE   BOTH
// ------------|---------------------------------------------------------------------------
//             |   Load / Save            Load / Save           Load / Save  
// INI         |   does nothing           uses INI file         uses INI file
//             |   return FALSE.
//             |
// ------------|---------------------------------------------------------------------------
//             |   Load / Save            Load / Save           Load / Save  
// REG         |   uses registry          does nothing          uses registry
//             |                          return FALSE.
//             |
// ------------|---------------------------------------------------------------------------
// INI+REG     |   Load / Save            Load / Save           Load uses registry first.
//             |   uses registry          uses ini file         Then overwrites it with 
//             |   only                   only                  contents of INI file.
//             |                                                So for Load - INI file data
//             |                                                has higher precedence.
//             |                                                Save saves data to both.                                                                                   
// ------------+---------------------------------------------------------------------------
//===========================================================================


// Suppress warnings, coming from Bill Gates's code.
//
#pragma  warning (disable : 4201)   // Suppress warning: "Nonstandard extension used..." (from MS *.H files.)
#pragma  warning (disable : 4514)   // Suppress warning: "Unreferenced inline function has been removed..."


///!!!#define  _WIN32_WINNT   0x0400      // So that 'ReadDirectoryChangesW' will get included.

#include <windows.h>
#include <process.h>                // Threads.
#include <stdio.h>
#include <conio.h>
#include <string.h>

#include "tg_all.h"


//---------------------------------------------------------------------------
// Local definitons.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Globals.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local function prototypes.
//---------------------------------------------------------------------------


//===========================================================================
//

BOOL  PersistentObject::LoadValue_Bool   (
         const char  *pValueName, 
         BOOL        *pbValue, 
         BOOL        bDefault,
         DWORD       dwFlags /* = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE) */
         )
{
   return (LoadValue_Dword  (pValueName, (DWORD *)pbValue, (DWORD)bDefault, dwFlags));
}
//===========================================================================

//===========================================================================
//

BOOL  PersistentObject::SaveValue_Bool   (
         const char  *pValueName,
         BOOL        bValue,
         DWORD       dwFlags /* = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE) */
         )
{
   return (SaveValue_Dword  (pValueName, (DWORD)bValue, dwFlags));
}
//===========================================================================

//===========================================================================
//

BOOL  PersistentObject::LoadValue_Int   (
         const char  *pValueName, 
         int         *piValue, 
         int         iDefault,
         DWORD       dwFlags /* = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE) */
         )
{
   return (LoadValue_Dword  (pValueName, (DWORD *)piValue, (DWORD)iDefault, dwFlags));
}
//===========================================================================

//===========================================================================
//

BOOL  PersistentObject::SaveValue_Int   (
         const char  *pValueName,
         int         iValue,
         DWORD       dwFlags /* = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE) */
         )
{
   return (SaveValue_Dword  (pValueName, (DWORD)iValue, dwFlags));
}
//===========================================================================

//===========================================================================
//

BOOL  PersistentObject::LoadValue_Dword   (
         const char  *pValueName, 
         DWORD       *pdwValue, 
         DWORD       dwDefault,
         DWORD       dwFlags /* = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE) */
         )
{
//---------------------------------------------------------------------------
   BOOL           bRetCode=FALSE;
//---------------------------------------------------------------------------

   if (dwFlags & USE_STORAGE_REGISTRY)
      {
      bRetCode =
         ::LoadValue_Dword   (
            GetBaseKeyHandle(),   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            GetFullRegistryKeyName(), // const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            pValueName, // const char  *pValueName:   Name of value
            pdwValue,   // DWORD       *pdwValue:      Where to read the value.
            dwDefault,  // DWORD       dwDefault:      Default value for the value, if not found.
            NULL        // const char  *pIniFilename /*=NULL*/:   Name of INI file, if INI file must be used.
            );
      }

   if (dwFlags & USE_STORAGE_INIFILE)
      {
      // Use default value - the one that's in the registry.
      // The value, that's stored into INI file will take precedence over
      // the one stored in registry.
      DWORD dwNewDefault = (dwFlags & USE_STORAGE_REGISTRY)?*pdwValue:dwDefault;
      bRetCode =
         ::LoadValue_Dword   (
            GetBaseKeyHandle(),   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            GetIniSectionName(), // const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            pValueName, // const char  *pValueName:   Name of value
            pdwValue,   // DWORD       *pdwValue:      Where to read the value.
            dwNewDefault,  // DWORD       dwDefault:      Default value for the value, if not found.
            GetIniFilename()// const char  *pIniFilename /*=NULL*/:   Name of INI file, if INI file must be used.
            );
      }

   return (bRetCode);
}
//===========================================================================

//===========================================================================
// 

BOOL  PersistentObject::SaveValue_Dword   (
         const char  *pValueName,
         DWORD       dwValue,
         DWORD       dwFlags /* = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE) */
         )
{
//---------------------------------------------------------------------------
   BOOL           bRetCode=FALSE;
//---------------------------------------------------------------------------

   if (dwFlags & USE_STORAGE_REGISTRY)
      {
      bRetCode =
         ::SaveValue_Dword   (
            GetBaseKeyHandle(),   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            GetFullRegistryKeyName(), // const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            pValueName, // const char  *pValueName:   Name of value
            dwValue,    // DWORD       dwValue:  Value.
            NULL        // const char  *pIniFilename /*=NULL*/:   Name of INI file, if INI file must be used.
            );
      }

   if (dwFlags & USE_STORAGE_INIFILE)
      {
      bRetCode =
         ::SaveValue_Dword   (
            GetBaseKeyHandle(),   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            GetIniSectionName(), // const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            pValueName, // const char  *pValueName:   Name of value
            dwValue,    // DWORD       dwValue:  Value.
            GetIniFilename()// const char  *pIniFilename /*=NULL*/:   Name of INI file, if INI file must be used.
            );
      }

   return (bRetCode);
}
//===========================================================================

//===========================================================================
//

BOOL  PersistentObject::LoadValue_Filetime   (
         const char  *pValueName,            // Name of value
         FILETIME    *pftToLoad,             // Where to read the value.
         FILETIME    *pftDefault,            // Default value for the value, if not found.
         DWORD       dwFlags /* = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE) */
         )
{
//---------------------------------------------------------------------------
   BOOL           bRetCode=FALSE;
//---------------------------------------------------------------------------

   if (dwFlags & USE_STORAGE_REGISTRY)
      {
      bRetCode =
         ::LoadValue_Filetime   (
            GetBaseKeyHandle(),   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            GetFullRegistryKeyName(), // const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            pValueName,             // const char  *pValueName,            // Name of value
            pftToLoad,              // FILETIME    *pftToLoad,             // Where to read the value.
            pftDefault,             // FILETIME    *pftDefault,            // Default value for the value, if not found.
            NULL                    // const char  *pIniFilename /*=NULL*/:   Name of INI file, if INI file must be used.
            );
      }

   if (dwFlags & USE_STORAGE_INIFILE)
      {
      // Use default value - the one that's in the registry.
      // The value, that's stored into INI file will take precedence over
      // the one stored in registry.
      FILETIME *pftNewDefault = (dwFlags & USE_STORAGE_REGISTRY)?pftToLoad:pftDefault;
      bRetCode =
         ::LoadValue_Filetime   (
            GetBaseKeyHandle(),   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            GetIniSectionName(), // const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            pValueName,             // const char  *pValueName,            // Name of value
            pftToLoad,              // FILETIME    *pftToLoad,             // Where to read the value.
            pftNewDefault,          // FILETIME    *pftDefault,            // Default value for the value, if not found.
            GetIniFilename()        // const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
            );
      }

   return (bRetCode);
}
//===========================================================================

//===========================================================================
// 

BOOL  PersistentObject::SaveValue_Filetime  (
         const char  *pValueName, 
         FILETIME    *pftValue,
         DWORD       dwFlags /* = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE) */
         )
{
//---------------------------------------------------------------------------
   BOOL           bRetCode=FALSE;
//---------------------------------------------------------------------------

   if (dwFlags & USE_STORAGE_REGISTRY)
      {
      bRetCode =
         ::SaveValue_Filetime  (
            GetBaseKeyHandle(),   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            GetFullRegistryKeyName(), // const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            pValueName,             // const char  *pValueName, 
            pftValue,               // FILETIME    *pftValue, 
            NULL                    // const char  *pIniFilename /*=NULL*/:   Name of INI file, if INI file must be used.
            );
      }

   if (dwFlags & USE_STORAGE_INIFILE)
      {
      bRetCode =
         ::SaveValue_Filetime  (
            GetBaseKeyHandle(),   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            GetIniSectionName(),    // const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            pValueName,             // const char  *pValueName, 
            pftValue,               // FILETIME    *pftValue, 
            GetIniFilename()        // const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
            );
      }

   return (bRetCode);
}
//===========================================================================

//===========================================================================
// 

BOOL  PersistentObject::LoadValue_Systemtime   (
         const char  *pValueName, 
         SYSTEMTIME  *pstToLoad, 
         SYSTEMTIME  *pstDefault,
         DWORD       dwFlags /* = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE) */
         )
{
//---------------------------------------------------------------------------
   BOOL           bRetCode=FALSE;
//---------------------------------------------------------------------------

   if (dwFlags & USE_STORAGE_REGISTRY)
      {
      bRetCode =
         ::LoadValue_Systemtime   (
            GetBaseKeyHandle(),   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            GetFullRegistryKeyName(), // const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            pValueName,             // const char  *pValueName, 
            pstToLoad,              // SYSTEMTIME  *pstToLoad, 
            pstDefault,             // SYSTEMTIME  *pstDefault, 
            NULL                    // const char  *pIniFilename /*=NULL*/:   Name of INI file, if INI file must be used.
            );
      }

   if (dwFlags & USE_STORAGE_INIFILE)
      {
      // Use default value - the one that's in the registry.
      // The value, that's stored into INI file will take precedence over
      // the one stored in registry.
      SYSTEMTIME *pstNewDefault = (dwFlags & USE_STORAGE_REGISTRY)?pstToLoad:pstDefault;
      bRetCode =
         ::LoadValue_Systemtime   (
            GetBaseKeyHandle(),   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            GetIniSectionName(),    // const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            pValueName,             // const char  *pValueName, 
            pstToLoad,              // SYSTEMTIME  *pstToLoad, 
            pstNewDefault,          // SYSTEMTIME  *pstDefault, 
            GetIniFilename()        // const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
            );
      }

   return (bRetCode);
}
//===========================================================================

//===========================================================================
//

BOOL  PersistentObject::SaveValue_Systemtime (
         const char  *pValueName, 
         SYSTEMTIME  *pstValue,
         DWORD       dwFlags /* = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE) */
         )
{
//---------------------------------------------------------------------------
   BOOL           bRetCode=FALSE;
//---------------------------------------------------------------------------

   if (dwFlags & USE_STORAGE_REGISTRY)
      {
      bRetCode =
         ::SaveValue_Systemtime (
            GetBaseKeyHandle(),   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            GetFullRegistryKeyName(), // const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            pValueName,             // const char  *pValueName, 
            pstValue,               // SYSTEMTIME  *pftValue, 
            NULL                    // const char  *pIniFilename /*=NULL*/:   Name of INI file, if INI file must be used.
            );
      }

   if (dwFlags & USE_STORAGE_INIFILE)
      {
      bRetCode =
         ::SaveValue_Systemtime (
            GetBaseKeyHandle(),   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            GetIniSectionName(),    // const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            pValueName,             // const char  *pValueName, 
            pstValue,               // SYSTEMTIME  *pftValue, 
            GetIniFilename()            // const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
            );
      }

   return (bRetCode);
}
//===========================================================================

//===========================================================================
// 

BOOL  PersistentObject::LoadValue_String       (
         const char  *pValueName, 
         char        *pDestString, 
         int         iDestStrSize, 
         char        *pStrDefault,
         DWORD       dwFlags /* = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE) */
         )
{
//---------------------------------------------------------------------------
   BOOL           bRetCode=FALSE;
//---------------------------------------------------------------------------

   if (dwFlags & USE_STORAGE_REGISTRY)
      {
      bRetCode =
         ::LoadValue_String       (
            GetBaseKeyHandle(),   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            GetFullRegistryKeyName(), // const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            pValueName,             // const char  *pValueName, 
            pDestString,            // char        *pDestString, 
            iDestStrSize,           // int         iDestStrSize, 
            pStrDefault,            // char        *pStrDefault,
            NULL                    // const char  *pIniFilename /*=NULL*/:   Name of INI file, if INI file must be used.
            );
      }

   if (dwFlags & USE_STORAGE_INIFILE)
      {
      // Use default value - the one that's in the registry.
      // The value, that's stored into INI file will take precedence over
      // the one stored in registry.
      char *pNewDefault;
      SetString (&pNewDefault, (dwFlags & USE_STORAGE_REGISTRY)?pDestString:pStrDefault);

      bRetCode =
         ::LoadValue_String       (
            GetBaseKeyHandle(),   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            GetIniSectionName(),    // const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            pValueName,             // const char  *pValueName, 
            pDestString,            // char        *pDestString, 
            iDestStrSize,           // int         iDestStrSize, 
            pNewDefault,            // char        *pStrDefault,
            GetIniFilename()        // const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
            );

      DeleteString (&pNewDefault);
      }

   return (bRetCode);
}
//===========================================================================

//===========================================================================
// 

BOOL  PersistentObject::SaveValue_String       (
         const char  *pValueName, 
         char        *pString,
         DWORD       dwFlags /* = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE) */
         )
{
//---------------------------------------------------------------------------
   BOOL           bRetCode=FALSE;
//---------------------------------------------------------------------------

   if (dwFlags & USE_STORAGE_REGISTRY)
      {
      bRetCode =
         ::SaveValue_String       (
            GetBaseKeyHandle(),   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            GetFullRegistryKeyName(), // const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            pValueName,             // const char  *pValueName, 
            pString,                // char        *pString, 
            NULL                    // const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
            );
      }

   if (dwFlags & USE_STORAGE_INIFILE)
      {
      bRetCode =
         ::SaveValue_String       (
            GetBaseKeyHandle(),   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            GetIniSectionName(),    // const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            pValueName,             // const char  *pValueName, 
            pString,                // char        *pString, 
            GetIniFilename()            // const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
            );
      }


   return (bRetCode);
}
//===========================================================================

//===========================================================================
// 
// Note: DeleteStringArray   (char ***pppArray, int *piArraySize)
//       must be used to free memory, allocated by this function.

BOOL  PersistentObject::LoadValue_StringArray  (
         const char  *pValueName, 
         char        ***pppStrArray, 
         int         *piStrArraySize,
         DWORD       dwFlags /* = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE) */
         )
{
//---------------------------------------------------------------------------
   BOOL           bRetCode=FALSE;
//---------------------------------------------------------------------------

   *pppStrArray    = NULL;
   *piStrArraySize = 0;

   if (dwFlags & USE_STORAGE_REGISTRY)
      {
      bRetCode =
         ::LoadValue_StringArray  (
            GetBaseKeyHandle(),   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            GetFullRegistryKeyName(), // const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            pValueName,             // const char  *pValueName, 
            pppStrArray,            // char        ***pppStrArray, 
            piStrArraySize,         // int         *piStrArraySize, 
            NULL                    // const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
            );
      }

   if (dwFlags & USE_STORAGE_INIFILE)
      {
      char  **ppStrArray = NULL;
      int   iArraySize   = 0;

      // Read INI file into the temporary storage.
      bRetCode =
         ::LoadValue_StringArray  (
            GetBaseKeyHandle(),   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            GetIniSectionName(),    // const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            pValueName,             // const char  *pValueName, 
            &ppStrArray,            // char        ***pppStrArray, 
            &iArraySize,            // int         *piStrArraySize, 
            GetIniFilename()        // const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
            );

      // If registry data was initialized and data from INI file was also present
      // then give INI file data precedence.
      if (iArraySize)
         {
         if (*piStrArraySize)
            DeleteStringArray (pppStrArray, piStrArraySize);

         *pppStrArray    = ppStrArray;
         *piStrArraySize = iArraySize;
         }
      }

   return (bRetCode);
}
//===========================================================================

//===========================================================================
// 

BOOL  PersistentObject::SaveValue_StringArray  (
         const char  *pValueName, 
         char        **ppString, 
         int         iStringsNum,
         DWORD       dwFlags /* = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE) */
         )
{
//---------------------------------------------------------------------------
   BOOL           bRetCode=FALSE;
//---------------------------------------------------------------------------

   if (dwFlags & USE_STORAGE_REGISTRY)
      {
      bRetCode =
         ::SaveValue_StringArray  (
            GetBaseKeyHandle(),   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            GetFullRegistryKeyName(), // const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            pValueName,             // const char  *pValueName, 
            ppString,               // char        **ppString, 
            iStringsNum,            // int         iStringsNum, 
            NULL                    // const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
            );
      }

   if (dwFlags & USE_STORAGE_INIFILE)
      {
      bRetCode =
         ::SaveValue_StringArray  (
            GetBaseKeyHandle(),   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            GetIniSectionName(),    // const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            pValueName,             // const char  *pValueName, 
            ppString,               // char        **ppString, 
            iStringsNum,            // int         iStringsNum, 
            GetIniFilename()            // const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
            );
      }

   return (bRetCode);
}
//===========================================================================

//===========================================================================
VOID  PersistentObject::EraseFullRegistryKeyName   (VOID)
{
   // Delete key and all subkeys
   //
   if (GetFullRegistryKeyName())
      HLP_DeleteRegistryKey (GetBaseKeyHandle(), GetFullRegistryKeyName());
}
//===========================================================================

//===========================================================================
VOID  PersistentObject::EraseIniSectionName        (VOID)
{
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

   // Delete whole section from INI file...
   //
   if (GetIniFilename() && GetIniSectionName())
      WritePrivateProfileString ( // BOOL: 0:fail, else:success.
         GetIniSectionName(), // LPCTSTR lpAppName:  pointer to section name 
         NULL,                // LPCTSTR lpKeyName:  pointer to key name 
         NULL,                // LPCTSTR lpString:   pointer to string to add 
         GetIniFilename()     // LPCTSTR lpFileName: pointer to initialization filename 
         );
}
//===========================================================================
