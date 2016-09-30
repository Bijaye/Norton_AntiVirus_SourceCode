// Copyright (c) 2005 Symantec corporation.  All rights reserved.

#include "NetwareInstaller.h"
#include "SAVSecureCA.h"
#include "ScsCommsUtils.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"
#include "password.h"
// NLMUI is only needed for the EI_DSRET constant
#include "nlmui.h"

#ifdef NLM
    #include <nwerror.h>
    #define printf ConsolePrintf
#endif

// ** CONSTANTS **
const LPCTSTR NetwareInstaller::AutoexecNCF_StartLine       =   ";###### Symantec AntiVirus Corporate Edition SECTION START #######";
const LPCTSTR NetwareInstaller::AutoexecNCF_CommentLine     =   ";# Do not modify this section";
const LPCTSTR NetwareInstaller::AutoexecNCF_LoadLine        =   "LOAD VPSTART.NLM";
const LPCTSTR NetwareInstaller::AutoexecNCF_EndLine         =   ";###### Symantec AntiVirus Corporate Edition SECTION END  #######";
const LPCTSTR NetwareInstaller::Autoexec_Fullpathname       =   "SYS:\\SYSTEM\\Autoexec.ncf";
const LPCTSTR NetwareInstaller::Autoexec_BackupPathname     =   "SYS:\\SYSTEM\\Autoexec.bak";

// Prototypes
extern "C" BOOL RegFunctionsLoaded;
DWORD LoadRegistryFunctions(int compatibility);
DWORD UnloadRegistryFunctions(void);

// Prototypes for NDS/Binary functions from InitNLM
DWORD AuthenticateUser();
int DSPrelims( void );
DWORD ParseObjLine( char *line, char *q, char ds );
DWORD ParseProfileLogin( char* line, char* q );
DWORD ParseBinderyLogin( char* line, char* q );
extern "C" char AuthUser[256];
extern "C" char AuthPass[256];
extern "C" char Container[128];
extern "C" char ServerDN[MAX_PATH]; // the distinguished name of the server
extern "C" char DSNew;
extern "C" BOOL UsingDS;
extern "C" int Authenticated;
extern "C" int ConnectionNum;
extern "C" int Install;
// UI functions
extern "C" BOOL GetContextUI (char *context);
extern "C" void UseDefaultContext();

// PSCAN globals needed for communications
HKEY hMainKey           = NULL;
HKEY hLicensingKey      = NULL;
HKEY hPattManKey        = NULL;
HKEY hProductControlKey = NULL;
HKEY hClientKey         = NULL;
HKEY hForwardKey        = NULL;
HKEY hSystemKey         = NULL;
HKEY hISKey             = NULL;
HKEY hClientsKey        = NULL;
HKEY hAccountsKey       = NULL;
HKEY hCommonKey         = NULL;
char CurrentUserName[NAME_SIZE] = {""};


// Constructor-destructor
NetwareInstaller::NetwareInstaller() : alreadyInstalled(false), amsInstalled(false), ignoreFailures(false), statusCallback(NULL), statusCallbackParam(NULL), instructionDebug(false)
{
    currProgressStatus.overallPercentage = 0;
    currProgressStatus.actionPercentage = 0;
    RegFunctionsLoaded = FALSE;
}

NetwareInstaller::~NetwareInstaller()
{
    // Nothing for now
}


HRESULT NetwareInstaller::ProcessScript( InstallMode mode, LPCTSTR scriptFilename, LPCTSTR moduleFilename, StatusCallbackFunction installStatusCallback, void* installStatusCallbackParam )
{
    return ProcessScript(scriptFilename, "", "", mode, true, moduleFilename, installStatusCallback, installStatusCallbackParam );
}

HRESULT NetwareInstaller::ProcessScript( tcstring scriptFilename, tcstring sectionNameOverride, tcstring variablesFilenameOverride, InstallMode mode, bool validateCoreVariables, LPCTSTR moduleFilename, StatusCallbackFunction installStatusCallback, void* installStatusCallbackParam )
// Processes the specified section of the script.  If variablesFilenameOverride is specified, this overrides the Configuration|variables
// entry in the file.  If validateCoreVariables is true, the Configuration variables are validated and the script is only
// executed if they pass.
{
    time_t          currentTime;
    tm              currentTimeTM;
    bool            coreVariablesValid      = true;
    tcstring        variablesFilename;
    TCHAR           homeDirectory[3*MAX_PATH+1] = {""};
    bool            vpregStarted            = false;
    HKEY            keyHandle               = NULL;
    StringPair      newVariable;
    TCHAR           logLine[3*MAX_PATH+1]   = {""};
    bool            actionProcessed         = false;
    tcstring        argumentString;
    tcstring        sectionName;
    HRESULT         returnValHR             = E_FAIL;
    DWORD           returnValDW             = ERROR_OUT_OF_PAPER;

    // Initialize
    time(&currentTime);
    currentTimeTM = *localtime(&currentTime);
    currModuleFilename                      = moduleFilename;
    currScriptFilename                      = scriptFilename;
    scriptConfigurationVariables.clear();
    variables.clear();
    scriptDirectory                         = GetParentPath(scriptFilename);
    sourceDirectory                         = "";
    ignoreFailures                          = false;
    statusCallback                          = installStatusCallback;
    statusCallbackParam                     = installStatusCallbackParam;
    currProgressStatus.overallPercentage    = 0;
    currProgressStatus.overallDescription   = "";
    alreadyInstalled                        = false;
    amsInstalled                            = false;

    // Log out state information
    LogMessage(StatusCallbackFunctionEventType_Info, "**********");
    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROCESSINTRO), (int) MAINPRODUCTVERSION, (int) SUBPRODUCTVERSION, (int) INLINEPRODUCTVERSION, (int) BUILDNUMBER);
    _tcsftime(logLine, sizeof(logLine)/sizeof(logLine[0]), "Script execution began at %x %X", &currentTimeTM);
    LogMessage(StatusCallbackFunctionEventType_Info, logLine);
    LogMessage(StatusCallbackFunctionEventType_Info, "   Script filename:  %s", currScriptFilename.c_str());
    LogMessage(StatusCallbackFunctionEventType_Info, "   Module filename:  %s", currModuleFilename.c_str());
    
    // Read in key variables from the script
    returnValHR = ReadVariablesFromFile(scriptFilename.c_str(), "Configuration", &scriptConfigurationVariables);
    if (returnValHR == S_OK)
    {
        coreVariablesValid = coreVariablesValid && (ReadVariableLogFailure(scriptConfigurationVariables, "Variables", &variablesFilename, "Configuration", scriptFilename) == S_OK);
        coreVariablesValid = coreVariablesValid && (ReadVariableLogFailure(scriptConfigurationVariables, "SourceDir", &sourceDirectory, "Configuration", scriptFilename) == S_OK);
        coreVariablesValid = coreVariablesValid && (ReadVariableLogFailure(scriptConfigurationVariables, "InstallDir", &installDirectory, "Configuration", scriptFilename) == S_OK);

        if (coreVariablesValid)
            ReplaceVariables(&installDirectory, variables);
        else
            returnValHR = E_FAIL;
    }
    else if (returnValHR == S_FALSE)
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_SECTIONNOTFOUND), "Configuration", currScriptFilename.c_str());
        returnValHR = E_FAIL;
    }
    else if (returnValHR = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_SCRIPTFILENOTFOUND), currScriptFilename.c_str());
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_READINGSECTION), returnValHR, "Configuration", currScriptFilename.c_str());
    }
    if (FAILED(returnValHR) && validateCoreVariables)
        LogMessage(StatusCallbackFunctionEventType_Info, "Ignoring variable validation failure.");

    // Load all other variables
    if (SUCCEEDED(returnValHR))
    {
        if (variablesFilenameOverride == "")
        {
            returnValHR = ReadVariablesFromFile(variablesFilename.c_str(), NULL, &variables);
            if (returnValHR != S_OK)
            {
                // Variables filename may be a full path or relative path - support both
                returnValHR = ReadVariablesFromFile((scriptDirectory +tcstring("\\")+ variablesFilename).c_str(), NULL, &variables);
                if (returnValHR == S_OK)
                    variablesFilename = scriptDirectory +tcstring("\\")+ variablesFilename;
                else
                    LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_READINGVARIABLES), returnValHR, variablesFilename.c_str());
            }
        }
        else
        {
            returnValHR = ReadVariablesFromFile(variablesFilenameOverride.c_str(), NULL, &variables);
            if (SUCCEEDED(returnValHR))
                variablesFilename = variablesFilenameOverride;
        }
    }

    // Validate source directory - it may specify either an exact path or path relative to scriptDirectory
    if (SUCCEEDED(returnValHR))
    {
        // Resolve the sourcedir value
        ReplaceVariables(&sourceDirectory, variables);
        
        // Is this a relative path?
        if (_tcschr(sourceDirectory.c_str(), ':') == NULL)
        {
            // Yes, fully qualify the pathname
            sourceDirectory = scriptDirectory +tcstring("\\")+ sourceDirectory;
        }
        if (mode != InstallMode_Uninstall)
        {
            returnValHR = DirectoryExists(sourceDirectory);
            if (FAILED(returnValHR))
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_CANNOTLOCATESOURCEDIR), sourceDirectory.c_str());
            if (!validateCoreVariables)
            {
                returnValHR = S_OK;
                LogMessage(StatusCallbackFunctionEventType_Info, "Ignoring SourceDir validation failure.");
            }
        }
    }

    // Determine if we're already installed and override INSTALLDIR if so
    if (SUCCEEDED(returnValHR))
    {
        // Resolve the Installdir value
        ReplaceVariables(&installDirectory, variables);

        if (access("SYS:SYSTEM\\vpreg.nlm", ACCESS_MODE_READ) == 0)
        {
            LogMessage(StatusCallbackFunctionEventType_Info, "   (Temporarilly loading VPReg)");
            if (LoadRegistryFunctions(TRUE) != ERROR_SUCCESS)
            {
                if (LoadLibrary("SYS:SYSTEM\\vpreg.nlm") != NULL)
                    vpregStarted = true;
            }
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Info, "   (VPReg not found)");
        }
        if (LoadRegistryFunctions(TRUE) == ERROR_SUCCESS)
        {
            returnValDW = RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER, &keyHandle);

            if (returnValDW == ERROR_SUCCESS)
            {
                GetStr(keyHandle, "Home Directory", homeDirectory, sizeof(homeDirectory)/sizeof(homeDirectory[0]), "");
                if (GetVal(keyHandle, "AMSInstalled", 0) != 0)
                {
                    LogMessage(StatusCallbackFunctionEventType_Info, "   AMS installation detected.");
                    amsInstalled = true;
                }
                RegCloseKey(keyHandle);
                keyHandle = NULL;

                if ((_tcsicmp(homeDirectory, "") != 0) && SUCCEEDED(DirectoryExists(homeDirectory)))
                {
                    alreadyInstalled = true;
                    LogMessage(StatusCallbackFunctionEventType_Info, "   Existing installation detected at %s", installDirectory.c_str());
                    if (_tcsicmp(installDirectory.c_str(), homeDirectory) != 0)
                    {
                        LogMessage(StatusCallbackFunctionEventType_Info, "   *** Overriding install target directory of %s with the existing installation directory of %s.", installDirectory.c_str(), homeDirectory);
                        installDirectory = homeDirectory;
                    }
                }
            }
            UnloadRegistryFunctions();
        }
        if (vpregStarted)
        {
            LogMessage(StatusCallbackFunctionEventType_Info, "Unloading VPReg...");
            system("unload vpreg.nlm");
        }

        // Update ProgramDir (used by ScsComms)
        // NOTE:  sizeof does not work here because ProgramDir and HomeDir are externs
        _tcsncpy(ProgramDir, installDirectory.c_str(), IMAX_PATH);
        _tcsncpy(HomeDir, ProgramDir, IMAX_PATH);

        // Add INSTALLDIR built-in variable
        newVariable.first = "INSTALLDIR";
        newVariable.second = installDirectory;
        variables.push_back(newVariable);
        LogMessage(StatusCallbackFunctionEventType_Info, "   Added variable %s=%s", newVariable.first.c_str(), newVariable.second.c_str());
        
        // Add AMSINSTALLED built-in variable
        newVariable.first = "AMSINSTALLED";
        if (amsInstalled)
            newVariable.second = "1";
        else
            newVariable.second = "0";
        variables.push_back(newVariable);
        LogMessage(StatusCallbackFunctionEventType_Info, "   Added variable %s=%s", newVariable.first.c_str(), newVariable.second.c_str());
    }


    // Optional:  Ignore failures override
    if (SUCCEEDED(returnValHR))
    {
        if (SUCCEEDED(GetVariable(scriptConfigurationVariables, "IgnoreFailures", &argumentString)))
            ignoreFailures = (_ttoi(argumentString.c_str()) != 0);
        LogMessage(StatusCallbackFunctionEventType_Info, "   IgnoreFailures = %d", (DWORD) ignoreFailures);
        argumentString = "";
        if (SUCCEEDED(GetVariable(scriptConfigurationVariables, "InstructionDebug", &argumentString)))
            instructionDebug = (_ttoi(argumentString.c_str()) != 0);
        if (instructionDebug)
            LogMessage(StatusCallbackFunctionEventType_Info, "   InstructionDebug ON", (DWORD) instructionDebug);
        
    }

    // Finally, execute the specified section of the script
    if (SUCCEEDED(returnValHR))
    {
        // In automatic mode, we automatically determine whether to install or reinstall
        if (mode == InstallMode_Automatic)
        {
            if (alreadyInstalled)
            {
                LogMessage(StatusCallbackFunctionEventType_Info, "   Auto mode:  reinstall");
                mode = InstallMode_Reinstall;
            }
            else
            {
                LogMessage(StatusCallbackFunctionEventType_Info, "   Auto mode:  install");
                mode = InstallMode_Install;
            }
        }
        
        // Determine what section to run
        if (mode == InstallMode_Install)
            sectionName = "Install";
        else if (mode == InstallMode_Reinstall)
            sectionName = "Reinstall";
        else if (mode == InstallMode_Uninstall)
            sectionName = "Uninstall";
        if (sectionNameOverride != "")
            sectionName = sectionNameOverride;

        // Log out state information
        LogMessage(StatusCallbackFunctionEventType_Info, "   Variables filename:  %s", variablesFilename.c_str());
        LogMessage(StatusCallbackFunctionEventType_Info, "   Source directory:    %s", sourceDirectory.c_str());
        LogMessage(StatusCallbackFunctionEventType_Info, "   Install target:      %s", installDirectory.c_str());
        LogMessage(StatusCallbackFunctionEventType_Info, "   Action sequence:     %s", sectionName.c_str());
        LogMessage(StatusCallbackFunctionEventType_Info, "");

        // Process the section
        returnValHR = ProcessActionSection(sectionName.c_str());
        actionProcessed = true;

        // Always perform a StopNTS - if we don't, the server will abend with a "process exited with threads still running error"
        LogMessage(StatusCallbackFunctionEventType_Info, "Executing terminating StopNTS...");
        returnValHR = DoActionStopNTS("1");

        LogMessage(StatusCallbackFunctionEventType_Info, "NDS shutdown...");
        DeInitDS();
        
        // Notify remote system we're done.  Send this thrice as it's an important signal.
        LogMessage(StatusCallbackFunctionEventType_FinishedSignal, "Processing complete.");
        NTxSleep(100);
        LogMessage(StatusCallbackFunctionEventType_FinishedSignal, "Processing complete.");
        NTxSleep(100);
        LogMessage(StatusCallbackFunctionEventType_FinishedSignal, "Processing complete.");

        // Log out completion
        LogMessage(StatusCallbackFunctionEventType_Info, "");
        if (SUCCEEDED(returnValHR))
            LogMessage(StatusCallbackFunctionEventType_Info, "==> Script execution completed SUCCESSFULLY, code 0x%08x", returnValHR);
        else
            LogMessage(StatusCallbackFunctionEventType_Info, "==> Script execution FAILED, code 0x%08x", returnValHR);
    }
    // Log out finished time    
    time(&currentTime);
    currentTimeTM = *localtime(&currentTime);
    _tcsftime(logLine, sizeof(logLine)/sizeof(logLine[0]), "Execution finished at %x %X", &currentTimeTM);
    LogMessage(StatusCallbackFunctionEventType_Info, logLine);
    LogMessage(StatusCallbackFunctionEventType_Info, "");
    // Log out variables only if we executed the script
    if (actionProcessed)
        DumpVariables(variables);

    // Shutdown, clear state, and return
    currModuleFilename                      = "";
    currScriptFilename                      = "";
    scriptConfigurationVariables.clear();
    variables.clear();
    scriptDirectory                         = "";
    sourceDirectory                         = "";
    ignoreFailures                          = false;
    statusCallback                          = NULL;
    statusCallbackParam                     = NULL;
    currProgressStatus.overallPercentage    = 0;
    currProgressStatus.overallDescription   = "";
    alreadyInstalled                        = false;
    amsInstalled                            = false;

    return returnValHR;
}

HRESULT NetwareInstaller::GetScriptVariable( tcstring scriptFilename, tcstring variableName, tcstring* variableValue )
// Sets *variableValue equal to the value of the specified variable in the variable file referred to by scriptFilename
{
    StringPairList      scriptConfigurationVariables;
    StringPairList      variables;
    tcstring            variablesFilename;
    tcstring            scriptDirectory;
    HRESULT             returnValHR                     = E_FAIL;
    
    // Validate parameters
    if (scriptFilename == "")
        return E_INVALIDARG;
    if (variableName == "")
        return E_INVALIDARG;
    if (variableValue == NULL)
        return E_POINTER;

    // Read in variables filename
    returnValHR = ReadVariablesFromFile(scriptFilename.c_str(), "Configuration", &scriptConfigurationVariables);
    if (returnValHR == S_OK)
        returnValHR = GetVariable(scriptConfigurationVariables, "Variables", &variablesFilename);

    // Load variables
    if (SUCCEEDED(returnValHR))
    {
        returnValHR = ReadVariablesFromFile(variablesFilename.c_str(), NULL, &variables);
        if (returnValHR != S_OK)
        {
            // Variables filename may be a full path or relative path - support both
            scriptDirectory = GetParentPath(scriptFilename);
            variablesFilename = scriptDirectory +tcstring("\\")+ variablesFilename;
            returnValHR = ReadVariablesFromFile(variablesFilename.c_str(), NULL, &variables);
        }
    }
    
    // Resolve variable value
    if (SUCCEEDED(returnValHR))
        returnValHR = GetVariable(variables, variableName.c_str(), variableValue);

    return returnValHR;
}


// ** PRIVATE FUNCTIONS **
// Action Processing
HRESULT NetwareInstaller::ProcessActionSection( LPCTSTR sectionName )
// Processes an INI file section that is a series of action commands for install, reinstall, or uninstall.
// Returns S_OK on success, S_FALSE if section empty, else the error code of the failure
{
    StringPairList              sectionKeys;
    StringPairList::iterator    currSectionKey;
    DWORD                       currSectionKeyNo        = 0;
    bool                        firstErrorCodeSaved     = false;
    HRESULT                     returnValHR             = E_FAIL;
    HRESULT                     returnValHRb            = E_FAIL;
    DWORD                       returnValDW             = ERROR_OUT_OF_PAPER;

    // Validate parameters
    if (sectionName == NULL)
        return E_POINTER;

    // Initialize
    returnValHR = GetIniSection(currScriptFilename.c_str(), sectionName, &sectionKeys);
    if (returnValHR == S_OK)
    {
        returnValHR = ReplaceVariables(&sectionKeys, variables);
        if (FAILED(returnValHR))
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_REPLACINGVARIABLES), returnValHR, sectionName, currScriptFilename.c_str());
    }
    else if (returnValHR == S_FALSE)
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_SECTIONNOTFOUND), sectionName, currScriptFilename.c_str());
        returnValHR = E_FAIL;
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_READINGSECTION), returnValHR, sectionName, currScriptFilename.c_str());
    }
    
    if (SUCCEEDED(returnValHR))
    {
        // Base case - quit if nothing to do
        if (sectionKeys.size() == 0)
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_WARNING_EMPTYSECTION), sectionName, currScriptFilename.c_str());
            return S_FALSE;
        }
        returnValHR = S_OK;
        for (currSectionKey = sectionKeys.begin(); currSectionKey != sectionKeys.end(); currSectionKey++)
        {
            // Update overall progress
            currProgressStatus.overallPercentage = (DWORD) (( (float) currSectionKeyNo/ (float) sectionKeys.size()) * 100);
            currProgressStatus.overallDescription = "";
            currProgressStatus.actionPercentage = 0;
            currProgressStatus.actionDescription = "";
            
            if (instructionDebug)
                LogMessage(StatusCallbackFunctionEventType_Info, "ProcessActionSequence:  Running instruction %s(%s).", currSectionKey->first.c_str(), currSectionKey->second.c_str());

            // Determine the action requested and do it
            if (_tcsicmp(currSectionKey->first.c_str(), "StopModules") == 0)
                returnValHRb = DoActionStopModules(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "CopyFiles") == 0)
                returnValHRb = DoActionCopyFiles(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "UpdateFiles") == 0)
                returnValHRb = DoActionUpdateFiles(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "CopyRegistry") == 0)
                returnValHRb = DoActionCopyRegistry(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "UpdateRegistry") == 0)
                returnValHRb = DoActionUpdateRegistry(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "RemoveDirectory") == 0)
                returnValHRb = DoActionDeleteDirectory(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "CreateCertificate") == 0)
                returnValHRb = DoActionCreateCertificate(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "StartSAV") == 0)
                returnValHRb = DoActionStartSAV(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "DeleteFiles") == 0)
                returnValHRb = DoActionDeleteFiles(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "DeleteSelf") == 0)
                returnValHRb = DoActionDeleteSelf(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "Chain") == 0)
                returnValHRb = DoActionChain(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "CopyFileSpec") == 0)
                returnValHRb = DoActionCopyFileSpec(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "CopyFileSpec") == 0)
                returnValHRb = DoActionCopyFileSpec(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "StartReg") == 0)
                returnValHRb = DoActionStartReg(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "StopReg") == 0)
                returnValHRb = DoActionStopReg(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "StartNTS") == 0)
                returnValHRb = DoActionStartNTS(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "StopNTS") == 0)
                returnValHRb = DoActionStopNTS(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "CreateDirectory") == 0)
                returnValHRb = DoActionCreatedirectory(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "WaitForSAVstop") == 0)
                returnValHRb = DoActionWaitForSAVstop(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "Wait") == 0)
                returnValHRb = DoActionWait(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "AutoReg") == 0)
                returnValHRb = DoActionAutoRegistry(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "NukeDir") == 0)
                returnValHRb = DoActionNukeDir(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "RemoveDupeRTVScan") == 0)
                returnValHRb = DoActionRemoveDuplicateRTVSCAN(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "AddAutoexecNCF") == 0)
                returnValHRb = DoActionAddAutoexecNCF(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "RemoveAutoexecNCF") == 0)
                returnValHRb = DoActionRemoveAutoexecNCF(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "OR") == 0)
                returnValHRb = DoActionOR(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "SetInheritGroupSettings") == 0)
                returnValHRb = DoActionSetInheritGroupSettings(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "NDSlogin") == 0)
                returnValHRb = DoActionNDSlogin(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "NWcreateGroups") == 0)
                returnValHRb = DoActionNWcreateGroups(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "NWsetRights") == 0)
                returnValHRb = DoActionNWsetRights(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "NWupdateLoginScript") == 0)
                returnValHRb = DoActionNWupdateLoginScript(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "NWremoveGroups") == 0)
                returnValHRb = DoActionNWremoveGroups(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "NWremoveRights") == 0)
                returnValHRb = DoActionNWremoveRights(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "NWremoveLoginScript") == 0)
                returnValHRb = DoActionNWupdateRemoveLoginScript(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "NWsetAuthenticated") == 0)
                returnValHRb = DoActionNWsetAuthenticated(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "SetIgnoreFailures") == 0)
                returnValHRb = DoActionSetIgnoreFailures(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "ChainSet") == 0)
                returnValHRb = DoActionChainSet(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "DumpReg") == 0)
                returnValHRb = DoActionDumpReg(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "ResolveAddress") == 0)
                returnValHRb = DoActionResolveAddress(currSectionKey->second);
            else if (_tcsicmp(currSectionKey->first.c_str(), "SetDomainGuid") == 0)
                returnValHRb = DoActionSetDomainGuid(currSectionKey->second);
            else // Unrecognized instruction
            {
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_UNRECOGNIZEDCOMMAND), currSectionKey->first.c_str(), sectionName, currScriptFilename.c_str());
                returnValHRb = E_FAIL;
            }
            if (instructionDebug)
                LogMessage(StatusCallbackFunctionEventType_Info, "ProcessActionSequence:  Ran instruction %s(%s).", currSectionKey->first.c_str(), currSectionKey->second.c_str());

            // Handle errors
            if (FAILED(returnValHRb))
            {
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_COMMANDFAILED), currSectionKey->first.c_str(), sectionName, currScriptFilename.c_str());
                if (!firstErrorCodeSaved)
                {
                    returnValHR = returnValHRb;
                    firstErrorCodeSaved = true;
                }
                if (!ignoreFailures)
                    break;
            }
            currSectionKeyNo += 1;
        }
        
        // Log out 100% completion of this section
        if (SUCCEEDED(returnValHR))
            LogProgressMessage_Overall(100, LS(IDS_NWINSTALL_PROGRESS_COMPLETE_OK), sectionName);
        else
            LogProgressMessage_Overall(100, LS(IDS_NWINSTALL_PROGRESS_COMPLETE_ERROR), sectionName);
    }
    if (instructionDebug)
        LogMessage(StatusCallbackFunctionEventType_Info, "ProcessActionSequence:  done with sequence %s.", sectionName);
    return returnValHR;
}

