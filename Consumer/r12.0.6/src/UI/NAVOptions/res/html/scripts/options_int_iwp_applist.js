// options_int_iwp_applist.js

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Global variables
g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 109;  // This uniquely defines this script file to the error handler

function OnHelp()
{
    try
    {
        window.external.Global.NAVOptions.Help(250002);
    }
    catch(err)
    {
        window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
        return;
    }
}

function OnLoad()
{
	// Set title
	document.title = window.external.Global.NAVOptions.ProductName;
	
	// Setting the working directory to the IWP path before launching ApplicationList
    window.external.Global.NAVOptions.SetWorkingDirIWP(true);
    
    // Get the property bag
	var fwUIPropertyBag = null;
	try
	{
		fwUIPropertyBag = window.external.Global.NAVOptions.FWUIPropBag;
	}
	catch(err)
	{
		window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
		fwUIPropertyBag = null;
		return;
	}
    
    try
    {
        // Initialize the application list with the property bag
        document.options_int_appcontrol.ApplicationList.Init(fwUIPropertyBag);
    }
    catch(err)
    {
        g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
										document.frames("Errors").ERROR_INIT_IWP_APPLIST_ID.innerText);
        return;
    }
    
    try
    {
		// Create Firewall settings object to get the default location.
		var oFirewallSettings = new ActiveXObject("ccFWSettg.FirewallSettings");
		var LocationList = oFirewallSettings.Locations;

		// Do not localized string.
		var DefaultLocation = LocationList.GetItemByName("Default");
		if( DefaultLocation == null )		
		{
			g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR_REINSTALL.innerText,
									document.frames("Errors").ERROR_IWP_APPLIST_DEFAULT_LOCATION_ID.innerText);
		}
		else
		{
			// Need to call pass in the location.
			document.options_int_appcontrol.ApplicationList.FilterByLocation = DefaultLocation;
		}
    }
    catch(err)
    {
		g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
										document.frames("Errors").ERROR_IWP_APPLIST_DEFAULT_LOCATION_ID.innerText);
		return;
    }
     
    try
    {
        // Call into to refresh rules.
        document.options_int_appcontrol.ApplicationList.RefreshRules();
		var iCount = document.options_int_appcontrol.ApplicationList.Count;
		if( iCount == 0)
		{
			document.options_int_appcontrol.Modify.disabled = true;
			document.options_int_appcontrol.Remove.disabled = true;
		}        
		else
		{
			document.options_int_appcontrol.Modify.disabled = false;
			document.options_int_appcontrol.Remove.disabled = false;
		}
    }
    catch(err)
    {
        g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
						document.frames("Errors").ERROR_IWP_APPLIST_REFRESH_RULES_ID.innerText);
    }
}
function OnUnload()
{
	// Setting the working directory back before exiting ApplicationList
	window.external.Global.NAVOptions.SetWorkingDirIWP(false);
}

function OnAdd()
{
    try
    {
        document.options_int_appcontrol.ApplicationList.Add();
		if(	document.options_int_appcontrol.Modify.disabled == true )
		{
			document.options_int_appcontrol.Modify.disabled = false;
		}
		if( document.options_int_appcontrol.Remove.disabled == true )
		{
			document.options_int_appcontrol.Remove.disabled = false;		
		}
    }
    catch(err)
    {
        g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
										document.frames("Errors").ERROR_IWP_APPLIST_ADD_ID.innerText);
    }
}

function OnModify()
{
    try
    {
        document.options_int_appcontrol.ApplicationList.Modify();
    }
    catch(err)
    {
        g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
										document.frames("Errors").ERROR_IWP_APPLIST_MODIFY_ID.innerText);
    }
}

function OnRemove()
{
    try
    {
        document.options_int_appcontrol.ApplicationList.Delete();
		var iCount = document.options_int_appcontrol.ApplicationList.Count;        
		if( iCount == 0 )
		{
			document.options_int_appcontrol.Modify.disabled = true;
			document.options_int_appcontrol.Remove.disabled = true;
		}
    }
    catch(err)
    {
        g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
										document.frames("Errors").ERROR_IWP_APPLIST_DELETE_ID.innerText);
    }
}
