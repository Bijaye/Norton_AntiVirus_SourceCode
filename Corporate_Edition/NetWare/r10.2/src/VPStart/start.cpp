// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#define IncDebugParser
#define DEF_REG
#define NETDB_USE_INTERNET
#include "pscan.h"
#include "nlmui.h"
#include "status.h"
#include "slash.h"
#include "SAVSecureCA.h"
#include "ScsCommsUtils.h"
#include <nwnamspc.h>
#include <nwfileio.h>
#include "password.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"
#include "NetwareInstaller.h"
#include "ConnectionlessMessage.h"
#include <arpa\inet.h>

// Following macro is required by NetWare's netdb.h
NETDB_DEFINE_CONTEXT
NETINET_DEFINE_CONTEXT

// ---------------------------------------
#define TRACE_LOG  "SavTrace.log"
#define WIN32_CLIENT_INSTALL_DIR  "Clt-inst" sSLASH "WIN32"
// ---------------------------------------

extern "C" FILE *TraceFile=NULL;  // installation trace log file  gdf sts 337273 03/09/01

extern "C" int offSafetyChecks = FALSE;   // gdf sts 337273 03/07/01

BOOL StrNEqual(char *s1,char *s2,int len);
int CheckNewerFileTime(char *to,char *from);
DWORD ParseRequiredCopyLine(char *to,char *from);

void UnloadModule(char *name);   // gdf 05/04/00

int UpdateRtvscan();

extern "C" char Running=1;
extern "C" char ProgramDir[IMAX_PATH];
extern "C" char HomeDir[IMAX_PATH] = "";
extern "C" int TGID=0;
extern "C" int ThreadID=0;
extern "C" int NWversion=0;
extern "C" int NWsubVersion=0;
extern "C" HKEY hMainKey=0;
extern "C" BOOL RegFunctionsLoaded = FALSE;

extern "C" BOOL gZero=0;

extern "C" int CLibVersion=0;
extern "C" DWORD ErrorCount=0;

extern "C" FILE_SERV_INFO g_fsinf={0};

extern "C" char FileServerName[50] = "";        // server where rtvscan is running
extern "C" char *load[MAX_LOAD] = {0};
extern "C" char VolumeName[64] = "SYS";
extern "C" BOOL RunningFromHome = FALSE; // this is set to true if running from VPHOME instead of SYS:SYSTEM
extern "C" char ThisFile[IMAX_PATH] = "";
extern "C" char *DoInf[MAX_INF] = {0};
extern "C" char *UpdateInf[MAX_INF] = {0};

DWORD DeleteFilesNow();
DWORD DeleteTree(char *szDir);

DWORD ResetCommandBlock();
DWORD CheckCopyServerCommand();
DWORD UpdateUpgradeStatus(DWORD status);
DWORD UpdateUpgradeError(DWORD error, char* ErrMsg);
extern "C" HKEY hRemote;
extern "C" HKEY hError;

void NewSection(char* section);

static BOOL CheckVpdata();

// Arguments for InstallUIhandler (userParam)
struct InstallUIhandlerArguments
{
    NetwareInstaller::tcstring      logFilename;
    CConnectionlessMessage*         statusUpdater;
};
// Displays the specified message on the install UI
void InstallUIhandler( NetwareInstaller::StatusCallbackFunctionEventType eventType, void* eventParam, void* userParam );
NetwareInstaller* SAVSecureCAdprintfInterfaceInstaller = NULL;

/****************************************************************************************/
#define REG_CREATED_NEW_KEY         (0x00000001L)   // New Registry Key created
#define REG_OPENED_EXISTING_KEY     (0x00000002L)   // Existing Key opened


