// options_exclude_byvid_dlg.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 130;  // This uniquely defines this script file to the error handler

var g_ThreatsByVID = null;
var g_DetailsDisplayed = false;

function enable_buttons()
{
   try
   {
     with(document.options_exclude_byvid_dlg)
     {
	    Details.disabled = ADD.disabled = Threats.options.selectedIndex < 0;
	    
	    if( g_DetailsDisplayed )
	        Details.disabled = true;
	 }
   }
   catch (err)
   {
	 g_ErrorHandler.DisplayException (err);
	 return;
   }
}

// Initialize the list
function Initialize()
{
    try
	{
	    with(document.options_exclude_byvid_dlg)
		{
			// Set caption
			document.title = window.external.Global.NAVOptions.ProductName;
		
		    // Enable scrolling for high-contrast mode
		    if( window.external.Global.NAVOptions.IsHighContrastMode )
		        body_ed.scroll = "auto";
	    	    
		    // Get the IThreatsByVID interface from the navoptions object
            // if necessary
		    if( g_ThreatsByVID == null )
		    {
		        try
		        {
		            // Get the object and assing the INAVComError object to it
		            g_ThreatsByVID = window.external.Global.NAVOptions.THREATSBYVID;
		            g_ThreatsByVID.NAVERROR = window.external.Global.NAVOptions.NAVError;
		        }
		        catch(NAVerr)
	            {
	                window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.NAVOptions.HWND);
	                location.href = 'closeme.xyz';
	                return;
	            }
		    }

	    	Threats.options.length = 0;

            // Populate the list control with the Items in the current filter
		    if( g_ThreatsByVID != null )
		    {
		        try
		        {
		            g_ThreatsByVID.Filter = category.options(category.selectedIndex).value;
		            var count = g_ThreatsByVID.Count;
		            
		            for(i=0; i<count; i++)
		            {
		                Threats.options.length++;
		                Threats.options[i].text = g_ThreatsByVID.ThreatName(i);
		                Threats.options[i].value = g_ThreatsByVID.ThreatVID(i);
		            }
		        }
		        catch(THREATErr)
		        {
		            g_ThreatsByVID.NAVERROR.LogAndDisplay(window.external.Global.NAVOptions.HWND);
		            return;
		        }
		    }
		    
		    enable_buttons();
		}
	}
	catch(err)
    {
        g_ErrorHandler.DisplayException (err);
    }
}

function OnCategoryChange()
{
    try
    {
        if( g_ThreatsByVID == null )
            return;
            
	    with(document.options_exclude_byvid_dlg)
	    {
	        try
	        {
		        g_ThreatsByVID.Filter = category.options(category.selectedIndex).value;
		    }
		    catch(THREATErr)
		    {
		        g_ThreatsByVID.NAVERROR.LogAndDisplay(window.external.Global.NAVOptions.HWND);
		        return;
		    }
		    Initialize();
	    }
	}
	catch(err)
	{
        g_ErrorHandler.DisplayException (err);
    }
}

// Risk Details button clicked
function OnClickDetails()
{
    try
  {
      with(document.options_exclude_byvid_dlg)
	  { 
	    if( g_ThreatsByVID == null || Threats.options.selectedIndex < 0 )
            return;

        g_DetailsDisplayed = true;
        Details.disabled = true;
	    g_ThreatsByVID.DisplayDetails(Threats.options[Threats.options.selectedIndex].value, window.external.Global.NAVOptions.HWND);
	    g_DetailsDisplayed = false;
	    Details.disabled = false;
	  }
  }
  catch(err)
  {
    g_ErrorHandler.DisplayException (err);
  }
}

// Double click on a specific threat item
function get_detailed_info()
{
  try
  {
      // We only want to show one details panel at a time
      if( g_DetailsDisplayed )
        return;
      
      // Make sure something was double-clicked on
      if( window.event.srcElement.value == 0 )
      {
        return;
      }
        
      with(document.options_exclude_byvid_dlg)
	  { 
	    if( g_ThreatsByVID == null )
            return;

        g_DetailsDisplayed = true;
        Details.disabled = true;
	    g_ThreatsByVID.DisplayDetails(window.event.srcElement.value, window.external.Global.NAVOptions.HWND);
	    g_DetailsDisplayed = false;
	    Details.disabled = false;
	  }
  }
  catch(err)
  {
    g_ErrorHandler.DisplayException (err);
  }

  return true;
}

// Launch Threat Exclusions help
function Help()
{
    try
    {
        try
        {
            window.external.Global.NAVOptions.Help(250065); // IDH_NAVW_OPTIONS_THREAT_EXCLUDE_BYVID 250065
        }
        catch(err)
        {
            window.external.Global.NAVOptions.NAVERROR.LogAndDisplay(window.external.Global.NAVOptions.HWND);
        }
    }
    catch(err)
    {
        g_ErrorHandler.DisplayException (err);
    }
}

// save options values from the screen
function Save()
{
  try
  {
	  with(document.options_exclude_byvid_dlg)
	  {
	    // Add all selected items to the user's list of exclusions
	    var count = Threats.options.length;
	    var newItems = new Array();
	    for( i=0; i<count; i++ )
	    {
	        if( Threats.options[i].selected == true )
	        {
	            try
	            {
	                var result = window.external.Global.NAVOptions.SetAnomalyExclusionItem(Threats.options[i].value, Threats.options[i].text, false );
	            }
	            catch(NAVerr)
	            {
	                window.external.Global.NAVOptions.NAVERROR.LogAndDisplay(window.external.Global.NAVOptions.HWND);
	                location.href = 'closeme.xyz';
	                return;
	            }
	            
	            if( result == 1) // new item
	            {
	                newItems.push(Threats.options[i]);
	            }
	        }
	    }
	    
	    window.returnValue = newItems;
	  }

	  location.href = 'closeme.xyz';
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	location.href = 'closeme.xyz';
	return;
  }
}