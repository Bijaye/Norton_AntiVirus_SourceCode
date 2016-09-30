// options_sys_sel_ext_dlg.js
// This is the dialog for the Manual Scanner extension inclusion list

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 125;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Enable / Disable the buttons
function enable_buttons()
{
  try
  {
	with(document.options_sys_sel_ext_dlg)
	{
		Remove.disabled = Extensions.options.selectedIndex < 0;
	}
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }  	
}

function Help()
{
    try
    {
        window.external.Global.NAVOptions.Help(1325); /* IDH_NAVW_WHATHIS_FILE_EXTENSIONS_LIST_DLG_HELP_BTN */
    }
    catch(err)
    {
        window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
    }
}

// Add an extension
function add_extension()
{
  try
  {
	with(document.options_sys_sel_ext_dlg)
	{
        var Ext;

        try
        {
            Ext = window.external.Global.NAVOptions.SCANNER.Ext;
        }
        catch(err)
        {
            window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
            location.href = 'closeme.xyz';
            return;
        }

		window.external.Global.Extensions = Extensions;
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

		try
		{
			var NAVPath;

            try
            {
                NAVPath = window.external.Global.NAVOptions.NortonAntiVirusPath;
            }
            catch(err)
            {
                window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
                delete sww;
                return;
            }

            sww.SetIcon2('res://' + NAVPath + '\\Navopts.dll/201');
            ret = sww.showModalDialog('res://' + NAVPath + '\\navopts.loc/options_new_ext_dlg.htm', 350, 160, null);
		}
		catch (NAVErr)
		{
		  g_ErrorHandler.DisplayNAVError (document.frames("Errors").document.all.ERROR_INTERNAL_ERROR.innerText,
										  document.frames("Errors").document.all.ERROR_SHOWING_EXTENSION_DIALOG_ID.innerText);	
		}

		delete sww;
		if (null != ret && '' != ret)
		{
		  // Check for buffer overflow.
		  if (2048 <= (Ext.length + ret.length))
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
		    
			sww.MsgBox(document.all.TooManyExt.innerText, document.title, 0);
			delete sww;			
			return;
		  }
		  
		  // Make the extension upper case
		  ret = ret.toUpperCase();

          try
          {
              // If the original list is not empty add a space to the end
              if( Ext.length != 0 )
              {
                Ext += ' ';
              }

		      // Add the '.' at the begining if needed
		      if (ret.indexOf('.'))
		      {
		        Ext += window.external.Global.NAVOptions.ANSI2OEM(ret);
			    ret = '.' + ret;
		      }
		      else
		      {
		        Ext += window.external.Global.NAVOptions.ANSI2OEM(ret.slice(1));
		      }
		      
		      // Save the list
		      Ext = Ext.split(' ');
		      Ext.sort();
              window.external.Global.NAVOptions.SCANNER.Ext = Ext.join(' ');
          }
          catch(err)
          {
            window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
            return;
          }

		  Extensions.options[++Extensions.options.length - 1].text = ret;
		}

		// Set focus to first element
		Extensions.focus();
	}
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }
 
  enable_buttons();
}

// Remove an extension
function remove_extension()
{
  try
  {
	with(document.options_sys_sel_ext_dlg)
	{
        var selectedIndex = Extensions.selectedIndex;
        var extensions;

        try
        {
		    // Remove the extension from the list
		    extensions = window.external.Global.NAVOptions.SCANNER.Ext.split(' ');
		    
		    for (var i = 0; extensions[i]; i++)
		    {
		        if( Extensions.options[selectedIndex].text.slice(1) == extensions[i] )
		        {
		            a = extensions.slice(0, i);
		            b = extensions.slice(i + 1);
		            window.external.Global.NAVOptions.SCANNER.Ext = a.concat(b).join(' ');
		        }
		    }
        }
        catch(err)
        {
            window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
            return;
        }

		// Remove it also from the listbox
		Extensions.options[selectedIndex] = null;

		// Set focus to first element
		if (Extensions.options.length)
		{
		  Extensions.focus();

		  if (selectedIndex >= Extensions.options.length)
			selectedIndex--;
		  Extensions.options[selectedIndex].selected = true;
		}
		else
		  OK.focus();
	}
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  } 
  
  enable_buttons();
}

// Reset to default extension list
function default_extension()
{
  try
  {
	try
    {
        window.external.Global.NAVOptions.SCANNER.Ext();
    }
    catch(err)
    {
        window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
        return;
    }

	document.options_sys_sel_ext_dlg.Extensions.options.length = 0;
	initialize_options_sys_sel_ext_dlg();
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  } 
}

// Initialize the form with the options value
function initialize_options_sys_sel_ext_dlg()
{
  try
  {
    // Set caption
	document.title = window.external.Global.NAVOptions.ProductName;
	
	// Enable scrolling for high-contrast mode
    if( window.external.Global.NAVOptions.IsHighContrastMode )
	    body_sys_sel_ext_dlg.scroll = "auto";

	with(document.options_sys_sel_ext_dlg)
	{
		if ('he' == navigator.systemLanguage)
		{
		  // Prevent crash after adding an extesion
		  // & saving options on Hebrew Win98, IE4;SP1
		  Extensions.style.fontFamily = '';
		}

		// Set these HTML controls from the NAVOptions object
		// Convert extension names to ANSI
        var varANSI;

        try
        {
		    varANSI = window.external.Global.NAVOptions.OEM2ANSI(window.external.Global.NAVOptions.SCANNER.Ext);
        }
        catch(err)
        {
            window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
            location.href = 'closeme.xyz';
            return;
        }

		var extensions = varANSI.split(' ');

		for (var i = 0; extensions[i]; i++)
		{
			Extensions.options.length++;
			Extensions.options[i].text = "." + extensions[i].toUpperCase();
		}

		// Set focus to first element
		Extensions.focus();
	}
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }
  enable_buttons();
}
