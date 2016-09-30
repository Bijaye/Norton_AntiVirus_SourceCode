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

var TimerID  = 0;
var MainFrame = window.parent;
//var Register = true;
//var Activate = true;
var nPanelSkippedTo = 0;
var ActSkipBtnText = window.parent.frames("bottom").ASkipBtnText.innerHTML;
//var RegSkipBtnText = window.parent.frames("bottom").RSkipBtnText.innerHTML;
var FinishBtnText = window.parent.frames("bottom").FinBtnText.innerHTML;
var NextBtnText = window.parent.frames("bottom").NxtBtnText.innerHTML;
var ActSkipHotKey = window.parent.frames("bottom").ASkipHotKey.innerHTML;
//var RegSkipHotKey = window.parent.frames("bottom").RSkipHotKey.innerHTML;
var FinishHotKey = window.parent.frames("bottom").FinHotKey.innerHTML;
var NextHotKey = window.parent.frames("bottom").NxtHotKey.innerHTML;
var	g_HelpTopicID = 0;
//var SendRegData = "false";

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


var DJSMAR_LicenseType_Violated     = -1;
var DJSMAR_LicenseType_Retail       = 0;
var DJSMAR_LicenseType_Trial        = 1;
var DJSMAR_LicenseType_Rental       = 2;
var DJSMAR_LicenseType_TryDie		= 3;
var DJSMAR_LicenseType_Beta			= 4;
var DJSMAR_LicenseType_Unlicensed	= 5;
var DJSMAR_LicenseType_ESD			= 6;

var DJSMAR00_LicenseState_TRIAL		= 0;
var DJSMAR00_LicenseState_PURCHASED = 1;
	
var DJSMAR00_QueryMode_Local						= (0);
var DJSMAR00_QueryMode_RemoteSilent					= (1);
var DJSMAR00_QueryMode_RemoteVerbose				= (2);
var DJSMAR00_QueryMode_RemoteSilentBackground		= (3);
var DJSMAR00_QueryMode_RemoteVerboseInstall			= (4);
var DJSMAR00_QueryMode_LocalVerbose					= (5);
var DJSMAR00_QueryMode_LocalVerboseOnlyWhenExpired	= (6);
var DJSMAR00_QueryMode_ForceOverride				= (7);

function DisableButtons()
{
	NextButton.disabled = true;
	NextButton.style.visibility = "visible";
	BackButton.disabled = true;
	BackButton.style.visibility = "visible";
}

function OnLoad()
{
	trace("OnLoad()");
	
	DisableButtons();
	document.body.tabIndex=-1;
	if (MainFrame.g_bRoadmapFrameLoaded == true)
	{
		UpdatePage();		
	}
	else
	{
		TimerID = setInterval("UpdatePage()", 500);
	}
}

function UpdatePage()
{
	trace("UpdatePage()");

	if (MainFrame.g_bRoadmapFrameLoaded == true)
	{
		clearInterval(TimerID);
		
		HandleControlStates(MainFrame.m_nCurrentPanel);
		MainFrame.g_bSelectFrameLoaded = true;
    }
}

