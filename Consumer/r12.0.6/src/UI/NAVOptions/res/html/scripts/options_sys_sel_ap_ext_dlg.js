// options_sys_sel_ap_ext_dlg.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 124;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Enable / Disable the buttons
function enable_buttons()
{
  try
  {
	with(document.options_sys_sel_ap_ext_dlg)
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
	with(document.options_sys_sel_ap_ext_dlg)
	{
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

        var NAVPath;
		try
        {
            NAVPath = window.external.Global.NAVOptions.NortonAntiVirusPath;
        }
        catch(err)
        {
            delete sww;
            window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
            return;
        }

        sww.SetIcon2('res://' + NAVPath + '\\Navopts.dll/201');
        ret = sww.showModalDialog('res://' + NAVPath + '\\navopts.loc/options_new_ext_dlg.htm', 350, 160, null);
		delete sww;

        // This will store the current extension list in the options store
        var APExtList;

        try
        {
            APExtList = window.external.Global.NAVOptions.APExtensionList;
        }
        catch(err)
        {
            window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
        }
        
		if (null != ret && '' != ret)
		{
		  // Check for buffer overflow.
		  if (2046 <= (APExtList.length + ret.length))
		  {
			try
			{
			  var sww = new ActiveXObject('CcWebWnd.ccWebWindow');
			}
			catch (NAVErr)
			{
			  g_ErrorHandler.DisplayNAVError (document.frames("Errors").document.all.ERROR_INTERNAL_ERROR_REINSTALL.innerText,  document.frames("Errors").document.all.ERROR_CREATING_NAVWEBWINDOW_OBJECT_ID.innerText);			  
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
              if( APExtList.length )
                APExtList += ' ';
                
		      // Add the '.' at the begining if needed
		      if (ret.indexOf('.'))
		      {
		        APExtList += window.external.Global.NAVOptions.ANSI2OEM(ret);
			    ret = '.' + ret;
		      }
		      else
		      {
			    APExtList += window.external.Global.NAVOptions.ANSI2OEM(ret.slice(1));
		      }

              // Store the list
              APExtList = APExtList.split(' ');
		      APExtList.sort();
              window.external.Global.NAVOptions.APExtensionList = APExtList.join(' ');
           }
           catch(err)
           {
              window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
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
	with(document.options_sys_sel_ap_ext_dlg)
	{
	    var selectedIndex = Extensions.selectedIndex;
        var extensions;

        try
        {
            // Remove the extension from the list
		    extensions = window.external.Global.NAVOptions.APExtensionList.split(' ');
		    
		    for (var i = 0; extensions[i]; i++)
		    {
		        if( Extensions.options[selectedIndex].text.slice(1) == extensions[i] )
		        {
		            a = extensions.slice(0, i);
		            b = extensions.slice(i + 1);
		            window.external.Global.NAVOptions.APExtensionList = a.concat(b).join(' ');
		        }
		    }
         }
         catch(err)
         {
            window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
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
	    window.external.Global.NAVOptions.AP.Ext();
    }
    catch(err)
    {
        window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
    }

	document.options_sys_sel_ap_ext_dlg.Extensions.options.length = 0;
	initialize_options_sys_sel_ap_ext_dlg();
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  } 
}

// Initialize the from with the options value
function initialize_options_sys_sel_ap_ext_dlg()
{
  try
  {
    // Set caption
	document.title = window.external.Global.NAVOptions.ProductName;
	
    // Enable scrolling for high-contrast mode
    if( window.external.Global.NAVOptions.IsHighContrastMode )
	    body_sys_sel_ap_ext_dlg.scroll = "auto";
	                    
	// Set the window height to display the whole form.
	// (changes with the screen font size)
	var height = document.body.scrollHeight + (document.body.offsetHeight - document.body.clientHeight);;
	//window.external.Global.sww.Height = height + 20;

	with(document.options_sys_sel_ap_ext_dlg)
	{
		if ('he' == navigator.systemLanguage)
		{
		  // Prevent crash after adding an extesion
		  // & saving options on Hebrew Win98, IE4;SP1
		  Extensions.style.fontFamily = '';
		}

        try
        {
		    // Set these HTML controls from the NAVOptions object in ANSI format
		    var varANSI = window.external.Global.NAVOptions.OEM2ANSI(window.external.Global.NAVOptions.APExtensionList);
		    var extensions = varANSI.split(' ');

		    for (var i = 0; extensions[i]; i++)
		    {
			    Extensions.options.length++;
			    Extensions.options[i].text = "." + extensions[i].toUpperCase();
		    }
        }
        catch(err)
        {
            window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
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
