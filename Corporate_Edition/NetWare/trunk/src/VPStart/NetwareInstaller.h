// Copyright (c) 2005 Symantec corporation.  All rights reserved.

#ifndef NETWAREINSTALLER
#define NETWAREINSTALLER

#ifdef NLM
    #include "pscan.h"
#endif
#include <string>
#include <list>
#include <algorithm>
#include <utility>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <direct.h>
#include <time.h>
#ifdef NLM
    #include <nwfinfo.h>
    #include <nwfileng.h>
    #include <nwnamspc.h>
#endif

class NetwareInstaller
{
public:
    // ** DATA TYPES **
    typedef std::basic_string<TCHAR> tcstring;
    typedef enum {InstallMode_Install, InstallMode_Reinstall, InstallMode_Automatic, InstallMode_Uninstall} InstallMode;
    typedef enum {StatusCallbackFunctionEventType_Error, StatusCallbackFunctionEventType_Warning, StatusCallbackFunctionEventType_Info, StatusCallbackFunctionEventType_Progress, StatusCallbackFunctionEventType_FinishedSignal} StatusCallbackFunctionEventType;
    // Called to report progress and log event info.  eventParam is interpreted differently for each event type.
    //      * Error, Warning, and Info:     an LPCTSTR
    //      * Progress:                     a StatusInfo_Progress*
    // NOTE:  percentages and action description only used for Progress events; all others report a single argument as overallDescription
    typedef void (*StatusCallbackFunction)( StatusCallbackFunctionEventType eventType, void* eventParam, void* userParam );
    struct StatusInfo_Progress
    {
        tcstring overallDescription;
        DWORD overallPercentage;
        tcstring actionDescription;
        DWORD actionPercentage;
    };
    
    // Constructor-destructor
    NetwareInstaller();
    ~NetwareInstaller();

    // Install, reinstall, uninstall script processor
    HRESULT ProcessScript( InstallMode mode, LPCTSTR scriptFilename, LPCTSTR moduleFilename, StatusCallbackFunction installStatusCallback, void* installStatusCallbackParam );
    // Processes the specified section of the script.  If variablesFilename is specified, this overrides the Configuration|variables
    // entry in the file.  If validateCoreVariables is true, the Configuration variables are validated and the script is only
    // executed if they pass.
    HRESULT ProcessScript( tcstring scriptFilename, tcstring sectionNameOverride, tcstring variablesFilenameOverride, InstallMode mode, bool validateCoreVariables, LPCTSTR moduleFilename, StatusCallbackFunction installStatusCallback, void* installStatusCallbackParam );
    
    // Sets *variableValue equal to the value of the specified variable in the variable file referred to by scriptFilename
    static HRESULT GetScriptVariable( tcstring scriptFilename, tcstring variableName, tcstring* variableValue );

    // Debugging
    void TestHarness( int argc, TCHAR* argv[] );

    // ** DATA TYPES **
    typedef std::list<tcstring> StringList;
    typedef std::vector<tcstring> StringArray;
private:
    typedef std::pair<tcstring, tcstring> StringPair;
    typedef std::list<StringPair> StringPairList;
    typedef std::pair<int, tcstring> IntStringPair;
    typedef std::vector<IntStringPair> IntStringPairArray;
    // An entry in a file table
    struct FileEntry
    {
        tcstring sourceFilename;
        tcstring targetFilename;
    };

    // ** CONSTANTS **
    #define ACCESS_MODE_READWRITE   6
    #define ACCESS_MODE_READ        4
    static const LPCTSTR AutoexecNCF_StartLine;     //";###### Symantec AntiVirus Corporate Edition SECTION START #######\n"
    static const LPCTSTR AutoexecNCF_CommentLine;   //";# Do not modify this section\n");
    static const LPCTSTR AutoexecNCF_LoadLine;      //"LOAD VPSTART.NLM\n");
    static const LPCTSTR AutoexecNCF_EndLine;       //";###### Symantec AntiVirus Corporate Edition SECTION END  #######\n");
    static const LPCTSTR Autoexec_Fullpathname;
    static const LPCTSTR Autoexec_BackupPathname;


