
var MainFrame = window.parent;
var Scheduler;
g_ModuleID = 3009;
g_ScriptID = 401;  // This uniquely defines this script file to the error handler
var TimerID = 0;
var DJSMAR00_LicenseType_Rental	= 2;

function OnLoad()
{
	// Cheesy IE hack alert
	// If your page has no tabable controls and you disable tabbing to the body you will
	// crash the webwindow.  Thus I have to enabled tabbing to the Text areas
	document.body.tabIndex=-1;
	
	MainFrame.g_WebWnd.CloseButton = false;
	
	// Display appropriate text if FSS is not run.
	if(MainFrame.g_bFullSystemScan)
	{	
		// Launch Quick Scan for SOS and Full System Scan for non-SOS
		if(MainFrame.g_iLicenseType == DJSMAR00_LicenseType_Rental)
		{
			QuickScanText.style.display = "";
			FSSScanText.style.display = "none";
		}
	}
	else
	{
		sOneTask.style.display = "none";
		sMultiTasks.style.display = "none";
		Scan.style.display = "none";
		// hiding all the liveupdate text
		LiveUpdate.style.display = "none";
	}
	
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
	}
}
