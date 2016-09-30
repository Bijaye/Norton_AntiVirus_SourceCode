g_ModuleID = 3009;
g_ScriptID = 404;  // This uniquely defines this script file to the error handler
var lOEM = 0;
var MainFrame = window.parent;
var CfgWizMgrObj;

var DJSMAR_LicenseType_Violated         = -1;
var DJSMAR_LicenseType_Retail           = 0;
var DJSMAR_LicenseType_Trial            = 1;
var DJSMAR_LicenseType_Rental           = 2;
var DJSMAR_LicenseType_TryDie           = 3;
var DJSMAR_LicenseType_Beta             = 4;
var DJSMAR_LicenseType_Unlicensed       = 5;
var DJSMAR_LicenseType_ESD              = 6;

// Licensing state
var DJSMAR00_LicenseState_PURCHASED = 1;

MainFrame.g_WebWnd = null;
MainFrame.bPromptUser = true;
MainFrame.bPromptClose = true;
MainFrame.g_iLicenseType = DJSMAR_LicenseType_Retail;
MainFrame.g_iLicenseState = -2;		// violated
MainFrame.g_strCancelMsg = null;
MainFrame.g_bSilentMode = 0;

function OnLoad()
{
    try
    {
        MainFrame.g_WebWnd = window.external.ObjectArg("CFGWIZ_WEBWINDOW_OBJECT");
        MainFrame.g_WebWnd.OnCloseCallback = OnCloseButton;
    }
    catch(err)
    {
        var msg = document.frames("Errors").document.all.ERR_INTERNAL_ERROR.innerText;
        var id = document.frames("Errors").document.all.ERR_ID_NO_WEBWND.innerText;
        g_ErrorHandler.DisplayNAVError (msg, id);
        return;
    }
    
    try
    {
        CfgWizMgrObj = window.external.ObjectArg("NAV_CFGWIZ_MGR_OBJ");  
    }
    catch(err)
    {
        var msg = document.frames("Errors").document.all.ERR_INTERNAL_ERROR.innerText;
        var id = document.frames("Errors").document.all.ERR_ID_NO_CFGWIZDLL.innerText;
        g_ErrorHandler.DisplayNAVError (msg, id);       
        return;
    }
    
    try
    {
        // Check if we're running in SilentMode... if so, do not prompt close message.
        if (CfgWizMgrObj.SilentMode)
        {
            MainFrame.bPromptUser = false;
        }
        
        // Check if OEM sku
        lOEM = CfgWizMgrObj.ProductType;

        try
        {                       
            // Load background image from AVRES.
            intro_table.style.backgroundImage = "url(\'res://" + CfgWizMgrObj.NortonAntiVirusPath + "\\avres.dll/CfgWizWelcomeBkg.gif\')";
        }
        catch(err)
        {
            CfgWizMgrObj.NAVError.LogAndDisplay(0);                         
        }                       
             
        g_strProductName = CfgWizMgrObj.GetBrandingText(4);       
        MainFrame.g_strCancelMsg = CancelWarning.innerText.replace(/%s/, g_strProductName);        
        
        // we've loaded up the background image, now wait for our preblock thread to finish
        try
	    {   
		    if(!CfgWizMgrObj.WaitForLicenseData() || 
			    !window.external.ObjectArg("NAV_CFGWIZ_PREBLOCK_RESULT"))
		    {
			    //fail silently...
			    MainFrame.navigate("res://closeme.xyz");
		    }		    
	    }
	    
	    catch(err)
	    {
            return;
	    }
        
        // If OEM layouts (LimitedOEM, CTO and PP), display OEM text.
        if (lOEM != 0)
        {
            // hide non-OEM text                
            text_table_cell1.style.display = "none";
            text_table_cell2.style.display = "none";
            p_ClickNext.style.display = "none";
            text_table_cell2_oem.style.display = "";  
            
			// Format text that contains product name.
			oem_text_table1.innerText = oem_text_table1.innerText.replace(/%s/g, g_strProductName);			                          
			
			if ( (lOEM == 2) || (lOEM == 3) ) //if CTO or PP	
			{                          
				// Prepaid/CTO - If license state is PURCHASED (i.e. reinstall product that has already been activated),
				//  assume EULA had been accepted because EULA panel will not be displayed in roadmap
				//  if license state is PURCHASED
				MainFrame.g_iLicenseState = window.external.ObjectArg("DRM::LicenseState");
				if (MainFrame.g_iLicenseState == DJSMAR00_LicenseState_PURCHASED)
					CfgWizMgrObj.AgreedToEULA = true;
			}
        }   
        else
        {
            text_table_cell2_spacer.style.display = "none";
        }
        
		// Hide IWP text if IWP is not installed.
		if(CfgWizMgrObj.IWPInstalled == false)
		{
			text_table_bullet2.style.display = "none";		
		}
        
		MainFrame.g_iLicenseType = window.external.ObjectArg("DRM::LicenseType");
			
        if(DJSMAR_LicenseType_Rental == MainFrame.g_iLicenseType)
        {             	
            var nTotalText;
            try
            {
                // Check to see if there is branding text.
                nTotalText = CfgWizMgrObj.TotalBrandingText;
            }
            catch(err)
            {
                CfgWizMgrObj.NAVError.LogAndDisplay(0);
            }
            
            // Get branding text from AVRES.             
            if(nTotalText == 0)
            {
                // Hide all bullets.
                //  (i.e. TOnline does not want to display any text in the welcome screen.)
                text_table_bullet1.style.display = "none";
                text_table_bullet2.style.display = "none";

            }
            else if(nTotalText > 0)
            {
                
                // Get branding text from AVRES.
                var aText = new Array(nTotalText);
                
                for(var i=0; i < nTotalText; i++)
                {
                    aText[i] = CfgWizMgrObj.GetBrandingText(i);             
                }
                
                switch(nTotalText)
                {                                                           
                    
                case 2:
                    {
						if(aText[0].length != 0)
							text_table_bullet1.innerText = aText[0];

						if(aText[1].length != 0)
						{
							text_table_bullet2.innerText = aText[1];
							text_table_bullet2.style.display = "";
						}

                    }
                    break;
                    
                case 1:
                    {
                        if(aText[0].length != 0)
                            text_table_bullet1.innerText = aText[0];
                            
                        // Hide the last two bullets.
                        text_table_bullet2.style.display = "none";
                    }
                    break;                          
                
                default:
                    break;
                }
            }               
        }
        						
        intro_table.style.backgroundRepeat = "no-repeat";
    }
    catch(err)
    {
        CfgWizMgrObj.NAVError.LogAndDisplay (0);
        window.navigate("res://closeme.xyz");
        return;
    }


    // Make sure we're at the top
    document.body.scrollTop = 0;
    document.body.focus();
}