// Action handlers
HRESULT NetwareInstaller::DoActionStopModules( tcstring actionArgument )
{
    StringArray                 moduleNames;
    StringArray::iterator       currModule;
    tcstring                    currModuleName;
    DWORD                       actionPercent       = 0;
    DWORD                       currModuleNo        = 0;
    TCHAR                       unloadCommand[512]  = {""};
    HRESULT                     returnValHR         = E_FAIL;
    DWORD                       returnValDW         = ERROR_OUT_OF_PAPER;
    
    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_STOPPINGMODULES));
    returnValHR = GetWords(actionArgument, &moduleNames);
    if (SUCCEEDED(returnValHR))
    {
        returnValHR = S_OK;
        for (currModule = moduleNames.begin(); currModule != moduleNames.end(); currModule++, currModuleNo++)
        {
            currModuleName = *currModule;
            actionPercent = (DWORD) (( (float) currModuleNo/ (float) moduleNames.size()) * 100);
            sssnprintf(unloadCommand, sizeof(unloadCommand), "UNLOAD %s", currModule->c_str());
            LogProgressMessage_Action(actionPercent, currModule->c_str());
            if (FindNLMHandle(currModule->c_str()))
            {
                returnValDW = system(unloadCommand);
                // A delay IS necessary here for the module to successfully unload.
                NTxSleep(250);
                if (returnValDW != 0)
                {
                    LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_STOPPING_MODULE), returnValDW, currModule->c_str());
                    returnValHR = E_FAIL;
                }
            }
            else
            {
                LogMessage(StatusCallbackFunctionEventType_Info, "Module %s not loaded.", currModule->c_str());
            }
        }        
        LogProgressMessage_Action(100, currModuleName.c_str());
    }
    return returnValHR;
}

HRESULT NetwareInstaller::DoActionCopyFiles( tcstring actionArgument )
{
    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_COPYINGFILES));
    return CopyFiles(currScriptFilename.c_str(), actionArgument.c_str(), variables, sourceDirectory.c_str(), true, ignoreFailures);
}

HRESULT NetwareInstaller::DoActionUpdateFiles( tcstring actionArgument )
{
    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_UPDATINGFILES));
    return CopyFiles(currScriptFilename.c_str(), actionArgument.c_str(), variables, sourceDirectory.c_str(), false, ignoreFailures);
}

HRESULT NetwareInstaller::DoActionCopyRegistry( tcstring actionArgument )
{
    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_MERGINGREG));
    return CopyRegistryEntries(currScriptFilename.c_str(), actionArgument.c_str(), variables, true, ignoreFailures);
}

HRESULT NetwareInstaller::DoActionUpdateRegistry( tcstring actionArgument )
{
    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_UPDATINGREG));
    return CopyRegistryEntries(currScriptFilename.c_str(), actionArgument.c_str(), variables, false, ignoreFailures);
}

HRESULT NetwareInstaller::DoActionDeleteDirectory( tcstring actionArgument )
{
    tcstring::size_type     separatorPos                = tcstring::npos;
    tcstring                deleteIgnoreFailuresString;
    tcstring                targetDirectoryName;
    bool                    deleteIgnoreFailures       = false;
    HRESULT                 returnValHR                 = HRESULT_FROM_WIN32(ERROR_OUT_OF_PAPER);

    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_DELETINGDIR));
    LogProgressMessage_Action(0, "%s", actionArgument.c_str());
    separatorPos = actionArgument.find_first_of(',');
    if (separatorPos != tcstring::npos)
    {
        deleteIgnoreFailuresString = actionArgument.substr(0, separatorPos);
        targetDirectoryName = actionArgument.substr(separatorPos+1);
        
        if ((deleteIgnoreFailuresString != "") && (targetDirectoryName != ""))
        {
            deleteIgnoreFailures = (_ttoi(deleteIgnoreFailuresString.c_str()) != 0);

            returnValHR = RemoveDirectoryDo(targetDirectoryName.c_str());
            if (returnValHR == S_FALSE)
                LogMessage(StatusCallbackFunctionEventType_Info, LS(IDS_NWINSTALL_INFO_DIRALREADYDELETED), targetDirectoryName.c_str());
            else if (FAILED(returnValHR))
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_REMOVINGDIRECTORY), returnValHR, targetDirectoryName.c_str());
            LogProgressMessage_Action(100, "%s", targetDirectoryName.c_str());
            
            if (deleteIgnoreFailures)
                returnValHR = S_OK;
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_NOARGUMENTS), "DeleteDirectory", actionArgument.c_str());
        }
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_SEPARATORNOTFOUND), "DeleteDirectory", actionArgument.c_str());
    }
    
    return returnValHR;
}

HRESULT NetwareInstaller::DoActionCreateCertificate( tcstring actionArgument )
{
    TCHAR       privateKeyFilespec[2*MAX_PATH+1]        = {""};
    StringList  files;
    HKEY        keyHandle                               = NULL;
    HKEY        domainDataKeyHandle                     = NULL;
    bool        useRootAuthentication                   = false;
    TCHAR       parentServerName[MAX_PATH+1]            = {""};
    DWORD       stringBufferSize                        = 0;
    TCHAR       savDomainUsername[MAX_PATH+1]           = {""};
    TCHAR       savDomainPassword[MAX_PATH+1]           = {""};
    TCHAR       domainName[MAX_PATH+1]                  = {""};
    bool        isPrimaryServer                         = false;
    bool        errorReadingConfiguration               = false;
    TCHAR       sourcePath[2*MAX_PATH+1]                = {""};
    HRESULT     returnValHR                             = E_FAIL;
    DWORD       returnValDW                             = ERROR_FILE_NOT_FOUND;


    // Validate state
    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_CERTIFICATE_HANDLING));
    if (!RegFunctionsLoaded)
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_REGISTRY_NOTLOADED));
        return E_FAIL;
    }
    
    try
    {
        // Determine if there is anything to do
        sssnprintf(privateKeyFilespec, sizeof(privateKeyFilespec), "%s\\%s\\*.*.*%s", installDirectory.c_str(), SYM_SCSCOMM_DIR_CER_PKEY, SYM_SCSCOMM_PRIV_KEY_EXT);
        GetFiles(privateKeyFilespec, &files);
        if (files.size() > 0)
        {
            LogProgressMessage_Action(100, LS(IDS_NWINSTALL_INFO_CERTIFICATE_EXISTINGCERTSFOUND));
            return S_OK;
        }
        
        // Do stuff
        LogProgressMessage_Action(0, LS(IDS_NWINSTALL_PROGRESS_CERTIFICATE_CREATINGDIRS));
        returnValHR = CreatePKIdirectories(installDirectory.c_str());
        if (SUCCEEDED(returnValHR))
        {
            returnValHR = S_OK;
            // Determine if this is a primary server or not
            returnValDW = RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER "\\" szReg_Key_DomainData, &domainDataKeyHandle);
            if (returnValDW == ERROR_SUCCESS)
            {
                LogMessage(StatusCallbackFunctionEventType_Info, "Primary server (domain data key found)");
                isPrimaryServer = true;
                RegCloseKey(domainDataKeyHandle);
                domainDataKeyHandle = NULL;
            }

            // Read configuration data from SAV registry
            LogProgressMessage_Action(20, LS(IDS_NWINSTALL_PROGRESS_CERTIFICATE_READINGCONFIG));
            returnValDW = RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER, &keyHandle);
            if (returnValDW == ERROR_SUCCESS)
            {
                stringBufferSize = sizeof(parentServerName)/sizeof(parentServerName[0]);
                returnValDW = SymSaferRegQueryValueEx(keyHandle, "Parent", 0, NULL, (LPBYTE) (parentServerName), &stringBufferSize);
                if (_tcsicmp(parentServerName, "") == 0)
                {
                    LogMessage(StatusCallbackFunctionEventType_Info, "Primary server (Parent not found)");
                    isPrimaryServer = true;
                }
                else
                {
                    LogMessage(StatusCallbackFunctionEventType_Info, "Not primary server");
                }

                stringBufferSize = sizeof(domainName)/sizeof(domainName[0]);
                returnValDW = SymSaferRegQueryValueEx(keyHandle, "DomainName", 0, NULL, (LPBYTE) (domainName), &stringBufferSize);
                if (returnValDW != ERROR_SUCCESS)
                    LogMessage(StatusCallbackFunctionEventType_Info, "Domain name not found (%d).", returnValDW);
                errorReadingConfiguration |= (returnValDW != ERROR_SUCCESS);
                returnValHR = SUCCEEDED(returnValHR) ? HRESULT_FROM_WIN32(returnValDW) : returnValHR;

                stringBufferSize = sizeof(savDomainUsername)/sizeof(savDomainUsername[0]);
                returnValDW = SymSaferRegQueryValueEx(keyHandle, szReg_Val_DomainAdminUsername, 0, NULL, (LPBYTE) (savDomainUsername), &stringBufferSize);
                if (returnValDW != ERROR_SUCCESS)
                    LogMessage(StatusCallbackFunctionEventType_Info, "Error %d reading SAV domain username value from registry.", returnValDW);
                returnValHR = SUCCEEDED(returnValHR) ? HRESULT_FROM_WIN32(returnValDW) : returnValHR;

                stringBufferSize = sizeof(savDomainPassword)/sizeof(savDomainPassword[0]);
                returnValDW = SymSaferRegQueryValueEx(keyHandle, szReg_Val_DomainPassword, 0, NULL, (LPBYTE) (savDomainPassword), &stringBufferSize);
                if (returnValDW != ERROR_SUCCESS)
                    LogMessage(StatusCallbackFunctionEventType_Info, "Error %d reading SAV domain password value from registry.", returnValDW);
                returnValHR = SUCCEEDED(returnValHR) ? HRESULT_FROM_WIN32(returnValDW) : returnValHR;

                RegCloseKey(keyHandle);
                keyHandle = NULL;
            }
            else
            {
                LogMessage(StatusCallbackFunctionEventType_Info, "Error %d opening registry key %s.", returnValDW, REGHEADER);
                returnValHR = HRESULT_FROM_WIN32(returnValDW);
            }

            // If not a primary server, ensure we can talk to the primary before proceeding.
            if (SUCCEEDED(returnValHR) && !isPrimaryServer)
            {
                CBA_Addr    address;
                BYTE        dataBuffer[MAX_PACKET_DATA]                         = {0};
                DWORD       noItemsSent                                         = 1;
                WORD        retrievedDataSize                                   = 0;
                BYTE*       retrievedData                                       = NULL;
                DWORD       returnValDW                                         = ERROR_OUT_OF_PAPER;

                // Resolve the primary's name so an AddresCache entry is created for it, in case we don't have one.
                // This ensures CreateDomainGUID will find what it needs, and that subsequent comms will work OK,
                // and also clarifies a common error.
                returnValHR = S_OK;
                memset(&address,0,sizeof(CBA_Addr));
                LogMessage(StatusCallbackFunctionEventType_Info, "Resolving server name %s...", parentServerName);
                returnValDW = FindComputerByName(parentServerName, &address);
                if (returnValDW == ERROR_SUCCESS)
                {
                    returnValHR = S_OK;
                    LogMessage(StatusCallbackFunctionEventType_Info, "Resolved %s to 0x%08x.\n", parentServerName, address.dstAddr.netAddr.ipAddr);
                }
                else
                {
                    returnValHR = HRESULT_FROM_WIN32(returnValDW);
                    LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_RESOLVENAME), HRESULT_FROM_WIN32(returnValDW), parentServerName);
                }
            }

            // Generate certificates
            if (SUCCEEDED(returnValHR))
            {
                TCHAR       savDomainPasswordClearText[PASS_MAX_PLAIN_TEXT_BYTES+1]   = {""};

                LogProgressMessage_Action(20, LS(IDS_NWINSTALL_PROGRESS_CERTIFICATE_CREATINGINITACCOUNT));
                CreateDomainGuid(parentServerName);
                UnMakeEP(savDomainPassword, sizeof(savDomainPassword), PASS_KEY1, PASS_KEY2, savDomainPasswordClearText, sizeof(savDomainPasswordClearText) );
                CreateInitialAccount(savDomainUsername, savDomainPasswordClearText);

                if (isPrimaryServer)
                {
                    LogProgressMessage_Action(40, LS(IDS_NWINSTALL_PROGRESS_CERTIFICATE_CREATEPRIMARY));
                    LogMessage(StatusCallbackFunctionEventType_Info, "Creating root certificates...");
                    CreateRootCertificates(installDirectory.c_str());
                }
                else
                {
                    // Copy certificates supplied by admin into our repository.  These are temporarilly copied to InstallPKItemp
                    // by Setup32.
                    LogMessage(StatusCallbackFunctionEventType_Info, "Copying any certificates in %s...", sourcePath);
                    sssnprintf(sourcePath, sizeof(sourcePath), "%s\\%s", installDirectory.c_str(), "InstallPKItemp");
                    CopyAllTrustedRootCerts(sourcePath, installDirectory.c_str());
                    ::NukeDir(sourcePath, true);

                    if (SUCCEEDED(returnValHR))
                    {
                        LogProgressMessage_Action(40, LS(IDS_NWINSTALL_PROGRESS_CERTIFICATE_CREATESECONDARY));
                        // Generate server certificate
                        LogMessage(StatusCallbackFunctionEventType_Info, "Creating server certificate...");
                        CreateServerPKI(false, useRootAuthentication, installDirectory.c_str(), parentServerName, savDomainUsername, savDomainPasswordClearText);

                        // Generate SSC login certificate
                        LogMessage(StatusCallbackFunctionEventType_Info, "Creating login certificate...");
                        CreateServerPKI(true, useRootAuthentication, installDirectory.c_str(), parentServerName, savDomainUsername, savDomainPasswordClearText);
                    }
                }
                _tcsncpy(savDomainPasswordClearText, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", sizeof(savDomainPasswordClearText)/sizeof(savDomainPasswordClearText[0]));
            }
        }
    }
    catch(...)
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_CERTIFICATE_EXCEPTION));
        returnValHR = E_FAIL;
    }

    return returnValHR;
}

HRESULT NetwareInstaller::DoActionDumpReg( tcstring actionArgument )
{
    FILE*   targetFileHandle    = NULL;
    HKEY    startKey            = HKEY_LOCAL_MACHINE;

    if (actionArgument == "")
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_NOARGUMENTS), "DumpReg", actionArgument.c_str());
        return E_FAIL;
    }
    if (!RegFunctionsLoaded)
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_REGISTRY_NOTLOADED));
        return E_FAIL;
    }

    targetFileHandle = fopen(actionArgument.c_str(), "wt");
    if (targetFileHandle != NULL)
    {
        DumpKeyToFile(startKey, "HKLM", targetFileHandle);
        
        fclose(targetFileHandle);
        targetFileHandle = NULL;
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_OPENINGFILE), FileErrnoToHRESULT(errno), actionArgument.c_str());
    }

    return S_OK;
}

HRESULT NetwareInstaller::DoActionResolveAddress( tcstring actionArgument )
{
    HKEY        keyHandle                                           = NULL;
    TCHAR       nameBuffer[MAX_PATH]                                = {""};
    DWORD       nameBufferSize                                      = 0;
    LPTSTR      separatorPos                                        = NULL;
    CBA_Addr    address;
    BYTE        dataBuffer[MAX_PACKET_DATA]                         = {0};
    DWORD       noItemsSent                                         = 1;
    WORD        retrievedDataSize                                   = 0;
    BYTE*       retrievedData                                       = NULL;
    DWORD       returnValDW = ERROR_OUT_OF_PAPER;
    HRESULT     returnValHR = E_FAIL;
    
    // Validate state
    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_RESOLVINGNAMES));
    if (!RegFunctionsLoaded)
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_REGISTRY_NOTLOADED));
        return E_FAIL;
    }
    
    returnValHR = S_OK;
    if (actionArgument == "")
    {
        LogMessage(StatusCallbackFunctionEventType_Info, "Resolving self and parent server addresses...");
        // Determine this computer's name and resolve it.  This *should* be done automatically by StartNTS,
        // but do this just to be safe.
        returnValDW = gethostname(nameBuffer, sizeof(nameBuffer)/sizeof(nameBuffer[0]));
        if (returnValDW == ERROR_SUCCESS)
        {
            separatorPos = _tcschr(nameBuffer, '.');
            if (separatorPos != NULL)
                *separatorPos = NULL;
            ZeroMemory(&address, sizeof(address));
            LogMessage(StatusCallbackFunctionEventType_Info, "Resolving server name %s...", nameBuffer);
            returnValDW = FindComputerByName(nameBuffer, &address);
            if (returnValDW == ERROR_SUCCESS)
                LogMessage(StatusCallbackFunctionEventType_Info, "Resolved %s to 0x%08x.\n", nameBuffer, address.dstAddr.netAddr.ipAddr);
            else
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_RESOLVENAME), HRESULT_FROM_WIN32(returnValDW), nameBuffer);
            returnValHR = SUCCEEDED(returnValHR) ? HRESULT_FROM_WIN32(returnValDW) : returnValHR;
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_GETTINGSERVERNAME), errno);
            returnValHR = E_FAIL;
        }

        // Resolve the primary server's name
        nameBuffer[0] = NULL;
        returnValDW = RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER, &keyHandle);
        if (returnValDW == ERROR_SUCCESS)
        {
            nameBufferSize = sizeof(nameBuffer)/sizeof(nameBuffer[0]);
            returnValDW = SymSaferRegQueryValueEx(keyHandle, "Parent", 0, NULL, (LPBYTE) (nameBuffer), &nameBufferSize);
            if (_tcsicmp(nameBuffer, "") == 0)
                LogMessage(StatusCallbackFunctionEventType_Info, "Parent is %s", nameBuffer);
            RegCloseKey(keyHandle);
            keyHandle = NULL;
        }
        if (_tcsicmp(nameBuffer, "") != 0)
        {
            ZeroMemory(&address, sizeof(address));
            LogMessage(StatusCallbackFunctionEventType_Info, "Resolving server name %s...", nameBuffer);
            returnValDW = FindComputerByName(nameBuffer, &address);
            if (returnValDW == ERROR_SUCCESS)
                LogMessage(StatusCallbackFunctionEventType_Info, "Resolved %s to 0x%08x.\n", nameBuffer, address.dstAddr.netAddr.ipAddr);
            else
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_RESOLVENAME), HRESULT_FROM_WIN32(returnValDW), nameBuffer);
            returnValHR = SUCCEEDED(returnValHR) ? HRESULT_FROM_WIN32(returnValDW) : returnValHR;
        }
    }
    else
    {
        // Resolve the requested name
        ZeroMemory(&address, sizeof(address));
        LogMessage(StatusCallbackFunctionEventType_Info, "Resolving server name %s...", actionArgument.c_str());
        returnValDW = FindComputerByName(actionArgument.c_str(), &address);
        if (returnValDW == ERROR_SUCCESS)
            LogMessage(StatusCallbackFunctionEventType_Info, "Resolved %s to 0x%08x.\n", actionArgument.c_str(), address.dstAddr.netAddr.ipAddr);
        else
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_RESOLVENAME), HRESULT_FROM_WIN32(returnValDW), nameBuffer);
        returnValHR = SUCCEEDED(returnValHR) ? HRESULT_FROM_WIN32(returnValDW) : returnValHR;
    }

    return returnValHR;
}

HRESULT NetwareInstaller::DoActionSetDomainGuid( tcstring actionArgument )
{
    HKEY        keyHandle               = NULL;
    TCHAR       nameBuffer[MAX_PATH]    = {""};
    DWORD       nameBufferSize          = 0;
    DWORD       returnValDW             = ERROR_OUT_OF_PAPER;
    HRESULT     returnValHR             = E_FAIL;
    
    if (!RegFunctionsLoaded)
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_REGISTRY_NOTLOADED));
        return E_FAIL;
    }
    
    if (_tcsicmp(actionArgument.c_str(), "parent") == 0)
    {
        // Automatic mode - resolve parent name, then set it as requested
        LogMessage(StatusCallbackFunctionEventType_Info, "Automatically regenerating GUID.");
        // Read the primary server name
        returnValDW = RegOpenKey(HKEY_LOCAL_MACHINE, _T(szReg_Key_Main), &keyHandle);
        if (returnValDW == ERROR_SUCCESS)
        {
            nameBufferSize = sizeof(nameBuffer)/sizeof(nameBuffer[0]);
            returnValDW = SymSaferRegQueryValueEx(keyHandle, "Parent", 0, NULL, (LPBYTE) (nameBuffer), &nameBufferSize);
            if (_tcsicmp(nameBuffer, "") == 0)
                LogMessage(StatusCallbackFunctionEventType_Info, "Parent is %s", nameBuffer);
            RegCloseKey(keyHandle);
            keyHandle = NULL;
        }

        // Set the GUID
        returnValHR = CreateDomainGuid(nameBuffer);
    }
    else if (_tcsicmp(actionArgument.c_str(), "cert") == 0)
    {
        tcstring                            certPath;
        Win32FindDataArray                  certFiles;
        Win32FindDataArray::iterator        currFile;
        FILETIME                            mostRecentFileTime;
        LPCTSTR                             mostRecentFileName      = NULL;
        LPTSTR                              firstSeparatorPos       = NULL;
        LPTSTR                              secondSeparatorPos      = NULL;
        tcstring                            guidString;
        GUID                                guidGUID                = GUID_NULL;
        bool                                guidOK                  = false;
        
        // Determine from certificates on machine
        certPath = installDirectory +"//"+ SYM_SCSCOMM_DIR_CER_CERTS +"*.*.*"+ SYM_SCSCOMM_CER_EXT;
        LogMessage(StatusCallbackFunctionEventType_Info, "Cert path is %s", certPath.c_str());
        returnValHR = GetFiles(installDirectory +"/"+ SYM_SCSCOMM_DIR_CER_CERTS +"/*.*.*"+ SYM_SCSCOMM_CER_EXT, &certFiles);
        returnValHR = S_FALSE;
        ZeroMemory(&mostRecentFileTime, sizeof(mostRecentFileTime));
        for (currFile = certFiles.begin(); currFile != certFiles.end(); currFile++)
        {
            returnValHR = S_OK;
            if (currFile->ftCreationTime > mostRecentFileTime)
            {
                currFile->ftCreationTime = mostRecentFileTime;
                mostRecentFileName = currFile->cFileName;
            }
        }
        if (mostRecentFileName != NULL)
        {
            // Extract the domain GUID string
            firstSeparatorPos = _tcschr(mostRecentFileName, '.');
            if (firstSeparatorPos != NULL)
                secondSeparatorPos = _tcschr(CharNext(firstSeparatorPos), '.');
            if ((firstSeparatorPos != NULL) && (secondSeparatorPos != NULL))
            {
                *secondSeparatorPos = NULL;
                guidString = CharNext(firstSeparatorPos);
                if (guidString.size() == (sizeof(guidGUID)*2))
                    guidOK = true;
                
            }
            if (guidOK)
            {
                ConvertBinaryStringToBytes((BYTE*) &guidGUID, sizeof(guidGUID), guidString.c_str());

                returnValDW = RegOpenKey(HKEY_LOCAL_MACHINE, _T(szReg_Key_Main), &keyHandle);
                if (returnValDW == ERROR_SUCCESS)
                {
                    returnValDW = RegSetValueEx(keyHandle, szReg_Val_DomainGUID, 0, REG_BINARY, (BYTE*) &guidGUID, sizeof(guidGUID));
                    returnValHR = HRESULT_FROM_WIN32(returnValDW);
                    if (returnValDW != ERROR_SUCCESS)
                        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_SETTINGBINARY), returnValHR, szReg_Val_DomainGUID, guidString.c_str());
                    RegCloseKey(keyHandle);
                    keyHandle = NULL;
                }
                else
                {
                    returnValHR = HRESULT_FROM_WIN32(returnValDW);
                    LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_OPENINGKEY), returnValHR, _T(szReg_Key_Main));
                }

            }
            else
            {
                returnValHR = E_FAIL;
                LogMessage(StatusCallbackFunctionEventType_Info, "Error parsing filename string for most recent cert filename %s.", mostRecentFileName);
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_NOCERTSFOUND));
            }
        }
        else
        {
            returnValHR = E_FAIL;
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_NOCERTSFOUND));
        }
    }
    else if (actionArgument != "")
    {
        GUID                                guidGUID                = GUID_NULL;

        // Set to specified GUID
        if (actionArgument.size() == (sizeof(guidGUID)*2))
        {
            ConvertBinaryStringToBytes((BYTE*) &guidGUID, sizeof(guidGUID), actionArgument.c_str());

            returnValDW = RegOpenKey(HKEY_LOCAL_MACHINE, _T(szReg_Key_Main), &keyHandle);
            if (returnValDW == ERROR_SUCCESS)
            {
                returnValDW = RegSetValueEx(keyHandle, szReg_Val_DomainGUID, 0, REG_BINARY, (BYTE*) &guidGUID, sizeof(guidGUID));
                returnValHR = HRESULT_FROM_WIN32(returnValDW);
                if (returnValDW != ERROR_SUCCESS)
                    LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_SETTINGBINARY), returnValHR, szReg_Val_DomainGUID, actionArgument.c_str());
                RegCloseKey(keyHandle);
                keyHandle = NULL;
            }
            else
            {
                returnValHR = HRESULT_FROM_WIN32(returnValDW);
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_OPENINGKEY), returnValHR, _T(szReg_Key_Main));
            }

        }
        else
        {
            returnValHR = E_FAIL;
            LogMessage(StatusCallbackFunctionEventType_Info, "Error parsing guid string %s", actionArgument.c_str());
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_NOCERTSFOUND));
        }
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_NOARGUMENTS), "SetDomainGUID", actionArgument.c_str());
        returnValHR = E_FAIL;
    }

    // Return result    
    return returnValHR;
}

