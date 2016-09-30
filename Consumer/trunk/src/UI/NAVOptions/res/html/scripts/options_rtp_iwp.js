////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// options_rtp_iwp.js

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Global variables
g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 108;  // This uniquely defines this script file to the error handler

// From IWPSettingsInterface.h
IWPStateError =	0;              // Couldn't get state - ERROR 
IWPStateNotRunning = 1;         // Agent isn't running (ccApp) but should be - ERROR
IWPStateNotAvailable = 100;     // Not supposed to exist - Not an error!
IWPStateYielding = 101;         // IWP is off because another product is taking precedence
IWPStateEnabled = 102;          // Default OK setting
IWPStateDisabled = 103;         // Loaded but turned off

IWP_EXCLUSIONS_DIALOG = 0;
IWP_APPLICATION_RULES_DIALOG = 1;
IWP_GENERAL_RULES_DIALOG = 2;
IWP_AUTOBLOCK_DIALOG = 3;

function EnableIWP()
{
	try
	{
		if(!document.options_rtp_iwp.IWP_Enabled.checked){
			ButtonDisable(document.options_rtp_iwp.Configure_IWP_Exclusions);
			ButtonDisable(document.options_rtp_iwp.Configure_Application_Rules);
			ButtonDisable(document.options_rtp_iwp.Configure_General_Rules);
			ButtonDisable(document.options_rtp_iwp.Configure_AutoBlock);
			ButtonDisable(document.options_rtp_iwp.IWP_Trust);
		}
		else{
			ButtonEnable(document.options_rtp_iwp.Configure_IWP_Exclusions);
			ButtonEnable(document.options_rtp_iwp.Configure_AutoBlock);
//*******************************************************************************************//
//*********************************************TODO*****************************************//	

			//if(parent.NAVOptions.IWPFirewallOn){
				ButtonEnable(document.options_rtp_iwp.Configure_Application_Rules);
				ButtonEnable(document.options_rtp_iwp.Configure_General_Rules);
			//}
			ButtonEnable(document.options_rtp_iwp.IWP_Trust);
//*******************************************************************************************//
		}
	}
	catch(err)
	{
	    parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
	}
		    
}

// Main page object
function WormProtectionPage()
{
   try
   {
	   // No page validation
	   this.Validate = function() { return true; }

	   // save options values from the screen if Messenger client is installed
	   this.Terminate = function()
	   {
	      try
	      {
		    if(parent.NAVOptions.IWPIsInstalled && parent.NAVOptions.IWPCanEnable &&
					(parent.NAVOptions.IWPState != IWPStateYielding)){
	            parent.NAVOptions.IWPUserWantsOn = Math.abs(document.options_rtp_iwp.IWP_Enabled.checked);
	            parent.NAVOptions.IWPUserWantsToTrust = Math.abs(document.options_rtp_iwp.IWP_Trust.checked);
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
            parent.NAVOptions.Help(250110); //NAVW_OPTIONS_IWP_ABOUT
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
		  try
		  {
		  	if(parent.NAVOptions.IWPCanEnable)
		  	{
				parent.NAVOptions.SetIWPDefaults();			
				document.options_rtp_iwp.IWP_Enabled.checked = parent.NAVOptions.IWPUserWantsOn;
				document.options_rtp_iwp.IWP_Trust.checked = parent.NAVOptions.IWPUserWantsToTrust;
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
                {
	                body_options_rtp_iwp.scroll = "auto";
	                document.options_rtp_iwp.Configure_AutoBlock.style.border = '1px solid white';
	                document.options_rtp_iwp.Configure_Application_Rules.style.border = '1px solid white';
	                document.options_rtp_iwp.Configure_General_Rules.style.border = '1px solid white';
	                document.options_rtp_iwp.Configure_IWP_Exclusions.style.border = '1px solid white';
                }

				/* If no firewall is not on, do not display firewall settings
					[Configure_Application_Rules] [Configure_General_rules]
				   If IWPCanEnable is not true, disable everything
				   If IWPState is yielding, disable everything
				   If IWP is available, but not on, disable [Configure_IWP_Exclusions]
				*/

				if(!parent.NAVOptions.IWPFirewallOn)
				{
					ButtonDisable(document.options_rtp_iwp.Configure_Application_Rules);
					ButtonDisable(document.options_rtp_iwp.Configure_General_Rules);
				}
							
				//If IWP is not installed, this page should never be loaded, redundancy
				
//*******************************************************************************************//
//*********************************************TODO*****************************************//				
				//Workaround: Never disable IWP
				// Always show as enabled
				//document.options_rtp_iwp.IWP_Enabled.checked = parent.NAVOptions.IWPUserWantsOn;	
				document.options_rtp_iwp.IWP_Enabled.checked = true;
				document.options_rtp_iwp.IWP_Trust.checked = true;
				if(!parent.NAVOptions.IWPIsInstalled || !parent.NAVOptions.IWPCanEnable ||
					(parent.NAVOptions.IWPState == IWPStateYielding))
				{
					document.options_rtp_iwp.IWP_Enabled.checked = false;
					document.options_rtp_iwp.IWP_Enabled.disabled = true;
					document.options_rtp_iwp.IWP_Trust.checked = true;
					document.options_rtp_iwp.IWP_Trust.disabled = true;
				}
				else
				{
					document.options_rtp_iwp.IWP_Enabled.checked = parent.NAVOptions.IWPUserWantsOn;
					document.options_rtp_iwp.IWP_Trust.checked = parent.NAVOptions.IWPUserWantsToTrust;
				}
//*******************************************************************************************//

				if(!document.options_rtp_iwp.IWP_Enabled.disabled)
				{
					document.options_rtp_iwp.IWP_Enabled.focus();
				}
		    }
		    catch(err)
		    {
		        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
		    }
		    
	   }  // end of Initialize()
	   
	   // Initialize this page
	   this.Initialize();
       top.iwpInitialized = true;
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}  // end function WormProtectionPage()

function ConfigureExclusions()
{
	try{
		parent.NAVOptions.InvokeIWPDialog(IWP_EXCLUSIONS_DIALOG);
	}
	catch(err)
	{
		parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
	}
}

function ConfigureApplication()
{
	try{
		parent.NAVOptions.InvokeIWPDialog(IWP_APPLICATION_RULES_DIALOG);
	}
	catch(err)
	{
		parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
	}
}

function ConfigureGeneral()
{
	try{
		parent.NAVOptions.InvokeIWPDialog(IWP_GENERAL_RULES_DIALOG);
	}
	catch(err)
	{
		parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
	}
}

function ConfigureAutoBlock()
{
	try{
		parent.NAVOptions.InvokeIWPDialog(IWP_AUTOBLOCK_DIALOG);
	}
	catch(err)
	{
		parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
	}
}