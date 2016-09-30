////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

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

    if(parent.NAVOptions.PasswordSupported)
    {
        // Get a ccPwd object to check if it's enabled or not
        
        try
        {
        var NAVPass = new ActiveXObject("Symantec.isPassword");

        // Set the NAVPassword features
            try
            {
            NAVPass.ProductID = parent.NIS_CONSUMER_PASSWORD_ID;
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
    }
    else
    {
        // Hide option password protection
        option_password_header.style.display = "none";
        option_password.style.display = "none";
    }

    // save options values from the screen
    this.Terminate = function()
    {
            try
            {
                // Set the NAVOptions object from the HTML controls
                if (-1 != parent.NAVOptions.OfficePlugin)
                parent.NAVOptions.OfficePlugin = Math.abs(document.options_adv_misc.OfficeProtection.checked);
                parent.NAVOptions.DEFALERT_EnableOldDefs = Math.abs(document.options_adv_misc.DefAlert.checked);
                parent.NAVOptions.SymProtectEnabled = Math.abs(document.options_adv_misc.SymProtect.checked);
                parent.NAVOptions.SUBMISSIONS_FeatureEnabled = Math.abs(document.options_adv_misc.AutoSubmit.checked);
                
                if(parent.NAVOptions.PasswordSupported)
                {
                
                    if (document.options_adv_misc.EnablePassword.checked == false)
                    {
                        try
                        {
                            NAVPass.Enabled = false;
                        }
                        catch(err)
                        {
                            parent.g_ErrorHandler.DisplayNAVError ( document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,  
                                                        document.frames("Errors").ERROR_CALLING_PASSWORD_FUNCTION_ID.innerText);
                            // Bail Out
                            return;
                        }
                        
                        /*
                        / clear the password if default page has been clicked
                        if (parent.defPassSet)
                        {
                            if (parent.ClearPass)
                            {
                                NAVpass.Clear();
                            }
                        }
                        */
                        
                    }
                    else
                    {
                        try
                        {
                            NAVPass.Enabled = true;
                        }
                        catch(err)
                        {
                            parent.g_ErrorHandler.DisplayNAVError ( document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,  
                                                        document.frames("Errors").ERROR_CALLING_PASSWORD_FUNCTION_ID.innerText);
                            // Bail Out
                            return;
                        }
                    }
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
                parent.NAVOptions.DEFALERT_EnableOldDefs();
                parent.NAVOptions.SUBMISSIONS_FeatureEnabled();
        
                // Default value for SymProtect is on
                parent.NAVOptions.SymProtectEnabled = true;
        
                if ( parent.NAVOptions.PasswordSupported && typeof (NAVPass) != 'undefined' )
                {
                    try
                    {
                        if( NAVPass.Enabled )
                        {
                            parent.defPassSet = true;
                            document.options_adv_misc.EnablePassword.checked = false;
                            ButtonDisable(document.options_adv_misc.SetPassword);
                            
                            if( NAVPass.UserPasswordSet )   // If the password was set then prepare to unset it
                            {
                                parent.ClearPass = true;
                            }
                        }
                        
                    }
                    catch(err)
                    {
                        document.options_adv_misc.EnablePassword.checked = false;
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
            {
                body_options_adv_misc.scroll = "auto";
                document.options_adv_misc.SetPassword.style.border = '1px solid white';
            }
      
            if( parent.NAVOptions.Is64Bit )
            {
                document.all.group_SymProtect.style.display = 'none';
            }
            
        try
            {
                // Set these HTML controls from the NAVOptions object
                if (-1 != parent.NAVOptions.OfficePlugin)
                document.options_adv_misc.OfficeProtection.checked = parent.NAVOptions.OfficePlugin;
                else  // Office 2K is not installed
                {
                document.options_adv_misc.OfficeProtection.disabled = true;
                document.all.OfficeInstallStatus.style.display = '';
                }
            
                document.options_adv_misc.DefAlert.checked = parent.NAVOptions.DEFALERT_EnableOldDefs;
                document.options_adv_misc.AutoSubmit.checked = parent.NAVOptions.SUBMISSIONS_FeatureEnabled;
            
            if( parent.NAVOptions.SystemMode )
                {
                    document.options_adv_misc.EnablePassword.disabled = true;
                    ButtonDisable(document.options_adv_misc.SetPassword);
                }
                else if ( parent.NAVOptions.PasswordSupported && typeof (NAVPass) != 'undefined' )
                {
                    document.options_adv_misc.EnablePassword.checked = NAVPass.Enabled;
                    if (NAVPass.Enabled)
                    {
                        ButtonEnable(document.options_adv_misc.SetPassword);
                    }
                    else
                    {
                        ButtonDisable(document.options_adv_misc.SetPassword);
                    }
                }
                document.options_adv_misc.SymProtect.checked = parent.NAVOptions.SymProtectEnabled;

                // Set focus to first element
                document.options_adv_misc.DefAlert.focus();
            }
            catch(err)
            {
                parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
            }
            
            parent.defPassSet = false;
            
    }  // end of Initialize()

    // Initialize the from with the options value without modifying the password settings
    this.InitializeNoPassword =  function()
    {
    
            // Enable scrolling for high-contrast mode
            if( parent.NAVOptions.IsHighContrastMode )
            {
                body_options_adv_misc.scroll = "auto";
                document.options_adv_misc.SetPassword.style.border = '1px solid white';
            }
      
        try
            {
                // Set these HTML controls from the NAVOptions object
                if (-1 != parent.NAVOptions.OfficePlugin)
                document.options_adv_misc.OfficeProtection.checked = parent.NAVOptions.OfficePlugin;
                else  // Office 2K is not installed
                {
                document.options_adv_misc.OfficeProtection.disabled = true;
                document.all.OfficeInstallStatus.style.display = '';
                }
            
                document.options_adv_misc.DefAlert.checked = parent.NAVOptions.DEFALERT_EnableOldDefs;
                document.options_adv_misc.AutoSubmit.checked = parent.NAVOptions.SUBMISSIONS_FeatureEnabled;
            document.options_adv_misc.SymProtect.checked = parent.NAVOptions.SymProtectEnabled;

                // Set focus to first element
                document.options_adv_misc.DefAlert.focus();
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
    if(!parent.NAVOptions.PasswordSupported)
    {
        return;
    }
    
    // Don't allow this in safe mode
        if( !parent.NAVOptions.SystemMode )
        {
        // First create the ccPwd object
            try 
            {
                var NAVPass = new ActiveXObject("Symantec.isPassword");
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
                NAVPass.ProductID = parent.NIS_CONSUMER_PASSWORD_ID;
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
                if( document.options_adv_misc.EnablePassword.checked )
                {
                // Prompt if it's the first time
                    try
                    {
                        var bAlreadySet = NAVPass.UserPasswordSet;
                    }
                    catch(err)
                    {
                        document.options_adv_misc.EnablePassword.checked = false;
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
                                document.options_adv_misc.EnablePassword.checked = false;
                            parent.g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,  
                                                        document.frames("Errors").ERROR_CALLING_PASSWORD_FUNCTION_ID.innerText);
                                // Bail Out
                                return;
                        }
                        try
                        {
                                // Check the return value from the Set() method and set the Enabled
                                // if it was successfully set
                                if( bSet  )
                            {
                                    parent.defPassSet = false;
                                    parent.ClearPass = false;
                            }
                            else    // Password was not set...bail out
                            {
                                document.options_adv_misc.EnablePassword.checked = false;
                                return;
                            }
                        }
                        catch(err)
                        {
                                document.options_adv_misc.EnablePassword.checked = false;
                            parent.g_ErrorHandler.DisplayNAVError ( document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,  
                                                        document.frames("Errors").ERROR_CALLING_PASSWORD_FUNCTION_ID.innerText);                                                       
                                // Bail Out
                                return;
                        }
                }

                    // Enable the password change button and the state to enabled
                    ButtonEnable(document.options_adv_misc.SetPassword);
            }
            else // Password is being disabled
                {
                try
                    {
                        // Store the disabled setting
                        
                    }
                    catch(err)
                    {
                        // Have to leave password enabled checked since we couldn't set the disabled property
                        document.options_adv_misc.EnablePassword.checked = true;
                        parent.g_ErrorHandler.DisplayNAVError ( document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,  
                                                    document.frames("Errors").ERROR_CALLING_PASSWORD_FUNCTION_ID.innerText);
                        // Bail Out
                        return;
                    }

                // Disable the button to change the password
                    document.options_adv_misc.EnablePassword.checked = false;
                    ButtonDisable(document.options_adv_misc.SetPassword);
            }
        }
            catch(PassError)
            { 
                parent.g_ErrorHandler.DisplayException (PassError);
                return;
            }

    }
}

function ChangePass()
{
    if(!parent.NAVOptions.PasswordSupported)
    {
        return;
    }
    
    // Don't allow this in safe mode
        if( !parent.NAVOptions.SystemMode )
        {
        try
            {
                // Call the ccPwd set function to change the password
                // tell it to check the old password.
                try
                {
                    // Create the password object
                    var NAVPass = new ActiveXObject("Symantec.isPassword");
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
                NAVPass.ProductID = parent.NIS_CONSUMER_PASSWORD_ID;
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
                    if (NAVPass.Set(true))
                    {
                        parent.defPassSet = false;
                        parent.ClearPass = false;
                    }
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
