//===========================================================================
// 
//                               BROWSER.H
// 
// 
// 
//===========================================================================



#ifndef  _BROWSER_H_
#define  _BROWSER_H_


//---------------------------------------------------------------------------
// Forward declarations.
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Definitions.
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Classes.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Function prototypes.

extern BOOL  AllocateFileInfoList (
         const char  *pDirName,        // Starting directory name.
         int         iMaxFileToSearch, // -1(unlimited):max number of files to return.
         Fileset     *pFileset,        // Initialized fileset.
         BOOL        bIncludeSubdirs,  // TRUE:=>Include subdirs in search.
         FILE_INFO   ***pppFileInfos,  // Ptr to an array of pointers to 'FILE_INFO' structure.
         int         *piFileInfosNum,  // Number of elements in array.
         char        **ppLastErrorText /*=NULL*/ // Ptr to the "reason for error" text description.
         );
extern VOID  FreeFileInfoList (
         FILE_INFO   ***pppFileInfos,  // Ptr to an array of pointers to 'FILE_INFO' structure.
         int         *piFileInfosNum   // Number of elements in array.
         );
//---------------------------------------------------------------------------


#endif   // _BROWSER_H_
