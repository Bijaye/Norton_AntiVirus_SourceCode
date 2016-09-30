// options_sys_ap.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 118;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

//Constants from APOptNames.h
enumAP_ACTION_PROMPT = 1;
enumAP_ACTION_REPAIR = 6;
enumAP_ACTION_QUARANTINE = 11;
enumAP_ACTION_DENYACCESS_NOTIFY = 5;
enumAP_ACTION_CONTINUE_NOTIFY = 3;

// Called by options.js::Load() so the objects exist first
function Load ()
{
	top.trace("OnAPLoad() -- parent.IsLoaded: " + parent.IsLoaded);
	if ( !parent.IsLoaded )
		return;

    try
    {
        parent.CurrentPage = new AutoProtectPage();
    }
    catch(err)
    {
        parent.g_ErrorHandler.DisplayException (err);
        return;
    }
}

// Lanches the AP Extensions dialog
function SelectAPExtensionsDlg()
{
    try
    {
        try
        {
            // Save the current extesion list.
            parent.OldExtensions = parent.NAVOptions.APExtensionList;
        }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
            return;
        }

        window.external.Global = parent;
        try
        {
            parent.sww = new ActiveXObject('ccWebWnd.ccWebWindow');
        }
        catch (NAVErr)
        {
            window.external.Global = null;
	        var msg = document.frames("Errors").document.all.ERROR_INTERNAL_ERROR_REINSTALL.innerText;
	        var id = document.frames("Errors").document.all.ERROR_CREATING_NAVWEBWINDOW_OBJECT_ID.innerText;
	        g_ErrorHandler.DisplayNAVError (msg, id);
		    return;
        }

        var AVPath;

        try
        {
            AVPath = parent.NAVOptions.NortonAntiVirusPath;
        }
        catch(err)
        {
            parent.sww = null;
            window.external.Global = null;
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
            return;
        }

        var rv;
        try
        {
            parent.sww.SetIcon2('res://' + AVPath + '\\Navopts.dll/201');
            rv = parent.sww.showModalDialog('res://' + AVPath + '\\navopts.loc/options_sys_sel_ap_ext_dlg.htm', 340, 380, null);
            parent.sww = null;
            window.external.Global = null;
        }
        catch(err)
        {
            g_ErrorHandler.DisplayException(err);
        }

        try
        {
            // if the user canceled revert to the old extensions.
            if (!rv)
                parent.NAVOptions.APExtensionList = parent.OldExtensions;
        }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
            return;
        }
    }
    catch (err)
    {
        g_ErrorHandler.DisplayException (err);
        return;
    }
}

// Enable / Disable
function SelectExtensions_enable()
{
   try
   {
	 document.options_sys_ap.SelectExtensions.disabled = 
		!document.options_sys_ap.Files2Scan[1].checked ||
        !document.options_sys_ap.idchk_EnableAutoProtect.checked || !document.Enabled;
   }
   catch (err)
   {
	 parent.g_ErrorHandler.DisplayException (err);
	 return;
   }
}

function OnEnableAPClick()
{
	enable_all();
}

function OnEnableAPChanged(status)
{
	if (top.apInitialized)
		top.snoozeAP = !status;
}

