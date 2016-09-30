// options_sys_ap_ad.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 119;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

function enable_both()
{
	try
	{
	  with(document.options_sys_ap_ad)
	  {
		ShutdownFloppiesScanBoth.disabled = !ShutdownFloppiesScan.checked
										 || !document.Enabled;

		if (!ShutdownFloppiesScan.checked)
		  ShutdownFloppiesScanBoth.checked = false;
	  }
	}
   catch (err)
   {
	 g_ErrorHandler.DisplayException (err);
	 return;
   }
}

// Enable / Disable
function enable_all()
{
   try
   {
	   if (parent.Is9x)
	   {
		 // Set Win9x specific controls
		 document.options_sys_ap_ad.LowLevelFormat.disabled            =
		 document.options_sys_ap_ad.BootRecordWriteHardDisk.disabled   =
		 document.options_sys_ap_ad.BootRecordWriteFloppyDisk.disabled = !document.Enabled;
		 enable_both();
	   }
	   else
	   {
	     // Set WinNt specific controls
	     document.options_sys_ap_ad.CompressedBlockedUI.disabled      = !document.Enabled;
		 try
		 {
		    document.options_sys_ap_ad.SystemLoad.disabled = !parent.NAVOptions.STARTUP.LoadVxD;
		 }
		 catch(err)
		 {
		    parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
		 }
	   }

	   document.options_sys_ap_ad.ScanFloppiesBootOnAccess.disabled =
	   document.options_sys_ap_ad.ShutdownFloppiesScan.disabled     = !document.Enabled;
   }
   catch (err)
   {
	 g_ErrorHandler.DisplayException (err);
	 return;
   }
}

// Main page object
function AutoProtectAdvancedPage()
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
		    if (parent.Is9x)
		    {
		      // Update Win9x specific values
		      parent.NAVOptions.TSR.Format        = document.options_sys_ap_ad.LowLevelFormat.value;
		      parent.NAVOptions.TSR.WriteHard     = document.options_sys_ap_ad.BootRecordWriteHardDisk.value;
		      parent.NAVOptions.TSR.WriteFlop     = document.options_sys_ap_ad.BootRecordWriteFloppyDisk.value;
		      parent.NAVOptions.TSR.ChkBothFlops  = Math.abs(document.options_sys_ap_ad.ShutdownFloppiesScanBoth.checked);
		    }
		    else
		    {
		      // Update WinNt specific values
		      parent.NAVOptions.APCOMPRESSED.BlockedAppUI = Math.abs(document.options_sys_ap_ad.CompressedBlockedUI.checked);
		      parent.NAVOptions.AUTOPROTECT.DelayLoad = Math.abs(!document.options_sys_ap_ad.SystemLoad.checked);
		    }

		    parent.NAVOptions.TSR.ChkFlopAccess = Math.abs(document.options_sys_ap_ad.ScanFloppiesBootOnAccess.checked);
		    parent.NAVOptions.TSR.ChkFlopOnBoot = Math.abs(document.options_sys_ap_ad.ShutdownFloppiesScan.checked);
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
		    parent.NAVOptions.Help(969); // IDH_NAVW_AP_ADVANCED_SETTINGS_DLG_HELP_BTN
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
		    if (parent.Is9x)
		    {
		      // Set Win9x specific defaults
		      parent.NAVOptions.TSR.Format();
		      parent.NAVOptions.TSR.WriteHard();
		      parent.NAVOptions.TSR.WriteFlop();
		      parent.NAVOptions.TSR.WriteExe();
		      parent.NAVOptions.TSR.ROAttrib();
		      parent.NAVOptions.TSR.ChkBothFlops();
		    }
		    else
		    {
		      // Set WinNt specific defaults
		      parent.NAVOptions.APCOMPRESSED.BlockedAppUI();
		      parent.NAVOptions.AUTOPROTECT.DelayLoad();
		    }

		    parent.NAVOptions.TSR.ChkFlopAccess();
		    parent.NAVOptions.TSR.ChkFlopOnBoot();
        }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }
	  }

	  // Initialize the from with the options value
	  this.Initialize =  function()
	  {
	    // Enable scrolling for high-contrast mode
        if( parent.NAVOptions.IsHighContrastMode )
	        body_options_sys_ap_ad.scroll = "auto";
	            
		// Global flag is set based on the currnet state of AP
		document.Enabled = parent.CurAPRunning;

		// Set focus to first element
		if (document.Enabled)
		  document.options_sys_ap_ad.ScanFloppiesBootOnAccess.focus();

        try
        {
		    // Set these HTML controls from the NAVOptions object
		    if (parent.Is9x)
		    {
		      // Set Win9x specific controls
		      document.options_sys_ap_ad.LowLevelFormat.value             = parent.NAVOptions.TSR.Format;
		      document.options_sys_ap_ad.BootRecordWriteHardDisk.value    = parent.NAVOptions.TSR.WriteHard;
		      document.options_sys_ap_ad.BootRecordWriteFloppyDisk.value  = parent.NAVOptions.TSR.WriteFlop;
		      document.options_sys_ap_ad.ShutdownFloppiesScanBoth.checked = parent.NAVOptions.TSR.ChkBothFlops;

		      //Hide the NT/2K/XP version of the scan when mounted text.
		      document.all.idtxt_mounted_NT2KXP.style.display="none";
		      document.all.WinNt.style.display="none";
		    }
		    else
		    {
		      // Set WinNt specific sections
		      document.options_sys_ap_ad.CompressedBlockedUI.checked = parent.NAVOptions.APCOMPRESSED.BlockedAppUI;
		      document.options_sys_ap_ad.SystemLoad.checked          = !parent.NAVOptions.AUTOPROTECT.DelayLoad;
		      
		      //Hide the 9x version of the scan when mounted text.
		      document.all.idtxt_mounted_9xMe.style.display="none";

		      // Hide all Win9x specific sections
		      for (var i = 0; i < document.all.Win9x.length; i++)
			     document.all.Win9x[i].style.display = 'none';
			     
			  // Show WinNt specific sections
			  document.all.WinNt.style.display="";
		    }

    		document.options_sys_ap_ad.ScanFloppiesBootOnAccess.checked = parent.NAVOptions.TSR.ChkFlopAccess;
	    	document.options_sys_ap_ad.ShutdownFloppiesScan.checked     = parent.NAVOptions.TSR.ChkFlopOnBoot;
        }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }

		enable_all();
	  }

	  // Initialize this page
	  this.Initialize();
   }
   catch (err)
   {
	 g_ErrorHandler.DisplayException (err);
	 return;
   }
}
