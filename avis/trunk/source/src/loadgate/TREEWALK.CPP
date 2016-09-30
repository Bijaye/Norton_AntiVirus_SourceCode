//===========================================================================
//
//                               TREEWALK.CPP
//
// Copyright (c) 1997  MENSK Technologies Corporation. All Rights Reserved.
// 
//
// TreeWalk() routine defined here.
//
//===========================================================================


// Suppress warnings, coming from Bill Gates's code.
//
#pragma  warning (disable : 4201)   // Suppress warning: "Nonstandard extension used..." (from MS *.H files.)
#pragma  warning (disable : 4514)   // Suppress warning: "Unreferenced inline function has been removed..."

#include <windows.h>
#include <stdio.h>
#include <string.h>


#include "treewalk.h"


//---------------------------------------------------------------------------
//         Linked list, used by TreeWalk to save/retrieve/delete directories:
//         -----------------------------------------------------------------
// RRRR - root DIRECTORY_STRUCT. See 'dRootDir'. It's a starting point in a
//        linked list.
//
// #### - represents directory. Every such directory points to:
//        DownSibling ('v') and to Parent ('<-').
//        Every top directory in every level also points to it's first
//        child ('->'). Levels represents hierarchies of dir tree.
//
// .... - Directory, that was already processed and deleted from linked list
//        (via 'DeleteDirectory()').
//
//            Level1
//            ....       Level2
// RRRR <-+   ....       ....       Level3
//        +-> #### <-+   ....
//             V     +-> #### <-+   ....
//            ####        v     +-> ####
//             V         ####        v  
//            ####        v         ####
//                       ####           
//                        v
//                       ####
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Definitions local to the module.

typedef  struct _DIRECTORY_STRUCT
{
   struct _DIRECTORY_STRUCT   *pParent;      // <-   0 if top level parent.
   struct _DIRECTORY_STRUCT   *pChild;       //  ->  0 if no children.
   struct _DIRECTORY_STRUCT   *pDownSibling; //  v   0 if no siblings.
   char                       *pName;        //      just one level name: "WINDOWS".
} DIRECTORY_STRUCT;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local function prototypes.

DIRECTORY_STRUCT  *AddDirectory     (DIRECTORY_STRUCT *pParent, DIRECTORY_STRUCT *pUpSibling, char *pName);
void              DeleteDirectory   (DIRECTORY_STRUCT *pDir);
void              FreeAllDirectories(DIRECTORY_STRUCT *pRootDir);
//---------------------------------------------------------------------------



