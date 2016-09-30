//===========================================================================
// 
//                               FILESET.CPP
// 
// Copyright (c) 1997  MENSK Technologies Corporation. All Rights Reserved.
// 
//
//===========================================================================


// Suppress warnings, coming from Bill Gates's code.
//
#pragma  warning (disable : 4201)   // Suppress warning: "Nonstandard extension used..." (from MS *.H files.)
#pragma  warning (disable : 4514)   // Suppress warning: "Unreferenced inline function has been removed..."


///!!!#define  _WIN32_WINNT   0x0400      // So that 'ReadDirectoryChangesW' will get included.

#include <windows.h>
#include <process.h>                // Threads.
#include <conio.h>
#include <stdio.h>
#include <string.h>

#include "tg_all.h"


//---------------------------------------------------------------------------
// Local definitons.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Globals.

const char *Fileset::pVName_IncludeSpecs     =  "Include Specs";
const char *Fileset::pVName_IncludeSizeMin   =  "Include Size Min";
const char *Fileset::pVName_IncludeSizeMax   =  "Include Size Max";
const char *Fileset::pVName_ExcludeSpecs     =  "Exclude Specs";
const char *Fileset::pVName_ExcludeSizeMin   =  "Exclude Size Min";
const char *Fileset::pVName_ExcludeSizeMax   =  "Exclude Size Max";
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local function prototypes.
//---------------------------------------------------------------------------


//===========================================================================
// 
// constructor for Fileset.
// '_pFilesetKeyName' - is the FULL key *before* name of fileset.
// We have to append name of fileset to assemble the key under which
// we will store fileset values.

Fileset::Fileset (
      const char *_pFilesetName,
      HKEY        _hBaseKeyHandle,
      const char *_pFullRegistryKeyName,
      const char *_pIniSectionName,
      const char *_pIniFilename
      )
   // Set persistent storage information, so that fileset would know
   // where to pull/put it's settings from.
   : PersistentObject (
      _hBaseKeyHandle,
      _pFullRegistryKeyName,
      _pIniSectionName,
      _pIniFilename
      )
{
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

   // Set name of fileset.
   SetString (&pFilesetName, _pFilesetName);

   ppIncludeSpecs    = NULL;
   iIncludeSpecsNum  = 0;

   // By default include all sizes.
   dwIncludeSizeMin  = 0;
   dwIncludeSizeMax  = FILESIZE_INFINITE;
   dwExcludeSizeMin  = FILESIZE_INFINITE;
   dwExcludeSizeMax  = FILESIZE_INFINITE;

   ppExcludeSpecs    = NULL;
   iExcludeSpecsNum  = 0;

   // Now fileset key, name and INI filename are initialized.
   LoadObject ();             // Load object from the registry.
}
//===========================================================================

//===========================================================================
//
// Destructor.

Fileset::~Fileset()
{
   switch (GetState())
      {
      case  STATE_MODIFIED_OBJECT:     SaveObject();  break;
      case  STATE_DEAD_OBJECT:         EraseObject(); break;
      case  STATE_UNCHANGED_OBJECT:
      default:                   break;   // Do nothing.
      }

   DeleteAllSpecs ();
   DeleteString (&pFilesetName);
}
//===========================================================================

