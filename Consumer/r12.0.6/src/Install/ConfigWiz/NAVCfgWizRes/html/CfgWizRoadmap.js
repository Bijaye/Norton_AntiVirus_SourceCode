//
// Global variables
//
g_ModuleID = 3009;
g_ScriptID = 400;  // This uniquely defines this script file to the error handler

var MainFrame = window.parent;

// things to turn on
MainFrame.g_bEnableAP = true;
MainFrame.g_bScheduleScan = true;
MainFrame.g_bEnableEmailScanning = true;
MainFrame.g_bEnableEmailScanningIn = true;
MainFrame.g_bEnableEmailScanningOut = true;
MainFrame.g_bEnableDefinitionAlert = true;
MainFrame.g_bEnableALU = true;
MainFrame.g_bDisableICF = true;
MainFrame.g_bEnableSSC = true;
MainFrame.g_bEnableSP = true;

// whether or not to show checkboxes
MainFrame.g_bIWPInstalled = false;
MainFrame.g_bShowDisableICF = false;
MainFrame.g_bShowEnableSSC = false;

// post tasks
MainFrame.g_bRunLiveUpdate = true;
MainFrame.g_bFullSystemScan = true;

// panel info
MainFrame.g_lOEM = 0;
MainFrame.g_strProductName;
MainFrame.g_strCancelMsg;
MainFrame.g_strWizardName;
MainFrame.m_nMaxPanels = 5;
MainFrame.m_nFirstPanel = 0;
MainFrame.m_nCurrentPanel = 0;
MainFrame.g_nBackPanelIndex = 0;
MainFrame.g_bSelectFrameLoaded = false;
MainFrame.g_bRoadmapFrameLoaded;
MainFrame.g_WebWnd = null;
MainFrame.g_PageOb = null;
MainFrame.bPromptUser = true;
MainFrame.bPromptClose = true;
MainFrame.g_bShowPostInstallTasks = false;
MainFrame.g_bSilentMode = false;

// Page numbers
var nEULAPanel			= -1;
var nActivationPanel	= -1;
var nPostInstall		= -1;
var nConfigurationPanel = -1;
var nTryDieMessagePanel = -1;

// MessageBox() Flags & return codes
var MB_YESNO =                  0x00000004;
var MB_ICONQUESTION =           0x00000020;
var IDYES =             6;
var IDNO =              7;

// Licesing information
MainFrame.g_bLicenseValid = false;
MainFrame.g_iLicenseType = -1;
MainFrame.g_iLicenseZone = 64;		// violated
MainFrame.g_iLicenseState = -2;		// violated
MainFrame.g_iDaysRemaining = 0;

// Licensing state
var DJSMAR00_LicenseState_TRIAL		= 0;
var DJSMAR00_LicenseState_PURCHASED = 1;

// Licensing Types
var DJSMAR00_LicenseType_Trial		= 1;
var DJSMAR00_LicenseType_Rental		= 2;
var DJSMAR00_LicenseType_TryDie		= 3;
var DJSMAR00_LicenseType_Unlicensed	= 5;

// Licensing Zones
var DJSMAR00_LicenseZone_PreActivation = 8;

//Licensing Vendors
var DJSMAR00_VendorID_XtreamLok		= 1;

// Scriptable objects
var CfgWizMgrObj;
var ScanTasksObj;
var Scheduler;
var APSCRObj;
var NAVOptionObj;

var g_TimerID = 0;

function WizardPanel(TitleID, Arrow, sTitle, htm, pPreAction, pPostAction)
{
	this.m_TitleID = TitleID;
	this.m_sTitle = sTitle;
	this.m_Arrow = Arrow;
	this.m_htm = htm;
	this.m_pPreAction = pPreAction;
	this.m_pPostAction = pPostAction;
	this.m_bLoadPanel = true;
	this.m_bDisplayPanel = true;
}