//===========================================================================
//
// Function will traverse directories and subdirectories of specified
// input tree. If will call PerDir() callback for every directory found
// and PerFile() callback for every file found. See notes below.
// Search will be performed for *.* dirs and files.
// Returns: TW_SUCCESS if search was normally terminated due to
// either callback's request or because of no more dirs.
// In case of any abnormal conditions (invalid input parameters or
// insufficient memory) TW_ERROR returned.
// 
// Parameters:
// -----------
//
//       const char      *pszTreeSpec                              // <- input
//          ASCIIZ starting tree path in format: "C:\WINDOWS\SYSTEM"
//          Must not be NULL.
//
//       void           (*pFnLevelBack)(DWORD, char *pathname)     // <- input
//          This pointer could be NULL.
//          Ptr to callback notification function about search in progress.
//          This function will be called every time tree search will about to 
//          return back one level. Second parameter passed - full name of directory
//          *before* cutting off level.
//          This callback will also be executed, after last directory 
//          under specified tree was found as last attempt to back up one
//          level.
//
//       void           (*pFnLevelAdded)(DWORD, char *pathname, char *name)   // <- input
//          This pointer could be NULL.
//          Ptr to callback notification function about search in progress.
//          This function will be called every time tree search will add
//          one more level to search path.
//          pathname - full name of directory *after* another level 'name'
//          was added.
//
//       int            (*pFnPerDir)(DWORD, int, char *, char *) // <- input
//          This pointer could be NULL.
//          Ptr to function to be called every time, when:
//          -  new directory found (second 'int' == DIR_FOUND).
//          -  done processing (file search) in new directory (second 'int' == DIR_DONE).
//          Second parameter allows to 'pFnPerDir()' to determine when
//          processing of new directory is done (i.e. last file was found
//          and TreeWalk() is about to switch to next directory.
//          'pFnPerDir()' with 'DIR_DONE' as a second parameter is called
//          only when file search wasn't cancelled from the previous call
//          to 'pFnPerDir()'. I.e. when 'pFnPerDir(DIR_FOUND)' returns
//          TW_CUT_SEARCH or TW_EXIT - 'pFnPerDir(DIR_DONE)' won't
//          be called.
//          'ulParam1' will be passed as first parameter and found full
//          directory name as 3-rd parameter, found partial dir name
//          (without path) as 4-th parameter.
//          Return codes (OR-ed flags) from pFnPerDir() will dictate
//          further actions of TreeWalk():
//          TW_DEFAULT  (0)   -  proceed normal traversing.
//          TW_NO_DEEPER (2)  -  do not go inside of currently reached level.
//          TW_CUT_SEARCH (4)  -  Skip search into found directory.
//                                     'pFnPerFile()' won't be called also.
//          TW_EXIT      (256)-  terminate search and exit.
//
//       int            (*pFnPerFile)(DWORD, char *, char *, WIN32_FIND_DATA *) // <- input
//          This pointer could be NULL.
//          Ptr to function to be called every time new directory found.
//          'ulParam1' will be passed as first parameter and found full
//          file name as a second parameter, found partial file name
//          (without path) as a third parameter. Additional information
//          about file: date, time, size, attributes will be pointed to by
//          WIN32_FIND_DATA structure.
//          Return codes (OR-ed flags) from pFnPerDir() will dictate
//          further actions of TreeWalk():
//          TW_DEFAULT  (0)   -  proceed normal traversing.
//          TW_NO_DEEPER (2)  -  do not go inside of currently reached level.
//          TW_EXIT      (256)-  terminate search and exit.
//
//       DWORD  ulParam1                                  // <- input
//          This parameter will be passed as a first parameter to 
//          all callback functions.
//          This parameter could be as a typecasted pointer to some data
//          structure, which is helpful for TreeWalk().
//
// Sequence of events:
//
// PerDir      (DIR_FOUND,  root_dir,   [0]=0)
// PerDir      (DIR_DONE,   root_dir,   [0]=0)
//
//    LevelAdded  (            dir_path_1, dir_name1)
//    PerDir      (DIR_FOUND,  dir_path_1, dir_name1)
//    PerDir      (DIR_DONE,   dir_path_1, dir_name1)
//
//       LevelAdded  (            dir_path_2, dir_name2)
//       PerDir      (DIR_FOUND,  dir_path_2, dir_name2)
//       PerDir      (DIR_DONE,   dir_path_2, dir_name2)
//
//       LevelBack   (            dir_path_2)
//
//    LevelBack   (            dir_path_1)
//
//
// Note: No LevelBack is executed for the root dir.