// Enable / Disable
function enable_all()
{
	try
	{
	  document.options_sys_ap.idrad_TSR_ACTION_REPAIR.disabled =
	  document.options_sys_ap.idrad_TSR_ACTION_PROMPT.disabled =
	  document.options_sys_ap.idrad_TSR_ACTION_QUAR_CANT_REP.disabled =
	  document.options_sys_ap.idrad_TSR_ACTION_DENY.disabled =
	  document.options_sys_ap.ShowAutoProtectIcon.disabled =
	  document.options_sys_ap.Files2Scan[0].disabled =
	  document.options_sys_ap.Files2Scan[1].disabled = 
	  document.options_sys_ap.idchk_APAutoScan.disabled = !document.Enabled
										|| !document.options_sys_ap.idchk_EnableAutoProtect.checked;
	  
	  // YOP would not like to give the users the ability to turn on AP Systray Icon												
	  document.options_sys_ap.ShowAutoProtectIcon.disabled = true;
	  
	  if (!parent.Is9x)
	  {
		// Never enable this option if not 9x
		document.options_sys_ap.idrad_TSR_ACTION_PROMPT.disabled=true;
		
		// Disable WinNt specific section
		document.options_sys_ap.ScanCompressed.disabled = !document.Enabled ||
													      !document.options_sys_ap.idchk_EnableAutoProtect.checked
	  }

	  SelectExtensions_enable();
	}
	catch (err)
	{
	  parent.g_ErrorHandler.DisplayException (err);
	  return;
	}
	  
}

