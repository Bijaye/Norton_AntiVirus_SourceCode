//===========================================================================
//
//                               BROWSER.CPP
//
//
//===========================================================================


// Suppress warnings, coming from Bill Gates's code.
//
#pragma  warning (disable : 4201)   // Suppress warning: "Nonstandard extension used..." (from MS *.H files.)
#pragma  warning (disable : 4514)   // Suppress warning: "Unreferenced inline function has been removed..."

#include <windows.h>
#include <process.h>
#include <conio.h>
#include <stdio.h>

#include "tg_all.h"


//---------------------------------------------------------------------------
// Local definitions.
#define  EXCLUDE_CHAR   ':'

typedef struct _LISTFILES_TW_DATA
{
   Fileset     *pFileset;              // Files of interest.
   BOOL        bIncludeSubdirs;        // TRUE: include subdirectories.
   int         iMaxFileToSearch;       // -1(unlimited):max number of files to return.

   FILE_INFO   **ppFileInfos;          // Array of 'FILE_INFO' structures.
   int         iFileInfosNum;          // Number of elements in array.

   BOOL        bEscPressed;            // Will be set to TRUE is ESC was pressed.
   DWORD       dwCurrentLevel;         // Currently reached directory level. Initially = 0.
   DWORD       dwFilesFound;
   DWORD       dwTotalSize;
} LISTFILES_TW_DATA;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Globals.

WORD     g_wOldAttrs;
char     g_acErrorText [1024];
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local function prototypes.

void  TW_LISTFILES_LevelAdded    (DWORD dwParam, char * /*pszPathDirname*/, char * /*pszDirName*/);
void  TW_LISTFILES_LevelBack     (DWORD dwParam, char * /*pszPathDirname*/);
int   TW_LISTFILES_PerDir   (DWORD dwParam, int DirWhat, char *pszPathDirname, char *pszDirName);
int   TW_LISTFILES_PerFile  (DWORD dwParam, char *pszPathFilename, char *pszFilename, WIN32_FIND_DATA *pw32FileData);
//---------------------------------------------------------------------------

//===========================================================================
//
// Returns:    TRUE:success, FALSE:failure.
// In case of failure - '*ppLastErrorText' - points to the last eror text.
//
// 'FreeFileInfoList()' must be called to 
// release memory, allocated by 'AllocateFileInfoList'.

BOOL  AllocateFileInfoList (
         const char  *pDirName,        // Starting directory name.
         int         iMaxFileToSearch, // -1(unlimited):max number of files to return.
         Fileset     *pFileset,        // Initialized fileset.
         BOOL        bIncludeSubdirs,  // TRUE:=>Include subdirs in search.
         FILE_INFO   ***pppFileInfos,  // Ptr to an array of pointers to 'FILE_INFO' structure.
         int         *piFileInfosNum,  // Number of elements in array.
         char        **ppLastErrorText /*=NULL*/ // Ptr to the "reason for error" text description.
         )
{
//---------------------------------------------------------------------------
   LISTFILES_TW_DATA    twData;
   BOOL                 bRetCode;
//---------------------------------------------------------------------------


   // Get current attributes.
   g_wOldAttrs = textattr (FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);

   // Reset it.
   g_acErrorText [0] = 0;
   if (ppLastErrorText)
      *ppLastErrorText = g_acErrorText;

   *pppFileInfos   = NULL;
   *piFileInfosNum = 0;

   if (!iMaxFileToSearch)
      return (TRUE);

// typedef struct _LISTFILES_TW_DATA
// {
//    Fileset     *pFileset;              // Files of interest.
//    BOOL        bIncludeSubdirs;        // TRUE: include subdirectories.
//    int         iMaxFileToSearch;       // -1(unlimited):max number of files to return.
// 
//    FILE_INFO   **ppFileInfos;          // Array of 'FILE_INFO' structures.
//    int         iFileInfosNum;          // Number of elements in array.
// 
//    BOOL        bEscPressed;            // Will be set to TRUE is ESC was pressed.
//    DWORD       dwCurrentLevel;         // Currently reached directory level. Initially = 0.
//    DWORD       dwFilesFound;
//    DWORD       dwTotalSize;
// } LISTFILES_TW_DATA;

   // Initialize 'LISTFILES_TW_DATA' structure.
   memset (&twData, 0, sizeof(twData));

   twData.pFileset               = pFileset;
   twData.bIncludeSubdirs        = bIncludeSubdirs;
   twData.iMaxFileToSearch       = iMaxFileToSearch;

   twData.ppFileInfos            = NULL;
   twData.iFileInfosNum          = 0;

   twData.bEscPressed            = FALSE;
   twData.dwCurrentLevel         = 0;
   twData.dwFilesFound           = 0;
   twData.dwTotalSize            = 0;

   TreeWalk (
      pDirName,            // const char     *pszTreeSpec,                                                  // Starting tree.
      TW_LISTFILES_LevelBack,    // void           (*pFnLevelBack)   (unsigned long),                             // NULL|callback: -- dir level.
      TW_LISTFILES_LevelAdded,   // void           (*pFnLevelAdded)  (unsigned long, char *name),                 // NULL|callback: ++ dir level.
      TW_LISTFILES_PerDir, // int            (*pFnPerDir)      (unsigned long, int DirWhat, char *pathname, char *name), // NULL|dir  found callback.
      TW_LISTFILES_PerFile,// int            (*pFnPerFile)     (unsigned long, char *pathname, char *name, WIN32_FIND_DATA *pData), // NULL|file found callback.
      (DWORD)&twData       // unsigned long  ulParam1                                                       // Passed to all callback functions.
      );


   if (twData.bEscPressed)
      {
      sprintf (g_acErrorText, "\nAborted per user's request.");

      // Free memory allocated before user pressed <ESC>.
      FreeFileInfoList (
         &twData.ppFileInfos,    // FILE_INFO   ***pppFileInfos,  // Ptr to an array of pointers to 'FILE_INFO' structure.
         &twData.iFileInfosNum   // int         *piFileInfosNum   // Number of elements in array.
         );
      goto  ABORT;
      }

   // Initialize final values.
   *pppFileInfos   = twData.ppFileInfos  ;
   *piFileInfosNum = twData.iFileInfosNum;


   bRetCode = TRUE;



EXIT:
   textattr (g_wOldAttrs);
   return (bRetCode);

ABORT:
   bRetCode = FALSE;
   goto EXIT;
}
//===========================================================================

