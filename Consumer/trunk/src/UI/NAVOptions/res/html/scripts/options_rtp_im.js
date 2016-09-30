////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// options_int_im.js

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// IM Scanner global variables (treat as #defines)
g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 107;  // This uniquely defines this script file to the error handler

// Same values used for the Manual Scanner
var RESPONSE_MANUAL = 0
var RESPONSE_AUTO_THEN_QUARANTINE = 2
var IMTYPE_ALL = 0
var IMTYPE_MSN = 1
var IMTYPE_AOL = 2
var IMTYPE_YAHOO = 3

// Variables that indicate which individual scanners are installed
// This prevents us from having to call into ccIMScan.dll everytime we want to know
var IsMSNInstalled = 0;
var IsAOLInstalled = 0;
var IsYAHOOInstalled = 0;

try
{
    IsMSNInstalled = parent.NAVOptions.IsIMInstalled(IMTYPE_MSN);
    IsAOLInstalled = parent.NAVOptions.IsIMInstalled(IMTYPE_AOL);
    IsYAHOOInstalled = parent.NAVOptions.IsIMInstalled(IMTYPE_YAHOO);
}
catch(err)
{
    parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
}

// Variable to determine if at least one IM scanner is installed
var aScannerIsInstalled = IsMSNInstalled || IsAOLInstalled || IsYAHOOInstalled;