function OnLoad()
{	
	// first we need to load all our ActiveX objects
	InitializeObjects();
	
	// now we need to initialize our licensing information
    InitializeLicensing();
    
    // now we need query the cfgwizmgr object for the correct behavior
    // this includes what options we should show based on which layout type
    // we are and what options and OEM vendor might have set.
	try
	{
		// get branding product name
		MainFrame.g_strProductName = CfgWizMgrObj.GetBrandingText(4);       
				
		// check if IWP is installed
		MainFrame.g_bIWPInstalled = CfgWizMgrObj.IWPInstalled;
		
		// if IWP is installed and ICF is enabled then we need to show the option to disable ICF
		if(!MainFrame.g_bSilentMode)
			MainFrame.g_bShowDisableICF = CfgWizMgrObj.ICFEnabled && CfgWizMgrObj.ShowICFCheckbox && MainFrame.g_bIWPInstalled;
		
		MainFrame.g_bDisableICF = CfgWizMgrObj.ICFEnabled && CfgWizMgrObj.DisableICF;
		
		// check to see if we should show SSC
		MainFrame.g_bShowEnableSSC = CfgWizMgrObj.ShowSSCOption;
		MainFrame.g_bEnableSSC = CfgWizMgrObj.SSCOption;
		
		// show post install tasks if something to be done
		MainFrame.g_bShowPostInstallTasks = MainFrame.g_bShowDisableICF || MainFrame.g_bShowEnableSSC;
	}
	catch(err)
	{
		CfgWizMgrObj.NAVError.LogAndDisplay(0);
	}
	
	// FLOW SETUP:
	// To setup the flow of panels we look at our licensing type, and our layout options 
	// to determine which panels to show and which ones to skip.
	
	// Start our array index at -1 and preincrement our index as we add panels.
	var nIndex = -1;
	
	// Allocate array of frames (more than we need)
	MainFrame.m_aWizardPanels = new Array(12);
	
	// If OEM or CTO show Eula
	if( MainFrame.g_lOEM )
	{
		++nIndex;
		nEULAPanel = nIndex;
		MainFrame.m_aWizardPanels[nEULAPanel] = new WizardPanel(RoadmapButton1Cat, RoadmapButton1Arrow, "EULA", "EULA.htm", null, null);
	}
	
	// check if we're try/die or not
	if ( DJSMAR00_LicenseType_TryDie != MainFrame.g_iLicenseType )
	{
	    // if we're not try/die:
	    // the new DRM activation panel is "smart". it will display activation when needed (ie: not
	    // purchased) and it will display web reg when needed (ie: not rental).  this means we can
	    // blindly always add it to the flow 
	    nActivationPanel = ++nIndex;
        MainFrame.m_aWizardPanels[nActivationPanel] = new WizardPanel(RoadmapButton2Cat, RoadmapButton2Arrow, "ACTIVATION", "ActivationPanel.htm", null, null);
	}
	else
	{
	    // we're try/die:
	    // there is never any activation or registration, instead we have special trydie page:
	    ++nIndex;
		nTryDieMessagePanel = nIndex;
		MainFrame.m_aWizardPanels[nTryDieMessagePanel] = new WizardPanel(RoadmapButton1Cat, RoadmapButton1Arrow, "TRYDIEMESSAGE", "TryDieMessage.htm", null, null);	    
	}
	
	// check if we need to show the postinstall tasks options page
	if (MainFrame.g_bShowPostInstallTasks)
	{
		// add the tasks page if we're supposed to
		nPostInstall = ++nIndex;
		MainFrame.m_aWizardPanels[nPostInstall] = new WizardPanel(RoadmapButton3Cat, RoadmapButton3Arrow, "POSTINSTALL", "PostInstall.htm", null, null);
	}
	
	// add the "finish" (aka. Configuration) page to the end.
	nConfigurationPanel = ++nIndex;
	MainFrame.m_aWizardPanels[nConfigurationPanel] = new WizardPanel(RoadmapButton4Cat, RoadmapButton4Arrow, "CONFIGURATION", "Configuration.htm", OnPreConfiguration, null);

	//Set the max panels to what we've counted up for the particular layout flow
	MainFrame.m_nMaxPanels = nIndex+1;


	// Create an array of panel location to keep track of what panels we've been to
	MainFrame.m_aBackTrackPanels = new Array( MainFrame.m_nMaxPanels );

	MainFrame.g_strWizardName = WizardName.innerText.replace(/%s/, MainFrame.g_strProductName);
	MainFrame.g_strCancelMsg = CancelWarning.innerText.replace(/%s/, MainFrame.g_strProductName);

	// Highlight the current panel
	
	MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_TitleID.style.backgroundImage = 'url(Current.gif)';
	MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_Arrow.style.display  = "";	

	if(IsFrameLoaded("main"))
	{
		ContinueOnload();		
	}
	else
	{
		g_TimerID = setInterval("ContinueOnload()", 500);
	}	
}
	