//===========================================================================
//
//

VOID  FreeFileInfoList (
         FILE_INFO   ***pppFileInfos,  // Ptr to an array of pointers to 'FILE_INFO' structure.
         int         *piFileInfosNum   // Number of elements in array.
         )
{
//---------------------------------------------------------------------------
   int            i;
//---------------------------------------------------------------------------

// typedef struct _FILE_INFO
// {
//    char        *pFullFilename;   // Dynamically allocated.
// 
//    // These are copied from 'WIN32_FIND_DATA' structure.
//    DWORD       dwFileAttributes; 
//    FILETIME    ftCreationTime; 
//    FILETIME    ftLastAccessTime; 
//    FILETIME    ftLastWriteTime; 
//    DWORD       nFileSizeHigh; 
//    DWORD       nFileSizeLow; 
// } FILE_INFO;

   for (i=0; i<*piFileInfosNum; i++)
      {
      delete [] (((*pppFileInfos)[i])->pFullFilename);
      delete ((*pppFileInfos)[i]);  // Deallocate next structure.
      }

   FreeArrayOfPtrs ((void ***)pppFileInfos, piFileInfosNum);
}
//===========================================================================

//===========================================================================
void  TW_LISTFILES_LevelAdded    (DWORD dwParam, char * /*pszPathDirname*/, char * /*pszDirName*/)
{
//---------------------------------------------------------------------------
   LISTFILES_TW_DATA *ptwData = (LISTFILES_TW_DATA *)dwParam;
//---------------------------------------------------------------------------

   ptwData->dwCurrentLevel ++;
}
//===========================================================================

//===========================================================================
void  TW_LISTFILES_LevelBack     (DWORD dwParam, char * /*pszPathDirname*/)
{
//---------------------------------------------------------------------------
   LISTFILES_TW_DATA *ptwData = (LISTFILES_TW_DATA *)dwParam;
//---------------------------------------------------------------------------

   ptwData->dwCurrentLevel --;
}
//===========================================================================

//===========================================================================
//
// 'iDirWhat':
//    DIR_FOUND
//    DIR_DONE
//
// Returns:
//          TW_DEFAULT     -  proceed normal traversing.
//          TW_SUCCESS     -  proceed normal traversing.
//          TW_NO_DEEPER   -  do not go inside of currently reached level.
//          TW_CUT_SEARCH  -  Skip filesearch into found directory.
//                            'pFnPerFile()' won't be called also.
//          TW_EXIT        -  terminate search and exit.

int   TW_LISTFILES_PerDir   (DWORD dwParam, int DirWhat, char * /*pszPathDirname*/, char * /*pszDirName*/)
{
//---------------------------------------------------------------------------
   LISTFILES_TW_DATA *ptwData = (LISTFILES_TW_DATA *)dwParam;
   int               iRetCode = TW_SUCCESS;
//---------------------------------------------------------------------------

// typedef struct _LISTFILES_TW_DATA
// {
//    Fileset     *pFileset;              // Files of interest.
//    BOOL        bIncludeSubdirs;        // TRUE: include subdirectories.
//    int         iMaxFileToSearch;       // -1(unlimited):max number of files to return.
// 
//    FILE_INFO   **ppFileInfos;          // Array of 'FILE_INFO' structures.
//    int         iFileInfosNum;          // Number of elements in array.
// 
//    BOOL        bEscPressed;            // Will be set to TRUE is ESC was pressed.
//    DWORD       dwCurrentLevel;         // Currently reached directory level. Initially = 0.
//    DWORD       dwFilesFound;
//    DWORD       dwTotalSize;
// } LISTFILES_TW_DATA;

   switch (DirWhat)
      {
      case  DIR_FOUND:  // New directory just been found in source tree.
         {
         if (ptwData->dwCurrentLevel > 0 && !ptwData->bIncludeSubdirs)
            iRetCode = (TW_NO_DEEPER | TW_CUT_SEARCH);
         }
         break;

      case  DIR_DONE:   // Finished searchning for files in this directory.
         break;

      default:          // Unknown control code.
         break;
      }

//EXIT:
   return (iRetCode);
}
//===========================================================================


