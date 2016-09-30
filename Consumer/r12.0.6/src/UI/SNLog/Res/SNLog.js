// JScript source code
var bEnableBT = true;
var ProdType;

function LaunchBackTrace(address)
{
	// NAV will not have the ability to back track.
	// So just return.
	return;
}

function NisEvtEventOnLoad()
{
	UserRow.style.display = "none";
}

function IDSEventOnLoad()
{
	// Hide the BackTrace description row if it exists.

	if (BTDescriptionRow != null)
		BTDescriptionRow.style.display = "none";

	// Change all BackTrace links to normal text.

	if (BTLink.length)
	{
		for (i = 0; i < BTLink.length; i++)
		{
			BTLink[i].className = "";
		}
	}
	else
		BTLink.className = "";

	// Turn off all BackTrace link onclick handler.

	bEnableBT = false;
}
