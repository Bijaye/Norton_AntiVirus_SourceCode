// optionsbtm.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 126;  // This uniquely defines this script file to the error handler

// Called by options.js::Load()
function Load ()
{
    try
    {
        parent.g_btnDefaultPage = options_btnDefault;
    }
    catch(err)
    {
	    parent.g_ErrorHandler.DisplayException (err);
    }

}

function Save()
{
	top.trace(">> Save()");
	try
	{
	  var bConfigure = false;
	  var bSwitchAP = false;
	  var bSwitchHPP = false;
      try
      {
      
	      bConfigure = parent.IsTrialValid
					    && (parent.InitialLoadVxD != parent.NAVOptions.STARTUP.LoadVxD
					        || (!parent.Is9x && parent.InitialDelayLoad != parent.NAVOptions.AUTOPROTECT.DelayLoad) );
      }
      catch(err)
      {
         // Don't configure AP if an error occurs
         bConfigure = false;
         parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
      }
	  try
	  {
		  bSwitchAP = parent.IsTrialValid
					   && parent.NAVAutoProtect.Enabled != parent.CurAPRunning;
	  }
	  catch(err)
	  {
		  // Don't switch AP if the state cannot be retrieved
		  bSwitchAP = false;
		  parent.NAVAutoProtect.NAVError.LogAndDisplay(parent.g_HWND);
	  }
	  try
	  {
		
		if(parent.IsTrialValid)
		{
			bSwitchHPP = (parent.NAVOptions.HPP.Enabled != parent.HPPEnabled);
			if(bSwitchHPP == false)
				bSwitchHPP = (parent.NAVOptions.HPP.Respond != parent.HPPRespond);
			if(bSwitchHPP == false)
				bSwitchHPP = (parent.NAVOptions.HPP.Control != parent.HPPControl);
		}
		
		
	  }
	  catch (err)
	  {
		 bSwitchHPP = false;
         parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
	  }
      var bSwitchProduct = false;
      var bSwitchVirusdefs = false;

      // Only switch the modes for ALU if the product reg catalog was available
      // during this session of options and ALU was not hosed
      try
      {
          if( !parent.NAVOptions.ALURunning && !parent.ALUhosed )
          {
	         bSwitchProduct = parent.IsTrialValid
				           && parent.InitialALUProduct != parent.NAVOptions.ALU.Product;
	         bSwitchVirusdefs = parent.IsTrialValid
				           && parent.InitialALUVirusdefs != parent.NAVOptions.ALU.Virusdefs;
	      }
       }
       catch(err)
       {
          parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
       }
       
	  // Flush changes to disk
	  try
	  {
		top.trace("Flush changes");
		parent.NAVOptions.Save();
		top.trace("Done");
	  }
	  catch (NAVErr)
	  {
		parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
		return;
	  }

	  // Set the startup configuration
	  if (bConfigure)
	  {
		top.trace("Set startup");
		
        var APstartup = 0;
        try
        {
            APstartup = parent.NAVOptions.STARTUP.LoadVxD;

            try
		    {
			    parent.NAVAutoProtect.Configure(APstartup);
		    }
		    catch(err)
		    {
			    parent.NAVAutoProtect.NAVError.LogAndDisplay(parent.g_HWND);
		    }
		    
            // Display the message only if the option to Load at startup was changed, not if
            // the delay load was changed
            if(parent.InitialLoadVxD != parent.NAVOptions.STARTUP.LoadVxD)
            {
				var MB_OK				= 0x00000000;
				var MB_ICONEXCLAMATION	= 0x00000030;
				var MB_ICONASTERISK		= 0x00000040;

				var sww = null;

				try 
				{
					sww = new ActiveXObject("CcWebWnd.ccWebWindow");
				}
				catch (NAVErr)
				{
					parent.g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR_REINSTALL.innerText,  
													document.frames("Errors").ERROR_CREATING_NAVWEBWINDOW_OBJECT_ID.innerText);
					return;
				}
				
				APWillStop.innerText = APWillStop.innerText.replace(/%s/, parent.NAVOptions.ProductName);

				var SnoozeAP = 0x01;
				var SnoozePeriod = parent.NAVOptions.GetSnoozePeriod(SnoozeAP);
				if(0 == SnoozePeriod)
				{
					if(0 == parent.NAVOptions.STARTUP.LoadVxD)
						sww.MsgBox(document.all.APWillStop.innerText, parent.NAVOptions.ProductName, MB_OK | MB_ICONEXCLAMATION);
				}
		    }
        }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }
	  }
	  
	  if(bSwitchHPP)
	  {
	  
		top.trace("Set HPP");
		try
		{
			var axOptionsChangedEvent = new ActiveXObject('HPPSCR.Util'); 
			axOptionsChangedEvent.ResetOptions();
			
	
		}
		catch(err)
		{
			parent.NAVAutoProtect.NAVError.LogAndDisplay(parent.g_HWND);
		}
	  }

	  // Set AP service if the state of AP is changed
	  if (bSwitchAP)
	  {
		top.trace("Set AP");
		
		try
		{
			parent.NAVAutoProtect.Enabled = parent.CurAPRunning;
		}
		catch(err)
		{
			parent.NAVAutoProtect.NAVError.LogAndDisplay(parent.g_HWND);
		}
	  }

	  try
	  {
		top.trace("Change ALU");
	  
		// Change ALU Update modes for product patches if it changed
		if (bSwitchProduct)
		{
			parent.NAVOptions.EnableALU(parent.NAVOptions.ALU.Product, true);
		}

		// Change ALU Update modes for Virus Def patches if it changed
		if (bSwitchVirusdefs)
		{
			parent.NAVOptions.EnableALU(parent.NAVOptions.ALU.Virusdefs, false);
		}
	  }
	  catch (NAVErr)
	  {
		parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
	  }

      // Clear the password if necessary
      if( parent.ClearPass )
      {
		top.trace("Clear password");
		
        try	
        {
            var NAVPass = new ActiveXObject("Symantec.Norton.AntiVirus.NAVPwd");

            // Reset the NAVPassword enabled flag to it's old state
            try
            {
                NAVPass.ProductID = parent.NAV_CONSUMER_PASSWORD_ID;
                NAVPass.Clear();
            }
            catch(err)
            {
                // Swallow error during save
            }
        }
        catch(err)
        {        
            // Swallow error during save
        }
      }

	}
	catch (err)
	{
		parent.g_ErrorHandler.DisplayException (err);
		return;
	}
	top.trace("<< Save()");
}

