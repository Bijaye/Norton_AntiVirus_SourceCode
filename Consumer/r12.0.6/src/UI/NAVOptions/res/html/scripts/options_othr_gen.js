// options_othr_gen.js

g_ModuleID = 3023;
g_ScriptID = 114;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Main page object
function GeneralPage()
{
  // No page validation
  this.Validate = function() { return true; }

    // Get a NAVPwd object to check if it's enabled or not
    try	
    {
        var NAVPass = new ActiveXObject("Symantec.Norton.AntiVirus.NAVPwd");

        // Set the NAVPassword features
        try
        {
            NAVPass.ProductID = parent.NAV_CONSUMER_PASSWORD_ID;
            NAVPass.FeatureName = document.frames("Errors").FeatureName.innerText.replace(/%s/, parent.NAVOptions.ProductName);
            NAVPass.ProductName = parent.NAVOptions.ProductName;
        }
        catch(err)
        {
		    parent.g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,  
										    document.frames("Errors").ERROR_SETTING_PASSWORD_FEATURES_ID.innerText);
        }
    }
    catch(err)
    {
         var msg = document.frames("Errors").ERROR_INTERNAL_ERROR_REINSTALL.innerText;
         var id = document.frames("Errors").ERROR_CREATING_PASSWORD_OBJECT_ID.innerText;
         g_ErrorHandler.DisplayNAVError ( msg, id );
    }
   

  // save options values from the screen
  this.Terminate = function()
  {
    try
    {
        // Set the NAVOptions object from the HTML controls
        if (-1 != parent.NAVOptions.OfficePlugin)
          parent.NAVOptions.OfficePlugin = Math.abs(document.options_othr_gen.OfficeProtection.checked);
        parent.NAVOptions.SCANNER.BackupRep = Math.abs(document.options_othr_gen.BackupFiles.checked);
        parent.NAVOptions.AP.BackupRep = Math.abs(document.options_othr_gen.BackupFiles.checked);
        parent.NAVOptions.DEFALERT.EnableOldDefs = Math.abs(document.options_othr_gen.DefAlert.checked);
        parent.NAVOptions.StartupScan = Math.abs(document.options_othr_gen.StartupScan.checked);
        
        if( !parent.Is9x )
        {
            parent.NAVOptions.SymProtectEnabled = Math.abs(document.options_othr_gen.SymProtect.checked);
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
        parent.NAVOptions.Help(1281); // IDH_NAVW_GENERAL_DLG_HELP_BTN
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
        if (-1 != parent.NAVOptions.OfficePlugin)
          parent.NAVOptions.OfficePlugin();
        parent.NAVOptions.SCANNER.BackupRep();
        parent.NAVOptions.DEFALERT.EnableOldDefs();
        parent.NAVOptions.StartupScan();
        
        if( !parent.Is9x )
        {
            // Default value for SymProtect is on
            parent.NAVOptions.SymProtectEnabled = true;
        }
        
        if ( typeof (NAVPass) != 'undefined' )
        {
            try
            {
                if( NAVPass.Enabled )
                {
                    NAVPass.Enabled = false;
                    parent.ClearPass = true;
                    parent.NAVOptions.IsPasswordDirty = true;
                }
                // If the password was set then prepare to unset it
                else if( NAVPass.UserPasswordSet )
                {
                    parent.ClearPass = true;
                    parent.NAVOptions.IsPasswordDirty = true;
                }
            }
            catch(err)
            {
                document.options_othr_gen.EnablePassword.checked = false;
			    parent.g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,  
												       document.frames("Errors").ERROR_CALLING_PASSWORD_FUNCTION_ID.innerText);
            }
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
    // Enable scrolling for high-contrast mode
    if( parent.NAVOptions.IsHighContrastMode )
	    body_gen.scroll = "auto";
	        
    // Hide WinMe sections
    if (!parent.Is9x || parent.IsMe)
    {
      sys_start_group.style.display = "none";
    }
    
    // Only show symprotect option on NT
    if( parent.Is9x )
    {
      SymProtectSpan.style.display = "none";
    }
     
    try
    {
        // Set these HTML controls from the NAVOptions object
        if (-1 != parent.NAVOptions.OfficePlugin)
          document.options_othr_gen.OfficeProtection.checked = parent.NAVOptions.OfficePlugin;
        else  // Office 2K is not installed
        {
          document.options_othr_gen.OfficeProtection.disabled = true;
          document.all.OfficeInstallStatus.style.display = '';
        }
        document.options_othr_gen.BackupFiles.checked = parent.NAVOptions.SCANNER.BackupRep;
        document.options_othr_gen.DefAlert.checked = parent.NAVOptions.DEFALERT.EnableOldDefs;
        document.options_othr_gen.StartupScan.checked = parent.NAVOptions.StartupScan;

        if( parent.NAVOptions.SystemMode )
        {
            document.options_othr_gen.EnablePassword.disabled = true;
	        document.options_othr_gen.SetPassword.disabled = true;
        }
        else if ( typeof (NAVPass) != 'undefined' )
        {
            document.options_othr_gen.EnablePassword.checked = NAVPass.Enabled;
	        document.options_othr_gen.SetPassword.disabled = !NAVPass.Enabled;
        }
        
        if( !parent.Is9x )
        {
            document.options_othr_gen.SymProtect.checked = parent.NAVOptions.SymProtectEnabled;
        }

        // Set focus to first element
        document.options_othr_gen.BackupFiles.focus();
    }
    catch(err)
    {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
    }
  }  // end of Initialize()

  // Initialize this page
  this.Initialize();
}  // end function GeneralPage()

// Enable password protection
function EnablePass()
{
    // Don't allow this in safe mode
    if( !parent.NAVOptions.SystemMode )
    {
    
        // First create the NAVPwd object
        try	
        {
            var NAVPass = new ActiveXObject("Symantec.Norton.AntiVirus.NAVPwd");
        }
        catch(err)
        {
		    parent.g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR_REINSTALL.innerText,  
										    document.frames("Errors").ERROR_CREATING_PASSWORD_OBJECT_ID.innerText);
    	                
		    // Bail Out
            return;
        }

        // Set the properties
        try
        {
            NAVPass.ProductID = parent.NAV_CONSUMER_PASSWORD_ID;
            NAVPass.FeatureName = document.frames("Errors").FeatureName.innerText.replace(/%s/, parent.NAVOptions.ProductName);
            NAVPass.ProductName = parent.NAVOptions.ProductName;
        }
        catch(err)
        {
		    parent.g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,  
										    document.frames("Errors").ERROR_SETTING_PASSWORD_FEATURES_ID.innerText);
            // Bail Out
            return;
        }

	    try
        {
            // Password is being enabled
		    if( document.options_othr_gen.EnablePassword.checked )
		    {
			    // Prompt if it's the first time
                try
                {
                    var bAlreadySet = NAVPass.UserPasswordSet;
                }
                catch(err)
                {
                    document.options_othr_gen.EnablePassword.checked = false;
				    parent.g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,  
												        document.frames("Errors").ERROR_CALLING_PASSWORD_FUNCTION_ID.innerText);	  		    					                   
                    // Bail Out
                    return;
                }
			    if( false == bAlreadySet  )
			    {
				    // Make sure the set succeeded before changing the options value
				    // If the set fails then uncheck the password enable checkbox
                    try
                    {
                        var bSet = NAVPass.Set(false);
                    }
                    catch(err)
                    {
                        document.options_othr_gen.EnablePassword.checked = false;
					    parent.g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,  
													        document.frames("Errors").ERROR_CALLING_PASSWORD_FUNCTION_ID.innerText);
                        // Bail Out
                        return;
                    }
                    try
                    {
                        // Check the return value from the Set() method and set the Enabled
                        // if it was successfully set
                        if( true == bSet  )
				        {
                            NAVPass.Enabled = true;
				        }
				        else    // Password was not set...bail out
				        {
					        document.options_othr_gen.EnablePassword.checked = false;
					        return;
				        }
                    }
                    catch(err)
                    {
                        document.options_othr_gen.EnablePassword.checked = false;
					    parent.g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,  
													        document.frames("Errors").ERROR_CALLING_PASSWORD_FUNCTION_ID.innerText);	  		    					                   
                        // Bail Out
                        return;
                    }
			    }

                // Enable the password change button and the state to enabled
                try
                {
			        NAVPass.Enabled = true;
                }
                catch(err)
                {
                    document.options_othr_gen.EnablePassword.checked = false;
				    parent.g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,  
													    document.frames("Errors").ERROR_CALLING_PASSWORD_FUNCTION_ID.innerText);
                    // Bail Out
                    return;
                }
			    document.options_othr_gen.SetPassword.disabled = false;
		    }
		    else // Password is being disabled
		    {
			    try
                {
                    // Store the disabled setting
			    NAVPass.Enabled = false;
                }
                catch(err)
                {
                    // Have to leave password enabled checked since we couldn't set the disabled property
                    document.options_othr_gen.EnablePassword.checked = true;
				    parent.g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,  
													    document.frames("Errors").ERROR_CALLING_PASSWORD_FUNCTION_ID.innerText);
                    // Bail Out
                    return;
                }

			    // Disable the button to change the password
			    document.options_othr_gen.SetPassword.disabled = true;
		    }

	    }
	    catch(PassError)
	    { 
		    parent.g_ErrorHandler.DisplayException (PassError);
            return;
	    }

        // Everything went well, set the dirty bit and password dirty flag
	    // if the password changed from it's initial value 
        try
        {
		    parent.NAVOptions.IsPasswordDirty = (parent.InitialPassword != document.options_othr_gen.EnablePassword.checked);
        }
        catch(err)
        {
            parent.g_ErrorHandler.DisplayException (PassError);
            return;
        }
    }
}