function HandleControlStates( nPanel )
{	
	if (nPanel == 0)
		BackButton.disabled = true;

	switch (MainFrame.m_aWizardPanels[nPanel].m_sTitle)
	{
		case "ACTIVATION":
			if ( window.external.ObjectArg("DRM::LicenseState") != DJSMAR00_LicenseState_PURCHASED)
			{
			    SkipButton.style.visibility = "visible";
				SkipButton.disabled = false;
			    SkipButton.innerHTML = ActSkipBtnText;
			    SkipButton.accessKey = ActSkipHotKey;
			 }
		break;
		
		//case "REGISTRATION":
		//	txtHelpLink.style.display = "none";
		//	
		//	SkipButton.style.visibility = "visible";
		//	SkipButton.disabled = false;
		//	SkipButton.innerHTML = RegSkipBtnText;
		//	SkipButton.accessKey = RegSkipHotKey;
		//	
		//	NextButton.disabled = false;
			
		//break;
		
		case "CONFIGURATION":
			// We are at the last panel.
			
			// Change Next to Finish.
			NextButton.style.visibility = "visible";
			NextButton.disabled = false;
			NextButton.accessKey = FinishHotKey;
			NextButton.innerHTML = FinishBtnText;
			
			// disable the back button
			BackButton.style.visibility = "visible";
			BackButton.disabled	= true;
			
			// Hide skip button.
			SkipButton.style.visibility = "hidden";
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
		//alert( "nPanel: " + nPanel);
		
		if ( nPanel == MainFrame.m_aBackTrackPanels[MainFrame.g_nBackPanelIndex] )
			return;
			
		if ( null == MainFrame.m_aBackTrackPanels[++MainFrame.g_nBackPanelIndex] )
		{
			//alert( "Adding: MainFrame.m_aBackTrackPanels[" + MainFrame.g_nBackPanelIndex + "] = " + MainFrame.m_aBackTrackPanels[MainFrame.g_nBackPanelIndex] );
			MainFrame.m_aBackTrackPanels[MainFrame.g_nBackPanelIndex] = nPanel;
			//alert( "Now At: MainFrame.m_aBackTrackPanels[" + MainFrame.g_nBackPanelIndex + "] = " + MainFrame.m_aBackTrackPanels[MainFrame.g_nBackPanelIndex] + " " + MainFrame.m_nCurrentPanel);
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
		//alert( "Deleting: MainFrame.m_aBackTrackPanels[" + MainFrame.g_nBackPanelIndex + "] = " + MainFrame.m_aBackTrackPanels[MainFrame.g_nBackPanelIndex] + " " + MainFrame.m_nCurrentPanel);
		MainFrame.m_aBackTrackPanels[MainFrame.g_nBackPanelIndex--] = null;
		nPanel = MainFrame.m_aBackTrackPanels[MainFrame.g_nBackPanelIndex];
		//alert( "Now At: MainFrame.m_aBackTrackPanels[" + MainFrame.g_nBackPanelIndex + "] = " + MainFrame.m_aBackTrackPanels[MainFrame.g_nBackPanelIndex] + " " + MainFrame.m_nCurrentPanel);
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
			window.parent.frames("contents").OnFinishButton();
			
		// Get the current panel
		var nCurPanel = MainFrame.m_nCurrentPanel;
		
		// Roadmap frame to access objects local to him
		var RMFrame = window.parent.frames("contents");
			
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
				
				// Check if we are not activated, in that case we have to 
				// do a custom action depending on layout
				if ( window.external.ObjectArg("DRM::LicenseState") != DJSMAR00_LicenseState_PURCHASED)
				{
					switch( MainFrame.g_iLicenseType )
					{
						// For retail flows skip subscription and go to postinstall
						case DJSMAR_LicenseType_Retail:
						case DJSMAR_LicenseType_Trial:
						case DJSMAR_LicenseType_Beta:
						case DJSMAR_LicenseType_ESD:
							if(MainFrame.g_bShowPostInstallTasks)
							{
								nNextPanel = window.parent.frames("contents").nPostInstall;
							}
							else
							{
								nNextPanel=window.parent.frames("contents").nConfigurationPanel;	
							}
						break;
						// For rental flow we close down CfgWiz
						case DJSMAR_LicenseType_Rental:
							if ( window.external.ObjectArg("DRM::SCSSTrialRemainingDays") > 0 )
							{
								if(MainFrame.g_bShowPostInstallTasks)
								{
									nNextPanel = window.parent.frames("contents").nPostInstall;
								}
								else
								{
									nNextPanel=window.parent.frames("contents").nConfigurationPanel;	
								}
							}
							else
							{
								MainFrame.bPromptUser = false;
								window.parent.frames("contents").OnCloseButton();
								return;
							}							
						break;
						
						// All others use the next panel
						default:
						break;
					}
				}
				
				//// Put our next panel in the backtrack list
				//InsertBackTrackPanel( nNextPanel );
				//// Load our next page
				//LoadPage( nNextPanel );
				//return;
			}
		}

		// See what panel we are on and see if we need to do custom actions 
		// for skipping the current panel
		//switch (MainFrame.m_aWizardPanels[nCurPanel].m_sTitle)
		//{
		//	// Custom actions for registration panel
		//	case "REGISTRATION":
		//	{
		//		// If we skipped registration don't send the reg data 
		//		// from the Activation control
		//		SendRegData = "false";
		//		nNextPanel = RMFrame.nLiveRegPanel + MainFrame.g_RegPanels;
		//	}
		//	break;
		//
		//	default:
		//	break;
		//}

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
			window.parent.frames("contents").OnFinishButton();

		// Roadmap frame to access objects local to him
		var RMFrame = window.parent.frames("contents");

		// Get the next panel
		var nNextPanel = MainFrame.m_nCurrentPanel + 1;
		
		// If we hit next on a registration page toggle flag to send registration
		// info throught the activation control to true			
		//if(MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_sTitle == "REGISTRATION")
		//{
		//	trace("REGISTRATION");
		//	
		//	var page = MainFrame.frames("main");
		//	SendRegData = "true";
		//	
		//	if (page.CanUnloadPage != null)
		//	{
		//		if (!page.CanUnloadPage())
		//			return;
		//	}
		//}
		
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
				
				// Check if we are not activated, in that case we have to 
				// do a custom action depending on layout
				if ( window.external.ObjectArg("DRM::LicenseState") != DJSMAR00_LicenseState_PURCHASED)
				{
					switch( MainFrame.g_iLicenseType )
					{
						// For retail flows skip subscription and go to postinstall
						case DJSMAR_LicenseType_Retail:
						case DJSMAR_LicenseType_Trial:
						case DJSMAR_LicenseType_Beta:
						case DJSMAR_LicenseType_ESD:
						
							if (MainFrame.g_bShowPostInstallTasks)
								nNextPanel = window.parent.frames("contents").nPostInstall;
								
						break;
						// For rental flow we close down CfgWiz
						case DJSMAR_LicenseType_Rental:
							if ( window.external.ObjectArg("DRM::SCSSTrialRemainingDays") > 0 )
							{
								if (MainFrame.g_bShowPostInstallTasks)
									nNextPanel = window.parent.frames("contents").nPostInstall;	
							}
							else
							{
								MainFrame.bPromptUser = false;
								window.parent.frames("contents").OnCloseButton();
								return;
							}							
						break;
						
						// All others use the next panel
						default:
						break;
					}
				}
				
				// Put our next panel in the backtrack list
				InsertBackTrackPanel( nNextPanel );
				
				// Load our next page
				LoadPage( nNextPanel );
				return;
			}
		}
		

		// Run the post action for the current panel
		if (MainFrame.m_nCurrentPanel >= 0)
		{
			if (MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_pPostAction != null)
			{
				MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_pPostAction();
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
				//trace("Register: " + Register);
				//trace("Activate: " + Activate);
				
				// If we are not going to register we are going to have to skip the registration panel
				//if (!Register)
				//{
					//if(Activate)
					//{
						// If chose to activate go directly to the activation panel
						nNextPanel = RMFrame.nActivationPanel;
					//}
					//else
					//{
						// If chose not to activate NAG THEM!!!
						
						// Get the days left in the grace period from the collection
						//var DaysLeft = MainFrame.g_iDaysRemaining;
						
						//if( DaysLeft > 0 )
						//{
							// If the days left are greater than 0 let them know how much time they have left
						//	var strSkipActivation = strActLaterGrace1.innerText + " " + DaysLeft + " " + strActLaterGrace2.innerText;
						//}
						//else
						//{
							// If the days left are 0 tell them they must activate or are not protected
						//	var strSkipActivation = strActLaterNoGrace.innerText;
						//}	
						
						// MessageBox
						//iMsgBoxReturn = MainFrame.g_WebWnd.MsgBox( strSkipActivation, MainFrame.g_strProductName, MB_YESNO | MB_ICONQUESTION);

						// If you still want to skip goto Product Specific panel (postinstall tasks) otherwise stay on selection panel
						//if (iMsgBoxReturn == IDYES)
						//	return;
						//else if (MainFrame.g_bShowPostInstallTasks)
						//{
						//	nNextPanel = RMFrame.nPostInstall;
						//}
						//else
						//{
						//	nNextPanel = RMFrame.nConfigurationPanel;
						//}
					//}
				//}
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
				
				// Run the pre action
				if (MainFrame.m_aWizardPanels[nNextPanel].m_pPreAction != null)
				{
					MainFrame.m_aWizardPanels[nNextPanel].m_pPreAction();
				}

				// Run the post action
				if (MainFrame.m_aWizardPanels[nNextPanel].m_pPostAction != null)
				{
					MainFrame.m_aWizardPanels[nNextPanel].m_pPostAction();
				}
				
				continue;
			}
			break;
		}
		// alert("nNextPanel: " + nNextPanel);		
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
		// Get the roadmap frame
		var RMFrame = window.parent.frames( "contents" );					
		
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

