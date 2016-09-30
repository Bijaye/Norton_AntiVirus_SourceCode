//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
////////////////////////

// TRACE disabled
var TRACE = false;

var axVkTrace = null;

if (TRACE)
{
	try
	{
		axVkTrace = new ActiveXObject("VkTools.VkTraceCC");
		axVkTrace.SetModuleName("CfgWizSelectJs");
	}
	catch (e)
	{
		TRACE = false;
	}
}

function trace(msg)
{
	if (TRACE)
	{
		try
		{
			axVkTrace.WriteLine(msg);
		}
		catch (e)
		{
			TRACE = false;
		}
	}
}

trace("Select.js is loading...");


g_ModuleID = 3009;
g_ScriptID = 403;  // This uniquely defines this script file to the error handler

var MainFrame = window.parent;

// Scriptable objects
var CfgWizMgrObj;
var APSCRObj;
var NAVOptionObj;

// things to turn on
MainFrame.g_bEnableDefinitionAlert = true;

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
MainFrame.g_WebWnd = null;
MainFrame.g_PageOb = null;
MainFrame.bPromptUser = true;
MainFrame.bPromptClose = true;
MainFrame.g_bSilentMode = false;

// Page numbers
var nEULAPanel			= -1;
var nActivationPanel	= -1;
var nTryDieMessagePanel = -1;


var nPanelSkippedTo = 0;
var ActSkipBtnText = ASkipBtnText.innerHTML;

var FinishBtnText = FinBtnText.innerHTML;
var NextBtnText = NxtBtnText.innerHTML;
var ActSkipHotKey = ASkipHotKey.innerHTML;

var FinishHotKey = FinHotKey.innerHTML;
var NextHotKey = NxtHotKey.innerHTML;
var	g_HelpTopicID = 0;
var	b_IsHighContrastMode = 0;

// MessageBox() Flags
var MB_OK =                     0x00000000;
var MB_OKCANCEL =               0x00000001;
var MB_ABORTRETRYIGNORE =       0x00000002;
var MB_YESNOCANCEL =            0x00000003;
var MB_YESNO =                  0x00000004;
var MB_RETRYCANCEL =            0x00000005;
var MB_ICONHAND =               0x00000010;
var MB_ICONQUESTION =           0x00000020;
var MB_ICONEXCLAMATION =        0x00000030;
var MB_ICONASTERISK  =          0x00000040;
var MB_USERICON =               0x00000080;
var MB_ICONWARNING =            MB_ICONEXCLAMATION;
var MB_ICONERROR =              MB_ICONHAND;
var MB_ICONINFORMATION =        MB_ICONASTERISK;
var MB_ICONSTOP =               MB_ICONHAND;

var IDOK =              1;
var IDCANCEL =          2;
var IDABORT =           3;
var IDRETRY =           4;
var IDIGNORE =          5;
var IDYES =             6;
var IDNO =              7;


// Licesing information
MainFrame.g_bLicenseValid = false;
MainFrame.g_iLicenseType = -1;
MainFrame.g_iLicenseAttribs = 0;		// violated
g_iDaysRemaining = 0;

//Licensing attributes
var cltLicenseAttrib_Trial = 0x00000008;
var cltLicenseAttrib_Active = 0x00000010;

//Licensing Vendors
var DJSMAR00_VendorID_XtreamLok		= 1;

// Licensing Types
var cltLicenseType_SOS = 0x00000040;   // SOS is Rental model (SCSS)
var cltLicenseType_TryDie = 0x00000020;   // Try Die no activation possible


// Set the panels
function WizardPanel(sTitle, htm)
{
	this.m_sTitle = sTitle;
	this.m_htm = htm;
	this.m_bLoadPanel = true;
	this.m_bDisplayPanel = true;
}

function DisableButtons()
{
	ButtonDisable(NextButton);
	NextButton.style.visibility = "visible";
	ButtonDisable(BackButton);
	BackButton.style.visibility = "visible";
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
}