LONG VPStartOldVPRegCompatibleRegCreateKeyEx (
    HKEY hKey,
    LPCSTR lpSubKey,
    DWORD Reserved,
    LPSTR lpClass,
    DWORD dwOptions,
    REGSAM samDesired,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    PHKEY phkResult,
    LPDWORD lpdwDisposition
    )
{
    UNREFERENCED_PARAMETER( Reserved );
    UNREFERENCED_PARAMETER( lpClass );
    UNREFERENCED_PARAMETER( dwOptions );
    UNREFERENCED_PARAMETER( samDesired );
    UNREFERENCED_PARAMETER( lpSecurityAttributes );

    DWORD dwRet = RegOpenKey( hKey, lpSubKey, phkResult );

    if( dwRet != ERROR_SUCCESS )
    {
        dwRet = RegCreateKey( hKey, lpSubKey, phkResult );

        if( lpdwDisposition != NULL )
            *lpdwDisposition = REG_CREATED_NEW_KEY;
    }
    else
    {
        if( lpdwDisposition != NULL )
            *lpdwDisposition = REG_OPENED_EXISTING_KEY;
    }

    return( dwRet );
}
/****************************************************************************************/
DWORD FindAndSetHomeDir(void)
{
	FILE *file = fopen(NW_SYSTEM_DIR sSLASH "VPDIR.DAT","rt");

    if ( file )
    {
        fgets(HomeDir,IMAX_PATH,file);
        if ( HomeDir[strlen(HomeDir)-1] == '\n' )
            HomeDir[strlen(HomeDir)-1] = 0;
        fclose(file);
    }
    else
    {
        // search all search for special file "NAVTAG.DAT"
//              if (!found)
        StrCopy(HomeDir,DEFAULT_HOME_DIR);
    }

    iprintf(LS(IDS_FOUND_HOME_DIR),HomeDir);

    return chdir(HomeDir);
}
/****************************************************************************************/
DWORD LoadRegistryFunctions(int compatibility)
{

    if ( (RegOpenKey = (tRegOpenKey)ImportSymbol(GetNLMHandle(),"vpRegOpenKey")) == NULL ) return IDS_ERR_OPEN_REGDB;
    if ( (RegCreateKey = (tRegCreateKey)ImportSymbol(GetNLMHandle(),"vpRegCreateKey")) == NULL ) return IDS_ERR_OPEN_REGDB;
    if ( (RegCreateKeyEx = (tRegCreateKeyEx)ImportSymbol(GetNLMHandle(),"vpRegCreateKeyEx")) == NULL)
    {
        if( !compatibility ) return IDS_ERR_OPEN_REGDB;
        else RegCreateKeyEx =  VPStartOldVPRegCompatibleRegCreateKeyEx;
    }
    if ( (RegDeleteKey = (tRegDeleteKey)ImportSymbol(GetNLMHandle(),"vpRegDeleteKey")) == NULL ) return IDS_ERR_OPEN_REGDB;
    if ( (RegDeleteValue = (tRegDeleteValue)ImportSymbol(GetNLMHandle(),"vpRegDeleteValue")) == NULL ) return IDS_ERR_OPEN_REGDB;
    if ( (RegCloseKey = (tRegCloseKey)ImportSymbol(GetNLMHandle(),"vpRegCloseKey")) == NULL ) return IDS_ERR_OPEN_REGDB;
    if ( (RegSetValueEx = (tRegSetValueEx)ImportSymbol(GetNLMHandle(),"vpRegSetValueEx")) == NULL ) return IDS_ERR_OPEN_REGDB;
    if ( (RegQueryValueEx = (tRegQueryValueEx)ImportSymbol(GetNLMHandle(),"vpRegQueryValueEx")) == NULL ) return IDS_ERR_OPEN_REGDB;
    if ( (RegEnumValue = (tRegEnumValue)ImportSymbol(GetNLMHandle(),"vpRegEnumValue")) == NULL ) return IDS_ERR_OPEN_REGDB;
    if ( (RegEnumKeyEx = (tRegEnumKeyEx)ImportSymbol(GetNLMHandle(),"vpRegEnumKeyEx")) == NULL ) return IDS_ERR_OPEN_REGDB;
    if ( (RegEnumKey = (tRegEnumKey)ImportSymbol(GetNLMHandle(),"vpRegEnumKey")) == NULL ) return IDS_ERR_OPEN_REGDB;
    if ( (RegFlushKey = (tRegFlushKey)ImportSymbol(GetNLMHandle(),"vpRegFlushKey")) == NULL ) return IDS_ERR_OPEN_REGDB;
    if ( (RegNotifyChangeKeyValue = (tRegNotifyChangeKeyValue)ImportSymbol(GetNLMHandle(),"vpRegNotifyChangeKeyValue")) == NULL ) return IDS_ERR_OPEN_REGDB;
    if ( (DuplicateHandle = (tDuplicateHandle)ImportSymbol(GetNLMHandle(),"vpDuplicateHandle")) == NULL ) return IDS_ERR_OPEN_REGDB;
    if ( (RegOpenKeyEx = (tRegOpenKeyEx)ImportSymbol(GetNLMHandle(),"vpRegOpenKeyEx")) == NULL ) return IDS_ERR_OPEN_REGDB;

    if ( (RegCloseDatabase = (tRegCloseDatabase)ImportSymbol(GetNLMHandle(),"RegCloseDatabase")) == NULL ) return IDS_ERR_OPEN_REGDB;
    if ( (RegOpenDatabase = (tRegOpenDatabase)ImportSymbol(GetNLMHandle(),"RegOpenDatabase")) == NULL ) return IDS_ERR_OPEN_REGDB;
    if ( (RegSaveDatabase = (tRegSaveDatabase)ImportSymbol(GetNLMHandle(),"RegSaveDatabase")) == NULL ) return IDS_ERR_OPEN_REGDB;
    if ( (RawDumpDatabase = (tRawDumpDatabase)ImportSymbol(GetNLMHandle(),"RawDumpDatabase")) == NULL ) return IDS_ERR_OPEN_REGDB;
    if ( (DumpFileNum = (tDumpFileNum)ImportSymbol(GetNLMHandle(),"DumpFileNum")) == NULL ) return IDS_ERR_OPEN_REGDB;
    if ( (FileDumpDatabase = (tFileDumpDatabase)ImportSymbol(GetNLMHandle(),"FileDumpDatabase")) == NULL ) return IDS_ERR_OPEN_REGDB;

    RegFunctionsLoaded=TRUE;

    return ERROR_SUCCESS;
}
/**********************************************************************************************/
DWORD UnloadRegistryFunctions(void)
{

    if ( !RegFunctionsLoaded )
        return ERROR_SUCCESS;

    UnimportSymbol(GetNLMHandle(),"vpRegOpenKey");
    UnimportSymbol(GetNLMHandle(),"vpRegCreateKey");
    UnimportSymbol(GetNLMHandle(),"vpRegCreateKeyEx");
    UnimportSymbol(GetNLMHandle(),"vpRegDeleteKey");
    UnimportSymbol(GetNLMHandle(),"vpRegDeleteValue");
    UnimportSymbol(GetNLMHandle(),"vpRegCloseKey");
    UnimportSymbol(GetNLMHandle(),"vpRegSetValueEx");
    UnimportSymbol(GetNLMHandle(),"vpRegQueryValueEx");
    UnimportSymbol(GetNLMHandle(),"vpRegEnumValue");
    UnimportSymbol(GetNLMHandle(),"vpRegEnumKeyEx");
    UnimportSymbol(GetNLMHandle(),"vpRegEnumKey");
    UnimportSymbol(GetNLMHandle(),"vpRegFlushKey");
    UnimportSymbol(GetNLMHandle(),"vpRegNotifyChangeKeyValue");
    UnimportSymbol(GetNLMHandle(),"vpDuplicateHandle");
    UnimportSymbol(GetNLMHandle(),"vpRegOpenKeyEx");

    UnimportSymbol(GetNLMHandle(),"RegCloseDatabase");
    UnimportSymbol(GetNLMHandle(),"RegOpenDatabase");
    UnimportSymbol(GetNLMHandle(),"RegSaveDatabase");
    UnimportSymbol(GetNLMHandle(),"RawDumpDatabase");
    UnimportSymbol(GetNLMHandle(),"DumpFileNum");
    UnimportSymbol(GetNLMHandle(),"FileDumpDatabase");
    RegOpenKey = NULL;              RegCreateKey = NULL;    RegDeleteKey = NULL;            RegDeleteValue = NULL;
    RegCloseKey = NULL;             RegSetValueEx = NULL;   RegQueryValueEx = NULL;         RegEnumValue = NULL;
    RegEnumKeyEx = NULL;    RegEnumKey = NULL;              RegFlushKey = NULL;                     RegNotifyChangeKeyValue = NULL;
    DuplicateHandle = NULL; RegOpenKeyEx = NULL;    RegCloseDatabase = NULL;        RegOpenDatabase = NULL;
    RegSaveDatabase = NULL; RawDumpDatabase = NULL; DumpFileNum = NULL;                     FileDumpDatabase = NULL;
	RegCreateKeyEx = NULL;

    RegFunctionsLoaded = FALSE;

    return ERROR_SUCCESS;
}
/*****************************************************************************************************/
// Checks the dates of the NLMs in the system directory.  If there are newer NLMs in the VPHOME
//directory then it copies the newer version over.  For the REgistry nlm itself, it unloads it first
DWORD CheckRegistryDates()
{
    char path[IMAX_PATH];
    char path2[IMAX_PATH];
    char *name=NULL;

// check the Registry NLM
	sssnprintf(path,sizeof(path),NW_SYSTEM_DIR sSLASH REGISTRY_NLM);
	sssnprintf(path2,sizeof(path2),"%s" sSLASH REGISTRY_NLM,HomeDir);
    switch ( CheckNewerFileTime(path,path2) )
    {
        case 1:         // file in system dir is newer (backrev)
        case -1:        // file in home dir is newer (update)
            if ( FindNLMHandle( REGISTRY_NLM ) )
                system("UNLOAD "REGISTRY_NLM);
            iprintf(LS(IDS_COPY_VPREG));

            //port from 1068430 in SAV version 9.0-MR
            // NetWare 6.5 support pack 2 requires a moment to reset the exported symbols after the UNLOAD VPREG.NLM command
            delay(500);

            MyCopyFile(path2,path);
            break;
    }

// check the RegEdit NLM
	sssnprintf(path,sizeof(path),NW_SYSTEM_DIR sSLASH REGEDIT_NLM);
	sssnprintf(path2,sizeof(path2),"%s" sSLASH REGEDIT_NLM,HomeDir);
    switch ( CheckNewerFileTime(path,path2) )
    {
        case 1:
        case -1:

            iprintf(LS(IDS_COPY_VPREGEDT));
            MyCopyFile(path2,path);
            break;
    }

// check the Start NLM
	name = StrRChar(ThisFile,cSLASH);
    if ( name )
        name++;
    else
        name = ThisFile;
	sssnprintf(path,sizeof(path),NW_SYSTEM_DIR sSLASH "%s",name);
	sssnprintf(path2,sizeof(path2),"%s" sSLASH "%s",HomeDir,name);
    switch ( CheckNewerFileTime(path,path2) )
    {
        case 1:
        case -1:
            iprintf(LS(IDS_COPY_NEW_FILE),name);
            MyCopyFile(path2,path);
            break;
    }

    return ERROR_SUCCESS;
}
/*****************************************************************************************************/
DWORD VerifyLocation(int remove)
{

    char *name;
    char path[IMAX_PATH];

    if ( remove==I_NORMAL_REMOVE )
    {
        PurgeFile(ThisFile);
        return ERROR_SUCCESS;
    }

    if ( StrComp(ProgramDir,NW_SYSTEM_DIR) )
    { // if we're not running from the system directory
        RunningFromHome = TRUE;
        StrCopy(HomeDir,ProgramDir);
		name = StrRChar(ThisFile,cSLASH);
        if ( name )
            name++;
        else
            name = ThisFile;
		sssnprintf(path,sizeof(path),NW_SYSTEM_DIR sSLASH "%s",name);
        iprintf(LS(IDS_COPYING_TO_SYSTEM),name);

        if ( !MyCopyFile(ThisFile,path) )
            LogInstallMessage("Failed to copy vpstart to system directory\n");
    }

    return 0;
}
/*****************************************************************************************************/
DWORD RemoveRegistry()
{
    int i;

    if ( _LoadLibrary(REGEDIT_NAME,false) )
    {
        system("unload "REGEDIT_NAME);
        iprintf(LS(IDS_UNLOADING_REGEDT));

        for ( i = 0; i < 60 && _LoadLibrary(REGEDIT_NAME,false); i++ )
            NTxSleep(500);

        iprintf(REGEDIT_NAME" %s", i < 60 ? LS(IDS_UNLOADED) : LS(IDS_NO_UNLOAD));
    }

    RegCloseDatabase(FALSE);

    UnloadRegistryFunctions();

    if ( _LoadLibrary(REGISTRY_NAME,false) )
    {
        system("unload "REGISTRY_NAME);
        iprintf(LS(IDS_UNLOADING_VPREG));

        for ( i = 0; i < 60 && _LoadLibrary(REGISTRY_NAME,false); i++ )
            NTxSleep(500);

        iprintf(REGISTRY_NAME" %s", i < 60 ? LS(IDS_UNLOADED) : LS(IDS_NO_UNLOAD));
    }

    PurgeFile(NW_SYSTEM_DIR sSLASH REGISTRY_NLM);
    PurgeFile(NW_SYSTEM_DIR sSLASH REGEDIT_NLM);
    PurgeFile(NW_SYSTEM_DIR sSLASH REGISTRY_DIR"\\VPREGDB.SAV");
    PurgeFile(NW_SYSTEM_DIR sSLASH REGISTRY_DIR"\\VPREGDB.BAK");
    PurgeFile(NW_SYSTEM_DIR sSLASH REGISTRY_DIR"\\VPREGDB.DAT");
    rmdir(NW_SYSTEM_DIR sSLASH REGISTRY_DIR);
    return ERROR_SUCCESS;
}
/*****************************************************************************************************/
DWORD VerifyRegistry(int remove, int compatibility)
{

    HANDLE han=NULL;
    char path[IMAX_PATH];
    char path2[IMAX_PATH];
    int rc = ERROR_SUCCESS;

    if ( remove==I_NORMAL_REMOVE )
        return RemoveRegistry();

    CheckRegistryDates();

    // -----------------------------
    // ksr - Certification, 7/21/2002

    //  Check to see if VPReg is already loaded
    //han = LoadLibrary(NW_SYSTEM_DIR sSLASH REGISTRY_NLM);

    if ( !access( NW_SYSTEM_DIR sSLASH REGISTRY_NLM, 0 ) )
    {
        han = LoadLibrary (NW_SYSTEM_DIR sSLASH REGISTRY_NLM );
        if ( !han )
            return ERROR_NO_DATABASE;
    }
    // -----------------------------

    if ( han == NULL )
    {
        if ( !offSafetyChecks )  // gdf sts 337273 03/07/01
        {
            // gdf sts 337273 03/07/01
            if ( !RunningFromHome )
            {
                iprintf(LS(IDS_NO_DATABASE));
                return ERROR_NO_DATABASE;
            }  // gdf sts 337273 03/07/01
        }   // gdf sts 337273 03/07/01
        else
            iprintf(LS(IDS_NO_DATABASE_OVERRIDE)); // sys:\system\vpreg.nlm

        sssnprintf(path,sizeof(path),NW_SYSTEM_DIR sSLASH REGISTRY_NLM);
		sssnprintf(path2,sizeof(path2),"%s" sSLASH REGISTRY_NLM,HomeDir);
        if ( MyCopyFile(path2,path) )     // gdf sts 337273 03/07/01
        {
            // gdf sts 337273 03/07/01
            LogInstallMessage("Failed to copy registry to system directory\n");
            iprintf(LS(IDS_VPREG_COPY_FAILED));   // gdf sts 337273 03/07/01
        }     // gdf sts 337273 03/07/01

        sssnprintf(path,sizeof(path),NW_SYSTEM_DIR sSLASH REGEDIT_NLM);
        sssnprintf(path2,sizeof(path2),"%s\\"REGEDIT_NLM,HomeDir);
        if ( MyCopyFile(path2,path) )    // gdf sts 337273 03/07/01
        {
            // gdf sts 337273 03/07/01
            LogInstallMessage("Failed to copy regedit to system directory\n");
            iprintf(LS(IDS_VPREGEDT_COPY_FAILED));   // gdf sts 337273 03/07/01
        }   // gdf sts 337273 03/07/01

        han = LoadLibrary(NW_SYSTEM_DIR sSLASH REGISTRY_NLM);
        if ( han == NULL )
        { // gdf sts 337273 03/07/01
            iprintf(LS(IDS_ATTEMPT_TO_LOAD_REG_SYS_FAILED));   // gdf sts 337273 03/07/01
            return ERROR_NO_DATABASE;
        } // gdf sts 337273 03/07/01
    }

    rc = LoadRegistryFunctions(compatibility);
    if ( rc )  // gdf sts 337273 03/07/01
        iprintf(LS(IDS_LOAD_REGFUNS_FAILED), rc);   // gdf sts 337273 03/07/01
    return rc;   // gdf sts 337273 03/07/01
}
/*****************************************************************************************************/
#define RELOAD_VPDATA   0xffff0001
#define INITIAL_INSTALL 0xffff0002
DWORD VerifySetup()
{
    DWORD cc = ERROR_SUCCESS;

    if ( !hMainKey ) RegOpenKey(HKEY_LOCAL_MACHINE,REGHEADER,&hMainKey);

    if ( cc == ERROR_SUCCESS )
    {
        GetStr(hMainKey,"Home Directory",HomeDir,sizeof(HomeDir),"");
        if ( HomeDir[0] == 0 )
            cc = 1;
    }

    if ( cc == ERROR_SUCCESS )
    {
        chdir(HomeDir);
        if ( !offSafetyChecks )  // gdf sts 337273 03/07/01
        {
            // gdf sts 337273 03/07/01
            if ( CheckVpdata() )
            {
                iprintf(LS(IDS_RELOAD_VPDATA));  // gdf sts 337273 03/07/01
                return RELOAD_VPDATA;
            }
        }   // gdf sts 337273 03/07/01
    }
    else
    {
        if ( RunningFromHome )    // we are running from the VPHOME directory and we couldn't
        {
            // open the registry so we must be installing for the first time
            strcpy(HomeDir,ProgramDir);
            cc=INITIAL_INSTALL;
            iprintf(LS(IDS_INITIAL_INSTALL));  // gdf sts 337273 03/07/01
        }
        else
        {
            FindAndSetHomeDir();
            iprintf(LS(IDS_EXEC_FINDHOME));  // gdf sts 337273 03/07/01
        }
    }
    return cc;
}
/*****************************************************************************************************/
// returns 0 if the times are the same or non-zero if the times are true
int CheckFileDate(char *component,char* file)
{

    HKEY comKey=0;
    DWORD type=0,dLen;
    char pData[256];
    VTIME Prev,Curr;
    int rc=0;  //  gdf sts 337273 03/07/01

    memset (&Prev,0,sizeof(VTIME));
    memset (&Curr,0,sizeof(VTIME));
    memset (pData,0,sizeof(pData));

    dprintf( "CheckFileDate(%s,%s)\n", component, file );

    if ( RegOpenKey( HKEY_LOCAL_MACHINE, REGHEADER"\\Components", &comKey ) != ERROR_SUCCESS )
    {
        dprintf("Couldn't open Components Key\n");
        iprintf(LS(IDS_COMPONENTS_OPEN));  //  gdf sts 337273 03/07/01
        return TRUE;
    }

    dLen=sizeof( VTIME );
    if ( SymSaferRegQueryValueEx( comKey, file, NULL, &type, (BYTE*)&Prev, &dLen) != ERROR_SUCCESS )
    {
        dprintf("Couldn't read value: %s\n",file);
        iprintf(LS(IDS_COMPONENTS_READ), file);  //  gdf sts 337273 03/07/01
        return TRUE;
    }

/*      if(type==REG_BINARY && dLen==sizeof(VTIME)) {   // maybe the inf file name was passed in
                memcpy(&Prev,pData,sizeof(VTIME));                      // so just copy the vtime data into the vtime struct
        }
        else {
                dLen=sizeof( VTIME );
                type=REG_BINARY;
                if(SymSaferRegQueryValueEx( comKey, pData, NULL, &type, (BYTE*)&Prev, &dLen) != ERROR_SUCCESS)
                        return TRUE;
                if(!(type==REG_BINARY && dLen==sizeof(VTIME)))
                        return TRUE;
        }
*/
    RegCloseKey( comKey );

    GetFileDate(file,&Curr);
    dprintf("Prev: %s\n",vctime(Prev));
    dprintf("Curr: %s\n",vctime(Curr));

    rc = VTcomp(Prev,Curr); //  gdf sts 337273 03/07/01
    iprintf(LS(IDS_VTCOMP_TIME), rc);  //  gdf sts 337273 03/07/01
    return rc;
}
/*****************************************************************************************************/
static BOOL CheckVpdata()
{

    int state = 0;
    FILE *file;
    char str[512];
    char *line;

    if ( CheckFileDate("VPDATA.VAR","VPDATA.VAR") )
    {
        dprintf("vpdata.var is out of date\n");
        iprintf(LS(IDS_VPDATA_DATE));  //  gdf sts 337273 03/07/01
        return TRUE;
    }

    file = fopen("vpdata.var","rt");
    if ( !file )
    {
//        dprintf("Can't open vpdata.var\n",line);  // gdf CW conversion
		dprintf("Can't open vpdata.var\n");  // gdf CW conversion
        iprintf(LS(IDS_VPDATA_OPEN));  //  gdf sts 337273 03/07/01
        return TRUE;
    }

    while ( fgets(str,sizeof(str),file) )
    {
        line = PrepareLine(str);
        if ( line == NULL )
            continue;
        if ( *line == '[' )
        {
            if ( !strnicmp(line,"[COMPONENTS]",12) )
                state = 1;
            else
                state = 0;
        }
        else if ( state == 1 )
        {
            char *q = StrChar(line,'=');
            if ( q )
            {
                *q++ = 0;
                if ( !access(q,0) && CheckFileDate(line,q) )
                {
                    dprintf("%s is out of date\n",line);
                    iprintf(LS(IDS_FILE_DATE), line);  //  gdf sts 337273 03/07/01
                    return TRUE;
                }
            }
        }
    }
    fclose(file);

    dprintf("All inf files are up to date\n");

    return FALSE;
}
/*****************************************************************************************************/
int ShellMain (int argc, char *argv[])
{

    int loaded=1;
    int index=0;
    int i,brk=0;
    char *q,*p;
    DWORD cc;
    DWORD Inst = TRUE;
    BOOL EnglishOnly=0;
    bool install                = false;
    bool update                 = false;
    bool remove                 = false;
    bool regRepair              = false;
    bool loadSAV                = false;
    bool automaticInstallMode   = false;
    bool runSection             = false;
    bool devTestCode            = false;
    HKEY hkey;
    char path[IMAX_PATH];
    char *arg;
    int InfCount=0;
    int UpdateInfCount=0;
    int NoCheckVolumes = 0;
    int NoScanMemory = 0;
    int NWversion;
    int j;  // ksr - Certification, 7/21/2002
	int NW65MinSP = 2;     // Minimum Netware Support pack required for NW6.5
	int NW60MinSP = 5;     // Minimum Netware Support pack required for NW6.0
	int NW51MinSP = 11;     // Minimum Netware Support pack required for NW5.1  // Support Pack 7 is returning 0xA for this version SP8 is returning 0xB
    DWORD           valueDataSize           = 0;
    DWORD           returnValDW             = ERROR_OUT_OF_PAPER;
    HRESULT         returnValHR             = E_FAIL;

    char traceLogPath[IMAX_PATH];
	char clientInstallPath[IMAX_PATH];
    char ExVolPath[IMAX_PATH];
    NetwareInstaller::tcstring  moduleDirectory;
    NetwareInstaller::tcstring  scriptFilename;
    NetwareInstaller            installer;
    CConnectionlessMessage      udpStatusReporter;
    NetwareInstaller::tcstring  statusIPstring;
    NetwareInstaller::tcstring  statusPortString;
    sockaddr_in                 statusTargetAddress;
    InstallUIhandlerArguments   uiHandlerArguments;
    NetwareInstaller::tcstring  installMessageFilename;
    LPCTSTR                     messageFilename         = NULL;
    DWORD                       oldCurrentNameSpace     = 0;
    DWORD                       oldTargetNamespace      = 0;
    NetwareInstaller::tcstring  variablesFilename;
    NetwareInstaller::tcstring  sectionName;
    bool                        parseComplete           = false;

    // Initialize Part 1
    returnValHR         = S_OK;
    ErrorCount          =0;
	RemoveInProgress    = FALSE;
    ThreadCount += 1;
    ThreadID            = NTxGetCurrentThreadId();
    TGID                = GetThreadGroupID();
    moduleDirectory     = NetwareInstaller::GetParentPath(argv[0]);

    // Initialize PScan globals
    // Determine installed directory (ProgramDir and HomeDir)
    // Note:  SCSCOMMS relies on ProgramDir
    returnValDW     = ERROR_OUT_OF_PAPER;
    ProgramDir[0]   = NULL;
    HomeDir[0]      = NULL;
    if (access(NW_SYSTEM_DIR sSLASH REGISTRY_NLM, 04) == 0)
    {
        if (LoadLibrary("SYS:SYSTEM\\vpreg.nlm") != NULL)
        {
            // Execute the uninstall script from the program directory
            if (LoadRegistryFunctions(TRUE) == ERROR_SUCCESS)
            {
                // Determine program dir
                returnValDW = RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER, &hkey);
                if (returnValDW == ERROR_SUCCESS)
                {
                    // sizeof does not work here because Programdir is extern'd
                    valueDataSize = IMAX_PATH;
                    returnValDW = RegQueryValueEx(hkey, szReg_Val_HomeDir, 0, NULL, (LPBYTE) ProgramDir, &valueDataSize);
                    if (returnValDW == ERROR_SUCCESS)
                        _tcsncpy(HomeDir, ProgramDir, IMAX_PATH);
                    RegCloseKey(hkey);
                    hkey = NULL;
                }
                UnloadRegistryFunctions();
            }
        }
    }
    if (ProgramDir[0] == NULL)
    {
        // ProgramDir and HomeDir are extern'd, thus sizeof will not work
        _tcsncpy(ProgramDir, moduleDirectory.c_str(), IMAX_PATH);
        _tcsncpy(HomeDir, ProgramDir, IMAX_PATH);
    }
    // Note:  hMainKey must be initialized for NTS's discovery.cpp functions to work (done in DoActionStartNTS)

    // Initialize Part 2
    scriptFilename                      = moduleDirectory + NetwareInstaller::tcstring("\\install.ini");
    installMessageFilename              = ProgramDir + NetwareInstaller::tcstring("\\rtvscan.msg");
    uiHandlerArguments.statusUpdater    = NULL;
    // ** NOTE THERE WAS A GPF IN CERT HANDLING BEFORE ABOVE LINE WAS ADDED - FIND A WAY TO DETECT AND PREVENT
    // Set namespace to long filenames, preserve filename case, and recognize *.* as an "all files" pattern
    oldCurrentNameSpace = SetCurrentNameSpace(4);
    oldTargetNamespace = SetTargetNameSpace(4);
    UseAccurateCaseForPaths(TRUE);
    UnAugmentAsterisk(true);