    // ** FUNCTION MEMBERS **
    // Action Processing
    HRESULT ProcessActionSection( LPCTSTR sectionName );
    HRESULT DoActionStopModules( tcstring actionArgument );
    HRESULT DoActionCopyFiles( tcstring actionArgument );
    HRESULT DoActionUpdateFiles( tcstring actionArgument );
    HRESULT DoActionCopyRegistry( tcstring actionArgument );
    HRESULT DoActionUpdateRegistry( tcstring actionArgument );
    HRESULT DoActionDeleteDirectory( tcstring actionArgument );
    HRESULT DoActionCreateCertificate( tcstring actionArgument );
    HRESULT DoActionStartSAV( tcstring actionArgument );
    HRESULT DoActionDeleteFiles( tcstring actionArgument );
    HRESULT DoActionDeleteSelf( tcstring actionArgument );
    HRESULT DoActionChain( tcstring actionArgument );
    HRESULT DoActionCopyFileSpec( tcstring actionArgument );
    HRESULT DoActionStartReg( tcstring actionArgument );
    HRESULT DoActionStopReg( tcstring actionArgument );
    HRESULT DoActionStartNTS( tcstring actionArgument );
    HRESULT DoActionStopNTS( tcstring actionArgument );
    HRESULT DoActionCreatedirectory( tcstring actionArgument );
    HRESULT DoActionWaitForSAVstop( tcstring actionArguments );
    HRESULT DoActionWait( tcstring actionArguments );
    HRESULT DoActionAutoRegistry( tcstring actionArgument );
    HRESULT DoActionNukeDir( tcstring actionArgument );
    HRESULT DoActionRemoveDuplicateRTVSCAN( tcstring actionArgument );
    HRESULT DoActionAddAutoexecNCF( tcstring actionArguments );
    HRESULT DoActionRemoveAutoexecNCF( tcstring actionArguments );
    HRESULT DoActionOR( tcstring actionArguments );
    HRESULT DoActionSetInheritGroupSettings( tcstring actionArguments );
    HRESULT DoActionNDSlogin( tcstring actionArguments );
    HRESULT DoActionNWcreateGroups( tcstring actionArguments );
    HRESULT DoActionNWsetRights( tcstring actionArguments );
    HRESULT DoActionNWupdateLoginScript( tcstring actionArguments );
    HRESULT DoActionNWremoveGroups( tcstring actionArguments );
    HRESULT DoActionNWremoveRights( tcstring actionArguments );
    HRESULT DoActionNWupdateRemoveLoginScript( tcstring actionArguments );
    HRESULT DoActionNWsetAuthenticated( tcstring actionArguments );
    HRESULT DoActionSetIgnoreFailures( tcstring actionArguments );
    HRESULT DoActionChainSet( tcstring actionArgument );
    HRESULT DoActionDumpReg( tcstring actionArgument );
    HRESULT DoActionResolveAddress( tcstring actionArgument );
    HRESULT DoActionSetDomainGuid( tcstring actionArgument );

    // Action handler shared implementations
    HRESULT CopyRegistryEntries( LPCTSTR filename, LPCTSTR sectionName, StringPairList variables, bool overwriteExistingValues, bool ignoreFailures );
    HRESULT CopyRegistryEntriesValueLine( LPCTSTR entryName, LPCTSTR entryValue, HKEY keyHandle, tcstring currKeyPath, bool overwriteExistingValues );
    HRESULT CopyFiles( LPCTSTR filename, LPCTSTR sectionName, StringPairList variables, LPCTSTR sourceDirectory, bool overwriteAlways, bool ignoreFailures );
    HRESULT DeleteFiles( LPCTSTR filename, LPCTSTR sectionName, StringPairList variables, bool ignoreFailures );
    HRESULT ParseFileEntry( tcstring keyValue, FileEntry* entry, bool emptySourceOK = false );
    HRESULT CreateGroups( tcstring sectionName, bool install, bool useNDS );
    HRESULT SetRights( tcstring sectionName, bool install, bool useNDS );
    HRESULT UpdateLoginScript( tcstring sectionName, bool install, bool useNDS );

    // Variable handling
    static HRESULT ReplaceVariables( StringList* sectionLines, StringPairList variables );
    static HRESULT ReplaceVariables( StringPairList* sectionKeys, StringPairList variables );
    static HRESULT ReplaceVariables( tcstring* sourceLine, StringPairList variables );
    static HRESULT ReadVariablesFromFile( LPCTSTR filename, LPCTSTR sectionName, StringPairList* variableList );
    static HRESULT GetVariable( StringPairList variables, LPCTSTR variableName, tcstring* variableValue );
    static HRESULT SetVariable( StringPairList* variables, LPCTSTR variableName, tcstring newValue );
    HRESULT ReadVariableLogFailure( StringPairList variables, tcstring variableName, tcstring* variableValue, tcstring sectionName, tcstring filename );

    // INI file format helpers
    static HRESULT GetIniSection( LPCTSTR filename, LPCTSTR sectionName, StringPairList* sectionKeys );
    static HRESULT GetIniSection( LPCTSTR filename, LPCTSTR sectionName, StringList* sectionLines );
    static HRESULT GetKeys( StringPairList sectionKeys, LPCTSTR keyName, StringList* keyValues );
    // Object INI helpers
    HRESULT GetIniSectionResolved( tcstring sectionName, StringPairList* sectionKeys );
    HRESULT GetIniSectionResolved( tcstring sectionName, StringList* sectionLines );

