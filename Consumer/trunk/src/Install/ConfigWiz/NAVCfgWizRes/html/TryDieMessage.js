////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

var SelectFrame = window.parent.frames("bottom");
var TimerID = 0;


function OnLoad()
{
	document.body.tabIndex=-1;
	
	if (SelectFrame.g_bSelectFrameLoaded == true)
	{
		UpdatePage();
	}
	else
	{
		TimerID = setInterval("UpdatePage()", 500);
	}
	
	// Get NAVOptionObj to detect high contrast settings.
	// Non-fatal if object is not obtained.
	try
    {
		NAVOptionObj = new ActiveXObject ("Symantec.Norton.AntiVirus.NAVOptions");
    }
    catch(err)
    {   
        return;
    }
}

function UpdatePage()
{
	if (SelectFrame.g_bSelectFrameLoaded == true)
	{
		clearInterval(TimerID);
		
		ButtonEnable(SelectFrame.NextButton);
		SelectFrame.NextButton.focus();
		
		if ( SelectFrame.g_iDaysRemaining > 0 )
			FullTryDieMsg.innerText = BeginTryDieMsg.innerText + SelectFrame.g_iDaysRemaining + " " + EndTryDieMsg.innerText;
		else
		{
			FullTryDieMsg.innerText = ExpiredMsg.innerText;
		}

		ButtonDisable(SelectFrame.NextButton);
		SelectFrame.SkipButton.style.visibility = "visible";
		ButtonEnable(SelectFrame.SkipButton);
		SelectFrame.SkipButton.innerHTML = SelectFrame.FinBtnText.innerHTML;
		SelectFrame.SkipButton.accessKey = "f";		
		SelectFrame.bPromptUser = false;
		
		FullTryDieMsg.style.display = "";
		
		// Keep button high contrast status updated
		SetButtonHighContrast(SelectFrame.BackButton);
		SetButtonHighContrast(SelectFrame.NextButton);
		SetButtonHighContrast(SelectFrame.SkipButton);
	}
	try
    {
		var CfgWizMgrObj = window.external.ObjectArg("NAV_CFGWIZ_MGR_OBJ");
		//Send event to shut down the pre cfgwiz progress dialog
		CfgWizMgrObj.SignalPreCfgwizUI();
    }
    catch(err)
    {
	    return;
    }
}
