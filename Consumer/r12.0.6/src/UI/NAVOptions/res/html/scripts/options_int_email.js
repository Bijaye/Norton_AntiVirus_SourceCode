// options_int_email.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 105;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

function OnEnableIncomingChanged(status)
{
	if (top.emailInitialized)
		top.snoozeEmailIn = !status;
}

function OnEnableOutgoingChanged(status)
{
	if (top.emailInitialized)
		top.snoozeEmailOut = !status;
}

function enable_controls()
{
	try
	{   
	   var fEnabled = document.options_int_email.ProxyEnabledIncoming.checked || document.options_int_email.ProxyEnabledOutgoing.checked

       parent.IsEmailEnabled = fEnabled;
       parent.IsOutgoingEmailEnabled = document.options_int_email.ProxyEnabledOutgoing.checked;
   
	   document.options_int_email.idrad_response_autorepair.disabled = 
	   document.options_int_email.idrad_response_prompt.disabled = 
	   document.options_int_email.idrad_response_Quarantine.disabled = 
	   document.options_int_email.idrad_response_Delete.disabled = 
	   document.options_int_email.idrad_response_QuarantineSilent.disabled = !fEnabled;

       fEnabled = document.options_int_email.ProxyEnabledOutgoing.checked;
       document.options_int_email.ProxyEnabledHeuristics.disabled = !fEnabled;

       fEnabled = document.options_int_email.ProxyEnabledHeuristics.disabled || !document.options_int_email.ProxyEnabledHeuristics.checked;
       document.options_int_email.SilentHeuristics.disabled = fEnabled;
       
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}

// Main page object
function EMailPage()
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
            if (parent.IsTrialValid)
            {
		      // Set the NAVOptions object from the HTML controls
		      //
		      // 0 = Manual
		      // 1 = Auto Repair
		      // 2 = Auto Repair, Quarantine
		      // 3 = Auto Repair, Quarantine Silently
		      // 4 = Auto Repair, Delete Silently
      
		      if (document.options_int_email.idrad_response_prompt.checked)
			     parent.NAVOptions.NAVEMAIL.ResponseMode = 0;
		      else if (document.options_int_email.idrad_response_Quarantine.checked)
			     parent.NAVOptions.NAVEMAIL.ResponseMode = 2;
		      else if (document.options_int_email.idrad_response_Delete.checked)
			     parent.NAVOptions.NAVEMAIL.ResponseMode = 4;
		      else if (document.options_int_email.idrad_response_QuarantineSilent.checked)
			     parent.NAVOptions.NAVEMAIL.ResponseMode = 3;
		      else //idrad_response_autorepair checked
			     parent.NAVOptions.NAVEMAIL.ResponseMode = 1;
      
		      parent.NAVOptions.NAVEMAIL.ScanIncoming          = Math.abs(document.options_int_email.ProxyEnabledIncoming.checked);
		      //ASZ 5-17-2001, IMAP support was dropped.  Leaving setting incase we add it back
		      //parent.NAVOptions.NAVEMAIL.ProxyIMAP         = Math.abs(document.options_int_email.ProxyEnabledIncoming.checked);
		      parent.NAVOptions.NAVEMAIL.ScanOutgoing         = Math.abs(document.options_int_email.ProxyEnabledOutgoing.checked);
		      parent.NAVOptions.NAVEMAIL.OEH        = Math.abs(document.options_int_email.ProxyEnabledHeuristics.checked);
              parent.NAVOptions.NAVEMAIL.OEHResponseMode     = Math.abs(document.options_int_email.SilentHeuristics.checked);
            }
         }
         catch(err)
         {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
            return;
         }  
	   }  // End of Terminate()

	   // Provide help for this page
	   this.Help = function()
	   {
          try
          {
		    parent.NAVOptions.Help(33000); // IDH_NAVW_EMAIL_PROTECTION_SETTINGS_HELP_BTN
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
		      // Set these HTML controls from the NAVOptions object
		      if (parent.IsTrialValid)
		      {
			     parent.NAVOptions.NAVEMAIL.ScanIncoming();
			     //ASZ 5-17-2001, IMAP support was dropped.  Leaving setting incase we add it back
			     //parent.NAVOptions.NAVEMAIL.ProxyIMAP();
			     parent.NAVOptions.NAVEMAIL.ScanOutgoing();
			     parent.NAVOptions.NAVEMAIL.ResponseMode();
			     parent.NAVOptions.NAVEMAIL.OEH();
                 parent.NAVOptions.NAVEMAIL.OEHResponseMode();
		      }
          }
          catch(err)
          {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
            return;
          }
	   }  // End of Default()

	   // Initialize the from with the options value
	   this.Initialize =  function()
	   {
        try
        {
          // Enable scrolling for high-contrast mode
	      if( parent.NAVOptions.IsHighContrastMode )
	        body_int_em.scroll = "auto";
	        
		  // Set these HTML controls from the NAVOptions object
		  if (parent.IsTrialValid)
		  {
			 //ASZ 5-17-2001, IMAP support was dropped.  Leaving setting incase we add it back
			 //if (parent.NAVOptions.NAVEMAIL.ScanIncoming > 0 || parent.NAVOptions.NAVEMAIL.ProxyIMAP > 0)
			 if (parent.NAVOptions.NAVEMAIL.ScanIncoming > 0)
				document.options_int_email.ProxyEnabledIncoming.checked = 1
			 else
				document.options_int_email.ProxyEnabledIncoming.checked = 0

			 document.options_int_email.ProxyEnabledOutgoing.checked = parent.NAVOptions.NAVEMAIL.ScanOutgoing;
			 document.options_int_email.ProxyEnabledHeuristics.checked = parent.NAVOptions.NAVEMAIL.OEH;
             document.options_int_email.SilentHeuristics.checked = parent.NAVOptions.NAVEMAIL.OEHResponseMode;
         
			 switch(parent.NAVOptions.NAVEMAIL.ResponseMode)
			 {
				case 0:
				   document.options_int_email.idrad_response_prompt.checked = 1;
				   break;
				case 2:
				   document.options_int_email.idrad_response_Quarantine.checked = 1;
				   break;
				case 4:
				   document.options_int_email.idrad_response_Delete.checked = 1;
				   break;
				case 3:
				   document.options_int_email.idrad_response_QuarantineSilent.checked = 1;
				   break;
				default:
				   document.options_int_email.idrad_response_autorepair.checked = 1;
				   break;
			 }
		  }
		  else
		  {
			 // Invalid trialware. disable everything.
			 document.options_int_email.ProxyEnabledIncoming.checked = false;
			 document.options_int_email.ProxyEnabledIncoming.disabled = true;
			 document.options_int_email.ProxyEnabledOutgoing.checked = false;
			 document.options_int_email.ProxyEnabledOutgoing.disabled = true;
			 document.options_int_email.ProxyEnabledHeuristics.checked = false;
			 document.options_int_email.ProxyEnabledHeuristics.disabled = true;
             document.options_int_email.SilentHeuristics.checked = false;
			 document.options_int_email.SilentHeuristics.disabled = true;
		  }

		  // Set focus to first element
		  if (parent.IsTrialValid)
			document.options_int_email.ProxyEnabledIncoming.focus();

		  enable_controls();
          }
         catch(err)
         {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
            return;
         }
	   }  // end of Initialize()

	   // Initialize this page
	   this.Initialize();
	   top.emailInitialized = true;
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}  // end function EMailPage()
