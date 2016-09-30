////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Button constants
var BUTTON_LOC = "_loc";
var BUTTON_HOVER = "_hover";
var BUTTON_FOCUSED = "_focused";
var BUTTON_PRESSED = "_pressed";
var BUTTON_DISABLED = "_dis";

function SetButtonHighContrast(button)
{
	if ((typeof(NAVOptionObj) != "undefined") && (NAVOptionObj.IsHighContrastMode))
	{
		button.style.border = '1px solid white';
	}
	else
	{
		button.style.border = '0px solid white';
	}
}

function GetButtonType(button)
{
	var className = button.className;
	return (className.substring(className.lastIndexOf("_")));
}

function SetButtonType(button, buttonType)
{
	ButtonNormalize(button);
	button.className = button.className + buttonType; 
	SetButtonHighContrast(button);
}

// This function returns a button to its "normal" style
function ButtonNormalize(button)
{
	var buttonType = GetButtonType(button);
	
	switch (buttonType)
	{
		// Fall-through for non-normal button types
		case BUTTON_HOVER:
		case BUTTON_FOCUSED:
		case BUTTON_PRESSED:
		case BUTTON_DISABLED:
			button.className = button.className.substring(0, button.className.lastIndexOf("_"));
			break;
		default:
			// Already normalized
			break;
	};
	
	return;
}

function ButtonDisable(button)
{
	SetButtonType(button, BUTTON_DISABLED);
	button.disabled = true;
}

function ButtonEnable(button)
{
	ButtonNormalize(button);
	button.disabled = false;
}

// Function called when hovering mouse over a button
function ButtonMouseOver(button)
{
	var buttonType = GetButtonType(button);
	// Only change non-focus and non-disabled buttons to hover
	if (buttonType != BUTTON_FOCUSED && buttonType != BUTTON_DISABLED)
	{
		SetButtonType(button, BUTTON_HOVER);
	}
}

function ButtonMouseDown(button)
{
	SetButtonType(button, BUTTON_PRESSED);
}

// Function called when mouse leaves a button
function ButtonMouseOut(button)
{
	// Reset the button only if coming out of a hover
	if (GetButtonType(button) == BUTTON_HOVER)
	{
		ButtonNormalize(button);
	}
	// This other part is for situations where you
	// "click and drag" a button
	else if (GetButtonType(button) == BUTTON_PRESSED)
	{
		SetButtonType(button, BUTTON_FOCUSED);
	}
}

function ButtonFocus(button)
{
	// Give ButtonMouseDown priority
	if (GetButtonType(button) != BUTTON_PRESSED)
	{
		SetButtonType(button, BUTTON_FOCUSED);
	}
}

function ButtonBlur(button)
{
	// Reset the button only if coming out of a focus
	if (GetButtonType(button) == BUTTON_FOCUSED)
	{
		ButtonNormalize(button);
	}
	// This other part is for buttons like "Page Default" where the
	// button loses focus when clicked, but we don't really want it to
	else if (GetButtonType(button) == BUTTON_PRESSED)
	{
		SetButtonType(button, BUTTON_FOCUSED);
	}
}

function ButtonKeyDown(button)
{
	try
	{
		// The "SPACE" and "ENTER" keys are used.
	   if ( (event.keyCode == 13) || (event.keyCode == 32) )
	   {		 
		 SetButtonType(button, BUTTON_PRESSED);
	   }
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}

function ButtonKeyUp(button)
{	
	try
	{
		// The "SPACE" and "ENTER" keys are used.
	   if ( (event.keyCode == 13) || (event.keyCode == 32) )
	   {	
			SetButtonType(button, BUTTON_FOCUSED);
			button.onclick();
	   }
	}
	catch (err)
	{
		g_ErrorHandler.DisplayException (err);
		return;
	}
}