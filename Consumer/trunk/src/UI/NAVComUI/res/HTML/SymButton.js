////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//
// Initiation of the Symantec-2005 style button (usually called from onload)
//
// The button must be wrapped by a span (or td) object that holds the border.
// This is required to display a button border in the high contrast mode.
//
// For example:
// <span style="border:1px solid #000000; width: 92; height: 25">
//   <button id="oBtn" class="symButton" style="background-image:url(btn90x22set.gif)">Button</button></span>
//
// The button background image must contain all button faces in the following order:

var SYMBTN_NORMAL	= 0;
var SYMBTN_OVER		= 1;
var SYMBTN_PRESSED	= 2;
var SYMBTN_FOCUSED	= 3;
var SYMBTN_DISABLED	= 4;

// Use:
//	Disabled(value) instead of the disabled property, and
//	Visibility(value) instead of the style.visibility property

function InitSymButton(btn)
{
	btn.focused = false;
	btn.mouseIn = false;
	btn.hasCapture = false;
	btn.Visibility = Visibility;
	btn.Disabled = Disabled;
	btn.attachEvent("onfocusin", OnFocusIn);
	btn.attachEvent("onfocusout", OnFocusOut);
	btn.attachEvent("onmouseenter", OnMouseEnter);	
	btn.attachEvent("onmouseleave", OnMouseLeave);
	btn.attachEvent("onmousedown", OnMouseDown);
	btn.attachEvent("onlosecapture", OnLoseCapture);
	btn.attachEvent("onkeypress", OnKeyPress);
	
	btn.parentElement.style.visibility = btn.style.visibility;
	
	if (document.activeElement == btn)
	{
		btn.focused = true;
		btn.style.backgroundPositionY = -(SYMBTN_FOCUSED * btn.clientHeight);
	}
	else
	{
		if (btn.disabled)
			btn.style.backgroundPositionY = -(SYMBTN_DISABLED * btn.clientHeight);
	}
	
	// XP button methods
	
	function Visibility(value)
	{
		this.style.visibility = this.parentElement.style.visibility = value;
	}

	function Disabled(value)
	{
		this.disabled = value;
		if (value)
		{
			this.focused = false;
			this.mouseIn = false;
			this.hasCapture = false;
		}
		this.parentElement.style.borderColor = value? "#C0C0C0" : "#000000";
		this.style.backgroundPositionY = -((value? SYMBTN_DISABLED : SYMBTN_NORMAL) * btn.clientHeight);
	}

	function OnFocusIn()
	{
		var btn = event.srcElement;
		btn.focused = true;
		btn.style.backgroundPositionY = -((btn.hasCapture? SYMBTN_PRESSED : SYMBTN_FOCUSED) * btn.clientHeight);
	}

	function OnFocusOut()
	{
		var btn = event.srcElement;
		btn.focused = false;
		btn.hasCapture = false;
		btn.style.backgroundPositionY = SYMBTN_NORMAL;
	}

	function OnMouseEnter()
	{
		var btn = event.srcElement;
		btn.mouseIn = true;
		btn.style.backgroundPositionY = -((btn.hasCapture? SYMBTN_PRESSED : SYMBTN_OVER) * btn.clientHeight);
	}

	function OnMouseLeave()
	{
		var btn = event.srcElement;
		btn.mouseIn = false;
		btn.style.backgroundPositionY = -((btn.hasCapture? SYMBTN_OVER : (btn.focused? SYMBTN_FOCUSED : SYMBTN_NORMAL)) * btn.clientHeight);
	}

	function OnMouseDown()
	{
		var btn = event.srcElement;
		btn.hasCapture = true;
		btn.style.backgroundPositionY = -(SYMBTN_PRESSED * btn.clientHeight);
	}

	function OnKeyPress()
	{
		if (event.keyCode == 32) // SPACEBAR
		{
			var btn = event.srcElement;
			btn.hasCapture = true;
		}
	}

	function OnLoseCapture()
	{
		var btn = event.srcElement;
		btn.hasCapture = false;
		btn.style.backgroundPositionY = -((btn.focused? (btn.mouseIn? SYMBTN_OVER : SYMBTN_FOCUSED) : SYMBTN_NORMAL) * btn.clientHeight);
	}
}
