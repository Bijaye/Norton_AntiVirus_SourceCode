// options_exclude_ap_dlg.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 103;  // This uniquely defines this script file to the error handler

// Defines for the 9x flags from SavRT32.h
var AP_EXC_VIRUS_FOUND = 1;
var AP_EXC_LOW_FORMAT  = 8;
var AP_EXC_WR_HBOOT    = 16;
var AP_EXC_WR_FBOOT    = 32;
var AP_EXC_VIRUS_FOUND_ARCHIVE  = 256;

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
        // & under 9x at least one flag checked
        OK.disabled = !document.all.Item.value.length
                   || (window.external.Global.Is9x
                    && !VirusDetection.checked
                    && !LowLevelFormat.checked
                    && !BootRecordWriteHD.checked
                    && !BootRecordWriteFloppy.checked);
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

// save options values from the screen
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
        if( Item.value.length != 0 )
            result = window.external.Global.NAVOptions.ValidateExclusionPath(Item.value);

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

		// Set the NAVOptions.APExclusionItem from the HTML controls
		with(window.external.Global.frames[1].document.all)
		{
		  try
		  {
              var flags = 0;
              var subFolders = false;
              
              if (window.external.Global.Is9x)
		      {
			    // Set Windows 9x properties
                if( VirusDetection.checked )
                    flags = flags | AP_EXC_VIRUS_FOUND;
                if( LowLevelFormat.checked )
                    flags = flags | AP_EXC_LOW_FORMAT;
                if( BootRecordWriteHD.checked )
                    flags = flags | AP_EXC_WR_HBOOT;
                if( BootRecordWriteFloppy.checked )
                    flags = flags | AP_EXC_WR_FBOOT;
		      }  // if (window.external.Global.Is9x)
		      else
		      {
		        // Set windows NT properties
			    
			    // Compressed only exclusion or all exclusions?
			    if( IncludeCompressed.checked )
			        flags = flags | AP_EXC_VIRUS_FOUND_ARCHIVE;
			    else
			        flags = flags | AP_EXC_VIRUS_FOUND;
			  }

              // Get current setting for excluding SubFolders
              if( IncludeSubfolders.checked )
                subFolders = true;

              try
              {
                  // Don't allow this if the exclusion already exists somewhere else
                  // Skip the current exclusion if editing to allow an edit on the same
                  // exclusion item flags
                  if( !window.external.Global.AddExclusion )
                  {
                      // Check for duplicates
			          for (var i = 0; i < Exclusions.options.length; i++)
			          {
			            var IsExclusionsDifferent;
						try
						{
							IsExclusionsDifferent = window.external.Global.NAVOptions.IsExclusionsDifferent(Exclusions.options[i].text, Item.value);
						}
						catch(err)
						{
							window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
							location.href = 'closeme.xyz';
							return;
						}

						if ( i != Exclusions.selectedIndex &&
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
								location.href = 'closeme.xyz';
								return;
				          }
				          sww.MsgBox(document.all.DuplicateExclusion.innerText + ' (' + Item.value + ')', document.title, 48);
				          delete sww;
				          Item.value = OldItem;
				          return;
			            }
						else if( i == Exclusions.selectedIndex && !IsExclusionsDifferent )
						{
						  try
						  {
						    // See if it's exactly the same flags in which case nothing has
						    // changed so we don't need to do any processing
                            var isExtension = window.external.Global.NAVOptions.IsExtensionExclusion(Item.value);
						    var oldItemFlags = window.external.Global.NAVOptions.APExclusionItemFlags(i);
                            var oldItemSubFolders = new Boolean(window.external.Global.NAVOptions.APExclusionItemSubFolders(i));
                            
                            // Ignore the flags if the item is an extension exclusion on 9x
                            // On NT there is the case that the compressed option was changed
                            // in which case we need to handle moving it to the new list
							if( (isExtension && window.external.Global.Is9x ) || // 9x and extension
							    (isExtension && oldItemFlags & AP_EXC_VIRUS_FOUND_ARCHIVE != flags & AP_EXC_VIRUS_FOUND_ARCHIVE) || //NT and compressed option was changed
							    (oldItemFlags == flags && oldItemSubFolders == subFolders) )
							{
								location.href = 'closeme.xyz';
								return;
							}
						  }
						  catch(err)
						  {
						    window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
							location.href = 'closeme.xyz';
							return;
						  }
						}
                     }
                  }

				  var varResult;

				   try
				   {
					  // Attempt to save this Exclusion item
					  varResult = window.external.Global.NAVOptions.SetAPExclusionItem(window.external.Global.AddSelectedIndex, // Index attempting to set 
																					   Item.value.toLowerCase(), // Path of exclusion
																						Math.abs(IncludeSubfolders.checked), // SubFolders
																						flags // Flags
																						);
				   }
				   catch(err)
				   {
						window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
						location.href = 'closeme.xyz';
						return;
				   }

                    if( !varResult ) // Failed to commit the Exclusion item
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
                    else if( varResult == 3) // Duplicate item
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
              catch( Err ) // Error setting the exclusion
              {
                  window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
			      location.href = 'closeme.xyz';
				  return;
              }

              // If adding the exclusion update the length
              if( window.external.Global.AddExclusion )
              {
                ++Exclusions.options.length;
                
                // Add the new exlusion to the UI
			    Exclusions.options[window.external.Global.AddSelectedIndex-1].text = Item.value;
              }
              else
			  {
                // Update the existing exclusion
                Exclusions.options[window.external.Global.AddSelectedIndex].text = Item.value;
			  }

              
		  }
		  catch (NAVErr)
		  {
			g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR_REINSTALL.innerText,
							document.frames("Errors").ERROR_CREATING_EXCLUSION_OBJECT_ID.innerText);
			location.href = 'closeme.xyz';
			return;
		  }
		}  // with(window.external.Global.frames[1].document.all)

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
	    }
	           
		with(window.external.Global.frames[1].document.all)
		{
		  // Set these HTML controls from the Auto-Protect Exclusions collection
		  if (!window.external.Global.AddExclusion && Exclusions.selectedIndex >= 0)
		  {
			OldItem = Item.value = Exclusions.options[Exclusions.selectedIndex].text;

            try
            {
                var Path = window.external.Global.NAVOptions.APExclusionItemPath(Exclusions.selectedIndex);
                var SubFolders = window.external.Global.NAVOptions.APExclusionItemSubFolders(Exclusions.selectedIndex);
                var flags = window.external.Global.NAVOptions.APExclusionItemFlags(Exclusions.selectedIndex);
            }
            catch(NAVErr)
            {
                window.external.Global.NAVOptions.NAVError.LogAndDisplay (0);
				location.href = 'closeme.xyz';
				return;
            }

            IncludeSubfolders.checked = SubFolders;

			if (window.external.Global.Is9x)
			{
			  // Set Windows 9x properties
			  VirusDetection.checked        = flags & AP_EXC_VIRUS_FOUND;
			  LowLevelFormat.checked        = flags & AP_EXC_LOW_FORMAT;
			  BootRecordWriteHD.checked     = flags & AP_EXC_WR_HBOOT;
			  BootRecordWriteFloppy.checked = flags & AP_EXC_WR_FBOOT;
			}
			else
			{
			  // Set Windows NT properties
			  IncludeCompressed.checked     = flags & AP_EXC_VIRUS_FOUND_ARCHIVE;
			}
		  }
		  else // This is a new exclusion item
		  {
			IncludeSubfolders.checked = true;
			VirusDetection.checked    = window.external.Global.Is9x;
		  }

		  // Hide all Win9x specific sections
		  if (!window.external.Global.Is9x)
		  {
			document.all.Win9x.style.display = 'none';
			
			// Show the NT compressed option
			document.all.NTCompressedCheckRow.style.display = '';
		  }
		  else
		  {
		    // Hide the NT section
		    document.all.NTCompressedCheckRow.style.display = 'none';
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
