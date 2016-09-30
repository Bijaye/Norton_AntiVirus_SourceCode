////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

g_ModuleID = 3009;
g_ScriptID = 404;  // This uniquely defines this script file to the error handler
var lOEM = 0;
var bShowDisableDefenderWindowBox = 1;
var bDefaultValueOfDefenderWindowBox = 1;
var MainFrame = window.parent;
var CfgWizMgrObj;

//License Type
var cltLicenseType_Violated = 0x00000001;   // Violated
var cltLicenseType_SOS = 0x00000040;   // SOS is Rental model (SCSS)

//Licensing attributes
var cltLicenseAttrib_Trial = 0x00000008;
var cltLicenseAttrib_Active = 0x00000010;

g_WebWnd = null;
g_iLicenseType = cltLicenseType_Violated;
g_iLicenseAttribs = 0;
g_strCancelMsg = null;

var	b_IsHighContrastMode = 0;

function OnLoad()
{
    try
    {
        g_WebWnd = window.external.ObjectArg("CFGWIZ_WEBWINDOW_OBJECT");
        g_WebWnd.OnCloseCallback = OnCloseButton;
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
    
    // Load the scriptable NAV Options optject
    try
    {
        NAVOptionObj = new ActiveXObject ("Symantec.Norton.AntiVirus.NAVOptions");
    }
    catch(err)
    {
	var msg = document.frames("Errors").document.all.ERR_INTERNAL_ERROR.innerText;
	var id = document.frames("Errors").document.all.ERR_ID_NO_OPTIONS.innerText;
	g_ErrorHandler.DisplayNAVError (msg, id);
	return;
    }
    
	// Set buttons to high contrast mode if needed
	SetButtonHighContrast(NextButton);
	
    
    try
    {
        // Check if OEM sku
        lOEM = CfgWizMgrObj.ProductType; 
		bShowDisableDefenderWindowBox  = CfgWizMgrObj.bShowDisableWDCheckBox;
		bDefaultValueOfDefenderWindowBox = CfgWizMgrObj.bDefaultValueForDisableWD;
		
        try
        {                       
            // Load background image from AVRES.
            body_intro.style.backgroundImage = "url(\'res://" + CfgWizMgrObj.NortonAntiVirusPath + "\\isres.dll/CfgWizBg.png\')";
        }
        catch(err)
        {
            CfgWizMgrObj.NAVError.LogAndDisplay(0);                         
        }                       

        g_strProductName = CfgWizMgrObj.GetBrandingText(4);       
        g_strCancelMsg = CancelWarning.innerText.replace(/%s/, g_strProductName);        

       // we've loaded up the background image, now wait for our preblock thread to finish
        try
	    {   
		    if(!CfgWizMgrObj.WaitForLicenseData() || 
			    !window.external.ObjectArg("NAV_CFGWIZ_PREBLOCK_RESULT"))
		    {
			    //fail silently...
			    navigate("res://closeme.xyz");
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
            p_ClickNext.style.display = "none";
            bullet_text_div.style.display = "none";
            oem_text_div.style.display = "";
            
			// Format text that contains product name.
			oem_text_table1.innerText = oem_text_table1.innerText.replace(/%s/g, g_strProductName);
        }   
        if(bShowDisableDefenderWindowBox)
        {
			chkArea_1.style.display = "";
			chkDefender.checked = bDefaultValueOfDefenderWindowBox; 
			//alert("bDefaultValueOfDefenderWindowBox " + bDefaultValueOfDefenderWindowBox);
        }
        else
        {
			chkDefender.checked = false; //init value for none oem build
        }
        

		// Hide IWP text if IWP is not installed.
		if(CfgWizMgrObj.IWPInstalled == false)
		{
			text_table_bullet2.style.display = "none";		
		}

		g_iLicenseType = window.external.ObjectArg("clt::LicenseType");

        if(cltLicenseType_SOS == g_iLicenseType)
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
		
        body_intro.style.backgroundRepeat = "no-repeat";
    }
    catch(err)
    {
        CfgWizMgrObj.NAVError.LogAndDisplay (0);
        window.navigate("res://closeme.xyz");
        return;
    }

	//Send event to shut down the pre cfgwiz progress dialog
    CfgWizMgrObj.SignalPreCfgwizUI();
	
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
	if(bShowDisableDefenderWindowBox)
	{
		CfgWizMgrObj.bDefaultValueForDisableWD=chkDefender.checked;    
    }
    try
    {   		
		// Navigate to frameset page without the
		window.location.replace("home_NoTasks.htm");
		
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

    // Prompt users to see if they really wanna cancel
    iMsgBoxReturn = g_WebWnd.MsgBox(g_strCancelMsg, MainFrame.g_strProductName, MB_YESNO | MB_ICONQUESTION);

    if (iMsgBoxReturn == IDYES)
    {
        MainFrame.navigate("res://closeme.xyz");
        return true;
    }
    else
    {
        return false;
    }
}