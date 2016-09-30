////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// optionsbtm.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 126;  // This uniquely defines this script file to the error handler

// Called by options.js::Load()
function Load ()
{
    try
    {
        parent.g_btnDefaultPage = navOpts_btnDefault;
        if(parent.NAVOptions.IsHighContrastMode)
        {
            this.navOpts_btnDefaultAll.style.border = '1px solid white';
            this.navOpts_btnDefault.style.border = '1px solid white';
            this.navOpts_btnCancel.style.border = '1px solid white';
            this.navOpts_btnOK.style.border = '1px solid white';
        }
    }
    catch(err)
    {
        parent.g_ErrorHandler.DisplayException (err);
    }
}

function Save()
{
    try
    {
      // Flush changes to disk
      try
      {
        parent.NAVOptions.Save();
        
      }
      catch (NAVErr)
      {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        return;
      }
    }
    catch (err)
    {
        parent.g_ErrorHandler.DisplayException (err);
        return;
    }
}

// Save and close
function OK()
{
    try
    {
    // Update options
    parent.bClosedWithX = false;

    if (!parent.CurrentPage.Validate())
        return;

    parent.CurrentPage.Terminate();
    // Save options
    Save();
    parent.location.href = 'closeme.xyz';
  }
  catch (err)
  {
    parent.g_ErrorHandler.DisplayException (err);
    return;
  }
}

// Cancel and close
function Cancel()
{
    // It is OK to just close the dialog, because the options.js Unload() takes care of the "dirty" passwords
    parent.location.href = 'closeme.xyz';
}

function ResetPage()
{
  try
  {
    // Reset page defaults
    parent.CurrentPage.Default();
  }
  catch(NAVerr)
  {
    parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
  }
	try
	{  
    	if (parent.NAVOptions.PasswordSupported && parent.defPassSet)  // hack for options_adv_misc password
		{
			parent.CurrentPage.InitializeNoPassword(); // if default button is clicked, don't initialize the password
			return;
		}
		parent.CurrentPage.Initialize();
	}
	catch (err)
	{
		parent.g_ErrorHandler.DisplayException (err);
		return;
	}
  }

function ResetOptions()
{
    try
    {
      // Did not close with X
      parent.bClosedWithX = false;

      // In safe mode tell users we will not restore defaults
      if( parent.NAVOptions.SystemMode )
      {
            try 
            {
                var sww = new ActiveXObject('CcWebWnd.ccWebWindow');
            }
            catch (NAVErr)
            {
                parent.g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR_REINSTALL.innerText,  
                                                document.frames("Errors").ERROR_CREATING_NAVWEBWINDOW_OBJECT_ID.innerText);
                return;
            }
            
            NoDefaultsInSafeMode.innerText = NoDefaultsInSafeMode.innerText.replace(/%s/, parent.NAVOptions.ProductName);
            sww.MsgBox(document.all.NoDefaultsInSafeMode.innerText, parent.NAVOptions.ProductName, 48);
            delete sww;
            return;
      }
      
      // Fix for defect 1-4FNKOS: Warning users about deleting exclusions.
      try
      {  
        try
        {
          var sww = new ActiveXObject('CcWebWnd.ccWebWindow');
        }
        catch (NAVErr)
        {
                parent.g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR_REINSTALL.innerText,  
                                                document.frames("Errors").ERROR_CREATING_NAVWEBWINDOW_OBJECT_ID.innerText);
            return;
        }
        
        DefaultsWarning.innerText = DefaultsWarning.innerText.replace(/%s/, parent.NAVOptions.ProductName);
        var an = sww.MsgBox(document.all.DefaultsWarning.innerText, parent.NAVOptions.ProductName, 52);
        delete sww;

        if(an != 6)
        {
          return;
        }
      }
      catch(err)
      {
          parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
      }
      
      try
      {
        // Reset options defaults
        parent.NAVOptions.Default();
//      parent.NAVOptions.SetSnoozePeriod(0x1F, 0);     // Clear all.
      }
      catch (NAVErr)
      {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        
        parent.location.href = 'closeme.xyz';
        return;
      }

      // User can't enable AP in a 'Safe Mode'
      if (parent.IsTrialValid)
      {
        /*
        try
        {
            parent.NAVAutoProtect.Enabled = parent.CurAPRunning = true;
            parent.NAVAutoProtect.Configure(parent.NAVOptions.STARTUP.LoadVxD);
        }
        catch(err)
        {
            parent.NAVAutoProtect.NAVError.LogAndDisplay(parent.g_HWND);
        }
        */
        try{
            parent.NAVOptions.APEnabled = parent.CurAPRunning = true;
        
        }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }
      }
      
		if(parent.NAVOptions.PasswordSupported)
		{
			try 
			{
				var NAVPass = new ActiveXObject("Symantec.isPassword");
				try
				{
					NAVPass.ProductID = parent.NIS_CONSUMER_PASSWORD_ID;
					NAVPass.Enabled = false;
					NAVPass.Clear();
				}
				catch(err)
				{
					g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,  
														document.frames("Errors").ERROR_SETTING_PASSWORD_FEATURES_ID.innerText);
				}
			}
			catch(err)
			{
				g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR_REINSTALL.innerText,  
													document.frames("Errors").ERROR_CREATING_PASSWORD_OBJECT_ID.innerText);
			}

		}
		
      parent.location.href = 'closeme.xyz';
    }
    catch (err)
    {
        g_ErrorHandler.DisplayException (err);
        parent.location.href = 'closeme.xyz';
        return;
    }
}
