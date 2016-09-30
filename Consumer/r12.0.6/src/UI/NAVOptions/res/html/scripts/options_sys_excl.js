// options_sys_excl.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 121;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Enable / Disable the buttons
function enable_buttons()
{
   try
   {
	 document.options_sys_excl.Change.disabled =
	 document.options_sys_excl.Remove.disabled = document.options_sys_excl.Exclusions.options.selectedIndex < 0;
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
        parent.NAVOptions.EXCLUSIONS.Remove(document.options_sys_excl.Exclusions.selectedIndex);
    }
    catch(err)
    {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        return;
    }

	document.options_sys_excl.Exclusions.options[document.options_sys_excl.Exclusions.selectedIndex] = null;

	// Re-set focus to the list
	document.options_sys_excl.Exclusions.focus();
  
	enable_buttons();
  }
  catch (err)
  {
	parent.g_ErrorHandler.DisplayException (err);
	return;
  }
}

function add_update_exclusion(varAdd)
{
  try
  {	
	// If user dblclicked on the list and there is no selection, change
	// default dblclick action to be Add.
	if (document.options_sys_excl.Exclusions.selectedIndex == -1)
		varAdd = true;
	var Clone;

    try
    {
	    // When we are editing keep a snapshot of the current exclusion
	    if (!varAdd)
		    Clone = parent.NAVOptions.EXCLUSIONS(document.options_sys_excl.Exclusions.selectedIndex).Clone;
    }
    catch(err)
    {
        g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
										document.frames("Errors").ERROR_EDITING_EXCLUSIONS_ID.innerText);
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
 
	try
	{
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
        var rv = sww.showModalDialog('res://' + NAVPath + '\\navopts.loc/options_exclude_dlg.htm', 445, 180, null);
	}
	catch (NAVErr)
	{
		g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
										document.frames("Errors").ERROR_SHOWING_EXCLUSION_DIALOG_ID.innerText);	
	}

	delete sww;
	window.external.Global = null;
  	
    try
    {
	    // If the user canceled the operation restore the previous properties
	    // of the exclusion.
	    if (!varAdd && !rv)
		    parent.NAVOptions.EXCLUSIONS(document.options_sys_excl.Exclusions.selectedIndex).Clone = Clone;
    }
    catch(err)
    {
        g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
						document.frames("Errors").ERROR_EDITING_EXCLUSIONS_ID.innerText);
        return;
    }

	// Re-set focus to the list
	document.options_sys_excl.Exclusions.focus();
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }
}

// Main page object
function ExclusionPage()
{
  try
  {
	// Set missing methods to default implementations
	this.Terminate = function() {}

	this.Default   = function()
	{
		try
        {
            parent.NAVOptions.EXCLUSIONS.Default();
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
            parent.NAVOptions.Help(1216); // IDH_NAVW_EXCLUSIONS_LIST_DLG_HELP_BTN
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
	            
            if (typeof(parent.NAVOptions.EXCLUSIONS) == "undefined")
                return;
                
            if ('he' == navigator.systemLanguage)
		    {
		        // Prevent crash after adding an extesion
		        // & saving options on Hebrew Win98, IE4;SP1
		        document.options_sys_excl.Exclusions.style.fontFamily = '';
		    }

            // re-Initialize the list (after defaults are restored)
		    document.options_sys_excl.Exclusions.options.length = 0;

       
		    // Set these HTML controls from the NAVOptions object
		    for (var i = 0; i < parent.NAVOptions.EXCLUSIONS.Count; i++)
		    {
			    document.options_sys_excl.Exclusions.options.length++;
			    document.options_sys_excl.Exclusions.options[i].text = parent.NAVOptions.EXCLUSIONS(i).Path;
			    document.options_sys_excl.Exclusions.options[i].value = i + 1;
		    }

            // Set focus to first element
		    document.options_sys_excl.Exclusions.focus();

		    enable_buttons();
        }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
            document.options_sys_excl.Change.disabled =
	        document.options_sys_excl.Remove.disabled =
            document.options_sys_excl.Create.disabled = true;
        }

	}  // end of Initialize()

	// Initialize this page
	this.Initialize();
  }
  catch (err)
  {
	parent.g_ErrorHandler.DisplayException (err);
	return;
  }
}
