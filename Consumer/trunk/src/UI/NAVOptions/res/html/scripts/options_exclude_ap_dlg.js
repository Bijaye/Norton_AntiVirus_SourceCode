////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// options_exclude_ap_dlg.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 103;  // This uniquely defines this script file to the error handler

var OldItem = '';

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Enable/Disable OK button
function EnableOK()
{
  try
  {
      with(document.options_exclude_ap_dlg)
      {
        // A valid entry must at least one charecter
        // 
        if (document.all.Item.value.length)
        {
			ButtonEnable(OK);
        }
        else
        {
			ButtonDisable(OK);
        }
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
		with(document.options_exclude_ap_dlg)
		{
			Item.value = window.external.Global.NAVOptions.Browse(3, Item.value);
			EnableOK();
		}
	}
	catch(err)
	{
		window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
	}

  return false;
}

// Launch AP Exclusions help
function Help()
{
    try
    {
        window.external.Global.NAVOptions.Help(1261); // IDH_NAVW_ADD_EXCLUSION_DLG_HELP_BTN
    }
    catch(err)
    {
        window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
    }
}

// save	options	values from	the	screen
function Save()
{
    try
    {
        with(document.options_exclude_ap_dlg)
        {
            // Match only a valid path

            // Remove leading and trailing white space
            Item.value = trim(Item.value);

            var result = 0;
            if(Item.value.length != 0)
                result = window.external.Global.NAVOptions.ValidateExclusionPath(Item.value);

            // Check for an empty exclusion item or an invalid one
            if(Item.value.length == 0 || !result)
            {
                try
                {
                    var sww = new ActiveXObject('CcWebWnd.ccWebWindow');
                }
                catch (NAVErr)
                {
                    g_ErrorHandler.DisplayNAVError(document.frames("Errors").ERROR_INTERNAL_ERROR_REINSTALL.innerText,
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

            // Now check for a path with wildcard characters in it which are
            // no longer supported
            if(2 == result)
            {
                try
                {
                    var sww = new ActiveXObject('CcWebWnd.ccWebWindow');
                }
                catch (NAVErr)
                {
                    g_ErrorHandler.DisplayNAVError(document.frames("Errors").ERROR_INTERNAL_ERROR_REINSTALL.innerText,
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
            if(3 == result)
            {
                try
                {
                    var sww = new ActiveXObject('CcWebWnd.ccWebWindow');
                }
                catch (NAVErr)
                {
                    g_ErrorHandler.DisplayNAVError(document.frames("Errors").ERROR_INTERNAL_ERROR_REINSTALL.innerText,
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

            // Perform an add or edit.
            with(window.external.Global.frames[2].document.all)
            {
                var idxEditUpLow;
                var idxEditUpHigh;

                try
                {
                    if( !window.external.Global.AddExclusion )
                    {
                        // ON EDIT

                        // Check if either text or checkbox is dirty. Bail out if not.
                        var isDirty = 0;

                        if(AP_Exclusions.options[AP_Exclusions.selectedIndex].text != Item.value)
                            isDirty++;

                        if((!window.external.Global.NAVOptions.APExclusionSubFolder(AP_Exclusions.selectedIndex) &&  IncludeSubfolders.checked) ||
                           ( window.external.Global.NAVOptions.APExclusionSubFolder(AP_Exclusions.selectedIndex) && !IncludeSubfolders.checked))
                            isDirty++;

                        if(!isDirty)
                        {
                            location.href = 'closeme.xyz';
                            return;
                        }

                        // Apply changes to exclusion item.
                        var editResult;

                        try
                        {
                            editResult = window.external.Global.NAVOptions.EditAPExclusion(
                                AP_Exclusions.selectedIndex, // Index of exclusion
                                Item.value.toLowerCase(), // Path of exclusion
                                Math.abs(IncludeSubfolders.checked) // SubFolders
                                );
                        }
                        catch(err)
                        {
                            window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
                            location.href = 'closeme.xyz';
                            return;
                        }

                        // Process result from applying exclusion item changes.
                        if(editResult == -1)
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
                            sww.MsgBox(document.all.DuplicateExclusion.innerText + ' (' + Item.value + ')', document.title, 48);
                            delete sww;
                            return;
                        }
                        else if(editResult < 0)
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
                            sww.MsgBox(document.all.ExclusionError.innerText + ' (' + Item.value + ')', document.title, 48);
                            delete sww;
                            return;
                        }

                        // Determine range for updating items.
                        if (AP_Exclusions.selectedIndex < editResult)
                        {
                            idxEditUpLow = AP_Exclusions.selectedIndex;
                            idxEditUpHigh = editResult;
                        }
                        else
                        {
                            idxEditUpLow = editResult;
                            idxEditUpHigh = AP_Exclusions.selectedIndex;
                        }
                    }
                    else
                    {
                        // ON ADD

                        // Apply changes to exclusion item.
                        var varResult;

                        try
                        {
                            varResult = window.external.Global.NAVOptions.AddAPExclusion(
                                Item.value.toLowerCase(), // Path of exclusion
                                Math.abs(IncludeSubfolders.checked) // SubFolders
                                );
                        }
                        catch(err)
                        {
                            window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
                            location.href = 'closeme.xyz';
                            return;
                        }

                        if(!varResult) // Failed to commit the Exclusion item
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
                            sww.MsgBox(document.all.ExclusionError.innerText + ' (' + Item.value + ')', document.title, 48);
                            delete sww;
                            return;
                        }
                        else if(varResult == 3) // Duplicate item
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
                            sww.MsgBox(document.all.DuplicateExclusion.innerText + ' (' + Item.value + ')', document.title, 48);
                            delete sww;
                            return;
                        }
                    }
                }
                catch( Err ) // Error setting the exclusion
                {
                    window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
                    location.href = 'closeme.xyz';
                    return;
                }

                if( window.external.Global.AddExclusion )
                {
                    // If adding the exclusion...

                    // ...update the length
                    ++AP_Exclusions.options.length;

                    // ...add the new exlusion to the UI
                    AP_Exclusions.options[window.external.Global.AddSelectedIndex-1].text = Item.value;
                }
                else
                {
                    // If editing the exclusion...

                    // ...update the UI (just the range of shifted items)
                    for (var idx = idxEditUpLow; idx <= idxEditUpHigh; idx++)
                        AP_Exclusions.options[idx].text = window.external.Global.NAVOptions.APExclusionPath(idx);

                    // ...select the item that we just edited, as its index might have changed
                    AP_Exclusions.selectedIndex = editResult;
                }
            } // with(window.external.Global.frames[1].document.all)

            window.returnValue = Item.value;

        }  // with(document.options_exclude_ap_dlg)

        location.href = 'closeme.xyz';
    }
    catch (err)
    {
        g_ErrorHandler.DisplayException (err);
        location.href = 'closeme.xyz';
        return;
    }
}

// Initialize the form with the options value
function initialize_options_exclude_ap_dlg()
{
  try
  {
	  with(document.options_exclude_ap_dlg)
	  {
		// Set caption
		document.title = window.external.Global.NAVOptions.ProductName;
		
	    // Enable scrolling for high-contrast mode
	    // Fix for defect 1-4AROGV. Need to shrink input in high contrast mode.
	    if( window.external.Global.NAVOptions.IsHighContrastMode )
	    {
	        body_ed.scroll = "auto";
	        Item.size = Item.size - 4;
	        document.options_exclude_ap_dlg.OK.style.border = '1px solid white';
	        document.options_exclude_ap_dlg.Cancel.style.border = '1px solid white';
	    }
	           
		with(window.external.Global.frames[2].document.all)
		{
		  // Set these HTML controls from the Auto-Protect Exclusions collection
		  if (!window.external.Global.AddExclusion && AP_Exclusions.selectedIndex >= 0)
		  {
			OldItem = Item.value = AP_Exclusions.options[AP_Exclusions.selectedIndex].text;

            	try
            	{
                	var Path = window.external.Global.NAVOptions.APExclusionPath(AP_Exclusions.selectedIndex);
                	var SubFolders = window.external.Global.NAVOptions.APExclusionSubFolder(AP_Exclusions.selectedIndex);

            	}
            	catch(NAVErr)
            	{
                	window.external.Global.NAVOptions.NAVError.LogAndDisplay (0);
				location.href = 'closeme.xyz';
				return;
            	}

            	IncludeSubfolders.checked = SubFolders;
		  }
		  else // This is a new exclusion item
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
	location.href = 'closeme.xyz';
	return;
  }
}