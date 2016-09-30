// options_int_iwp.js

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Global variables
g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 108;  // This uniquely defines this script file to the error handler

// From IWPSettingsInterface.h
IWPStateError =	0;              // Couldn't get state - ERROR 
IWPStateNotRunning = 1;         // Agent isn't running (ccApp) but should be - ERROR
IWPStateNotAvailable = 100;     // Not supposed to exist - Not an error!
IWPStateYielding = 101;         // IWP is off because another product is taking precedence
IWPStateEnabled = 102;          // Default OK setting
IWPStateDisabled = 103;         // Loaded but turned off
IWPStateLoading = 104;          // Agent is loading
IWPStateUnloading = 105;        // Agent is unloading

var fwUI = null;

// Main page object
function WormProtectionPage()
{
   try
   {
	   // No page validation
	   this.Validate = function() { return true; }

	   // save options values from the screen if Messenger client is installed
	   this.Terminate = function()
	   {
	      try
	      {
	        if( parent.NAVOptions.IWPState != IWPStateYielding )
	            parent.NAVOptions.IWPUserWantsOn = Math.abs(document.options_int_wormprotection.IWP_Enabled.checked);
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
            parent.NAVOptions.Help(250001);
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
		  try
		  {
		    // Default value is on
		    if( parent.NAVOptions.IWPState != IWPStateYielding)
	            parent.NAVOptions.IWPUserWantsOn = true;
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
	                body_int_iwp.scroll = "auto";
        	        
	            // Set the enabled check box depending on the current user setting
	            document.options_int_wormprotection.IWP_Enabled.checked = parent.NAVOptions.IWPUserWantsOn;
	            
	            // Hide Firewall components?
	            if( !parent.NAVOptions.IWPFirewallOn )
	            {
	                document.all.AppRulesTable.style.display = 'none';
	                document.all.GeneralRulesTable.style.display = 'none';
	                document.all.TrojanRulesTable.style.display = 'none';
	            }
        		
		        // Find out if we should disable these options
		        if( parent.NAVOptions.IWPState == IWPStateYielding )
		        {
		            document.options_int_wormprotection.IWP_Enabled.disabled = true;
		            document.options_int_wormprotection.Configure_IWP_Exclusions.disabled = true;
		            document.options_int_wormprotection.Configure_Application_Rules.disabled = true;
		            document.options_int_wormprotection.Configure_General_Rules.disabled = true;
		            document.options_int_wormprotection.Configure_Trojan_Rules.disabled = true;
		            document.options_int_wormprotection.Configure_Auto_Block.disabled = true;
		            document.all.WormProtectionDisabled.style.display = '';
		            
		            // set focus to the help link
	                document.all.iwp_help_link.focus();
		        }
		        else
		        {
		            // set focus to the enable/disable option
		            document.options_int_wormprotection.IWP_Enabled.focus();
		            
		            // Create the firewall UI object
		            if( fwUI == null )
		            {
		                try
		                {
		                    fwUI = new ActiveXObject("FwUI.FWRuleEditor");
		                }
		                catch(err)
		                {
		                    g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
										                     document.frames("Errors").ERROR_IWP_CREATE_FWUI_ID.innerText);
		                    fwUI = null;
		                    return;
		                }
		                
		                // Get the property bag
		                var fwUIPropBag = null;
		                try
		                {
		                    fwUIPropBag = parent.NAVOptions.FWUIPropBag;
		                }
		                catch(err)
		                {
		                    parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
		                    fwUIPropBag = null;
		                    return;
		                }
		                try
		                {   
		                    fwUI.Init(fwUIPropBag);
		                }
		                catch(err)
		                {
		                    g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
										                    document.frames("Errors").ERROR_IWP_INIT_FWUI_ID.innerText);
		                    fwUI = null;
		                }
		            }
	            }
		    }
		    catch(err)
		    {
		        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
		    }
		    
	   }  // end of Initialize()

	   // Initialize this page
	   this.Initialize();
       top.iwpInitialized = true;
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}  // end function WormProtectionPage()

function ConfigureExclusions()
{
    if( fwUI != null )
    {
        parent.NAVOptions.SetWorkingDirIWP(true);
        fwUI.LaunchSignatureExclusions();
        parent.NAVOptions.SetWorkingDirIWP(false);
    }
    else
    {
        g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
										document.frames("Errors").ERROR_IWP_CREATE_FWUI_ID.innerText);
    }
}

function ConfigureApplication()
{
    // Launch the application list
    window.external.Global = parent;
    
    try
	{
		var sww = new ActiveXObject('CcWebWnd.ccWebWindow');
	}
	catch (NAVErr)
	{
	    window.external.Global = null;
		g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR_REINSTALL.innerText,
										document.frames("Errors").ERROR_CREATING_NAVWEBWINDOW_OBJECT_ID.innerText);
		return;
	}
	
	try
	{
	    // Set the icon
        sww.SetIcon2('res://'+parent.NAVOptions.NortonAntiVirusPath+'\\Navopts.dll/201');
        
        // Pass in the property bag which the application list needs for initialization
        var rv = sww.showModalDialogNoParent('res://'+parent.NAVOptions.NortonAntiVirusPath+'\\navopts.loc/options_int_iwp_applist.htm', 450, 300, null);
	}
	catch (NAVErr)
	{
		g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
										document.frames("Errors").ERROR_SHOWING_IWP_APPLIST_DIALOG_ID.innerText);
	}

	delete sww;
	window.external.Global = null;
}

function ConfigureAutoBlock()
{
    // Launch the auto block page
    window.external.Global = parent;
    
    try
	{
		var sww = new ActiveXObject('CcWebWnd.ccWebWindow');
	}
	catch (NAVErr)
	{
	    window.external.Global = null;
		g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR_REINSTALL.innerText,
										document.frames("Errors").ERROR_CREATING_NAVWEBWINDOW_OBJECT_ID.innerText);
		return;
	}
	
	try
	{
	    // Set the icon
        sww.SetIcon2('res://'+parent.NAVOptions.NortonAntiVirusPath+'\\Navopts.dll/201');
        
        parent.NAVOptions.SetWorkingDirIWP(true);
        
        // Pass in the property bag which the application list needs for initialization
        var rv = sww.showModalDialogNoParent('res://'+parent.NAVOptions.NortonAntiVirusPath+'\\navopts.loc/options_int_iwp_autoblock.htm', 400, 300);
	}
	catch (NAVErr)
	{
		g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
										document.frames("Errors").ERROR_SHOWING_IWP_AUTOBLOCK_DIALOG_ID.innerText);
	}
	
	parent.NAVOptions.SetWorkingDirIWP(false);

	delete sww;
	window.external.Global = null;
}

function ConfigureGeneral()
{
    if( fwUI != null )
    {
        parent.NAVOptions.SetWorkingDirIWP(true);
        fwUI.EditSystemRules(0);
        parent.NAVOptions.SetWorkingDirIWP(false);
    }
    else
    {
        g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
										document.frames("Errors").ERROR_IWP_CREATE_FWUI_ID.innerText);
    }
}

function ConfigureTrojan()
{
    if( fwUI != null )
    {
        parent.NAVOptions.SetWorkingDirIWP(true);
        fwUI.EditTrojanRules(0);
        parent.NAVOptions.SetWorkingDirIWP(false);
    }
    else
    {
        g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
										document.frames("Errors").ERROR_IWP_CREATE_FWUI_ID.innerText);
    }
}

function OnEnableIWPChanged(status)
{
	if (top.iwpInitialized)
		top.snoozeWP = !status;
}