function ContinueOnload()
{
	if (IsFrameLoaded("main"))
	{
		if(g_TimerID != 0)
			clearInterval(g_TimerID);
	
		window.parent.frames("main").location.replace( MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_htm);
		MainFrame.m_aBackTrackPanels[MainFrame.g_nBackPanelIndex] = MainFrame.m_nCurrentPanel;
		
		// read which features to turn on/off.
		try
		{
			MainFrame.g_bSilentMode = CfgWizMgrObj.SilentMode;
			MainFrame.g_bEnableAP = CfgWizMgrObj.AutoProtect;
			MainFrame.g_bScheduleScan = CfgWizMgrObj.WeeklyScan;
			MainFrame.g_bScanAtStartup = CfgWizMgrObj.ScanAtStartup;
			MainFrame.g_bEnableEmailScanning = CfgWizMgrObj.EmailScanner;
			MainFrame.g_bEnableEmailScanningIn = CfgWizMgrObj.EmailScannerIn;
			MainFrame.g_bEnableEmailScanningOut = CfgWizMgrObj.EmailScannerOut;
			MainFrame.g_bEnableDefinitionAlert = CfgWizMgrObj.DefinitionAlert;
			MainFrame.g_bEnableALU = CfgWizMgrObj.AutoLiveUpdate;
			MainFrame.g_bRunLiveUpdate = CfgWizMgrObj.LiveUpdate;
			MainFrame.g_bFullSystemScan = CfgWizMgrObj.FullSystemScan;
			MainFrame.g_bLaunchUI = CfgWizMgrObj.MainUI;
			MainFrame.g_bEnableSP = CfgWizMgrObj.EnableSymProtect;
		}
		catch(err)
		{
			CfgWizMgrObj.NAVError.LogAndDisplay(0);
		}
		
		// Roadmap frame is now loaded;
		MainFrame.g_bRoadmapFrameLoaded = true;
		
		// Are we running in silent mode
		if (MainFrame.g_bSilentMode == true)
		{
			MainFrame.g_bRunLiveUpdate = false;
			MainFrame.g_bFullSystemScan = false;
			MainFrame.g_bLaunchUI = false;
			MainFrame.bPromptUser = false;
			
			try
			{
				// Set EULA=1
				CfgWizMgrObj.AgreedToEULA = true;
			}
			catch(err)
			{
				CfgWizMgrObj.NAVError.LogAndDisplay(0);
			}
			
			// Go through each wizard panel and execute the pre and post actions
			for (i = MainFrame.m_nCurrentPanel; i < MainFrame.m_nMaxPanels; i++)
			{
				if (MainFrame.m_aWizardPanels[i].m_bLoadPanel == true)
				{
					// Run the pre action
					if (MainFrame.m_aWizardPanels[i].m_pPreAction != null)
					{
						MainFrame.m_aWizardPanels[i].m_pPreAction();
					}

					// Run the post action
					if (MainFrame.m_aWizardPanels[i].m_pPostAction != null)
					{
						MainFrame.m_aWizardPanels[i].m_pPostAction();
					}
				}	
			}
			
			// Set the current panel to the last panel
			MainFrame.m_nCurrentPanel = MainFrame.m_nMaxPanels - 1
			
			// Close CfgWiz
			OnFinishButton();
		}
		else
		{
			// Run the PreAction for the current panel
			
			if (MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_pPreAction != null)
				MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_pPreAction();
		}
	}
}

function IsFrameLoaded(name)
{
	try
	{
		var oTemp = window.parent.frames(name);		
		return true;
	}
	catch(err)
	{
		return false;
	}
}