HRESULT NetwareInstaller::DoActionStartSAV( tcstring actionArgument )
{
    // Read in the list of NLMs to load in order to start RTVScan
    IntStringPairArray              modulesToLoad;
    IntStringPair                   newModuleToLoad;
    IntStringPairArray::iterator    currModule;
    TCHAR                           moduleLoadCommand[1024] = {""};
    DWORD                           actionPercentage        = 0;
    DWORD                           currModuleNo            = 0;
    HKEY                            keyHandle               = NULL;
    DWORD                           valueNo                 = 0;
    TCHAR                           valueName[MAX_PATH]     = {""};
    DWORD                           valueNameSize           = 0;
    DWORD                           valueType               = REG_DWORD;
    DWORD                           valueData               = 0;
    DWORD                           valueDataSize           = 0;
    tcstring                        currModuleName;
    bool                            appendNLM               = false;
    LPCTSTR                         nlmPos                  = NULL;
    HRESULT                         returnValHR             = E_FAIL;
    DWORD                           returnValDW             = ERROR_OUT_OF_PAPER;
    
    LogProgressMessage_Overall(LS(IDS_STARTING_LDVP));
    if (RegFunctionsLoaded)
    {
        returnValDW = RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER"\\NLMsToLoad", &keyHandle);
        if (returnValDW == ERROR_SUCCESS)
        {
            valueNameSize = sizeof(valueName)/sizeof(valueName[0]);
            valueDataSize = sizeof(valueData);
            while (RegEnumValue(keyHandle, valueNo++, valueName, &valueNameSize, 0, &valueType, (LPBYTE) &valueData, &valueDataSize) == ERROR_SUCCESS)
            {
                if (valueType == REG_DWORD)
                {
                    newModuleToLoad.first = valueData;
                    if (_tcsnicmp(valueName, "$HOME$", 6)==0)
                    {
                        sssnprintf(moduleLoadCommand, sizeof(moduleLoadCommand), "%s%s", HomeDir, valueName+6);
                        newModuleToLoad.second = moduleLoadCommand;
                    }
                    else
                    {
                        newModuleToLoad.second = valueName;
                    }
                    modulesToLoad.push_back(newModuleToLoad);
                }
                valueNameSize = sizeof(valueName)/sizeof(valueName[0]);
                valueDataSize = sizeof(valueData);
            }
            RegCloseKey(keyHandle);
            keyHandle = NULL;

            // Sort list based on priority field
            sort(modulesToLoad.begin(), modulesToLoad.end(), PrioritySortPredicate);
            
            // Load all of the NLMs determined above
            if (modulesToLoad.size() != 0)
            {
                returnValHR = S_OK;
                for (currModule = modulesToLoad.begin(); currModule != modulesToLoad.end(); currModule++, currModuleNo++)
                {
                    // Ensure module name ends in ".nlm"
                    currModuleName = currModule->second;
                    appendNLM = false;
                    nlmPos = _tcsstr(currModuleName.c_str(), ".nlm");
                    if (nlmPos == NULL)
                        appendNLM = true;
                    else if (*(nlmPos + 4) != NULL)
                        appendNLM = true;
                    if (appendNLM)
                        currModuleName = currModuleName + tcstring(".nlm");
                        
                    actionPercentage = ((float) currModuleNo/modulesToLoad.size())*100;
                    ThreadSwitchWithDelay();
                    if (_tcsicmp(currModule->second.c_str(), "") != 0)
                    {
                        LogProgressMessage_Action(actionPercentage, currModuleName.c_str());
                        if (LoadLibrary(currModule->second.c_str()) == NULL)
                        {
                            returnValHR = E_FAIL;
                            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_LOADINGMODULE), currModuleName.c_str());
                        }
                    }
                }
                LogProgressMessage_Action(100, currModuleName.c_str());
            }
            else
            {
                LogMessage(StatusCallbackFunctionEventType_Info, LS(IDS_NWINSTALL_ERROR_NOMODULESTOLOAD));
            }
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_OPENINGKEY), HRESULT_FROM_WIN32(returnValDW), REGHEADER"\\NLMsToLoad");
        }
    }
    else
    {
        returnValHR = E_FAIL;
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_REGISTRY_NOTLOADED));
    }
    return returnValHR;
}

HRESULT NetwareInstaller::DoActionDeleteFiles( tcstring actionArgument )
{
    tcstring::size_type     separatorPos                = tcstring::npos;
    tcstring                deleteIgnoreFailuresString;
    tcstring                targetDirectoryName;
    bool                    deleteIgnoreFailures       = false;
    HRESULT                 returnValHR                 = E_INVALIDARG;
    
    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_DELETINGFILES));
    separatorPos = actionArgument.find_first_of(',');
    if (separatorPos != tcstring::npos)
    {
        deleteIgnoreFailuresString = actionArgument.substr(0, separatorPos);
        targetDirectoryName = actionArgument.substr(separatorPos+1);
        
        if ((deleteIgnoreFailuresString != "") && (targetDirectoryName != ""))
        {
            deleteIgnoreFailures = (_ttoi(deleteIgnoreFailuresString.c_str()) != 0);
            returnValHR = DeleteFiles(currScriptFilename.c_str(), targetDirectoryName.c_str(), variables, deleteIgnoreFailures);
            if (deleteIgnoreFailures)
            {
                if (FAILED(returnValHR))
                    LogMessage(StatusCallbackFunctionEventType_Warning, LS(IDS_NWINSTALL_ERROR_DELETINGFILEIGNORED), returnValHR, targetDirectoryName.c_str());
                returnValHR = S_OK;
            }
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_NOARGUMENTS), "DeleteFiles", actionArgument.c_str());
        }
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_SEPARATORNOTFOUND), "DeleteFiles", actionArgument.c_str());
    }
    
    return returnValHR;
}

HRESULT NetwareInstaller::DoActionDeleteSelf( tcstring actionArgument )
{
    LogProgressMessage_Overall(LS(IDS_NWINSTALL_ERROR_DELETINGSELF));
    // NOTE:  this does not work on Windows but does on NetWare
    return DeleteFileDo(currModuleFilename.c_str(), true);
}

HRESULT NetwareInstaller::DoActionChain( tcstring actionArgument )
{
    StringArray             words;
    tcstring                conditionString;
    tcstring                targetSequence;
    bool                    doAction                = false;
    HRESULT                 returnValHR             = E_FAIL;
    
    returnValHR = GetWords(actionArgument, &words);
    if (SUCCEEDED(returnValHR))
    {
        if ((words.size() == 2) || (words.size() == 3))
        {
            returnValHR = GetVariable(variables, words[0].c_str(), &conditionString);
            if (returnValHR != S_OK)
                LogMessage(StatusCallbackFunctionEventType_Info, "Chain:  Note variable %s not found.\n", words[0].c_str());
            doAction = (_ttoi(conditionString.c_str()) != 0);
            if (doAction)
                targetSequence = words[1];
            else if (words.size() == 3)
                targetSequence = words[2];

            if (targetSequence != "")
            {
                LogMessage(StatusCallbackFunctionEventType_Info, "Chain:  taking branch to %s (condition string %s).", targetSequence.c_str(), conditionString.c_str());
                returnValHR = ProcessActionSection(targetSequence.c_str());
            }
            else
            {
                LogMessage(StatusCallbackFunctionEventType_Info, "Chain:  not taking branch to %s (condition string %s).", targetSequence.c_str(), conditionString.c_str());
            }
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_BADNUMBEROFARGUMENTS), "Chain", actionArgument.c_str());
        }
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_PARSINGARGUMENTS), actionArgument.c_str(), "Chain");
    }

    return returnValHR;
}

HRESULT NetwareInstaller::DoActionCopyFileSpec( tcstring actionArgument )
{
    FileEntry                   copyInfo;
    StringList                  files;
    StringList::iterator        currFile;
    TCHAR                       sourceFilename[1024]    = {""};
    TCHAR                       targetFilename[1024]    = {""};
    DWORD                       currFileNo              = 0;
    DWORD                       actionPercentage        = 0;
    bool                        firstErrorCodeSaved     = false;
    HRESULT                     returnValHRb            = E_FAIL;
    HRESULT                     returnValHR             = E_FAIL;

    returnValHR = ParseFileEntry(actionArgument, &copyInfo);
    if (SUCCEEDED(returnValHR))
    {
        sssnprintf(sourceFilename, sizeof(sourceFilename), "%s\\%s", sourceDirectory.c_str(), copyInfo.sourceFilename.c_str());
        LogMessage(StatusCallbackFunctionEventType_Info, "CopyFileSpec:  Searching for files spec %s", sourceFilename);
        returnValHR = GetFiles(sourceFilename, &files);
        if (returnValHR == S_OK)
        {
            for (currFile = files.begin(); currFile != files.end(); currFile++, currFileNo++)
            {
                actionPercentage = (DWORD) (((float) currFileNo/files.size()) * 100);
                sssnprintf(sourceFilename, sizeof(sourceFilename), "%s\\%s", sourceDirectory.c_str(), currFile->c_str());
                sssnprintf(targetFilename, sizeof(targetFilename), "%s\\%s", copyInfo.targetFilename.c_str(), currFile->c_str());
                LogProgressMessage_Action(actionPercentage, GetNodeName(sourceFilename).c_str());
                LogMessage(StatusCallbackFunctionEventType_Info, LS(IDS_NWINSTALL_INFO_COPYINGFILE), sourceFilename, targetFilename);

                returnValHRb = RecursiveCreateDirectory(GetParentPath(targetFilename));
                if (SUCCEEDED(returnValHRb))
                {
                    // Notify callback if we created the target directory for this file
                    if (returnValHRb == S_OK)
                        LogMessage(StatusCallbackFunctionEventType_Info, LS(IDS_NWINSTALL_INFO_CREATEDDIRECTORY), GetParentPath(targetFilename).c_str());
                    returnValHRb = CopyFileDo(sourceFilename, targetFilename, false);
                    if (FAILED(returnValHRb))
                        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_COPYINGFILE), returnValHRb, sourceFilename, targetFilename);
                }
                // Handle errors
                if (FAILED(returnValHRb))
                {
                    if (!firstErrorCodeSaved)
                    {
                        returnValHR = returnValHRb;
                        firstErrorCodeSaved = true;
                    }
                    if (!ignoreFailures)
                        break;
                }
            }
            LogProgressMessage_Action(100, GetNodeName(sourceFilename).c_str());
        }
        else if (returnValHR == S_FALSE)
        {
            LogMessage(StatusCallbackFunctionEventType_Info, "No files found to copy using source filespec %s.", copyInfo.sourceFilename.c_str());
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_GETTINGDIR), returnValHR, copyInfo.sourceFilename.c_str());
        }
    }
    return returnValHR;
}

HRESULT NetwareInstaller::DoActionStartReg( tcstring actionArgument )
{
    LPCTSTR     vpregPathname   = "SYS:SYSTEM\\vpreg.nlm";
    DWORD       returnValDW     = ERROR_OUT_OF_PAPER;
    HRESULT     returnValHR     = E_FAIL;
 
    if (!RegFunctionsLoaded)
    {
        if (access(vpregPathname, ACCESS_MODE_READ) == 0)
        {
            if (LoadLibrary(vpregPathname) != NULL)
            {
                returnValDW = LoadRegistryFunctions(FALSE);
                if (returnValDW == ERROR_SUCCESS)
                    returnValHR = S_OK;
                else
                    LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_REGISTRY_SYMBOLIMPORT));
            }
            else
            {
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_LOADINGMODULE), vpregPathname);
            }
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_REGISTRY_VPREGNOTFOUND));
        }
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Info, "Registry already started.\n");
    }

    return returnValHR;    
}

HRESULT NetwareInstaller::DoActionStopReg( tcstring actionArgument )
{
    NWCONN_HANDLE       connectionHandle;
    DWORD               returnValDW         = ERROR_OUT_OF_PAPER;
    HRESULT             returnValHR         = E_FAIL;

    if (RegFunctionsLoaded || (FindNLMHandle("vpreg.nlm") != NULL))
    {
        UnloadRegistryFunctions();

        if (FindNLMHandle("vpreg.nlm") != NULL)
        {
            LogMessage(StatusCallbackFunctionEventType_Info, "Unloading VPReg...");
            // NOTE:  Recode this using NWSMUnloadNLM
            returnValDW = system("unload vpreg.nlm");
            if (returnValDW == 0)
            {
                // A delay IS necessary here for the module to successfully unload.
                NTxSleep(250);
                if (FindNLMHandle("vpreg.nlm") == NULL)
                {
                    returnValHR = S_OK;
                    LogMessage(StatusCallbackFunctionEventType_Info, "VPReg unloaded.");
                }
                else
                {
                    LogMessage(StatusCallbackFunctionEventType_Info, "Unload command sent and returned success, but VPREG remains loaded.\n");
                }
            }
            else if (returnValDW == -1)
            {
                LogMessage(StatusCallbackFunctionEventType_Info, "Error %d returned by OS unloading VPReg.", errno);
            }
        }
        else
        {
            returnValHR = S_FALSE;
            LogMessage(StatusCallbackFunctionEventType_Info, "VPReg not loaded.");
        }
    }
    else
    {
        returnValHR = S_FALSE;
        LogMessage(StatusCallbackFunctionEventType_Info, LS(IDS_NWINSTALL_ERROR_REGISTRY_NOTLOADED));
    }

    return returnValHR;
}

HRESULT NetwareInstaller::DoActionStartNTS( tcstring actionArgument )
{
    DWORD       returnValDW     = ERROR_OUT_OF_PAPER;
    HRESULT     returnValHR     = E_FAIL;
    HRESULT     returnValHRb    = E_FAIL;
    
    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_NTS_START_OVERALL));

    // Validate state
    if (!RegFunctionsLoaded)
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_REGISTRY_NOTLOADED));
        return E_ABORT;
    }

    // Open keys required for PScan (among other things, AddressCache updating).
    // Note:  Keys taken from Pscan.cpp::InitCrit()
    returnValHR = S_OK;
    LogProgressMessage_Action(0, LS(IDS_NWINSTALL_PROGRESS_NTS_START_OPENKEYS));
    returnValHRb = OpenKeyLogMessage(HKEY_LOCAL_MACHINE, REGHEADER, &hMainKey);
    returnValHR = SUCCEEDED(returnValHR) ? returnValHRb : returnValHR;
    returnValHRb = OpenKeyLogMessage(hMainKey, szReg_Key_Licensing, &hLicensingKey, false);
    returnValHR = SUCCEEDED(returnValHR) ? returnValHRb : returnValHR;
    returnValHRb = OpenKeyLogMessage(hMainKey, szReg_Key_PatternManager, &hPattManKey);
    returnValHR = SUCCEEDED(returnValHR) ? returnValHRb : returnValHR;
    returnValHRb = OpenKeyLogMessage(hMainKey, szReg_Key_ProductControl, &hProductControlKey);
    returnValHR = SUCCEEDED(returnValHR) ? returnValHRb : returnValHR;
    returnValHRb = OpenKeyLogMessage(hMainKey, szReg_Key_ClientScan, &hClientKey, false);
    returnValHR = SUCCEEDED(returnValHR) ? returnValHRb : returnValHR;

    LogProgressMessage_Action(15, LS(IDS_NWINSTALL_PROGRESS_NTS_START_OPENKEYS));
    returnValHRb = OpenKeyLogMessage(hMainKey, szReg_Key_ForwardScan, &hForwardKey);
    returnValHR = SUCCEEDED(returnValHR) ? returnValHRb : returnValHR;
    returnValHRb = OpenKeyLogMessage(hMainKey, szReg_Key_SystemScan, &hSystemKey);
    returnValHR = SUCCEEDED(returnValHR) ? returnValHRb : returnValHR;
    returnValHRb = OpenKeyLogMessage(hMainKey, szReg_Key_IntegrityShield, &hISKey);
    returnValHR = SUCCEEDED(returnValHR) ? returnValHRb : returnValHR;
    returnValHRb = OpenKeyLogMessage(hMainKey, szReg_Key_Clients, &hClientsKey, false);
    returnValHR = SUCCEEDED(returnValHR) ? returnValHRb : returnValHR;
    returnValHRb = OpenKeyLogMessage(hMainKey, szReg_Key_Accounts, &hAccountsKey, false);
    returnValHR = SUCCEEDED(returnValHR) ? returnValHRb : returnValHR;
    returnValHRb = OpenKeyLogMessage(hMainKey, szReg_Key_Common_Key, &hCommonKey);
    returnValHR = SUCCEEDED(returnValHR) ? returnValHRb : returnValHR;

    // Start communications
    LogProgressMessage_Action(30, LS(IDS_NWINSTALL_PROGRESS_NTS_START_STARTNTS));
    returnValDW = StartNTS(NULL);
    
    // Return result
    if (returnValDW == ERROR_SUCCESS)
    {
        returnValHR = S_OK;
        LogMessage(StatusCallbackFunctionEventType_Info, "StartNTS succeeded.");
    }
    else
    {
        returnValHR = E_FAIL;
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_STARTNTS), returnValDW);
    }
    return returnValHR;
}

HRESULT NetwareInstaller::DoActionStopNTS( tcstring actionArgument )
{
    bool                                failureIsError      = true;
    StatusCallbackFunctionEventType     messageType         = StatusCallbackFunctionEventType_Error;
    DWORD                               returnValDW         = ERROR_OUT_OF_PAPER;
    HRESULT                             returnValHR         = E_FAIL;

    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_NTS_STOP_OVERALL));

    // Configure error handling    
    if (_ttoi(actionArgument.c_str()) != 0)
    {
        failureIsError = false;
        messageType = StatusCallbackFunctionEventType_Info;
    }

    // Validate state
    if (!RegFunctionsLoaded)
    {
        
        LogMessage(messageType, LS(IDS_NWINSTALL_ERROR_REGISTRY_NOTLOADED));
        if (failureIsError)
            return E_ABORT;
        else
            return S_OK;
    }

    // Shutdown NTS
    returnValDW = StopNTS();
    LogProgressMessage_Action(70, LS(IDS_NWINSTALL_PROGRESS_NTS_STOP_CLOSEKEYS));

    // Close registry keys
    if (hMainKey != NULL)
        RegCloseKey(hMainKey);
    hMainKey = NULL;
    if (hLicensingKey != NULL)
        RegCloseKey(hLicensingKey);
    hLicensingKey = NULL;
    if (hPattManKey != NULL)
        RegCloseKey(hPattManKey);
    hPattManKey = NULL;
    if (hProductControlKey != NULL)
        RegCloseKey(hProductControlKey);
    hProductControlKey = NULL;
    if (hClientKey != NULL)
        RegCloseKey(hClientKey);
    hClientKey = NULL;

    LogProgressMessage_Action(85, LS(IDS_NWINSTALL_PROGRESS_NTS_STOP_CLOSEKEYS));
    if (hForwardKey != NULL)
        RegCloseKey(hForwardKey);
    hForwardKey = NULL;
    if (hSystemKey != NULL)
        RegCloseKey(hSystemKey);
    hSystemKey = NULL;
    if (hISKey != NULL)
        RegCloseKey(hISKey);
    hISKey = NULL;
    if (hClientsKey != NULL)
        RegCloseKey(hClientsKey);
    hClientsKey = NULL;
    if (hAccountsKey != NULL)
        RegCloseKey(hAccountsKey);
    hAccountsKey = NULL;
    if (hCommonKey != NULL)
        RegCloseKey(hCommonKey);
    hCommonKey = NULL;

    // Return result
    returnValHR = HRESULT_FROM_WIN32(returnValDW);
    if (FAILED(returnValHR))
    {
        LogMessage(messageType, LS(IDS_NWINSTALL_ERROR_STOPNTS), returnValDW);
        if (!failureIsError)
            returnValHR = S_FALSE;
    }
    return returnValHR;
}

HRESULT NetwareInstaller::DoActionCreatedirectory( tcstring actionArgument )
{
    HRESULT returnValHR = E_FAIL;
    
    if (actionArgument == "")
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_NOARGUMENTS), "CreateDirectory", actionArgument.c_str());
        return E_INVALIDARG;
    }
    
    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_CREATEDIRECTORY));
    LogProgressMessage_Action(0, "%s", actionArgument.c_str());
    returnValHR = RecursiveCreateDirectory(actionArgument);
    if (returnValHR == S_FALSE)
        LogMessage(StatusCallbackFunctionEventType_Info, "Directory %s already exists.", actionArgument.c_str());
    else if (FAILED(returnValHR))
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_CREATINGDIRECTORY), returnValHR, actionArgument.c_str());
    LogProgressMessage_Action(100, "%s", actionArgument.c_str());
    
    return returnValHR;
}

HRESULT NetwareInstaller::DoActionWaitForSAVstop( tcstring actionArguments )
{
    DWORD       startTime       = 0;
    DWORD       rtvscanHandle   = NULL;
    DWORD       currTime        = 0;
    DWORD       elapsedTime     = 0;
    DWORD       maxWait         = 0;
    DWORD       pollDelay       = 200;
    HRESULT     returnValHR     = E_FAIL;
    
    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_SAVSHUTDOWNWAIT));
    
    maxWait = _ttoi(actionArguments.c_str()) * 18;
    startTime = GetCurrentTicks();    
    do 
    {
        rtvscanHandle = FindNLMHandle("rtvscan.nlm");
        
        // Calculate elapsed time and sleep
        currTime = GetCurrentTicks();
        if (currTime >= startTime)
            elapsedTime = currTime - startTime;
        else
            elapsedTime = (0xffffffff - startTime) + currTime;
        NTxSleep(pollDelay);
    }
    while ((rtvscanHandle != NULL) && (elapsedTime <= maxWait));
    
    if (rtvscanHandle == NULL)
    {
        LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_SAVSHUTDOWNDETECTED));
        return S_OK;
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_SAVSHUTDOWNWAITTIMEOUT), (maxWait/18));
        return E_FAIL;
    }
}


HRESULT NetwareInstaller::DoActionWait( tcstring actionArguments )
{
    DWORD       startTime       = 0;
    DWORD       currTime        = 0;
    DWORD       elapsedTime     = 0;
    DWORD       maxWait         = 0;
    DWORD       secondsToWait   = 0;
    DWORD       pollDelay       = 1000;
    
    maxWait = _ttoi(actionArguments.c_str()) * 18;
    startTime = GetCurrentTicks();    
    do 
    {
        // Report current status
        secondsToWait = (maxWait - elapsedTime)/18;
        LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_WAIT), secondsToWait);

        // Calculate elapsed time and sleep
        currTime = GetCurrentTicks();
        if (currTime >= startTime)
            elapsedTime = currTime - startTime;
        else
            elapsedTime = (0xffffffff - startTime) + currTime;
        NTxSleep(pollDelay);
    }
    while (elapsedTime <= maxWait);

    return S_OK;    
}

HRESULT NetwareInstaller::DoActionAutoRegistry( tcstring actionArgument )
{
    if (alreadyInstalled)
        return DoActionUpdateRegistry(actionArgument);
    else
        return DoActionCopyRegistry(actionArgument);
}

