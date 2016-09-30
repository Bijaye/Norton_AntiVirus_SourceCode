////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// options_adv_securityrisks.js
g_ModuleID = 3023;
g_ScriptID = 115;  // This uniquely defines this script file to the error handler

//Threat Categories from NAVOptions.idl
var AllNonVirals = 0;
var SecurityRisk = 4;
var HackTool = 5;
var Spyware = 6;
var Trackware = 7;
var Dialers = 8;
var RemoteAccess = 9;
var Adware = 10;
var JokePrograms = 11;

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Main page object
function AdvancedSecurityRisksPage()
{
  // No page validation
  this.Validate = function() { return true; }

  // save options values from the screen
  this.Terminate = function()
  {
    try
    {
        if( parent.NAVOptions.IsTrialValid )
        {
            // Set the NAVOptions object from the HTML controls
            parent.NAVOptions.ThreatCategoryEnabled(SecurityRisk) =	Math.abs(document.options_adv_securityrisks.SecurityRisk.checked);
            parent.NAVOptions.ThreatCategoryEnabled(HackTool) = Math.abs(document.options_adv_securityrisks.HackTool.checked);
            parent.NAVOptions.ThreatCategoryEnabled(Spyware) =	Math.abs(document.options_adv_securityrisks.Spyware.checked);
            parent.NAVOptions.ThreatCategoryEnabled(Trackware) = 		Math.abs(document.options_adv_securityrisks.Trackware.checked);
            parent.NAVOptions.ThreatCategoryEnabled(Dialers) = 		Math.abs(document.options_adv_securityrisks.Dialers.checked);
            parent.NAVOptions.ThreatCategoryEnabled(RemoteAccess) = 	Math.abs(document.options_adv_securityrisks.RemoteAccess.checked);
            parent.NAVOptions.ThreatCategoryEnabled(Adware) = 		Math.abs(document.options_adv_securityrisks.Adware.checked);
            parent.NAVOptions.ThreatCategoryEnabled(JokePrograms) = 	Math.abs(document.options_adv_securityrisks.JokePrograms.checked);
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
        	 //parent.NAVOptions.ThreatCategoryEnabled(SecurityRisk, parent.NAVOptions.DefaultThreatCategoryEnabled(SecurityRisk));
			//parent.NAVOptions.ThreatCategoryEnabled(SecurityRisk, parent.NAVOptions.DefaultThreatCategoryEnabled(SecurityRisk));
        	          	  
        	  
        	parent.NAVOptions.DefaultThreatCategoryEnabled(SecurityRisk);
        	parent.NAVOptions.DefaultThreatCategoryEnabled(HackTool);
        	parent.NAVOptions.DefaultThreatCategoryEnabled(Spyware);
        	parent.NAVOptions.DefaultThreatCategoryEnabled(Trackware);
        	parent.NAVOptions.DefaultThreatCategoryEnabled(Dialers);
        	parent.NAVOptions.DefaultThreatCategoryEnabled(RemoteAccess);
        	parent.NAVOptions.DefaultThreatCategoryEnabled(Adware);
        	parent.NAVOptions.DefaultThreatCategoryEnabled(JokePrograms);
        	
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
	        body_options_adv_securityrisks.scroll = "auto";
	            
        if( parent.IsTrialValid )
        {
            // Set these HTML controls from the NAVOptions object
            document.options_adv_securityrisks.Adware.checked 		= parent.NAVOptions.ThreatCategoryEnabled(Adware);
            document.options_adv_securityrisks.Dialers.checked 		= parent.NAVOptions.ThreatCategoryEnabled(Dialers);
            document.options_adv_securityrisks.HackTool.checked 		= parent.NAVOptions.ThreatCategoryEnabled(HackTool);
            document.options_adv_securityrisks.RemoteAccess.checked 	= parent.NAVOptions.ThreatCategoryEnabled(RemoteAccess);
            document.options_adv_securityrisks.SecurityRisk.checked 	= parent.NAVOptions.ThreatCategoryEnabled(SecurityRisk);
            document.options_adv_securityrisks.Spyware.checked 		= parent.NAVOptions.ThreatCategoryEnabled(Spyware);
            document.options_adv_securityrisks.JokePrograms.checked 	= parent.NAVOptions.ThreatCategoryEnabled(JokePrograms);
            document.options_adv_securityrisks.Trackware.checked 	= parent.NAVOptions.ThreatCategoryEnabled(Trackware);
            
            // Set focus to first element
            document.options_adv_securityrisks.Spyware.focus();
        }
        else // Invalid TrialWare
        {
            document.options_adv_securityrisks.Adware.disabled =
            document.options_adv_securityrisks.Dialers.disabled =
            document.options_adv_securityrisks.HackTool.disabled = 
            document.options_adv_securityrisks.RemoteAccess.disabled =
            document.options_adv_securityrisks.SecurityRisk.disabled =
            document.options_adv_securityrisks.Spyware.disabled =
            document.options_adv_securityrisks.Trackware.disabled =
            document.options_adv_securityrisks.JokePrograms.disabled = true;
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