// Save and close
function OK()
{
	top.trace(">> OK()");
  try
  {
	// enum SnoozeFeature
	var SnoozeAP		    = 0x01;
	var SnoozeIWP			= 0x02;
	var SnoozeALU			= 0x04;
	var SnoozeEmailIn		= 0x08;
	var SnoozeEmailOut		= 0x10;
	
	// Update options
	parent.bClosedWithX = false;

	if (!parent.CurrentPage.Validate())
		return;

	parent.CurrentPage.Terminate();

	// Build snooze list	
	var slist = 0;
	
	if (top.snoozeAP)
		slist |= SnoozeAP;
		
	if (top.snoozeWP)
		slist |= SnoozeIWP;
		
	if (top.snoozeEmailIn)
		slist |= SnoozeEmailIn;
		
	if (top.snoozeEmailOut)
		slist |= SnoozeEmailOut;
		
	if (top.snoozeLU)
		slist |= SnoozeALU;
		
	if (slist != 0)
	{
		if (!parent.NAVOptions.Snooze(parent.g_HWND, slist, true))
		{
			Cancel();
			return;
		}

		if (top.snoozeAP)
		{
			switch (parent.NAVOptions.GetSnoozePeriod(SnoozeAP))
			{
			case 0:
				// Turn off permanently
				parent.NAVOptions.STARTUP.LoadVxD = 0;
				break;
			
			case -1:
				// Wake up after reboot
				parent.NAVOptions.STARTUP.LoadVxD = 1;
				break;
				
			default:
				// You get a positive number when you snooze for a 
				// specific time period. Below, after saving the 
				// options, this will be changed to Configure(0), 
				// because you can snooze across a reboot.
				// When the snooze period expires, this will be reset 
				// to Configure(1).
				parent.NAVOptions.STARTUP.LoadVxD = 0;
				break;
			}
		}
	}
	
	// Clear snooze flags
	
	slist = 0;
	
	if (!top.snoozeAP && top.snoozeAP != null && parent.NAVOptions.GetSnoozePeriod(SnoozeAP) != 0)
	{
		//parent.NAVAutoProtect.Configure(parent.NAVOptions.STARTUP.LoadVxD);
		slist |= SnoozeAP;
	}
	
	if (!top.snoozeWP && top.snoozeWP != null)
		slist |= SnoozeIWP;
		
	if (!top.snoozeEmailIn && top.snoozeEmailIn != null)
		slist |= SnoozeEmailIn;
		
	if (!top.snoozeEmailOut && top.snoozeEmailOut != null)
		slist |= SnoozeEmailOut;

	if (!top.snoozeLU && top.snoozeLU != null)
		slist |= SnoozeALU;

	if (slist != 0)
		parent.NAVOptions.SetSnoozePeriod(slist, 0);
		
	// Save options
	Save();

	// If you are snoozing for a specific time period, turn off starting AP on
	// reboot, since you can snooze across a reboot.
	//if (parent.NAVOptions.GetSnoozePeriod(SnoozeAP) > 0 && parent.NAVOptions.STARTUP.LoadVxD > 0)
	//{
		// Do not start after reboot.
	    //parent.NAVAutoProtect.Configure(0);
	    // The Snooze alert monitor will turn on AP on startup when it wakes 
	    // up the feature at the end of the snooze period.
	//}

	parent.location.href = 'closeme.xyz';
  }
  catch (err)
  {
	parent.g_ErrorHandler.DisplayException (err);
	return;
  }
	top.trace("<< OK()");
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
	parent.CurrentPage.Initialize();
  }
  catch (err)
  {
	parent.g_ErrorHandler.DisplayException (err);
	return;
  }
}
