//===========================================================================
// 
//                               PERSOBJ.H
// 
// 
// 
//===========================================================================



#ifndef  _PERSOBJ_H_
#define  _PERSOBJ_H_


//---------------------------------------------------------------------------
// Forward declarations.
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Definitions.
                                          // States of object:
#define  STATE_UNCHANGED_OBJECT     0     // Discard all changes:                   Ignore all changes made to the object.
#define  STATE_MODIFIED_OBJECT      1     // Save object parameters in registry:    Update object's information into the registry.
#define  STATE_DEAD_OBJECT          2     // Erase object reference from registry:  Object's references must be deleted from the registry.
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Classes.

//====================================
// Base class for any savable object, be that into 
// registry or into INI file.

#define  USE_STORAGE_REGISTRY    1
#define  USE_STORAGE_INIFILE     2

class PersistentObject
{
private:
   HKEY  hBaseKeyHandle;         // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
   char  *pFullRegistryKeyName;
   char  *pIniSectionName;
   char  *pIniFilename;

   int   iState;                 // STATE_UNCHANGED_OBJECT - discard all changes
                                 // STATE_MODIFIED_OBJECT  - save object parameters in registry. 
                                 // STATE_DEAD_OBJECT      - erase object reference from registry.
public:
   PersistentObject  (
      HKEY        _hBaseKeyHandle,
      const char *_pFullRegistryKeyName,
      const char *_pIniSectionName,
      const char *_pIniFilename
      )
      {
      SetPersistentStorage (_hBaseKeyHandle, _pFullRegistryKeyName, _pIniSectionName, _pIniFilename);
      SetState             (STATE_MODIFIED_OBJECT);   // To allow it to get saved on delete.
      }

   ~PersistentObject ()
      {
      DeleteString (&pFullRegistryKeyName);
      DeleteString (&pIniSectionName);
      DeleteString (&pIniFilename);
      }

   VOID   SetPersistentStorage (
      HKEY        _hBaseKeyHandle,
      const char *_pFullRegistryKeyName,
      const char *_pIniSectionName,
      const char *_pIniFilename
      )
      {
      hBaseKeyHandle = _hBaseKeyHandle;
      SetString   (&pFullRegistryKeyName, _pFullRegistryKeyName);
      SetString   (&pIniSectionName,      _pIniSectionName);
      SetString   (&pIniFilename,         _pIniFilename);
      }

   HKEY  GetBaseKeyHandle              (VOID)         { return (hBaseKeyHandle);       }
   char  *GetFullRegistryKeyName       (VOID)         { return (pFullRegistryKeyName); }
   char  *GetIniSectionName            (VOID)         { return (pIniSectionName);      }
   char  *GetIniFilename               (VOID)         { return (pIniFilename);         }

   // Set/Save/Erase must be called by derived objects at their convenience.
   virtual VOID   LoadObject           (VOID) = 0; // Load from registry.
   virtual VOID   SaveObject           (VOID) = 0; // Save to registry.
   virtual VOID   EraseObject          (DWORD dwFlags = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE))
      { 
      if (dwFlags & USE_STORAGE_REGISTRY) EraseFullRegistryKeyName();   
      if (dwFlags & USE_STORAGE_INIFILE)  EraseIniSectionName();   
      }

   virtual int    GetState             (VOID)         { return (iState);               }
   virtual VOID   SetState             (int iNewVal)  { iState = iNewVal;              }

   BOOL  LoadValue_Bool                (const char *pValueName, BOOL *pbValue, BOOL bDefault, DWORD dwFlags = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE));
   BOOL  SaveValue_Bool                (const char *pValueName, BOOL bValue, DWORD dwFlags = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE));
   BOOL  LoadValue_Int                 (const char *pValueName, int *piValue, int iDefault, DWORD dwFlags = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE));
   BOOL  SaveValue_Int                 (const char *pValueName, int iValue, DWORD dwFlags = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE));
   BOOL  LoadValue_Dword               (const char *pValueName, DWORD *pdwValue, DWORD dwDefault, DWORD dwFlags = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE));
   BOOL  SaveValue_Dword               (const char *pValueName, DWORD dwValue, DWORD dwFlags = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE));
   BOOL  LoadValue_Filetime            (const char *pValueName, FILETIME *pftToLoad, FILETIME *pftDefault, DWORD dwFlags = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE));
   BOOL  SaveValue_Filetime            (const char *pValueName, FILETIME *pftValue, DWORD dwFlags = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE));
   BOOL  LoadValue_Systemtime          (const char *pValueName, SYSTEMTIME *pstToLoad, SYSTEMTIME *pstDefault, DWORD dwFlags = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE));
   BOOL  SaveValue_Systemtime          (const char *pValueName, SYSTEMTIME *pstValue, DWORD dwFlags = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE));
   BOOL  LoadValue_String              (const char *pValueName, char *pDestStr, int iDestStrSize, char *pStrDefault, DWORD dwFlags = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE));
   BOOL  SaveValue_String              (const char *pValueName, char *pStr, DWORD dwFlags = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE));
   BOOL  LoadValue_StringArray         (const char *pValueName, char ***pppStrArray, int *piStrArraySize, DWORD dwFlags = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE));
   BOOL  SaveValue_StringArray         (const char *pValueName, char **ppStr, int iStringsNum, DWORD dwFlags = (USE_STORAGE_REGISTRY|USE_STORAGE_INIFILE));
   VOID  EraseFullRegistryKeyName      (VOID);   
   VOID  EraseIniSectionName           (VOID);   
};
//====================================

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Function prototypes.
//---------------------------------------------------------------------------


#endif   // _PERSOBJ_H_
