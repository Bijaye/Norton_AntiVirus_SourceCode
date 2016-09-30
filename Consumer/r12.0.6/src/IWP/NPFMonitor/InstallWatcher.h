#pragma once

#include "ccThread.h"

// structure for storing NIS version info
typedef struct tagVersion
{
	DWORD nMajorHi;
	DWORD nMajorLo;
	DWORD nMinorHi;
	DWORD nMinorLo;
	DWORD nPartialBuildLetter;
	DWORD nPartsUsed;
} NPFVERSION, *LP_NPFVERSION;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class			: CInstallWatcher
// Author			: Collin Davis
// Description		: This object keeps track of known legacy firewall clients as they come and
//					  go from the system.  When it detects a legacy client being installed it will
//					  begin the yielding process and complete it after the reboot.  Each time it
//					  is contructed (when the service starts), it will update the machine state
//					  and act on any changes detected.
// Base Classes		: ccLib::CThread - implements a thread for listening on registry changes
// Base Interfaces	: None.
// Dependancies		: CNPFMonitor - needed for making changes to the IWP yielding state
//					  CNAVOptHelper - navopt32 wrapper class used to store data in a DAT file
//
class CInstallWatcher : public ccLib::CThread
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor, Destructor, and Inititialization 
//
public:
	///////////////////////////////////////////////////////////////////////////////////////////////
	// Name			: CInstallWatcher()
	// Description	: Constructor for CInstallWatcher object.  Checks if anything has changed on 
	//				  the system since the last time we were loaded.  If a legacy firewall was
	//				  just installed then it will complete the yielding process.  If one was just
	//				  uninstalled then it will attempt to "unyield".
	// Parameters	: None
	// Return		: None
	// Notes		: None
	CInstallWatcher(void);

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Name			: ~CInstallWatcher()
	// Description	: Destructor cleans up the object on service shutdown. Also performs a quick
	//				  check to see if a legacy product was installed and if it was then it saves
	//				  the IntroWiz run key and then deletes it.  We keep the tasks performed here
	//				  to a minimum since it is called on service shutdown and we can't hang the machine.
	// Parameters	: None
	// Return		: None
	// Notes		: None
	virtual ~CInstallWatcher(void);
	
///////////////////////////////////////////////////////////////////////////////////////////////////
// CThread overrides
//
public:
	///////////////////////////////////////////////////////////////////////////////////////////////
	// Name			: ~PostTerminate()
	// Description	: Destructor cleans up the object on service shutdown. Also performs a quick
	//				  check to see if a legacy product was installed and if it was then it saves
	//				  the IntroWiz run key and then deletes it.  We keep the tasks performed here
	//				  to a minimum since it is called on service shutdown and we can't hang the machine.
	// Parameters	: None
	// Return		: None
	// Notes		: None
    virtual void PostTerminate() throw();

protected:
	///////////////////////////////////////////////////////////////////////////////////////////////
	// Name			: Run()
	// Description	: Implementation of the threadproc that listens for changes to the installedapps key
	// Parameters	: None
	// Return		: Returns NO_ERROR for success
	// Notes		: From ccLib:CThread: The thread work routine. Run() is called only if 
	//				  InitInstance() returned TRUE
    virtual int Run() throw();

///////////////////////////////////////////////////////////////////////////////////////////////////
// Internal Functions and Data
//
private:
	///////////////////////////////////////////////////////////////////////////////////////////////
	// Name			: InitRun()
	// Description	: Sets up for thread run, checks state and looks for state changes.
	// Parameters	: None.
	// Return		: Returns TRUE if everything looks OK, FALS on error.
	// Notes		: We need to do this stuff here and not the constructor so it will run in
	//				  a thread separate from the main service thread.  When we try to run it in
	//				  the main service thread it hangs the startup for the SCM
	BOOL InitRun();

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Name			: CheckRegistry()
	// Description	: Looks for the NIS installedapps key and updates the options file
	// Parameters	: lpszInstallPath - gets populated with the install dir of the currently installed client
	//				  dwSize - size of the lpszInstallPath buffer
	// Return		: Returns TRUE if it finds a FW key and FALSE otherwise
	// Notes		: See CNPFMonitor::DisableIWPForLegacyFirewall()
	BOOL CheckRegistry(LPTSTR lpszInstallPath = NULL, DWORD dwSize = 0);
	
	///////////////////////////////////////////////////////////////////////////////////////////////
	// Name			: StartYieldOnKeyDetected()
	// Description	: If we're not already yeilding, starts the process of doing so for legacy FWs:
	//				  step 1 -  set the IWP not to yielding
	//				  step 3 -  set the cc services mode to manual so we prevent the boottime race condition
	//				  step 4 -  reset the SymNETDrv driver settings
	//				  step 5 -  reset the IDS settings
	//				  step 6 -	prevent introwiz from running
	// Parameters	: None
	// Return		: Returns TRUE for success, FALSE for failure
	// Notes		: None.
	BOOL StartYieldOnKeyDetected();

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Name			: IDSUnsubscribe()
	// Description	: Unsubscribes legacy client from IDS when we detect it has been uninstalled
	// Parameters	: None
	// Return		: Returns TRUE for success, FALSE for failure
	// Notes		: None.
	BOOL IDSUnsubscribe();
	
	///////////////////////////////////////////////////////////////////////////////////////////////
	// Name			: ShutOffNAVFW()
	// Description	: Shuts off the NAV FW on every boot if we are yielding.
	// Parameters	: None
	// Return		: Returns TRUE for success, FALSE for failure
	// Notes		: None.
	BOOL ShutOffNAVFW();

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Name			: GetVersionFromString()
	// Description	: Internal utitily to oopulate an NPFVERSION struct from the string ready from NISVER.DAT
	//					(this code was taken from the Utilities.cpp in the InstallToolBox.
	// Parameters	: szVersion - string to convert, NPFVERSION - structure to populate
	// Return		: Returns TRUE for success
	// Notes		: None.
	BOOL GetVersionFromString(const LPCTSTR szVersion, /*out*/NPFVERSION &ver);

private:
	ATL::CEvent m_RegListenEvent;
	ATL::CEvent m_StopEvent;
	
	// info about installed legacy product
	bool m_bLegacyFWInstalled;
	NPFVERSION m_NisVer;
};