HRESULT NetwareInstaller::DoActionNukeDir( tcstring actionArgument )
{
    StringArray             arguments;
    tcstring                targetDirectory;
    tcstring                exceptionFilename;
    HRESULT                 returnValHR = E_FAIL;
    
    // Validate parameters
    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_DELETINGDIRECTORY), actionArgument.c_str());
    returnValHR = GetWords(actionArgument, &arguments);
    if (SUCCEEDED(returnValHR))
    {
        if (arguments.size() == 0)
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_NOARGUMENTS), "NukeDir", actionArgument.c_str());
            returnValHR = E_INVALIDARG;
        }
        else if (arguments.size() == 1)
        {
            returnValHR = NukeDir(actionArgument);
            if (FAILED(returnValHR))
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_DELETINGDIR), returnValHR, actionArgument.c_str());
        }
        else if (arguments.size() == 2)
        {
            targetDirectory = arguments[0];
            exceptionFilename = arguments[1];

            returnValHR = NukeDir(targetDirectory, exceptionFilename);
            if (FAILED(returnValHR))
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_DELETINGDIR), returnValHR, actionArgument.c_str());
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_BADNUMBEROFARGUMENTS), "NukeDir", actionArgument.c_str());
            returnValHR = E_INVALIDARG;
        }
    }
    
    return returnValHR;
}

HRESULT NetwareInstaller::DoActionRemoveDuplicateRTVSCAN( tcstring actionArgument )
// Removes a duplicate SAV from the NLMStoLoad list.  The duplicate uses the full pathname to SAV instead
// of a $HOME$ relative path.
{
    HKEY                    keyHandle               = NULL;
    DWORD                   valueNo                 = 0;
    TCHAR                   valueName[MAX_PATH]     = {""};
    DWORD                   valueNameSize           = 0;
    TCHAR                   valueNameLower[MAX_PATH]= {""};
    DWORD                   valueType               = REG_DWORD;
    DWORD                   valueData               = 0;
    DWORD                   valueDataSize           = 0;
    StringList              valuesToDelete;
    StringList::iterator    currValueToDelete;
    DWORD                   returnValDW             = ERROR_OUT_OF_PAPER;
    HRESULT                 returnValHR             = E_FAIL;

    // Ensure registry functions are usable    
    if (!RegFunctionsLoaded)
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_REGISTRY_NOTLOADED));
        return E_FAIL;
    }

    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_DUPENLMTOLOAD_SCAN));
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

        // Delete the duplicate value(s)
        if (valuesToDelete.size() != 0)
        {
            LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_DUPENLMTOLOAD_DELETE));
            returnValHR = S_OK;
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Info, "No duplicate startup commands.");
            returnValHR = S_FALSE;
        }
        for (currValueToDelete = valuesToDelete.begin(); currValueToDelete != valuesToDelete.end(); currValueToDelete++)
        {
            returnValDW = RegDeleteValue(keyHandle, currValueToDelete->c_str());
            if (returnValDW == ERROR_SUCCESS)
            {
                LogMessage(StatusCallbackFunctionEventType_Info, "Deleted value %s.", currValueToDelete->c_str());
            }
            else
            {
                LogMessage(StatusCallbackFunctionEventType_Info, "Error 0x%08x deleting value %s.", HRESULT_FROM_WIN32(returnValDW), currValueToDelete->c_str());
                if (returnValHR == S_OK)
                    returnValHR = HRESULT_FROM_WIN32(returnValDW);
            }
        }

        // Cleanup and return
        RegCloseKey(keyHandle);
        keyHandle = NULL;
    }
    else
    {
        returnValHR = HRESULT_FROM_WIN32(returnValDW);
    }
    
    return returnValHR;
}

HRESULT NetwareInstaller::DoActionAddAutoexecNCF( tcstring actionArguments )
{
    FILE*       fileHandle              = NULL;
    TCHAR       currLine[1000]          = {""};
    bool        autoexecLoadsSAV        = false;
    HRESULT     returnValHR             = E_FAIL;
    
    // Scan Autoexec.ncf for our entry
    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_NCFSCAN_SCAN));
    returnValHR = fsopen(Autoexec_Fullpathname, "rt", &fileHandle);
    if (fileHandle != NULL)
    {
        while ((fgets(currLine, sizeof(currLine)/sizeof(currLine[0]), fileHandle) != NULL))
        {
            // Make sure currLine is NULL terminated
            currLine[(sizeof(currLine)/sizeof(currLine[0]))-1] = NULL;
            TerminateAtNewline(currLine);
            
            if (_tcsicmp(currLine, AutoexecNCF_LoadLine) == 0)
            {
                autoexecLoadsSAV = true;
                break;
            }
        }
        
        if (ferror(fileHandle) == 0)
            returnValHR = S_OK;
        fclose(fileHandle);
        fileHandle = NULL;
    }
    
    // Add our entry to autoexec.ncf
    if (!autoexecLoadsSAV)
    {
        LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_NCFSCAN_ADD));
        DeleteFileDo(Autoexec_BackupPathname);
        returnValHR = CopyFileDo(Autoexec_Fullpathname, Autoexec_BackupPathname, false);
        if (FAILED(returnValHR))
            LogMessage(StatusCallbackFunctionEventType_Warning, LS(IDS_NWINSTALL_WARNING_NCFSCAN_BACKUP), returnValHR);
        returnValHR = fsopen(Autoexec_Fullpathname, "at", &fileHandle);
        if (fileHandle != NULL)
        {
            returnValHR = E_FAIL;
            fprintf(fileHandle, "\n");
            fprintf(fileHandle, "%s\n", AutoexecNCF_StartLine);
            fprintf(fileHandle, "%s\n", AutoexecNCF_CommentLine);
            fprintf(fileHandle, "%s\n", AutoexecNCF_LoadLine);
            fprintf(fileHandle, "%s\n", AutoexecNCF_EndLine);

            if (ferror(fileHandle) == 0)
                returnValHR = S_OK;
            fclose(fileHandle);
            fileHandle = NULL;
        }
    }

    if (returnValHR == E_FAIL)
        returnValHR = FileErrnoToHRESULT(errno);
    return returnValHR;
}

HRESULT NetwareInstaller::DoActionRemoveAutoexecNCF( tcstring actionArguments )
{
    FILE*                   sourceFileHandle        = NULL;
    FILE*                   targetFileHandle        = NULL;
    StringList              fileLines;
    TCHAR                   currLine[1000]          = {""};
    bool                    copyLine                = true;
    bool                    autoexecLoadsSAV        = false;
    TCHAR                   tempFilename[2*MAX_PATH]= {""};
    StringList::iterator    currFileLine;
    HRESULT                 returnValHR             = E_FAIL;
    HRESULT                 returnValHRb            = E_FAIL;

    // Scan Autoexec.ncf for our entry
    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_NCFSCAN_SCAN));
    returnValHR = fsopen(Autoexec_Fullpathname, "rt", &sourceFileHandle);
    if (sourceFileHandle != NULL)
    {
        returnValHR = E_FAIL;
        while ((fgets(currLine, sizeof(currLine)/sizeof(currLine[0]), sourceFileHandle) != NULL))
        {
            // Make sure currLine is NULL terminated
            copyLine = true;
            currLine[(sizeof(currLine)/sizeof(currLine[0]))-1] = NULL;
            TerminateAtNewline(currLine);

            if (_tcsicmp(currLine, AutoexecNCF_StartLine) == 0)
                copyLine = false;
            if (_tcsicmp(currLine, AutoexecNCF_CommentLine) == 0)
                copyLine = false;
            if (_tcsicmp(currLine, AutoexecNCF_LoadLine) == 0)
                copyLine = false;
            if (_tcsicmp(currLine, AutoexecNCF_EndLine) == 0)
                copyLine = false;

            if (copyLine)
                fileLines.push_back(currLine);
            else
                autoexecLoadsSAV = true;
        }
        
        if (ferror(sourceFileHandle) == 0)
            returnValHR = S_OK;
        fclose(sourceFileHandle);
        sourceFileHandle = NULL;
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_OPENINGFILE), returnValHR, Autoexec_Fullpathname);
    }
    
    // Is SAV loaded?
    if (autoexecLoadsSAV)
    {
        LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_NCFREMOVE));
        // Yes, remove the lines that load SAV
        DeleteFileDo(Autoexec_BackupPathname);
        returnValHR = CopyFileDo(Autoexec_Fullpathname, Autoexec_BackupPathname, false);
        if (FAILED(returnValHR))
            LogMessage(StatusCallbackFunctionEventType_Warning, LS(IDS_NWINSTALL_WARNING_NCFSCAN_BACKUP), returnValHR);
        tmpnam(tempFilename);
        returnValHR = E_FAIL;
        returnValHR = fsopen(tempFilename, "at", &targetFileHandle);
        if (targetFileHandle != NULL)
        {
            returnValHR = E_FAIL;
            for (currFileLine = fileLines.begin(); currFileLine != fileLines.end(); currFileLine++)
                fprintf(targetFileHandle, "%s\r\n", currFileLine->c_str());

            if (ferror(targetFileHandle) == 0)
            {
                fclose(targetFileHandle);
                targetFileHandle = NULL;
                returnValHR = CopyFileDo(tempFilename, Autoexec_Fullpathname, false);
                if (SUCCEEDED(returnValHR))
                    returnValHR = S_OK;
                else
                    LogMessage(StatusCallbackFunctionEventType_Warning, LS(IDS_NWINSTALL_ERROR_COPYINGFILE), returnValHR, tempFilename, Autoexec_Fullpathname);
            }
            else
            {
                fclose(targetFileHandle);
                targetFileHandle = NULL;
            }
            returnValHRb = DeleteFileDo(tempFilename);
            if (FAILED(returnValHRb))
                LogMessage(StatusCallbackFunctionEventType_Warning, LS(IDS_NWINSTALL_ERROR_DELETINGFILE), returnValHRb, tempFilename);
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_OPENINGFILE), returnValHR, tempFilename);
        }
    }

    if (returnValHR == E_FAIL)
        returnValHR = FileErrnoToHRESULT(errno);
    return returnValHR;
}

HRESULT NetwareInstaller::DoActionOR( tcstring actionArguments )
// Set a variable based on whether other variables are set to non-zero
// actionArguments format:  <variableToSet>(,<testVar>)*
{
    StringArray                 words;
    StringArray::iterator       currWord;
    tcstring                    variableValue;
    tcstring                    evaluationString;
    bool                        evaluationValue     = false;
    HRESULT                     returnValHR         = E_FAIL;
    
    // Validate parameters
    if (actionArguments == "")
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_NOARGUMENTS), "OR", actionArguments.c_str());
        return E_ABORT;
    }
    
    returnValHR = GetWords(actionArguments, &words);
    if (SUCCEEDED(returnValHR))
    {
        if (words.size() > 1)
        {
            currWord = words.begin();
            evaluationValue = false;
            for (currWord++; currWord != words.end(); currWord++)
            {
                variableValue = "";
                if (SUCCEEDED(GetVariable(variables, currWord->c_str(), &variableValue)))
                {
                    if (_ttoi(variableValue.c_str()) != 0)
                    {
                        evaluationString = evaluationString + currWord->c_str() + "(TRUE) ";
                        evaluationValue = true;
                    }
                    else
                    {
                        evaluationString = evaluationString + currWord->c_str() + "(FALSE) ";
                    }
                }
                else
                {
                    evaluationString = evaluationString + currWord->c_str() + "(FALSE - not found) ";
                }
            }

            // Log out the value and set it
            LogMessage(StatusCallbackFunctionEventType_Info, "Term %s evaluates to %d:  %s", words[0].c_str(), (DWORD) evaluationValue, evaluationString.c_str());
            if (evaluationValue)
                SetVariable(&variables, words[0].c_str(), "1");
            else
                SetVariable(&variables, words[0].c_str(), "0");
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_BADNUMBEROFARGUMENTS), "OR", actionArguments.c_str());
            returnValHR = E_INVALIDARG;
        }
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_PARSINGARGUMENTS), returnValHR, actionArguments.c_str(), "OR");
    }
    
    return returnValHR;
}

HRESULT NetwareInstaller::DoActionSetInheritGroupSettings( tcstring actionArguments )
{
    HKEY            keyHandle               = NULL;
    DWORD           valueDataSize           = 0;
    TCHAR           parentName[MAX_PATH]    = {""};
    const DWORD     valueOne                = 1;
    DWORD           returnValDW             = ERROR_OUT_OF_PAPER;
    HRESULT         returnValHR             = E_FAIL;

    valueDataSize = sizeof(parentName);
    returnValDW = RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER, &keyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        returnValHR = S_FALSE;
        returnValDW = SymSaferRegQueryValueEx(keyHandle, szReg_Val_Parent, 0, NULL, reinterpret_cast<unsigned char*>(parentName), &valueDataSize);
        if (returnValDW == ERROR_SUCCESS)
        {
            if (_tcslen(parentName) > 0)
            {
                returnValDW = RegSetValueEx(keyHandle, szReg_Val_InheritGroupSettings, 0, REG_DWORD, (BYTE*) &valueOne, sizeof(valueOne));
                returnValHR = HRESULT_FROM_WIN32(returnValDW);
            }
        }
        RegCloseKey (keyHandle);
    }
    
    return returnValHR;
}

HRESULT NetwareInstaller::DoActionNDSlogin( tcstring actionArguments )
{
    tcstring        ndsContainer;
    tcstring        ndsUsername;
    tcstring        ndsPassword;
    HKEY            keyHandle       = NULL;
    DWORD           returnValDW     = ERROR_OUT_OF_PAPER;
    HRESULT         returnValHR     = E_FAIL;

    // Validate state
    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_LOGGINGIN));
    if (!RegFunctionsLoaded)
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_REGISTRY_NOTLOADED));
        return E_ABORT;
    }

    // Set globals for DSPrelims
    GetVariable(variables, "OU", &ndsContainer);
    GetVariable(variables, "USERNAME", &ndsUsername);
    GetVariable(variables, "PASSWORD", &ndsPassword);
    if ((ndsUsername != "") && (ndsPassword != ""))
    {
        LogMessage(StatusCallbackFunctionEventType_Info, "Using cached login info...");
        // LoginToDirectory automatically decryptes the password if it is prefixed with !CRYPT!, assuming encryption is MakeEP(username, PROFILE_PASS_KEY)
        // NOTE:  Cannot use sizeof because these symbols are extern'd...grr...
        _tcsncpy(Container, ndsContainer.c_str(), 128);
        _tcsncpy(AuthUser, ndsUsername.c_str(), 256);
        _tcsncpy(AuthPass, ndsPassword.c_str(), 256);
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Info, "NDS login information not set, retrieving from registry...");
        
        returnValDW = RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER, &keyHandle);
        if (returnValDW == ERROR_SUCCESS)
        {
            LogMessage(StatusCallbackFunctionEventType_Info, "Reading in login info...");
            // NOTE:  sizeof will not work with these because they are extern'd
            GetStr(keyHandle, "Container", Container, 128, "");
            GetStr(keyHandle, "ProfileUsername", AuthUser, 256, "");
            GetStr(keyHandle, "ProfilePassword", AuthPass, 256, "");
            RegCloseKey(keyHandle);
            keyHandle = NULL;
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_OPENINGKEY), HRESULT_FROM_WIN32(returnValDW), REGHEADER);
        }
        
    }

    UsingDS = TRUE;
    if (DSNew)
    {
        HKEY mainKeyHandle = NULL;
        DSNew = 0;

        returnValDW = RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER, &mainKeyHandle);
        if (returnValDW == ERROR_SUCCESS)
        {
            if (NumChars(Container)<1)
            {
                BadDefAuth = FALSE;
                UseDefaultContext();
                if (!GetContextUI(Container))
                {
                    LogMessage(StatusCallbackFunctionEventType_Info, "Error getting context before initDS in DS preliminaries");
                    RegCloseKey(mainKeyHandle);
                    mainKeyHandle = NULL;
                    return E_FAIL;
                }

                PutStr(mainKeyHandle,"Container",Container);
            }
            returnValDW = InitDS();
            if (returnValDW != ERROR_SUCCESS)
            {
                LogMessage(StatusCallbackFunctionEventType_Info, "Error (0x%x)(%d) initializing DS in DS Preliminaries", returnValDW, returnValDW);
                RegCloseKey(mainKeyHandle);
                mainKeyHandle = NULL;
                return E_FAIL;
            }
            returnValDW = AuthenticateUser();
            if (returnValDW != ERROR_SUCCESS)
            {
                LogMessage(StatusCallbackFunctionEventType_Info, "Error (0x%x)(%d) authenticating user in DS preliminaries", returnValDW, returnValDW);
                RegCloseKey(mainKeyHandle);
                mainKeyHandle = NULL;
                return E_FAIL;
            }

            PutStr(mainKeyHandle, "ProfileUserName", AuthUser);
            PutStr(mainKeyHandle, "ProfilePassword", AuthPass);
            RegCloseKey(mainKeyHandle);
            mainKeyHandle = NULL;
        }
        else
        {
            returnValHR = HRESULT_FROM_WIN32(returnValDW);
        }

    }
    if (Authenticated)
        return S_OK;
    else
        return E_FAIL;
}


HRESULT NetwareInstaller::DoActionNWcreateGroups( tcstring actionArguments )
{
    StringArray     words;
    bool            isNDS           = true;
    HRESULT         returnValHR     = E_FAIL;
    
    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_NET_CREATEGROUPS));
    if (Authenticated)
    {
        returnValHR = GetWords(actionArguments, &words);
        if (SUCCEEDED(returnValHR))
        {
            if (words.size() == 2)
            {
                isNDS = _ttoi(words[0].c_str());
                return CreateGroups(words[1], true, isNDS);
            }
            else
            {
                returnValHR = E_INVALIDARG;
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_BADNUMBEROFARGUMENTS), "NWcreateGroups", actionArguments.c_str());
            }
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_PARSINGARGUMENTS), actionArguments.c_str(), "NWcreateGroups");
        }
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_NOTLOGGEDIN), "NWcreateGroups");
        returnValHR = E_ABORT;
    }            
    return returnValHR;
}

HRESULT NetwareInstaller::DoActionNWsetRights( tcstring actionArguments )
{
    StringArray     words;
    bool            isNDS           = true;
    HRESULT         returnValHR     = E_FAIL;
    
    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_NET_GRANTRIGHTS));
    if (Authenticated)
    {
        returnValHR = GetWords(actionArguments, &words);
        if (SUCCEEDED(returnValHR))
        {
            if (words.size() == 2)
            {
                isNDS = _ttoi(words[0].c_str());
                return SetRights(words[1], true, isNDS);
            }
            else
            {
                returnValHR = E_INVALIDARG;
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_BADNUMBEROFARGUMENTS), "NWsetRights", actionArguments.c_str());
            }
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_PARSINGARGUMENTS), actionArguments.c_str(), "NWsetRights");
        }
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_NOTLOGGEDIN), "NWsetRights");
        returnValHR = E_ABORT;
    }            
    return returnValHR;
}

HRESULT NetwareInstaller::DoActionNWupdateLoginScript( tcstring actionArguments )
{
    StringArray     words;
    bool            isNDS           = true;
    HRESULT         returnValHR     = E_FAIL;
    
    LogProgressMessage_Overall(LS(IDS_UPDATE_LOGIN));
    if (Authenticated)
    {
        returnValHR = GetWords(actionArguments, &words);
        if (SUCCEEDED(returnValHR))
        {
            if (words.size() == 2)
            {
                isNDS = _ttoi(words[0].c_str());
                return UpdateLoginScript(words[1], true, isNDS);
            }
            else
            {
                returnValHR = E_INVALIDARG;
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_BADNUMBEROFARGUMENTS), "NWupdateLoginScript", actionArguments.c_str());
            }
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_PARSINGARGUMENTS), actionArguments.c_str(), "NWupdateLoginScript");
        }
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_NOTLOGGEDIN), "NWupdateLoginScript");
        returnValHR = E_ABORT;
    }            
    return returnValHR;
}

HRESULT NetwareInstaller::DoActionNWremoveGroups( tcstring actionArguments )
{
    StringArray     words;
    bool            isNDS           = true;
    HRESULT         returnValHR     = E_FAIL;
    
    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_NET_REMOVEGROUPS));
    if (Authenticated)
    {
        returnValHR = GetWords(actionArguments, &words);
        if (SUCCEEDED(returnValHR))
        {
            if (words.size() == 2)
            {
                isNDS = _ttoi(words[0].c_str());
                return CreateGroups(words[1], false, isNDS);
            }
            else
            {
                returnValHR = E_INVALIDARG;
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_BADNUMBEROFARGUMENTS), "NWremoveGroups", actionArguments.c_str());
            }
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_PARSINGARGUMENTS), actionArguments.c_str(), "NWremoveGroups");
        }
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_NOTLOGGEDIN), "NWremoveGroups");
        returnValHR = E_ABORT;
    }            
    return returnValHR;
}

HRESULT NetwareInstaller::DoActionNWremoveRights( tcstring actionArguments )
{
    StringArray     words;
    bool            isNDS           = true;
    HRESULT         returnValHR     = E_FAIL;
    
    LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_NET_REVOKERIGHTS));
    if (Authenticated)
    {
        returnValHR = GetWords(actionArguments, &words);
        if (SUCCEEDED(returnValHR))
        {
            if (words.size() == 2)
            {
                isNDS = _ttoi(words[0].c_str());
                return SetRights(words[1], false, isNDS);
            }
            else
            {
                returnValHR = E_INVALIDARG;
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_BADNUMBEROFARGUMENTS), "NWremoveRights", actionArguments.c_str());
            }
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_PARSINGARGUMENTS), actionArguments.c_str(), "NWremoveRights");
        }
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_NOTLOGGEDIN), "NWremoveRights");
        returnValHR = E_ABORT;
    }            
    return returnValHR;
}

HRESULT NetwareInstaller::DoActionNWupdateRemoveLoginScript( tcstring actionArguments )
{
    StringArray     words;
    bool            isNDS           = true;
    HRESULT         returnValHR     = E_FAIL;

    LogProgressMessage_Overall(LS(IDS_CLEANUP_LOGIN));
    if (Authenticated)
    {
        returnValHR = GetWords(actionArguments, &words);
        if (SUCCEEDED(returnValHR))
        {
            if (words.size() == 2)
            {
                isNDS = _ttoi(words[0].c_str());
                return UpdateLoginScript(words[1], false, isNDS);
            }
            else
            {
                returnValHR = E_INVALIDARG;
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_BADNUMBEROFARGUMENTS), "NWremoveLoginScript", actionArguments.c_str());
            }
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_PARSINGARGUMENTS), actionArguments.c_str(), "NWremoveLoginScript");
        }
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_NOTLOGGEDIN), "NWremoveLoginScript");
        returnValHR = E_ABORT;
    }            
    return returnValHR;
}

HRESULT NetwareInstaller::DoActionNWsetAuthenticated( tcstring actionArguments )
{
    // Validate parameters
    if (actionArguments == "")
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_NOARGUMENTS), "SetNWauthenticated", actionArguments.c_str());
        return E_INVALIDARG;
    }

    // Set Authenticated so legacy initnlm code will not prompt for login in bindery mode
    if (_ttoi(actionArguments.c_str()) != 0)
    {
        Authenticated = 1;
        LogMessage(StatusCallbackFunctionEventType_Info, "Authenticated = 1");
    }
    else
    {
        Authenticated = 0;
        LogMessage(StatusCallbackFunctionEventType_Info, "Authenticated = 0");
    }
    return S_OK;    
}

HRESULT NetwareInstaller::DoActionSetIgnoreFailures( tcstring actionArguments )
{
    // Validate parameters
    if (actionArguments == "")
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_NOARGUMENTS), "SetIgnoreFailures", actionArguments.c_str());
        return E_INVALIDARG;
    }

    // Set ignore failures as directed    
    if (_ttoi(actionArguments.c_str()) != 0)
    {
        ignoreFailures = true;
        LogMessage(StatusCallbackFunctionEventType_Info, "IgnoreFailures = 1");
    }
    else
    {
        ignoreFailures = false;
        LogMessage(StatusCallbackFunctionEventType_Info, "IgnoreFailures = 0");
    }
    return S_OK;
}

HRESULT NetwareInstaller::DoActionChainSet( tcstring actionArgument )
{
    StringArray             words;
    StringArray::iterator   currWord;
    tcstring                instructionSection;
    tcstring                variableValue;
    bool                    doAction                = false;
    HRESULT                 returnValHR             = E_FAIL;
    
    returnValHR = GetWords(actionArgument, &words);
    if (SUCCEEDED(returnValHR))
    {
        if (words.size() >= 2)
        {
            instructionSection = words[0];
            currWord = words.begin();
            currWord++;
            for (;currWord != words.end(); currWord++)
            {
                variableValue = "";
                GetVariable(variables, currWord->c_str(), &variableValue);
                if (variableValue != "")
                {
                    LogMessage(StatusCallbackFunctionEventType_Info, "ChainSet:  arg %s set (%s).", currWord->c_str(), variableValue.c_str());
                    doAction = true;
                    break;
                }
            }
            if (doAction)
            {
                LogMessage(StatusCallbackFunctionEventType_Info, "ChainSet:  taking branch to %s (args string %s).", instructionSection.c_str(), actionArgument.c_str());
                returnValHR = ProcessActionSection(instructionSection.c_str());
            }
            else
            {
                LogMessage(StatusCallbackFunctionEventType_Info, "ChainSet:  not taking branch to %s (args %s).", instructionSection.c_str(), actionArgument.c_str());
            }
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_BADNUMBEROFARGUMENTS), "ChainSet", actionArgument.c_str());
        }
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_PARSINGARGUMENTS), actionArgument.c_str(), "ChainSet");
    }

    return returnValHR;
}

