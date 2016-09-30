// options_sys_scan.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 123;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Manual Scan global variables (treat as #defines).
var RESPONSE_MANUAL = 0
var RESPONSE_AUTOMATIC = 1
var RESPONSE_AUTO_THEN_QUARANTINE = 2

// Enable / disable SelectExtensions
function SelectExtensions_enable()
{
	try
	{
		document.options_sys_scan.SelectExtensions.disabled = !document.options_sys_scan.FilesType[1].checked || document.options_sys_scan.FilesType[1].disabled;
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}

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
		  parent.NAVOptions.SCANNER.MasterBootRec = Math.abs(document.options_sys_scan.MasterBootRecords.checked);
		  parent.NAVOptions.SCANNER.BootRecs      = Math.abs(document.options_sys_scan.BootRecords.checked);
		  parent.NAVOptions.GENERAL.ScanZipFiles = Math.abs(document.options_sys_scan.ScanCompressedFiles.checked);
		  parent.NAVOptions.SCANNER.EnableSideEffectScan = Math.abs(document.options_sys_scan.ScanSideEffects.checked);
		  
		  // Response Mode
		  if (document.options_sys_scan.idrad_AutoRepair.checked)
			 parent.NAVOptions.SCANNER.ResponseMode = RESPONSE_AUTOMATIC;
		  else if (document.options_sys_scan.idrad_AskMe.checked)
			 parent.NAVOptions.SCANNER.ResponseMode = RESPONSE_MANUAL;
		  else
			 parent.NAVOptions.SCANNER.ResponseMode = RESPONSE_AUTO_THEN_QUARANTINE;
		  parent.NAVOptions.SCANNER.ScanAll       = Math.abs(document.options_sys_scan.FilesType[0].checked);

		  // For 9x add memory scans
		  if (parent.Is9x)
			parent.NAVOptions.SCANNER.Memory = Math.abs(document.options_sys_scan.ScanMemory.checked);
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
		    parent.NAVOptions.SCANNER.MasterBootRec();
		    parent.NAVOptions.SCANNER.BootRecs();
		    parent.NAVOptions.SCANNER.ResponseMode();
		    parent.NAVOptions.SCANNER.ScanAll();
		    parent.NAVOptions.GENERAL.ScanZipFiles();
		    parent.NAVOptions.SCANNER.EnableSideEffectScan();

		    // For 9x add memory scans
		    if (parent.Is9x)
		    {
		      parent.NAVOptions.SCANNER.Memory();
		    }
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
	            body_options_sys_scan.scroll = "auto";
	                    
		    // Set these HTML controls from the NAVOptions object
            document.options_sys_scan.MasterBootRecords.checked   = parent.NAVOptions.SCANNER.MasterBootRec;
            document.options_sys_scan.BootRecords.checked         = parent.NAVOptions.SCANNER.BootRecs;
            document.options_sys_scan.ScanCompressedFiles.checked = parent.NAVOptions.GENERAL.ScanZipFiles;
		    document.options_sys_scan.ScanSideEffects.checked     = parent.NAVOptions.SCANNER.EnableSideEffectScan;

		    switch(parent.NAVOptions.SCANNER.ResponseMode)
		    {
		       case RESPONSE_MANUAL:
			      document.options_sys_scan.idrad_AskMe.checked = true;
			      break;
		       case RESPONSE_AUTOMATIC:
			      document.options_sys_scan.idrad_AutoRepair.checked = true;
			      break;
		       default:
			      document.options_sys_scan.idrad_Quarantine.checked = true;
			      break;
		    }

		    document.options_sys_scan.FilesType[Number(!parent.NAVOptions.SCANNER.ScanAll)].checked = true;

		    // For 9x add memory scans
		    if (parent.Is9x)
		    {
		      document.options_sys_scan.ScanMemory.checked = parent.NAVOptions.SCANNER.Memory;
		    }
		    else
		    {
		      // Hide all Win9x specific sections
		      document.all.Win9x.style.display = 'none'; 
		    }
        }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }

		// Set focus to first element
		document.options_sys_scan.BootRecords.focus();

		SelectExtensions_enable();
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