//===========================================================================
//
// Returns:
//          TW_DEFAULT     -  proceed normal traversing.
//          TW_SUCCESS     -  proceed normal traversing.
//          TW_NO_DEEPER   -  do not go inside of currently reached level.
//          TW_CUT_SEARCH  -  Skip filesearch into found directory.
//                            'pFnPerFile()' won't be called also.
//          TW_EXIT        -  terminate search and exit.

int   TW_LISTFILES_PerFile  (DWORD dwParam, char *pszPathFilename, char *pszFilename, WIN32_FIND_DATA *pw32FileData)
{
//---------------------------------------------------------------------------
   LISTFILES_TW_DATA *ptwData    = (LISTFILES_TW_DATA *)dwParam;

   FILE_INFO         *pNewInfo   = NULL;

   int               iRetCode    = TW_SUCCESS;
//---------------------------------------------------------------------------

// typedef struct _LISTFILES_TW_DATA
// {
//    Fileset     *pFileset;              // Files of interest.
//    BOOL        bIncludeSubdirs;        // TRUE: include subdirectories.
//    int         iMaxFileToSearch;       // -1(unlimited):max number of files to return.
// 
//    FILE_INFO   **ppFileInfos;          // Array of 'FILE_INFO' structures.
//    int         iFileInfosNum;          // Number of elements in array.
// 
//    BOOL        bEscPressed;            // Will be set to TRUE is ESC was pressed.
//    DWORD       dwCurrentLevel;         // Currently reached directory level. Initially = 0.
//    DWORD       dwFilesFound;
//    DWORD       dwTotalSize;
// } LISTFILES_TW_DATA;

   // Process possible keystroke.
   if (IsEscPressed ())
      {
      ptwData->bEscPressed = TRUE;
      iRetCode = TW_EXIT;
      goto  EXIT;
      }

   // Make sure filespec matches.
   if (!ptwData->pFileset->SpecMatch (pszFilename))
      goto  EXIT;

   // Make sure filesize matches.
   if (!ptwData->pFileset->SizeMatch (pw32FileData->nFileSizeLow))
      goto  EXIT;

   // Spec and size match!

// typedef struct _FILE_INFO
// {
//    char        *pFullFilename;   // Dynamically allocated.
// 
//    // These are copied from 'WIN32_FIND_DATA' structure.
//    DWORD       dwFileAttributes; 
//    FILETIME    ftCreationTime; 
//    FILETIME    ftLastAccessTime; 
//    FILETIME    ftLastWriteTime; 
//    DWORD       nFileSizeHigh; 
//    DWORD       nFileSizeLow; 
// } FILE_INFO;

   // Add information about found file to array of pointers to FILE_INFO structures.
   pNewInfo = new FILE_INFO;
   memset (pNewInfo, 0, sizeof (*pNewInfo));
   pNewInfo->pFullFilename = new char [strlen(pszPathFilename) + 1];
   strcpy (pNewInfo->pFullFilename, pszPathFilename);
   pNewInfo->dwFileAttributes = pw32FileData->dwFileAttributes;
   pNewInfo->ftCreationTime   = pw32FileData->ftCreationTime  ;
   pNewInfo->ftLastAccessTime = pw32FileData->ftLastAccessTime;
   pNewInfo->ftLastWriteTime  = pw32FileData->ftLastWriteTime ;
   pNewInfo->nFileSizeHigh    = pw32FileData->nFileSizeHigh   ;
   pNewInfo->nFileSizeLow     = pw32FileData->nFileSizeLow    ;
   AddPtrToArray ((void ***)&ptwData->ppFileInfos, &ptwData->iFileInfosNum, pNewInfo);
   

   // Save some statistics about "would-be-copied files".
   ptwData->dwFilesFound ++;
   ptwData->dwTotalSize += pw32FileData->nFileSizeLow;

   if (ptwData->iMaxFileToSearch >= 0)
      {
      if (ptwData->dwFilesFound >= (DWORD)ptwData->iMaxFileToSearch)
         iRetCode = TW_EXIT;  // Stop search after max number of files found.
      }


EXIT:
   // Restore attributes.
   textattr (g_wOldAttrs);

   return (iRetCode);
}
//===========================================================================