#ifdef MemCheck
    char* certPtr=NULL;
#endif

    // Verify server minimum version
    GetServerInformation (sizeof(g_fsinf),&g_fsinf);
	if(g_fsinf.netwareVersion == 5) // NW 5 or NW 6
	{
		if(g_fsinf.netwareSubVersion == 0x46) // NW 6.5
		{
			if(g_fsinf.revisionLevel < NW65MinSP)     // as of 01/28/2005 SP 2 or greater
			{
		        ConsolePrintf(LS(IDS_OLD_NW65_SERVICE_PACK));
                returnValHR = E_FAIL;
			}
		}
		else if(g_fsinf.netwareSubVersion==0x3C)   // NW 6.0 
		{
			if(g_fsinf.revisionLevel < NW60MinSP)  // as of 01/28/2005 SP 5 of greater
			{
		        ConsolePrintf(LS(IDS_OLD_NW60_SERVICE_PACK));
                returnValHR = E_FAIL;
			}
		}
		else if(g_fsinf.netwareSubVersion == 0x00)   // NW 5.1
		{
			if(g_fsinf.revisionLevel < NW51MinSP)  // as of 01/28/2005 SP 8 of greater
			{
		        ConsolePrintf(LS(IDS_OLD_NW51_SERVICE_PACK));
                returnValHR = E_FAIL;
			}
		}
	}
	else
	{
        ConsolePrintf( LS(IDS_OLD_NW_VERSION) );
		returnValHR = E_FAIL;
	}
    NWversion = g_fsinf.netwareVersion;

    // Report CLib verson for debugging
    if (SUCCEEDED(returnValHR))
    {
        strncpy(FileServerName,g_fsinf.serverName, 50);
        CLibVersion=(g_fsinf.CLibMajorVersion*100 + g_fsinf.CLibMinorVersion)*100;
        if ( g_fsinf.CLibRevision > 0x60 )
            CLibVersion += g_fsinf.CLibRevision - 0x60;
        // fix - revision is probably not ascii char - decimal value - adding else
        else
            CLibVersion += g_fsinf.CLibRevision;

        dprintf("CLib Version = %d\n", CLibVersion);
    }
   
    // Parse command line
    if (SUCCEEDED(returnValHR))
    {
        // Rectify argv[0] if launched from system because of secure console
        if ((argc == 3) && !strcmp(argv[1], "/SECURE_CONSOLE"))
        {
            argv[0] = argv[2];
            argc -=2;
        }
        if ((argc == 4) && !strcmp(argv[2], "/SECURE_CONSOLE"))
        {
            argv[0] = argv[3];
            argc -=2;
        }

        for (i=1; (i < argc) && !parseComplete; i++)
        {
            strupr(argv[i]);

            ThreadSwitchWithDelay();

            arg = argv[i];
            if ( *arg == '\\' || *arg == '/' )
                arg++;

            switch ( arg[0] )
            {
                case 'D':
                    if ( StrEqual(arg,"DEBUG") )
                    {
                        debug |= DEBUGPRINT;
                        continue;
                    }
                    else if ( StrNEqual(arg,"DEBUG=",6) )
                    {
                        InitializeDebugLogging(argv[i]+6);
                        continue;
                    }
                    else if (StrEqual(arg,"DEVTEST"))
                    {
                        devTestCode = true;
                        continue;
                    }
                    break;

                case 'R':
                    if ( StrEqual(arg,"REGREPAIR") )
                    {
                        regRepair = true;
                        continue;
                    }
                    if ( StrEqual(arg,"REMOVE") )
                    {
                        remove = true;
                        continue;
                    }
                    if ( StrEqual(arg, "RUNSECTION") )
                    {
                        if ((argc == 3) || (argc == 4) || (argc == 5))
                        {
                            runSection = true;
                            parseComplete = true;
                            break;
                        }
                        else
                        {
                            ConsolePrintf("Insufficient arguments for RUNSECTION.\n");
                        }
                    }
                    break;

                case 'B':
                    if ( StrEqual(arg,"BREAK") )
                    {
                        brk = 1;
                        continue;
                    }
                    break;

                case 'I':
                    if ( StrEqual(arg,"INSTALL") )
                    {
                        install = true;
                        continue;
                    }
                    break;

                case  'E':
                    if ( StrEqual(arg,"ENGLISH") )
                    {
                        EnglishOnly=TRUE;
                        continue;
                    }
                    break;

    #ifdef MemCheck
                case 'M':
    #ifndef _DEBUG
                    if ( StrEqual(arg,"MEMCHECK") )
    #endif // !_DEBUG
                    {
                        TrackMallocs = TRUE;
                        continue;
                    }
                    break;
    #endif // MemCheck
                case  'N':
                    if ( StrEqual(argv[i],"NO_VOLUME_CHECK") )
                    {
                        NoCheckVolumes = 1;
                        continue;
                    }
                    if ( StrEqual(arg,"NOMEMSCAN") )
                    {
                        NoScanMemory = 1;
                        continue;
                    }
                    break;
                case  'O':  // gdf sts 337273 03/07/00
                    if ( StrEqual(arg, "OFFSCHECKS") ) // gdf sts 337273 03/07/00
                    {
                        // gdf sts 337273 03/07/00
                        offSafetyChecks = TRUE;  // gdf sts 337273 03/07/00
                        ConsolePrintf(LS(IDS_SAFETY_CHECKS_OFF));   // gdf sts 337273 03/07/00
                        continue;  // gdf sts 337273 03/07/00
                    }   // gdf sts 337273 03/07/00
                    break;   // gdf sts 337273 03/07/00
    #ifdef SemaCheck
                case 'S':
    #ifndef _DEBUG
                    if ( StrEqual(arg,"SEMACHECK") )
    #endif // !_DEBUG
                    {
                        TrackSemaphores = TRUE;
                        continue;
                    }
                    break;
    #endif // SemaCheck
    #ifdef ThreadCheck
                case 'T':
    #ifndef _DEBUG
                    if ( StrEqual(arg,"THREADCHECK") )
    #endif// !_DEBUG
                    {
                        TrackThreads = TRUE;
                        continue;
                    }
                    break;
    #endif
                case  'U':
                    if ( StrEqual(arg,"UPDATE") )
                    {
                        update = TRUE;
                        continue;
                    }
                    break;
                case 'A':
                    if (StrEqual(arg, "AUTO"))
                    {
                        automaticInstallMode = true;
                        continue;
                    }
                default:
                    _printf(LS(IDS_BAD_OPTION),arg);
                    returnValHR = E_FAIL;
                    break;
            }
        }
    }

    // Initialize UI and strings
    if (SUCCEEDED(returnValHR))
    {
        if (install | update)
            messageFilename = installMessageFilename.c_str();
        if ( OpenStringTable(EnglishOnly, messageFilename) != ERROR_SUCCESS )
        {
            if (offSafetyChecks)
            {
                ConsolePrintf(LS(IDS_STRING_TABLE_RELOAD));
            }
            else
            {
                ConsolePrintf(LS(IDS_STRING_TABLE_FAILURE));
                returnValHR = E_FAIL;
            }
        }
        if (SUCCEEDED(returnValHR))
            SetupInstallUI(FALSE);
    }

    // Do what was requested on the command line
    if (SUCCEEDED(returnValHR))
    {
        if (devTestCode)
        {
            ConsolePrintf("*** Running DevTest code...\n");
            if (true)
            {
                char test[50] = {""};
                ZeroMemory(test, sizeof(test));
                snprintf(test, 10, "blah blah blah %d", 987654321);
                ConsolePrintf("Test code is:  >%s<\n", test);
                
                ConsolePrintf("vsnprintf test done.\n");
            }
            if (false)
            {
                TCHAR       nameBuffer[MAX_PATH]    = {""};
                hostent*    hostInfo                = NULL;
                DWORD       currAddressNo           = 0;
                in_addr*    currAddressPtr          = NULL;
                DWORD       returnValDW             = 0;

                hostInfo = gethostbyname("wxppro-ds23");
                if (hostInfo != NULL)
                {
                    ConsolePrintf("Resolved named.\n");
                    while( hostInfo->h_addr_list[currAddressNo] != NULL )
                    {
                        //get the IP address
                        currAddressPtr = (in_addr*) hostInfo->h_addr_list[currAddressNo];
                        ConsolePrintf("Resolved name to %s.\n", inet_ntoa(*currAddressPtr));
                        currAddressNo += 1;
                    }
                }

				// this used to call exit, but calling exit bypasses all the deconstructors, since
				// exit doesn't return!

				return 0;
            }
            if (false)
            {
                LPCTSTR filename = "SYS:\\test.txt";
                int openFileHandle = -1;
                FILE* fopenFileHandle = NULL;

                // Open the file in shareable mode
                openFileHandle = sopen(filename, O_APPEND | O_RDWR, SH_DENYNO);
                if (openFileHandle == -1)
                {
                    ConsolePrintf("*** DML:  initial open failed %d.\n", errno);
                    // Create the file, then close and re-open in shareable mode
                    openFileHandle = sopen(filename, O_CREAT | O_APPEND | O_RDWR, SH_DENYNO, S_IWRITE);
                    if (openFileHandle != -1)
                    {
                        ConsolePrintf("*** DML:  sopen created file.\n");
                        close(openFileHandle);
                        openFileHandle = -1;
                        openFileHandle = sopen(filename, O_APPEND | O_RDWR, SH_DENYNO);
                    }
                }
                
                if (openFileHandle != -1)
                {
                    FILE* fopenFileHandle = NULL;
                    fopenFileHandle = fdopen(openFileHandle, "at");
                    if (fopenFileHandle != NULL)
                    {
                        ConsolePrintf("*** DML:  fdopen SUCCEEDED.\n");
                        fprintf(fopenFileHandle, "blah blah blah\n");
                        fclose(fopenFileHandle);
                        fopenFileHandle = NULL;
                    }
                    else
                    {
                        ConsolePrintf("*** DML:  fdopen FAILED %d.\n", errno);
                    }
                }
            }
            
            if (false)
            {
                HKEY                    keyHandle               = NULL;
                DWORD                   valueNo                 = 0;
                TCHAR                   valueName[MAX_PATH]     = {""};
                DWORD                   valueNameSize           = 0;
                TCHAR                   valueNameLower[MAX_PATH]= {""};
                DWORD                   valueType               = REG_DWORD;
                DWORD                   valueData               = 0;
                DWORD                   valueDataSize           = 0;
                NetwareInstaller::StringList              valuesToDelete;
                NetwareInstaller::StringList::iterator    currValueToDelete;
                DWORD                   returnValDW             = ERROR_OUT_OF_PAPER;
                HRESULT                 returnValHR             = E_FAIL;

                LoadLibrary("SYS:SYSTEM\\vpreg.nlm");
                if (LoadRegistryFunctions(TRUE) == ERROR_SUCCESS)
                {
                    returnValDW = RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER "\\" szReg_Val_NLMsToLoad, &keyHandle);
                    if (returnValDW == ERROR_SUCCESS)
                    {
                        // Find all problematic values
                        valueNameSize = sizeof(valueName)/sizeof(valueName[0]);
                        valueDataSize = sizeof(valueData);
                        while (RegEnumValue(keyHandle, valueNo, valueName, &valueNameSize, 0, &valueType, (LPBYTE) &valueData, &valueDataSize) == ERROR_SUCCESS)
                        {
                            if (valueType == REG_DWORD)
                            {
                                _tcsncpy(valueNameLower, valueName, sizeof(valueNameLower)/sizeof(valueNameLower[0]));
                                strlwr(valueNameLower);
                                if (_tcsstr(valueNameLower, "rtvscan") != NULL)
                                {
                                    if (_tcsstr(valueNameLower, "$home$") == NULL)
                                        valuesToDelete.push_back(valueName);
                                }
                            }

                            valueNameSize = sizeof(valueName)/sizeof(valueName[0]);
                            valueDataSize = sizeof(valueData);
                            valueNo += 1;
                        }

                        // Delete the values
                        if (valuesToDelete.size() != 0)
                            returnValHR = S_OK;
                        else
                            returnValHR = S_FALSE;
                        for (currValueToDelete = valuesToDelete.begin(); currValueToDelete != valuesToDelete.end(); currValueToDelete++)
                        {
                            returnValDW = RegDeleteValue(keyHandle, currValueToDelete->c_str());
                            if ((returnValDW != ERROR_SUCCESS) && (returnValHR == S_OK))
                                returnValHR = HRESULT_FROM_WIN32(returnValDW);
                        }

                        // Cleanup and return
                        RegCloseKey(keyHandle);
                        keyHandle = NULL;
                    }
                    else
                    {
                        returnValHR = HRESULT_FROM_WIN32(returnValDW);
                    }

                    UnloadRegistryFunctions();
                    ConsolePrintf("Returnval is 0x%08x.\n", returnValHR);
                }
                else
                {
                    ConsolePrintf("LoadRegistryFunctions failed.\n");
                }
            }

            
            // Test code for status updater
            if (false)
            {
                uiHandlerArguments.statusUpdater = &udpStatusReporter;
                uiHandlerArguments.logFilename = "install.log";
                ZeroMemory(&statusTargetAddress, sizeof(statusTargetAddress));
                returnValHR = NetwareInstaller::GetScriptVariable(scriptFilename, "STATUSIP", &statusIPstring);
                if (FAILED(returnValHR))
                    ConsolePrintf("Error 0x%08x getting variable ip.\n", returnValHR);
                returnValHR = NetwareInstaller::GetScriptVariable(scriptFilename, "STATUSPORT", &statusPortString);
                if (FAILED(returnValHR))
                    ConsolePrintf("Error 0x%08x getting variable port.\n", returnValHR);

                if ((statusIPstring != "") && (statusPortString != ""))
                {
                    statusTargetAddress.sin_family = AF_INET;
                    statusTargetAddress.sin_port = _ttoi(statusPortString.c_str());
                    statusTargetAddress.sin_addr.S_un.S_addr = inet_addr((char*) statusIPstring.c_str());

                    returnValHR = udpStatusReporter.Initialize(statusTargetAddress);
                    if (FAILED(returnValHR))
                        ConsolePrintf("Error 0x%08x initializing remote status reporter.\n", returnValHR);
                }
                else
                {
                    ConsolePrintf("Status updater NOT initialized.  ipstring = >%s<, port=>%s<\n", statusIPstring.c_str(), statusPortString.c_str());
                }
                // Initialize the SAVSecure->NetwareInstaller logging interface
                SAVSecureCAdprintfInterfaceInstaller = &installer;

                for (int x = 0; x < (20*10); x++)
                {
                    if ((x % 10) == 0)
                        ConsolePrintf("SendMessage %d.\n", x);
                    udpStatusReporter.SendMessage_Status(x, "fake overall", 100-x, "fake action");
                    NTxSleep(100);
                }
            }

            // GetParentPath test cases
            if (false)
            {
                ConsolePrintf("DMLtest:  Start.\n");
                ConsolePrintf("0:  %s\n", NetwareInstaller::GetParentPath("SYS:\\SYSTEM\\vpreg").c_str());
                ConsolePrintf("1:  %s\n", NetwareInstaller::GetParentPath("SYS:\\SYSTEM").c_str());
                ConsolePrintf("2:  %s\n", NetwareInstaller::GetParentPath("SYS:SYSTEM").c_str());
                ConsolePrintf("3:  %s\n", NetwareInstaller::GetParentPath("SYS:SYSTEM\\vpreg").c_str());
                ConsolePrintf("4:  %s\n", NetwareInstaller::GetParentPath("SYS:\\").c_str());
                ConsolePrintf("5:  %s\n", NetwareInstaller::GetParentPath("SYS:").c_str());
            }

            // Generic NetwareInstaller test harness
            if (false)
            {
                NetwareInstaller x;
                x.TestHarness(argc, argv);
            }

            ConsolePrintf("DMLtest:  End.\n");
        }
        // On initial install, we are run with no command line parameters.  Detect this
        // and perform the install action instead of nothing.
        if (!install && !update && !remove && !regRepair && !devTestCode)
        {
            if (access(NW_SYSTEM_DIR sSLASH REGISTRY_NLM, 04) != 0)
                install = true;
        }
        if (automaticInstallMode)
        {
            // Install or update, we have to determine which
            install = false;
            update = false;
            remove = false;
            // If not yet installed, either install or update.
            if (access(NW_SYSTEM_DIR sSLASH REGISTRY_NLM, 04) == 0)
            {
                update = true;
                ConsolePrintf(LS(IDS_NWINSTALL_AUTOMODE_UPDATE));
            }
            else
            {
                install = true;
                ConsolePrintf(LS(IDS_NWINSTALL_AUTOMODE_INSTALL));
            }
        }
        // Initialize the SAVSecure->NetwareInstaller logging interface
        SAVSecureCAdprintfInterfaceInstaller = &installer;
        uiHandlerArguments.statusUpdater = &udpStatusReporter;
        uiHandlerArguments.logFilename = LS(IDS_NWINSTALL_LOGFILENAME);

        // Initialize status reporter and logging services
        if (install || update)
        {
            TCHAR userMessage[2*MAX_PATH+1] = {""};

            uiHandlerArguments.statusUpdater = &udpStatusReporter;
            uiHandlerArguments.logFilename = LS(IDS_NWINSTALL_LOGFILENAME);
            ZeroMemory(&statusTargetAddress, sizeof(statusTargetAddress));
            
            sssnprintf(userMessage, sizeof(userMessage), LS(IDS_NWINSTALL_PROCESSING_SCRIPT), scriptFilename.c_str());
            PrintToInstallPortal(userMessage);
            ConsolePrintf(userMessage);
            returnValHR = NetwareInstaller::GetScriptVariable(scriptFilename, "STATUSIP", &statusIPstring);

            // TEMPORARY FIX FOR CLIENT HANDLE CLOSE ISSUE
            if (returnValHR == HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION))
            {
                TCHAR userMessage[2*MAX_PATH+1] = {""};
                
                sssnprintf(userMessage, sizeof(userMessage), LS(IDS_NWINSTALL_ERROR_SHARINGVIOLATION_INITIAL), scriptFilename.c_str());
                PrintToInstallPortal(userMessage);
                PrintToInstallPortal(LS(IDS_NWINSTALL_SHARINGVIOLATIONWAIT));
                NTxSleep(30000);
                returnValHR = NetwareInstaller::GetScriptVariable(scriptFilename, "STATUSIP", &statusIPstring);
                // This problem only occurs on upgrade - tell user what to do to get past it
                if (returnValHR == HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION))
                {
                    ConsolePrintf(LS(IDS_NWINSTALL_ERROR_SHARINGVIOLATION_FINAL), scriptFilename.c_str(), argv[0]);

					// this used to call exit, but calling exit bypasses all the deconstructors, since
					// exit doesn't return!

                    return 0;
                }
            }
            
            if (FAILED(returnValHR))
                ConsolePrintf(LS(IDS_NWINSTALL_ERROR_STATUS_RETRIEVINGIP), returnValHR);
            returnValHR = NetwareInstaller::GetScriptVariable(scriptFilename, "STATUSPORT", &statusPortString);
            if (FAILED(returnValHR))
                ConsolePrintf(LS(IDS_NWINSTALL_ERROR_STATUS_RETRIEVINGPORT), returnValHR);

            if ((statusIPstring != "") && (statusPortString != ""))
            {
                statusTargetAddress.sin_family = AF_INET;
                statusTargetAddress.sin_port = _ttoi(statusPortString.c_str());
                statusTargetAddress.sin_addr.S_un.S_addr = inet_addr((char*) statusIPstring.c_str());

                returnValHR = udpStatusReporter.Initialize(statusTargetAddress);
                if (FAILED(returnValHR))
                    ConsolePrintf(LS(IDS_NWINSTALL_ERROR_STATUS_INIT), returnValHR);
            }
            else
            {
                ConsolePrintf(LS(IDS_NWINSTALL_ERROR_STATUS_NOTINITIALIZED), statusIPstring.c_str(), statusPortString.c_str());
            }
        }
        if (install)
        {
            // Install
            returnValHR = installer.ProcessScript(NetwareInstaller::InstallMode_Install, scriptFilename.c_str(), argv[0], InstallUIhandler, (void*) &uiHandlerArguments);
            // Give user time to read the display if an error occured
            if (FAILED(returnValHR))
                NTxSleep(3000);
        }
        else if (update)
        {
            // Build update
            returnValHR = installer.ProcessScript(NetwareInstaller::InstallMode_Reinstall, scriptFilename.c_str(), argv[0], InstallUIhandler, (void*) &uiHandlerArguments);
            // Give user time to read the display if an error occured
            if (FAILED(returnValHR))
                NTxSleep(3000);
        }
        else if (remove)
        {
            // Uninstall
            scriptFilename = NetwareInstaller::tcstring(HomeDir) + NetwareInstaller::tcstring("\\install.ini");
            uiHandlerArguments.statusUpdater = NULL;
            returnValHR = installer.ProcessScript(NetwareInstaller::InstallMode_Uninstall, scriptFilename.c_str(), argv[0], InstallUIhandler, (void*) &uiHandlerArguments);
            // Give user time to read the display if an error occured
            if (FAILED(returnValHR))
                NTxSleep(3000);
        }
        else if (regRepair)
        {
            // Registry repair
            // Uninstall
            scriptFilename = NetwareInstaller::tcstring(HomeDir) + NetwareInstaller::tcstring("\\install.ini");
            uiHandlerArguments.statusUpdater = NULL;
            returnValHR = installer.ProcessScript(scriptFilename, "RegRepair", "", NetwareInstaller::InstallMode_Install, false, argv[0], InstallUIhandler, (void*) &uiHandlerArguments);
            // Give user time to read the display if an error occured
            if (FAILED(returnValHR))
                NTxSleep(3000);
        }
        else if (runSection)
        {
            uiHandlerArguments.statusUpdater = NULL;
            // Run an arbitrary section of a script file
            sectionName = argv[2];
            if (argc > 3)
            {
                scriptFilename = argv[3];
                if (argc == 5)
                    variablesFilename = argv[4];
            }
            uiHandlerArguments.logFilename = LS(IDS_NWINSTALL_LOGFILENAME);
            uiHandlerArguments.statusUpdater = NULL;
            returnValHR = installer.ProcessScript(scriptFilename, sectionName, variablesFilename, NetwareInstaller::InstallMode_Automatic, false, argv[0], InstallUIhandler, (void*) &uiHandlerArguments);
            // Give user time to read the display if an error occured
            if (FAILED(returnValHR))
                NTxSleep(3000);

        }
        else if (!devTestCode)
        {
            // Start SAV
            PrintToInstallPortal(LS(IDS_STARTING_LDVP));

            if (LoadLibrary("SYS:SYSTEM\\vpreg.nlm") != NULL)
            {
                // Load the list of modules that we need to load, to load SAV
                if (LoadRegistryFunctions(FALSE) == ERROR_SUCCESS)
                {
                    // Read in the list of NLMs to load in order to start RTVScan
                    memset(load,0,sizeof(load));
                    if (RegOpenKey(HKEY_LOCAL_MACHINE,REGHEADER"\\NLMsToLoad",&hkey) == ERROR_SUCCESS)
                    {
                        char name[IMAX_PATH];
                        DWORD pos;
                        DWORD ns=IMAX_PATH,ps=sizeof(DWORD);
                        while (RegEnumValue(hkey,index++,name,&ns,0,NULL,(BYTE*)&pos,&ps) == ERROR_SUCCESS)
                        {
                            loadSAV = true;
                            if ( pos < MAX_LOAD )
                            {
                                if ( !strnicmp(name,"$HOME$",6) )
                                    sssnprintf(path,sizeof(path),"%s%s",HomeDir,name+6);
                                else
                                    StrCopy(path,name);

                                if ( strstr(path,"Rtvscan") || strstr(path,"rtvscan") || strstr(path,"RTVSCAN") )
                                {
                                    if ( NoCheckVolumes )
                                        strcat(path," NO_VOLUME_CHECK");

                                    if ( NoScanMemory )
                                        strcat(path," NOMEMSCAN");

                                    if ( EnglishOnly )
                                        strcat(path," ENGLISH");

                #ifdef MemCheck
                                    if ( TrackMallocs )
                                        strcat(path," MEMCHECK");
                #endif // MemCheck
                #ifdef SemaCheck
                                    if ( TrackSemaphores )
                                        strcat(path," SEMACHECK");
                #endif // SemaCheck
                #ifdef ThreadCheck
                                    if ( TrackThreads )
                                        strcat(path," THREADCHECK");
                #endif // ThreadCheck

                                    if ( brk )
                                        strcat(path," BREAK");

                                    if ( debug )
                                    {
                                        strcat(path," DEBUG=");
                                        if ( debug&DEBUGLOG )
                                            strcat(path,"Logging,");
                                        if ( debug&DEBUGOUTPUT )
                                            strcat(path,"OutputString,");
                                        if ( debug&DEBUGPRINT )
                                            strcat(path,"Print,");
                                        if ( debug&DEBUGSAFELOG )
                                            strcat(path,"SafeLog,");
                                        if ( debug&DEBUGVERBOSE )
                                            strcat(path,"Verbose,");
                                        path[strlen(path)-1]=0;
                                    }
                                }

                                load[pos] = (char*)malloc(strlen(path)+1);
                                if ( load[pos] )
                                {
                                    StrCopy(load[pos],path);
                                }
                                else
                                {
                                    for ( j = 0; j < MAX_LOAD; j++ )
                                    {
                                        if (load[j])
                                            free(load[j]);
                                    }
                                    loadSAV = false;
                                    break;
                                }
                                // -----------------------------

                            }
                            ns=IMAX_PATH;
                            ps=sizeof(DWORD);
                        }
                        RegCloseKey(hkey);
                    }

                    // Load all of the NLMs determined above
                    if (loadSAV)
                    {
                        for ( loaded=1,i=0;i<MAX_LOAD;i++ )
                        {
                            ThreadSwitchWithDelay();
                            if ( load[i] )
                            {
                                ConsolePrintf(LS(IDS_LOADING_FILE),load[i]);

                                if ( !LoadLibrary(load[i]) )
                                    ConsolePrintf(LS(IDS_VPSTART_LOADING_FILE),load[i]); // gdf sts 337273 03/09/0
                                free(load[i]);
                                if (update)
                                    loaded++;
                            }
                        }
                    }
                }
                else
                {
                    ConsolePrintf("%s\n", LS(IDS_NWINSTALL_ERROR_REGISTRY_SYMBOLIMPORT));
                }
            }
            else
            {
                ConsolePrintf("%s\n", LS(IDS_NWINSTALL_ERROR_REGISTRY_LOADING));
            }
        }
    }

    // Cleanup and exit
    if ( hMainKey )
        RegCloseKey(hMainKey);

    cc = ERROR_SUCCESS;
    DestroyInstallUI();
    UnloadRegistryFunctions();
    Running = 0;
    NTxSleep(200);
    CloseStringTable();
    ThreadCount--;
    NLMRunning = 0;

    // Restore namespace
    SetCurrentNameSpace(oldCurrentNameSpace);
    SetTargetNameSpace(oldTargetNamespace);

	// this used to call exit, but calling exit bypasses all the deconstructors, since
	// exit doesn't return!

    return 0;
}

