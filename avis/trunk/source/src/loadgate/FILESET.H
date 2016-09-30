//===========================================================================
// 
//                               FILESET.H
// 
// 
// 
//===========================================================================



#ifndef  _FILESET_H_
#define  _FILESET_H_


//---------------------------------------------------------------------------
// Forward declarations.
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Definitions.
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Classes.

//====================================
//

#define  FILESIZE_INFINITE    (0xFFFFFFFF)

class Fileset : public PersistentObject
{
private:
   char  *pFilesetName;                            // "My Documents".

   static const char *pVName_IncludeSpecs;         // "Include Specs"
   char  **ppIncludeSpecs;
   int   iIncludeSpecsNum;
   static const char *pVName_IncludeSizeMin;       // "Include Size Min"
   DWORD dwIncludeSizeMin;                         // Allows to specify range of filesizes to *include*.
   static const char *pVName_IncludeSizeMax;       // "Include Size Max"
   DWORD dwIncludeSizeMax;

   static const char *pVName_ExcludeSpecs;         // "Exclude Specs"
   char  **ppExcludeSpecs;
   int   iExcludeSpecsNum;
   static const char *pVName_ExcludeSizeMin;       // "Exclude Size Min"
   DWORD dwExcludeSizeMin;                         // Allows to specify range of filesizes to *include*.
   static const char *pVName_ExcludeSizeMax;       // "Exclude Size Max"
   DWORD dwExcludeSizeMax;

public:
   Fileset  (
      const char *_pszFilesetName,
      HKEY        _hBaseKeyHandle,
      const char *_pFullRegistryKeyName, 
      const char *_pIniSectionName,
      const char *_pIniFilename
      );
   ~Fileset ();

   Fileset     &operator = (Fileset &fs)
      {
      // Warning!!! 
      // this code does NOT copy the fileset name and registry/ini file stuff!
      DWORD    dwMin, dwMax;
      AddSpecs (TRUE,  fs.GetSpecs (TRUE),  fs.GetSpecsNum (TRUE));
      AddSpecs (FALSE, fs.GetSpecs (FALSE), fs.GetSpecsNum (FALSE));
      fs.GetIncludeSizes (&dwMin, &dwMax);
      SetIncludeSizes    (dwMin,  dwMax);
      fs.GetExcludeSizes (&dwMin, &dwMax);
      SetExcludeSizes    (dwMin,  dwMax);
      
      return (*this);
      }
   char        *GetFilesetName         (VOID)   { return (pFilesetName);      }
   virtual VOID   LoadObject           (VOID);
   virtual VOID   SaveObject           (VOID);
   virtual VOID   EraseObject          (VOID);
   const char  **GetSpecs              (BOOL bInclude);
   int         GetSpecsNum             (BOOL bInclude);
   BOOL        AddSpec                 (BOOL bInclude, const char *pszSpec);
   VOID        AddSpecs                (BOOL bInclude, const char **ppSpecs, int iSpecsNum);
   VOID        DeleteSpec              (BOOL bInclude, const char *pszSpec);
   VOID        DeleteAllSpecs          (VOID);
   const char  *GetSpecElement         (BOOL bInclude, int i);
   BOOL        FindSpec                (BOOL bInclude, const char *pszSpec);
   BOOL        SpecMatch               (const char *pszFilename);
   VOID        GetIncludeSizes         (DWORD *pdwIncludeSizeMin, DWORD *pdwIncludeSizeMax)
                                          { 
                                          *pdwIncludeSizeMin = dwIncludeSizeMin;
                                          *pdwIncludeSizeMax = dwIncludeSizeMax;
                                          }
   VOID        SetIncludeSizes         (DWORD _dwIncludeSizeMin, DWORD _dwIncludeSizeMax)
                                          { 
                                          dwIncludeSizeMin = _dwIncludeSizeMin;
                                          dwIncludeSizeMax = _dwIncludeSizeMax;
                                          }
   VOID        GetExcludeSizes         (DWORD *pdwExcludeSizeMin, DWORD *pdwExcludeSizeMax)
                                          { 
                                          *pdwExcludeSizeMin = dwExcludeSizeMin;
                                          *pdwExcludeSizeMax = dwExcludeSizeMax;
                                          }
   VOID        SetExcludeSizes         (DWORD _dwExcludeSizeMin, DWORD _dwExcludeSizeMax)
                                          { 
                                          dwExcludeSizeMin = _dwExcludeSizeMin;
                                          dwExcludeSizeMax = _dwExcludeSizeMax;
                                          }
   BOOL        SizeMatch               (DWORD dwFileSizeLow);
};
//====================================

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Function prototypes.
//---------------------------------------------------------------------------


#endif   // _FILESET_H_