// Action handler shared implementations
HRESULT NetwareInstaller::CopyRegistryEntries( LPCTSTR filename, LPCTSTR sectionName, StringPairList variables, bool overwriteExistingValues, bool ignoreFailures )
// Processes the specified registry section.  If overwriteExistingValues is TRUE, then any existing values are overwritten
// with the values specified in this section.  Errors are reported to errorLogCallback, if specified
// Variables is a list of text replacements to make where variables are specified by $<varname>$ in the specified file
// If ignoreFailures is TRUE, does not stop at the 1st failure and instead tries to process all lines in the section
// Returns S_OK on success, else the error code of the failure.
{
    StringPairList              sectionKeys;
    StringPairList::iterator    currSectionKey;
    HKEY                        currKeyHandle           = NULL;
    tcstring                    currKeyPath;
    tcstring                    newKeyPath;
    DWORD                       currSectionKeyNo        = 0;
    DWORD                       actionProgressPercent   = 0;
    bool                        firstErrorCodeSaved     = false;
    HRESULT                     returnValHR             = E_FAIL;
    HRESULT                     returnValHRb            = E_FAIL;
    DWORD                       returnValDW             = ERROR_OUT_OF_PAPER;

    // Validate parameters
    if ((filename == NULL) || (sectionName == NULL))
        return E_POINTER;
    // Validate state
    if (!RegFunctionsLoaded)
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_REGISTRY_NOTLOADED));
        return E_ABORT;
    }
    
    // Initialize
    returnValHR = GetIniSection(filename, sectionName, &sectionKeys);
    if (returnValHR == S_OK)
    {
        returnValHR = ReplaceVariables(&sectionKeys, variables);
        if (FAILED(returnValHR))
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_REPLACINGVARIABLES), returnValHR, sectionName, filename);
    }
    else if (returnValHR == S_FALSE)
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_SECTIONNOTFOUND), sectionName, filename);
        returnValHR = E_FAIL;
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_READINGSECTION), returnValHR, sectionName, filename);
    }


    if (SUCCEEDED(returnValHR))
    {
        // Base case - quit if nothing to do
        if (sectionKeys.size() == 0)
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_WARNING_EMPTYSECTION), sectionName, currScriptFilename.c_str());
            return S_FALSE;
        }
        returnValHR = S_OK;
        LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_MERGINGREG));
        for (currSectionKey = sectionKeys.begin(); currSectionKey != sectionKeys.end(); currSectionKey++, currSectionKeyNo++)
        {
            actionProgressPercent = (DWORD) (( (float) currSectionKeyNo/ (float) sectionKeys.size()) * 100);

            if (_tcsicmp(currSectionKey->first.c_str(), "!KEY!") == 0)
            {
                if (_tcsnicmp(currSectionKey->second.c_str(), "\\", 1) == 0)
                    newKeyPath = "HKEY_LOCAL_MACHINE" + currSectionKey->second;
                else
                    newKeyPath = "HKEY_LOCAL_MACHINE\\" + currSectionKey->second;
                LogProgressMessage_Action(actionProgressPercent, "%s", newKeyPath.c_str());
                // Yes.  Open/create the specified key (not recursive)
                if (currKeyHandle != NULL)
                {
                    RegCloseKey(currKeyHandle);
                    currKeyHandle = NULL;
                    currKeyPath = "";
                }
                returnValHRb = HRESULT_FROM_WIN32(RegOpenKeyEx(HKEY_LOCAL_MACHINE, currSectionKey->second.c_str(), NULL, KEY_READ | KEY_WRITE, &currKeyHandle));
                if (SUCCEEDED(returnValHRb))
                {
                    currKeyPath = newKeyPath;
                }
                else
                {
                    returnValHRb = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_LOCAL_MACHINE, currSectionKey->second.c_str(), NULL, NULL, NULL, KEY_ALL_ACCESS, NULL, &currKeyHandle, NULL));
                    // Notify caller if we created a new key
                    if (SUCCEEDED(returnValHRb))
                    {
                        currKeyPath = newKeyPath;
                        LogMessage(StatusCallbackFunctionEventType_Info, LS(IDS_NWINSTALL_INFO_CREATEDKEY), currKeyPath.c_str());
                    }
                    if (FAILED(returnValHRb))
                        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_CREATINGKEY), returnValHRb, newKeyPath.c_str());
                }
            }
            else
            {
                returnValHRb = CopyRegistryEntriesValueLine(currSectionKey->first.c_str(), currSectionKey->second.c_str(), currKeyHandle, currKeyPath, overwriteExistingValues);
            }

            // Handle errors
            if (FAILED(returnValHRb))
            {
                if (!firstErrorCodeSaved)
                {
                    returnValHR = returnValHRb;
                    firstErrorCodeSaved = true;
                }
                if (!ignoreFailures)
                    break;
            }
        }
        LogProgressMessage_Action(100, currKeyPath.c_str());

        if (currKeyHandle != NULL)
        {
            RegCloseKey(currKeyHandle);
            currKeyHandle = NULL;
        }        
    }
    return returnValHR;
}

HRESULT NetwareInstaller::CopyRegistryEntriesValueLine( LPCTSTR entryName, LPCTSTR entryValue, HKEY keyHandle, tcstring currKeyPath, bool overwriteExistingValues )
// Handles one line of a registry section of an install script, logging out all failures.
// Returns S_OK on success, S_FALSE if overwriteExistingValues and value already exists, else the error code of the failure.
{
    LPCTSTR entryValueData              = NULL;
    TCHAR   entryValueType              = ' ';
    tcstring    entryFullPathname       = "";
    DWORD   valueType                   = REG_DWORD;
    DWORD   valueDataSize               = 0;
    DWORD   valueDW                     = 0;
    BYTE    dataBuffer[4096]            = {0};
    DWORD   valueSize                   = 1024;
    TCHAR   binaryDataLengthStr[5]      = {""};
    DWORD   binaryDataLength            = 0;
    DWORD   returnValDW                 = ERROR_OUT_OF_PAPER;
    HRESULT returnValHR                 = E_FAIL;

    // Validate parameters
    if ((entryName == NULL) || (entryValue == NULL) || (keyHandle == NULL))
        return E_INVALIDARG;
    if (_tcslen(entryName) == 0)
        return E_INVALIDARG;
    if (_tcslen(entryValue) == 0)
        return E_INVALIDARG;
    // Validate state
    if (!RegFunctionsLoaded)
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_REGISTRY_NOTLOADED));
        return E_ABORT;
    }

    // Initialize
    entryValueData = CharNext(entryValue);      // Not really necessary - 1st char is ALWAYS ASCII
    entryValueType = entryValue[0];
    entryFullPathname = currKeyPath +"\\"+ entryName;
    returnValDW = RegQueryValueEx(keyHandle, entryName, NULL, &valueType, NULL, &valueDataSize); 
    if (((returnValDW == ERROR_SUCCESS) || (returnValDW == ERROR_MORE_DATA)) && !overwriteExistingValues)
    {
        LogMessage(StatusCallbackFunctionEventType_Info, LS(IDS_NWINSTALL_INFO_SKIPPINGEXISTINGVALUE), entryFullPathname.c_str());
        return S_FALSE;
    }

    switch (entryValueType)
    {
        case 'D':
            valueDW = _ttoi(entryValueData);
            returnValDW = RegSetValueEx(keyHandle, entryName, 0, REG_DWORD, (BYTE*) &valueDW, sizeof(valueDW));
            if (returnValDW == ERROR_SUCCESS)
                LogMessage(StatusCallbackFunctionEventType_Info, LS(IDS_NWINSTALL_INFO_SETDWORD), entryFullPathname.c_str(), valueDW);
            else
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_SETTINGDWORD), HRESULT_FROM_WIN32(returnValDW), entryFullPathname.c_str(), entryValueData);
            returnValHR = HRESULT_FROM_WIN32(returnValDW);
            break;

        case 'S':
            returnValDW = RegSetValueEx(keyHandle, entryName, 0, REG_SZ, (PBYTE) entryValueData, (DWORD) _tcslen(entryValueData)+sizeof(TCHAR));
            if (returnValDW == ERROR_SUCCESS)
                LogMessage(StatusCallbackFunctionEventType_Info, LS(IDS_NWINSTALL_INFO_SETSTRING), entryFullPathname.c_str(), entryValueData);
            else
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_SETTINGSTRING), HRESULT_FROM_WIN32(returnValDW), entryFullPathname.c_str(), entryValueData);
            returnValHR = HRESULT_FROM_WIN32(returnValDW);
            break;
/*
        case 'M' :
            dataBuffer[0] = 0;
            dataBuffer[1] = 0;
            entryValue = _tcschr(CharNext(entryValue), ';');
            RegQueryValueEx(keyHandle, entryName, 0, &type, dataBuffer, &valueSize);
            while (entryValue)
            {
                valueSize = Append((char*) dataBuffer, entryValue);
                entryValue = _tcschr(CharNext(entryValue), ';');
            }
            if (valueSize <= 1)
            {
                RegDeleteValue(keyHandle, entryName);
            }
            else
            {
                returnValDW = RegSetValueEx(keyHandle,entryName, 0, REG_MULTI_SZ, dataBuffer, valueSize);
                if (returnValDW != ERROR_SUCCESS)
                    DluxLogInstallMessage(LS(IDS_INSTHELP_ERROR_SETTINGMULTISTRING), entryName);
            }
            returnValHR = HRESULT_FROM_WIN32(returnValDW);
            break;
*/
        case 'B' :
            ZeroMemory(&dataBuffer, sizeof(dataBuffer));

            // Determine the length of the binary data, stored as the 1st 4 characters of the binary data string, in hex
            memcpy(binaryDataLengthStr, entryValueData, 4*sizeof(TCHAR));
            entryValueData = CharNext(entryValueData);
            entryValueData = CharNext(entryValueData);
            entryValueData = CharNext(entryValueData);
            entryValueData = CharNext(entryValueData);
            binaryDataLengthStr[4] = 0;
            binaryDataLength = (DWORD) _tcstol(binaryDataLengthStr, NULL, 16);

            // Convert the characters to the bytes they represent
            ConvertBinaryStringToBytes(dataBuffer, sizeof(dataBuffer), entryValueData);
            returnValDW = RegSetValueEx(keyHandle, entryName, 0, REG_BINARY, dataBuffer, binaryDataLength);
            if (returnValDW == ERROR_SUCCESS)
                LogMessage(StatusCallbackFunctionEventType_Info, LS(IDS_NWINSTALL_INFO_SETBINARY), entryFullPathname.c_str(), entryValueData);
            else
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_SETTINGBINARY), HRESULT_FROM_WIN32(returnValDW), entryFullPathname.c_str(), entryValueData);
            returnValHR = HRESULT_FROM_WIN32(returnValDW);
            break;

        default:
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_UNRECOGNIZEDTYPE), entryValueType, entryFullPathname.c_str(), entryValueData);
            returnValHR = E_FAIL;
            break;
    }
    
    return returnValHR;
}

HRESULT NetwareInstaller::CopyFiles( LPCTSTR filename, LPCTSTR sectionName, StringPairList variables, LPCTSTR sourceDirectory, bool overwriteAlways, bool ignoreFailures )
// Processes the specified file section.  Normally, files are only overwritten if a last modification date comparison shows the source file as equal or newer.
// Errors are reported to errorLogCallback, if specified.
// Variables is a list of text replacements to make where variables are specified by $<varname>$ in the specified file
// If overwriteAlways is TRUE, files are always overwritten.
// Returns S_OK on success, S_FALSE if nothing to do and logs nothing to do, else the error code of the failure
{
    StringPairList              sectionKeys;
    StringPairList::iterator    currSectionKey;
    FileEntry                   currFileEntry;
    TCHAR                       sourceFilename[2048]    = {""};
    bool                        copyFile                = false;
    struct stat                 sourceFileInfo;
    struct stat                 targetFileInfo;
    tm                          targetFileLMD;
    tm                          sourceFileLMD;
    DWORD                       currSectionKeyNo        = 0;
    DWORD                       actionProgressPercent   = 0;
    bool                        firstErrorCodeSaved     = false;
    HRESULT                     returnValHR             = E_FAIL;
    HRESULT                     returnValHRb            = E_FAIL;
    DWORD                       returnValDW             = ERROR_OUT_OF_PAPER;

    // Validate parameters
    if ((filename == NULL) || (sectionName == NULL))
        return E_POINTER;

    // Initialize
    returnValHR = GetIniSection(filename, sectionName, &sectionKeys);
    if (returnValHR == S_OK)
    {
        returnValHR = ReplaceVariables(&sectionKeys, variables);
        if (FAILED(returnValHR))
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_REPLACINGVARIABLES), returnValHR, sectionName, filename);
    }
    else if (returnValHR == S_FALSE)
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_SECTIONNOTFOUND), sectionName, filename);
        returnValHR = E_FAIL;
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_READINGSECTION), returnValHR, sectionName, filename);
    }
    
    
    if (SUCCEEDED(returnValHR))
    {
        // Base case - quit if nothing to do
        if (sectionKeys.size() == 0)
        {
            LogMessage(StatusCallbackFunctionEventType_Info, LS(IDS_NWINSTALL_WARNING_EMPTYSECTION), sectionName, currScriptFilename.c_str());
            return S_FALSE;
        }
        returnValHR = S_OK;
        for (currSectionKey = sectionKeys.begin(); currSectionKey != sectionKeys.end(); currSectionKey++)
        {
            // Section key format:  <file key>=<file entry data>
            //    <file key> := arbitrary unique ID
            //    <file entry data> := parsed by ParseFileEntry into
            //       <source filename> := filename relative to sourceDirectory
            //       <target filename> := full pathname.  In file, should use either MAINPROGRAMDIR or SYS variables.
            // Initialize
            returnValHRb = S_OK;
            returnValDW = ERROR_OUT_OF_PAPER;
            copyFile = false;
            currFileEntry.sourceFilename = "";
            currFileEntry.targetFilename = "";
            actionProgressPercent = (DWORD) (( (float) currSectionKeyNo/ (float) sectionKeys.size()) * 100);

            // Parse this entry
            returnValHRb = ParseFileEntry(currSectionKey->second, &currFileEntry);
            if (SUCCEEDED(returnValHRb))
                sssnprintf(sourceFilename, sizeof(sourceFilename), "%s\\%s", sourceDirectory, currFileEntry.sourceFilename.c_str());
            else
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_PARSINGFILEENTRY), returnValHRb, currSectionKey->first.c_str(), currSectionKey->second.c_str(), sectionName, sourceFilename);

            // Make sure the source file exists
            if (SUCCEEDED(returnValHRb))
                returnValDW = access(sourceFilename, ACCESS_MODE_READ);
            if (returnValDW == 0)
            {
                // Does the target file exist and can we access it?
                returnValDW = access(currFileEntry.targetFilename.c_str(), ACCESS_MODE_READWRITE);
                if (returnValDW == 0)
                {
                    // Yes and yes.  Is the existing file older?
                    stat(sourceFilename, &sourceFileInfo);
                    stat(currFileEntry.targetFilename.c_str(), &targetFileInfo);
                    sourceFileLMD = *localtime(&(sourceFileInfo.st_mtime));
                    targetFileLMD = *localtime(&(targetFileInfo.st_mtime));
                    if (sourceFileInfo.st_mtime >= targetFileInfo.st_mtime)
                    {
                        copyFile = true;
                    }
                    else
                    {
                        if (overwriteAlways)
                        {
                            copyFile = true;
                            LogMessage(StatusCallbackFunctionEventType_Warning, LS(IDS_NWINSTALL_WARNING_OVERWRITINGNEWERFILE), currFileEntry.targetFilename.c_str(), (targetFileLMD.tm_year+1900), targetFileLMD.tm_mon, targetFileLMD.tm_mday, targetFileLMD.tm_hour, targetFileLMD.tm_min, targetFileLMD.tm_sec, sourceFilename, (sourceFileLMD.tm_year+1900), sourceFileLMD.tm_mon, sourceFileLMD.tm_mday, sourceFileLMD.tm_hour, sourceFileLMD.tm_min, sourceFileLMD.tm_sec);
                        }
                        else
                        {
                            LogMessage(StatusCallbackFunctionEventType_Warning, LS(IDS_NWINSTALL_WARNING_NOTOVERWRITINGNEWERFILE), currFileEntry.targetFilename.c_str(), (targetFileLMD.tm_year+1900), targetFileLMD.tm_mon, targetFileLMD.tm_mday, targetFileLMD.tm_hour, targetFileLMD.tm_min, targetFileLMD.tm_sec, sourceFilename, (sourceFileLMD.tm_year+1900), sourceFileLMD.tm_mon, sourceFileLMD.tm_mday, sourceFileLMD.tm_hour, sourceFileLMD.tm_min, sourceFileLMD.tm_sec);
                        }
                    }
                }
                else
                {
                    if (errno == ENOENT)
                    {
                        // No and Yes, we're good.
                        copyFile = true;
                    }
                    else if (errno == EACCES)
                    {
                        // Yes and no -  insufficient rights
                        copyFile = false;
                        returnValHRb = HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
                        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_COPYACCESSDENIED), sourceFilename, currFileEntry.targetFilename.c_str());
                    }
                    else
                    {
                        // No...but we don't know why
                        copyFile = false;
                        returnValHRb = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, errno);
                        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_UNKNOWNCOPYERROR), errno, sourceFilename, currFileEntry.targetFilename.c_str());
                    }
                }
            }
            else
            {
                copyFile = false;
                returnValHRb = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_SOURCENOTFOUND), sourceFilename, currFileEntry.targetFilename.c_str());
            }
            if (copyFile)
            {
                returnValHRb = RecursiveCreateDirectory(GetParentPath(currFileEntry.targetFilename.c_str()));
                if (SUCCEEDED(returnValHRb))
                {
                    // Notify callback if we created the target directory for this file
                    if (returnValHRb == S_OK)
                        LogMessage(StatusCallbackFunctionEventType_Info, LS(IDS_NWINSTALL_INFO_CREATEDDIRECTORY), GetParentPath(currFileEntry.targetFilename.c_str()).c_str());
                    // Notify callback that we're copying a file
                    LogProgressMessage_Action(actionProgressPercent, GetNodeName(sourceFilename).c_str());
                    LogMessage(StatusCallbackFunctionEventType_Info, LS(IDS_NWINSTALL_INFO_COPYINGFILE), sourceFilename, currFileEntry.targetFilename.c_str());
                    // Copy the file
                    returnValHRb = CopyFileDo(sourceFilename, currFileEntry.targetFilename.c_str(), false);
                    if (FAILED(returnValHRb))
                        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_COPYINGFILE), returnValHRb, sourceFilename, currFileEntry.targetFilename.c_str());
                }
                else
                {
                    LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_CREATINGTARGETDIR), returnValHRb, GetParentPath(currFileEntry.targetFilename.c_str()).c_str(), sourceFilename, currFileEntry.targetFilename.c_str());
                }
            }
            else
            {
                // Make sure progress percentage is advanced
                if (SUCCEEDED(returnValHRb))
                    LogProgressMessage_Action(actionProgressPercent, LS(IDS_NWINSTALL_PROGRESS_SKIPPINGFILE), GetNodeName(sourceFilename).c_str());
                else
                    LogProgressMessage_Action(actionProgressPercent, GetNodeName(sourceFilename).c_str());
            }

            // Handle errors
            if (FAILED(returnValHRb))
            {
                if (!firstErrorCodeSaved)
                {
                    returnValHR = returnValHRb;
                    firstErrorCodeSaved = true;
                }
                if (!ignoreFailures)
                    break;
            }
            currSectionKeyNo += 1;
        }
        LogProgressMessage_Action(100, GetNodeName(sourceFilename).c_str());
    }

    return returnValHR;
}

HRESULT NetwareInstaller::DeleteFiles( LPCTSTR filename, LPCTSTR sectionName, StringPairList variables, bool ignoreFailures )
// Processes the specified file section.  Normally, files are only overwritten if a last modification date comparison shows the source file as equal or newer.
// Errors are reported to errorLogCallback, if specified.
// Variables is a list of text replacements to make where variables are specified by $<varname>$ in the specified file
// If overwriteAlways is TRUE, files are always overwritten.
// Returns S_OK on success, S_FALSE if nothing to do and logs nothing to do, else the error code of the failure
{
    StringPairList              sectionKeys;
    StringPairList::iterator    currSectionKey;
    FileEntry                   currFileEntry;
    DWORD                       currSectionKeyNo        = 1;
    DWORD                       actionProgressPercent   = 0;
    bool                        firstErrorCodeSaved     = false;
    HRESULT                     returnValHR             = E_FAIL;
    HRESULT                     returnValHRb            = E_FAIL;
    DWORD                       returnValDW             = ERROR_OUT_OF_PAPER;

    // Validate parameters
    if ((filename == NULL) || (sectionName == NULL))
        return E_POINTER;

    // Initialize
    returnValHR = GetIniSection(filename, sectionName, &sectionKeys);
    if (returnValHR == S_OK)
    {
        returnValHR = ReplaceVariables(&sectionKeys, variables);
        if (FAILED(returnValHR))
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_REPLACINGVARIABLES), returnValHR, sectionName, filename);
    }
    else if (returnValHR == S_FALSE)
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_SECTIONNOTFOUND), sectionName, filename);
        returnValHR = E_FAIL;
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_READINGSECTION), returnValHR, sectionName, filename);
    }
    
    if (SUCCEEDED(returnValHR))
    {
        // Base case - quit if nothing to do
        if (sectionKeys.size() == 0)
        {
            LogMessage(StatusCallbackFunctionEventType_Info, 0, LS(IDS_NWINSTALL_WARNING_EMPTYSECTION), sectionName, currScriptFilename.c_str());
            return S_FALSE;
        }
        returnValHR = S_OK;
        for (currSectionKey = sectionKeys.begin(); currSectionKey != sectionKeys.end(); currSectionKey++)
        {
            // Section key format:  <file key>=<file entry data>
            //    <file key> := arbitrary unique ID
            //    <file entry data> := parsed by ParseFileEntry into
            //       <source filename> := filename relative to sourceDirectory
            //       <target filename> := full pathname.  In file, should use either MAINPROGRAMDIR or SYS variables.
            // Initialize
            returnValHRb = S_OK;
            returnValDW = ERROR_OUT_OF_PAPER;
            actionProgressPercent = (DWORD) (( (float) currSectionKeyNo/ (float) sectionKeys.size()) * 100);

            // Parse this entry
            returnValHRb = ParseFileEntry(currSectionKey->second, &currFileEntry, true);
            if (SUCCEEDED(returnValHRb))
            {
                // Notify callback that we're deleting a file
                LogProgressMessage_Action(actionProgressPercent, currFileEntry.targetFilename.c_str());
                returnValHRb = DeleteFileDo(currFileEntry.targetFilename.c_str(), true);
                if (returnValHRb == S_FALSE)
                    LogMessage(StatusCallbackFunctionEventType_Info, LS(IDS_NWINSTALL_INFO_FILEALREADYDELETED), currFileEntry.targetFilename.c_str());
                else if (FAILED(returnValHRb))
                    LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_DELETINGFILE), returnValHRb, currFileEntry.targetFilename.c_str());
            }
            else
            {
                LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_PARSINGFILEENTRY), returnValHRb, currSectionKey->first.c_str(), currSectionKey->second.c_str(), sectionName, filename);
            }

            // Handle errors
            if (FAILED(returnValHRb))
            {
                if (!firstErrorCodeSaved)
                {
                    returnValHR = returnValHRb;
                    firstErrorCodeSaved = true;
                }
                if (!ignoreFailures)
                    break;
            }
            currSectionKeyNo += 1;
        }
        LogProgressMessage_Action(100, currFileEntry.targetFilename.c_str());
    }
    return returnValHR;
}

HRESULT NetwareInstaller::ParseFileEntry( tcstring keyValue, FileEntry* entry, bool emptySourceOK )
{
    LPCTSTR     separatorPos = NULL;
    LPCTSTR     arg1End         = NULL;
    LPCTSTR     arg2Start       = NULL;

    // Validate parameters
    if (keyValue == "")
        return E_INVALIDARG;
    if (entry == NULL)
        return E_POINTER;

    separatorPos = _tcschr(keyValue.c_str(), ',');
    if (separatorPos != NULL)
    {
        entry->sourceFilename = keyValue.substr(0, separatorPos - keyValue.c_str());
        arg1End = FindLastNonWhitespace(entry->sourceFilename.c_str());
        if (arg1End != NULL)
            entry->sourceFilename = entry->sourceFilename.substr(0, CharNext(arg1End) - entry->sourceFilename.c_str());
        arg2Start = FindFirstNonWhitespace(CharNext(separatorPos));
        if (arg2Start != NULL)
            entry->targetFilename = keyValue.substr((arg2Start - keyValue.c_str()));
        if (((entry->sourceFilename != "") && (entry->targetFilename != "")) || ((entry->sourceFilename == "") && emptySourceOK))
            return S_OK;
        else
            return E_FAIL;
    }
    else
    {
        return E_FAIL;
    }
}