function LoadPage(nPanel)
{
	trace("LoadPage(" + nPanel + ")");

	if (nPanel > MainFrame.m_nMaxPanels - 1  || nPanel < -1)
	{
		return;
	}

	if (MainFrame.m_nCurrentPanel >= 0)
	{
		// Update the "content" frame
		MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_TitleID.style.backgroundImage = '';
		MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_Arrow.style.display = "none";

		MainFrame.m_nCurrentPanel = nPanel;
		MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_TitleID.style.backgroundImage = 'url(Current.gif)';
		MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_Arrow.style.display = "";
	}
	else
	{
		MainFrame.m_nCurrentPanel = nPanel;	

		// Update the "content" frame
		MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_TitleID.style.backgroundImage = 'url(Current.gif)';
		MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_Arrow.style.display = "";
	}
	
	// Run the Pre action for the panel
	if (MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_pPreAction != null)
		MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_pPreAction();
		
	// Load the page into the "main" frame
	window.parent.frames("main").location.replace(MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_htm);
	
	HandleControlStates( nPanel );
	

	// Disable the Back button 
	if (MainFrame.m_nCurrentPanel <= MainFrame.m_nFirstPanel && MainFrame.m_aWizardPanels[MainFrame.m_nCurrentPanel].m_bLoadPanel)
	{
		BackButton.disabled	= true;
	}
	else
	{
		BackButton.disabled = false;
	}

	// Determine the Next and Finish button state
	if (MainFrame.m_nCurrentPanel == MainFrame.m_nMaxPanels - 1)
	{
		// We are at the last panel.
		
		// Change Next to Finish.
		NextButton.style.visibility = "visible";
		NextButton.disabled = false;
		NextButton.accessKey = FinishHotKey;
		NextButton.innerHTML = FinishBtnText;
		NextButton.focus();
		
		// disable the back button
		BackButton.disabled	= true;
		
		// Hide skip button.
		SkipButton.style.visibility = "hidden";
	}
	else
	{
		// We are not at the last panel
		// Set button to Next.
		NextButton.style.visibility = "visible";
		NextButton.disabled = false;
		NextButton.accessKey = NextHotKey;
		NextButton.innerHTML = NextBtnText;
		NextButton.focus();	
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
