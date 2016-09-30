////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// options.js: Common routines
/////////////////////////////////////////////////////

// Scope globals
//
g_btnDefaultPage = null;
g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 100;  // This uniquely defines this script file to the error handler
var NAVOptions = 0;

//Constant from ccPasswdExp.h
var NIS_CONSUMER_PASSWORD_ID = 2; // share the NIS password ID.

var IsTrialValid = 0;
var InitialLoadVxD = 0;
var InitialDelayLoad = 0;
var InitialALUProduct = -1;
var InitialALUVirusdefs = -1;
var ALUhosed = 0;
var InitialPassword = false;
var InitialPasswordSet = false;
var ClearPass = false;
var defPassSet = false;

var CurrentPage = 0;
var IsLoaded = 0;
var bClosedWithX = true;
var IsEmailEnabled = 0;
var IsOutgoingEmailEnabled = 0;
var g_bStopLoading = false;
var g_HWND = 0;
var g_IsHighContrastMode = false;

/* constants from NAVOptions.idl
var ShowPageID_AUTOPROTECT = 1
var ShowPageID_EMAIL = 2
var ShowPageID_SPYWARE = 3
var ShowPageID_INBOUND_FIREWALL = 4
var ShowPageID_IM = 5
var ShowPageID_WORM_BLOCKING = 6
var ShowPageID_AUTO_UPDATE = 7
var ShowPageID_LIVE_UPDATE = 8
var ShowPageID_REAL_TIME_PROTECTION = 9
*/

var ShowPageID_RTP_GENERAL	= 9;			//options_rtp_general.htm	(ShowPageID_REAL_TIME_PROTECTION = 9)
var ShowPageID_RTP_AP		= 1;			//options_rtp_ap.htm		(ShowPageID_AUTOPROTECT = 1)
var ShowPageID_RTP_EMAIL	= 2;			//options_rtp_email.htm		(ShowPageID_EMAIL = 2)
var ShowPageID_RTP_IWP		= 4;			//options_rtp_iwp.htm		(ShowPageID_INBOUND_FIREWALL = 4)
var ShowPageID_RTP_IM		= 5;			//options_rtp_im.htm		(ShowPageID_IM = 5)
var ShowPageID_SCAN_GENERAL 	= 10;			//options_scan_general.htm	
var ShowPageID_LU_GENERAL	= 8;			//options_lu_general.htm	(ShowPageID_LIVE_UPDATE = 8)
var ShowPageID_ADV_EXCLUSIONS   = 11;			//options_adv_exclusions.htm
var ShowPageID_ADV_EXCLUSIONS_SECURITYRISKS = 12;	//options_adv_exclusions_securityrisks.htm
var ShowPageID_ADV_SECURITYRISKS = 3;			//options_adv_securityrisks.htm (ShowPageID_SPYWARE = 3)
var ShowPageID_ADV_MISC         = 13;			//options_adv_misc.htm

// Button constants
var BUTTON_LOC = "_loc";
var BUTTON_HOVER = "_hover";
var BUTTON_FOCUSED = "_focused";
var BUTTON_PRESSED = "_pressed";
var BUTTON_DISABLED = "_dis";

function Load ()
{
    try
    {
        CurrentPage = new CurrentPageDummy();
        
        NAVOptions = window.external.ObjectArg;
        
        document.title = document.title.replace(/%s/, NAVOptions.ProductName);

	g_IsHighContrastMode = NAVOptions.IsHighContrastMode;
	
        optionsmenu.Load ();
		
        if( g_bStopLoading )
        {
            location.href = 'closeme.xyz';
            return;
        }
        

        optionsbtm.Load ();
        
        IsLoaded = 1;
        optionsap.Load ();
        
        optionsmenu.g_LastRow = optionsmenu.rtp_general;
        
        switch (NAVOptions.InitialPage)
        {
			case ShowPageID_RTP_GENERAL:
				optionsmenu.RowClick(optionsmenu.rtp_general);
				break;
			case ShowPageID_RTP_AP:
				optionsmenu.RowClick(optionsmenu.rtp_ap);
				break;
			case ShowPageID_RTP_EMAIL:
				optionsmenu.RowClick(optionsmenu.rtp_email);
				break;
			case ShowPageID_RTP_IWP:
				optionsmenu.RowClick(optionsmenu.rtp_iwp);
				break;
			case ShowPageID_RTP_IM:
				optionsmenu.RowClick(optionsmenu.rtp_im);
				break;
			case ShowPageID_SCAN_GENERAL:
				optionsmenu.RowClick(optionsmenu.scan_general);
				break;
			case ShowPageID_LU_GENERAL:
				optionsmenu.RowClick(optionsmenu.lu_general);
				break;
			case ShowPageID_ADV_EXCLUSIONS:
				optionsmenu.RowClick(optionsmenu.adv_exclusions);
				break;
			case ShowPageID_ADV_EXCLUSIONS_SECURITYRISKS:
				optionsmenu.RowClick(optionsmenu.adv_exclusions_securityrisks);
				break;
			case ShowPageID_ADV_SECURITYRISKS:
				optionsmenu.RowClick(optionsmenu.adv_securityrisks);
				break;
			case ShowPageID_ADV_MISC:
				optionsmenu.RowClick(optionsmenu.adv_misc);
				break;
			default:
				//Default to RTP General Page
				optionsmenu.RowClick(optionsmenu.rtp_general);
				break;
        }
    }
    catch(err)
    {
        g_ErrorHandler.DisplayException(err);
    }
}