HRESULT NetwareInstaller::CreateGroups( tcstring sectionName, bool install, bool useNDS )
{
    StringPairList              sectionKeys;
    StringPairList::iterator    currLine;
    HRESULT                     returnValHR     = S_OK;
    DWORD                       returnValDW     = ERROR_OUT_OF_PAPER;

    // Initialize
    if (install)
        LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_NET_CREATEGROUPS));
    else
        LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_NET_REMOVEGROUPS));
    returnValHR = GetIniSectionResolved(sectionName.c_str(), &sectionKeys);
    if (SUCCEEDED(returnValHR))
    {
        // Set operation mode global for InitNLM code
        if (install)
            Install = TRUE;
        else
            Install = FALSE;

        // Process the section
        returnValHR = S_OK;
        for (currLine = sectionKeys.begin(); currLine != sectionKeys.end(); currLine++)
        {
            LogMessage(StatusCallbackFunctionEventType_Info, "Processing CreateGroups(%d) line %s", (DWORD) useNDS, currLine->second.c_str());
            if (useNDS)
                returnValDW = ParseObjLine((LPTSTR) "GROUPMEMBERS", (LPTSTR) currLine->second.c_str(), 1);
            else
                returnValDW = ParseObjLine((LPTSTR) "GROUPMEMBERS", (LPTSTR) currLine->second.c_str(), 0);
            
            if (returnValDW != ERROR_SUCCESS)
                returnValHR = LogInitNLMerror(returnValDW, install, "CreateGroups", currLine->second.c_str());
        }
    }

    return returnValHR;
}

HRESULT NetwareInstaller::SetRights( tcstring sectionName, bool install, bool useNDS )
{
    StringPairList              sectionKeys;
    StringPairList::iterator    currLine;
    HRESULT                     returnValHR     = S_OK;
    DWORD                       returnValDW     = ERROR_OUT_OF_PAPER;

    // Initialize
    if (install)
        LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_NET_GRANTRIGHTS));
    else
        LogProgressMessage_Overall(LS(IDS_NWINSTALL_PROGRESS_NET_REVOKERIGHTS));
    returnValHR = GetIniSectionResolved(sectionName.c_str(), &sectionKeys);
    if (SUCCEEDED(returnValHR))
    {
        // Set operation mode global for InitNLM code
        if (install)
            Install = TRUE;
        else
            Install = FALSE;

        // Process the section
        returnValHR = S_OK;
        for (currLine = sectionKeys.begin(); currLine != sectionKeys.end(); currLine++)
        {
            LogMessage(StatusCallbackFunctionEventType_Info, "Processing SetRights line %s", currLine->second.c_str());
            if (useNDS)
                returnValDW = ParseObjLine((LPTSTR) "GROUPPRIMS", (LPTSTR) currLine->second.c_str(), 1);
            else
                returnValDW = ParseObjLine((LPTSTR) "GROUPPRIMS", (LPTSTR) currLine->second.c_str(), 0);
            if (returnValDW != ERROR_SUCCESS)
                returnValHR = LogInitNLMerror(returnValDW, install, "SetRights", currLine->second.c_str());
        }
    }

    return returnValHR;
}

HRESULT NetwareInstaller::UpdateLoginScript( tcstring sectionName, bool install, bool useNDS )
{
    StringList                  sectionLines;
    StringList::iterator        currLine;
    HRESULT                     returnValHR     = S_OK;
    DWORD                       returnValDW     = ERROR_OUT_OF_PAPER;

    // Initialize
    returnValHR = GetIniSectionResolved(sectionName.c_str(), &sectionLines);
    if (SUCCEEDED(returnValHR))
    {
        LogMessage(StatusCallbackFunctionEventType_Info, "UpdateLoginScript:  Processing section %s (%d lines)", sectionName.c_str(), sectionLines.size());
        // Set operation mode global for InitNLM code
        if (install)
        {
            Install = TRUE;
            RemoveInProgress = FALSE;
        }
        else
        {
            Install = FALSE;
            RemoveInProgress = TRUE;
        }
            
        // Process the section
        returnValHR = S_OK;
        for (currLine = sectionLines.begin(); currLine != sectionLines.end(); currLine++)
        {
            if (_tcsicmp(currLine->c_str(), "!END!") != 0)
            {
                if (useNDS)
                    returnValDW = ParseProfileLogin((LPTSTR) currLine->c_str(), NULL);
                else
                    returnValDW = ParseBinderyLogin((LPTSTR) currLine->c_str(), NULL);
            }
            else
            {
                LogMessage(StatusCallbackFunctionEventType_Info, "UpdateLoginScript:  Finalizing");
                if (useNDS)
                    returnValDW = ParseProfileLogin(NULL, NULL);
                else
                    returnValDW = ParseBinderyLogin(NULL, NULL);
            }
            if (returnValDW != ERROR_SUCCESS)
                returnValHR = LogInitNLMerror(returnValDW, install, "UpdateLoginScript", currLine->c_str());
        }
    }

    return returnValHR;
}

HRESULT NetwareInstaller::LogInitNLMerror( DWORD returnValDW, bool install, tcstring commandName, tcstring commandArg )
{
    HRESULT returnValHR = E_FAIL;
    bool uninstallObjectNotFound = false;
    
    if (returnValDW == ERROR_SUCCESS)
    {
        returnValHR = S_OK;
    }
    else
    {
        if (!install && ((returnValDW & 0xff000000) == EI_DSRET) && ((returnValDW & 0xffffff) == (ERR_NO_SUCH_ENTRY & 0xffffff)))
            uninstallObjectNotFound = true;
        if (!install && ((returnValDW & 0xff000000) == EI_DSRET) && ((returnValDW & 0xffffff) == (NO_SUCH_OBJECT & 0xffffff)))
            uninstallObjectNotFound = true;
        if (!uninstallObjectNotFound)
        {
            LogMessage(StatusCallbackFunctionEventType_Info, "%s:  NDS error %d (0x%08x) processing line %s.", commandName.c_str(), (DWORD) (returnValDW & 0xffffff), returnValDW, commandArg.c_str());
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Info, "%s:  NDS returned object not found (0x%08x) processing line %s.", commandName.c_str(), returnValDW, commandArg.c_str());
            returnValHR = S_OK;
        }
    }
    
    return returnValHR;
}

HRESULT NetwareInstaller::OpenKeyLogMessage( HKEY parentKey, tcstring keyPath, HKEY* keyHandle, bool failureIsError )
// Opens the specified registry key and logs any failure.  Also logs if key handle is already open (!= NULL)
{
    StatusCallbackFunctionEventType     messageType     = StatusCallbackFunctionEventType_Error;
    DWORD                               returnValDW     = ERROR_OUT_OF_PAPER;
    
    // Validate parameters
    if (keyHandle == NULL)
        return E_FAIL;

    if (!failureIsError)
        messageType = StatusCallbackFunctionEventType_Info;
    if (*keyHandle == NULL)
    {
        returnValDW = RegOpenKey(parentKey, keyPath.c_str(), keyHandle);
        if (returnValDW != ERROR_SUCCESS )
            LogMessage(messageType, LS(IDS_NWINSTALL_ERROR_OPENINGKEY), HRESULT_FROM_WIN32(returnValDW), keyPath.c_str());
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Info, "Key %s already open.", keyPath.c_str());
    }
    
    return HRESULT_FROM_WIN32(returnValDW);
}

HRESULT NetwareInstaller::DumpKeyToFile( HKEY keyHandle, tcstring keyPathname, FILE* targetFileHandle )
{
    TCHAR       valueName[MAX_PATH]     = {""};
    DWORD       valueNameSize           = sizeof(valueName)/sizeof(valueName[0]);
    TCHAR       valueData[MAX_PATH]     = {""};
    DWORD       valueDataSize           = sizeof(valueData)/sizeof(valueData[0]);
    DWORD       valueType               = REG_DWORD;
    DWORD       currValueNo             = 0;
    DWORD       currKeyNo               = 0;
    HKEY        subkeyHandle            = NULL;
    tcstring    subkeyPathname;
    DWORD       returnValDW             = ERROR_OUT_OF_PAPER;

    // Validate parameters
    if ((keyHandle == NULL) || (targetFileHandle == NULL))
        return E_POINTER;
    
    // Export values in this key
    fprintf(targetFileHandle, "[%s]\n", keyPathname.c_str());
    returnValDW = RegEnumValue(keyHandle, currValueNo, valueName, &valueNameSize, 0, &valueType, (BYTE*) valueData, &valueDataSize);
    while (returnValDW == ERROR_SUCCESS)
    { 
        switch (valueType)
        {
            case REG_SZ:
                fprintf(targetFileHandle, "%s = >%s<\n", valueName, valueData);
                break;
            case REG_DWORD:
                fprintf(targetFileHandle, "%s = 0x%08x\n", valueName, *((DWORD*) &valueData));
                break;
            case REG_BINARY:
                fprintf(targetFileHandle, "%s = (%d bytes of binary data)\n", valueName, valueDataSize);
                break;
            default:
                fprintf(targetFileHandle, "%s (Unsupported type 0x%08x)\n", valueName, valueType);
                break;
        }
        valueName[0] = NULL;
        valueData[0] = NULL;
        valueNameSize = sizeof(valueName)/sizeof(valueName[0]);
        valueDataSize = sizeof(valueData)/sizeof(valueData[0]);
        currValueNo += 1;
        returnValDW = RegEnumValue(keyHandle, currValueNo, valueName, &valueNameSize, 0, &valueType, (BYTE*) valueData, &valueDataSize);
    }
    fprintf(targetFileHandle, "\n");

    // Export all subkeys, one at a time.
    valueNameSize = sizeof(valueName)/sizeof(valueName[0]);
    returnValDW = RegEnumKey(keyHandle, currKeyNo, valueName, valueNameSize);
    while (returnValDW == ERROR_SUCCESS)
    { 
        returnValDW = RegOpenKey(keyHandle, valueName, &subkeyHandle);
        subkeyPathname = keyPathname +tcstring("\\")+ valueName;
        if (returnValDW == ERROR_SUCCESS)
        {
            DumpKeyToFile(subkeyHandle, subkeyPathname, targetFileHandle);
            subkeyPathname = "";
            RegCloseKey(subkeyHandle);
            subkeyHandle = NULL;
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_OPENINGKEY), HRESULT_FROM_WIN32(returnValDW), keyPathname.c_str(), subkeyPathname.c_str());
        }
        subkeyPathname = "";
        currKeyNo += 1;
        valueNameSize = sizeof(valueName)/sizeof(valueName[0]);
        returnValDW = RegEnumKey(keyHandle, currKeyNo, valueName, valueNameSize);
    }

    return S_OK;
}

// Variable handling
HRESULT NetwareInstaller::ReplaceVariables( StringList* sectionLines, StringPairList variables )
// Replaces all instances of the variables named in variables (name, value) in sectionLines with the corresponding value
{
    StringList::iterator        currLine;
    StringPairList::iterator    currVariable;
    tcstring                    variableString;
    tcstring                    resultString;

    // Validate parameters
    if (sectionLines == NULL)
        return E_POINTER;

    // Replace variables
    for (currVariable = variables.begin(); currVariable != variables.end(); currVariable++)
    {
        variableString = tcstring("$") + currVariable->first + tcstring("$");
        for (currLine = sectionLines->begin(); currLine != sectionLines->end(); currLine++)
        {
            StringReplace(*currLine, variableString, currVariable->second, &resultString);
            *currLine = resultString;
        }
    }
    return S_OK;
}

HRESULT NetwareInstaller::ReplaceVariables( StringPairList* sectionKeys, StringPairList variables )
// Replaces all instances of the variables named in variables (name, value) in sectionLines with the corresponding value
{
    StringPairList::iterator    currKey;
    StringPairList::iterator    currVariable;
    tcstring                    variableString;
    tcstring                    resultString;

    // Validate parameters
    if (sectionKeys == NULL)
        return E_POINTER;

    // Replace variables
    for (currVariable = variables.begin(); currVariable != variables.end(); currVariable++)
    {
        variableString = tcstring("$") + currVariable->first + tcstring("$");
        for (currKey = sectionKeys->begin(); currKey != sectionKeys->end(); currKey++)
        {
            StringReplace(&(currKey->first), variableString, currVariable->second);
            StringReplace(&(currKey->second), variableString, currVariable->second);
        }
    }
    return S_OK;
}

HRESULT NetwareInstaller::ReplaceVariables( tcstring* sourceLine, StringPairList variables )
{
    StringPairList::iterator    currVariable;
    tcstring                    variableString;

    // Validate parameters
    if (sourceLine == NULL)
        return E_FAIL;
        
    // Replace variables
    for (currVariable = variables.begin(); currVariable != variables.end(); currVariable++)
    {
        variableString = tcstring("$") + currVariable->first + tcstring("$");
        StringReplace(sourceLine, variableString, currVariable->second);
    }
    return S_OK;
}


HRESULT NetwareInstaller::ReadVariablesFromFile( LPCTSTR filename, LPCTSTR sectionName, StringPairList* variableList )
{
    // Validate parameters
    if (filename == NULL)
        return E_POINTER;
    if (variableList == NULL)
        return E_POINTER;

    return GetIniSection(filename, sectionName, variableList);
}

HRESULT NetwareInstaller::GetVariable( StringPairList variables, LPCTSTR variableName, tcstring* variableValue )
// Searches for the variable named variableName in variables and sets *variableValue equal to it's value
// Returns S_OK on success, S_FALSE if variableName not found, else the error code of the failure
{
    StringPairList::iterator        currVariable;

    // Validate parameters
    if (variableValue == NULL)
        return E_POINTER;

    for (currVariable = variables.begin(); currVariable != variables.end(); currVariable++)
    {
        if (_tcsicmp(currVariable->first.c_str(), variableName) == 0)
        {
            *variableValue = currVariable->second;
            return S_OK;
        }
    }
    return S_FALSE;
}

HRESULT NetwareInstaller::SetVariable( StringPairList* variables, LPCTSTR variableName, tcstring newValue )
// Sets the variable variableName equal to the specified value.
// Returns S_OK on success, S_FALSE if success and variable did not previously exist, else the error code of the failure
{
    StringPairList::iterator        currVariable;
    StringPair                      newVariable;

    // Validate parameters
    if (variables == NULL)
        return E_POINTER;
    if (variableName == NULL)
        return E_POINTER;

    for (currVariable = variables->begin(); currVariable != variables->end(); currVariable++)
    {
        if (_tcsicmp(currVariable->first.c_str(), variableName) == 0)
        {
            currVariable->second = newValue;
            return S_OK;
        }
    }
    
    // Variable not found - add it
    newVariable.first = variableName;
    newVariable.second = newValue;
    variables->push_back(newVariable);
    return S_FALSE;
}

HRESULT NetwareInstaller::ReadVariableLogFailure( StringPairList variables, tcstring variableName, tcstring* variableValue, tcstring sectionName, tcstring filename )
{
    tcstring tempVariableValue;
    HRESULT returnValHR = E_FAIL;

    // Validate parameters    
    if (variableValue == NULL)
        return E_POINTER;

    returnValHR = GetVariable(variables, variableName.c_str(), &tempVariableValue);
    if (returnValHR == S_OK)
    {
        if (tempVariableValue != "")
        {
            *variableValue = tempVariableValue;
        }
        else
        {
            returnValHR = E_FAIL;
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_VARIABLENOTSET), variableName.c_str(), sectionName.c_str(), filename.c_str());
        }
    }
    else
    {
        returnValHR = E_FAIL;
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_READINGVARIABLE), returnValHR, variableName.c_str(), sectionName.c_str(), filename.c_str());
    }
    return returnValHR;
}


// ** INI FUNCTIONS **
HRESULT NetwareInstaller::GetIniSection( LPCTSTR filename, LPCTSTR sectionName, StringPairList* sectionKeys )
// Sets *sectionKeys equal to a list of all the key pairs in the specified section
// If sectionName is NULL, reads in all keys in the entire file
// Returns S_OK on success, S_FALSE if section not found, else the error code of the failure
{
    FILE*           fileHandle              = NULL;
    bool            sectionComplete         = false;
    TCHAR           currLine[2048]          = {""};
    tcstring        currSectionName         = "";
    LPTSTR          commentPos              = NULL;
    LPTSTR          separatorPos            = NULL;
    LPTSTR          lastNonWhitespace       = NULL;
    LPTSTR          endSeparatorPos         = NULL;
    bool            foundSection            = false;
    StringPair      currLineKey;
    bool            valueLine               = false;
    HRESULT         returnValHR             = E_FAIL;

    // Validate parameters
    if ((filename == NULL) || (sectionKeys == NULL))
        return E_POINTER;

    returnValHR = fsopen(filename, "rt", &fileHandle);
    if (fileHandle != NULL)
    {
        returnValHR = E_FAIL;
        sectionKeys->clear();
        while ((fgets(currLine, sizeof(currLine)/sizeof(currLine[0]), fileHandle) != NULL) && !sectionComplete)
        {
            // Initialize
            separatorPos        = NULL;
            endSeparatorPos     = NULL;
            commentPos          = NULL;
            valueLine           = false;

            // Make sure currLine is NULL terminated
            currLine[(sizeof(currLine)/sizeof(currLine[0]))-1] = NULL;
            
            // Strip off any comment
            commentPos = _tcschr(currLine, ';');
            if (commentPos != NULL)
                *commentPos = NULL;
            TerminateAtNewline(currLine);

            // Strip off trailing whitespace
            lastNonWhitespace = (LPTSTR) FindLastNonWhitespace(currLine);
            if (lastNonWhitespace != NULL)
                *CharNext(lastNonWhitespace) = NULL;

            // Are there any non-whitepsace characters on this line?
            separatorPos = (LPTSTR) FindFirstNonWhitespace(currLine);
            if (separatorPos != NULL)
            {
                // Yes.  What kind of line is this?
                if (*separatorPos == '[')
                {
                    // Section start.  Are we limited to a specific section?
                    if (sectionName != NULL)
                    {
                        if (!foundSection)
                        {
                            // New Section Start
                            endSeparatorPos = _tcschr(separatorPos, ']');
                            if (endSeparatorPos != NULL)
                            {
                                currSectionName = CharNext(separatorPos);
                                currSectionName.erase(CharPrev(separatorPos, endSeparatorPos) - CharNext(separatorPos) + 1);
                                if (_tcsicmp(currSectionName.c_str(), sectionName) == 0)
                                    foundSection = true;
                                else
                                    foundSection = false;
                            }
                        }
                        else
                        {
                            sectionComplete = true;
                        }
                    }
                }
                else
                {
                    // Data line
                    // Should we save it?
                    if (foundSection || (sectionName == NULL))
                    {
                        // Yes.  Parse out the key and value text
                        separatorPos = _tcschr(currLine, '=');
                        if (separatorPos != NULL)
                        {
                            lastNonWhitespace = (LPTSTR) FindFirstNonWhitespace(CharNext(separatorPos));
                            if (lastNonWhitespace != NULL)
                                currLineKey.second = lastNonWhitespace;
                            else
                                currLineKey.second = "";
                            *separatorPos = NULL;
                            // Strip off trailing whitespace from key name
                            lastNonWhitespace = (LPTSTR) FindLastNonWhitespace(currLine);
                            if (lastNonWhitespace != NULL)
                                *CharNext(lastNonWhitespace) = NULL;
                            currLineKey.first = currLine;
                            sectionKeys->push_back(currLineKey);
                        }
                    }
                }
            }
        }
        
        
        if (ferror(fileHandle) == 0)
        {
            if (sectionName != NULL)
            {
                if (foundSection)
                    returnValHR = S_OK;
                else
                    returnValHR = S_FALSE;
            }
            else
            {
                returnValHR = S_OK;
            }
        }
        fclose(fileHandle);
        fileHandle = NULL;
    }

    // Translate errno to HRESULT
    if (returnValHR == E_FAIL)
        returnValHR = FileErrnoToHRESULT(errno);
    
    return returnValHR;
}

HRESULT NetwareInstaller::GetIniSection( LPCTSTR filename, LPCTSTR sectionName, StringList* sectionLines )
{
    FILE*           fileHandle              = NULL;
    bool            sectionComplete         = false;
    TCHAR           currLine[2048]          = {""};
    tcstring        currSectionName         = "";
    LPTSTR          commentPos              = NULL;
    LPTSTR          separatorPos            = NULL;
    LPTSTR          lastNonWhitespace       = NULL;
    LPTSTR          endSeparatorPos         = NULL;
    bool            foundSection            = false;
    HRESULT         returnValHR             = E_FAIL;

    // Validate parameters
    if ((filename == NULL) || (sectionName == NULL) || (sectionLines == NULL))
        return E_POINTER;

    returnValHR = fsopen(filename, "rt", &fileHandle);
    if (fileHandle != NULL)
    {
        returnValHR = E_FAIL;
        sectionLines->clear();
        while ((fgets(currLine, sizeof(currLine)/sizeof(currLine[0]), fileHandle) != NULL) && !sectionComplete)
        {
            // Initialize
            separatorPos        = NULL;
            endSeparatorPos     = NULL;
            commentPos          = NULL;

            // Make sure currLine is NULL terminated
            currLine[(sizeof(currLine)/sizeof(currLine[0]))-1] = NULL;
            
            // Strip off any comment
            commentPos = _tcschr(currLine, ';');
            if (commentPos != NULL)
                *commentPos = NULL;
            TerminateAtNewline(currLine);

            // Strip off trailing whitespace
            lastNonWhitespace = (LPTSTR) FindLastNonWhitespace(currLine);
            if (lastNonWhitespace != NULL)
                *CharNext(lastNonWhitespace) = NULL;

            // Are there any non-whitepsace characters on this line?
            separatorPos = (LPTSTR) FindFirstNonWhitespace(currLine);
            if (separatorPos != NULL)
            {
                // Yes.  What kind of line is this?
                if (*separatorPos == '[')
                {
                    if (!foundSection)
                    {
                        // New Section Start
                        endSeparatorPos = _tcschr(separatorPos, ']');
                        if (endSeparatorPos != NULL)
                        {
                            currSectionName = CharNext(separatorPos);
                            currSectionName.erase(CharPrev(separatorPos, endSeparatorPos) - CharNext(separatorPos) + 1);
                            if (_tcsicmp(currSectionName.c_str(), sectionName) == 0)
                                foundSection = true;
                            else
                                foundSection = false;
                        }
                    }
                    else
                    {
                        sectionComplete = true;
                    }
                }
                else
                {
                    // Data line, save only if in right section
                    if (foundSection)
                        sectionLines->push_back(currLine);
                }
            }
        }
        
        
        if (ferror(fileHandle) == 0)
        {
            if (foundSection)
                returnValHR = S_OK;
            else
                returnValHR = S_FALSE;
        }
        fclose(fileHandle);
        fileHandle = NULL;
    }
    
    // Translate errno to HRESULT
    if (returnValHR == E_FAIL)
        returnValHR = FileErrnoToHRESULT(errno);
    return returnValHR;
}

HRESULT NetwareInstaller::GetIniSectionResolved( tcstring sectionName, StringPairList* sectionKeys )
// As GetIniSection on currScriptFilename, but resolves variable values using variables and logs out any failures retrieving section or resolving values
{
    HRESULT     returnValHR = E_FAIL;
    
    returnValHR = GetIniSection(currScriptFilename.c_str(), sectionName.c_str(), sectionKeys);
    if (returnValHR == S_OK)
    {
        returnValHR = ReplaceVariables(sectionKeys, variables);
        if (FAILED(returnValHR))
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_REPLACINGVARIABLES), returnValHR, sectionName.c_str(), currScriptFilename.c_str());
    }
    else if (returnValHR == S_FALSE)
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_SECTIONNOTFOUND), sectionName.c_str(), currScriptFilename.c_str());
        returnValHR = E_FAIL;
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_READINGSECTION), returnValHR, sectionName.c_str(), currScriptFilename.c_str());
    }

    return returnValHR;
}

HRESULT NetwareInstaller::GetIniSectionResolved( tcstring sectionName, StringList* sectionLines )
// As GetIniSection on currScriptFilename, but resolves variable values using variables and logs out any failures retrieving section or resolving values
{
    HRESULT     returnValHR = E_FAIL;
    
    returnValHR = GetIniSection(currScriptFilename.c_str(), sectionName.c_str(), sectionLines);
    if (returnValHR == S_OK)
    {
        returnValHR = ReplaceVariables(sectionLines, variables);
        if (FAILED(returnValHR))
            LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_REPLACINGVARIABLES), returnValHR, sectionName.c_str(), currScriptFilename.c_str());
    }
    else if (returnValHR == S_FALSE)
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_SECTIONNOTFOUND), sectionName.c_str(), currScriptFilename.c_str());
        returnValHR = E_FAIL;
    }
    else
    {
        LogMessage(StatusCallbackFunctionEventType_Error, LS(IDS_NWINSTALL_ERROR_READINGSECTION), returnValHR, sectionName.c_str(), currScriptFilename.c_str());
    }

    return returnValHR;
}

