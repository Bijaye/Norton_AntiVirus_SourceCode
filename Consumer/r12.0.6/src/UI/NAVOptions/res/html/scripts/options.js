// options.js: Common routines

/////////////////////////////////////////////////////
// TRACE disabled
var TRACE = false;

var axVkTrace = null;

if (TRACE)
{
	try
	{
		axVkTrace = new ActiveXObject("VkTools.VkTraceCC");
		axVkTrace.SetModuleName("NavOptJs");
	}
	catch (e)
	{
		TRACE = false;
	}
}

function trace(msg)
{
	if (TRACE)
	{
		try
		{
			axVkTrace.WriteLine(msg);
		}
		catch (e)
		{
			TRACE = false;
		}
	}
}

trace("options.js is loading...");

/////////////////////////////////////////////////////

// Scope globals
//
g_btnDefaultPage = null;
g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 100;  // This uniquely defines this script file to the error handler
var NAVAutoProtect = 0;
var NAVOptions = 0;

//Constant from NAVPwdExp.h
var NAV_CONSUMER_PASSWORD_ID = 1;

var IsTrialValid = 0;
var InitialLoadVxD = 0;
var InitialDelayLoad = 0;
var InitialALUProduct = -1;
var InitialALUVirusdefs = -1;
var ALUhosed = 0;
var InitialPassword = false;
var InitialPasswordSet = false;
var ClearPass = false;
var	IsNAVPro = 0;
var Is9x = 0;
var IsMe = 0;
var IsNT4 = 0;
var Is2K = 0;
var IsXP = 0;
var IsIE4 = 0;
var CurrentPage = 0;
var IsLoaded = 0;
var bClosedWithX = true;
var IsEmailEnabled = 0;
var IsOutgoingEmailEnabled = 0;
var g_bStopLoading = false;
var g_HWND = 0;

// constants from NAVOptions.idl
var ShowPageID_AUTOPROTECT = 1
var ShowPageID_EMAIL = 2
var ShowPageID_SPYWARE = 3
var ShowPageID_INBOUND_FIREWALL = 4
var ShowPageID_IM = 5
var ShowPageID_WORM_BLOCKING = 6
var ShowPageID_AUTO_UPDATE = 7
var ShowPageID_LIVE_UPDATE = 8


function Load ()
{
	trace("Load()");
    try
    {
        CurrentPage = new CurrentPageDummy();
        
        // Get the options object passed in
        NAVOptions = window.external.ObjectArg;
        
        document.title = document.title.replace(/%s/, NAVOptions.ProductName);

        optionsmenu.Load ();

        if( g_bStopLoading )
        {
            location.href = 'closeme.xyz';
            return;
        }

        optionsbtm.Load ();
        IsLoaded = 1;
        optionsap.Load ();
        
        switch (NAVOptions.InitialPage)
        {
			case ShowPageID_EMAIL:
                optionsmenu.RowClick(optionsmenu.int_email);
				break;
				
			case ShowPageID_SPYWARE:
                optionsmenu.RowClick(optionsmenu.other_threat);
				break;

			case ShowPageID_INBOUND_FIREWALL:
                optionsmenu.RowClick(optionsmenu.int_iwp);
				break;

			case ShowPageID_IM:
                optionsmenu.RowClick(optionsmenu.int_im);
				break;

			case ShowPageID_WORM_BLOCKING:
                optionsmenu.RowClick(optionsmenu.int_email);
				break;

			case ShowPageID_AUTO_UPDATE:
                optionsmenu.RowClick(optionsmenu.int_lu);
				break;

			case ShowPageID_LIVE_UPDATE:
                optionsmenu.RowClick(optionsmenu.int_lu);
				break;
				
			case ShowPageID_AUTOPROTECT:
			default:
				// do nothing start on ap page
				break;
        }
    }
    catch(err)
    {
        g_ErrorHandler.DisplayException(err);
    }
}

function CurrentPageDummy()
{
    try
    {
        this.Validate  = function() { return true; };
        this.Terminate = function() { return true; };
        this.Help      = function() { return true; };
    }
    catch(err)
    {
	    g_ErrorHandler.DisplayException (err);
	    return;
    }
}

// Enable the OK button only if the user typed in an extension
function OnKey()
{
  try
  {
    // if enter key was pressed click the OK button
	if (13 == window.event.keyCode)
	{
		window.event.cancelBubble = true;
	    window.event.returnValue = false;
	    if (!document.all.OK.disabled)
		  document.all.OK.onclick();
	}
	else
		setTimeout('EnableOK();', 0);
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }
}

function SelectScannerExtensionsDlg()
{
    try
    {
	    try
        {
	      // Save the current extesion list.
	      parent.OldExtensions = parent.NAVOptions.SCANNER.Ext;
	    }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }

        window.external.Global = parent;
        try
        {
            parent.sww = new ActiveXObject('CcWebWnd.ccWebWindow');
        }
        catch (NAVErr)
        {
            // Already would have given error message to user so just bail out
             window.external.Global = null;
            return;
        }
	      
        var AVPath;

        try
        {
            AVPath = parent.NAVOptions.NortonAntiVirusPath;
        }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
            parent.sww = null;
            window.external.Global = null;
            return;
        }

        var rv;
        try
        {
            parent.sww.SetIcon2('res://' + AVPath + '\\Navopts.dll/201');
            rv = parent.sww.showModalDialog('res://' + AVPath + '\\navopts.loc/options_sys_sel_ext_dlg.htm', 340, 380, null);
            parent.sww = null;
            window.external.Global = null;
        }
        catch(err)
        {
            g_ErrorHandler.DisplayException(err);
        }

        try
        {
            // if the user canceled revert to the old extensions.
            if (!rv)
                parent.NAVOptions.SCANNER.Ext = parent.OldExtensions;
        }
        catch (err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
            return;
        }
    }
    catch(err)
    {
	    g_ErrorHandler.DisplayException (err);
	    return;
    }
}

function onHelp()
{
    try
    {
        parent.NAVOptions.Help(0);
    }
    catch(err)
    {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
    }
}

function Unload()
{
	trace("Unload()");
	
    if( bClosedWithX )
    {
      try
      {
          // Check password state
          if( parent.NAVOptions.IsPasswordDirty )
          {
            // If the password is dirty we need to undirty it on a WM_QUIT
                var NAVpass = new ActiveXObject("Symantec.Norton.AntiVirus.NAVPwd");
                NAVpass.ProductID = NAV_CONSUMER_PASSWORD_ID;
                NAVpass.Enabled = InitialPassword;

                // Reset the password set on a WM_QUIT
                if( !NAVpass.Enabled && parent.InitialPasswordSet != NAVpass.UserPasswordSet )
                    NAVpass.Clear();

            }
        }
        catch(err)
        {               
		    // Already would have given error message to user so just bail out
            return;
        }
   }
}

function trim(str) 
{
	var trimvalue = "";
	strlen = str.length;
	if (strlen < 1) return trimvalue;

	i = 0;
	pos = -1;
	while (i < strlen) 
    {
		if (str.charAt(i) != ' ' && !isNaN(str.charCodeAt(i)))
        {
			pos = i;
			break;
		}
		i++;
	}

	var lastpos = -1;
	i = strlen;
	while (i >= 0)
    {
		if (str.charAt(i) != ' ' && !isNaN(str.charCodeAt(i)))
        {
			lastpos = i;
			break;
		}
		i--;
	}

	return str.substring(pos,lastpos + 1);
}