// Main page object
function AutoProtectPage()
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
		    parent.CurAPRunning = Math.abs(document.options_sys_ap.idchk_EnableAutoProtect.checked);
    
		    parent.NAVOptions.STARTUP.LoadVxD = Math.abs(document.options_sys_ap.When2StartAutoProtect.checked);

            // Only configure the hide icon setting if the option is available
            if( !parent.NAVOptions.NAVOPTION.HideAPIconOption )
		        parent.NAVOptions.TSR.HideIcon = Math.abs(!document.options_sys_ap.ShowAutoProtectIcon.checked);
		        
		    parent.NAVOptions.TSR.ScanAll  = Math.abs(document.options_sys_ap.Files2Scan[0].checked);

            // Check to see if repair then quarantine is checked which will be the same
            // for 9x and NT
            if (document.options_sys_ap.idrad_TSR_ACTION_QUAR_CANT_REP.checked == true)
            {
		       parent.NAVOptions.AP.FileAction1 = enumAP_ACTION_REPAIR;
		       parent.NAVOptions.AP.MacroAction1 = enumAP_ACTION_REPAIR;
		       parent.NAVOptions.AP.FileAction2 = enumAP_ACTION_QUARANTINE;
		       parent.NAVOptions.AP.MacroAction2 = enumAP_ACTION_QUARANTINE;
		       
		       // Set the boot action to repair
		       parent.NAVOptions.AP.BootAction = enumAP_ACTION_REPAIR;
		    }
		    
		    // Deny is also shared by the OS's
		    else if (document.options_sys_ap.idrad_TSR_ACTION_DENY.checked == true) // deny
		    {
			    // Deny with prompt
		        parent.NAVOptions.AP.FileAction1 = enumAP_ACTION_DENYACCESS_NOTIFY;
		        parent.NAVOptions.AP.MacroAction1 = enumAP_ACTION_DENYACCESS_NOTIFY;
		        parent.NAVOptions.AP.FileAction2 = enumAP_ACTION_DENYACCESS_NOTIFY;
		        parent.NAVOptions.AP.MacroAction2 = enumAP_ACTION_DENYACCESS_NOTIFY;
		        
		        // Set the boot record action to just notify the user
		        parent.NAVOptions.AP.BootAction = enumAP_ACTION_CONTINUE_NOTIFY;
			}
		    else if (parent.Is9x)
		    {
		      if (document.options_sys_ap.idrad_TSR_ACTION_PROMPT.checked == true) // prompt
		      {
		            // Just prompt
		            parent.NAVOptions.AP.FileAction1 = enumAP_ACTION_PROMPT;
		            parent.NAVOptions.AP.MacroAction1 = enumAP_ACTION_PROMPT;
		            parent.NAVOptions.AP.FileAction2 = enumAP_ACTION_PROMPT;
		            parent.NAVOptions.AP.MacroAction2 = enumAP_ACTION_PROMPT;
		            
		            // Set the boot record action to prompt the user
		            parent.NAVOptions.AP.BootAction = enumAP_ACTION_PROMPT;
		      }
		      
			  else // repair
		      {
		            // Attempt repair then prompt
		            parent.NAVOptions.AP.FileAction1 = enumAP_ACTION_REPAIR;
		            parent.NAVOptions.AP.MacroAction1 = enumAP_ACTION_REPAIR;
		            parent.NAVOptions.AP.FileAction2 = enumAP_ACTION_PROMPT;
		            parent.NAVOptions.AP.MacroAction2 = enumAP_ACTION_PROMPT;
		            
		            // Set the boot action to repair
		            parent.NAVOptions.AP.BootAction = enumAP_ACTION_REPAIR;
		      }
		    }
		    else // NT - only repair is left
		    {
		        // Attempt repair then deny
		        parent.NAVOptions.AP.FileAction1 = enumAP_ACTION_REPAIR;
		        parent.NAVOptions.AP.MacroAction1 = enumAP_ACTION_REPAIR;
		        parent.NAVOptions.AP.FileAction2 = enumAP_ACTION_DENYACCESS_NOTIFY;
		        parent.NAVOptions.AP.MacroAction2 = enumAP_ACTION_DENYACCESS_NOTIFY;
		        
		        // Set the boot action to repair
		        parent.NAVOptions.AP.BootAction = enumAP_ACTION_REPAIR;
		    }
		    
		    if( !parent.Is9x )
		    {
		        // Set WinNt specific items from the controls
		        parent.NAVOptions.AP.ScanContainerFiles = Math.abs(document.options_sys_ap.ScanCompressed.checked);
		    }
		    
            parent.NAVOptions.AUTOPROTECT.AutoScanSideEffects = Math.abs(document.options_sys_ap.idchk_APAutoScan.checked);		    
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
				parent.NAVOptions.STARTUP.LoadVxD();
                parent.NAVOptions.TSR.ScanAll();
			    parent.NAVOptions.TSR.ScanRun();
			    parent.NAVOptions.TSR.ScanCreate();
			    
			    // Only set the hide icon default if the option is available
                if( !parent.NAVOptions.NAVOPTION.HideAPIconOption )
			        parent.NAVOptions.TSR.HideIcon();
			        
			    parent.NAVOptions.TSR.ScanAll();
			    parent.NAVOptions.AP.FileAction1();
			    parent.NAVOptions.AP.FileAction2();
			    parent.NAVOptions.AP.MacroAction1();
			    parent.NAVOptions.AP.MacroAction2();
			    
			    parent.NAVOptions.AUTOPROTECT.AutoScanSideEffects();
			    
			    if( !parent.Is9x )
			    {
			        // Set WinNT specific defaults
			        parent.NAVOptions.AP.ScanContainerFiles();
			    }
			}
			catch(err)
			{
				parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
			}

			try
			{
				parent.NAVAutoProtect.Configure(parent.NAVOptions.STARTUP.LoadVxD);
			}
			catch(err)
			{
				parent.NAVAutoProtect.NAVError.LogAndDisplay(parent.g_HWND);
			}
		    try
		    {
			    // User can't enable AP in a 'Safe Mode'
			    if (!parent.NAVOptions.SystemMode && parent.IsTrialValid)
			      parent.CurAPRunning = true;
		    }
		    catch(err)
		    {
			    parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
		    } 
         } 
	  }

	  // Initialize the from with the options value
	  this.Initialize =  function()
	  {
	    // Enable scrolling for high-contrast mode
	    if( parent.NAVOptions.IsHighContrastMode )
	        body_options_sys_ap.scroll = "auto";
	        
	    // Hide the AP icon if necessary
        if( parent.NAVOptions.NAVOPTION.HideAPIconOption )
            document.all.APHideIconRow.style.display = "none";
	    
		//Hide options not available on NT.
		if (!parent.Is9x)
		{
		  document.all.Win9x.style.display = 'none';
		  document.options_sys_ap.idrad_TSR_ACTION_PROMPT.disabled=true;
		  
		  // Show NT specific items
		  document.all.CompressedNtOnlySection.style.display = "";
		}
		else
		{
		  // Hide NT specific items
		  document.all.CompressedNtOnlySection.style.display = "none";
		}

		document.options_sys_ap.idchk_EnableAutoProtect.checked = parent.CurAPRunning;
		
		try
		{
			// User can't enable AP in a 'Safe Mode'
			if (parent.NAVOptions.SystemMode)
			  document.options_sys_ap.idchk_EnableAutoProtect.disabled = true;
			  
			document.options_sys_ap.When2StartAutoProtect.checked = parent.NAVOptions.STARTUP.LoadVxD;
		}
		catch(err)
		{
			parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
		}

		// Set these HTML controls from the NAVOptions object
		if (parent.IsTrialValid)
		{
		  document.Enabled = true;
		}
		else
		{
		  // Invalid trialware. disable everything.
		  document.Enabled = false;
		  document.options_sys_ap.When2StartAutoProtect.disabled = true;
		  document.options_sys_ap.idchk_EnableAutoProtect.disabled = true;
		}

		try
		{
		    // Only set the hide icon checkbox if the option is available
            if( !parent.NAVOptions.NAVOPTION.HideAPIconOption )
			    document.options_sys_ap.ShowAutoProtectIcon.checked = !parent.NAVOptions.TSR.HideIcon;
			    
			document.options_sys_ap.Files2Scan[Number(!parent.NAVOptions.TSR.ScanAll)].checked = true;
			
			// Set NT specific controls from the options object
			if( !parent.Is9x )
			    document.options_sys_ap.ScanCompressed.checked = parent.NAVOptions.AP.ScanContainerFiles;
		}
		catch(err)
		{
			parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
		}

		var MainAction1 = 6; // default to repair
		var MainAction2 = 5; // default to deny

		try
		{
			MainAction1 = parent.NAVOptions.AP.FileAction1;
			MainAction2 = parent.NAVOptions.AP.FileAction2;
		}
		catch(err)
		{
			parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
		}
		
		// Is it repair then quarantine? which is a cross platform setting
		if( MainAction1 == enumAP_ACTION_REPAIR && MainAction2 == enumAP_ACTION_QUARANTINE )
		    document.options_sys_ap.idrad_TSR_ACTION_QUAR_CANT_REP.checked = true;
		    
		// Is it deny access which is also shared by 9x and NT?
		else if( MainAction1 == enumAP_ACTION_DENYACCESS_NOTIFY && MainAction2 == enumAP_ACTION_DENYACCESS_NOTIFY )
		    document.options_sys_ap.idrad_TSR_ACTION_DENY.checked = true;
		    
		// Check the 9x specfic options
		else if( parent.Is9x )
		{
		    // Prompt?
		    if( MainAction1 == enumAP_ACTION_PROMPT && MainAction2 == enumAP_ACTION_PROMPT )
		        document.options_sys_ap.idrad_TSR_ACTION_PROMPT.checked = true;
		    // Gots to be repair
		    else
		        document.options_sys_ap.idrad_TSR_ACTION_REPAIR.checked = true;
		}
		else
		{
		    // NT only has repair left as an option
		    document.options_sys_ap.idrad_TSR_ACTION_REPAIR.checked = true;
		}
		
        document.options_sys_ap.idchk_APAutoScan.checked = parent.NAVOptions.AUTOPROTECT.AutoScanSideEffects;                       

		// Set focus to first element
		if (!document.options_sys_ap.idchk_EnableAutoProtect.disabled)
		  document.options_sys_ap.idchk_EnableAutoProtect.focus();
		enable_all();
	  }

	  // Initialize this page
	  this.Initialize();
	  top.apInitialized = true;
	}
	catch (err)
	{
	  parent.g_ErrorHandler.DisplayException (err);
	  return;
	}	  
}  // end of AutoProtectPage()
