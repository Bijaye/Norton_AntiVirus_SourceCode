////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// optionsmenu.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 101;  // This uniquely defines this script file to the error handler

//Holds the last selected row.  Used to compare current row against
//new row.
g_LastRow = null;

// From IWPSettingsInterface.h
IWPStateError =	0;              // Couldn't get state - ERROR 
IWPStateNotRunning = 1;         // Agent isn't running (ccApp) but should be - ERROR
IWPStateNotAvailable = 100;     // Not supposed to exist - Not an error!
IWPStateYielding = 101;         // IWP is off because another product is taking precedence
IWPStateEnabled = 102;          // Default OK setting
IWPStateDisabled = 103;         // Loaded but turned off

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
	  var i = parent.frames[2].location.href.lastIndexOf('/');
	  var href = parent.frames[2].location.href.substring(i + 1);

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
		parent.frames[2].location.replace(url);
		parent.frames[2].focus();
	  }

	  return false;
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
	   if ( parent.g_btnDefaultPage != null )
		 parent.g_btnDefaultPage.disabled = false;
   
	   //Select Panel
	   switch(objTR)
	   {
	   case rtp_general:
		  GoTo('options_rtp_general.htm', objTR);
		  break;
	   case rtp_ap:
		  GoTo('options_rtp_ap.htm', objTR);
		  break;
	   case rtp_email:
		  GoTo('options_rtp_email.htm', objTR, true);
		  break;
	   case rtp_iwp: 
		  GoTo('options_rtp_iwp.htm', objTR, true);
		  break;
	   case rtp_im:
		  GoTo('options_rtp_im.htm', objTR);
		  break;
	   case scan_general:
		  GoTo('options_scan_general.htm', objTR);
		  break;
	   case lu_general:
		  GoTo('options_lu_general.htm', objTR);
		  break;
	   case adv_exclusions:
		  GoTo('options_adv_exclusions.htm', objTR);
		  break;
	   case adv_exclusions_securityrisks:
		  GoTo('options_adv_exclusions_securityrisks.htm', objTR);
		  break;			  
	   case adv_securityrisks:
		  GoTo('options_adv_securityrisks.htm', objTR);
		  break;
	   case adv_misc:
		  GoTo('options_adv_misc.htm', objTR);
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
		if(parent.NAVOptions.IsHighContrastMode)
		{
			options_menu_body.scroll = "auto";
			options_menu_body.style.borderTop = "2px SOLID WHITE";
		}
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
			if( !parent.NAVOptions.IWPIsInstalled ||
				parent.NAVOptions.IWPState == IWPStateYielding)
			{
				document.getElementById("rtp_iwp").style.display="none";
			}
     	}
		catch(err)
		{
			parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
		}
		/*try
		{
			if( parent.NAVOptions.THREAT_NoThreatCat )
			{
				other_threat_exclusions_dashline.style.display="none";
				other_threat_exclusions.style.display="none";
			}
		}
		catch(err)
		{
		}
		*/	
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
