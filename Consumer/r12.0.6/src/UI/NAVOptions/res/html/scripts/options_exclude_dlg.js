// options_exclude_dlg.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 104;  // This uniquely defines this script file to the error handler

var OldItem = '';

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Enable/Disable OK button
function EnableOK()
{
  try
  {
      with(document.options_exclude_dlg)
      {
        // A valid entry must at least one charecter
        OK.disabled = !document.all.Item.value.length;
      }
  }
  catch(err)
  {
    g_ErrorHandler.DisplayException (err);
  }

  return true;
}

// Browse disk for excluded files
function BrowseExclusions()
{
  try
  {
	with(document.options_exclude_dlg)
	{
		Item.value = window.external.Global.NAVOptions.Browse(3, Item.value);
		EnableOK();
	}
  }
  catch (err)
  {
	window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
	return;
  }

  return false;
}

// Launch the manual scan help
function Help()
{
    try
    {
        window.external.Global.NAVOptions.Help(1262); // IDH_NAVW_ADD_EXCLUSION_DLG_HELP_BTN for Manual scanner
    }
    catch(err)
    {
        window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
    }
}

// save options values from the screen
function Save()
{
  try
  {

	  with(document.options_exclude_dlg)
	  {
		// Match only a valid path

        // Remove leading and trailing white space
        Item.value = trim(Item.value);

        var result = 0;
        if( Item.value.length != 0 )
        try
        {
            var result = window.external.Global.NAVOptions.EXCLUSIONS.Validate2(Item.value);
        }
        catch(err)
        {
            window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
        }

        // Check for an empty exclusion item or an invalid one
		if (Item.value.length == 0 || !result)
		{
		  try
		  {
			var sww = new ActiveXObject('CcWebWnd.ccWebWindow');
		  }
		  catch (NAVErr)
		  {
			g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR_REINSTALL.innerText,
											document.frames("Errors").ERROR_CREATING_NAVWEBWINDOW_OBJECT_ID.innerText);
			location.href = 'closeme.xyz';
			return;
		  }

		  sww.MsgBox(document.all.IlegalFile.innerText + ' (' + Item.value + ')', document.title, 48);
		  delete sww;
		  Item.value = OldItem;
		  EnableOK();
		  return;
		}
		
		// If this is an extension exlusion make sure it has a '*' prepending it
        if( window.external.Global.NAVOptions.IsExtensionExclusion(Item.value) )
        {
            if( Item.value.charAt(0) != '*' )
                Item.value = "*" + Item.value;
        }

        // Now check for a path with wildcard characters in it which are
        // no longer supported
        if( 2 == result )
        {
            try
		  {
			var sww = new ActiveXObject('CcWebWnd.ccWebWindow');
		  }
		  catch (NAVErr)
		  {
			g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR_REINSTALL.innerText,
											document.frames("Errors").ERROR_CREATING_NAVWEBWINDOW_OBJECT_ID.innerText);
			location.href = 'closeme.xyz';
			return;
		  }

		  sww.MsgBox(document.all.WildCardInPath.innerText, document.title, 48);
		  delete sww;
		  Item.value = OldItem;
		  EnableOK();
		  return;
        }

        // Also deny the *.* extension exlusion
        if( 3 == result )
        {
            try
		  {
			var sww = new ActiveXObject('CcWebWnd.ccWebWindow');
		  }
		  catch (NAVErr)
		  {
			g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR_REINSTALL.innerText,
											document.frames("Errors").ERROR_CREATING_NAVWEBWINDOW_OBJECT_ID.innerText);
			location.href = 'closeme.xyz';
			return;
		  }

		  sww.MsgBox(document.all.AllPathsError.innerText, document.title, 48);
		  delete sww;
		  Item.value = OldItem;
		  EnableOK();
		  return;
        }

		// Set the NAVOptions.EXCLUSIONS collection from the HTML controls
		with(window.external.Global.frames[1].document.all)
		{
            var varExclusion;

            if (window.external.Global.AddExclusion)
            {
	            try
                {
                    varExclusion = new ActiveXObject('Symantec.Norton.Antivirus.Exclusion');
                }
                catch(err)
                {
		            g_ErrorHandler.DisplayNAVError (document.frames("Errors").document.all.ERROR_INTERNAL_ERROR_REINSTALL.innerText,  
										            document.frames("Errors").document.all.ERROR_CREATING_EXCLUSION_OBJECT_ID.innerText);
		            return;
                }
            }
            else  // if (window.external.Global.AddExclusion)
            {
	            try
                {
                    varExclusion = window.external.Global.NAVOptions.EXCLUSIONS(Exclusions.selectedIndex);
                }
                catch(err)
                {
                    window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
                    return;
                }
            }

		  // Set the exclusion properties
		  varExclusion.Path       = Item.value.toLowerCase();
		  varExclusion.SubFolders = Math.abs(IncludeSubfolders.checked);
		  varExclusion.Viruses = 1;  // always checked for manual scans

        // Check for duplicates
		for (var i = 0; i < Exclusions.options.length; i++)
		{
            var IsExclusionsDifferent;

            try
            {
            IsExclusionsDifferent = window.external.Global.NAVOptions.EXCLUSIONS.IsExclusionsDifferent(Exclusions.options[i].text, Item.value);
            }
            catch(err)
            {
            window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
            }

			if (i != Exclusions.selectedIndex && 
			    !IsExclusionsDifferent)
			{
			try
			{
				var sww = new ActiveXObject('CcWebWnd.ccWebWindow');
			}			
			catch (NAVErr)
			{
				g_ErrorHandler.DisplayNAVError (document.frames("Errors").document.all.ERROR_INTERNAL_ERROR_REINSTALL.innerText,  
												document.frames("Errors").document.all.ERROR_CREATING_NAVWEBWINDOW_OBJECT_ID.innerText);
				return;
			}
			sww.MsgBox(document.all.DuplicateExclusion.innerText + ' (' + Item.value + ')', document.title, 48);
			delete sww;
			Item.value = OldItem;
			return;
			}
		}
			
		  // Change the parent page and the collection
		  if (window.external.Global.AddExclusion)
		  {
		    try
		    {
		        varResult = window.external.Global.NAVOptions.EXCLUSIONS.Add(varExclusion)
		    }
		    catch(err)
		    {
		        window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
                return;
		    }
		    // Check for failure to add
		    if( 0 == varResult )
		    {
		        try
			    {
				    var sww = new ActiveXObject('CcWebWnd.ccWebWindow');
			    }
			    catch (NAVErr)
			    {
				    g_ErrorHandler.DisplayNAVError (document.frames("Errors").document.all.ERROR_INTERNAL_ERROR_REINSTALL.innerText,  
												    document.frames("Errors").document.all.ERROR_CREATING_NAVWEBWINDOW_OBJECT_ID.innerText);
				    return;
			    }
			    sww.MsgBox(document.all.ExclusionError.innerText + ' (' + Item.value + ')', document.title, 48);
			    delete sww;
			    return;
		    }
		    // Check for duplicate exclusion item
			else if (3 == varResult)
			{
			  try
			  {
				var sww = new ActiveXObject('CcWebWnd.ccWebWindow');
			  }
			  catch (NAVErr)
			  {
				g_ErrorHandler.DisplayNAVError (document.frames("Errors").document.all.ERROR_INTERNAL_ERROR_REINSTALL.innerText,  
												document.frames("Errors").document.all.ERROR_CREATING_NAVWEBWINDOW_OBJECT_ID.innerText);
				return;
			  }
			  sww.MsgBox(document.all.DuplicateExclusion.innerText + ' (' + Item.value + ')', document.title, 48);
			  delete sww;
			  return;
			}

			// Add the new exlusion
			Exclusions.options[++Exclusions.options.length - 1].text = Item.value;
		  }
		  else  // if (window.external.Global.AddExclusion)
		  {
			// Change the exlusion
			Exclusions.options[Exclusions.selectedIndex].text = Item.value;
            try
            {
			    window.external.Global.NAVOptions.EXCLUSIONS.Item(Exclusions.selectedIndex) = varExclusion;
            }
            catch(err)
            {
                window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
            }
		  }
		}  // with(window.external.Global.frames[1].document.all)

		window.returnValue = Item.value;
	  }  // with(document.options_exclude_dlg)

	  location.href = 'closeme.xyz';
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }
}

// Initialize the from with the options value
function initialize_options_exclude_dlg()
{
  try
  {
	  with(document.options_exclude_dlg)
	  {
		// Set caption
		document.title = window.external.Global.NAVOptions.ProductName;
			
	    // Enable scrolling for high-contrast mode
	    if( window.external.Global.NAVOptions.IsHighContrastMode )
	        body_ed.scroll = "auto";
	        
		with(window.external.Global.frames[1].document.all)
		{
		  // Set these HTML controls from the NAVOptions.EXCLUSIONS collection
		  if (!window.external.Global.AddExclusion && Exclusions.selectedIndex >= 0)
		  {
			OldItem = Item.value = Exclusions.options[Exclusions.selectedIndex].text;

			var varExclusion;
            
            try
            {
                varExclusion = window.external.Global.NAVOptions.EXCLUSIONS(Exclusions.selectedIndex);
            }
            catch(err)
            {
                window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
            }

			IncludeSubfolders.checked = varExclusion.SubFolders;
		  }
		  else
		  {
			IncludeSubfolders.checked = true;
		  }

		  // Set focus to first element
		  Item.focus();

		  EnableOK();
		}
	  }
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }
}
