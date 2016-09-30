#ifndef PROTECTIONPROVIDER_UTIL
#define PROTECTIONPROVIDER_UTIL

#include "stdafx.h"
#include <list>
#include <comcat.h>
#include "tstring.h"


typedef std::list<CLSID> CLSIDList;
typedef std::list<tstring::tstring> StringList;
struct ProcessInfo
{
	DWORD id;
	tstring::tstring imageBaseFilename;
	tstring::tstring imageFullPathname;
};
typedef std::list<ProcessInfo> ProcessInfoList;
struct ModuleInfo
{
	tstring::tstring	baseFilename;
	tstring::tstring	fullPathname;
};
typedef std::list<ModuleInfo> ModuleInfoList;
typedef std::list<tstring::tstring> StringList;


// ** CONSTANTS **
const SYSTEMTIME SYSTEMTIME_NULL = {0,0,0,0,0,0,0,0};


// ** FUNCTIONS **
// Sets *components equal to a list of the CLSIDs in the specified component category
// Returns S_OK on success else the error code of the failure
HRESULT EnumCategoryComponents( CATID componentCategory, CLSIDList* components );
// Appends a list of all product codes installed with the specified upgrade code to *productCodes
HRESULT GetProductsWithUpgradeCode( LPCTSTR upgradeCode, DWORD dwMinMajorVer, DWORD dwMinMinorVer, DWORD dwMaxMajorVer, DWORD dwMaxMinorVer, StringList* productCodes );
// Sets *moduleInfos equal to a list of moduleInfos on for the process specified by processID
HRESULT GetProcessModules( DWORD processID, ModuleInfoList* moduleInfos );
// Sets *thisProcessInfo equal to basic information on the first process found whose base filename matches the specified filename
HRESULT GetProcessInfo( LPCTSTR processBaseFilename, ProcessInfo* thisProcessInfo );
// Reads the specified DWORD value into valueData, or sets to defaultValue if not present/not DWORD/any error
// Returns S_OK if successfully read in, S_FALSE if not present, HFW32(ERROR_DATATYPE_MISMATCH) if value exists but is
// of wrong type, else the error code of the failure
// *valueData is ALWAYS set as specified in the beginning regardless of the return value
HRESULT RegQueryDword( HKEY keyHandle, LPCTSTR valueName, DWORD* valueData, DWORD defaultValue );
HRESULT RegQueryInt( HKEY keyHandle, LPCTSTR valueName, int* valueData, int defaultValue );
HRESULT RegQueryBool( HKEY keyHandle, LPCTSTR valueName, bool* valueData, bool defaultValue );
HRESULT RegQueryBOOL( HKEY keyHandle, LPCTSTR valueName, BOOL* valueData, BOOL defaultValue );
HRESULT RegQueryVARIANTBOOL( HKEY keyHandle, LPCTSTR valueName, VARIANT_BOOL* valueData, BOOL defaultValue );
HRESULT RegQueryString( HKEY keyHandle, LPCTSTR valueName, LPTSTR valueData, DWORD valueDataSize, LPCTSTR defaultValue );
HRESULT RegQueryString( HKEY keyHandle, LPCTSTR valueName, tstring::tstring* valueData, LPCTSTR defaultValue );
HRESULT RegQueryString( HKEY keyHandle, LPCTSTR valueName, BSTR* valueData, LPCTSTR defaultValue );
// Sets *pathBuffer equal to the full pathname to the SAV directory with no terminating
// backslash.
HRESULT GetSavDirectory( tstring::tstring* pathBuffer );
// Returns TRUE if filename exists, else FALSE
bool FileExists( const tstring::tstring& filename );
// Sets *subkeyNames equal to a list of all keys under parentKeyHandle\keyPath.  keyPath is an optional subkey path.
HRESULT GetKeySubkeys( HKEY parentKeyHandle, LPCTSTR keyPath, StringList* subkeyNames );


// ** CLASSES **
// A simple performance timer for debugging
class CSimplePerformanceTimer
{
public:
    CSimplePerformanceTimer();
    ~CSimplePerformanceTimer();

    void StartTimer();
    void StopTimer();
    DWORD GetElapsedTime( void );
    void OutputElapsedTime( LPCTSTR timerDescription );
private:
    DWORD startTime;
    DWORD stopTime;
};


// ** TEMPLATE DEFINITIONS **
template<class T> HRESULT CreateComponents( CLSIDList componentIDs, std::list<T*>* components )
// Sets *components equal to a list of components created with the T interface
// Returns S_OK on success, S_FALSE if nothing to do, else the error code of the failure
{
    CLSIDList::iterator         currComponentID;
    CComPtr<T>                  currComponentInterface;
    CComPtr<ICatInformation>    categoryManager;
    CComPtr<IEnumGUID>          componentGUIDs;
    CLSID                       currCLSID           = GUID_NULL;
    ULONG                       noFetched           = 0;
    CComBSTR                    shortDescription;
    HRESULT                     returnValHR         = E_FAIL;

    // Validate parameters
    if (components == NULL)
        return E_POINTER;

    if (componentIDs.size() > 0)
    {
        for (currComponentID = componentIDs.begin(); currComponentID != componentIDs.end(); currComponentID++)
        {
            returnValHR = currComponentInterface.CoCreateInstance(*currComponentID);
            if (SUCCEEDED(returnValHR))
            {
                try
                {
                    components->push_back(currComponentInterface.Detach());
                }
                catch(std::bad_alloc&)
                {
                    returnValHR = E_OUTOFMEMORY;
                    break;
                }
            }
        }
    }
    else
    {
        returnValHR = S_FALSE;
    }

    return returnValHR;
}

// Temporary stub class to resolve C2664 in ATL's _CopyInterface function due to const mismatch
// with copy member signature.
template<class T>
class CopyInterfaceFixed
{
public:
	static HRESULT copy(T** p1, T* const* p2)
	{
		ATLENSURE(p1 != NULL && p2 != NULL);
		*p1 = *p2;
		if (*p1)
			(*p1)->AddRef();
		return S_OK;
	}
	static void init(T** ) {}
	static void destroy(T** p) {if (*p) (*p)->Release();}
};

#endif // PROTECTIONPROVIDER_UTIL