function OnUnload()
{	
	// Delete all the Wizard Panel objects 
	for (i = 0; i < MainFrame.m_nMaxPanels; i++)
	{
		if( MainFrame.m_aWizardPanels[i] != null )
			delete MainFrame.m_aWizardPanels[i];
	}
}

function OnAgreedToEULA()
{
	try
	{
		// If it's an OEM SKU or CTO SKU, reactivate virus def subscription
		if (MainFrame.g_lOEM == 1 || MainFrame.g_lOEM == 2)
		{
			CfgWizMgrObj.ReactivateSubscription();
		}
	}
	catch(err)
	{
		CfgWizMgrObj.NAVError.LogAndDisplay(0);
	}
}

function OnPreConfiguration()
{	
	if( MainFrame.g_bSelectFrameLoaded == true )
	{
		var SelectFrame = MainFrame.frames("bottom");

		try
		{
			SelectFrame.BackButton.disabled = true;
			SelectFrame.NextButton.disabled = true;
		}
		catch(err)
		{	
			CfgWizMgrObj.NAVError.LogAndDisplay(0);
		}
	}
	
	if (MainFrame.g_bSilentMode != true)
	{
		window.external.Global = MainFrame;
		try
		{
			WebWnd = new ActiveXObject ("ccWebWnd.ccWebWindow");
			WebWnd.FrameWindowStyle = 0;
			WebWnd.Width = 400;
			WebWnd.Height = 125;
			WebWnd.LaunchWindow('res://CfgWzRes.dll/PostInstallProgress.htm', MainFrame.g_strWizardName, false);
			window.external.Global = null;
		}
		catch(err)
		{
			var msg = document.frames("Errors").document.all.ERR_INTERNAL_ERROR.innerText;
			var id = document.frames("Errors").document.all.ERR_ID_NO_WEBWND.innerText;
			g_ErrorHandler.DisplayNAVError (msg, id);
			return;
		}
	}
	else
	{
		PostInstallWorker();
	}

	if( MainFrame.g_bSelectFrameLoaded == true )
    {
		var SelectFrame = MainFrame.frames("bottom");

	    try
	    {	
			    SelectFrame.BackButton.disabled = false;
			    SelectFrame.NextButton.disabled = false;
        }
        catch(err)
	    {	
		    CfgWizMgrObj.NAVError.LogAndDisplay(0);
	    }
    }
		
	try
	{
	    // Check to see if we need to enable IM Scanner
	    CfgWizMgrObj.EnableMSNScan();
	}
    catch(err)
	{	
		CfgWizMgrObj.NAVError.LogAndDisplay(0);
	}
	
	try
	{	
		// Check to see if we need to launch LU
		if (MainFrame.g_bRunLiveUpdate == true)
			CfgWizMgrObj.LaunchLiveUpdate();
    }
    catch(err)
	{	
		CfgWizMgrObj.NAVError.LogAndDisplay(0);
	}
	
	try
	{
		// Check to see if we need to scan system for viruses
		if (MainFrame.g_bFullSystemScan == true)
		{
			CfgWizMgrObj.LaunchSystemScan();
		}
	}
    catch(err)
	{	
		CfgWizMgrObj.NAVError.LogAndDisplay(0);
	}	
	
	try
	{
		// Launch main UI?
		if (MainFrame.g_bLaunchUI == true)
		{	
			CfgWizMgrObj.LaunchUI ();
		}
    }
    catch(err)
	{	
		CfgWizMgrObj.NAVError.LogAndDisplay(0);
	}
	
	try
	{
		// Repair Start Menu and desktop shortcuts
		CfgWizMgrObj.RepairShortcuts();
	}
	catch(err)
	{	
		CfgWizMgrObj.NAVError.LogAndDisplay(0);
	}	
	
	try
	{
	    // set the cfgwiz complete flag
		CfgWizMgrObj.CfgWizCompleted = true;
    }
    catch(err)
	{	
		CfgWizMgrObj.NAVError.LogAndDisplay(0);
	}
}

function OnFinishButton()
{
	EnableALU();
	
	CloseNotify();
		
	MainFrame.navigate("res://closeme.xyz");
}

