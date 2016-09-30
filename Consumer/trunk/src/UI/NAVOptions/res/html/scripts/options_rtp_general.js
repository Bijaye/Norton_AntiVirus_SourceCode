////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// options_rtp_general.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 118;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Called by options.js::Load() so the objects exist first
function Load ()
{
    if ( !parent.IsLoaded )
		return;
    try
    {
		parent.CurrentPage = new RealTimeProtectionPage();
    }
    catch(err)
    {
		parent.g_ErrorHandler.DisplayException (err);
        return;
    }
}

// Enable / Disable
function enable_all()
{
	try
	{
        	document.options_rtp_general.ScanFloppiesBootOnAccess.disabled =
	    	document.options_rtp_general.ShutdownFloppiesScan.disabled =
	    	!document.Enabled;
	}
	catch (err)
	{
	  	parent.g_ErrorHandler.DisplayException (err);
	  	return;
	}
}

// Main page object
function RealTimeProtectionPage()
{
  	try
  	{
		this.Initialize =  function()
	  	{
			// Enable scrolling for high-contrast mode
			if( parent.NAVOptions.IsHighContrastMode )
	        		body_options_rtp_general.scroll = "auto";
	        
			// Grab settings from NAVOptions
			document.options_rtp_general.ScanFloppiesBootOnAccess.checked = parent.NAVOptions.AUTOPROTECT_ScanBR;
			document.options_rtp_general.ShutdownFloppiesScan.checked = parent.NAVOptions.NAVAP_CheckFloppyOnBoot;

			// Set these HTML controls from the NAVOptions object
			if (parent.IsTrialValid)
			{
				document.Enabled = true;
			}
			else
			{
				// Invalid trialware. disable everything.
				document.Enabled = false;
			}

			// Set focus to first element
			if (!document.options_rtp_general.ScanFloppiesBootOnAccess.disabled)
				document.options_rtp_general.ScanFloppiesBootOnAccess.focus();

			enable_all();
		}

  		// No page validation
	  	this.Validate = function() { return true; }

	  	// save options values from the screen
	  	this.Terminate = function()
	  	{
			try
			{
				// Set the NAVOptions object from the HTML controls
				parent.NAVOptions.AUTOPROTECT_ScanBR = Math.abs(document.options_rtp_general.ScanFloppiesBootOnAccess.checked);
				parent.NAVOptions.NAVAP_CheckFloppyOnBoot = Math.abs(document.options_rtp_general.ShutdownFloppiesScan.checked);
			}
			catch(err)
			{
				parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
			} 
	  	}

	  	// Provide help for this page
	  	this.Help = function()
	  	{
			try
			{
				parent.NAVOptions.Help(888); // IDH_NAVW_AP_HELP_BTN
			}
        		catch(err)
        		{
            		parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        		}  
	  	}

	  	// restore default options values
	  	this.Default = function()
	  	{
			if (parent.IsTrialValid)
			{
				try
				{
					parent.NAVOptions.AUTOPROTECT_ScanBR();
					parent.NAVOptions.NAVAP_CheckFloppyOnBoot();
				}
				catch(err)
				{
					parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
				}
         		} 
	  	}

	  	// Initialize this page
	  	this.Initialize();
	  	top.rtp_general_initialized = true;
	}
	catch (err)
	{
	  	parent.g_ErrorHandler.DisplayException (err);
	  	return;
	}	  
}  // end of RealTimeProtectionPage()
