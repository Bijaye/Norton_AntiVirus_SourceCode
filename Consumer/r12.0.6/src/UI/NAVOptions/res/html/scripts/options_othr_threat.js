// options_othr_threat.js

g_ModuleID = 3023;
g_ScriptID = 115;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Main page object
function GeneralPage()
{
  // No page validation
  this.Validate = function() { return true; }

  // save options values from the screen
  this.Terminate = function()
  {
    try
    {
        if( parent.IsTrialValid )
        {
            // Set the NAVOptions object from the HTML controls
            parent.NAVOptions.THREAT.Threat10 = Math.abs(document.options_othr_threat.Adware.checked);
            parent.NAVOptions.THREAT.Threat8 = Math.abs(document.options_othr_threat.Dialer.checked);
            parent.NAVOptions.THREAT.Threat5 = Math.abs(document.options_othr_threat.HackerTool.checked);
            parent.NAVOptions.THREAT.Threat9 = Math.abs(document.options_othr_threat.RemoteAccess.checked);
            parent.NAVOptions.THREAT.Threat4 = Math.abs(document.options_othr_threat.SecurityRisk.checked);
            parent.NAVOptions.THREAT.Threat6 = Math.abs(document.options_othr_threat.Spyware.checked);
            parent.NAVOptions.THREAT.Threat11 = Math.abs(document.options_othr_threat.Joke.checked);
            parent.NAVOptions.THREAT.Threat7 = Math.abs(document.options_othr_threat.Trackware.checked);
        }
    }
    catch(err)
    {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
    }
  }  // End of Terminate()

  // Provide help for this page
  this.Help = function()
  {
    try
    {
        parent.NAVOptions.Help(11706); // #define IDH_NAVW_OPTIONS_THREAT_CAT_MAIN_HELP_BTN	11706
    }
    catch(err)
    {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
    }
  }

  // restore default options values
  this.Default = function()
  {
    try
    {
        if( parent.IsTrialValid )
        {
            parent.NAVOptions.THREAT.Threat10();
            parent.NAVOptions.THREAT.Threat8();
            parent.NAVOptions.THREAT.Threat5();
            parent.NAVOptions.THREAT.Threat9();
            parent.NAVOptions.THREAT.Threat4();
            parent.NAVOptions.THREAT.Threat6();
            parent.NAVOptions.THREAT.Threat11();
            parent.NAVOptions.THREAT.Threat7();
        }
    }
    catch(err)
    {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
    }
  }  // End of Default()

  // Initialize the from with the options value
  this.Initialize =  function()
  {  
    try
    {
        // Enable scrolling for high-contrast mode
        if( parent.NAVOptions.IsHighContrastMode )
	        body_threat.scroll = "auto";
	            
        if( parent.IsTrialValid )
        {
            // Set these HTML controls from the NAVOptions object
            document.options_othr_threat.Adware.checked = parent.NAVOptions.THREAT.Threat10;
            document.options_othr_threat.Dialer.checked = parent.NAVOptions.THREAT.Threat8;
            document.options_othr_threat.HackerTool.checked = parent.NAVOptions.THREAT.Threat5;
            document.options_othr_threat.RemoteAccess.checked = parent.NAVOptions.THREAT.Threat9;
            document.options_othr_threat.SecurityRisk.checked = parent.NAVOptions.THREAT.Threat4;
            document.options_othr_threat.Spyware.checked = parent.NAVOptions.THREAT.Threat6;
            document.options_othr_threat.Joke.checked = parent.NAVOptions.THREAT.Threat11;
            document.options_othr_threat.Trackware.checked = parent.NAVOptions.THREAT.Threat7;
            
            // Set focus to first element
            document.options_othr_threat.SecurityRisk.focus();
        }
        else // Invalid TrialWare
        {
            document.options_othr_threat.Adware.disabled =
            document.options_othr_threat.Dialer.disabled =
            document.options_othr_threat.HackerTool.disabled = 
            document.options_othr_threat.RemoteAccess.disabled =
            document.options_othr_threat.SecurityRisk.disabled =
            document.options_othr_threat.Spyware.disabled =
            document.options_othr_threat.Trackware.disabled =
            document.options_othr_threat.Joke.disabled = true;
        }
    }
    catch(err)
    {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
    }
  }  // end of Initialize()

  // Initialize this page
  this.Initialize();
}  // end function GeneralPage()