function OnFocus()
{
    NextButton.focus();
}

function NextPage()
{          
    try
    {   
         try
        {
            CfgWizMgrObj = window.external.ObjectArg("NAV_CFGWIZ_MGR_OBJ");  
        }
        catch(err)
        {
            var msg = document.frames("Errors").document.all.ERR_INTERNAL_ERROR.innerText;
            var id = document.frames("Errors").document.all.ERR_ID_NO_CFGWIZDLL.innerText;
            g_ErrorHandler.DisplayNAVError (msg, id);       
            return;
        }
		
		var bShowSSC = CfgWizMgrObj.ShowSSCOption;
		var bShowICF = CfgWizMgrObj.ShowICFCheckbox && CfgWizMgrObj.ICFEnabled && CfgWizMgrObj.IWPInstalled;
		
		if(bShowSSC || bShowICF)
		{       
			// Navigate to frameset page with the tasks
			window.location.replace("home.htm");
		}
		else
		{
			// Navigate to frameset page without the
			window.location.replace("home_NoTasks.htm");
		}
		
    }
    catch(err)
    {
        // Fatal error
        var msg = document.frames("Errors").document.all.ERR_INTERNAL_ERROR.innerText;
        var id = document.frames("Errors").document.all.ERR_ID_FATAL_DJS.innerText;
        g_ErrorHandler.DisplayNAVError (msg, id);
        return;
    }
    
    // Pass license sub type to the next page.
    window.external.Global = window.external.ObjectArg;
}

function OnScroll ()
{
    document.body.scrollTop = 0;
}

function OnCloseButton()
{
    // MessageBox() Flags
    var MB_YESNO =                  0x00000004;
    var MB_ICONQUESTION =           0x00000020;
    var IDYES =             6;
    var IDNO  =             7;

    if (MainFrame.m_nCurrentPanel == MainFrame.m_nMaxPanels - 1)
    {
        MainFrame.navigate("res://closeme.xyz");
        return true;
    }
    else
    {
        // Prompt users to see if they really wanna cancel
        if( MainFrame.bPromptUser == true )
        {
            if (MainFrame.bPromptClose == true)
            {
                MainFrame.bPromptClose = false;
                
                iMsgBoxReturn = MainFrame.g_WebWnd.MsgBox(MainFrame.g_strCancelMsg, MainFrame.g_strProductName, MB_YESNO | MB_ICONQUESTION);

                if (iMsgBoxReturn == IDYES)
                {                   					
                    MainFrame.bPromptClose = true;
                    MainFrame.navigate("res://closeme.xyz");
                    return true;
                }
                else if (iMsgBoxReturn == IDNO)
                {
                    MainFrame.bPromptClose = true;
                }
                    
                return false;
            }
            else
                return false;
        }
        else
            MainFrame.navigate("res://closeme.xyz");
    }
    	
    return true;
}
