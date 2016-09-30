// options_int_email_adv.js
g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 106;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Main page object
function AdvEMailPage()
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
		  parent.NAVOptions.NAVPROXY.ShowTrayIcon      = Math.abs(document.options_int_email_adv.DisplayTrayIcon.checked);
		  parent.NAVOptions.NAVPROXY.ShowProgressOut   = Math.abs(document.options_int_email_adv.ShowProgressOut.checked);
		  parent.NAVOptions.NAVPROXY.TimeOutProtection = Math.abs(document.options_int_email_adv.EmailTimeouts.checked);
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
            parent.NAVOptions.Help(33035); // IDH_NAVW_EMAIL_ADVANCED_SETTINGS_HELP_BTN
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
              parent.NAVOptions.NAVPROXY.ShowTrayIcon();
		      parent.NAVOptions.NAVPROXY.ShowProgressOut();
		      parent.NAVOptions.NAVPROXY.TimeOutProtection();
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
	            body_int_em.scroll = "auto";
	        
		      // Set these HTML controls from the NAVOptions object
		      document.options_int_email_adv.DisplayTrayIcon.checked = parent.NAVOptions.NAVPROXY.ShowTrayIcon;
		      document.options_int_email_adv.ShowProgressOut.checked = parent.NAVOptions.NAVPROXY.ShowProgressOut;
		      document.options_int_email_adv.EmailTimeouts.checked   = parent.NAVOptions.NAVPROXY.TimeOutProtection;

              // If no email scanning is enabled disable timeout and tray icon controls
              if( !parent.IsEmailEnabled )
              {
                document.options_int_email_adv.DisplayTrayIcon.disabled = true;
		        document.options_int_email_adv.EmailTimeouts.disabled   = true;
              }
              // If outgoing scanning is not enabled disable the outgoing progress control
              if( !parent.IsOutgoingEmailEnabled )
              {
                document.options_int_email_adv.ShowProgressOut.disabled = true;
              }
          }
          catch(error)
          {
             parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
          }

		  // Set focus to first element
		  if (parent.IsTrialValid && parent.IsEmailEnabled)
			document.options_int_email_adv.EmailTimeouts.focus();
	   }  // end of Initialize()

	   // Initialize this page
	   this.Initialize();
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}  // end function AdvEMailPage()