int main (int argc, char *argv[])
{
	// but I do need to call exit, since that cleans up the threads, just returning doesn't
	// and I definitely have a bunch of threads hanging around that haven't exited yet,
	// LoadLibrary and such. If I leave really quickly then I have a race condition with
	// them for tear down of things.

	// so I hoisted all the objects up in to ShellMain - that is a little cleaner than
	// just putting another level of braces in a function that is kind of big already

	exit( ShellMain( argc, argv ) );
}

/************************************************************************/

void InstallUIhandler( NetwareInstaller::StatusCallbackFunctionEventType eventType, void* eventParam, void* userParam )
// Displays the specified message on the install UI
{
    int                                         logFileHandle           = NULL;
#ifdef NLM
    TCHAR                                       logLine[1024]           = {""};
#endif
    InstallUIhandlerArguments*                  arguments               = NULL;
    TCHAR                                       linePrefix[50]          = {""};
    time_t                                      currentTime;
    tm                                          currentTimeTM;
    NetwareInstaller::StatusInfo_Progress*      progressInfo            = NULL;
    LPCTSTR                                     statusLine              = NULL;
    static NetwareInstaller::tcstring           lastOverallStatus;
    bool                                        displayStatusOnScreen   = false;

    // Validate parameters
    if (eventParam == NULL)
        return;
    if (userParam == NULL)
        return;

    // Initialize
    arguments = (InstallUIhandlerArguments*) userParam;

    // Open log file        
    if (userParam != NULL)
    {
        // Open the file in shareable mode
        logFileHandle = sopen(arguments->logFilename.c_str(), O_APPEND | O_RDWR, SH_DENYNO);
        if (logFileHandle == -1)
        {
            // Create the file, then close and re-open in shareable mode
            logFileHandle = sopen(arguments->logFilename.c_str(), O_CREAT | O_APPEND | O_RDWR, SH_DENYNO, S_IWRITE);
            if (logFileHandle != -1)
            {
                close(logFileHandle);
                logFileHandle = -1;
                logFileHandle = sopen(arguments->logFilename.c_str(), O_APPEND | O_RDWR, SH_DENYNO);
            }
        }
    }

    // Initialize
    time(&currentTime);
    currentTimeTM = *localtime(&currentTime);
    sssnprintf(linePrefix, sizeof(linePrefix), "%d:%d:%d ", currentTimeTM.tm_hour, currentTimeTM.tm_min, currentTimeTM.tm_sec);
    switch (eventType)
    {
    case NetwareInstaller::StatusCallbackFunctionEventType_Error:
        _tcscat(linePrefix, "ERROR");
        break;
    case NetwareInstaller::StatusCallbackFunctionEventType_Warning:
        _tcscat(linePrefix, "WARNING");
        break;
    case NetwareInstaller::StatusCallbackFunctionEventType_Progress:
        _tcscat(linePrefix, "Progress");
        break;
    case NetwareInstaller::StatusCallbackFunctionEventType_Info:
        _tcscat(linePrefix, "Info");
        break;
    }

    if ((eventType == NetwareInstaller::StatusCallbackFunctionEventType_Error) || (eventType == NetwareInstaller::StatusCallbackFunctionEventType_Warning) || (eventType == NetwareInstaller::StatusCallbackFunctionEventType_Info))
    {
        // Log the line
        statusLine = (LPCTSTR) eventParam;
        sssnprintf(logLine, sizeof(logLine), "%s:  %s\r\n", linePrefix, statusLine);
        if (logFileHandle != -1)
            write(logFileHandle, logLine, _tcslen(logLine));
            
        // Display on screen
        if ((eventType == NetwareInstaller::StatusCallbackFunctionEventType_Error) || (eventType == NetwareInstaller::StatusCallbackFunctionEventType_Warning))
        {
            sssnprintf(logLine, sizeof(logLine), "%s\n", statusLine);
            PrintToInstallPortal(logLine);
        }
        
        // Send to deployment system
        if (arguments->statusUpdater != NULL)
            arguments->statusUpdater->SendMessage_TypedLine(eventType, statusLine);
    }
    else if (eventType == NetwareInstaller::StatusCallbackFunctionEventType_FinishedSignal)
    {
        // Send to deployment system only to tell it we're done
        statusLine = (LPCTSTR) eventParam;
        if (arguments->statusUpdater != NULL)
        {
            if (statusLine != NULL)
                arguments->statusUpdater->SendMessage_TypedLine(eventType, statusLine);
            else
                arguments->statusUpdater->SendMessage_TypedLine(eventType, "");
        }
    }
    else if (eventType == NetwareInstaller::StatusCallbackFunctionEventType_Progress)
    {
        // Log the line
        progressInfo = (NetwareInstaller::StatusInfo_Progress*) eventParam;

        if (progressInfo->actionDescription == "")
        {
            sssnprintf(logLine, sizeof(logLine), "%s:  %s (%d%%)\r\n", linePrefix, progressInfo->overallDescription.c_str(), progressInfo->overallPercentage);
            if (logFileHandle != -1)
                write(logFileHandle, logLine, _tcslen(logLine));
            lastOverallStatus = progressInfo->overallDescription;
        }
        else if (_tcsicmp(progressInfo->overallDescription.c_str(), lastOverallStatus.c_str()) == 0)
        {
            sssnprintf(logLine, sizeof(logLine), "%s:     %s (%d%%|%d%%)\r\n", linePrefix, progressInfo->actionDescription.c_str(), progressInfo->actionPercentage, progressInfo->overallPercentage);
            if (logFileHandle != -1)
                write(logFileHandle, logLine, _tcslen(logLine));
        }
        else
        {
            sssnprintf(logLine, sizeof(logLine), "%s:  %s (%d%%)\r\n", linePrefix, progressInfo->overallDescription.c_str(), progressInfo->overallPercentage);
            if (logFileHandle != -1)
                write(logFileHandle, logLine, _tcslen(logLine));
            sssnprintf(logLine, sizeof(logLine), "%s:     %s (%d%%)\r\n", linePrefix, progressInfo->actionDescription.c_str(), progressInfo->actionPercentage);
            if (logFileHandle != -1)
                write(logFileHandle, logLine, _tcslen(logLine));
            
            lastOverallStatus = progressInfo->overallDescription;
        }

        // Display on console screen
        if (progressInfo->actionDescription == "")
        {
            sssnprintf(logLine, sizeof(logLine), "%s (%d%%)\n", progressInfo->overallDescription.c_str(), progressInfo->overallPercentage);
            lastOverallStatus = progressInfo->overallDescription;
            PrintToInstallPortal(logLine);
        }
        else if (_tcsicmp(progressInfo->overallDescription.c_str(), lastOverallStatus.c_str()) == 0)
        {
            sssnprintf(logLine, sizeof(logLine), "   %s (%d%%)\n", progressInfo->actionDescription.c_str(), progressInfo->actionPercentage);
            PrintToInstallPortal(logLine);
        }
        else
        {
            sssnprintf(logLine, sizeof(logLine), "%s (%d%%)\n", linePrefix, progressInfo->overallDescription.c_str(), progressInfo->overallPercentage);
            PrintToInstallPortal(logLine);
            sssnprintf(logLine, sizeof(logLine), "   %s (%d%%)\n", linePrefix, progressInfo->actionDescription.c_str(), progressInfo->actionPercentage);
            PrintToInstallPortal(logLine);
            
            lastOverallStatus = progressInfo->overallDescription;
        }

        // Send to deployment system
        if (arguments->statusUpdater != NULL)
            arguments->statusUpdater->SendMessage_Status(progressInfo->overallPercentage, progressInfo->overallDescription.c_str(), progressInfo->actionPercentage, progressInfo->actionDescription.c_str());
    }
    
    if (logFileHandle != -1)
    {
        close(logFileHandle);
        logFileHandle = -1;
    }
}

void SAVSecureCAdprintfInterface( LPCTSTR format, ... )
{
    va_list     marker;

    va_start(marker, format);
    if (SAVSecureCAdprintfInterfaceInstaller != NULL)
        SAVSecureCAdprintfInterfaceInstaller->LogMessage(NetwareInstaller::StatusCallbackFunctionEventType_Info, format, marker);
    va_end(marker);
}