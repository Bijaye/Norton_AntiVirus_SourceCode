// options_int_lu.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 111;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Enable/disable the radio buttons
function disable_apply_updates(varDisable)
{
    try
	{
	   // Disable/Enable the how to update radio controls for Virus Defs
	   for (var i = 0; i < document.options_int_lu.ApplyUpdates.length; i++)
	   {
		  if( varDisable || !document.options_int_lu.ApplyVirusUpdates.checked )
			document.options_int_lu.ApplyUpdates[i].disabled = true;
		  else
			document.options_int_lu.ApplyUpdates[i].disabled = false;
	   }

	   // Disable/Enable the what to update checkboxes if they've been
       // initialized
       if( -1 != parent.InitialALUProduct )
	      document.options_int_lu.ApplyProductUpdates.disabled = varDisable;

       if( -1 != parent.InitialALUVirusdefs )
	      document.options_int_lu.ApplyVirusUpdates.disabled = varDisable;
	}
	catch (err)
	{
		top.snoozeLU = false;
		g_ErrorHandler.DisplayException (err);
		return;
	}
}

function OnEnableALUClick(status)
{
	disable_apply_updates(!status);
}

function OnEnableALUChanged(status)
{
	if (top.luInitialized)
		top.snoozeLU = !status;
}

// OnClick handler for the Virusdef update checkbox
function VirusUpdatesChk()
{
	try
	{
		if( document.options_int_lu.ApplyVirusUpdates.checked )
		{
			// Enable the apply update modes for the virus defs
			for (var i = 0; i < document.options_int_lu.ApplyUpdates.length; i++)
			{
			   document.options_int_lu.ApplyUpdates[i].disabled = false;
			}
		}
		else
		{
			// Disable the apply update modes for the virus defs
			for (var i = 0; i < document.options_int_lu.ApplyUpdates.length; i++)
			{
			   document.options_int_lu.ApplyUpdates[i].disabled = true;
			}
		}
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}

// Main page object
function LiveUpdatePage()
{
   try
   {
	   // No page validation
	   this.Validate = function() { return true; }

	   // save options values from the screen
	   this.Terminate = function()
	   {
		  // Set the NAVOptions object from the HTML controls
		  try
          {
		    if (!document.options_int_lu.AutomaticLiveUpdate.disabled && !parent.NAVOptions.ALUrunning && !parent.ALUhosed)
		    {
			    parent.NAVOptions.LiveUpdate = Math.abs(document.options_int_lu.AutomaticLiveUpdate.checked);
			    parent.NAVOptions.LiveUpdateMode = document.options_int_lu.ApplyUpdates[0].checked ? 3 : 0;
			    parent.NAVOptions.ALU.Product = Math.abs(document.options_int_lu.ApplyProductUpdates.checked);
			    parent.NAVOptions.ALU.Virusdefs = Math.abs(document.options_int_lu.ApplyVirusUpdates.checked);
            }
            parent.NAVOptions.SCANNER.DefUpdateScan = Math.abs(document.options_int_lu.SideEffectScan.checked);
		  }
		  catch(err)
		  {
		  }
	   }  // End of Terminate()

	  // Provide help for this page
	  this.Help = function()
	  {
        try
        {
		    parent.NAVOptions.Help(33041); // IDH_NAVW_AUTOUPDATE_SETTINGS_HELP_BTN
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
          if( !parent.NAVOptions.ALUrunning && !parent.ALUhosed )
          {
		      parent.NAVOptions.LiveUpdate();
		      parent.NAVOptions.LiveUpdateMode();
		      parent.NAVOptions.ALU.Product();
		      parent.NAVOptions.ALU.Virusdefs();
          }
          parent.NAVOptions.SCANNER.DefUpdateScan();
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
	        body_lu.scroll = "auto";
	        
	    // Format product name in the page
	    productname.innerText = parent.NAVOptions.ProductName;
	    lu_howtokeepproduct.innerText = lu_howtokeepproduct.innerText.replace(/%s/, parent.NAVOptions.ProductName);
	    lu_howtokeepproduct.style.fontWeight = "bold";
	    	        
        // Get the ALU running status
        var isALURunning = false;
        try
        {
            isALURunning = parent.NAVOptions.ALUrunning;
        }
        catch(err)
        {
            // On an error assume not running, this will never happen!
            isALURunning = false;
        }

        try
        {
            if( isALURunning && !parent.ALUhosed)
            {
                // Need to make a call to attempt to reset the ALU running status
                // as long as ALU isn't hosed
                var ProdSubStr;
                if( parent.Is9x )
                    ProdSubStr = "NAV95";
                else
                    ProdSubStr = "NAVNT";
                            
                test = parent.NAVOptions.IsAluOn(ProdSubStr);

                try
                {
                    isALURunning = parent.NAVOptions.ALUrunning;
                }
                catch(err)
                {
                    isALURunning = false;
                }
            }
        }
        catch(err)
        {
        }

        if( !isALURunning || parent.ALUhosed )
        {
            if( !parent.ALUhosed )
            {
                try
                {
                    // Ensure the initial Product and VirusDefs ALU status has already been retrieved
                    if( -1 == parent.InitialALUProduct )
                    {
                        var ProdSubStr;
                        if( parent.Is9x )
                            ProdSubStr = "NAV95";
                        else
                            ProdSubStr = "NAVNT";
                        if( -1 != (parent.InitialALUProduct = parent.NAVOptions.IsAluOn(ProdSubStr)) )
                            parent.NAVOptions.ALU.Product = parent.InitialALUProduct;
                    }
                }
                catch(err)
                {
                    parent.ALUhosed = 1;
                }
                try
                {
                    if( -1 == parent.InitialALUVirusdefs )
                    {
                        if( -1 != (parent.InitialALUVirusdefs = parent.NAVOptions.IsAluOn("MicroDefs")) )
                            parent.NAVOptions.ALU.Virusdefs = parent.InitialALUVirusdefs;
                    }
                }
                catch(err)
                {
                    parent.ALUhosed = 1;
                }
            }

		    // Set these HTML controls from the NAVOptions object
		    //with(document.options_int_lu)
		    {
              try
              {
		          if (-1 != parent.NAVOptions.LiveUpdate)
		          {
			        document.options_int_lu.AutomaticLiveUpdate.checked = parent.NAVOptions.LiveUpdate;
		          }
                  else
                  {
                    parent.ALUhosed = 1;
                    document.options_int_lu.AutomaticLiveUpdate.disabled = true;
                    document.all.ALUError.style.display = '';
                  }
              }
              catch(err)
              {
                parent.ALUhosed = 1;
                parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
                document.options_int_lu.AutomaticLiveUpdate.disabled = true;
                document.all.ALUError.style.display = '';
              }

              if( -1 != parent.InitialALUProduct )
              {
                try
                {
                    document.options_int_lu.ApplyProductUpdates.checked = parent.NAVOptions.ALU.Product;
                }
                catch(err)
                {
                    parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
                }
              }
              else
              {
                document.options_int_lu.ApplyProductUpdates.disabled = true;
              }

              if( -1 != parent.InitialALUVirusdefs )
              {
                try
                {
                    document.options_int_lu.ApplyVirusUpdates.checked = parent.NAVOptions.ALU.Virusdefs;
                }
                catch(err)
                {
                    parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
                }
              }
              else
              {
                document.options_int_lu.ApplyVirusUpdates.disabled = true;
              }

              var LUMode;

              try
              {
                LUMode = parent.NAVOptions.LiveUpdateMode;
              }
              catch(err)
              {
                parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
              }

		      // Set the Virusdefs update mode
		      switch (LUMode)
		      {
		      case 0: // Prompt for updates
			     document.options_int_lu.ApplyUpdates_1.checked = true;
			     break;
		      case -1:
			     //Uninitialized
			     break;
		      default: // Silent updates, Mode should be 3
			     document.options_int_lu.ApplyUpdates_0.checked = true;
			     break;
		      }

		      // Set focus to first element
		      if (!document.options_int_lu.AutomaticLiveUpdate.disabled)
			    document.options_int_lu.AutomaticLiveUpdate.focus();

              disable_apply_updates(!document.options_int_lu.AutomaticLiveUpdate.checked | document.options_int_lu.AutomaticLiveUpdate.disabled);
            }
          }
          else // ALU is running so blow everything away for now
          {
            document.options_int_lu.AutomaticLiveUpdate.disabled = true;
            document.options_int_lu.ApplyProductUpdates.disabled = true;
	        document.options_int_lu.ApplyVirusUpdates.disabled = true;
            disable_apply_updates(true);
            document.all.ALUNotRunning.style.display = 'none';
            document.all.ALURunning.style.display = '';
          }
          
          document.options_int_lu.SideEffectScan.checked = parent.NAVOptions.SCANNER.DefUpdateScan;
	  }  // end of Initialize()

	  // Initialize this page
	  this.Initialize();
	  top.luInitialized = true;
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}  // end function LiveUpdatePage()
