////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// options_othr_excl_main.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 128;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 117;  // This uniquely defines this script file to the error handler

var g_ThreatsByVID = null;

//-------------------------------------FS Exclusions---------------------------------//

// Enable / Disable the buttons
function enable_fs_buttons()
{
   try
   {
		if (document.options_adv_exclusions_fs.FS_Exclusions.options.selectedIndex < 0)
		{
			ButtonDisable(document.options_adv_exclusions_fs.Change);
			ButtonDisable(document.options_adv_exclusions_fs.Remove);
		}
		else
		{
			ButtonNormalize(document.options_adv_exclusions_fs.Change);
			ButtonNormalize(document.options_adv_exclusions_fs.Remove);
		}
   }
   catch (err)
   {
	 g_ErrorHandler.DisplayException (err);
	 return;
   }
}

// Remove an exclusion
function remove_fs_exclusion()
{
  try
  {
    try
    {
	    parent.NAVOptions.RemovePathExclusionItem(
			document.options_adv_exclusions_fs.FS_Exclusions.selectedIndex);	
    }
    catch(err)
    {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
    }

	document.options_adv_exclusions_fs.FS_Exclusions.options[document.options_adv_exclusions_fs.FS_Exclusions.selectedIndex] = null;

	// Re-set focus to the list
	document.options_adv_exclusions_fs.FS_Exclusions.focus();
  
	enable_fs_buttons();
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }
}

function fs_initialize()
{
    try
    {
		// re-Initialize the list (after defaults are restored)
		document.options_adv_exclusions_fs.FS_Exclusions.options.length = 0;
		// Set these HTML controls from the NAVOptions object
		try
		{
			var count = parent.NAVOptions.PathExclusionCount;
		}
		catch(NAVErr)
		{
			parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
		}
		for (var i = 0; i < count; i++)
		{
			// Get the current item
			try
			{
			
				var itemPath = parent.NAVOptions.PathExclusionItemPath(i);
			}
			catch(NAVErr)
			{
				parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
			}
			document.options_adv_exclusions_fs.FS_Exclusions.options.length++;
			document.options_adv_exclusions_fs.FS_Exclusions.options[i].text = itemPath;
			document.options_adv_exclusions_fs.FS_Exclusions.options[i].value = i + 1;
		}
    }
    catch(err)
    {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
    }

	// Set focus to first element	
	document.options_adv_exclusions_fs.FS_Exclusions.focus();

	enable_fs_buttons();
}

function set_fs_defaults()
{
	try
	{
		parent.NAVOptions.SetPathExclusionDefaults();
		this.fs_initialize();
	}
	catch(err)
	{
		parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
	}
}