//===========================================================================
// 
/* virtual */
VOID  Fileset::LoadObject     (VOID)
{
   LoadValue_StringArray (
      pVName_IncludeSpecs,          // const char  *pValueName, 
      &ppIncludeSpecs,              // char        ***pppStrArray, 
      &iIncludeSpecsNum             // int         *piStrArraySize, 
      );
   LoadValue_Dword   (
      pVName_IncludeSizeMin,        // const char *pValueName, 
      &dwIncludeSizeMin,            // DWORD *pdwValue, 
      dwIncludeSizeMin              // DWORD dwDefault, 
      );
   LoadValue_Dword   (
      pVName_IncludeSizeMax,        // const char *pValueName, 
      &dwIncludeSizeMax,            // DWORD *pdwValue, 
      dwIncludeSizeMax              // DWORD dwDefault, 
      );

   LoadValue_StringArray (
      pVName_ExcludeSpecs,          // const char  *pValueName, 
      &ppExcludeSpecs,              // char        ***pppStrArray, 
      &iExcludeSpecsNum             // int         *piStrArraySize, 
      );
   LoadValue_Dword   (
      pVName_ExcludeSizeMin,        // const char *pValueName, 
      &dwExcludeSizeMin,            // DWORD *pdwValue, 
      dwExcludeSizeMin              // DWORD dwDefault, 
      );
   LoadValue_Dword   (
      pVName_ExcludeSizeMax,        // const char *pValueName, 
      &dwExcludeSizeMax,            // DWORD *pdwValue, 
      dwExcludeSizeMax              // DWORD dwDefault, 
      );

}
//===========================================================================

//===========================================================================
// 

/* virtual */
VOID  Fileset::SaveObject     (VOID)
{
   if (GetState() == STATE_DEAD_OBJECT)
      return;
   
   SaveValue_StringArray (
      pVName_IncludeSpecs,          // const char  *pValueName, 
      ppIncludeSpecs,               // char        **ppString, 
      iIncludeSpecsNum              // int         iStringsNum
      );
   SaveValue_Dword (
      pVName_IncludeSizeMin,        // const char *pValueName, 
      dwIncludeSizeMin              // DWORD dwValue, 
      );
   SaveValue_Dword (
      pVName_IncludeSizeMax,        // const char *pValueName, 
      dwIncludeSizeMax              // DWORD dwValue, 
      );

   SaveValue_StringArray (
      pVName_ExcludeSpecs,          // const char  *pValueName, 
      ppExcludeSpecs,               // char        **ppString, 
      iExcludeSpecsNum              // int         iStringsNum
      );
   SaveValue_Dword (
      pVName_ExcludeSizeMin,        // const char *pValueName, 
      dwExcludeSizeMin              // DWORD dwValue, 
      );
   SaveValue_Dword (
      pVName_ExcludeSizeMax,        // const char *pValueName, 
      dwExcludeSizeMax              // DWORD dwValue, 
      );
}
//===========================================================================

//===========================================================================
// 

/* virtual */
VOID  Fileset::EraseObject    (VOID)
{
   PersistentObject::EraseObject();
}
//===========================================================================

//===========================================================================
const char  **Fileset::GetSpecs     (BOOL bInclude)
{
   if (bInclude)
      return ((const char **)ppIncludeSpecs);
   else
      return ((const char **)ppExcludeSpecs);
}
//===========================================================================

//===========================================================================
int   Fileset::GetSpecsNum    (BOOL bInclude)
{
   if (bInclude)
      return (iIncludeSpecsNum);
   else
      return (iExcludeSpecsNum);
}
//===========================================================================

//===========================================================================
//
// 'bInclude'=TRUE  => include spec.
// 'bInclude'=FALSE => exclude spec.

BOOL  Fileset::AddSpec        (BOOL bInclude, const char *pszSpec)
{
//---------------------------------------------------------------------------
   BOOL           bRetCode;
//---------------------------------------------------------------------------


   if (FindSpec (bInclude, pszSpec))
      return (TRUE); // Already existed.
   if (FindSpec (!bInclude, pszSpec))
      // File spec already exists.
      return (TRUE);

   if (bInclude)
      bRetCode = AddStringToArray (&ppIncludeSpecs, &iIncludeSpecsNum, pszSpec);
   else
      bRetCode = AddStringToArray (&ppExcludeSpecs, &iExcludeSpecsNum, pszSpec);


   return (bRetCode);
}
//===========================================================================

//===========================================================================

VOID  Fileset::AddSpecs       (BOOL bInclude, const char **ppSpecs, int iSpecsNum)
{
//---------------------------------------------------------------------------
   int            i;
//---------------------------------------------------------------------------

   for (i=0; i<iSpecsNum; i++)
      AddSpec (bInclude, ppSpecs[i]);
}
//===========================================================================

