// options_int_im.js

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// IM Scanner global variables (treat as #defines)
g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 107;  // This uniquely defines this script file to the error handler

// Same values used for the Manual Scanner
var RESPONSE_MANUAL = 0
var RESPONSE_AUTOMATIC = 1
var RESPONSE_AUTO_THEN_QUARANTINE = 2
var IMTYPE_ALL = 0
var IMTYPE_MSN = 1
var IMTYPE_AOL = 2
var IMTYPE_YAHOO = 3
var IMTYPE_TOM = 4

// Variables that indicate which individual scanners are installed
// This prevents us from having to call into ccIMScan.dll everytime we want to know
var IsMSNInstalled = 0;
var IsAOLInstalled = 0;
var IsYAHOOInstalled = 0;
var IsTonline = 0;
var IsTOMInstalled = 0;

try
{
    IsMSNInstalled = parent.NAVOptions.IsIMInstalled(IMTYPE_MSN);
    IsAOLInstalled = parent.NAVOptions.IsIMInstalled(IMTYPE_AOL);
    IsYAHOOInstalled = parent.NAVOptions.IsIMInstalled(IMTYPE_YAHOO);
    
    // Find out if this is a Tonline-build
    // WJORDAN: Commenting out for beta since there is no t-online client available yet
    //          Need to add this back post-beta!!!
    IsTonline = false; //parent.NAVOptions.OEMVendor.indexOf("T-Online")>-1;
    
    if( IsTonline )
    {
        IsTOMInstalled = parent.NAVOptions.IsIMInstalled(IMTYPE_TOM);
    }
    else
        IsTOMInstalled = false;
}
catch(err)
{
    parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
}

// Variable to determine if at least one IM scanner is installed
var aScannerIsInstalled = IsMSNInstalled || IsAOLInstalled || IsYAHOOInstalled || IsTOMInstalled;