HRESULT NetwareInstaller::GetKeys( StringPairList sectionKeys, LPCTSTR keyName, StringList* keyValues )
// Sets *keyValues equal to the values of all keys in the specified section with the specified name, in the order listed
// Returns S_OK on success, S_FALSE on success but no keys named keyName are found, else the error code of the failure
{
    StringPairList::iterator        currKey;
    HRESULT                         returnValHR = E_FAIL;
    
    // Validate parameters
    if (keyValues == NULL)
        return E_POINTER;

    // Find the keys        
    returnValHR = S_FALSE;
    keyValues->clear();
    for (currKey = sectionKeys.begin(); currKey != sectionKeys.end(); currKey++)
    {
        if (_tcsicmp(keyName, currKey->first.c_str()) == 0)
        {
            keyValues->push_back(currKey->first);
            returnValHR = S_OK;
        }
    }
    
    return returnValHR;
}


// ** MISCELLANEOUS HELPERS **
HRESULT NetwareInstaller::RecursiveCreateDirectory( tcstring pathname )
{
    tcstring    parentPath;
    DWORD       returnValDW         = ERROR_OUT_OF_PAPER;
    HRESULT     returnValHR         = E_FAIL;

    // Validate parameters
    if (pathname == "")
        return E_INVALIDARG;
    if (access(pathname.c_str(), 0) == 0)
        return S_FALSE;

    parentPath = GetParentPath(pathname);
    if (parentPath != "")
        returnValHR = RecursiveCreateDirectory(parentPath);
    else
        returnValHR = S_OK;

    // Create this directory
    if (SUCCEEDED(returnValHR))
    {
        returnValDW = mkdir(pathname.c_str());
        if (returnValDW == 0)
        {
            return S_OK;
        }
        else
        {
            if (errno == EEXIST)
                return S_FALSE;
            else
                return HRESULT_FROM_WIN32(ERROR_CANNOT_MAKE);
        }
    }

    return returnValHR;
}

NetwareInstaller::tcstring NetwareInstaller::GetParentPath( tcstring pathname )
// Returns the path of the parent of pathname, or "" if no parent
{
    LPCTSTR     lastBackslash       = NULL;
    LPCTSTR     lastForewardSlash   = NULL;
    LPCTSTR     colonPos            = NULL;
    LPCTSTR     lastSlash           = NULL;
    HRESULT     returnValHR         = E_FAIL;

    // Validate parameters
    if (pathname == "")
        return "";

    // Locate the final separator
    lastBackslash = _tcsrchr(pathname.c_str(), '\\');
    lastForewardSlash = _tcsrchr(pathname.c_str(), '/');
    colonPos = _tcschr(pathname.c_str(), ':');
    if ((lastBackslash != NULL) && (lastForewardSlash != NULL))
    {
        if (lastBackslash > lastForewardSlash)
            lastSlash = lastBackslash;
        else
            lastSlash = lastForewardSlash;
    }
    else if (lastBackslash != NULL)
    {
        lastSlash = lastBackslash;
    }
    else if (lastSlash != NULL)
    {
        lastSlash = lastForewardSlash;
    }
    else if (colonPos != NULL)
    {
        // Make sure to include the : in the substring - SYS\SYSTEM is not valid, only SYS:\SYSTEM
        return pathname.substr(0, (colonPos - pathname.c_str()) + 1);
    }

    // Create the parent
    if (lastBackslash != NULL)
        return pathname.substr(0, lastBackslash - pathname.c_str());
    else
        return "";
}

NetwareInstaller::tcstring NetwareInstaller::GetNodeName( tcstring pathname )
{
    tcstring parentPath;

    parentPath = GetParentPath(pathname);
    if (parentPath != "")
        return pathname.substr(parentPath.size()+1);        // Separator characters are ASCII and therefore always 1 TCHAR in size
    else
        return "";
}

DWORD NetwareInstaller::ConvertBinaryStringToBytes( BYTE *dataBuffer, DWORD dataBufferSize, LPCTSTR sourceString )
// Converts an ASCII representation of binary data into binary data
{
    LPBYTE  currTargetByte          = NULL;
    LPCTSTR sourceStringNextByte    = NULL;
    LPCTSTR currSourceCharacter     = NULL;
    char    currByteStringRep[5]    = {""};

    // Validate parameters
    if ((dataBuffer == NULL) || (sourceString == NULL))
        return E_POINTER;
    // Base case - verify there is work
    if ((dataBufferSize == 0) && (*sourceString != NULL))
        return TYPE_E_BUFFERTOOSMALL;

    // Convert sourceString 1 pair of nybbles (2 characters represent 1 byte) at a time
    currTargetByte = dataBuffer;
    currSourceCharacter = sourceString;
    while ((currSourceCharacter[0] != NULL) && (currSourceCharacter[1] != NULL) && (currTargetByte < (dataBuffer+dataBufferSize)))
    {
        // Get source characters for this byte
        sourceStringNextByte = CharNext(currSourceCharacter);
        sourceStringNextByte = CharNext(sourceStringNextByte);
        memcpy(currByteStringRep, currSourceCharacter, sourceStringNextByte - currSourceCharacter);
        currByteStringRep[2] = 0;

        *currTargetByte = (BYTE) _tcstol(currByteStringRep, NULL, 16);

        // Advance to next byte
        currTargetByte += 1;
        currSourceCharacter = sourceStringNextByte;
    }

    if ((currTargetByte < (dataBuffer+dataBufferSize)) && ((sourceString[0] != NULL) && (sourceString[1] != NULL)))
        return S_OK;
    else
        return TYPE_E_BUFFERTOOSMALL;
}

HRESULT NetwareInstaller::CopyFileDo( LPCTSTR sourceFilename, LPCTSTR targetFilename, BOOL failIfExists )
{
    // Validate Parameter
    if (sourceFilename == NULL)
        return E_POINTER;
    if (targetFilename == NULL)
        return E_POINTER;

#ifdef NLM
    int             sourceFileHandle        = -1;
    int             targetFileHandle        = -1;
    DWORD           bytesCopied             = 0;
    struct stat     sourceFileInfo;
    struct stat     targetFileInfo;
    DOSDateAndTime  creationTime;
    DOSDateAndTime  lastModificationTime;
    DOSDateAndTime  lastAccessTime;
    DOSDateAndTime  lastArchiveTime;
    HRESULT         returnValHR             = E_FAIL;
    DWORD           returnValDW             = ERROR_OUT_OF_PAPER;
    
    if (failIfExists && (access(targetFilename, ACCESS_MODE_READ) == 0))
        return HRESULT_FROM_WIN32(ERROR_FILE_EXISTS);

    sourceFileHandle = open(sourceFilename, O_BINARY|O_RDONLY);
    if (sourceFileHandle != -1)
    {
        targetFileHandle = open(targetFilename, O_WRONLY|O_TRUNC|O_BINARY|O_CREAT, S_IWRITE|S_IREAD);
        if ((targetFileHandle == -1) && (errno == EACCES))
        {
            chmod(targetFilename, S_IWRITE);
            targetFileHandle = open(targetFilename, O_WRONLY|O_TRUNC|O_BINARY|O_CREAT, S_IWRITE|S_IREAD);
        }
        if (targetFileHandle != -1)
        {
            returnValDW = FileServerFileCopy(sourceFileHandle, targetFileHandle, 0, 0, INT_MAX, &bytesCopied);
            switch (returnValDW)
            {
            case ESUCCESS:
                returnValHR = S_OK;
                break;
            case ERR_NO_READ_PRIVILEGE:
            case ERR_NO_WRITE_PRIVILEGE_OR_READONLY:
                returnValHR = HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
                break;
            case ERR_INSUFFICIENT_SPACE:
                returnValHR = HRESULT_FROM_WIN32(ERROR_DISK_FULL);
                break;
            case ERR_NETWORK_DISK_IO:
                returnValHR = HRESULT_FROM_WIN32(ERROR_WRITE_FAULT);
                break;
            default:
                returnValHR = E_FAIL;
                break;
            }

            // Finally, copy the date and time
            if (returnValHR == S_OK)
            {
                returnValHR = E_FAIL;
                if (fstat(sourceFileHandle, &sourceFileInfo) == 0)
                {
                    close(targetFileHandle);
                    targetFileHandle = -1;

                    DOSDateTime creationTime;
                    DOSDateTime modificationTime;
                    DOSDateTime lastAccessTime;
                    DOSDateTime lastBackupTime;
                    
                    // Convert time_t -> DOS time
                    _ConvertTimeToDOS(sourceFileInfo.st_ctime, &creationTime.date, &creationTime.time);       // create date and time
                    _ConvertTimeToDOS(sourceFileInfo.st_atime, &lastAccessTime.date, &lastAccessTime.time);     // Last access date and time
                    _ConvertTimeToDOS(sourceFileInfo.st_btime, &lastBackupTime.date, &lastBackupTime.time);     // Last archive date and time
                    _ConvertTimeToDOS(sourceFileInfo.st_mtime, &modificationTime.date, &modificationTime.time);     // Last update/modify date and time
                    returnValDW = SetFileInfo(targetFilename, 6, sourceFileInfo.st_attr, (char*) &(creationTime), (char*) &(lastAccessTime.date), (char*) &(modificationTime), (char*) &(lastBackupTime), sourceFileInfo.st_uid);
                    
                    // Fixed at success because SetFileInfo appears to be returning failure when it succeeds (??)
                    returnValHR = S_OK;
                }
                else
                {
                    returnValHR = E_FAIL;
                }
            }
            if (targetFileHandle != -1)
            {
                close(targetFileHandle);
                targetFileHandle = -1;
            }
        }
        else
        {
            returnValHR = FileErrnoToHRESULT(errno);
        }
        close(sourceFileHandle);
        sourceFileHandle = -1;
    }
    else
    {
        returnValHR = FileErrnoToHRESULT(errno);
    }

    return returnValHR;
#else
    ::CopyFile(sourceFilename, targetFilename, failIfExists);
    return HRESULT_FROM_WIN32(GetLastError());
#endif
    
}

HRESULT NetwareInstaller::DeleteFileDo( LPCTSTR targetFilename, bool deleteIfReadonly )
{
    DWORD   returnValDW     = ERROR_OUT_OF_PAPER;

    // Validate Parameter
    if (targetFilename == NULL)
        return E_POINTER;

    returnValDW = unlink(targetFilename);
    // Did the deletion fail?
    if (returnValDW == -1)
    {
        // Yes.  Remove the RO attribute if requested and retry
        if (deleteIfReadonly)
        {
            if (chmod(targetFilename, S_IWRITE|S_IREAD) == 0)
                returnValDW = unlink(targetFilename);
        }
    }
    if (returnValDW == 0)
    {
        return S_OK;
    }
    else
    {
        if (errno == ENOENT)
            return S_FALSE;
        else
            return FileErrnoToHRESULT(errno);
    }
    return E_FAIL;
}

HRESULT NetwareInstaller::RemoveDirectoryDo( LPCTSTR targetDirectory )
{
    if (rmdir(targetDirectory) == 0)
        return S_OK;
    else
        return FileErrnoToHRESULT(errno);
}

HRESULT NetwareInstaller::GetFiles( tcstring findspec, StringList* fileList )
// Sets *fileList equal to the node names of the files meeting findspec.
{
    HANDLE              findHandle          = 0;
    WIN32_FIND_DATA     fileInfo;
    BOOL                foundAnotherFile    = TRUE;
    HRESULT             returnValHR         = E_FAIL;

    // Validate parameters
    if (fileList == NULL)
        return E_POINTER;

    findHandle = FindFirstFile(findspec.c_str(), &fileInfo);
    returnValHR = S_FALSE;
    while ((findHandle != INVALID_HANDLE_VALUE) && (foundAnotherFile))
    {
        returnValHR = S_OK;
        fileList->push_back(fileInfo.cFileName);
        foundAnotherFile = FindNextFile(findHandle, &fileInfo);
    }
    if (findHandle != INVALID_HANDLE_VALUE)
    {
        FindClose(findHandle);
        findHandle = NULL;
    }
    return returnValHR;
}

HRESULT NetwareInstaller::GetFiles( tcstring findspec, Win32FindDataArray* fileList )
// Sets *fileList equal to the node names of the files meeting findspec.
{
    HANDLE              findHandle          = 0;
    WIN32_FIND_DATA     fileInfo;
    BOOL                foundAnotherFile    = TRUE;
    HRESULT             returnValHR         = E_FAIL;

    // Validate parameters
    if (fileList == NULL)
        return E_POINTER;

    findHandle = FindFirstFile(findspec.c_str(), &fileInfo);
    returnValHR = S_FALSE;
    while ((findHandle != INVALID_HANDLE_VALUE) && (foundAnotherFile))
    {
        returnValHR = S_OK;
        fileList->push_back(fileInfo);
        foundAnotherFile = FindNextFile(findHandle, &fileInfo);
    }
    if (findHandle != INVALID_HANDLE_VALUE)
    {
        FindClose(findHandle);
        findHandle = NULL;
    }
    return returnValHR;
}

HRESULT NetwareInstaller::DirectoryExists( tcstring directoryPath )
// Returns S_OK if directory exists, HRFW32(ERROR_PATH_NOT_FOUND) if path not found, else the error code of the failure
{
    struct stat     fileInfo;
    HRESULT         returnValHR     = E_FAIL;
    
    if (stat(directoryPath.c_str(), &fileInfo) == 0)
    {
        if (fileInfo.st_mode & S_IFDIR)
            returnValHR = S_OK;
        else
            returnValHR = E_FAIL;
    }
    else
    {
        returnValHR = FileErrnoToHRESULT(errno);
        if (returnValHR == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
            returnValHR = HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
    }
    
    return returnValHR;
}

HRESULT NetwareInstaller::NukeDir( tcstring directoryName, tcstring exceptionFilename )
// Deletes all files in directoryName and all subdirectories and removes all subdirectories
// if exceptionFilename != "", excludes a file with this full pathname
{
    StringList              files;
    StringList::iterator    currFile;
    tcstring                pathname;
    HRESULT                 returnValHR     = E_FAIL;
    HRESULT                 returnValHRb    = E_FAIL;

    LogMessage(StatusCallbackFunctionEventType_Info, "Nukedir:  Called on %s.", directoryName.c_str());
    // Base case - make sure there is something to do
    if (DirectoryExists(directoryName) == HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND))
    {
        LogMessage(StatusCallbackFunctionEventType_Info, "Nukedir:  directory %s does not exist.", directoryName.c_str());
        return S_FALSE;
    }

    // Delete all files and recursively delete all subdirs    
    returnValHR = GetFiles(directoryName +tcstring("\\*.*"), &files);
    if (SUCCEEDED(returnValHR))
    {
        returnValHR = S_OK;
        for (currFile = files.begin(); currFile != files.end(); currFile++)
        {
            if ((_tcsicmp(currFile->c_str(), ".") != 0) && (_tcsicmp(currFile->c_str(), "..") != 0))
            {
                pathname = directoryName +tcstring("\\")+ *currFile;
                if (DirectoryExists(pathname) == S_OK)
                {
                    returnValHRb = NukeDir(pathname, exceptionFilename);
                }
                else
                {
                    if (_tcsicmp(pathname.c_str(), exceptionFilename.c_str()) != 0)
                        returnValHRb = DeleteFileDo(pathname.c_str(), true);
                    else
                        returnValHR = S_OK;
                }
                if (FAILED(returnValHRb) && (returnValHR == S_OK))
                {
                    LogMessage(StatusCallbackFunctionEventType_Info, "Nukedir:  Error 0x%08x deleting %s.", returnValHRb, pathname.c_str());
                    returnValHR = returnValHRb;
                }
            }
       }
    }
    // Delete this directory
    returnValHRb = RemoveDirectoryDo(directoryName.c_str());
    if (FAILED(returnValHRb) && (returnValHR == S_OK))
    {
        if (_tcsicmp(GetParentPath(exceptionFilename).c_str(), directoryName.c_str()) != 0)
        {
            LogMessage(StatusCallbackFunctionEventType_Info, "Nukedir:  Error 0x%08x removing directory %s.", returnValHRb, directoryName.c_str());
            returnValHR = returnValHRb;
        }
        else
        {
            LogMessage(StatusCallbackFunctionEventType_Info, "Nukedir:  Directory %s removed, leaving file %s.", directoryName.c_str(), exceptionFilename.c_str());
        }
    }
    
    return returnValHR;
}

bool NetwareInstaller::PrioritySortPredicate( IntStringPair elem1, IntStringPair elem2 )
// Used to sort a list of IntStringPairs in descending order by int
// Returns TRUE if elem1.first is M elem2.first
{
   return elem1.first < elem2.first;
}

HRESULT NetwareInstaller::GetCBAAddr( LPCTSTR serverName, SENDCOM_HINTS hints, CBA_Addr* address, DWORD* savErrorCode )
// Sets *address = the CBA address of serverName.
// Returns S_OK on success, E_FAIL on failure and sets *savErrorCode if supplied
{
    HKEY    keyHandle                       = NULL;
    TCHAR   serverAddressKey[MAX_PATH]      = {""};
    TCHAR   protocolToUse[100]              = {""};
    DWORD   returnValDW                     = 0;
    DWORD   valueSize                       = 0;
    
    sssnprintf(serverAddressKey, sizeof(serverAddressKey), REGHEADER"\\AddressCache\\%s", serverName);
    returnValDW = RegCreateKey(HKEY_LOCAL_MACHINE, serverAddressKey, &keyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        valueSize = sizeof(protocolToUse);
        returnValDW = SymSaferRegQueryValueEx(keyHandle, "Protocol", NULL, NULL, reinterpret_cast<unsigned char*>(protocolToUse), &valueSize);
        if (returnValDW == ERROR_SUCCESS)
        {       
            valueSize = sizeof(CBA_Addr);
            returnValDW = SymSaferRegQueryValueEx(keyHandle, protocolToUse, NULL, NULL, (BYTE*) address, &valueSize);
        }
        RegCloseKey( keyHandle );
    }

    if (returnValDW != ERROR_SUCCESS)
    {
        unsigned short  protocolCode    = CBA_PROTOCOL_IPX;
        CBA_NETADDR     rawAddress;

        ZeroMemory(&rawAddress,sizeof(rawAddress));
        if (ResolveAddress(serverName, &rawAddress, &protocolCode))
        {
            returnValDW = ERROR_SUCCESS;
            // This address may not work!
            NTSBuildAddr(&rawAddress, address, 0, (unsigned char) protocolCode);
            VerifyAnAddress(reinterpret_cast<char*>(address), hints);
        }
    }

    // Return result
    if (savErrorCode != NULL)
        *savErrorCode = returnValDW;
    if (returnValDW == ERROR_SUCCESS)
        return S_OK;
    else
        return E_FAIL;
}

HRESULT NetwareInstaller::fsopen( LPCTSTR filename, LPCTSTR mode, FILE** fileHandle )
// Like fopen, but always opens files shareably:  read => SH_DENYNO, write => SH_DENYWRITE.
// Returns S_OK on success and sets *fileHandle equal to the fopen-compatible handle,
// else translates errno into an HRESULT and returns the error code of the failure
{
    bool    requestRead         = false;
    bool    requestWrite        = false;
    bool    requestAppend       = false;
    bool    requestUpdate       = false;
    int     sopenFileHandle      = -1;
    FILE*   fopenFileHandle     = NULL;
    DWORD   openMode            = 0;
    DWORD   shareMode           = SH_DENYWR;
    HRESULT returnValHR         = E_FAIL;

    // Validate parameters
    if ((filename == NULL) || (mode == NULL) || (fileHandle == NULL))
        return E_POINTER;
    if (*fileHandle != NULL)
        return E_INVALIDARG;

    // Identify mode(s) requested
    if (_tcschr(mode, 'r') != NULL)
        requestRead = true;
    if (_tcschr(mode, 'w') != NULL)
        requestWrite = true;
    if (_tcschr(mode, 'a') != NULL)
        requestAppend = true;
    if (_tcschr(mode, '+') != NULL)
        requestUpdate = true;
    
    // Parameter validation part 2 - caller must request something
    if (!requestRead && !requestWrite && !requestAppend && !requestUpdate)
        return E_INVALIDARG;

    // Convert fopen mode to sopen mode
    // r, rb, rt
    if (requestRead && !requestWrite && !requestAppend && !requestUpdate)
    {
        openMode = O_RDONLY;
        shareMode = SH_DENYNO;
    }
    // w, wb, wt
    if (!requestRead && requestWrite && !requestAppend && !requestUpdate)
        openMode = O_WRONLY | O_TRUNC;
    // a, ab, at
    if (!requestRead && !requestWrite && requestAppend && !requestUpdate)
        openMode = O_WRONLY | O_APPEND;
    // r+, r+b, r+t, rb+, rt+
    if (requestRead && !requestWrite && !requestAppend && requestUpdate)
        openMode = O_RDWR;
    // w+, w+b, w+t, wb+, wt+
    if (!requestRead && requestWrite && !requestAppend && requestUpdate)
        openMode = O_RDWR | O_TRUNC;
    // a+, a+b, a+t, ab+, at+
    if (!requestRead && !requestWrite && requestAppend && requestUpdate)
        openMode = O_RDWR | O_APPEND;

    // Open the file in shareable mode.  This is a 2-stage process if the file does not exist
    // because sopen does not support the shareMode argument for newly created files.
    sopenFileHandle = sopen(filename, openMode, shareMode);
    if (sopenFileHandle == -1)
    {
        if (openMode != O_RDONLY)
        {
            sopenFileHandle = sopen(filename, O_CREAT | openMode, shareMode, S_IREAD | S_IWRITE);
            if (sopenFileHandle != -1)
            {
                close(sopenFileHandle);
                sopenFileHandle = -1;
                sopenFileHandle = sopen(filename, openMode, shareMode);
            }
        }
    }

    // Now fdopen the sopen file handle
    if (sopenFileHandle != -1)
    {
        fopenFileHandle = fdopen(sopenFileHandle, mode);
        if (fopenFileHandle != NULL)
        {
            *fileHandle = fopenFileHandle;
            returnValHR = S_OK;
        }
        else
        {
            close(sopenFileHandle);
            sopenFileHandle = -1;
        }
    }

    // Return result
    if (returnValHR != S_OK)
        returnValHR = FileErrnoToHRESULT(errno);
    return returnValHR;
}

HRESULT NetwareInstaller::FileErrnoToHRESULT( int erroNo )
// Translates an errno from a file function into an HRESULT
{
    HRESULT returnValHR = E_FAIL;
    
    if (erroNo == ENOENT)
        returnValHR = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    else if (erroNo == EACCES)
        returnValHR = HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
    else if (erroNo == EMFILE)
        returnValHR = HRESULT_FROM_WIN32(ERROR_TOO_MANY_OPEN_FILES);
    else if (erroNo == EINUSE)
        returnValHR = HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION);
    else if (erroNo == EBADF)
        returnValHR = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);
    else if (erroNo == EEXIST)
        returnValHR = HRESULT_FROM_WIN32(ERROR_FILE_EXISTS);
    else if (erroNo == EINVAL)
        returnValHR = HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS);
    else if (erroNo == ENOSPC)
        returnValHR = HRESULT_FROM_WIN32(ERROR_DISK_FULL);
    else if (errno == ENOTEMPTY)
        returnValHR = HRESULT_FROM_WIN32(ERROR_DIR_NOT_EMPTY);
    else
        returnValHR = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, erroNo);

    return returnValHR;
}

// ** STRING UTILITIES **
int NetwareInstaller::StringReplace( tcstring* subjectString, tcstring findString, tcstring replaceString )
//Searches for findString in subjectString and replaces all occurances with replaceString
{
    int returnVal = -1;
    tcstring resultString = "";

    //Validate parameters
    if (subjectString == NULL)
        return -1;

    //Do the search/replace
    returnVal = StringReplace(*subjectString, findString, replaceString, &resultString);
    if (returnVal > 0)
        *subjectString = resultString;
    return returnVal;
}

int NetwareInstaller::StringReplace( tcstring sourceString, tcstring findString, tcstring replaceString, tcstring* resultString )
//Identical to StringReplace, but takes C++ strings for args
{
    LPTSTR resultBuffer = NULL;
    DWORD resultBufferSize = 0;
    DWORD maxResultBufferSize = 32768;      //Max size we'll support
    int returnVal = -1;

    //Validate parameters
    if (resultString == NULL)
        return -1;

    //Only failure condition for StringReplace is insufficient buffer size
    //Call StringReplace.  If fails, repeat with larger buffer sizes, up to maxResultBufferSize
    for( resultBufferSize = 2048; (resultBufferSize <= maxResultBufferSize) && (returnVal == -1); resultBufferSize = resultBufferSize*2)
    {
        if (resultBuffer)
            delete resultBuffer;
        resultBuffer = new(std::nothrow) TCHAR[resultBufferSize];
        resultBuffer[0] = NULL;
        returnVal = StringReplace(resultBuffer, resultBufferSize, sourceString.c_str(), findString.c_str(), replaceString.c_str());
    }

    //Cleanup and return result
    if (resultBuffer)
    {
        *resultString = resultBuffer;
        delete resultBuffer;
    }
    return returnVal;
}

