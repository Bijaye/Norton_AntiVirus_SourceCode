////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// options_int_lu.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 111;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Main page object
function LiveUpdatePage()
{
	try
	{
		// No page validation
	   	this.Validate = function() { return true; }

	   	// save options values from the screen
	   	this.Terminate = function()
	   	{
			// Set the NAVOptions object from the HTML controls
		  	try
          	{
		    		parent.NAVOptions.LiveUpdate = Math.abs(document.options_lu_general.AutomaticLiveUpdate.checked);
            		parent.NAVOptions.SCANNER_DefUpdateScan = Math.abs(document.options_lu_general.SideEffectScan.checked);
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
		    		parent.NAVOptions.Help(33041); // IDH_NAVW_AUTOUPDATE_SETTINGS_HELP_BTN
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
          		parent.NAVOptions.DefaultLiveUpdate();
          		parent.NAVOptions.SCANNER_DefUpdateScan();
        		}
        		catch(err)
        		{
            		parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        		}
	  	}  // End of Default()

	  	// Initialize the from with the options value
	  	this.Initialize =  function()
	  	{	  
	    		// Enable scrolling for high-contrast mode
        		if( parent.NAVOptions.IsHighContrastMode )
	        		body_options_lu_general.scroll = "auto";
	        
			try{
				document.options_lu_general.AutomaticLiveUpdate.checked = parent.NAVOptions.LiveUpdate;
				document.options_lu_general.SideEffectScan.checked = parent.NAVOptions.SCANNER_DefUpdateScan;
	    			document.options_lu_general.AutomaticLiveUpdate.focus();
          	}
        		catch(err)
        		{
            		parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        		}

	  	}  // end of Initialize()


	  	// Initialize this page
	  	this.Initialize();
	  	top.luInitialized = true;	  	
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}  // end function LiveUpdatePage()
