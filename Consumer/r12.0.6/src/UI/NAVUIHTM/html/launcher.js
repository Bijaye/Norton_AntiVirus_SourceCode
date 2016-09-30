// TRACE disabled
var TRACE = false;

if (TRACE)
{
	try
	{
		top.axVkTrace = new ActiveXObject("VkTools.VkTraceCC");
		top.axVkTrace.SetModuleName("NavReportsJs");
	}
	catch (e)
	{
		TRACE = false;
	}
}

function trace(msg)
{
	if (TRACE)
	{
		try
		{
			top.axVkTrace.WriteLine(msg);
		}
		catch (e)
		{
			TRACE = false;
		}
	}
}

trace("launcher.js is loading...");


// launcher.js
g_ModuleID = 3031;
g_ScriptID = 103;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

var selectedTask = null;
var axLauncher = null;
var launchedElement = null;
var launchedCursor = null;

var taskActivityLog		= 1;
var taskQuarantine		= 2;
var taskUnErase			= 3;
var taskWipeInfo		= 4;
var taskVirusEncyclopedia = 5;
		
function UnspecifiedNAVError(msg, file_loc, line_no)
{
	trace("*** Error. Line: " + line_no + ", " + msg);
	
	// Hide the real reason of failure
	var msgText;
	try
	{
		msgText = resErr.oERR_UNSPECIFIED.innerText;
	}
	catch (e)
	{
		msgText = "Unspecified error.";				// Make sure we can always show error.
	}
	GlobalErrorHandler(msgText, file_loc, line_no);	// Defined in DisplayScriptError.js
	return true;
}

function NAVError(errObj)
{
	trace("*** NAV error");
	
	var e = errObj.innerText.split("|", 2);
	var errNumber = e[0];
	var errText = e[1];
	if (errText.length == 0)
		errText = resErr.oERR_DEFAULT.innerText;

	trace("*** Error: " + errNumber + " : " + errText);
		
	g_ErrorHandler.DisplayNAVError(errText, errNumber);		// g_ErrorHandler defined in DisplayScriptError.js
}

function OnLoad ()
{
	window.onerror = UnspecifiedNAVError;
}	

function OnItemFocus(element)
{
	if (element.tagName == "BODY")
		return null;

	if (element.tagName != "TR")
	    return OnItemFocus(element.parentElement);
		
	if (selectedTask != null && selectedTask != element)
	{
		selectedTask.className = null;
		selectedTask = null;
	}
	
	if (selectedTask == null)
	{
		selectedTask = element;
		selectedTask.className = "navItemSelected";
	}
}

function LaunchTask(element, task)
{
	trace("LaunchTask: " + task);
	
	if (launchedElement != null)	
		return;				// Wait for a previous task to launch (disable double-clicks)
		
	if (axLauncher == null)
	{
		try
		{
			trace("Create AppLauncher");
			axLauncher = new ActiveXObject("Symantec.Norton.AntiVirus.AppLauncher");
		}
		catch (e)
		{
			NAVError(resErr.oERR_NO_LAUNCHER);
			axLauncher = null;
		}
	}
	
	if (axLauncher != null)
	{
		if (typeof(task) == "string")
		{
			axLauncher.LaunchURL(task);
		}
		else
		{
		    // Exceptions thrown below will come from the NAVLaunch object which
		    // will fill in it's own NAVError information so catch it and display
		    // the proper error.
		    try
		    {
			    switch (task)
			    {
			    case taskActivityLog:
				    axLauncher.LaunchActivityLog();
				    break;
			    case taskQuarantine:
				    axLauncher.LaunchQuarantine();
				    break;
			    case taskUnErase:
				    axLauncher.LaunchUnEraseWizard();
				    break;
			    case taskWipeInfo:
				    axLauncher.LaunchWipeInfo();
				    break;
    				
			    case taskVirusEncyclopedia:
				    axLauncher.LaunchVirusEncyclopedia();
				    break;
    				
			    }
			}
			catch(NAVErr)
			{
			    axLauncher.NAVError.LogAndDisplay(0);
			    return;
			}
		}
		
		launchedElement = element;
		launchedCursor = launchedElement.style.cursor;
		launchedElement.style.cursor = "wait";
		window.setTimeout("EnableLauncher()",1000);
	}
}

function EnableLauncher()
{
	if (launchedElement != null)
		launchedElement.style.cursor = launchedCursor;
	launchedElement = null;
}