int NetwareInstaller::StringReplace( LPTSTR lpszResult, size_t cchResult, LPCTSTR lpszStringIn, LPCTSTR lpszFind, LPCTSTR lpszReplaceWith )
//Replace all occurances of lpszFind in lpszStringIn with lpszReplace
//Returns the number of replacements made, or -1 if buffer was too small
//Ripped from Phoenix (NSW2) install
{
   // Reset result
   int nResult = -1;
   if (lpszResult && cchResult && lpszStringIn && lpszFind && lpszReplaceWith)
   {
       LPTSTR lpszBuffer = (LPTSTR) new(std::nothrow) TCHAR[cchResult];
      LPCTSTR lpszCFS = lpszStringIn;
      LPCTSTR lpszLFS = lpszStringIn;
      LPCTSTR lpszCCS = lpszFind;
      LPTSTR lpszDest = lpszBuffer;
      int    cchSS = 0;

      // reset to 0
      nResult = 0;

      // Determine character count of text to find
      while (*lpszCCS)
      {
         cchSS++;
         lpszCCS = CharNext( lpszCCS );
      }

      if (lpszBuffer)
      {
         ZeroMemory(lpszBuffer, cchResult * sizeof(TCHAR));
         while (*lpszLFS)
         {
            // Start compares at last starting position
            lpszCFS = lpszLFS;

            // Reset find index ptr
            lpszCCS = lpszFind;

            // Compare the string we're looking for with the buffer
            if ((*lpszCFS == *lpszCCS) && 0 == _tcsnicmp(lpszCFS, lpszCCS, cchSS))
            {
               // Increment our return value
               nResult++;

               // Concatenate the new string
               lstrcpy( lpszDest, lpszReplaceWith );

               // Update the dest ptr
               while (*lpszDest)
                  lpszDest = CharNext( lpszDest );

               // Move search ptr to end of the old string in the buffer
               int i = 0;
               while (i < cchSS)
               {
                  lpszLFS = CharNext( lpszLFS );
                  i++;
               }
            }
            else
            {  // The substring was not found starting at lpszLFS,
               // so copy it to the output buffer, move it forward
               // one character and try again.
               *lpszDest = *lpszLFS;               // Copy character

               // check for possible DBCS character
               #if defined(WIN32) && !defined(_UNICODE)
                   if (IsDBCSLeadByte(*lpszLFS))
                      *(lpszDest+1) = *(lpszLFS+1);
               #else
                     if (NWCharType(*lpszLFS) == NWDOUBLE_BYTE)
                        *(lpszDest+1) = *(lpszLFS+1);
               #endif

               lpszDest = CharNext( lpszDest );    // Move dest forward
               lpszLFS = CharNext( lpszLFS );      // Move src forward
            }
         }
         // If the resulting string fits in the output buffer, save it,
         // otherwise, set the result to an error
         if ((size_t)lstrlen( lpszBuffer ) < cchResult)
            lstrcpy( lpszResult, lpszBuffer );
         else
         {
             TCHAR szMsg[255] = {""};
            sssnprintf(szMsg, sizeof(szMsg), "StringReplace character size %d, Actual Buffer Length %d", cchResult, lstrlen(lpszBuffer));
            nResult = -1;
         };

         // Free our working buffer
         delete lpszBuffer;
      }
   }

   return (nResult);
}

HRESULT NetwareInstaller::GetWords( tcstring sourceString, StringArray* words )
// Sets *words equal to the list of all words in sourceString, a whitespace delimited list of words
// Returns S_OK on success, S_FALSE if no words, else the error code of the failure
{
    tcstring::size_type currPos             = 0;
    LPCTSTR             wordStartPos        = NULL;
    LPCTSTR             wordEndPos          = NULL;
    LPCTSTR             stringStart         = NULL;
    HRESULT             returnValHR         = E_FAIL;
    
    // Validate parameters
    if (words == NULL)
        return E_FAIL;

    returnValHR = S_FALSE;
    words->clear();
    currPos = 0;
    while (currPos < sourceString.size())
    {
        if (FindFirstWord(sourceString.c_str() + currPos, &wordStartPos, &wordEndPos))
        {
            words->push_back(sourceString.substr(wordStartPos - sourceString.c_str(), (CharNext(wordEndPos) - wordStartPos)));
            returnValHR = S_OK;
            currPos = CharNext(wordEndPos) - sourceString.c_str();
        }
        else
        {
            // No more words
            break;
        }
    }
    
    return returnValHR;
}

LPCTSTR NetwareInstaller::FindFirstWhitespace( LPCTSTR searchString )
// Locates the first whitespace character in the passed string
// Returns pointer to character, or string if none found
{
    LPCTSTR     currChar                        = searchString;

    // Validate parameters
    if (searchString == NULL)
        return searchString;

    while((*currChar != NULL) && (*currChar != ' ') && (*currChar != '\t'))
        currChar = CharNext(currChar);

    // Return result
    if ((*currChar == ' ') || (*currChar == '\t'))
        return currChar;
    else
        return NULL;
}

LPCTSTR NetwareInstaller::FindFirstNonWhitespace( LPCTSTR searchString )
// Locates the first non whitespace character in the passed string
// Returns pointer to character, or string if none found
{
    LPCTSTR     currChar                        = searchString;

    // Validate parameters
    if (searchString == NULL)
        return searchString;

    while((*currChar != NULL) && ((*currChar == ' ') || (*currChar == '\t')))
        currChar = CharNext(currChar);

    // Return result
    if ((*currChar != ' ') && (*currChar != '\t') && (*currChar != NULL))
        return currChar;
    else
        return NULL;
}

LPCTSTR NetwareInstaller::FindLastNonWhitespace( LPCTSTR searchString )
// Locates the last non whitespace character in the passed string
// Returns pointer to character, or string if none found
{
    LPCTSTR     currChar                        = searchString;
    LPCTSTR     lastNonWhitespace               = NULL;

    // Validate parameters
    if (searchString == NULL)
        return searchString;

    while(*currChar != NULL)
    {
        if ((*currChar != ' ') && (*currChar != '\t') && (*currChar != '\r') && (*currChar != '\n'))
            lastNonWhitespace = currChar;
        currChar = CharNext(currChar);
    }

    // Return result
    return lastNonWhitespace;
}

bool NetwareInstaller::FindFirstWord( LPCTSTR searchString, LPCTSTR* wordStartPos, LPCTSTR* wordEndPos )
// Finds the first word in searchString.  A word is a sequence of non-whitespace characters separated by whitespace.
// Returns TRUE if a word was found and sets wordStartPos/wordEndPos pointers appropriately, else FALSE
{
    LPCTSTR     wordStartPosTemp                = NULL;
    LPCTSTR     wordEndPosTemp                  = NULL;

    // Validate parameters
    if ((searchString == NULL) || (wordStartPos == NULL) || (wordEndPos == NULL))
        return false;

    // Locate beginning of word
    wordStartPosTemp = FindFirstNonWhitespace(searchString);
    if (wordStartPosTemp == NULL)
        return false;

    // Find the end of the word
    wordEndPosTemp = FindFirstWhitespace(wordStartPosTemp);
    if (wordEndPosTemp != NULL)
    {
        // Word end is the character immediately before the first whitespace character encountered
        wordEndPosTemp = CharPrev(wordStartPosTemp, wordEndPosTemp);
    }
    else
    {
        // Word end is string end
        wordEndPosTemp = wordStartPosTemp + (_tcslen(wordStartPosTemp) - 1);
    }
    *wordStartPos = wordStartPosTemp;
    *wordEndPos = wordEndPosTemp;
    return true;
}

bool NetwareInstaller::FindNextWord( LPCTSTR searchString, LPCTSTR* wordStartPos, LPCTSTR* wordEndPos )
// Finds the NEXT word in searchString.  A word is a sequence of non-whitespace characters separated by whitespace.
// Skips past any non-whitespace characters at searchString until it finds one or more whitespace characters, then looks for a word
// Returns TRUE if a word was found and sets wordStartPos/wordEndPos pointers appropriately, else FALSE
{
    LPCTSTR     wordStartPosTemp                = NULL;

    // Validate parameters
    if ((searchString == NULL) || (wordStartPos == NULL) || (wordEndPos == NULL))
        return false;

    // Find next piece of whitespace
    wordStartPosTemp = FindFirstWhitespace(searchString);
    if (wordStartPosTemp != NULL)
        return FindFirstWord(wordStartPosTemp, wordStartPos, wordEndPos);

    return false;
}

void NetwareInstaller::TerminateAtNewline( LPTSTR lineBuffer )
// Checks the end of lineBuffer for a series of \r and \n characters - if found, converts them to NULLs
{
    LPTSTR      currChar    = lineBuffer + _tcslen(lineBuffer);
    bool        atStart     = false;

    while ((currChar >= lineBuffer) && !atStart)
    {
        if (currChar == lineBuffer)
            atStart = true;
        if ((*currChar == '\r') || (*currChar == '\n'))
            *currChar = NULL;
        else if (*currChar != NULL)
            break;
        currChar = CharPrev(lineBuffer, currChar);
    }
}

void NetwareInstaller::LogMessage( StatusCallbackFunctionEventType eventType, LPCTSTR format, ... )
// Makes an Info, Warning, or Error callback.
{
    TCHAR       logLine[1024] = {""};
    va_list     marker;

    va_start(marker, format);
    if ((statusCallback != NULL) && (eventType != StatusCallbackFunctionEventType_Progress))
    {
        ssvsnprintf(logLine, sizeof(logLine), format, marker);
        statusCallback(eventType, logLine, statusCallbackParam);
    }
    va_end(marker);
}

void NetwareInstaller::LogMessage( StatusCallbackFunctionEventType eventType, LPCTSTR format, va_list args )
// Makes an Info, Warning, or Error callback.
{
    TCHAR       logLine[1024] = {""};

    if ((statusCallback != NULL) && (eventType != StatusCallbackFunctionEventType_Progress))
    {
        ssvsnprintf(logLine, sizeof(logLine), format, args);
        statusCallback(eventType, logLine, statusCallbackParam);
    }
}

void NetwareInstaller::LogProgressMessage_Overall( DWORD overallProgressPercentage, LPCTSTR format, ... )
// Makes a Progress callback with the specified overall description (format) and percentage no (""/0) action information
{
    TCHAR       logLine[1024] = {""};
    va_list     marker;

    va_start(marker, format);

    if (statusCallback != NULL)
    {
        ssvsnprintf(logLine, sizeof(logLine), format, marker);
        currProgressStatus.overallDescription   = logLine;
        currProgressStatus.overallPercentage    = overallProgressPercentage;
        currProgressStatus.actionDescription    = "";
        currProgressStatus.actionPercentage     = 0;
        statusCallback(StatusCallbackFunctionEventType_Progress, (void*) &currProgressStatus, statusCallbackParam);
    }

    va_end(marker);
}

void NetwareInstaller::LogProgressMessage_Overall( LPCTSTR format, ... )
// Makes a Progress callback with the specified overall description (format), current overall percentage, and no (""/0) action information
{
    TCHAR       logLine[1024] = {""};
    va_list     marker;

    va_start(marker, format);

    if (statusCallback != NULL)
    {
        ssvsnprintf(logLine, sizeof(logLine), format, marker);
        currProgressStatus.overallDescription   = logLine;
        currProgressStatus.actionDescription    = "";
        currProgressStatus.actionPercentage     = 0;
        statusCallback(StatusCallbackFunctionEventType_Progress, (void*) &currProgressStatus, statusCallbackParam);
    }

    va_end(marker);
}

void NetwareInstaller::LogProgressMessage_Action( DWORD actionProgressPercentage, LPCTSTR format, ... )
// Makes a Progress callback with the current overall description/percentage and the specified action description and percentage
{
    TCHAR       logLine[1024] = {""};
    va_list     marker;

    va_start(marker, format);

    if (statusCallback != NULL)
    {
        ssvsnprintf(logLine, sizeof(logLine), format, marker);
        currProgressStatus.actionDescription = logLine;
        currProgressStatus.actionPercentage = actionProgressPercentage;
        statusCallback(StatusCallbackFunctionEventType_Progress, (void*) &currProgressStatus, statusCallbackParam);
    }

    va_end(marker);
}

void NetwareInstaller::TestHarness( int argc, TCHAR* argv[] )
{
    NetwareInstaller::StringList                lines;
    NetwareInstaller::StringList::iterator      currLine;
    NetwareInstaller::StringPairList            variables;
    NetwareInstaller::StringPairList::iterator  currVariable;
    DWORD                                       currLineNo          = 0;
    tcstring                                    sourceDirectory;
    LPCTSTR                                     filename            = "d:\\test.ini";
    LPCTSTR                                     sectionName         = "REG_REQ";
    HRESULT                                     returnValHR         = E_FAIL;


    {
        FILE* fileHandle = NULL;
        TCHAR currLine[MAX_PATH+1] = {""};
        filename = "SYS:\\dlt\\file.txt";
        fileHandle = fopen(filename, "rt");
        if (fileHandle != NULL)
        {
            fgets(currLine, sizeof(currLine)/sizeof(currLine[0]), fileHandle);
            TerminateAtNewline(currLine);
            ConsolePrintf("test read:  >%s<\n", currLine);
            fgets(currLine, sizeof(currLine)/sizeof(currLine[0]), fileHandle);
            TerminateAtNewline(currLine);
            ConsolePrintf("test read:  >%s<\n", currLine);
            fgets(currLine, sizeof(currLine)/sizeof(currLine[0]), fileHandle);
            TerminateAtNewline(currLine);
            ConsolePrintf("test read:  >%s<\n", currLine);

            fclose(fileHandle);
            fileHandle = NULL;
        }
        else
        {
            ConsolePrintf("DML:  TestHarness:  fopen of %s failed %d.\n", filename, errno);
        }

        filename = "\\dlt\\file.txt";
        fileHandle = fopen(filename, "rt");
        if (fileHandle != NULL)
        {
            fgets(currLine, sizeof(currLine)/sizeof(currLine[0]), fileHandle);
            TerminateAtNewline(currLine);
            ConsolePrintf("test read:  >%s<\n", currLine);
            fgets(currLine, sizeof(currLine)/sizeof(currLine[0]), fileHandle);
            TerminateAtNewline(currLine);
            ConsolePrintf("test read:  >%s<\n", currLine);
            fgets(currLine, sizeof(currLine)/sizeof(currLine[0]), fileHandle);
            TerminateAtNewline(currLine);
            ConsolePrintf("test read:  >%s<\n", currLine);

            fclose(fileHandle);
            fileHandle = NULL;
        }
        else
        {
            ConsolePrintf("DML:  TestHarness:  fopen of %s failed %d.\n", filename, errno);
        }
        
        filename = "SYS\\dlt\\file.txt";
        fileHandle = fopen(filename, "rt");
        if (fileHandle != NULL)
        {
            fgets(currLine, sizeof(currLine)/sizeof(currLine[0]), fileHandle);
            TerminateAtNewline(currLine);
            ConsolePrintf("test read:  >%s<\n", currLine);
            fgets(currLine, sizeof(currLine)/sizeof(currLine[0]), fileHandle);
            TerminateAtNewline(currLine);
            ConsolePrintf("test read:  >%s<\n", currLine);
            fgets(currLine, sizeof(currLine)/sizeof(currLine[0]), fileHandle);
            TerminateAtNewline(currLine);
            ConsolePrintf("test read:  >%s<\n", currLine);

            fclose(fileHandle);
            fileHandle = NULL;
        }
        else
        {
            ConsolePrintf("DML:  TestHarness:  fopen of %s failed %d.\n", filename, errno);
        }        

        filename = "SYS:dlt\\file.txt";
        fileHandle = fopen(filename, "rt");
        if (fileHandle != NULL)
        {
            fgets(currLine, sizeof(currLine)/sizeof(currLine[0]), fileHandle);
            TerminateAtNewline(currLine);
            ConsolePrintf("test read:  >%s<\n", currLine);
            fgets(currLine, sizeof(currLine)/sizeof(currLine[0]), fileHandle);
            TerminateAtNewline(currLine);
            ConsolePrintf("test read:  >%s<\n", currLine);
            fgets(currLine, sizeof(currLine)/sizeof(currLine[0]), fileHandle);
            TerminateAtNewline(currLine);
            ConsolePrintf("test read:  >%s<\n", currLine);

            fclose(fileHandle);
            fileHandle = NULL;
        }
        else
        {
            ConsolePrintf("DML:  TestHarness:  fopen of %s failed %d.\n", filename, errno);
        }        

        filename = "SYS:\\install.log";
        fileHandle = fopen(filename, "rt");
        if (fileHandle != NULL)
        {
            fgets(currLine, sizeof(currLine)/sizeof(currLine[0]), fileHandle);
            TerminateAtNewline(currLine);
            ConsolePrintf("test read:  >%s<\n", currLine);
            fgets(currLine, sizeof(currLine)/sizeof(currLine[0]), fileHandle);
            TerminateAtNewline(currLine);
            ConsolePrintf("test read:  >%s<\n", currLine);
            fgets(currLine, sizeof(currLine)/sizeof(currLine[0]), fileHandle);
            TerminateAtNewline(currLine);
            ConsolePrintf("test read:  >%s<\n", currLine);

            fclose(fileHandle);
            fileHandle = NULL;
        }
        else
        {
            ConsolePrintf("DML:  TestHarness:  fopen of %s failed %d.\n", filename, errno);
        }
    }
    return;

    // ProcessScript
    printf("** Test harness start.\n");
    try
    {
#ifdef NLM
        // On NetWare, ConsolePrintfs are atomic but can be skipped
        returnValHR = ProcessScript(InstallMode_Install, "SYS:\\nwinstl.ini", argv[0], DebugLogInstallMessage, (void*) "SYS:\\DluxLog.txt");
#else
        returnValHR = ProcessScript(InstallMode_Install, "d:\\nwinstl.ini", argv[0], DebugLogInstallMessage, (void*) stdout);
#endif
    }
    catch(...)
    {
        printf("*** EXCEPTION occured somehwere.\n");
    }
    return;

    statusCallback = DebugLogInstallMessage;
    statusCallbackParam = reinterpret_cast<void*>(stdout);

    returnValHR = GetIniSection("d:\\nwinstl.ini", "Install", &variables);
    if (SUCCEEDED(returnValHR))
    {
        for(currVariable = variables.begin(); currVariable != variables.end(); currVariable++)
        {
            printf("%02d:  >%s<=>%s<\n", currLineNo, currVariable->first.c_str(), currVariable->second.c_str());
            currLineNo += 1;
        }
    }
    return;

    // File tests
    returnValHR = ReadVariablesFromFile("D:\\vars.ini", "VARIABLES", &variables);
    if (SUCCEEDED(returnValHR))
    {
        filename = "d:\\test.ini";
        sectionName = "FileTest";
        variables.push_back(StringPair("TARGET_MAINPROGRAMDIRECTORY", "d:\\test\\mainprogramdir"));
        variables.push_back(StringPair("TARGET_SYSTEM", "d:\\test\\sys"));
        returnValHR = CopyFiles(filename, sectionName, variables, "d:\\test\\sourcedir", true, false);
    }
    return;

    // Registry tests
    returnValHR = ReadVariablesFromFile("D:\\vars.ini", "VARIABLES", &variables);
    if (SUCCEEDED(returnValHR))
    {
        printf("Variables:\n");
        currLineNo = 0;
        for (currVariable = variables.begin(); currVariable != variables.end(); currVariable++)
        {
            printf("   %02d:  >%s< = >%s<\n", currLineNo, currVariable->first.c_str(), currVariable->second.c_str());
            currLineNo += 1;
        }
        returnValHR = CopyRegistryEntries(filename, sectionName, variables, false, false);
        if (SUCCEEDED(returnValHR))
            printf("Registry processing complete.\n");
        else
            printf("Reg processing failed 0x%08x.\n", returnValHR);
    }
    return;

    // Base tests
    returnValHR = GetIniSection(filename, sectionName, &lines);
    if (SUCCEEDED(returnValHR))
    {
        for(currLine = lines.begin(); currLine != lines.end(); currLine++)
        {
            printf("%03d:  >%s<\n", currLineNo, currLine->c_str());
            currLineNo += 1;
        }
    }

    returnValHR = ReadVariablesFromFile("D:\\vars.ini", "VARIABLES", &variables);
    if (SUCCEEDED(returnValHR))
    {
        printf("Variables test.\n");
        currLineNo = 0;
        for (currVariable = variables.begin(); currVariable != variables.end(); currVariable++)
        {
            printf("%03d:  >%s< = >%s<\n", currLineNo, currVariable->first.c_str(), currVariable->second.c_str());
            currLineNo += 1;
        }
    }

    returnValHR = ReplaceVariables(&lines, variables);
    if (SUCCEEDED(returnValHR))
    {
        printf("Replacement test.\n");
        currLineNo = 0;
        for(currLine = lines.begin(); currLine != lines.end(); currLine++)
        {
            printf("%03d:  >%s<\n", currLineNo, currLine->c_str());
            currLineNo += 1;
        }
    }

}

void NetwareInstaller::DebugLogInstallMessage( StatusCallbackFunctionEventType eventType, void* eventParam, void* userParam )
// Logs the specified install message
{
    int                     logFileHandle   = NULL;
#ifdef NLM
    TCHAR                   logLine[1024]   = {""};
#endif
    TCHAR                   linePrefix[50]  = {""};
    time_t                  currentTime;
    tm                      currentTimeTM;
    StatusInfo_Progress*    progressInfo    = NULL;
    LPCTSTR                 statusLine      = NULL;

    if (userParam != NULL)
    {
        // Open the file in shareable mode
        logFileHandle = sopen((LPCTSTR) userParam, O_APPEND | O_RDWR, SH_DENYNO);
        if (logFileHandle == -1)
        {
            // Create the file, then close and re-open in shareable mode
            logFileHandle = sopen((LPCTSTR) userParam, O_CREAT | O_APPEND | O_RDWR, SH_DENYNO, S_IWRITE);
            if (logFileHandle != -1)
            {
                close(logFileHandle);
                logFileHandle = NULL;
                logFileHandle = sopen((LPCTSTR) userParam, O_APPEND | O_RDWR, SH_DENYNO);
            }
        }
        if (logFileHandle == -1)
            ConsolePrintf("Log file open failed.\n");
    }
    
    time(&currentTime);
    currentTimeTM = *localtime(&currentTime);
    sssnprintf(linePrefix, sizeof(linePrefix), "%d:%d:%d ", currentTimeTM.tm_hour, currentTimeTM.tm_min, currentTimeTM.tm_sec);
    switch (eventType)
    {
    case StatusCallbackFunctionEventType_Error:
        _tcscat(linePrefix, "ERROR");
        break;
    case StatusCallbackFunctionEventType_Warning:
        _tcscat(linePrefix, "WARNING");
        break;
    case StatusCallbackFunctionEventType_Progress:
        _tcscat(linePrefix, "Progress");
        break;
    case StatusCallbackFunctionEventType_Info:
        _tcscat(linePrefix, "Info");
        break;
    }
    if (eventType != StatusCallbackFunctionEventType_Progress)
    {
        statusLine = (LPCTSTR) eventParam;
        sssnprintf(logLine, sizeof(logLine), "%s:  %s\n", linePrefix, statusLine);
        if (logFileHandle != -1)
            write(logFileHandle, logLine, _tcslen(logLine));
        printf(logLine);
        
    }
    else
    {
        progressInfo = (StatusInfo_Progress*) eventParam;
        sssnprintf(logLine, sizeof(logLine), "%s O (%02d%%):  %s\n", linePrefix, progressInfo->overallPercentage, progressInfo->overallDescription.c_str());
        if (logFileHandle != -1)
            write(logFileHandle, logLine, _tcslen(logLine));
        printf("%s", logLine);
        if (progressInfo->actionDescription != "")
        {
            sssnprintf(logLine, sizeof(logLine), "%s A (%02d%%):  %s\n", linePrefix, progressInfo->actionPercentage, progressInfo->actionDescription.c_str());
            if (logFileHandle != -1)
                write(logFileHandle, logLine, _tcslen(logLine));
            printf("%s", logLine);
        }
    }
    
    if (logFileHandle != -1)
    {
        close(logFileHandle);
        logFileHandle = NULL;
    }
}

HRESULT NetwareInstaller::DumpVariables( StringPairList variables )
// Outputs all variables to the log callback
{
    StringPairList::iterator currVariable;

    LogMessage(StatusCallbackFunctionEventType_Info, "*** Variable Values ***");
    for (currVariable = variables.begin(); currVariable != variables.end(); currVariable++)
        LogMessage(StatusCallbackFunctionEventType_Info, "%s=%s", currVariable->first.c_str(), currVariable->second.c_str());

    return S_OK;
}

// ** Required for NTS, copied from RTVScan\nlmmain.cpp
DWORD StartNTSService(void)
{
#ifndef LINUX
    HANDLE han;
    char libpath[IMAX_PATH];

    sssnprintf(libpath,sizeof(libpath),NW_SYSTEM_DIR"\\%s","PDS.NLM");
    han = _LoadLibrary(libpath,true);

    if ( han == NULL )
        return P_NO_VIRUS_ENGINE;
#endif
    return 0;
}
