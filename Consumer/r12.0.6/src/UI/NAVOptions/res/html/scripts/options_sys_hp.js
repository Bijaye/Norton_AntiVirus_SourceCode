// options_sys_hp.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 129;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Enable / Disable radio buttons
function HomePageActn_disable()
{

	try
	{
	  for (var i = 0; i < document.options_sys_hp.HomePageActn.length; i++)
	  {
		document.options_sys_hp.HomePageActn[i].disabled = document.options_sys_hp.EnableHomePageProtection.disabled
															 ||  !document.options_sys_hp.EnableHomePageProtection.checked;
	  }
	  document.options_sys_hp.HomePageRespondActn.disabled = document.options_sys_hp.EnableHomePageProtection.disabled
															 ||  !document.options_sys_hp.EnableHomePageProtection.checked;
	  
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}

}

// Main page object
function HomePageProtectionPage()
{

	try
	{
	  // No page validation
	  this.Validate = function() { return true; }
      this.Terminate = function()
	  {
		if (!document.options_sys_hp.EnableHomePageProtection.disabled)
		{
            try
            {
		       // Set the NAVOptions object from the HTML controls
		       parent.NAVOptions.HPP.Enabled = Math.abs(document.options_sys_hp.EnableHomePageProtection.checked) + 1;
		       parent.NAVOptions.HPP.Respond = Math.abs(document.options_sys_hp.HomePageRespondActn.checked); //1 = block, 2 = allow
		       if(parent.NAVOptions.HPP.Respond == 0)
		       {
					parent.NAVOptions.HPP.Respond = 2;
		       }
		       if(Math.abs(document.options_sys_hp.HomePageActn[1].checked) == 1)
		        {
					parent.NAVOptions.HPP.Control = 1;     //1 = When Home Page Changes
				}
				else  
				if(Math.abs(document.options_sys_hp.HomePageActn[2].checked) == 1)
				{	
					parent.NAVOptions.HPP.Control = 3;    //3 = Never
				}
				else 
				{
					parent.NAVOptions.HPP.Control = 2;	   //2  = On IE Startup, Default
				}		
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
    		parent.NAVOptions.Help(250050); 
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
		    // Set NAVOptions object back to defaults
		    if (parent.IsTrialValid)       // && parent.NAVOptions.HaveScriptBlocking)
		      parent.NAVOptions.HPP.Enabled();
		    parent.NAVOptions.HPP.Respond();
		    parent.NAVOptions.HPP.Control();
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
	            body_options_sys_hp.scroll = "auto";
	            
	        // Set product name
	        prodname.innerText = parent.NAVOptions.ProductName;
		   
	        
	        if (parent.IsTrialValid)      
		    {
				if(parent.NAVOptions.HPP.Enabled == 2)
				{
					document.options_sys_hp.EnableHomePageProtection.checked = true;
				}
				else
				{
					document.options_sys_hp.EnableHomePageProtection.checked = false;
				}
		    }
		    else
		    {
		       // Invalid trialware. disable everything.
		       document.options_sys_hp.EnableHomePageProtection.checked = false;
		       document.options_sys_hp.EnableHomePageProtection.disabled = true;
		    }
		    if(parent.NAVOptions.HPP.Respond == 1)
		    {
				document.options_sys_hp.HomePageRespondActn.checked = true;
			}
			else
			{
				document.options_sys_hp.HomePageRespondActn.checked = false;
			}			    
			if ( Math.abs(parent.NAVOptions.HPP.Control) == 1 )
			    document.options_sys_hp.HomePageActn[1].checked = true;
		    else if( Math.abs(parent.NAVOptions.HPP.Control) == 3 )
			    document.options_sys_hp.HomePageActn[2].checked = true;
			else 
			{
				document.options_sys_hp.HomePageActn[0].checked = true;
			}
			  
		    // Set focus to first element
		    if (parent.IsTrialValid) // && parent.NAVOptions.HomePageProtection  
		      document.options_sys_hp.EnableHomePageProtection.focus();

	    }
	    
         catch(err)
         {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
         }
         HomePageActn_disable();
	  }  // End of Initialize()
      
	  // Initialize this page
	  this.Initialize();
	}
	
	catch (err)
	{
		parent.g_ErrorHandler.DisplayException (err);
		return;
	}
	
}  // end function HomePageProtectionPage()
