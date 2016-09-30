////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// options_int_email.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 105;  // This uniquely defines this script file to the error handler // 106 for the tray icon, progress, and timeout

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

function OnEnableClick(){
	EnableControls();
}

function OnOEHEnableClick()
{
	document.options_rtp_email.RbOEHResponseAuto.disabled = 
	document.options_rtp_email.RbOEHResponseAsk.disabled = 
	!document.options_rtp_email.ProxyEnabledHeuristics.checked;
}

function EnableControls()
{
	try
	{
		//Worm Blocking and Progress Indicator only relevant for Outgoing Email
		//Security Risks only enabled if !NoThreatCat
		var bEnabled = document.options_rtp_email.ProxyEnabledIncoming.checked || 
						document.options_rtp_email.ProxyEnabledOutgoing.checked;

		parent.IsEmailEnabled = bEnabled;
     	parent.IsOutgoingEmailEnabled = document.options_rtp_email.ProxyEnabledOutgoing.checked;
		
		document.options_rtp_email.ProxyEnabledHeuristics.disabled = 
		document.options_rtp_email.EmailShowProgressOut.disabled = 
		!document.options_rtp_email.ProxyEnabledOutgoing.checked;

		document.options_rtp_email.RbOEHResponseAuto.disabled = 
		document.options_rtp_email.RbOEHResponseAsk.disabled = 
		!document.options_rtp_email.ProxyEnabledHeuristics.checked
		|| document.options_rtp_email.ProxyEnabledHeuristics.disabled;

		document.options_rtp_email.EmailTimeouts.disabled = 
	    	document.options_rtp_email.RbAlertRemove.disabled =
	    	document.options_rtp_email.RbAlertIgnore.disabled =
	    	document.options_rtp_email.RbAlertAsk.disabled =	
		document.options_rtp_email.EmailDisplayTray.disabled = !bEnabled;
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
	      			parent.NAVOptions.EmailScanIncoming 		=	Math.abs(document.options_rtp_email.ProxyEnabledIncoming.checked);
	          		parent.NAVOptions.EmailScanOutgoing 		=	Math.abs(document.options_rtp_email.ProxyEnabledOutgoing.checked);
	          		parent.NAVOptions.EmailScanOEH 			=	Math.abs(document.options_rtp_email.ProxyEnabledHeuristics.checked);

	          		// Keep these in sync: TimeOutProtection and OEHReponseMode must be the same
	  			parent.NAVOptions.NAVPROXY_TimeoutProtection = 	Math.abs(document.options_rtp_email.EmailTimeouts.checked);

				if(document.options_rtp_email.RbOEHResponseAuto.checked)
					parent.NAVOptions.NAVEMAIL_OEHResponseMode = 0;
				else
					parent.NAVOptions.NAVEMAIL_OEHResponseMode = 2;
				          			
	          		parent.NAVOptions.NAVPROXY_TrayAnimation      = 	Math.abs(document.options_rtp_email.EmailDisplayTray.checked);
	          		parent.NAVOptions.NAVPROXY_OutgoingProgress   = 	Math.abs(document.options_rtp_email.EmailShowProgressOut.checked);

				var alertMode;
				if(document.options_rtp_email.RbAlertRemove.checked)
					alertMode = 0;
				else if(document.options_rtp_email.RbAlertIgnore.checked)
					alertMode = 1;
				else
					alertMode = 2;

				parent.NAVOptions.NAVEMAIL_ResponseMode = alertMode;
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
				parent.NAVOptions.Help(33000); //IDH_NAVW_EMAIL_PROTECTION_SETTINGS_HELP_BTN
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
				parent.NAVOptions.SetEmailPageDefaults();
				parent.NAVOptions.NAVPROXY_TimeoutProtection();
				parent.NAVOptions.NAVPROXY_TrayAnimation();
				parent.NAVOptions.NAVEMAIL_OEHResponseMode();
				parent.NAVOptions.NAVPROXY_OutgoingProgress();
				parent.NAVOptions.NAVEMAIL_ResponseMode();	               	
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
	        			body_options_rtp_email.scroll = "auto";
	        			
 		  		// Set these HTML controls from the NAVOptions object
		  		if (parent.IsTrialValid)
		  		{	
					document.options_rtp_email.ProxyEnabledIncoming.checked 	= parent.NAVOptions.EmailScanIncoming; 
			 		document.options_rtp_email.ProxyEnabledOutgoing.checked	= parent.NAVOptions.EmailScanOutgoing;
			 		document.options_rtp_email.ProxyEnabledHeuristics.checked	= parent.NAVOptions.EmailScanOEH;

					document.options_rtp_email.EmailTimeouts.checked   		= parent.NAVOptions.NAVPROXY_TimeoutProtection;	
					document.options_rtp_email.EmailDisplayTray.checked 		= parent.NAVOptions.NAVPROXY_TrayAnimation;
					document.options_rtp_email.EmailShowProgressOut.checked 	= parent.NAVOptions.NAVPROXY_OutgoingProgress;	

					var alertMode = parent.NAVOptions.NAVEMAIL_ResponseMode;
					if(alertMode == 0)
						document.options_rtp_email.RbAlertRemove.checked = true;
					else if(alertMode == 1)
						document.options_rtp_email.RbAlertIgnore.checked = true;
					else
						document.options_rtp_email.RbAlertAsk.checked = true;
						
					alertMode = parent.NAVOptions.NAVEMAIL_OEHResponseMode;
					if(alertMode == 0)
						document.options_rtp_email.RbOEHResponseAuto.checked = true;
					else
						document.options_rtp_email.RbOEHResponseAsk.checked = true;
					
					EnableControls();

		  			//ScanIncomingEnabled will never be disabled, so focus can be set here
		  			document.options_rtp_email.ProxyEnabledIncoming.focus();
		  		}
		  		else
		  		{
			 		// Invalid trialware. disable everything.
			 		document.options_rtp_email.ProxyEnabledIncoming.checked = false;
			 		document.options_rtp_email.ProxyEnabledIncoming.disabled = true;
			 		document.options_rtp_email.ProxyEnabledOutgoing.checked = false;
			 		document.options_rtp_email.ProxyEnabledOutgoing.disabled = true;
			 		document.options_rtp_email.ProxyEnabledHeuristics.checked = false;
			 		document.options_rtp_email.ProxyEnabledHeuristics.disabled = true;
			 		document.options_rtp_email.EmailTimeouts.checked = false;
			 		document.options_rtp_email.EmailTimeouts.disabled = true;
			 		document.options_rtp_email.EmailDisplayTray.checked = false;
			 		document.options_rtp_email.EmailDisplayTray.disabled = true;
			 		document.options_rtp_email.EmailShowProgressOut.checked = false;
			 		document.options_rtp_email.EmailShowProgressOut.disabled = true;
			 		document.options_rtp_email.RbAlertRemove.disabled = true;
			 		document.options_rtp_email.RbAlertIgnore.disabled = true;
			 		document.options_rtp_email.RbAlertAsk.disabled = true;
		  		}
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