// Check to see if the Configure button should be enabled or disabled
function ConfCheck()
{
    try
	{
		if( document.options_rtp_im.MSN_Enabled.checked || 
		    document.options_rtp_im.YIM_Enabled.checked || 
		    document.options_rtp_im.AIM_Enabled.checked )
		{
			document.options_rtp_im.RbAlertRemove.disabled =
			document.options_rtp_im.RbAlertIgnore.disabled =
			document.options_rtp_im.RbAlertAsk.disabled = false;
			
		}
		else
		{
			document.options_rtp_im.RbAlertRemove.disabled =
			document.options_rtp_im.RbAlertIgnore.disabled =
			document.options_rtp_im.RbAlertAsk.disabled = true;			
		}
		
		ButtonNormalize(document.options_rtp_im.Configure_IM);
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}

// Configure IM Scanners on the fly
function ConfigureIM()
{
    try
    {
		// Configure the IM Clients that are currently checked
        try
		{
			if( document.options_rtp_im.MSN_Enabled.checked )
		       	parent.NAVOptions.ConfigureIM(IMTYPE_MSN);
	        if( document.options_rtp_im.YIM_Enabled.checked )
		       	parent.NAVOptions.ConfigureIM(IMTYPE_YAHOO);
	        if( document.options_rtp_im.AIM_Enabled.checked )
		       	parent.NAVOptions.ConfigureIM(IMTYPE_AOL);
		}
		catch (NAVErr)
		{
			parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
			return;
		}

        // Show success alert
		try
	    {	
			var sww = new ActiveXObject('CcWebWnd.ccWebWindow');
		}
		catch (NAVErr)
		{
			g_ErrorHandler.DisplayNAVError (document.frames("Errors").document.all.ERROR_INTERNAL_ERROR_REINSTALL.innerText,  
							document.frames("Errors").document.all.ERROR_CREATING_NAVWEBWINDOW_OBJECT_ID.innerText);
			return;			
		}
		sww.MsgBox(document.all.ConfiguredGood.innerText, document.title, 64);
		delete sww;
    }
    catch(err)
    {
		g_ErrorHandler.DisplayNAVError (document.frames("Errors").document.all.ERROR_INTERNAL_ERROR.innerText,  
						document.frames("Errors").document.all.ERROR_CONFIGURING_IM_SCANNERS_ID.innerText);
        return;
    }
}

// Main page object
function InstantMessengerPage()
{
   try
   {
	   // No page validation
	   this.Validate = function() { return true; }

	   // save options values from the screen if Messenger client is installed
	   this.Terminate = function()
	   {
		  // Don't worry about saving anything if nothing is installed
		  if( aScannerIsInstalled )
		  {
			try
			{
				if( IsMSNInstalled )
					parent.NAVOptions.IMEnabled(IMTYPE_MSN) = Math.abs(document.options_rtp_im.MSN_Enabled.checked);
			 	if( IsYAHOOInstalled )
			 		parent.NAVOptions.IMEnabled(IMTYPE_YAHOO) = Math.abs(document.options_rtp_im.YIM_Enabled.checked);
				if( IsAOLInstalled )
				   	parent.NAVOptions.IMEnabled(IMTYPE_AOL) = Math.abs(document.options_rtp_im.AIM_Enabled.checked);

				var alertMode;
				if(document.options_rtp_im.RbAlertRemove.checked)
					alertMode = 0;
				else if(document.options_rtp_im.RbAlertIgnore.checked)
					alertMode = 1;
				else
					alertMode = 2;

				parent.NAVOptions.IMSCAN_ResponseMode = alertMode;
			}
			catch(err)
			{
				parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
				return;
			}
		  }
	   }  // End of Terminate()

	   // Provide help for this page
	   this.Help = function()
	   {
		  try
            {
                 parent.NAVOptions.Help(11650); // IDH_NAVW_OPTIONS_INSTANT_MESSENGER_HELP_BTN
            }
            catch(err)
            {
                 parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
                 return;
            }
	   }

	   // restore default options values
	   this.Default = function()
	   {
		  // Set these HTML controls from the NAVOptions object
		  if (parent.IsTrialValid)
		  {
			  try
              	  {
					parent.NAVOptions.DefaultIMEnabled();
					parent.NAVOptions.IMSCAN_ResponseMode();
	            }
     	       catch(err)
          	  {
               		parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
              	  }
           }
	    }  // End of Default()

		// Initialize the from with the options value
		this.Initialize =  function()
		{
			// Set caption
			document.title = parent.NAVOptions.FeatureName;
			
			// Enable scrolling for high-contrast mode
			if( parent.NAVOptions.IsHighContrastMode )
			{
	     		body_rtp_im.scroll = "auto";
	     		document.options_rtp_im.Configure_IM.style.border = '1px solid white';
	     	}
	            	            
			// Set these HTML controls from the NAVOptions object
			if (parent.IsTrialValid)
			{ 
		  		if(!aScannerIsInstalled )
				{
					// No IM Scanners are installed disable everything and bail out
					document.options_rtp_im.MSN_Enabled.checked = false;
					document.options_rtp_im.YIM_Enabled.checked = false;
					document.options_rtp_im.AIM_Enabled.checked = false;
			
					document.options_rtp_im.MSN_Enabled.disabled = true;
					document.options_rtp_im.YIM_Enabled.disabled = true;
					document.options_rtp_im.AIM_Enabled.disabled = true;
						
			 		document.options_rtp_im.RbAlertRemove.disabled = true;
			 		document.options_rtp_im.RbAlertIgnore.disabled = true;
			 		document.options_rtp_im.RbAlertAsk.disabled = true;
			 		
					return;
				}
			 
				if( !IsMSNInstalled )
				{
					//document.all.msn_recommended.style.display = "none";
					//document.all.msn_not_available.style.display = "";
					document.options_rtp_im.MSN_Enabled.checked = false;
					document.options_rtp_im.MSN_Enabled.disabled = true;
				}
				else
				{
					document.options_rtp_im.MSN_Enabled.checked = parent.NAVOptions.IMEnabled(IMTYPE_MSN);
				}

				if( !IsYAHOOInstalled )
				{
					//document.all.yahoo_recommended.style.display = "none";
					//document.all.yahoo_not_available.style.display = "";
					document.options_rtp_im.YIM_Enabled.checked = false;
					document.options_rtp_im.YIM_Enabled.disabled = true;
				}
				else
				{
					document.options_rtp_im.YIM_Enabled.checked = parent.NAVOptions.IMEnabled(IMTYPE_YAHOO);
				}
			
				if( !IsAOLInstalled )
				{
					//document.all.aol_recommended.style.display = "none";
					//document.all.aol_not_available.style.display = "";
					document.options_rtp_im.AIM_Enabled.checked = false;
					document.options_rtp_im.AIM_Enabled.disabled = true;
				}
				else
				{
					document.options_rtp_im.AIM_Enabled.checked = parent.NAVOptions.IMEnabled(IMTYPE_AOL);
				}
				
				var alertMode = parent.NAVOptions.IMSCAN_ResponseMode;
				if(alertMode == 0)
					document.options_rtp_im.RbAlertRemove.checked = true;
				else if(alertMode == 1)
					document.options_rtp_im.RbAlertIgnore.checked = true;
				else
					document.options_rtp_im.RbAlertAsk.checked = true;				
			}
			else
			{
				// Invalid trialware. disable everything.
				document.options_rtp_im.MSN_Enabled.checked = false;
				document.options_rtp_im.YIM_Enabled.checked = false;
				document.options_rtp_im.AIM_Enabled.checked = false;

				document.options_rtp_im.MSN_Enabled.disabled = true;
				document.options_rtp_im.YIM_Enabled.disabled = true;
				document.options_rtp_im.AIM_Enabled.disabled = true;
				
				document.options_rtp_im.RbAlertRemove.disabled = true;
				document.options_rtp_im.RbAlertIgnore.disabled = true;
				document.options_rtp_im.RbAlertAsk.disabled = true;
				
				ButtonDisable(document.options_rtp_im.Configure_IM);
				
			}
      
			// Disable the Configure button if no scanners are checked
			ConfCheck();
      
			// Set focus to first non disabled element
			if (parent.IsTrialValid)
			{
				if( !document.options_rtp_im.AIM_Enabled.disabled )
					document.options_rtp_im.AIM_Enabled.focus();
				else if( !document.options_rtp_im.MSN_Enabled.disabled )
					document.options_rtp_im.MSN_Enabled.focus();
				else if( !document.options_rtp_im.YIM_Enabled.disabled )
					document.options_rtp_im.YIM_Enabled.focus();
			}
	   }  // end of Initialize()

	   // Initialize this page
	   this.Initialize();
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}  // end function InstantMessengerPage()
