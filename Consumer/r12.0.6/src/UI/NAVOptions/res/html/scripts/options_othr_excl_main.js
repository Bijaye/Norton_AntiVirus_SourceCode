// options_othr_excl_main.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 128;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 117;  // This uniquely defines this script file to the error handler

var g_ThreatsByVID = null;

// Enable / Disable the buttons
function enable_fs_buttons()
{
   try
   {
     document.options_othr_threat_excl.Change.disabled =
	 document.options_othr_threat_excl.Remove.disabled = document.options_othr_threat_excl.Exclusions.options.selectedIndex < 0;
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
	    parent.NAVOptions.RemoveThreatExclusionItem(document.options_othr_threat_excl.Exclusions.selectedIndex);	
    }
    catch(err)
    {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
    }

	document.options_othr_threat_excl.Exclusions.options[document.options_othr_threat_excl.Exclusions.selectedIndex] = null;

	// Re-set focus to the list
	document.options_othr_threat_excl.Exclusions.focus();
  
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
        // Enable scrolling for high-contrast mode
        if( parent.NAVOptions.IsHighContrastMode )
	        body_options_othr_threat_excl.scroll = "auto";
	        
		// re-Initialize the list (after defaults are restored)
		document.options_othr_threat_excl.Exclusions.options.length = 0;

		// Set these HTML controls from the NAVOptions object
		try
		{
			var count = parent.NAVOptions.ThreatExclusionCount;
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
			
				var itemPath = parent.NAVOptions.ThreatExclusionItemPath(i);
			}
			catch(NAVErr)
			{
				parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
			}
			
			document.options_othr_threat_excl.Exclusions.options.length++;
			document.options_othr_threat_excl.Exclusions.options[i].text = itemPath;
			document.options_othr_threat_excl.Exclusions.options[i].value = i + 1;
		}
    }
    catch(err)
    {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
    }

	// Set focus to first element	
	document.options_othr_threat_excl.Exclusions.focus();

	enable_fs_buttons();
}

function set_fs_defaults()
{
	try
	{
		parent.NAVOptions.SetThreatExclusionDefaults();
		this.fs_initialize();
	}
	catch(err)
	{
		parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
	}
}

