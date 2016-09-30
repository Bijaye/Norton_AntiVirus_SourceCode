////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// options_scan_general.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 123;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Manual Scan global variables (treat as #defines).
var RESPONSE_MANUAL = 0
var RESPONSE_AUTO_THEN_QUARANTINE = 2

// Main page object
function ManualScanPage()
{
	try
	{
	  // No page validation
	  this.Validate = function() { return true; }

	  // save options values from the screen
	  this.Terminate = function()
	  {
			try
        		{
		  		// Set the NAVOptions object from the HTML controls
		  		parent.NAVOptions.SCANNER_ScanZipFiles = Math.abs(document.options_scan_general.ScanCompressedFiles.checked);
		  		parent.NAVOptions.SCANNER_EnableSideEffectScan = Math.abs(document.options_scan_general.ScanSideEffects.checked);
		  			  
				// Enable disable the heuristic level
				// There's no on/off for ScanMgr Bloodhound
				parent.NAVOptions.SCANNER_UserHeuristicLevel = Math.abs(document.options_scan_general.EnableBloodhoundHeuristics.checked)? 2 : 0;
          
				// Tracking Cookies
				parent.NAVOptions.SCANNER_ScanTrackingCookies = Math.abs(document.options_scan_general.ScanTrackingCookies.checked);
				
				// Rootkits
				parent.NAVOptions.SCANNER_StealthScan = Math.abs(document.options_scan_general.ScanStealthItems.checked);
				
				var alertMode;
				if(document.options_scan_general.RbAlertRemove.checked)
					alertMode = 0;
				else if(document.options_scan_general.RbAlertIgnore.checked)
					alertMode = 1;
				else
					alertMode = 2;

				parent.NAVOptions.SCANNER_ResponseMode = alertMode;				
        		}
        		catch(err)
        		{
	            		parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        		}
	  }  // End of Terminate()

	  // Provide help for this page
	  this.Help = function()
	  {
		try
		{
			parent.NAVOptions.Help(698); // IDH_NAVW_SCANNER_SETTINGS_DLG_HELP_BTN
		}
		catch(err)
		{
			parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
		}
	  }

	  // restore default options values
	  this.Default = function()
	  {
        	try
        	{
            	// Set these HTML controls from the NAVOptions object
		    	parent.NAVOptions.SCANNER_ScanZipFiles();
		    	parent.NAVOptions.SCANNER_EnableSideEffectScan();
	        	parent.NAVOptions.SCANNER_UserHeuristicLevel();
	        	parent.NAVOptions.SCANNER_ScanTrackingCookies();
	        	parent.NAVOptions.SCANNER_StealthScan();
	        	parent.NAVOptions.SCANNER_ResponseMode();
       		}
       		catch(err)
       		{
           		parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
       		}
	  }  // End of Default()

	  // Initialize the from with the options value
	  this.Initialize =  function()
	  {
        	try
        	{
	            	// Enable scrolling for high-contrast mode
	            	if( parent.NAVOptions.IsHighContrastMode )
	           		body_options_scan_general.scroll = "auto";
	                   
		    	// Set these HTML controls from the NAVOptions object
	            	document.options_scan_general.ScanCompressedFiles.checked = parent.NAVOptions.SCANNER_ScanZipFiles;
		    	document.options_scan_general.ScanSideEffects.checked     = parent.NAVOptions.SCANNER_EnableSideEffectScan;
			document.options_scan_general.EnableBloodhoundHeuristics.checked = parent.NAVOptions.SCANNER_UserHeuristicLevel;
			document.options_scan_general.ScanTrackingCookies.checked = parent.NAVOptions.SCANNER_ScanTrackingCookies;
			document.options_scan_general.ScanStealthItems.checked = parent.NAVOptions.SCANNER_StealthScan;

			var alertMode = parent.NAVOptions.SCANNER_ResponseMode;
			if(alertMode == 0)
				document.options_scan_general.RbAlertRemove.checked = true;
			else if(alertMode == 1)
				document.options_scan_general.RbAlertIgnore.checked = true;
			else
				document.options_scan_general.RbAlertAsk.checked = true;
        	}
        	catch(err)
        	{
	            	parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        	}

		// Set focus to first element
		document.options_scan_general.ScanCompressedFiles.focus();

	  }  // end of Initialize()

	  // Initialize this page
	  this.Initialize();
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}

}  // end function ManualScanPage()