function InitializeLicensing()
{
    try
    {
		MainFrame.g_iLicenseType = window.external.ObjectArg("clt::LicenseType");
		MainFrame.g_iLicenseAttribs = window.external.ObjectArg("clt::LicenseAttribs");
		g_iDaysRemaining = window.external.ObjectArg("clt::RemainingDays");
				
    	var bTrial = ((MainFrame.g_iLicenseAttribs & cltLicenseAttrib_Trial) == cltLicenseAttrib_Trial);
 		var bActive = ((MainFrame.g_iLicenseAttribs & cltLicenseAttrib_Active) == cltLicenseAttrib_Active);
 		MainFrame.g_bLicenseValid = (bTrial || bActive);
    	
	    // Remaining days need to be updated for XLOK SCSS
		if ( MainFrame.g_iLicenseType == cltLicenseType_SOS && 
			 window.external.ObjectArg("clt::LicenseVendorId") == DJSMAR00_VendorID_XtreamLok )
		{
			g_iDaysRemaining = window.external.ObjectArg("clt::SCSSTrialRemainingDays");
		}
	    else
	    {
		    g_iDaysRemaining = window.external.ObjectArg("clt::RemainingDays");
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

function OnLoad()
{
	trace("OnLoad()");

	DisableButtons();

	document.body.tabIndex=-1;

	// first we need to load all our ActiveX objects
	InitializeObjects();

	// Set buttons to high contrast mode if needed
	SetButtonHighContrast(BackButton);
	SetButtonHighContrast(NextButton);
	SetButtonHighContrast(SkipButton);
	
	// Load SymLogo
	try
	{              
		buttonDiv.style.backgroundImage = "url(\'res://" + CfgWizMgrObj.NortonAntiVirusPath + "\\isres.dll/CfgWizSymLogo.png\')";
	}
	catch(err)
	{
		CfgWizMgrObj.NAVError.LogAndDisplay(0);                         
	}                       
	// now we need to initialize our licensing information
    InitializeLicensing();

    // now we need query the cfgwizmgr object for the correct behavior
    // this includes what options we should show based on which layout type
    // we are and what options and OEM vendor might have set.
	try
	{
		// get branding product name
		MainFrame.g_strProductName = CfgWizMgrObj.GetBrandingText(4);
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
	MainFrame.m_aWizardPanels = new Array(8);

	// check if we're try/die or not
	if ( cltLicenseType_TryDie != MainFrame.g_iLicenseType )
	{
	    // if we're not try/die:
	    // the new DRM activation panel is "smart". it will display activation when needed (ie: not
	    // purchased) and it will display web reg when needed (ie: not rental).  this means we can
	    // blindly always add it to the flow 
	    nActivationPanel = ++nIndex;
        MainFrame.m_aWizardPanels[nActivationPanel] = new WizardPanel("ACTIVATION", "ActivationPanel.htm");
	}
	else
	{
	    // we're try/die:
	    // there is never any activation or registration, instead we have special trydie page:
	    ++nIndex;
		nTryDieMessagePanel = nIndex;
		MainFrame.m_aWizardPanels[nTryDieMessagePanel] = new WizardPanel("TRYDIEMESSAGE", "TryDieMessage.htm");	    
	}

	//Set the max panels to what we've counted up for the particular layout flow
	MainFrame.m_nMaxPanels = nIndex+1;

	// Create an array of panel location to keep track of what panels we've been to
	MainFrame.m_aBackTrackPanels = new Array( MainFrame.m_nMaxPanels );

	MainFrame.g_strWizardName = WizardName.innerText.replace(/%s/, MainFrame.g_strProductName);
	MainFrame.g_strCancelMsg = CancelWarning.innerText.replace(/%s/, MainFrame.g_strProductName);

	// read which features to turn on/off.
	try
	{
		MainFrame.g_bSilentMode = CfgWizMgrObj.SilentMode;
		MainFrame.g_bEnableDefinitionAlert = CfgWizMgrObj.DefinitionAlert;
		MainFrame.g_bRunLiveUpdate = CfgWizMgrObj.LiveUpdate;
		MainFrame.g_bFullSystemScan = CfgWizMgrObj.FullSystemScan;
		MainFrame.g_bLaunchUI = CfgWizMgrObj.MainUI;
	}
	catch(err)
	{
		CfgWizMgrObj.NAVError.LogAndDisplay(0);
	}
	
	// Highlight the current panel
	// Do not navigate to a particular panel in silent mode.  This prevents the OnLoad()
	// handler for the page from executing and performing UI functions when they
	// are not necessary.	
	if (MainFrame.g_bSilentMode == false)
	{
		window.parent.frames("main").location.replace( MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_htm);
		MainFrame.m_aBackTrackPanels[MainFrame.g_nBackPanelIndex] = MainFrame.m_nCurrentPanel;
	}

	// Are we running in silent mode
	if (MainFrame.g_bSilentMode == true)
	{
		MainFrame.g_bRunLiveUpdate = false;
		MainFrame.g_bFullSystemScan = false;
		MainFrame.g_bLaunchUI = false;
		MainFrame.bPromptUser = false;

		// Set the current panel to the last panel
		MainFrame.m_nCurrentPanel = MainFrame.m_nMaxPanels - 1;
		
		// Close CfgWiz
		OnFinishButton();
	}

	g_bSelectFrameLoaded = true;

	HandleControlStates(MainFrame.m_nCurrentPanel);
}

function HandleControlStates( nPanel )
{	
	if (nPanel == 0)
		ButtonDisable(BackButton);

	switch (MainFrame.m_aWizardPanels[nPanel].m_sTitle)
	{
		case "ACTIVATION":
			var bActive = ((MainFrame.g_iLicenseAttribs & cltLicenseAttrib_Active) == cltLicenseAttrib_Active);
			if ( !bActive )
			{
			    SkipButton.style.visibility = "visible";
				ButtonEnable(SkipButton);
			    SkipButton.innerHTML = ActSkipBtnText;
			    SkipButton.accessKey = ActSkipHotKey;
			 }
		break;

		default:
			SkipButton.style.visibility = "hidden";
			//txtHelpLink.style.visibility = "hidden";
		break;
	}
}

function InsertBackTrackPanel( nPanel )
{
	if( MainFrame.g_nBackPanelIndex < MainFrame.m_nMaxPanels )
	{
		if ( nPanel == MainFrame.m_aBackTrackPanels[MainFrame.g_nBackPanelIndex] )
			return;
			
		if ( null == MainFrame.m_aBackTrackPanels[++MainFrame.g_nBackPanelIndex] )
		{
			MainFrame.m_aBackTrackPanels[MainFrame.g_nBackPanelIndex] = nPanel;
			return;
		}
	}
	return;	
}

function GetBackTrackPanel()
{
	var nPanel = 0;
	
	if ( MainFrame.g_nBackPanelIndex > 0 )
	{
		MainFrame.m_aBackTrackPanels[MainFrame.g_nBackPanelIndex--] = null;
		nPanel = MainFrame.m_aBackTrackPanels[MainFrame.g_nBackPanelIndex];
	}
	
	return nPanel;
}

function OnSkipButton()
{
	trace("OnSkipButton()  CurrentPanel: " + MainFrame.m_nCurrentPanel);
		
	try
	{
		// If skip is renamed to finish and someone clicks it bail out.
		if( SkipButton.innerHTML == FinishBtnText )
			OnFinishButton();

		// Get the current panel
		var nCurPanel = MainFrame.m_nCurrentPanel;
			
		// Get our next panel to go to	
		var nNextPanel = MainFrame.m_nCurrentPanel+1;
		
		// If PageOb (activex that supports ICCIWNAvigation) is defined
		// let him get first crack at the skip button otherwise we are in control
		if(MainFrame.g_PageOb != null)
		{
			// PageOb has control - returns true
			if (MainFrame.g_PageOb.Skip())
			{
				return;
			}
			// PageOb gave up control - returns false
			else
			{
				// We have control so set the PageObject to null
				MainFrame.g_PageOb = null;
				
				//There are no more pages after Activation. 
				OnFinishButton();
				return;
			}
		}

		// Insert the panel in the backtrack array
		InsertBackTrackPanel( nNextPanel );

		// load the new page
		LoadPage( nNextPanel );
	}
	catch(err)
	{
		var msg = document.frames("Errors").document.all.ERR_INTERNAL_ERROR_REINSTALL.innerText;
		var id = document.frames("Errors").document.all.ERR_ID_NAVIGATION.innerText;
		g_ErrorHandler.DisplayNAVError (msg, id);
		return;
	}
	
}

function OnNextButton()
{
	trace("OnNextButton()  CurrentPanel: " + MainFrame.m_nCurrentPanel);

	try
	{	
		// If skip is renamed to finish and summon clicks it bail out.
		if( NextButton.innerHTML == FinishBtnText )
			OnFinishButton();
		
		// Get the next panel
		var nNextPanel = MainFrame.m_nCurrentPanel + 1;

		// If PageOb (activex that supports ICCIWNAvigation) is defined
		// let him get first crack at the next button otherwise we are in control
		if( MainFrame.g_PageOb != null )
		{
			trace("ICCIWNAvigation");

			// PageOb has control - returns true
			if (MainFrame.g_PageOb.Next())
			{
				return;
			}
			// PageOb gave up control - returns false
			else
			{
				// We have control so set the PageObject to null
				MainFrame.g_PageOb = null;
				
				//There are no more pages after Activation. 
				OnFinishButton();
				return;
			}
		}
		
		trace("Title: " + MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_sTitle);
		
		// See what panel we are on and see if we need to do custom actions 
		// for clicking next on the the current panel
		switch( MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_sTitle )
		{
			// Custom actions for the Activation selection panels
			case "SELECTACTIVATION":
			{
			}
			break;
			
			default:
			break;
		}
		
		for (; nNextPanel < MainFrame.m_nMaxPanels; nNextPanel++)
		{	
			if (MainFrame.m_aWizardPanels[nNextPanel].m_bLoadPanel == false || MainFrame.m_aWizardPanels[nNextPanel].m_bDisplayPanel == false)
			{
				trace("Skip panel: " + 	nNextPanel);
				
				continue;
			}
			break;
		}

		// Put our next panel in the backtrack list
		InsertBackTrackPanel( nNextPanel );
		// Load our next page
		LoadPage(nNextPanel);
	}
	catch(err)
	{
		var msg = document.frames("Errors").document.all.ERR_INTERNAL_ERROR_REINSTALL.innerText;
		var id = document.frames("Errors").document.all.ERR_ID_NAVIGATION.innerText;
		g_ErrorHandler.DisplayNAVError (msg, id);
		return;
	}
}

function OnBackButton()
{
	trace("OnBackButton()  CurrentPanel: " + MainFrame.m_nCurrentPanel);
	
	try
	{
		// If PageOb (activex that supports ICCIWNAvigation) is defined
		// let him get first crack at the next button otherwise we are in control
		if( MainFrame.g_PageOb != null )
		{
			if( MainFrame.g_PageOb.Back() )
			{
				// PageOb has control - returns true
				return;
			}
			else
			{
				// PageOb has control - returns false
				MainFrame.g_PageOb = null;
			}
		}
		
		// Get the previous panel from the backtrack array
		var nPrevPanel = GetBackTrackPanel();

		LoadPage(nPrevPanel);
					
		for (; nPrevPanel >= 0; nPrevPanel--)
		{
			if (MainFrame.m_aWizardPanels[nPrevPanel].m_bLoadPage == false || MainFrame.m_aWizardPanels[nPrevPanel].m_bDisplayPage == false)
			{
				continue;
			}
			break;
		}
	}
	catch(err)
	{
		var msg = document.frames("Errors").document.all.ERR_INTERNAL_ERROR_REINSTALL.innerText;
		var id = document.frames("Errors").document.all.ERR_ID_NAVIGATION.innerText;
		g_ErrorHandler.DisplayNAVError (msg, id);
		return;
	}
}

function OnFinishButton()
{
	var bEulaAccepted = 1;

	if ((MainFrame.g_lOEM != 0) && (!CfgWizMgrObj.IsAggregateProduct))
	{
		bEulaAccepted = 0;
        bEulaAccepted = window.external.ObjectArg("PI::EulaAccepted");
	}
	
	//Do PreConfiguration tasks only if EULA is accepted.
	if (bEulaAccepted == 1)
	{
		// Call on PreConfiguration to do configuration work.
		OnPreConfiguration();
	}

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

function LoadPage(nPanel)
{
	trace("LoadPage(" + nPanel + ")");

	if (nPanel > MainFrame.m_nMaxPanels - 1  || nPanel < -1)
	{
		return;
	}
		
	// Load the page into the "main" frame
	window.parent.frames("main").location.replace(MainFrame.m_aWizardPanels[nPanel].m_htm);
	
	HandleControlStates( nPanel );
	
	// Update m_nCurrentPanel ID
	MainFrame.m_nCurrentPanel = nPanel;	

	// Disable the Back button 
	if (MainFrame.m_nCurrentPanel <= MainFrame.m_nFirstPanel && MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_bLoadPanel)
	{
		ButtonDisable(BackButton);
	}
	else
	{
		ButtonEnable(BackButton);
	}
	
	// Determine the Next and Finish button state
	if (MainFrame.m_nCurrentPanel == MainFrame.m_nMaxPanels - 1)
	{
		// We are at the last panel.
		
		// Change Next to Finish.
		NextButton.style.visibility = "visible";
		ButtonEnable(NextButton);
		NextButton.accessKey = FinishHotKey;
		NextButton.innerHTML = FinishBtnText;
		NextButton.focus();
		
		// disable the back button
		ButtonDisable(BackButton);
		
		// Hide skip button.
		SkipButton.style.visibility = "hidden";
	}
	else
	{
		// We are not at the last panel
		// Set button to Next.
		NextButton.style.visibility = "visible";
		ButtonEnable(NextButton);
		NextButton.accessKey = NextHotKey;
		NextButton.innerHTML = NextBtnText;
		NextButton.focus();	
	}
}

function OnPreConfiguration()
{	
	if( MainFrame.g_bSelectFrameLoaded == true )
	{
		try
		{
			ButtonDisable(BackButton);
			ButtonDisable(NextButton);
		}
		catch(err)
		{	
			CfgWizMgrObj.NAVError.LogAndDisplay(0);
		}
	}

	PostInstallWorker();
	
	if( MainFrame.g_bSelectFrameLoaded == true )
    {
	    try
	    {	
			    ButtonEnable(BackButton);
				ButtonEnable(NextButton);
        }
        catch(err)
	    {	
		    CfgWizMgrObj.NAVError.LogAndDisplay(0);
	    }
    }

	try
	{
	    // Check to see if we need to enable IM Scanner
	    CfgWizMgrObj.EnableIMScan();
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
	    // set the cfgwiz complete flag
		CfgWizMgrObj.CfgWizCompleted = true;
    }
    catch(err)
	{	
		CfgWizMgrObj.NAVError.LogAndDisplay(0);
	}
}

function PostInstallWorker()
{	
	try
	{
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

	}
	catch(err)
	{
		NAVOptionObj.NAVError.LogAndDisplay(0);
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

function OnHelp( topicID )
{
	try
	{
		window.external.ObjectArg("CFGWIZ_BASE_METHODS").Help("SymHelp.chm", topicID);
	}
	catch(err)
	{
	}
}