// Check to see if the Configure button should be enabled or disabled
function ConfCheck()
{
    try
	{
		if( document.options_int_instantmessenger.MSN_Enabled.checked || document.options_int_instantmessenger.YIM_Enabled.checked || document.options_int_instantmessenger.AOL_Enabled.checked )
		{
			// At least one is checked, enable configure button and radio boxes
			document.options_int_instantmessenger.Configure_IM.disabled = false;
            document.options_int_instantmessenger.idrad_im_AskMe.disabled = false;
			document.options_int_instantmessenger.idrad_im_AutoRepair.disabled = false;
			document.options_int_instantmessenger.idrad_im_Quarantine.disabled = false;
		}
		else
		{
			// None checked, disable configure button and radio boxes
			document.options_int_instantmessenger.Configure_IM.disabled = true;
            document.options_int_instantmessenger.idrad_im_AskMe.disabled = true;
			document.options_int_instantmessenger.idrad_im_AutoRepair.disabled = true;
			document.options_int_instantmessenger.idrad_im_Quarantine.disabled = true;
		}

        // If the MSN is checked or unchecked enable\disable the alert option
        if( document.options_int_instantmessenger.MSN_Enabled.checked )
        {
            document.options_int_instantmessenger.MSN_Alert_Enabled.disabled = false;
        }
        else
        {
            document.options_int_instantmessenger.MSN_Alert_Enabled.disabled = true;
        }
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}

// Configure IM Scanners on the fly
function ConfigureIM()
{
    try
    {
        // Configure the IM Clients that are currently checked
        try
		{
			if( document.options_int_instantmessenger.MSN_Enabled.checked )
		        parent.NAVOptions.ConfigureIM(IMTYPE_MSN);
	        if( document.options_int_instantmessenger.YIM_Enabled.checked )
		        parent.NAVOptions.ConfigureIM(IMTYPE_YAHOO);
	        if( document.options_int_instantmessenger.AOL_Enabled.checked )
		        parent.NAVOptions.ConfigureIM(IMTYPE_AOL);
		    if( IsTonline && document.options_int_instantmessenger.TOM_Enabled.checked )
		        parent.NAVOptions.ConfigureIM(IMTYPE_TOM);
		}
		catch (NAVErr)
		{
			parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
			return;
		}

        // Show success alert
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
		sww.MsgBox(document.all.ConfiguredGood.innerText, document.title, 64);
		delete sww;
    }
    catch(err)
    {
		g_ErrorHandler.DisplayNAVError (document.frames("Errors").document.all.ERROR_INTERNAL_ERROR.innerText,  
						document.frames("Errors").document.all.ERROR_CONFIGURING_IM_SCANNERS_ID.innerText);
                return;
    }
}

// Main page object
function InstantMessengerPage()
{
   try
   {
	   // No page validation
	   this.Validate = function() { return true; }

	   // save options values from the screen if Messenger client is installed
	   this.Terminate = function()
	   {
		  // Don't worry about saving anything if nothing is installed
		  if( aScannerIsInstalled )
		  {
			  try
              {
                  if( IsMSNInstalled )
                  {
				    parent.NAVOptions.IMSCAN.ScanMIM = Math.abs(document.options_int_instantmessenger.MSN_Enabled.checked);
                    parent.NAVOptions.IMSCAN.MessageSender = Math.abs(document.options_int_instantmessenger.MSN_Alert_Enabled.checked);
                  }
			      if( IsYAHOOInstalled )
				    parent.NAVOptions.IMSCAN.ScanYIM = Math.abs(document.options_int_instantmessenger.YIM_Enabled.checked);
			      if( IsAOLInstalled )
				    parent.NAVOptions.IMSCAN.ScanAIM = Math.abs(document.options_int_instantmessenger.AOL_Enabled.checked);
				  if( IsTOMInstalled )
				    parent.NAVOptions.IMSCAN.ScanTOL = Math.abs(document.options_int_instantmessenger.TOM_Enabled.checked);
      
			      if (document.options_int_instantmessenger.idrad_im_AutoRepair.checked)
				     parent.NAVOptions.IMSCAN.ResponseMode = RESPONSE_AUTOMATIC
			      else if (document.options_int_instantmessenger.idrad_im_AskMe.checked)
				     parent.NAVOptions.IMSCAN.ResponseMode = RESPONSE_MANUAL
			      else
				     parent.NAVOptions.IMSCAN.ResponseMode = RESPONSE_AUTO_THEN_QUARANTINE
                }
                catch(err)
                {
                    parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
                    return;
                }
		  }
	   }  // End of Terminate()

	   // Provide help for this page
	   this.Help = function()
	   {
		  try
          {
            parent.NAVOptions.Help(11650); // IDH_NAVW_OPTIONS_INSTANT_MESSENGER_HELP_BTN
          }
          catch(err)
          {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
            return;
          }
	   }

	   // restore default options values
	   this.Default = function()
	   {
		  // Set these HTML controls from the NAVOptions object
		  if (parent.IsTrialValid)
		  {
			 try
             {
                 parent.NAVOptions.IMSCAN.ScanMIM();
			     parent.NAVOptions.IMSCAN.ScanYIM();
			     parent.NAVOptions.IMSCAN.ScanAIM();
			     parent.NAVOptions.IMSCAN.ResponseMode();
                 parent.NAVOptions.IMSCAN.MessageSender();
                 
                 if( IsTonline )
                 {
                    parent.NAVOptions.IMSCAN.ScanTOL();
                 }
             }
             catch(err)
             {
               parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
             }
		  }
	   }  // End of Default()

	   // Initialize the from with the options value
	   this.Initialize =  function()
	   {
			// Set caption
			document.title = parent.NAVOptions.ProductName;
			
	      // Enable scrolling for high-contrast mode
          if( parent.NAVOptions.IsHighContrastMode )
	        body_int_im.scroll = "auto";
	            
		  // Set these HTML controls from the NAVOptions object
		  if (parent.IsTrialValid)
		  { 
		    // Show the TOnline client for T-Online builds
		    if( IsTonline )
		    {
		        document.all.TOMRow.style.display="";
		    }
		    
			// Only enable the radio controls if at least one scanner is installed
			if( aScannerIsInstalled )
			{
				 var ResponseMode;
                 try
                 {
                    ResponseMode = parent.NAVOptions.IMSCAN.ResponseMode
                 }
                 catch(err)
                 {
                   parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
                   return;
                 }

                 switch(ResponseMode)
				 {
					case RESPONSE_MANUAL:
					  document.options_int_instantmessenger.idrad_im_AskMe.checked = true;
					  break;
					case RESPONSE_AUTOMATIC:
					  document.options_int_instantmessenger.idrad_im_AutoRepair.checked = true;
					  break;
					default:
					  document.options_int_instantmessenger.idrad_im_Quarantine.checked = true;
					  break;
				 }
			 }
			 else
			 {
				 // No IM Scanners are installed disable everything and bail out

				 document.options_int_instantmessenger.MSN_Enabled.checked = false;
				 document.options_int_instantmessenger.YIM_Enabled.checked = false;
				 document.options_int_instantmessenger.AOL_Enabled.checked = false;
				 document.options_int_instantmessenger.idrad_im_AskMe.checked = false;
				 document.options_int_instantmessenger.idrad_im_AutoRepair.checked = false;
				 document.options_int_instantmessenger.idrad_im_Quarantine.checked = false;
                 document.options_int_instantmessenger.Configure_IM.checked = false;
                 document.options_int_instantmessenger.MSN_Alert_Enabled.checked = false;

				 document.options_int_instantmessenger.MSN_Enabled.disabled = true;
				 document.options_int_instantmessenger.YIM_Enabled.disabled = true;
				 document.options_int_instantmessenger.AOL_Enabled.disabled = true;
				 document.options_int_instantmessenger.idrad_im_AskMe.disabled = true;
				 document.options_int_instantmessenger.idrad_im_AutoRepair.disabled = true;
				 document.options_int_instantmessenger.idrad_im_Quarantine.disabled = true;
				 document.options_int_instantmessenger.Configure_IM.disabled = true;
                 document.options_int_instantmessenger.MSN_Alert_Enabled.disabled = true;

				 document.all.msn_recommended.style.display = "none";
				 document.all.msn_not_available.style.display = "";
				 document.all.yahoo_recommended.style.display = "none";
				 document.all.yahoo_not_available.style.display = "";
				 document.all.aol_recommended.style.display = "none";
				 document.all.aol_not_available.style.display = "";

                 if( IsTonline )
				 {
				    document.options_int_instantmessenger.TOM_Enabled.checked = false;
				    document.options_int_instantmessenger.TOM_Enabled.disabled = true;
				    document.all.tom_recommended.style.display = "none";
				 }
				 
				 // Set focus to the help link
				 document.all.im_help_link.focus();
				 
				 return;
			 }

			 // Only enable the checkbox if the IM services are installed
			 if( !IsMSNInstalled )
			 {
				document.all.msn_recommended.style.display = "none";
				document.all.msn_not_available.style.display = "";
				document.options_int_instantmessenger.MSN_Enabled.checked = false;
				document.options_int_instantmessenger.MSN_Enabled.disabled = true;
                document.options_int_instantmessenger.MSN_Alert_Enabled.checked = false;
                document.options_int_instantmessenger.MSN_Alert_Enabled.disabled = true;
			 }
			 else
             {
				document.options_int_instantmessenger.MSN_Enabled.checked = parent.NAVOptions.IMSCAN.ScanMIM;
                document.options_int_instantmessenger.MSN_Alert_Enabled.checked = parent.NAVOptions.IMSCAN.MessageSender;
             }

			 if( !IsYAHOOInstalled )
			 {
				document.all.yahoo_recommended.style.display = "none";
				document.all.yahoo_not_available.style.display = "";
				document.options_int_instantmessenger.YIM_Enabled.checked = false;
				document.options_int_instantmessenger.YIM_Enabled.disabled = true;
			 }
			 else
				document.options_int_instantmessenger.YIM_Enabled.checked = parent.NAVOptions.IMSCAN.ScanYIM;

			 if( !IsAOLInstalled )
			 {
				document.all.aol_recommended.style.display = "none";
				document.all.aol_not_available.style.display = "";
				document.options_int_instantmessenger.AOL_Enabled.checked = false;
				document.options_int_instantmessenger.AOL_Enabled.disabled = true;
			 }
			 else
				document.options_int_instantmessenger.AOL_Enabled.checked = parent.NAVOptions.IMSCAN.ScanAIM;
				
		     if( IsTonline )
		     {
		        if( !IsTOMInstalled )
		        {
		            document.all.tom_recommended.style.display = "none";
		            document.options_int_instantmessenger.TOM_Enabled.checked = false;
				    document.options_int_instantmessenger.TOM_Enabled.disabled = true;
		        }
		        else
		            document.options_int_instantmessenger.TOM_Enabled.checked = parent.NAVOptions.IMSCAN.ScanTOL;
		    }
		  }
		  else
		  {
			 // Invalid trialware. disable everything.
			 document.options_int_instantmessenger.MSN_Enabled.checked = false;
			 document.options_int_instantmessenger.YIM_Enabled.checked = false;
			 document.options_int_instantmessenger.AOL_Enabled.checked = false;
			 if( IsTonline )
			    document.options_int_instantmessenger.TOM_Enabled.checked = false;
			 document.options_int_instantmessenger.idrad_im_AskMe.checked = false;
			 document.options_int_instantmessenger.idrad_im_AutoRepair.checked = false;
			 document.options_int_instantmessenger.idrad_im_Quarantine.checked = false;
             document.options_int_instantmessenger.MSN_Alert_Enabled.checked = false;

			 document.options_int_instantmessenger.MSN_Enabled.disabled = true;
			 document.options_int_instantmessenger.YIM_Enabled.disabled = true;
			 document.options_int_instantmessenger.AOL_Enabled.disabled = true;
			 if( IsTonline )
			    document.options_int_instantmessenger.TOM_Enabled.disabled = true;
			 document.options_int_instantmessenger.idrad_im_AskMe.disabled = true;
			 document.options_int_instantmessenger.idrad_im_AutoRepair.disabled = true;
			 document.options_int_instantmessenger.idrad_im_Quarantine.disabled = true;
			 document.options_int_instantmessenger.Configure_IM.disabled = true;
             document.options_int_instantmessenger.MSN_Alert_Enabled.disabled = true;
		  }
      
		 // Disable the Configure button if no scanners are checked
		 ConfCheck();
      
		  // Set focus to first non disabled element
		  if (parent.IsTrialValid)
		  {
			if( !document.options_int_instantmessenger.AOL_Enabled.disabled )
				document.options_int_instantmessenger.AOL_Enabled.focus();
			else if( !document.options_int_instantmessenger.MSN_Enabled.disabled )
				document.options_int_instantmessenger.MSN_Enabled.focus();
			else if( !document.options_int_instantmessenger.YIM_Enabled.disabled )
				document.options_int_instantmessenger.YIM_Enabled.focus();
			else if( IsTonline && !document.options_int_instantmessenger.TOM_Enabled.disabled )
			    document.options_int_instantmessenger.TOM_Enabled.focus();
		  }
	   }  // end of Initialize()

	   // Initialize this page
	   this.Initialize();
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}  // end function InstantMessengerPage()