function ChangePass()
{
    // Don't allow this in safe mode
    if( !parent.NAVOptions.SystemMode )
    {
	    try
        {
		    // Call the NAVPwd set function to change the password
		    // tell it to check the old password.
            try
            {
			    // Create the password object
                var NAVPass = new ActiveXObject("Symantec.Norton.AntiVirus.NAVPwd");
            }
            catch(err)
            {      
			    parent.g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR_REINSTALL.innerText,  
											    document.frames("Errors").ERROR_CREATING_PASSWORD_OBJECT_ID.innerText);	  		    

                // Bail Out
                return;
            }
            try
            {
                NAVPass.ProductID = parent.NAV_CONSUMER_PASSWORD_ID;
	            NAVPass.FeatureName = document.frames("Errors").FeatureName.innerText.replace(/%s/, parent.NAVOptions.ProductName);
                NAVPass.ProductName = parent.NAVOptions.ProductName;
            }
            catch(err)
            {          
			    parent.g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,  
											    document.frames("Errors").ERROR_SETTING_PASSWORD_FEATURES_ID.innerText);
                // Bail Out
                return;
            }
            
            try
            {
		        NAVPass.Set(true);
            }
            catch(err)
            {	
			    parent.g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,  
											    document.frames("Errors").ERROR_CHANGING_PASSWORD_ID.innerText);
                // Bail Out
                return;
            }
	    }
	    catch(ChangeErr)
	    {
		    parent.g_ErrorHandler.DisplayException (ChangeErr);
	    }
	}
}
