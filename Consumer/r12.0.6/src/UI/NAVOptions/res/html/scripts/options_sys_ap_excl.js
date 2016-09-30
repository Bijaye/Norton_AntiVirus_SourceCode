// options_sys_ap_excl.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 120;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Enable / Disable the buttons
function enable_buttons()
{
   try
   {
	 document.options_sys_ap_excl.Change.disabled =
	 document.options_sys_ap_excl.Remove.disabled = document.options_sys_ap_excl.Exclusions.options.selectedIndex < 0;
   }
   catch (err)
   {
	 g_ErrorHandler.DisplayException (err);
	 return;
   }
}

// Remove an exclusion
function remove_exclusion()
{
  try
  {
    try
    {
	    parent.NAVOptions.RemoveAPExclusionItem(document.options_sys_ap_excl.Exclusions.selectedIndex);
    }
    catch(err)
    {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
    }

	document.options_sys_ap_excl.Exclusions.options[document.options_sys_ap_excl.Exclusions.selectedIndex] = null;

	// Re-set focus to the list
	document.options_sys_ap_excl.Exclusions.focus();
  
	enable_buttons();
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }
}

function add_update_exclusion(varAdd)
{
  try
  {

	// If user dblclicked on the list and there is no selection, change
	// default dblclick action to be Add.
	if (document.options_sys_ap_excl.Exclusions.selectedIndex == -1)
		varAdd = true;

    try
    {
        // If user is adding a new item then use the next available exclusion count
        // as the selected index
        if( varAdd )
            parent.AddSelectedIndex = parent.NAVOptions.APExclusionCount + 1;
        else
            parent.AddSelectedIndex = document.options_sys_ap_excl.Exclusions.selectedIndex;
    }
    catch(err)
    {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
    }

	parent.AddExclusion = varAdd;
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

	var NAVPath;
    
    try
    {
        NAVPath = parent.NAVOptions.NortonAntiVirusPath;
    }
    catch(err)
    {
        delete sww;
        window.external.Global = null;
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        return;
    }

    sww.SetIcon2('res://' + NAVPath + '\\Navopts.dll/201');
    if( parent.NAVOptions.IsHighContrastMode )
        sww.showModalDialog('res://' + NAVPath + '\\navopts.loc/options_exclude_ap_dlg.htm', 545, parent.Is9x ? 390 : 280, null);
    else
        sww.showModalDialog('res://' + NAVPath + '\\navopts.loc/options_exclude_ap_dlg.htm', 445, parent.Is9x ? 290 : 180, null);
	delete sww;
	window.external.Global = null;

	// Re-set focus to the list
	document.options_sys_ap_excl.Exclusions.focus();
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }
}

// Main page object
function APExclusionPage()
{
  try
  {
	// Set missing methods to default implementations
	this.Terminate = function() {}

	this.Default   = function()
	{
		try
		{
			// Need to set the extension list to defaults first since
			// the DefaultAPExclusions function will reset the internal
			// vector that is enumerated in the initialize function
			parent.NAVOptions.AP.ExclusionExt();
			parent.NAVOptions.DefaultAPExclusions();
			this.Initialize();
		}
		catch(err)
		{
			parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
		}
	}  // end of Default()

	this.Validate  = function() { return true; }

	// Provide help for this page
	this.Help = function()
	{
        try
        {
		    parent.NAVOptions.Help(1217); // IDH_NAVW_OPTIONS_AP_EXCLUSIONS_LIST_HELP_BTN
        }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }
	}

	// Initialize the from with the options value
	this.Initialize =  function()
	{
        try
        {
            // Enable scrolling for high-contrast mode
            if( parent.NAVOptions.IsHighContrastMode )
	            body_options_sys_excl.scroll = "auto";
	            
		    // re-Initialize the list (after defaults are restored)
		    document.options_sys_ap_excl.Exclusions.options.length = 0;

		    // Set these HTML controls from the NAVOptions object
		    for (var i = 0; i < parent.NAVOptions.APExclusionCount; i++)
		    {
			    document.options_sys_ap_excl.Exclusions.options.length++;
			    document.options_sys_ap_excl.Exclusions.options[i].text = parent.NAVOptions.APExclusionItemPath(i);
			    document.options_sys_ap_excl.Exclusions.options[i].value = i + 1;
		    }
        }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }

		// Set focus to first element
		document.options_sys_ap_excl.Exclusions.focus();

		enable_buttons();
	}  // end of Initialize()

	  // Initialize this page
	  this.Initialize();
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }
}