function add_update_fs_exclusion(varAdd)
{
  with( document.options_adv_exclusions_fs )
  {

  var OldItem = "";
  var NewItem = "";

  try
  {
	// If user dblclicked on the list and there is no selection, change
	// default dblclick action to be Add.
	if (FS_Exclusions.selectedIndex == -1)
		varAdd = true;

    try
    {
        // If user is adding a new item then use the next available exclusion count
        // as the selected index
        if( varAdd )
            parent.AddSelectedIndex = parent.NAVOptions.PathExclusionCount + 1;
        else
        {
            parent.AddSelectedIndex = FS_Exclusions.selectedIndex;
        }
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
		parent.g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR_REINSTALL.innerText,  
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
    {
        sww.showModalDialog('res://' + NAVPath + '\\navopts.loc/options_exclude_threat_dlg.htm', 545, 280, null);
    }
    else
    {
        sww.showModalDialog('res://' + NAVPath + '\\navopts.loc/options_exclude_threat_dlg.htm', 445, 180, null);
    }
	delete sww;
	window.external.Global = null;

	// Re-set focus to the list
	FS_Exclusions.focus();
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }
  
 } // with(document.options_adv_exclusions_fs)
}

//-------------------------------------AP Exclusions---------------------------------//

// Enable / Disable the buttons
function enable_ap_buttons()
{
   try
   {
	 if (document.options_adv_exclusions_ap.AP_Exclusions.options.selectedIndex < 0)
	 {
	   ButtonDisable(document.options_adv_exclusions_ap.Change);
	   ButtonDisable(document.options_adv_exclusions_ap.Remove);	      
	 }
	 else
	 {
	   ButtonNormalize(document.options_adv_exclusions_ap.Change);
	   ButtonNormalize(document.options_adv_exclusions_ap.Remove);	
	 } 
   }
   catch (err)
   {
	 g_ErrorHandler.DisplayException (err);
	 return;
   }
}

// Remove an exclusion
function remove_ap_exclusion()
{
  try
  {
    try
    {
	    parent.NAVOptions.DeleteAPExclusion(document.options_adv_exclusions_ap.AP_Exclusions.selectedIndex);
    }
    catch(err)
    {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
    }

	document.options_adv_exclusions_ap.AP_Exclusions.options[document.options_adv_exclusions_ap.AP_Exclusions.selectedIndex] = null;

	// Re-set focus to the list
	document.options_adv_exclusions_ap.AP_Exclusions.focus();
  
	enable_ap_buttons();
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }
}

function add_update_ap_exclusion(varAdd)
{
  try
  {

	// If user dblclicked on the list and there is no selection, change
	// default dblclick action to be Add.
	if (document.options_adv_exclusions_ap.AP_Exclusions.selectedIndex == -1)
		varAdd = true;

    try
    {
        // If user is adding a new item then use the next available exclusion count
        // as the selected index
        if( varAdd )
            parent.AddSelectedIndex = parent.NAVOptions.APExclusionCount + 1;
        else
            parent.AddSelectedIndex = document.options_adv_exclusions_ap.AP_Exclusions.selectedIndex;
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
        sww.showModalDialog('res://' + NAVPath + '\\navopts.loc/options_exclude_ap_dlg.htm', 545, 280, null);
    else
        sww.showModalDialog('res://' + NAVPath + '\\navopts.loc/options_exclude_ap_dlg.htm', 445, 180, null);
	delete sww;
	window.external.Global = null;

	if(varAdd)
	{
		ap_initialize();	//re-initialize list if adding an element	
	}

	// Re-set focus to the list
	document.options_adv_exclusions_ap.AP_Exclusions.focus();
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }
}

function ap_initialize()
{            
	try{
		// re-Initialize the list (after defaults are restored)
		document.options_adv_exclusions_ap.AP_Exclusions.options.length = 0;
		var count = 0;
		try
		{
			count = parent.NAVOptions.APExclusionCount;
		}
		catch(NAVErr)
		{
			parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
		}
		
		// Set these HTML controls from the NAVOptions object
		for (var i = 0; i < count; i++)
		{
			document.options_adv_exclusions_ap.AP_Exclusions.options.length++;
			document.options_adv_exclusions_ap.AP_Exclusions.options[i].text = 
				parent.NAVOptions.APExclusionPath(i);
			document.options_adv_exclusions_ap.AP_Exclusions.options[i].value = i;
		}
	}
    catch(err)
    {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
    }
	// Set focus to first element
	document.options_adv_exclusions_ap.AP_Exclusions.focus();
	enable_ap_buttons();
}

//----------------------------------------------------------------------------//

// ** STD EXCL **

// Main page object
function ExclusionPage()
{

  var MB_YESNO = 0x00000004;
  var MB_ICONEXCLAMATION	= 0x00000030;
  var ID_YES = 6;
  var ID_NO = 7;
  try
  {
	// Set missing methods to default implementations
	this.Terminate = function() {}

	this.Default   = function()
	{
		try
		{
	        window.external.Global = parent;
            
	        try
	        {
		        var sww = new ActiveXObject('CcWebWnd.ccWebWindow');
	        }
	        catch (NAVErr)
	        {
	            window.external.Global = null;
		        parent.g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR_REINSTALL.innerText,  
										        document.frames("Errors").ERROR_CREATING_NAVWEBWINDOW_OBJECT_ID.innerText);
		        return;
	        }

            var an = sww.MsgBox(document.all.DefaultsWarning.innerText, parent.NAVOptions.ProductName, MB_YESNO|MB_ICONEXCLAMATION);
	        delete sww;
	        window.external.Global = null;

            if(an == ID_YES)
            {
		        parent.NAVOptions.SetPathExclusionDefaults();
    	        parent.NAVOptions.SetAPExclusionDefaults();
		        this.Initialize();
		    }
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
        		parent.NAVOptions.Help(1216);	//IDH_NAVW_EXCLUSIONS_LIST_DLG_HELP_BTN 1216
        						//IDH_NAVW_OPTIONS_AP_EXCLUSIONS_LIST_HELP_BTN 1217
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
        {
	        body_options_adv_exclusions.scroll = "auto";
	        document.options_adv_exclusions_fs.Create.style.border = '1px solid white';
	        document.options_adv_exclusions_fs.Change.style.border = '1px solid white';
	        document.options_adv_exclusions_fs.Remove.style.border = '1px solid white';
	        document.options_adv_exclusions_ap.Create.style.border = '1px solid white';
	        document.options_adv_exclusions_ap.Change.style.border = '1px solid white';
	        document.options_adv_exclusions_ap.Remove.style.border = '1px solid white';
	   }
	        
	    fs_initialize();
		ap_initialize();
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

function sortSelect(obj)
{
  try
  {
    if (obj == null || obj.options == null) 
	    return;
	    
	var o = new Array();

	for (var i=0; i<obj.options.length; i++)
	{
		o[o.length] = new Option( obj.options[i].text, obj.options[i].value, obj.options[i].defaultSelected, obj.options[i].selected) ;
	}
	
	if (o.length == 0)
	    return;

	o = o.sort( 
		function(a,b)
		{ 
			if ((a.text+"") < (b.text+""))
			    return -1;
			if ((a.text+"") > (b.text+""))
			    return 1;
			    
			return 0;
		} 
		);

	for (var i=0; i<o.length; i++)
	{
		obj.options[i] = new Option(o[i].text, o[i].value, o[i].defaultSelected, o[i].selected);
	}
  }
  catch(err)
  {
    g_ErrorHandler.DisplayException (err);
  }
}