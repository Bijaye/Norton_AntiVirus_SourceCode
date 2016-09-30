var MainFrame = window.parent;
var TimerID = 0;


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
		clearInterval(TimerID);
		
		var SelectFrame = window.parent.frames("bottom");
		
		SelectFrame.NextButton.disabled = false;
		SelectFrame.NextButton.focus();
		
		if ( MainFrame.g_iDaysRemaining > 0 )
			FullTryDieMsg.innerText = BeginTryDieMsg.innerText + MainFrame.g_iDaysRemaining + " " + EndTryDieMsg.innerText;
		else
		{
			FullTryDieMsg.innerText = ExpiredMsg.innerText;
			SelectFrame.NextButton.disabled = true;
			SelectFrame.SkipButton.style.visibility = "visible";
			SelectFrame.SkipButton.disabled = false;
			SelectFrame.SkipButton.innerHTML = SelectFrame.FinBtnText.innerHTML;
			SelectFrame.SkipButton.accessKey = "f";
		}
		
		MainFrame.bPromptUser = false;
		
		FullTryDieMsg.style.display = "";
	}
}
