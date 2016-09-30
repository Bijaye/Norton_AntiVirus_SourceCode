////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// options_rtp_ap.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 118;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Called by options.js::Load() so the objects exist first
function Load ()
{
    if ( !parent.IsLoaded )
        return;
        try
        {
            parent.CurrentPage = new AutoProtectPage();
        }
        catch(err)
        {
            parent.g_ErrorHandler.DisplayException (err);
            return;
        }
}

function OnEnableAPClick()
{
    enable_all();
}

function OnEnableAPChanged(status)
{
    if (top.apInitialized)
        top.snoozeAP = !status;
}

// Enable / Disable
function enable_all()
{
    try
    {
            document.options_rtp_ap.ShowAutoProtectIcon.disabled =
            document.options_rtp_ap.SystemLoad.disabled =
            document.options_rtp_ap.EnableBloodhoundHeuristics.disabled = 
            document.options_rtp_ap.RbAlertRemove.disabled =
            document.options_rtp_ap.RbAlertIgnore.disabled =
            document.options_rtp_ap.RbAlertAsk.disabled =
            
        !document.Enabled || !document.options_rtp_ap.EnableAutoProtect.checked;
            
        document.options_rtp_ap.EnableAutoProtect.disabled = !document.Enabled;
        
        if(parent.NAVOptions.NAVAP_DelayLoadLock)
        {
          document.options_rtp_ap.SystemLoad.disabled = true;
        }
    }
    catch (err)
    {
        parent.g_ErrorHandler.DisplayException (err);
        return;
    }
}

// Main page object
function AutoProtectPage()
{
  try
  {
      // No page validation
      this.Validate = function() { return true; }

      // save options values from the screen
      this.Terminate = function()
      {
        try
            {
                parent.NAVOptions.APEnabled = Math.abs(document.options_rtp_ap.EnableAutoProtect.checked);
                parent.NAVOptions.AUTOPROTECT_DriversSystemStart    = Math.abs(document.options_rtp_ap.SystemLoad.checked);
                parent.NAVOptions.AUTOPROTECT_HeuristicLevel    = (document.options_rtp_ap.EnableBloodhoundHeuristics.checked) ? 2:0;
                
                    parent.CurAPRunning = Math.abs(document.options_rtp_ap.EnableAutoProtect.checked);
                
                var alertMode;
                if(document.options_rtp_ap.RbAlertRemove.checked)
                    alertMode = 0;
                else if(document.options_rtp_ap.RbAlertIgnore.checked)
                    alertMode = 1;
                else
                    alertMode = 2;
                
                parent.NAVOptions.NAVAP_ResponseMode = alertMode;
                
            //Only configure the hide icon setting if the option is available
            if( !parent.NAVOptions.NAVOPTION_HideAPIconOption )
                parent.NAVOptions.NAVAP_ShowIcon = Math.abs(document.options_rtp_ap.ShowAutoProtectIcon.checked);
            }
            catch(err)
            {
                    parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
            }    
      }

    // Provide help for this page
    this.Help = function()
    {
        try
        {
            parent.NAVOptions.Help(250100); //NAVW_ABOUT_AP_OPTIONS
        }
            catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }  
    }

      // restore default options values
      this.Default = function()
      {
        if (parent.IsTrialValid)
        {
            try
            {
                parent.NAVOptions.SetAPDefaults();
                parent.NAVOptions.AUTOPROTECT_HeuristicLevel();
                    parent.NAVOptions.NAVAP_ResponseMode();
                    
                    if( !parent.NAVOptions.NAVOPTION_HideAPIconOption )
                    parent.NAVOptions.NAVAP_ShowIcon();
            }
            catch(err)
            {
                parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
            }
            
            try
            {
                // User can't enable AP in a 'Safe Mode'
                if (!parent.NAVOptions.SystemMode && parent.IsTrialValid)
                        parent.CurAPRunning = true;
            }
            catch(err)
            {
                    parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
            } 
            } 
    }

    // Initialize the from with the options value
    this.Initialize =  function()
    {
        try
        {
        
            // Enable scrolling for high-contrast mode
            if( parent.NAVOptions.IsHighContrastMode )
                body_options_rtp_ap.scroll = "auto";

            document.options_rtp_ap.EnableAutoProtect.checked       = parent.NAVOptions.APEnabled;
            document.options_rtp_ap.ShowAutoProtectIcon.checked     = parent.NAVOptions.NAVAP_ShowIcon;
            document.options_rtp_ap.SystemLoad.checked              = parent.NAVOptions.AUTOPROTECT_DriversSystemStart;
            document.options_rtp_ap.EnableBloodhoundHeuristics.checked = (parent.NAVOptions.AUTOPROTECT_HeuristicLevel != 0);

                var alertMode = parent.NAVOptions.NAVAP_ResponseMode;
                if(alertMode == 0)
                    document.options_rtp_ap.RbAlertRemove.checked = true;
                else if(alertMode == 1)
                    document.options_rtp_ap.RbAlertIgnore.checked = true;
                else
                    document.options_rtp_ap.RbAlertAsk.checked = true;
                
            // Only set the hide icon checkbox if the option is available
            if( parent.NAVOptions.NAVOPTION_HideAPIconOption )
                document.getElementById("s_ShowAutoProtectIcon").style.display = "none";
                            
        }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }
        
        try
        {
            // User can't enable AP in a 'Safe Mode'
            if (parent.NAVOptions.SystemMode)
                document.options_rtp_ap.EnableAutoProtect.disabled = true;
        }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }

        // Set these HTML controls from the NAVOptions object
        if (parent.IsTrialValid)
        {
            document.Enabled = true;
        }
        else
        {
          // Invalid trialware. disable everything.
          document.Enabled = false;
          document.options_rtp_ap.SystemLoad.disabled = true;
          document.options_rtp_ap.EnableAutoProtect.disabled = true;
        }

        // Set focus to first element
        if (!document.options_rtp_ap.EnableAutoProtect.disabled)
            document.options_rtp_ap.EnableAutoProtect.focus();
        enable_all();
    
    }

    // Initialize this page
    this.Initialize();
    top.apInitialized = true;
  }
  catch (err)
  {
    parent.g_ErrorHandler.DisplayException (err);
    return;
  }   
}  // end of AutoProtectPage()
