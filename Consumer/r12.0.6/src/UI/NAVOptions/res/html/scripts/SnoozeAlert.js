// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// enum SnoozeFeature
var SnoozeAP		    = 0x01;
var SnoozeIWP			= 0x02;
var SnoozeALU			= 0x04;
var SnoozeEmailIn		= 0x08;
var SnoozeEmailOut		= 0x10;

var REMOVE_TURN_OFF_AP = 0x80000000;

var returnValue = 0;	// Return value
var features = dialogArguments;

var axLauncher = null;

function OnLoad()
{
	if ((features & SnoozeAP) != 0)
	{
		if ((features & REMOVE_TURN_OFF_AP) != 0)
			oSnoozeAP.options.remove(oSnoozeAP.options.length - 1);	// Remove "Permanently" option

		oItemAP.style.display = "inline";
	}

	if ((features & SnoozeIWP) != 0)
	{
		oItemWP.style.display = "inline";
	}

	if ((features & SnoozeALU) != 0)
	{
		oItemLU.style.display = "inline";
	}

	if ((features & (SnoozeEmailIn | SnoozeEmailOut)) != 0)
	{
		oItemEm.style.display = "inline";
	}
}

function OnOK()
{
	try
	{
		returnValue = 1;
		
		var axSnoozeAlert = new ActiveXObject("Symantec.Norton.AntiVirus.SnoozeAlert");

		if ((features & SnoozeAP) != 0)
		{
			axSnoozeAlert.SetSnoozePeriod(SnoozeAP, parseInt(oSnoozeAP.value));
		}

		if ((features & SnoozeIWP) != 0)
		{
			axSnoozeAlert.SetSnoozePeriod(SnoozeIWP, parseInt(oSnoozeWP.value));
		}

		if ((features & SnoozeALU) != 0)
		{
			axSnoozeAlert.SetSnoozePeriod(SnoozeALU, parseInt(oSnoozeLU.value));
		}

		if ((features & SnoozeEmailIn) != 0)
		{
			axSnoozeAlert.SetSnoozePeriod(SnoozeEmailIn, parseInt(oSnoozeEm.value));
		}

		if ((features & SnoozeEmailOut) != 0)
		{
			axSnoozeAlert.SetSnoozePeriod(SnoozeEmailOut, parseInt(oSnoozeEm.value));
		}
	}
	catch (err)
	{
	}
	window.navigate("res://closeme.xyz");
}

function OnCancel()
{
	window.navigate("res://closeme.xyz");
}

function OnMoreInfo()
{
	try
	{
		if (axLauncher == null)
			axLauncher = new ActiveXObject("Symantec.Norton.AntiVirus.AppLauncher");
		axLauncher.LaunchHelp(0);		// Oleg: Need Snooze Alert Help ID here
	}
	catch (e)
	{
	}
}