function add_update_fs_exclusion(varAdd)
{
  with( document.options_othr_threat_excl )
  {

  var OldItem = "";
  var NewItem = "";

  try
  {
	// If user dblclicked on the list and there is no selection, change
	// default dblclick action to be Add.
	if (Exclusions.selectedIndex == -1)
		varAdd = true;

    try
    {
        // If user is adding a new item then use the next available exclusion count
        // as the selected index
        if( varAdd )
            parent.AddSelectedIndex = parent.NAVOptions.ThreatExclusionCount + 1;
        else
        {
            parent.AddSelectedIndex = Exclusions.selectedIndex;
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
	Exclusions.focus();
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }
  
 } // with(document.options_othr_threat_excl)
}

// ** ANOMALY EXCL **

// Enable / Disable the buttons
function enable_anomaly_buttons()
{
   try
   {
	 document.options_othr_anomaly_excl.Remove.disabled = document.options_othr_anomaly_excl.Details.disabled = document.options_othr_anomaly_excl.AnomalyExclusions.options.selectedIndex < 0;
   }
   catch (err)
   {
	 g_ErrorHandler.DisplayException (err);
	 return;
   }
}

// Remove an exclusion
function remove_anomaly_exclusion()
{
  try
  {
    with( document.options_othr_anomaly_excl )
    {
        // Remove all selected items from the anomaly exclusions
	    var removeItems = new Array();
	    for( i=0; i<AnomalyExclusions.options.length; i++ )
	    {
	        if( AnomalyExclusions.options[i].selected == true )
	        {
	            try
                {
	                parent.NAVOptions.RemoveAnomalyExclusionItem(i);
	                AnomalyExclusions.options[i] = null;
	                i--;
                }
                catch(err)
                {
                    parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
                    return;
                }
	        }
	    }
	    
	    // Re-set focus to the list
	    AnomalyExclusions.focus();
      
	    enable_anomaly_buttons();
	    
	 } // with( document.options_othr_anomaly_excl )
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }
}

function anomaly_initialize()
{
    try
    {
        // Enable scrolling for high-contrast mode
        if( parent.NAVOptions.IsHighContrastMode )
	        body_options_othr_threat_excl.scroll = "auto";

		// re-Initialize the list (after defaults are restored)
		document.options_othr_anomaly_excl.AnomalyExclusions.options.length = 0;

		// Set these HTML controls from the NAVOptions object
		try
		{
			var count = parent.NAVOptions.AnomalyExclusionCount;
		}
		catch(NAVErr)
		{
			parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
		}
		
		for (var i = 0; i < count; i++)
		{
			// Get the current item path and vid
			try
			{
				var itemPath = parent.NAVOptions.AnomalyExclusionItemPath(i);
				var ulVid = parent.NAVOptions.AnomalyExclusionItemVID(i);
			}
			catch(NAVErr)
			{
				parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
			}
			
			document.options_othr_anomaly_excl.AnomalyExclusions.options.length++;
			document.options_othr_anomaly_excl.AnomalyExclusions.options[i].text = itemPath;
			document.options_othr_anomaly_excl.AnomalyExclusions.options[i].value = ulVid;
		}
		
    }
    catch(err)
    {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
    }

	// Set focus to first element
	document.options_othr_anomaly_excl.AnomalyExclusions.focus();


	enable_anomaly_buttons();
}

function set_anomaly_defaults()
{
	try
	{
		parent.NAVOptions.SetAnomalyExclusionDefaults();
		this.Initialize();
	}
	catch(err)
	{
		parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
	}
}

function add_update_anomaly_exclusion()
{
  with( document.options_othr_anomaly_excl )
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
    var ItemsAdded = sww.showModalDialog('res://' + NAVPath + '\\navopts.loc/options_exclude_byvid_dlg.htm', 400, 400, null);
	delete sww;
	window.external.Global = null;
	
	if( ItemsAdded != null )
	{
	    for( i=0; i<ItemsAdded.length; i++ )
		{
		    // New items were added to the anomaly list so we need to also add these to our
		    // exclusion list
		    var index = AnomalyExclusions.options.length;
		    AnomalyExclusions.options.length++;
			AnomalyExclusions.options[index].text = ItemsAdded[i].text;
			AnomalyExclusions.options[index].value = ItemsAdded[i].value;
		}
		
		sortSelect(AnomalyExclusions);
	}

	// Re-set focus to the list
	AnomalyExclusions.focus();
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }

 } // with(document.options_othr_anomaly_excl)
}

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
		        parent.NAVOptions.SetThreatExclusionDefaults();
    	        if(parent.NAVOptions.ExclusionType == false)
	            {
	                parent.NAVOptions.SetAnomalyExclusionDefaults();
    	        }
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
	        // ExclusionType true == manual scan, ExclusionType false == security risk
	        if(parent.NAVOptions.ExclusionType)
	        {
		        parent.NAVOptions.Help(1262); // #define IDH_NAVW_OPTIONS_MANUAL_SCAN_EXCLUSIONS_ADD_EDIT_HELP_BTN 1262
	        }
	        else
	        {
		        parent.NAVOptions.Help(11708); // #define IDH_NAVW_OPTIONS_THREAT_CAT_EXCLUSIONS_HELP_BTN 11708
	        }
        }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }
	}

	// Initialize the from with the options value
	this.Initialize =  function()
	{
	    fs_initialize();
	    // ExclusionType true == viral, ExclusionType false == nonviral
	    if(parent.NAVOptions.ExclusionType == false)
	    {	    
	        anomaly_initialize();
	    }

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

function OnClickAnomalyDetails()
{
    try
    {
        with(document.options_othr_anomaly_excl)
        {
            if( AnomalyExclusions.options.selectedIndex < 0 )
                return;
                
            DisplayVIDDetails(AnomalyExclusions.options[AnomalyExclusions.selectedIndex].value);
        }
    }
    catch(err)
    {
        g_ErrorHandler.DisplayException (err);
    }
}

function OnDoubleClickAnomalyList()
{
    try
    {
        // Make sure something was double-clicked on
        if( window.event.srcElement.value == 0 )
        {
            return;
        }
        
        DisplayVIDDetails(window.event.srcElement.value);
    }
    catch(err)
    {
        g_ErrorHandler.DisplayException (err);
    }
    
    return true;
}

function DisplayVIDDetails(ulVid)
{
    try
    {
        // Get the IThreatsByVID interface from the navoptions object
        // if necessary
		if( g_ThreatsByVID == null )
		{
		    try
		    {
		        // Get the object and assing the INAVComError object to it
		        g_ThreatsByVID = parent.NAVOptions.THREATSBYVID;
		        g_ThreatsByVID.NAVERROR = parent.NAVOptions.NAVError;
		    }
		    catch(NAVerr)
	        {
	            return;
	        }
		}
		
		g_ThreatsByVID.DisplayDetails(ulVid, parent.NAVOptions.HWND);
    }
    catch(err)
    {
        g_ErrorHandler.DisplayException (err);
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
