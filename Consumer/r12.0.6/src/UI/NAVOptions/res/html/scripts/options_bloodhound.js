// options_bloodhound.js
g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 102;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Enable / disable radio buttons
function HeuristicsSensitivity_enable()
{
	try
	{   
	   document.options_bloodhound.HeuristicsSensitivity[0].disabled =
	   document.options_bloodhound.HeuristicsSensitivity[1].disabled =
	   document.options_bloodhound.HeuristicsSensitivity[2].disabled = (!document.options_bloodhound.EnableBloodhoundHeuristics.checked || !document.Enabled);
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}

// Main page object
function BloodhoundPage()
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
		    // Set the NAVOptions object from the HTML controls
		    switch(parent.BloodHound)
		    {
		    case 0:  // Auto-Protect
		      parent.NAVOptions.TSR.EnableHeuristicScan = Math.abs(document.options_bloodhound.EnableBloodhoundHeuristics.checked);

		      //Find the level user selected.
		      if (document.options_bloodhound.HeuristicsSensitivity_0.checked)
		      {
			     //Highest level
			     parent.NAVOptions.TSR.HeuristicLevel = 3;
		      }
		      else if (document.options_bloodhound.HeuristicsSensitivity_2.checked)
		      {
			     //Lowest level
			     parent.NAVOptions.TSR.HeuristicLevel = 1;
		      }
		      else
		      {
			     //Default to options file default value of 2.
			     parent.NAVOptions.TSR.HeuristicLevel = 2;
		      }
		      break;
		    case 1:  // Manual Scan
		     
			  //Find the level user selected.
			  if (document.options_bloodhound.HeuristicsSensitivity_0.checked)
			  {
			    //Highest level
			    parent.NAVOptions.SCANNER.UserHeuristicLevel = 3;
			  }
			  else if (document.options_bloodhound.HeuristicsSensitivity_2.checked)
			  {
			    //Lowest level
			    parent.NAVOptions.SCANNER.UserHeuristicLevel = 1;
			  }
			  else
			  {
			    //Default to options file default value of 2.
			    parent.NAVOptions.SCANNER.UserHeuristicLevel = 2;
			  }
			     
			  // Enable disable the heuristic level
		      if (document.options_bloodhound.EnableBloodhoundHeuristics.checked)
		      {
		        parent.NAVOptions.SCANNER.HeuristicLevel = parent.NAVOptions.SCANNER.UserHeuristicLevel;
		      }
		      else
		      {
			     //User disabled.
			     parent.NAVOptions.SCANNER.HeuristicLevel = 0;
		      }
		      break;
		    case 2:  // E-Mail
		      if (false)
		      {
			     if (typeof(parent.NAVOptions.NAVPROXY.EnableHeuristicScan) == "undefined")
			     {
			     }
			     else
			     {
			       parent.NAVOptions.NAVPROXY.EnableHeuristicScan = Math.abs(document.options_bloodhound.EnableBloodhoundHeuristics.checked);
			       for (var i = 0; i < document.options_bloodhound.HeuristicsSensitivity.length; i++)
			       {
				     // Check whech of the radio buttons is selected
				     if (document.options_bloodhound.HeuristicsSensitivity[i].checked)
				     {
				       parent.NAVOptions.NAVPROXY.HeuristicLevel = i + 1;
				       break;
				     }
			       }
			     }
		      }
		      break;
		    }
        }
        catch(err)
        {
            try
			{
				parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
			}
			catch(err)
			{
				g_ErrorHandler.DisplayException (err);
			}
        }
	  }  // End of Terminate()

	  // Provide help for this page
	  this.Help = function()
	  {
		try
        {
            switch(parent.BloodHound)
		    {
		    case 0:  // Auto-Protect
                parent.NAVOptions.Help(532); // AP Heuristics Help ID
                break;
            case 1:  // Manual Scan
                parent.NAVOptions.Help(531); // Manual Scan Heuristics Help ID
                break;
            case 2: // E-Mail - Not currently defined
                if(false)
                {
                    parent.NAVOptions.Help(0);
                    break;
                }
                break;
                
            }
            
        }
        catch(err)
        {
            try
			{
				parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
			}
			catch(err)
			{
				g_ErrorHandler.DisplayException (err);
			}
        }
	  }

	  // restore default options values
	  this.Default = function()
	  {
        try 
        {
		    switch(parent.BloodHound)
		    {
		    case 0:  // Auto-Protect
		      parent.NAVOptions.TSR.EnableHeuristicScan();
		      parent.NAVOptions.TSR.HeuristicLevel();
		      break;
		    case 1:  // Manual Scan
		      parent.NAVOptions.SCANNER.HeuristicLevel();
		      break;
		    case 2:  // E-Mail
		      if (false)
		      {
			     parent.NAVOptions.NAVPROXY.EnableHeuristicScan;
			     parent.NAVOptions.NAVPROXY.HeuristicLevel();
		      }
		      break;
		    }
        }
        catch(err)
        {
            try
			{
				parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
			}
			catch(err)
			{
				g_ErrorHandler.DisplayException (err);
			}
        }
	  }  // End of Default()

	  // Initialize the from with the options value
	  this.Initialize =  function()
	  {
          try
          {
            // Enable scrolling for high-contrast mode
	        if( parent.NAVOptions.IsHighContrastMode )
	            body_opt_bloodhound.scroll = "auto";
	        
		    // Set these HTML controls from the NAVOptions object
		    switch(parent.BloodHound)
		    {
		    case 0:  // Auto-Protect
		      document.options_bloodhound.EnableBloodhoundHeuristics.checked = parent.NAVOptions.TSR.EnableHeuristicScan;
		      //Find the level user selected.
		      switch (parent.NAVOptions.TSR.HeuristicLevel)
		      {
		      case 1:
			     //Lowest setting
			     document.options_bloodhound.HeuristicsSensitivity_2.checked = true;
			     break;
		      case 3:
			     //highest setting
			     document.options_bloodhound.HeuristicsSensitivity_0.checked = true;
			     break;
		      default:
			     //Default to medium
			     document.options_bloodhound.HeuristicsSensitivity_1.checked = true;
			     break;
		      }
		      document.Enabled = parent.CurAPRunning;
		      break;
		    case 1:  // Manual Scan
		      if (parent.NAVOptions.SCANNER.HeuristicLevel > 0)
		      {
			     //Bloodhound active
			     document.options_bloodhound.EnableBloodhoundHeuristics.checked = 1;
			     switch (parent.NAVOptions.SCANNER.HeuristicLevel)
			     {
			     case 1:
				    //Lowest setting
				    document.options_bloodhound.HeuristicsSensitivity_2.checked = true;
				    break;
			     case 3:
				    //highest setting
				    document.options_bloodhound.HeuristicsSensitivity_0.checked = true;
				    break;
			     default:
				    //Default to medium
				    document.options_bloodhound.HeuristicsSensitivity_1.checked = true;
				    break;
			     }
		      }
		      else
		      {
			     //Bloodhound not active
			     document.options_bloodhound.EnableBloodhoundHeuristics.checked = 0;
         
			     switch (parent.NAVOptions.SCANNER.UserHeuristicLevel)
		         {
		         case 1:
			         //Lowest setting
			         document.options_bloodhound.HeuristicsSensitivity_2.checked = true;
			         break;
		         case 3:
			         //highest setting
			         document.options_bloodhound.HeuristicsSensitivity_0.checked = true;
			         break;
		         default:
			         //Default to medium
			         document.options_bloodhound.HeuristicsSensitivity_1.checked = true;
			         break;
		         }
		      }
		      document.Enabled = true;
		      break;
		    case 2:  // E-Mail
		      if (false)
		      {
			     if (typeof(parent.NAVOptions.NAVPROXY.EnableHeuristicScan) == "undefined")
			     {
			     }
			     else
			     {
			       document.options_bloodhound.EnableBloodhoundHeuristics.checked = parent.NAVOptions.NAVPROXY.EnableHeuristicScan;
			     }
			     document.options_bloodhound.HeuristicsSensitivity[parent.NAVOptions.NAVPROXY.HeuristicLevel - 1].checked = true;
			     document.Enabled = true;
		      }
		      break;
		    }

		    document.options_bloodhound.EnableBloodhoundHeuristics.disabled = !document.Enabled;

		    // Set focus to first element
		    if (!document.options_bloodhound.EnableBloodhoundHeuristics.disabled)
		      document.options_bloodhound.EnableBloodhoundHeuristics.focus();

		    HeuristicsSensitivity_enable();
        }
        catch(err)
        {
            try
			{
				parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
			}
			catch(err)
			{
				g_ErrorHandler.DisplayException (err);
			}
        }
	  }  // end of Initialize()

	  // Initialize this page
	  this.Initialize();
	
	} // end of try
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}  // end function BloodhoundPage()
