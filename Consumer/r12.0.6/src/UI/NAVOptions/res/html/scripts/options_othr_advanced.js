// options_othr_gen.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 113;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Main page object
function AdvancedPage()
{
	try
	{
	  // No page validation
	  this.Validate = function() { return true; }

	  // save options values from the screen
	  this.Terminate = function()
	  {
	  }  // End of Terminate()

	  // Provide help for this page
	  this.Help = function()
	  {
        try
        {
		    parent.NAVOptions.Help(11600); //  IDH_NAVW_ADVANCED_TOOLS_DLG_HELP_BTN
        }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }
	  }

	  // restore default options values
	  this.Default = function()
	  {
	  }  // End of Default()

	  // Initialize the from with the options value
	  this.Initialize =  function()
	  {
	    // Enable scrolling for high-contrast mode
        if( parent.NAVOptions.IsHighContrastMode )
	        body_gen.scroll = "auto";
	        
		// Set focus to first element
		//document.options_othr_advanced.idtxt_opts_othr_advanced_recyclebin.focus();
	  }  // end of Initialize()

	  // Initialize this page
	  this.Initialize();
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}  // end function AdvancedPage()

function modify_recyclebin()
{
	try
	{
		parent.NAVOptions.ModifyRecycleBin ();
	}
	catch (err)
	{
		parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
		return;
	}
}

function modify_nprb()
{
	try
	{
		parent.NAVOptions.ModifyNPRB ();
	}
	catch (err)
	{
		parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
		return;
	}
}