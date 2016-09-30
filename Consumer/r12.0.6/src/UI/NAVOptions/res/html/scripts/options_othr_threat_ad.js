// options_othr_threat_ad.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 116;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

function enable_controls()
{
	try
	{
	    with( document.options_othr_threat_ad )
	    {
	        var bEnabled = ManualScan.checked;
	        idrad_ManualAskMe.disabled = idrad_ManualAutoDelete.disabled = idrad_ManualRecommended.disabled = !bEnabled;

            bEnabled = EmailScan.checked;
            idrad_EmailAskMe.disabled = idrad_EmailAutoDelete.disabled  = idrad_EmailRecommended.disabled = !bEnabled;
            
            bEnabled = IMScan.checked;
            idrad_IMAskMe.disabled = idrad_IMAutoDelete.disabled = idrad_IMRecommended.disabled = !bEnabled;
        } // end with(...)
       
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}

// Main page object
function ThreatAdvancedPage()
{
        try
        {
          // No page validation
          this.Validate = function() { return true; }

          // save options values from the screen
          this.Terminate = function()
          {
                with( document.options_othr_threat_ad )
                {
                    try
                    {
                        if (parent.IsTrialValid)
                        {
                            // Set the NAVOptions object from the HTML controls
						    //
						    // 0 = Manual
						    // 1 = Auto Delete
                            parent.NAVOptions.SCANNER.ThreatCatEnabled = Math.abs(ManualScan.checked);
                            if( idrad_ManualAskMe.checked )
                        	    parent.NAVOptions.SCANNER.ThreatCatResponse = 0;
                            else if ( idrad_ManualRecommended.checked )
                                parent.NAVOptions.SCANNER.ThreatCatResponse = 2;
                            else // Auto Delete
                        	    parent.NAVOptions.SCANNER.ThreatCatResponse = 1;
                            
                            parent.NAVOptions.NAVEMAIL.ThreatCatEnabled = Math.abs(EmailScan.checked);
                            if( idrad_EmailAskMe.checked )
                        	    parent.NAVOptions.NAVEMAIL.ThreatCatResponse = 0;
                        	else if ( idrad_EmailRecommended.checked )
                                parent.NAVOptions.NAVEMAIL.ThreatCatResponse = 2;
                            else // Auto Delete
                        	    parent.NAVOptions.NAVEMAIL.ThreatCatResponse = 1;
                            	
                            parent.NAVOptions.IMSCAN.ThreatCatEnabled = Math.abs(IMScan.checked);
                            if( idrad_IMAskMe.checked )
                        	    parent.NAVOptions.IMSCAN.ThreatCatResponse = 0;
                            else if ( idrad_IMRecommended.checked )
                                parent.NAVOptions.IMSCAN.ThreatCatResponse = 2;
                            else // Auto Delete
                        	    parent.NAVOptions.IMSCAN.ThreatCatResponse = 1;                        	    
                            
                            if ( !parent.Is9x )
                            {
                                if ( Math.abs(AP.checked))
                                    parent.NAVOptions.AP.RespondToThreats |= 2;
                                else
                                    parent.NAVOptions.AP.RespondToThreats &= ~2; // AND of a NOT 2
                            }
                        }
                    }
                    catch(err)
                    {
                        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
                    }
                } // end with( document.options_othr_threat_ad )
          }  // End of Terminate()

          // Provide help for this page
          this.Help = function()
          {
	        try
	        {
	            parent.NAVOptions.Help(11707); //#define IDH_NAVW_OPTIONS_THREAT_CAT_ADVANCED_HELP_BTN	11707
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
	            if (parent.IsTrialValid)
	            {
	                // Set these HTML controls from the NAVOptions object defaults
	                parent.NAVOptions.SCANNER.ThreatCatEnabled();
	                parent.NAVOptions.SCANNER.ThreatCatResponse();
	                parent.NAVOptions.NAVEMAIL.ThreatCatEnabled();
	                parent.NAVOptions.NAVEMAIL.ThreatCatResponse();
	                parent.NAVOptions.IMSCAN.ThreatCatEnabled();
	                parent.NAVOptions.IMSCAN.ThreatCatResponse();
	                
	                if ( !parent.Is9x )
                        parent.NAVOptions.AP.RespondToThreats();
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
          	with( document.options_othr_threat_ad )
          	{
		        try
		        {
		            // Enable scrolling for high-contrast mode
                    if( parent.NAVOptions.IsHighContrastMode )
	                    body_options_othr_threat_ad.scroll = "auto";
	                    
	                // Set product name
	                manual_scan_product_name.innerText = parent.NAVOptions.ProductName;
	                email_product_name.innerText = parent.NAVOptions.ProductName;
	                im_product_name.innerText = parent.NAVOptions.ProductName;
	               
	   
		            if (parent.IsTrialValid)
		            {
	                    // Set these HTML controls from the NAVOptions object
	                    ManualScan.checked = parent.NAVOptions.SCANNER.ThreatCatEnabled;
	                    switch(parent.NAVOptions.SCANNER.ThreatCatResponse)
			            {
				            case 0:
				            idrad_ManualAskMe.checked = 1;
				            break;
				            case 1:
				            idrad_ManualAutoDelete.checked = 1;
				            break;
				            default:
				            idrad_ManualRecommended.checked = 1;
				            break;
			            }
    		            
		                EmailScan.checked = parent.NAVOptions.NAVEMAIL.ThreatCatEnabled;
		                switch(parent.NAVOptions.NAVEMAIL.ThreatCatResponse)
			            {
				            case 0:
				            idrad_EmailAskMe.checked = 1;
				            break;
				            case 1:
				            idrad_EmailAutoDelete.checked = 1;
				            break;
				            default:
				            idrad_EmailRecommended.checked = 1
				            break;
			            }
    			        
			            IMScan.checked = parent.NAVOptions.IMSCAN.ThreatCatEnabled;
			            switch(parent.NAVOptions.IMSCAN.ThreatCatResponse)
			            {
				            case 0:
				            idrad_IMAskMe.checked = 1;
				            break;
				            case 1:
				            idrad_IMAutoDelete.checked = 1;
				            break;
				            default:
				            idrad_IMRecommended.checked = 1;
				            break;
			            }
    			        
                        if ( !parent.Is9x )
                        {
                            // Set the current AP threat scanning check box
                            AP.checked = (parent.NAVOptions.AP.RespondToThreats & 2); // SAVRT_THREAT_MODE_NONVIRAL
                            
                            if( parent.CurAPRunning )
                            {
                                AP.disabled = false;
                                AP.focus();
                            }
                            else
                            {
                                // Disable the AP option since the main AP option is disabled
                                AP.disabled = true;
                                ManualScan.focus();
                            }
                        }
                        else
                        {
                            // Hide the AP threat option which is not available on 9x
                            threat_AP.style.display = "none";
                            AP.style.display = "none";
                            
                            // Set focus to the manual scan check box
                            ManualScan.focus();
                        }
            
	                    enable_controls();
                    }
                    else //Trialware invalid
                    {
                        ManualScan.disabled = 
                        idrad_ManualAskMe.disabled = 
                        idrad_ManualAutoDelete.disabled =
                        EmailScan.disabled = 
                        idrad_EmailAskMe.disabled = 
                        idrad_EmailAutoDelete.disabled = 
                        IMScan.disabled =
                        idrad_IMAskMe.disabled = 
                        idrad_IMAutoDelete.disabled =
                        AP.disabled =
                        idchk_APAutoScan.disabled = true;
                    }
		        }
		        catch(err)
		        {
		            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
		        }
	        } // end with( document.options_othr_threat_ad )
          }  // end of Initialize()

          // Initialize this page
          this.Initialize();
        }
        catch (err)
        {
            g_ErrorHandler.DisplayException (err);
            return;
        }

}  // end function ThreatAdvancedPage()