int   TreeWalk (
         const char *pszTreeSpec,                                          // Starting tree.
         void     (*pFnLevelBack)   (DWORD, char *pathname),               // NULL|callback: -- dir level (pathname - before cutting level).
         void     (*pFnLevelAdded)  (DWORD, char *pathname, char *name),   // NULL|callback: ++ dir level (pathame - after level 'name' added).
         int      (*pFnPerDir)      (DWORD, int DirWhat, char *pathname, char *name), // NULL|dir  found callback.
         int      (*pFnPerFile)     (DWORD, char *pathname, char *name, WIN32_FIND_DATA *pData), // NULL|file found callback.
         DWORD    ulParam1                                                 // Passed to all callback functions.
         )
{
//---------------------------------------------------------------------------
   int                     iRetCode, iPerObjRetCode;
   char                    pszCurrPath[MAX_PATH_LEN];
   int                     PrevPath_len, CurrPath_len;

   // For PerDir() part.
   HANDLE                  hFindHandle;

   DIRECTORY_STRUCT        dRootDir       = {0,0,0,"."}; // "Starting" structure.
   DIRECTORY_STRUCT        *pCurrDir      = &dRootDir;  // The one, that found and where search will take place.
   DIRECTORY_STRUCT        *pLastSibling  = 0;

   char                    *pFoundDirName = "";

   WIN32_FIND_DATA         win32_find_data;   // Misc file info: data/time/attrib...
//---------------------------------------------------------------------------


   //------------------------------------------------------------------------
   // Do simple input parameters validation. These must not be NULL and
   // must have meaningful values.
   //------------------------------------------------------------------------

   if (!pszTreeSpec || !(*pszTreeSpec))
      // Invalid parameter.
      return (TW_ERROR);


   //------------------------------------------------------------------------
   // Set 'pszCurrPath'.
   //------------------------------------------------------------------------

   if (pszTreeSpec[0] == '\\' && pszTreeSpec[1] != '\\')
      strcpy (pszCurrPath, pszTreeSpec+1);    // Copy dir name.
   else
      strcpy (pszCurrPath, pszTreeSpec);

   CurrPath_len   = strlen(pszCurrPath);

   // RTrim last backslash.
   if (pszCurrPath[CurrPath_len-1] == '\\')
      {
      pszCurrPath[CurrPath_len-1] = 0;       // RTrim last backslash
      CurrPath_len--;                        //  and set 'CurrPath_len'.
      }

   PrevPath_len = CurrPath_len;


   //------------------------------------------------------------------------
   // "C:\TEST",0 -> "C:\TEST",0,0
   //
   // This will fix problem, when 'pFnPerDir()' called very first time
   // and there is some remaining garbage in 'pszCurrPath' from previous
   // calls.
   //------------------------------------------------------------------------
   pszCurrPath[CurrPath_len+1] = 0;


   /*       === Loop is doing the following: ===
   -  dir found - execute callback.
   -  findfirst/next files in current dir.
   -  execute callback for every found file.
   -  execute callback dir_done.
   -  [index--] find first/next dir in current one until normal dir found.
   -  construct name of newly found dir.
   */


   do
      {
      //---------------------------------------------------------------------
      // 'pszCurrPath', 'PrevPath_len', 'CurrPath_len' is guaranteed to 
      // be ready here.
      //
      // Find first dir...
      //---------------------------------------------------------------------

      //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
      //
      // Executing callback: 'pFnPerDir()' and processing results...

      if (pFnPerDir)
         iPerObjRetCode = pFnPerDir(ulParam1, DIR_FOUND, pszCurrPath, pszCurrPath+PrevPath_len+1);
      else
         iPerObjRetCode = TW_DEFAULT;


      //---------------------------------------------------------------------
      //       Return codes after 'pFnPerxxx()' and actions:
      // TW_DEFAULT (0)          Do file search.
      //                         Do first dir search (go in subtrees).
      //                
      // TW_NO_DEEPER (2)        NO first dir search.
      // 
      // TW_CUT_SEARCH (4)       Do not proceed search into current dir. 
      // 
      // TW_EXIT (256)           NO file search.
      //                         NO first dir search.
      //                         TERMINATE TreeWalk();.
      //---------------------------------------------------------------------

      if (
         !(iPerObjRetCode & TW_CUT_SEARCH)  &&
         !(iPerObjRetCode & TW_EXIT)
         )
         {
         //------------------------------------------------------------------
         // Do file searching...
         //------------------------------------------------------------------

         // C:\WIN => C:\WIN\*.*
         pszCurrPath[CurrPath_len] = '\\';
         strcpy (pszCurrPath + CurrPath_len + 1, "*.*");

         hFindHandle =
            FindFirstFile ( // HANDLE: INVALID_HANDLE_VALUE:fail, else:success.
               &pszCurrPath[0],  // LPCTSTR lpFileName: pointer to name of file to search for  
               &win32_find_data  // LPWIN32_FIND_DATA lpFindFileData:  pointer to returned information 
               );
         if (hFindHandle == INVALID_HANDLE_VALUE)
            iRetCode = -1;
         else
            iRetCode = 0;

         while (
            iRetCode == 0                      && 
            !(iPerObjRetCode & TW_CUT_SEARCH)  &&
            !(iPerObjRetCode & TW_EXIT)
            )
            {
            if (!(win32_find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
               {
               strcpy (pszCurrPath+CurrPath_len+1, win32_find_data.cFileName);

               //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
               //
               // Executing callback 'pFnPerFile()'.

               if (pFnPerFile)
                  iPerObjRetCode |= pFnPerFile (ulParam1, pszCurrPath, pszCurrPath+CurrPath_len+1, &win32_find_data);
               else
                  iPerObjRetCode |= TW_DEFAULT;
               //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
               }
            else
               {
               // Ok, dir found!
               //...check whether it's non'.' and save it into 
               //... dir-cache-list.
///               if (win32_find_data.cFileName[0] != '.')
               if (
                  (win32_find_data.cFileName[0] == '.' && win32_find_data.cFileName[1] == 0) ||
                  (win32_find_data.cFileName[0] == '.' && win32_find_data.cFileName[1] == '.' && win32_find_data.cFileName[2] == 0)
                  )
                  ;
               else
                  pLastSibling = AddDirectory (pCurrDir, pLastSibling, win32_find_data.cFileName);
               }
            iRetCode =
               FindNextFile ( // BOOL: 0:fail, else:success.
                  hFindHandle,      // HANDLE hFindFile:   handle to search  
                  &win32_find_data  // LPWIN32_FIND_DATA lpFindFileData:  pointer to structure for data on found file  
                  );
            iRetCode = !iRetCode;  // Make it: 0:success, else:fail.
            }
         if (hFindHandle && hFindHandle != INVALID_HANDLE_VALUE)
            FindClose (hFindHandle);

         pszCurrPath[CurrPath_len]   = 0;           // C:\WIN\xxx => C:\WIN0xxx
         pszCurrPath[CurrPath_len+1] = 0;           // C:\WIN0xxx => C:\WIN00xx


         //------------------------------------------------------------------
         // Executing callback: 'pFnPerDir (DIR_DONE)'.
         // Now it is a notification, that directory processing finished.
         // 'pFnPerDir()' with 'DIR_DONE' as a second parameter is called
         // only when file search wasn't cancelled from the previous call 
         // to 'pFnPerDir()'.
         //------------------------------------------------------------------

         if (pFnPerDir)
            iPerObjRetCode |= pFnPerDir(ulParam1, DIR_DONE, pszCurrPath, pszCurrPath+PrevPath_len+1);
         else
            iPerObjRetCode |= TW_DEFAULT;
         }

      if (iPerObjRetCode & TW_EXIT)
         {
         FreeAllDirectories (&dRootDir);
         return (TW_EXIT);
         }
      //
      //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


      //------------------------------------------------------------------
      // Find first directory if not TW_NO_DEEPER.
      //------------------------------------------------------------------

      if (!(iPerObjRetCode & TW_NO_DEEPER))
         {
         pszCurrPath[CurrPath_len] = '\\';
         strcpy (pszCurrPath + CurrPath_len + 1, "*.*");

         // Here - pull out dir name from dir-cache-list
         // "findfirst" emulation - name of first child dir under current one.
         //
         if (pCurrDir->pChild)
            {
            pFoundDirName = pCurrDir->pChild->pName;
            pCurrDir = pCurrDir->pChild;
            pLastSibling = 0; // Now search will be done from under new dir.
            iRetCode = 0;
            }
         else
            iRetCode = -1;

         pszCurrPath[CurrPath_len] = 0;
         }
      else
         iRetCode = -1;

      if (iRetCode != -1)
         iRetCode = 0;


      do
         {
         while (iRetCode != 0 && pCurrDir)
            {
            DIRECTORY_STRUCT   *pTmp;
            //---------------------------------------------------------------
            // No more (first) dirs under current one.
            // Going back to previous until found.
            //---------------------------------------------------------------

            // Up one level.
            pTmp = pCurrDir->pParent;
            if (pTmp)
               DeleteDirectory (pCurrDir);
            pCurrDir = pTmp;

            //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
            // Executing callback notification about changing of directory
            // name up one level.
            //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

            if (pFnLevelBack && pTmp)  // Do not call it for root directory.
               pFnLevelBack (ulParam1, pszCurrPath);  // Passing full pathname *before* cutting level.

            if (pCurrDir)
               {
               *strrchr (pszCurrPath, '\\') = 0;

               //------------------------------------------------------------
               // Update value of 'PrevPath_len' 
               // before changing of 'CurrPath_len'.
               //------------------------------------------------------------
               PrevPath_len = CurrPath_len;

               CurrPath_len  = strlen(pszCurrPath);

               // retrieve next dir from dir-list.
               //
               if (pCurrDir->pChild)
                  {
                  pFoundDirName = pCurrDir->pChild->pName;
                  pCurrDir = pCurrDir->pChild;
                  pLastSibling = 0; // Now search will be done from under new dir.
                  iRetCode = 0;
                  }
               else
                  iRetCode = -1;
               }
            }


         //------------------------------------------------------------------
         // Wait until normal dir found.
         // It means: 
         //    -  first char of subdir name must not be '.'.
         //    -  Found name must have subdir bit set.
         //------------------------------------------------------------------

         }
      while (iRetCode != 0 && pCurrDir);


      //---------------------------------------------------------------------
      // We're here - so either right dir found or absolutely
      // no more dirs.
      //
      // So found finally?
      //---------------------------------------------------------------------

      if (iRetCode == 0)
         {
         //------------------------------------------------------------------
         // Ok, first directory under the current one found.
         // Prepare 'pszCurrPath'.
         //------------------------------------------------------------------
         pszCurrPath[CurrPath_len] = '\\';
         strcpy (pszCurrPath+CurrPath_len+1, pFoundDirName);

         //------------------------------------------------------------------
         // Update value of 'PrevPath_len' 
         // before changing of 'CurrPath_len'.
         //------------------------------------------------------------------
         PrevPath_len = CurrPath_len;

         CurrPath_len   = strlen(pszCurrPath);

         //------------------------------------------------------------------
         // Executing callback: "one more dir level added to search path"...
         //------------------------------------------------------------------
         if (pFnLevelAdded)
            pFnLevelAdded (ulParam1, pszCurrPath, pFoundDirName);
         }
      }
   while (pCurrDir);


   FreeAllDirectories (&dRootDir);
   return (TW_SUCCESS);

}
//===========================================================================


//===========================================================================
//

DIRECTORY_STRUCT *AddDirectory (DIRECTORY_STRUCT *pParent, DIRECTORY_STRUCT *pUpSibling, char *pName)
{
//---------------------------------------------------------------------------
   DIRECTORY_STRUCT      *pNewDir;
//---------------------------------------------------------------------------

   // In a near future, when malloc's will be replaced on 'new'
   // there will be nothing to check.
   //
   pNewDir                 = (DIRECTORY_STRUCT *)malloc (sizeof(DIRECTORY_STRUCT));
   pNewDir->pParent        = 0;  
   pNewDir->pChild         = 0;
   pNewDir->pDownSibling   = 0;
   pNewDir->pName          = (char *)malloc (strlen(pName) + 1);
   strcpy (pNewDir->pName, pName);

   // tie parent to newly created dir.
   pNewDir->pParent = pParent;

   if (!(pParent->pChild))   // If parent doesn't have a child setted up....
      pParent->pChild = pNewDir; // set it to it.

   if (pUpSibling)
      (pUpSibling)->pDownSibling = pNewDir;

   return (pNewDir);
}
//===========================================================================


//===========================================================================
// Directory must be top sibling with no children.
//
void  DeleteDirectory (DIRECTORY_STRUCT *pDir)
{
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

   free (pDir->pName);

   if (pDir->pParent)
      (pDir->pParent)->pChild = pDir->pDownSibling;

   free (pDir);
}
//===========================================================================


//===========================================================================
//
// Function will free all memory upon exiting from TreeWalk().

void  FreeAllDirectories (DIRECTORY_STRUCT *pRootDir)
{
//---------------------------------------------------------------------------
   DIRECTORY_STRUCT  *pDir, *pNextTop, *pCurrDir, *pNextSibling;
//---------------------------------------------------------------------------

   // Browse through linked list and free everything.
   //
   for (pDir=pRootDir->pChild; pDir; pDir=pNextTop)
      {
      pNextTop = pDir->pChild;                  // Save next top.

      for (pCurrDir=pDir; pCurrDir; pCurrDir=pNextSibling)
         {
         pNextSibling = pCurrDir->pDownSibling; // Save next sibling.

         free (pCurrDir->pName);
         free (pCurrDir);
         }
      }
}
//===========================================================================
