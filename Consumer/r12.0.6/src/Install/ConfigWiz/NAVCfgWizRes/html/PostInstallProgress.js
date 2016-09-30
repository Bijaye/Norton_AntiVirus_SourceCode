var iTimerID;
var MainFrame = window.external.Global;

function OnLoad()
{
	document.body.tabIndex=-1;
	window.title = MainFrame.g_strWizardName;	
	prodname.innerText = MainFrame.g_strProductName;
	//PostInstallProgress_table_cell2.innerText = PostInstallProgress_table_cell2.innerText.replace(/%s/, MainFrame.g_strProductName);	
	iTimerID = setInterval("Run()", 500);
}

function Run()
{
	// Kill the timer

	clearInterval(iTimerID);

	// Activate NAV with the selected settings

	MainFrame = window.external.Global;
	MainFrame.frames("contents").PostInstallWorker();

	// Close the window

	window.navigate("res://closeme.xyz");
}