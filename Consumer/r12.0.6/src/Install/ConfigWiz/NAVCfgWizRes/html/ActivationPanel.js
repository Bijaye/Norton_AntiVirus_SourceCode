var MainFrame = window.parent;
var TimerID = 0;
var SelectFrame = null;
var bReady = false;
var g_lTimerID = 0;

//Enums for button states
var NEXT_BUTTON	= 0;
var BACK_BUTTON	= 1;
var SKIP_BUTTON	= 2;
	
var BUTTON_KILLED	= -1;
var BUTTON_DISABLE	= 0;
var BUTTON_ENABLE	= 1;
var BUTTON_HIDE	= 2;
var BUTTON_SHOW	= 4;
var BUTTON_SETFOCUS = 8;
var BUTTON_CLICK	= 16;

// Licensing Constants
var DJSMAR00_LicenseState_TRIAL		= 0;
var DJSMAR00_LicenseState_PURCHASED = 1;

var DJSMAR_LicenseType_Rental			= 2;
var DJSMAR_LicenseType_Unlicensed		= 5;

function OnLoad()
{
	document.body.tabIndex=-1;
	
	if (MainFrame.g_bRoadmapFrameLoaded == true && MainFrame.g_bSelectFrameLoaded == true)
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
	if (MainFrame.g_bRoadmapFrameLoaded == true && MainFrame.g_bSelectFrameLoaded == true)
	{
		try
		{
			SelectFrame = window.parent.frames("bottom");
			clearInterval(TimerID);
			
			// Disable the buttons for beginning state
			SelectFrame.DisableButtons();
			
			// Show the skip button
			SelectFrame.SkipButton.disabled = true;
			SelectFrame.SkipButton.style.visibility = "visible";
			
			if ( MainFrame.g_iLicenseType == DJSMAR_LicenseType_Rental )
			{
				ActBtnSkipText.innerHTML = ActBtnRentalSkipText.innerHTML;
				ActBtnSkipHotKey.innerText = ActBtnRentalSkipHotKey.innerText;
			}
			
			SelectFrame.SkipButton.innerHTML = ActBtnSkipText.innerHTML;
			SelectFrame.SkipButton.accessKey = ActBtnSkipHotKey.innerText;
			SelectFrame.bControlDone = false;
			
			// Get a pointer to our Activation object
			{
				MainFrame.g_PageOb = SymUIAx;
				
					
				// Set background to white		
				SymUIAx.SetProperty("Color0", "FFFFFF");
				SymUIAx.SetProperty("Color1", "FFFFFF");
				SymUIAx.SetProperty("Color2", "FFFFFF");
				SymUIAx.SetProperty("Color3", "FFFFFF");
				
				// Set us to first in chain if we are the first panel.
				if( MainFrame.m_nCurrentPanel  == 0 )
					SymUIAx.SetProperty( "FirstInChain", "true" );
				else
				SymUIAx.SetProperty( "FirstInChain", "false" );
	
				SymUIAx.SetProperty( "LastInChain", "false" );
				
				//SymUIAx.SetProperty( "SendLiveRegistration", SelectFrame.SendRegData);
				
				// Get the path to the helper DLL from the collection and tell the control
				SymUIAx.SetProperty( "HelperDLLPath", window.external.ObjectArg("DRM::ActivationUIPath"));
	
				SymUIAx.SetDataCollection( window.external.ObjectArg );
				
				SymUIAx.Initialize();
			}
		}
		catch(err)
		{
		}
	}
}

function OnActivateNowClick()
{
	MainFrame.g_bActivateLicenseNow = true;
}

function OnActivateLaterClick()
{
	MainFrame.g_bActivateLicenseNow = false;
}

function OnUnLoad()
{
	// Refresh license data after activation control is done.	
	
	MainFrame.g_iLicenseZone = window.external.ObjectArg("DRM::LicenseZone");
	MainFrame.g_iLicenseState = window.external.ObjectArg("DRM::LicenseState");
	MainFrame.g_bLicenseValid = ((MainFrame.g_iLicenseState == DJSMAR00_LicenseState_TRIAL) || (MainFrame.g_iLicenseState == DJSMAR00_LicenseState_PURCHASED));	
}
