// TRACE disabled
var TRACE = false;

if (TRACE)
{
	try
	{
		top.axVkTrace = new ActiveXObject("VkTools.VkTraceCC");
		top.axVkTrace.SetModuleName("NavScanJs");
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

trace("scan.js is loading...");

//////////////////////////////////////////////////////////////////////////

g_ModuleID = 3031;	// Used by DisplayScriptError.js
g_ScriptID = 102;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// The g_fStartingScan variable is used to disable the launching of new scans
// for a bit after it has been pressed.  It's a hack to get around not knowing
// when we are currently executing a scan.



// ActiveX objects
var axNAVScanTasks = null;
var axScheduler = null;
var axWebWnd = null;
var axNAVLicense = null;

// Flags
var panicArea = false;			// Disable UI in case of error
var schedulerInstalled = false;
var scanDisabled = false;
var taskListLoaded = false;		// Set by the tasks frame

// Variables
var taskList = null;			// Task list tbody
var taskTemplate = null;		// Task template row
var taskSeparator = null;		// Task separator row
var taskIndex = 0;
var selectedTask = null;

// enum SCANTASKTYPE in ScanTask.h
var	scanUnset = 0;
var	scanComputer = 1;
var scanAllHardDrives =2;
var scanRemovable = 3;
var scanAFloppy = 4;
var scanSelectedDrives = 5;
var scanFolder = 6;
var scanFiles = 7;
var scanCustom = 8;
var scanQuick = 9;
var scanLast = scanQuick;

// MessageBox() Flags
var MB_OK =					0x00000000;
var MB_OKCANCEL =			0x00000001;
var MB_ABORTRETRYIGNORE =	0x00000002;
var MB_YESNOCANCEL =		0x00000003;
var MB_YESNO =				0x00000004;
var MB_RETRYCANCEL =		0x00000005;

var MB_ICONHAND =			0x00000010;
var MB_ICONQUESTION =		0x00000020;
var MB_ICONEXCLAMATION =	0x00000030;
var MB_ICONASTERISK  =		0x00000040;

var MB_USERICON =			0x00000080;
var MB_ICONWARNING =		MB_ICONEXCLAMATION;
var MB_ICONERROR =			MB_ICONHAND;

var MB_ICONINFORMATION =	MB_ICONASTERISK;
var MB_ICONSTOP =			MB_ICONHAND;

var IDOK =		1;
var IDCANCEL =	2;
var IDABORT =	3;
var IDRETRY =	4;
var IDIGNORE =	5;
var IDYES =		6;
var IDNO =		7;


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
	if (panicArea)
		Panic();
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

	if (panicArea)
		Panic();
}

function Panic()
{
	scanDisabled = true;
	document.body.disabled = true;
	oTaskList.document.body.style.display = "none";
}

function MessageBox(text, caption, type)
{
	try
	{
		if (axWebWnd == null)
			axWebWnd = new ActiveXObject("CcWebWnd.ccWebWindow");
			
		return axWebWnd.MsgBox(text, caption, type);
	}
	catch (e)
	{
		NAVError(resErr.oERR_NO_WEBWND);
		return 0;
	}
}

function OnLoad ()
{
	trace("-> OnLoad()");

	window.onerror = UnspecifiedNAVError;	// Global error handling. This helps to eliminate the most of try-blocks.
	setTimeout("Initialize()", 10);

	trace("<- OnLoad()");
}	

function Initialize()
{
	trace("-> Initialize()");
	trace("taskListLoaded: " + taskListLoaded);
	
	if (!taskListLoaded)
	{
		setTimeout(Initialize, 100);
		return;		// Wait for the tasks frame to load
	}

	panicArea = true;
	
	if (CreateMainInterfaces())
	{
		schedulerInstalled = axScheduler.Installed;
		BuildTaskList();
	}

	panicArea = false;
	
	trace("<- Initialize()");
}


function CreateMainInterfaces()
{
	trace("-> Create MainInterfaces");
	
	try
	{
		axNAVScanTasks = new ActiveXObject("Symantec.Norton.AntiVirus.ScanTasks");
	}
	catch (e)
	{
		NAVError(resErr.oERR_NO_NAVTASKS);
		return false;
	}

	trace("axNAVScanTasks loaded");
		
	try
	{
		axScheduler = new ActiveXObject("NAVTasks.Scheduler");
	}
	catch (e)
	{
		NAVError(resErr.oERR_NO_SCHEDULER);
		return false;
	}

	trace("axScheduler loaded");

	trace("<- Create MainInterfaces");
	
	return true;
}

function BuildTaskList()
{
	taskList = oTaskList.document.all("oTasks");
	taskTemplate = taskList.all("oTemplate");
	taskSeparator = taskList.all("oSeparator");
	
	var count = axNAVScanTasks.Count;

	trace("BuildTaskList of : " + count);
	
	// Use an array to arrange tasks by type
	var tasks = new Array();
	var customTasks = 0;
	
	for (var i = 0; i < count; i++)
	{
		var type = axNAVScanTasks(i).TaskType;
		var name = axNAVScanTasks(i).TaskName;
		
		if (type < 0 || type > scanLast)
			continue;

		if (type == scanCustom)
		{
		    customTasks++;
			tasks[scanLast + customTasks] = name;
		}
		else
		{
		    var index = type;
		 
		    // Force the my computer scan to index 1 and the quick scan to index 2   
		    if( type == scanAllHardDrives )
		        index = 1;
		    else if( type == scanQuick )
		        index = 2;
		        
			tasks[index] = name;
	    }
	}
	
	var firstTask = null;
	
	for (var i = 0; i < tasks.length; i++)
	{
		if (tasks[i] != null)
		{
			var task = InsertTask(i <= scanLast ? i : scanCustom, tasks[i]);
			
			if (firstTask == null)
				firstTask = task;
		}
	}
	
	if (firstTask != null)
	{
		trace("firstTask: " + firstTask);
		firstTask.scrollIntoView(true);
		SelectTask(firstTask);
	}
	
	oTaskList.document.body.style.visibility = "visible";
}

function GetTaskIcon(type)
{
    var icon;
    
    switch(type )
    {
	    case scanRemovable:
	        icon = "removable.gif";
	        break;
	    case scanAFloppy:
	    	icon = "floppy.gif";
	    	break;
	    case scanSelectedDrives:
	    	icon = "drives.gif";
	    	break;
	    case scanFolder:
	    	icon = "folder.gif";
	    	break;
	    case scanFiles:
	    	icon = "files.gif";
	    	break;
	    case scanCustom:
	    	icon = "virus_loupe.gif";
	    	break;
	    default:
	        icon = "comp.gif";
	        break;
	}

    return icon;
}

function InsertTask(type, name)
{
	if (taskIndex > 0)
	{
		var separator = taskSeparator.cloneNode(true);
		separator.id += taskIndex
		separator.style.display = "inline";
		taskList.appendChild(separator);
	}
	
	var task = taskTemplate.cloneNode(true);
	task.id = "oTask" + taskIndex;
	task.taskName = name;
	task.all("oTaskIcon").src = GetTaskIcon(type);
	task.all("oTaskName").innerText = name;
	
	if (type == scanCustom)
	{
		task.all("oTaskEdit").style.visibility = "inherit";
		task.all("oTaskDelete").style.visibility = "inherit";
	}

    // Allow scheduling for custom, my computer, and quick scans
	if (schedulerInstalled && (type == scanCustom || type == scanAllHardDrives || type == scanComputer || type == scanQuick))
		UpdateScheduleStatus(task, name);

	task.style.display = "inline";
	
	taskList.appendChild(task);
	taskIndex++;
	
	return task;
}

function UpdateScheduleStatus(task, name)
{
	try
	{
  		var scheduled = axNAVScanTasks(name).Scheduled;
  		
  		trace(name + " scheduled " + scheduled);

		if (axNAVScanTasks(name).CanSchedule && scheduled != -1)
		{
			// task.all("oTaskScheduleStatus").innerText = scheduled == 0? oNotScheduled.innerText : oScheduled.innerText;
			task.all("oTaskScheduleIcon").src = scheduled == 0? "scheduleOff.gif" : "scheduleOn.gif";
			task.all("oTaskSchedule").style.visibility = "inherit";
			return;
		}
	}
	catch (e)
	{
	}
	task.all("oTaskScheduleStatus").innerText = "";
	task.all("oTaskSchedule").style.visibility = "hidden";
}

function GetTask(element)
{
	if (element.tagName == "TR")
	{
		SelectTask(element);
		return element;
	}
	if (element.tagName == "BODY")
		return null;
    return GetTask(element.parentElement);
}

function SelectTask(task)
{
	trace("Select task: " + task.taskName);
	
	if (selectedTask != null && selectedTask != task)
	{
		// selectedTask.className = null;
		selectedTask = null;
		trace("Selection erased");
	}
	
	if (selectedTask == null)
	{
		selectedTask = task;
		// selectedTask.className = "navTaskTRSelected";
		trace("Selected: " + task.taskName);
	}
}

function OnItemFocus(element)
{
	GetTask(element);
}

var startingScan = false;		// Prevents launching a few scans on double-click
var startingScanElement = null;	// An element that "launched" the scan

function OnScan(element)
{
	if (startingScan)
		return;

    // We want to show the error information from the scan tasks class
    try
    {
	    axNAVScanTasks(GetTask(element).taskName).Scan();
	}
	catch(NAVErr)
	{
	    axNAVScanTasks.NAVError.LogAndDisplay(0);
	    return;
	}
	
	startingScanElement = element;
	startingScanElement.style.cursor = "wait";
	window.setTimeout("ScanStarted()",1000)
}

function ScanStarted()
{
	if (startingScanElement != null)
		startingScanElement.style.cursor = "hand";
	startingScanElement = null;
	startingScan = false;
}

function OnDelete(element)
{
	var Caption = oNAV.innerText;
	
	try
	{
	
		if (axNAVLicense == null)
		{
			axNAVLicense = new ActiveXObject("NAVLicense.NAVLicenseInfo");
			trace("axNAVLicense loaded");
			Caption = axNAVLicense.ProductName;
		}
	}
	catch (e)
	{
		// Ignore error
	}
	
	if (MessageBox(oAreYouSure.innerText, Caption, MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		var task = GetTask(element);
		axNAVScanTasks.Delete(task.taskName);
		taskList.deleteRow(task.sectionRowIndex - 1);	// separator
		taskList.deleteRow(task.sectionRowIndex);		// task
		selectedTask = null;
	}
}

function OnEdit(element)
{
	var task = GetTask(element);
	var name = task.taskName;
	var index = axNAVScanTasks.TaskIndex(name);
	axNAVScanTasks(name).Edit();
	name = axNAVScanTasks(index).TaskName;
	task.taskName = name;
	task.all("oTaskName").innerText = name;
}

function OnSchedule(element)
{
	var task = GetTask(element);
	var name = task.taskName;
	axNAVScanTasks(name).Schedule(false);
	UpdateScheduleStatus(task, name);
}

function OnNewTask()
{
	if (scanDisabled)
		return;
		
	var newTask = axNAVScanTasks.Create();
	if (newTask)
	{
		var task = InsertTask(scanCustom, newTask.TaskName);
		task.scrollIntoView(false);
		SelectTask(task);
	}
}