function OnCloseButton()
{
    // Prompt users to see if they really wanna cancel
    if( MainFrame.bPromptUser == true )
    {
        if (MainFrame.bPromptClose == true) 
        {
            MainFrame.bPromptClose = false;

            iMsgBoxReturn = MainFrame.g_WebWnd.MsgBox(MainFrame.g_strCancelMsg, MainFrame.g_strProductName, MB_YESNO | MB_ICONQUESTION);

            if (iMsgBoxReturn == IDYES)
            {
                MainFrame.bPromptClose = true;  
                CloseNotify();                
                MainFrame.navigate("res://closeme.xyz");
                return true;
            }
            else if (iMsgBoxReturn == IDNO)
            {
                MainFrame.bPromptClose = true;
            }
                        
            return false;
        }
        else
            return false;
    }
    else
    {
        CloseNotify();                
        MainFrame.navigate("res://closeme.xyz");
    }
    
    return true;
}

function CloseNotify()
{
		try
		{
			if( MainFrame.g_PageOb != null )
			{
				MainFrame.g_PageOb.SetProperty( "ShutDown", "true" );
			}			
		}
		catch(err)
		{
		}
}

// we need to enable ALU after the FINISH button gets pressed.
function EnableALU()
{
    try
	{	
        // check if we need to turn on ALU and do so
		if (MainFrame.g_bEnableALU == true)
		{		
			NAVOptionObj.LiveUpdate = Math.abs(Number(true));
		}
		else
		{
			NAVOptionObj.LiveUpdate = Math.abs(Number(false));		
		}
	
		NAVOptionObj.Save();
	}
	catch(err)
	{
		NAVOptionObj.NAVError.LogAndDisplay(0);
	}
	
	return true;
}

