////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

var MainFrame = window.parent;
var TimerID = 0;
var SelectFrame = window.parent.frames("bottom");
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
var cltLicenseAttrib_Trial = 0x00000008;
var cltLicenseAttrib_Active = 0x00000010;
var cltLicenseType_SOS			= (0x00000040);

function OnLoad()
{
	document.body.tabIndex=-1;
	
	if (SelectFrame.g_bSelectFrameLoaded == true)
	{
		UpdatePage();
	}
	else
	{
		TimerID = setInterval("UpdatePage()", 500);
	}
	
	// Get NAVOptionObj to detect high contrast settings.
	// Non-fatal if object is not obtained.
	try
    {
		NAVOptionObj = new ActiveXObject ("Symantec.Norton.AntiVirus.NAVOptions");
    }
    catch(err)
    {   
        return;
    }
}

function UpdatePage()
{
	if (SelectFrame.g_bSelectFrameLoaded == true)
	{
		try
		{
			clearInterval(TimerID);
			
			// Disable the buttons for beginning state
			SelectFrame.DisableButtons();
			
			// Show the skip button
			ButtonDisable(SelectFrame.SkipButton);
			SelectFrame.SkipButton.style.visibility = "visible";
			
			if ( MainFrame.g_iLicenseType == cltLicenseType_SOS )
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
	
				SymUIAx.SetProperty( "LastInChain", "true" );
				
				//SymUIAx.SetProperty( "SendLiveRegistration", SelectFrame.SendRegData);
				
				// Get the path to the helper DLL from the collection and tell the control
				SymUIAx.SetProperty( "HelperDLLPath", window.external.ObjectArg("clt::ActivationUIPath"));
	
				SymUIAx.SetDataCollection( window.external.ObjectArg );
				
				SymUIAx.Initialize();
			}
			
			// Keep button high contrast status updated
			SetButtonHighContrast(SelectFrame.BackButton);
			SetButtonHighContrast(SelectFrame.NextButton);
			SetButtonHighContrast(SelectFrame.SkipButton);
			
		}
		catch(err)
		{
		}
	}
}

function OnUnLoad()
{
	// Refresh license data after activation control is done.	
	MainFrame.g_iLicenseAttribs = window.external.ObjectArg("clt::LicenseAttribs");
	var bTrial = ((MainFrame.g_iLicenseAttribs & cltLicenseAttrib_Trial) == cltLicenseAttrib_Trial);
	var bActive = ((MainFrame.g_iLicenseAttribs & cltLicenseAttrib_Active) == cltLicenseAttrib_Active);
	MainFrame.g_bLicenseValid = (bTrial || bActive);

}