    // String Utilities
    static LPCTSTR FindFirstWhitespace( LPCTSTR searchString );
    static LPCTSTR FindFirstNonWhitespace( LPCTSTR searchString );
    static LPCTSTR FindLastNonWhitespace( LPCTSTR searchString );
    static bool FindFirstWord( LPCTSTR searchString, LPCTSTR* wordStartPos, LPCTSTR* wordEndPos );
    static bool FindNextWord( LPCTSTR searchString, LPCTSTR* wordStartPos, LPCTSTR* wordEndPos );
    static void TerminateAtNewline( LPTSTR lineBuffer );
    static int StringReplace( LPTSTR lpszResult, size_t cchResult, LPCTSTR lpszStringIn, LPCTSTR lpszFind, LPCTSTR lpszReplaceWith );
    static int StringReplace( tcstring sourceString, tcstring findString, tcstring replaceString, tcstring* resultString );
    static int StringReplace( tcstring* subjectString, tcstring findString, tcstring replaceString );
    static HRESULT GetWords( tcstring sourceString, StringArray* words );

    // General helpers
    static HRESULT RecursiveCreateDirectory( tcstring pathname );
public:
    static tcstring GetParentPath( tcstring pathname );
private:
    // ** DATA TYPES **
#ifdef NLM
    typedef struct DOSDateTime
    {
        struct _DOSDate date;
        struct _DOSTime time;
    };
#endif
    typedef std::vector<WIN32_FIND_DATA> Win32FindDataArray;

    // ** FUNCTIONS **
    static tcstring GetNodeName( tcstring pathname );
    static DWORD ConvertBinaryStringToBytes( BYTE *dataBuffer, DWORD dataBufferSize, LPCTSTR sourceString );
    // NOTE:  Do postfix is necessary due to a #define of DeleteFile and RemoveDirectory
    HRESULT CopyFileDo( LPCTSTR sourceFilename, LPCTSTR targetFilename, BOOL failIfExists );
    HRESULT DeleteFileDo( LPCTSTR targetFilename, bool deleteIfReadonly = false );
    static HRESULT RemoveDirectoryDo( LPCTSTR targetDirectory );
    // Sets *fileList equal to the node names of the files meeting findspec.
    static HRESULT GetFiles( tcstring findspec, StringList* fileList );
    static HRESULT GetFiles( tcstring findspec, Win32FindDataArray* fileList );
    static HRESULT DirectoryExists( tcstring directoryPath );
    HRESULT NukeDir( tcstring directoryName, tcstring exceptionFilename = "" );
    static bool PrioritySortPredicate( IntStringPair elem1, IntStringPair elem2 );
    static HRESULT GetCBAAddr( LPCTSTR serverName, SENDCOM_HINTS hints, CBA_Addr* address, DWORD* savErrorCode );
    static HRESULT fsopen( LPCTSTR filename, LPCTSTR mode, FILE** fileHandle );
    static HRESULT FileErrnoToHRESULT( int erroNo );
    HRESULT LogInitNLMerror( DWORD returnValDW, bool install, tcstring commandName, tcstring commandArg );
    // Opens the specified registry key and logs any failure.  Also logs if key handle is already open (!= NULL)
    HRESULT OpenKeyLogMessage( HKEY parentKey, tcstring keyPath, HKEY* keyHandle, bool failureIsError = true );
    HRESULT DumpKeyToFile( HKEY keyHandle, tcstring keyPathname, FILE* targetFileHandle );

    // Logging
    // These functions call statusCallback if != NULL, otherwise do nothing
    void LogMessage( StatusCallbackFunctionEventType eventType, LPCTSTR format, ... );
public:
    void LogMessage( StatusCallbackFunctionEventType eventType, LPCTSTR format, va_list args );
private:
    void LogProgressMessage_Overall( DWORD overallProgressPercentage, LPCTSTR format, ... );
    void LogProgressMessage_Overall( LPCTSTR format, ... );
    void LogProgressMessage_Action( DWORD actionProgressPercentage, LPCTSTR format, ... );

    // Debugging
    static void DebugLogInstallMessage( StatusCallbackFunctionEventType eventType, void* eventParam, void* userParam );
    // Outputs all variables to the log callback
    HRESULT DumpVariables( StringPairList variables );

    // ** DATA MEMBERS **
    tcstring                    currModuleFilename;
    tcstring                    currScriptFilename;
    StringPairList              scriptConfigurationVariables;       // [Configuration] section variables; for debugging/support use only
    StringPairList              variables;
    tcstring                    scriptDirectory;
    tcstring                    sourceDirectory;
    tcstring                    installDirectory;                   // Location where SAV is or will be installed to
    bool                        alreadyInstalled;                   // TRUE if some version of SAV is already installed, else FALSE
    bool                        amsInstalled;                       // TRUE if AMS is detected as installed locally, else FALSE
    bool                        ignoreFailures;
    StatusCallbackFunction      statusCallback;
    void*                       statusCallbackParam;
    StatusInfo_Progress         currProgressStatus;
    bool                        instructionDebug;                   // TRUE to log out detailed debug info on instruction start/stop, else FALSE
};

#endif // NETWAREINSTALLER