function CurrentPageDummy()
{
    try
    {
        this.Validate  = function() { return true; };
        this.Terminate = function() { return true; };
        this.Help      = function() { return true; };
    }
    catch(err)
    {
	    g_ErrorHandler.DisplayException (err);
	    return;
    }
}

// Enable the OK button only if the user typed in an extension
function OnKey()
{
  try
  {
    // if enter key was pressed click the OK button
	if (13 == window.event.keyCode)
	{
		window.event.cancelBubble = true;
	    window.event.returnValue = false;
	    if (!document.all.OK.disabled)
		  document.all.OK.onclick();
	}
	else
		setTimeout('EnableOK();', 0);
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }
}

function onHelp()
{
    try
    {
        parent.NAVOptions.Help(0);
    }
    catch(err)
    {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
    }
}

function Unload()
{
}

function trim(str) 
{
	var trimvalue = "";
	strlen = str.length;
	if (strlen < 1) return trimvalue;

	i = 0;
	pos = -1;
	while (i < strlen) 
    {
		if (str.charAt(i) != ' ' && !isNaN(str.charCodeAt(i)))
        {
			pos = i;
			break;
		}
		i++;
	}

	var lastpos = -1;
	i = strlen;
	while (i >= 0)
    {
		if (str.charAt(i) != ' ' && !isNaN(str.charCodeAt(i)))
        {
			lastpos = i;
			break;
		}
		i--;
	}

	return str.substring(pos,lastpos + 1);
}

// This function allows user to navigate the options menu by
// using the keyboard. This is to comply with Section 508. KM
function KeyboardNavigate(objTR) 
{
	try
	{
		// The "SPACE" and "ENTER" keys are used.
	   if ( (event.keyCode == 13) || (event.keyCode == 32) )
	   {
		 objTR.onclick();
	   }
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}

function GetButtonType(button)
{
	var className = button.className;
	return (className.substring(className.lastIndexOf("_")));
}

function SetButtonType(button, buttonType)
{
	ButtonNormalize(button);
	button.className = button.className + buttonType; 
}

function SetButtonHighContrast(button)
{
	if (g_IsHighContrastMode)
	{
		button.style.border = '1px solid white';
	}
}

// This function returns a button to its "normal" style
function ButtonNormalize(button)
{
	var buttonType = GetButtonType(button);
	
	switch (buttonType)
	{
		// Fall-through for non-normal button types
		case BUTTON_HOVER:
		case BUTTON_FOCUSED:
		case BUTTON_PRESSED:
		case BUTTON_DISABLED:
			button.className = button.className.substring(0, button.className.lastIndexOf("_"));
			button.disabled = false;
			break;
		default:
			// Already normalized
			break;
	};
	
	return;
}

function ButtonEnable(button)
{
	ButtonNormalize(button);
	button.enabled = true;
	
	SetButtonHighContrast(button);
}

function ButtonDisable(button)
{
	SetButtonType(button, BUTTON_DISABLED);
	button.disabled = true;
	
	SetButtonHighContrast(button);
}

// Function called when hovering mouse over a button
function ButtonMouseOver(button)
{
	var buttonType = GetButtonType(button);
	
	// Only change non-focus and non-disabled buttons to hover
	if (buttonType != BUTTON_FOCUSED && buttonType != BUTTON_DISABLED)
	{
		SetButtonType(button, BUTTON_HOVER);
	}
	
	SetButtonHighContrast(button);
}

function ButtonMouseDown(button)
{
	SetButtonType(button, BUTTON_PRESSED);
	
	SetButtonHighContrast(button);
}

// Function called when mouse leaves a button
function ButtonMouseOut(button)
{
	// Reset the button only if coming out of a hover
	if (GetButtonType(button) == BUTTON_HOVER)
	{
		ButtonNormalize(button);
	}
	// This other part is for situations where you
	// "click and drag" a button
	else if (GetButtonType(button) == BUTTON_PRESSED)
	{
		SetButtonType(button, BUTTON_FOCUSED);
	}
	
	SetButtonHighContrast(button);
}

function ButtonKeyDown(button)
{
	try
	{
		// The "SPACE" and "ENTER" keys are used.
	   if ( (event.keyCode == 13) || (event.keyCode == 32) )
	   {		 
		 SetButtonType(button, BUTTON_PRESSED);
	   }
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
	
	SetButtonHighContrast(button);
}

function ButtonKeyUp(button)
{	
	try
	{
		// The "SPACE" and "ENTER" keys are used.
	   if ( (event.keyCode == 13) || (event.keyCode == 32) )
	   {	
			SetButtonType(button, BUTTON_FOCUSED);
			button.onclick();
	   }
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
		
	SetButtonHighContrast(button);	
}

function ButtonFocus(button)
{
	// Give ButtonMouseDown priority
	if (GetButtonType(button) != BUTTON_PRESSED)
	{
		SetButtonType(button, BUTTON_FOCUSED);
	}
	
	SetButtonHighContrast(button);
}

function ButtonBlur(button)
{
	// Reset the button only if coming out of a focus
	if (GetButtonType(button) == BUTTON_FOCUSED)
	{
		ButtonNormalize(button);
	}
	// This other part is for buttons like "Page Default" where the
	// button loses focus when clicked, but we don't really want it to
	else if (GetButtonType(button) == BUTTON_PRESSED)
	{
		SetButtonType(button, BUTTON_FOCUSED);
	}

	SetButtonHighContrast(button);
}