function PostInstallWorker()
{	
	try
	{
		// Get updated license data (this could have changed since they activated)
		MainFrame.g_iLicenseZone = window.external.ObjectArg("DRM::LicenseZone");
		MainFrame.g_iLicenseState = window.external.ObjectArg("DRM::LicenseState");
		MainFrame.g_bLicenseValid = ((MainFrame.g_iLicenseState == DJSMAR00_LicenseState_TRIAL) || (MainFrame.g_iLicenseState == DJSMAR00_LicenseState_PURCHASED));
		
		// we need to set SymTDI to now load on demand.  it could be set to ignore requests to load
		// by an OEM flag
		CfgWizMgrObj.StartSymTDI();
		
		// spawn background tasks
	    try
	    {
		    CfgWizMgrObj.RunBackGroundTasks();
	    }
	    catch(err)
	    {
		    CfgWizMgrObj.NAVError.LogAndDisplay(0);
	    }
		
		// Check to see if we need to enable Email Scanning.
		if (MainFrame.g_bEnableEmailScanningIn == true)
		{
			NAVOptionObj.NAVEMAIL.ScanIncoming = Math.abs(Number(true));			
		}
		else
		{
			NAVOptionObj.NAVEMAIL.ScanIncoming = Math.abs(Number(false));			
		}

		if (MainFrame.g_bEnableEmailScanningOut == true)
		{
			NAVOptionObj.NAVEMAIL.ScanOutgoing = Math.abs(Number(true));
		}
		else
		{
			NAVOptionObj.NAVEMAIL.ScanOutgoing = Math.abs(Number(false));		
		}
	}
	catch(err)
	{
		NAVOptionObj.NAVError.LogAndDisplay(0);
	}


	//  Check if we need to enable AP at startup
	//  If license is not valid, we disable AP but we should not uncheck
	//  the option "Enable AP on start up" (same as set NAVOptionObj.STARTUP.LoadVxD to false.)
	try
	{
		if (MainFrame.g_bEnableAP == true)
		{
			NAVOptionObj.STARTUP.LoadVxD = Math.abs(Number(true));
		}
		else
		{
			NAVOptionObj.STARTUP.LoadVxD = Math.abs(Number(false));	
		}
	}
	catch(err)
	{
		NAVOptionObj.NAVError.LogAndDisplay(0);
	}

	try
	{	
		NAVOptionObj.AUTOPROTECT.DelayLoad = Math.abs(Number(CfgWizMgrObj.EnableDelayLoad));
		NAVOptionObj.Save();
	}
	catch(err)
	{
		NAVOptionObj.NAVError.LogAndDisplay(0);
	}

	try
	{
		// set the service to system start
		APSCRObj.SetSAVRTPELStartMode(1);
	}
	catch(err)
	{
		APSCRObj.NAVError.LogAndDisplay(0);
	}
		
	try
	{
		APSCRObj.Configure(MainFrame.g_bEnableAP);
		
		APSCRObj.Enabled = MainFrame.g_bEnableAP;
	}
	catch(err)
	{
		APSCRObj.NAVError.LogAndDisplay(0);
	}

	// Fix a scheduler bug related to Wise/Microsoft (Defect# 362816)
	try
	{
		Scheduler.Running = 0;
		Scheduler.Running = 1;
	}
	catch(err)
	{
		// Don't show an error display here. There's
		// nothing the user can do anyway and it's not
		// a critical error.
	}

	var SchedulerInstalled;
	try
	{
		SchedulerInstalled = Scheduler.Installed;
	}
	catch(err)
	{
		Scheduler.NAVError.LogAndDisplay(0);
	}

	if ( SchedulerInstalled == true)
	{
		if (MainFrame.g_bScheduleScan == true)
		{
			try
			{
				ScanTasksObj.ScheduleMyComputer();
			}
			catch(err)
			{
				ScanTasksObj.NAVError.LogAndDisplay(0);
			}
		}
		else
		{
			try
			{
				MyComputer = ScanTasksObj.MyComputer;
			}
			catch(err)
			{
				ScanTasksObj.NAVError.LogAndDisplay(0);
			}

			try
			{
				if (MyComputer.Scheduled == 1)
					MyComputer.DeleteSchedule();
			}
			catch(err)
			{
				MyComputer.NAVError.LogAndDisplay(0);
			}
		}	
	}
	
	// start symprotect
	try
	{
	    // if the the high bit is 0 in our 2 bit, bit mask then enable SP
	    if(0 == MainFrame.g_bEnableSP || 1 == MainFrame.g_bEnableSP)
		{
		    NAVOptionObj.SymProtectEnabled = true;
        }
        else
        {
            NAVOptionObj.SymProtectEnabled = false;
        }
        
        NAVOptionObj.Save();
	}
	catch(err)
	{
		NAVOptionObj.NAVError.LogAndDisplay(0);
	}
	
	// enable SSC
	try
	{
		if (MainFrame.g_bShowEnableSSC)
			CfgWizMgrObj.EnableSSC = MainFrame.g_bEnableSSC;
		else if (!CfgWizMgrObj.AlreadyShownSSCOption)
			CfgWizMgrObj.EnableSSC = CfgWizMgrObj.SSCOption;
	}
	catch(err)
	{
		CfgWizMgrObj.NAVError.LogAndDisplay(0);
	}
	
	// disable ICF
	try
	{
		if(!MainFrame.g_bSilentMode)
		{	
			if (MainFrame.g_bDisableICF)
				CfgWizMgrObj.DisableICF = true;
			else
				CfgWizMgrObj.DisableICF = false;
		}
	}
	catch(err)
	{
		CfgWizMgrObj.NAVError.LogAndDisplay(0);
	}
	
	// Activate Home Page Protection
	try
	{
		HPPFeatureEnabled = NAVOptionObj.HPP.FeatureEnabled;
		if(HPPFeatureEnabled)
        {		
            var axHPPOptionsChangedEvent = new ActiveXObject('HPPSCR.Util'); 
		    axHPPOptionsChangedEvent.ResetOptions();
		}
	}
	catch(err)
	{
		CfgWizMgrObj.NAVError.LogAndDisplay(0);
	}
}

