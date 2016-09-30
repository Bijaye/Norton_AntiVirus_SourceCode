// options_new_ext_dlg.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 112;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Enable/Disable OK button
function EnableOK()
{
  document.all.OK.disabled = !document.all.Extension.value.length;

  return true;
}

function Help()
{
    try
    {
        window.external.Global.NAVOptions.Help(475); /* IDH_NAVW_NEW_FILE_EXTENSIONS_DLG_HELP_BTN */
    }
    catch(err)
    {
        window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
    }
}
// Validate and save
function Save()
{
  try
  {
	// Must be '.xxx' format
	var re = /^\.{0,1}[^.><"'|:\\ ]{1,255}$/;

	with(document.options_new_ext_dlg)
	{
		if (!re.test(Extension.value))
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

		  sww.MsgBox(document.all.InvalidExt.innerText + ' (' + Extension.value + ')', document.title, 0);
		  delete sww;      
		  return;
		}

		// Check that this is not a duplicate
		for (var i = 0; i < window.external.Global.Extensions.length; i++)
		{
		  var ext = window.external.Global.Extensions.options[i];

		  // Normelize the format of the extension
		  if ('.' != Extension.value.charAt(0))
			Extension.value = '.' + Extension.value;

		  if (Extension.value.toUpperCase() == ext.text)
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
			sww.MsgBox(document.all.DuplicateExt.innerText + ' (' + Extension.value + ')', document.title, 0);
			delete sww;        
			return;
		  }
		}

		window.returnValue = Extension.value;
	}

	location.href = 'closeme.xyz';
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }
}

// Initialize the from with the options value
function initialize_options_new_ext_dlg()
{
  try
  {
	with(document.options_new_ext_dlg)
	{
		// Set caption
		document.title = window.external.Global.NAVOptions.ProductName;
		
	    // Enable scrolling for high-contrast mode
        if( window.external.Global.NAVOptions.IsHighContrastMode )
	        body_extdlg.scroll = "auto";
	        
		// Set these HTML controls from the NAVOptions object
		OK.disabled = '' == Extension.value;

		// Set focus to first element
		Extension.focus();
	}
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }
}
