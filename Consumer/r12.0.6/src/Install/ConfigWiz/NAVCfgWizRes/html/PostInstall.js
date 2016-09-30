var MainFrame = window.parent;
var TimerID = 0;
g_ModuleID = 3009;
g_ScriptID = 402;  // This uniquely defines this script file to the error handler

function OnLoad()
{
	document.body.tabIndex=-1;		
	if (MainFrame.g_bRoadmapFrameLoaded == true && MainFrame.g_bSelectFrameLoaded == true)
	{
		UpdatePage();
	}
	else
	{
		TimerID = setInterval("UpdatePage()", 500);
	}
}

function UpdatePage()
{
	if (MainFrame.g_bRoadmapFrameLoaded == true && MainFrame.g_bSelectFrameLoaded == true)
	{		
		// Remove the timer, we're done.
		clearInterval(TimerID);
	
		// Format text that contains product name.
		prodname.innerText = MainFrame.g_strProductName;
		//ICFText.innerText = ICFText.innerText.replace(/%s/, MainFrame.g_strProductName);			
		
		var SelectFrame = window.parent.frames("bottom");
		
		// If the Next button is disabled, enable it.
		if (SelectFrame.NextButton.disabled == true)
			SelectFrame.NextButton.disabled = false;
		
		// Set focus to the Next button.
		SelectFrame.NextButton.style.visibility = "visible";	
		SelectFrame.NextButton.focus();
		
		
		// we have all the ids hidden in this page by default so
		// that the page appears blank while we are loading,
		// otherwise it looks funny when everything loads up and
		// then dissapears.
		PostInstallPageTitle.style.display = "";
		
		if(MainFrame.g_bShowDisableICF == true)
		{
			ICFGroup.style.display = "";
			
			// Check for the default state of ICF checkbox
			if (MainFrame.g_bDisableICF == true)
				ICF.checked = true;
			else
				ICF.checked = false;
		}
			
		if(MainFrame.g_bShowEnableSSC == true)
		{
			SSCGroup.style.display = "";
			
			if (MainFrame.g_bEnableSSC == true)
			{
				SSC.checked = true;
			}
			else
			{
				SSC.checked = false;
			}
		}
		else
		{
			MainFrame.g_bEnableSSC = false;
		}
	}
}

function OnICFClick()
{
	if (ICF.checked == true)
	{
		MainFrame.g_bDisableICF = true;
	}
	else
	{
		MainFrame.g_bDisableICF = false;
	}
}

function OnSSCClick()
{
	if (SSC.checked == true)
	{
		MainFrame.g_bEnableSSC = true;
	}
	else
	{
		MainFrame.g_bEnableSSC = false;
	}
}