//===========================================================================
//
// 

VOID  Fileset::DeleteSpec      (BOOL bInclude, const char *pszSpec)
{
   if (bInclude)
      DeleteStringFromArray (&ppIncludeSpecs, &iIncludeSpecsNum, pszSpec);
   else
      DeleteStringFromArray (&ppExcludeSpecs, &iExcludeSpecsNum, pszSpec);
}
//===========================================================================

//===========================================================================
//

VOID  Fileset::DeleteAllSpecs  (VOID)
{
   DeleteStringArray (&ppIncludeSpecs, &iIncludeSpecsNum);
   DeleteStringArray (&ppExcludeSpecs, &iExcludeSpecsNum);
}
//===========================================================================

//===========================================================================
//
// Returns 0-based spec element.

const char  *Fileset::GetSpecElement (BOOL bInclude, int i)
{
//---------------------------------------------------------------------------
   int            iSpecsNum;
   char           **ppSpecs;
//---------------------------------------------------------------------------

   if (bInclude)
      {
      iSpecsNum = iIncludeSpecsNum;
      ppSpecs   = ppIncludeSpecs;
      }
   else
      {
      iSpecsNum = iExcludeSpecsNum;
      ppSpecs   = ppExcludeSpecs;
      }

   if (i >= iSpecsNum)
      return (NULL);

   return (ppSpecs[i]);
}
//===========================================================================

//===========================================================================
//
// 

BOOL  Fileset::FindSpec        (BOOL bInclude, const char *pszSpec)
{
//---------------------------------------------------------------------------
   int            iSpecsNum;
   char           **ppSpecs;
//---------------------------------------------------------------------------

   if (bInclude)
      {
      iSpecsNum = iIncludeSpecsNum;
      ppSpecs   = ppIncludeSpecs;
      }
   else
      {
      iSpecsNum = iExcludeSpecsNum;
      ppSpecs   = ppExcludeSpecs;
      }

   for (int i=0; i<iSpecsNum; i++)
      if (strcmpi (pszSpec, ppSpecs[i]) == 0)
         return (TRUE);

   return (FALSE);
}
//===========================================================================

//===========================================================================
//
// Checks whether filename matches to the set defined specifications.

BOOL  Fileset::SpecMatch   (const char *pszFilename)
{
//---------------------------------------------------------------------------
   int            i;
   const char     *pSpec;
//---------------------------------------------------------------------------

   if (!pszFilename)
      return (FALSE);

   // Check first whether is in "exclude" specs list...
   for (i=0, pSpec=GetSpecElement(FALSE, 0); pSpec; i++, pSpec=GetSpecElement(FALSE, i))
      if (::SpecMatch (pSpec, pszFilename))
         return FALSE;  // In one of "exclude" specs...

   // Check first whether is in "include" specs list...
   for (i=0, pSpec=GetSpecElement(TRUE, 0); pSpec; i++, pSpec=GetSpecElement(TRUE, i))
      if (::SpecMatch (pSpec, pszFilename))
         return TRUE;   // In one of "include" specs...

   // In neither...
   return (FALSE);
}
//===========================================================================


//===========================================================================
//
// Checks whether input file size is within range of sizes of this fileset.
// 'dwFilesizeLow' - lower DWORD of filesize.

BOOL  Fileset::SizeMatch   (DWORD   dwFileSizeLow)
{
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
   
   // Is within "exclude" range?
   if (dwFileSizeLow >= dwExcludeSizeMin && dwFileSizeLow <= dwExcludeSizeMax)
      return (FALSE);

   // Is within "exclude" range?
   if (dwFileSizeLow >= dwIncludeSizeMin && dwFileSizeLow <= dwIncludeSizeMax)
      return (TRUE);

   return (FALSE);
}
//===========================================================================
