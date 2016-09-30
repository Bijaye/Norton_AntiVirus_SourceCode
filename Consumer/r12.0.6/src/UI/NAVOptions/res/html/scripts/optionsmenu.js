// optionsmenu.js
g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 101;  // This uniquely defines this script file to the error handler

//Holds the last selected row.  Used to compare current row against
//new row.
g_LastRow = null;

function CurrentPageDummy()
{
	try
	{
	  this.Initialize  = function() { return true; };
	  this.Validate  = function() { return true; };
	  this.Terminate = function() { return true; };
	  this.Help      = function() { return true; };
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}

function GoTo(url, objTR)
{
  try
  {
	  var i = parent.frames[1].location.href.lastIndexOf('/');
	  var href = parent.frames[1].location.href.substring(i + 1);

	  if (url != href)
	  {
		if (parent.CurrentPage)
		{
		  // If the information on the page is not valid don't let the user switch
		  // (The Validate() function should tell the user what's wrong)
		  if (!parent.CurrentPage.Validate())
			return false;

		  parent.CurrentPage.Terminate();
      
		  // On slow machines, it is possible to get a script error
		  // while accessing a freed script.  After calling terminate,
		  // the current page is no longer valid so substitute in a
		  // static set of CurrentPage functions.
		  parent.CurrentPage = new CurrentPageDummy();
		}

		HilightItems(objTR);
		parent.frames[1].location.replace(url);
		parent.frames[1].focus();
	  }

	  return false;
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }

}

function GoToBloodhound(page, objTR)
{
  try
  {
	var i = parent.frames[1].location.href.lastIndexOf('/');
	var href = parent.frames[1].location.href.substring(i + 1);
	if ('options_bloodhound.htm' == href)
	{
		// If we navegating from another bloodhound page ther is no need to reload
		 parent.CurrentPage.Terminate();
		 parent.BloodHound = page;
		 parent.CurrentPage.Initialize();
		 HilightItems(objTR);
	}
	else
	{
		 parent.BloodHound = page;
		 GoTo('options_bloodhound.htm', objTR);
	}

	return false;
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }
}

function GoToExclusions(page, objTR, isViral)
{
  try
  {
    parent.NAVOptions.ExclusionType = (isViral?true:false);
	GoTo(page, objTR);
	
	return false;
  }
  catch (err)
  {
	g_ErrorHandler.DisplayException (err);
	return;
  }
}

function ExpandAP(bExpand)
{
	try
	{
		if( true == bExpand )
		{
			// Expand AP Items
			twistie_sys_ap.src = "arrow_dots_down.gif"
			sys_ap_bloodhound_dashline.style.display = "";
			sys_ap_bloodhound.style.display = "";
			sys_ap_advanced_dashline.style.display = "";
			sys_ap_advanced.style.display = "";
			sys_ap_exclusions_dashline.style.display = "";
			sys_ap_exclusions.style.display = "";
		}
		else
		{
			 // Retract AP Items
			 twistie_sys_ap.src = "arrow_dots_rgt.gif"
			 sys_ap_bloodhound_dashline.style.display = "none";
			 sys_ap_bloodhound.style.display = "none";
			 sys_ap_advanced_dashline.style.display = "none";
			 sys_ap_advanced.style.display = "none";
			 sys_ap_exclusions_dashline.style.display = "none";
			 sys_ap_exclusions.style.display = "none";
		}
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}

function ExpandMS(bExpand)
{
    try
	{
		if( true == bExpand )
		{
			// Expand MS Items
			twistie_sys_ms.src = "arrow_dots_down.gif"
			sys_ms_bloodhound_dashline.style.display = "";
			sys_ms_bloodhound.style.display = "";
			sys_ms_exclusions_dashline.style.display = "";
			sys_ms_exclusions.style.display = "";
		}
		else
		{
			// Retract MS Items
			twistie_sys_ms.src = "arrow_dots_rgt.gif"
			sys_ms_bloodhound_dashline.style.display = "none";
			sys_ms_bloodhound.style.display = "none";
			sys_ms_exclusions_dashline.style.display = "none";
			sys_ms_exclusions.style.display = "none";
		}
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}

function ExpandEmail(bExpand)
{
    try
	{
		if( true == bExpand )
		{
			// Expand Email Items
			twistie_int_email.src = "arrow_dots_down.gif";
			int_email_advanced_dashline.style.display = "";
			int_email_advanced.style.display = "";
		}
		else
		{
			// Retract Email Items
			twistie_int_email.src = "arrow_dots_rgt.gif";
			int_email_advanced_dashline.style.display = "none";
			int_email_advanced.style.display = "none";
		}
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}

function ExpandThreat(bExpand)
{
    try
	{
		if( true == bExpand )
		{
			// Expand Threat Items
			twistie_other_threat.src = "arrow_dots_down.gif";
			other_threat_advanced_dashline.style.display = "";
			other_threat_advanced.style.display = "";
			other_threat_exclusions_dashline.style.display = "";
			other_threat_exclusions.style.display = "";
		}
		else
		{
			// Retract Threat Items
			twistie_other_threat.src = "arrow_dots_rgt.gif";
			other_threat_advanced_dashline.style.display = "none";
			other_threat_advanced.style.display = "none";
			other_threat_exclusions_dashline.style.display = "none";
			other_threat_exclusions.style.display = "none";
		}
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}

function RowClick(objTR)
{
   try
   {
	   //Expand or contract any children.
	   switch(objTR)
	   {
	   case sys_ap:
		  if(twistie_sys_ap.src.indexOf('down')>-1)
		  {
			 ExpandAP(false);
		  }
		  else
		  {
			 // Expand AP, retract Email, Threat, and Manual Scan
			 ExpandAP(true);
			 ExpandMS(false);
			 ExpandEmail(false);
			 ExpandThreat(false);
		  }
		  break;
	   case sys_ms:
		  if(twistie_sys_ms.src.indexOf('down')>-1)
		  {
			 ExpandMS(false);
		  }
		  else
		  {
			 // Expand Manual Scan, retract AP, Threat, and Email
			 ExpandMS(true);
			 ExpandAP(false);
			 ExpandEmail(false);
			 ExpandThreat(false);
		  }
		  break;
	   case int_email:
		  if(twistie_int_email.src.indexOf('down')>-1)
		  {
			 ExpandEmail(false);
		  }
		  else
		  {
			 // Expand Email, retract AP, Threat, and Manual Scan
			 ExpandEmail(true);
			 ExpandAP(false);
			 ExpandMS(false);
			 ExpandThreat(false);
		  }
          break;
	   case other_threat:
		  if(twistie_other_threat.src.indexOf('down')>-1)
		  {
			 ExpandThreat(false);
		  }
		  else
		  {
			 // Expand Threat, retract AP, Email, and Manual Scan
			 ExpandThreat(true);
			 ExpandEmail(false);
			 ExpandAP(false);
			 ExpandMS(false);
		  }
		  break;
	   }

	   if ( parent.g_btnDefaultPage != null )
		 parent.g_btnDefaultPage.disabled = false;
   
	   //Select Panel
	   switch(objTR)
	   {
	   case sys_ap:
		  GoTo('options_sys_ap.htm', objTR);
		  break;
	   case sys_ap_bloodhound:
		  GoToBloodhound(0, objTR);
		  break;
	   case sys_ap_advanced:
		  GoTo('options_sys_ap_ad.htm', objTR);
		  break;
	   case sys_ap_exclusions:
		  GoTo('options_sys_ap_excl.htm', objTR);
		  break;
	   case sys_ms:
		  GoTo('options_sys_scan.htm', objTR);
		  break;
	   case sys_ms_bloodhound:
		  GoToBloodhound(1, objTR);
		  break;
	   case sys_ms_exclusions:
		  GoToExclusions('options_sys_excl.htm', objTR, true);
		  break;
	   case sys_hp:
		  GoTo('options_sys_hp.htm', objTR);
		  break;
	   case int_email:
		  GoTo('options_int_email.htm', objTR);
		  break;
	   case int_email_advanced:
		  GoTo('options_int_email_adv.htm', objTR);
		  break;
	   case int_lu:
		  GoTo('options_int_lu.htm', objTR);
		  break;
	   case int_im:
		  GoTo('options_int_im.htm', objTR);
		  break;
	   case int_iwp:
	      GoTo('options_int_iwp.htm', objTR);
	      break;
	   case other_misc:
		  GoTo('options_othr_gen.htm', objTR);
		  break;
	   case other_advanced:
		  if ( parent.g_btnDefaultPage != null )
   			parent.g_btnDefaultPage.disabled = true;
		  GoTo('options_othr_advanced.htm', objTR);
          break;
	   case other_threat:
		  GoTo('options_othr_threat.htm', objTR);
          break;
	   case other_threat_advanced:
	      GoTo('options_othr_threat_ad.htm', objTR);
          break;
	   case other_threat_exclusions:
	      GoToExclusions('options_othr_threat_excl.htm', objTR, false);
		  break;
	   }   
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}

// Called by options.js::Load()
//
function Load()
{
	try
	{
	    //
        // Create global objects
        //
        
        // Only create a NAVOptions object if we did not have one passed in properly
        if( parent.NAVOptions == null )
        {
            try
            {
                parent.NAVOptions = new ActiveXObject ("Symantec.Norton.AntiVirus.NAVOptions");
            }
            catch (err)
            {
	            var msg = document.frames("Errors").document.all.ERROR_INTERNAL_ERROR_REINSTALL.innerText;
	            var id = document.frames("Errors").document.all.ERROR_CREATING_OPTIONS_OBJECT_ID.innerText;
	            g_ErrorHandler.DisplayNAVError (msg, id);
			    parent.g_bStopLoading = true;
			    return;
            }
        }

        try 
        {
            parent.NAVAutoProtect = new ActiveXObject ("NAVAPSCR.ScriptableAutoProtect");
        }
        catch (err)
        {
	        var msg = document.frames("Errors").document.all.ERROR_INTERNAL_ERROR_REINSTALL.innerText;
	        var id = document.frames("Errors").document.all.ERROR_CREATING_AP_OBJECT_ID.innerText;
	        g_ErrorHandler.DisplayNAVError (msg, id);
			parent.g_bStopLoading = true;
			return;
        }

        // Init global variables
        //
        parent.Is9x = navigator.userAgent.indexOf('Windows 9')>-1;
        parent.IsMe = navigator.userAgent.indexOf('Win 9x')>-1;
        parent.IsNT4 = navigator.userAgent.indexOf('Windows NT)')>-1;
        parent.Is2K = navigator.userAgent.indexOf('Windows NT 5')>-1;
        parent.IsIE4 = navigator.userAgent.indexOf('MSIE 4')>-1;
        parent.IsXP = navigator.userAgent.indexOf('Windows NT 5.1')>-1;
        
        // Get the HWND for the options
        try
        {
            parent.g_HWND = parent.NAVOptions.HWND;
        }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(0);
        }

        try
        {
            parent.IsEmailEnabled = parent.NAVOptions.NAVEMAIL.ScanIncoming || parent.NAVOptions.NAVEMAIL.ScanOutgoing;
        }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }
        try
        {
            parent.InitialLoadVxD = parent.NAVOptions.STARTUP.LoadVxD; // Save the current startup state of AP
            
            if( !parent.Is9x )
                parent.InitialDelayLoad = parent.NAVOptions.AUTOPROTECT.DelayLoad;
        }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }
        try
        {
	        parent.IsTrialValid = parent.NAVOptions.IsTrialValid;
	    }   
        catch(err)
        {
	        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }
        try
        {
            var ProdSubStr;
            if( parent.Is9x )
                ProdSubStr = "NAV95";
            else
                ProdSubStr = "NAVNT";
	        parent.InitialALUProduct = parent.NAVOptions.IsAluOn(ProdSubStr);         // Save current state of ALU Product updates
	        parent.InitialALUVirusdefs = parent.NAVOptions.IsAluOn("MicroDefs"); // Save current state of ALU VirusDef updates
        }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
            parent.ALUhosed = 1;
        }
        try
        {

	        // Set the ALU Options file values to the correct initial state
	        // (in case they were changed by a force outside of our options)
            if( !parent.NAVOptions.ALUrunning && !parent.ALUhosed )
            {
	            parent.NAVOptions.ALU.Virusdefs = parent.InitialALUVirusdefs;
	            parent.NAVOptions.ALU.Product = parent.InitialALUProduct;
            }

	        parent.IsNAVPro = parent.NAVOptions.IsNAVPro;
        }
        catch(err)
        {
	        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }

        // Get the initial state of the password protection
        try	
        {
            var NAVPass = new ActiveXObject("Symantec.Norton.AntiVirus.NAVPwd");

            try
            {
                NAVPass.ProductID = parent.NAV_CONSUMER_PASSWORD_ID;
                parent.InitialPassword = Boolean(NAVPass.Enabled);
				parent.InitialPasswordSet = NAVPass.UserPasswordSet;
            }
            catch(err)
            {
		        // Set the default initial value since the password is hosed
                parent.InitialPassword = false;
                parent.InitialPasswordSet = false;
            }
        }
        catch(err)
        {
            // Swallowing error here since it will be displayed by the Miscellaneous page
        }

        try
        {
	        parent.CurAPRunning = parent.NAVAutoProtect.Enabled;   // Save the current running state of AP
										          // This variable will get updated if user
										          // Enables AP with the Enable AP checkbox.
        }
        catch(err)
        {
			// If we can't get the state, then save the current state as disabled
			parent.CurAPRunning = false;
			parent.NAVAutoProtect.NAVError.LogAndDisplay(parent.g_HWND);
        }
        
        try
        {
            parent.HPPFeatureEnabled = parent.NAVOptions.HPP.FeatureEnabled;
			if(parent.HPPFeatureEnabled)
            {
                parent.HPPEnabled = parent.NAVOptions.HPP.Enabled;
			    parent.HPPRespond = parent.NAVOptions.HPP.Respond;
			    parent.HPPControl = parent.NAVOptions.HPP.Control;
			}
			else
			{
				parent.HPPEnabled = false;
			    parent.HPPRespond = false;
			    parent.HPPControl = false;
            }
        }
        catch (err)
        {
			parent.HPPFeatureEnabled = false;
			parent.HPPEnabled = false;
			parent.HPPRespond = false;
			parent.HPPControl = false;
			parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }
        
      // Hide Advanced section if this is not NAV Professional
	  if ( !parent.HPPFeatureEnabled )
	  {
		sys_hp.style.display = "none";
		sys_hp_dashline.style.display = "none";
	  }

	  // If on NT4 IE4, there is no ALU so remove the page.
	  if (parent.IsNT4 && parent.IsIE4)
	  {
		int_lu_dashline.style.display = "none";
		int_lu.style.display = "none";
		//document.all.NotNT4IE4.style.display = 'none';
	  }

      try
      {
        // Hide Threat Category panels if hidden attribute is set
	    if( parent.NAVOptions.THREAT.NoThreatCat )
	    {
	        other_threat.style.display="none";
	        other_threat_advanced_dashline.style.display="none";
	        other_threat_advanced.style.display="none";
	        other_threat_exclusions_dashline.style.display="none";
	        other_threat_exclusions.style.display="none";
	        
	        other_misc_dashline.style.display="none";
	    }
	  }
	  catch(err)
	  {
	  }

      // Hide Advanced section if this is not NAV Professional
	  if ( !parent.IsNAVPro )
	  {
		other_advanced.style.display = "none";
		other_advanced_dashline.style.display = "none";
	  }
	  
	  // Hide IWP if not installed
	  try
	  {
	    if( !parent.NAVOptions.IWPIsInstalled )
	    {
	        int_iwp_dashline.style.display = "none";
	        int_iwp.style.display = "none";
	    }
	  }
	  catch(err)
	  {
	  }
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
	 	  
}

function HilightItems(objTR)
{
   try
   {
	 //Deselect old row.
	 if (g_LastRow != null && g_LastRow != objTR)
	   g_LastRow.className = "OptsListEntry";
      
	 //Switch the hilight to the new item.
	 objTR.className = "OptsListEntrySelected"; 
	 g_LastRow = objTR;

   }
   catch (err)
   {
	 g_ErrorHandler.DisplayException (err);
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
		parent.NAVOptions.SetSnoozePeriod(0x1F, 0);		// Clear all.
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
		try
		{
			parent.NAVAutoProtect.Enabled = parent.CurAPRunning = true;
			parent.NAVAutoProtect.Configure(parent.NAVOptions.STARTUP.LoadVxD);
		}
		catch(err)
		{
			parent.NAVAutoProtect.NAVError.LogAndDisplay(parent.g_HWND);
		}
	  }
	  
        try	
        {
            var NAVPass = new ActiveXObject("Symantec.Norton.AntiVirus.NAVPwd");
            try
            {
                NAVPass.ProductID = parent.NAV_CONSUMER_PASSWORD_ID;
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

      // Reset the ALU modes
      try
	  {
        var bSwitchProduct = false;
        var bSwitchVirusdefs = false;

        // Only update the modes for ALU if the product reg catalog was available
        // during this session of options and ALU wasn't hosed
        if( !parent.NAVOptions.ALURunning && !parent.ALUhosed )
        {
           // Reset the defaults right now
           parent.NAVOptions.ALU.Product();
           parent.NAVOptions.ALU.Virusdefs();
           
	       bSwitchProduct = parent.IsTrialValid
		           && parent.InitialALUProduct != parent.NAVOptions.ALU.Product;
	       bSwitchVirusdefs = parent.IsTrialValid
		           && parent.InitialALUVirusdefs != parent.NAVOptions.ALU.Virusdefs;
	    }
		
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
        parent.location.href = 'closeme.xyz';
		return;
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

// This function allows user to navigate the options menu by
// using the keyboard. This is to comply with Section 508. KM
function KeyboardNavigate(objTR) 
{
	try
	{
		// The "SPACE" and "ENTER" keys are used.
	   if ( (event.keyCode == 13) || (event.keyCode == 32) )
	   {
		 RowClick(objTR);
	   }
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}