function InitializeObjects()
{
	// Hook up to WebWnd
	try
	{
		MainFrame.g_WebWnd = window.external.ObjectArg("CFGWIZ_WEBWINDOW_OBJECT");
		MainFrame.g_WebWnd.OnCloseCallback = OnCloseButton;
	}
	catch(err)
	{
		var msg = document.frames("Errors").document.all.ERR_INTERNAL_ERROR.innerText;
		var id = document.frames("Errors").document.all.ERR_ID_NO_WEBWND.innerText;
		g_ErrorHandler.DisplayNAVError (msg, id);
		return;
	}

    // Load the Scriptable CfgWizMgr object
	try
	{
		CfgWizMgrObj = window.external.ObjectArg("NAV_CFGWIZ_MGR_OBJ");
	}
	catch(err)
	{
		var msg = document.frames("Errors").document.all.ERR_INTERNAL_ERROR.innerText;
		var id = document.frames("Errors").document.all.ERR_ID_NO_CFGWIZDLL.innerText;
		g_ErrorHandler.DisplayNAVError (msg, id);
		return;
	}
	
	// Load the scriptable NAV Options optject
	try
	{
		NAVOptionObj = new ActiveXObject ("Symantec.Norton.AntiVirus.NAVOptions");
	}
	catch(err)
	{
		var msg = document.frames("Errors").document.all.ERR_INTERNAL_ERROR.innerText;
		var id = document.frames("Errors").document.all.ERR_ID_NO_OPTIONS.innerText;
		g_ErrorHandler.DisplayNAVError (msg, id);
		return;
	}
	
	// Load the APScriptable object
    try
	{
		APSCRObj = new ActiveXObject ("NAVAPSCR.ScriptableAutoProtect");
	}
	catch(err)
	{
		var msg = document.frames("Errors").document.all.ERR_INTERNAL_ERROR.innerText;
		var id = document.frames("Errors").document.all.ERR_ID_NO_AP.innerText;
		g_ErrorHandler.DisplayNAVError (msg, id);
		return;
	}
	
	// Load the Scan Tasks object
	try
	{
		ScanTasksObj = new ActiveXObject ("Symantec.Norton.AntiVirus.ScanTasks");
	}
	catch(err)
	{
		var msg = document.frames("Errors").document.all.ERR_INTERNAL_ERROR.innerText;
		var id = document.frames("Errors").document.all.ERR_ID_NO_SCANTASKS.innerText;
		g_ErrorHandler.DisplayNAVError (msg, id);
		return;
	}
	
	// Load the Scheduler object
	try
	{
		Scheduler = new ActiveXObject ("NAVTasks.Scheduler");
	}
	catch(err)
	{
		var msg = document.frames("Errors").document.all.ERR_INTERNAL_ERROR.innerText;
		var id = document.frames("Errors").document.all.ERR_ID_NO_SCHEDULER.innerText;
		g_ErrorHandler.DisplayNAVError (msg, id);
		return;
	}
}

function InitializeLicensing()
{
    try
    {
	    MainFrame.g_iLicenseZone = window.external.ObjectArg("DRM::LicenseZone");
	    MainFrame.g_iLicenseType = window.external.ObjectArg("DRM::LicenseType");
	    MainFrame.g_iLicenseState = window.external.ObjectArg("DRM::LicenseState");
	    MainFrame.g_bLicenseValid = ((MainFrame.g_iLicenseState == DJSMAR00_LicenseState_TRIAL) || (MainFrame.g_iLicenseState == DJSMAR00_LicenseState_PURCHASED));

	    // Remaining days need to be updated for XLOK SCSS
	    if ( MainFrame.g_iLicenseType == DJSMAR00_LicenseType_Rental && 
			    window.external.ObjectArg("DRM::LicenseVendorId") == DJSMAR00_VendorID_XtreamLok )
	    {
		    MainFrame.g_iDaysRemaining = window.external.ObjectArg("DRM::SCSSTrialRemainingDays");
	    }
	    else
	    {
		    MainFrame.g_iDaysRemaining = window.external.ObjectArg("DRM::RemainingDays");
	    }

        // Get ProductType
        // 0 - Pure Retail
        // 1 - OEM
        // 2 - CTO (behaves like retail but need to show EULA)					
        MainFrame.g_lOEM = CfgWizMgrObj.ProductType;
    }
 	catch(err)
	{
		CfgWizMgrObj.NAVError.LogAndDisplay(0);
	}
}