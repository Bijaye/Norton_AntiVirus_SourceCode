// TRACE disabled
var TRACE = false;

if (TRACE)
{
	try
	{
		top.axVkTrace = new ActiveXObject("VkTools.VkTraceCC");
		top.axVkTrace.SetModuleName("NavStatsJs");
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

trace("navstats.js is loading...");

// --

g_ModuleID = 3031;	// Used by DisplayScriptError.js
g_ScriptID = 101;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Resource object: this will help to move resources in an external file
var res = this;

// Often used text
var textOn			= null;
var textOff			= null;
var textError		= null;

// Set all items to ths Urgent state in case of error
var panicArea		= false;

// ActiveX objects. Do not set them to null because they can be preset by simulator.
var axLauncher;
var axOptions;
var axAP;
var axNAVLicense;
var axNAVScanTasks;

// Licensing

var licenseType = 0;

// License zone
var DJSMAR_LicenseZone_Trial		    =   (0x00000001); // in a trial period
var DJSMAR_LicenseZone_Expired		    =   (0x00000002); // the license has expired
var DJSMAR_LicenseZone_Warning		    =   (0x00000004); // the license is about to expire
var DJSMAR_LicenseZone_PreActivation	=   (0x00000008); // the license has never been activated
var DJSMAR_LicenseZone_Purchased		=   (0x00000020); // the license is in a purchased(AKA Activated) state
var DJSMAR_LicenseZone_Killed			=   (0x00000080); // the license has been purposely killed (By the server)
var DJSMAR_LicenseZone_Violated			=   (0x00000040); // the license has been violated (i.e. failed security checks)
var DJSMAR_LicenseZone_Grace			=   (0x00000400); // This distinguishes grace period from regular trial period

// License state
var DJSMAR00_LicenseState_Violated   = -2;
var DJSMAR00_LicenseState_EXPIRED    = -1;

// License types
var DJSMAR_LicenseType_Retail	= 0;
var DJSMAR_LicenseType_Trial	= 1;
var DJSMAR_LicenseType_Rental	= 2;
var DJSMAR_LicenseType_TryDie	= 3;
var DJSMAR_LicenseType_Beta		= 4;
var DJSMAR_LicenseType_Unlicensed = 5;
var DJSMAR_LicenseType_ESD		= 6;
var DJSMAR_LicenseType_OEM		= 7;

// Help Id
var IDH_AP = 11512;
var IDH_SW = 352045;
var IDH_Em = 11513;
var IDH_FS = 11515;
var IDH_VD = 11516;
var IDH_LU = 11517;
var IDH_LS = 11518;
var IDH_WP = 250006;
var IDH_WPUpgraded = 250007;
var IDH_SUB_AUTORENEW = 220546;

var IDH_RETAIL_PREACTIVATION_TRIAL = 11912;
var IDH_RETAIL_PREACTIVATION_EXPIRED = 11900;
var IDH_TRYBUY_PREACTIVATION_TRIAL = 11520;
var IDH_TRYBUY_PREACTIVATION_EXPIRED = 11521;
var IDH_SCSS_PREACTIVATION_TRIAL = 11902;
var IDH_SCSS_PREACTIVATION_TRIAL_EXPIRED = 11903;
var IDH_SCSS_ACTIVE = 11525;
var IDH_SCSS_WARNING = 11526;
var IDH_SCSS_EXPIRED = 11527;
var IDH_SCSS_KILLED = 11528;

// From IWPSettingsInterface.h
IWPStateError =	0;              // Couldn't get state - ERROR 
IWPStateNotRunning = 1;         // Agent isn't running (ccApp) but should be - ERROR
IWPStateNotAvailable = 100;     // Not supposed to exist - Not an error!
IWPStateYielding = 101;         // IWP is off because another product is taking precedence
IWPStateEnabled = 102;          // Default OK setting
IWPStateDisabled = 103;         // Loaded but turned off
IWPStateLoading = 104;          // Agent is loading
IWPStateUnloading = 105;        // Agent is unloading

// enum SnoozeFeature
var SnoozeAP		    = 0x01;
var SnoozeIWP			= 0x02;
var SnoozeALU			= 0x04;
var SnoozeEmailIn		= 0x08;
var SnoozeEmailOut		= 0x10;

// --

// NAV Service status returned by ActiveX
var statusNone			= 0;    // Couldn't get state or uninitialized - ERROR
var statusNotInstalled	= 1;	// Should be installed but isn't, tried to init but failed - ERROR
var statusNotRunning	= 2;	// Not loaded
var statusEnabled		= 3;   	// Default OK setting
var statusDisabled		= 4;    // Loaded but turned off
var statusNotAvailable	= 5;	// Not supposed to exist ( e.g. Rescue Disk on NT ) Not an error!
// Additional status values used by the script
var statusUpgraded		= -1;

// NAV Service state
var stateNone			= -1;
var stateOK				= 0;
var stateWarn			= 1;
var stateUrgent			= 2;
var stateUpgraded		= 3;

// NAV Protection state
var stateExpiredSub		= 10;
var stateOldUpdates		= 11;

// NAV Product Status
var iwpInstalled = true;		// Internet Worm Protection installed
var swInstalled = true;         // Spyware installed
var layoutInitialized = false;	// Page layout is initialized
var productEnabled = true;		// License is OK
var navProtection = stateOK;	// Subs and defs are OK
var setProtectionLevel = true;	// Update navProtection values

// Status tab indexes
var indexAP			= 0;	// Auto-Protect
var indexSW         = 1;    // Spyware
var indexEm			= 2;	// Email
var indexWP			= 3;	// Internet worm protection
var indexFS			= 4;	// Full Scan
var indexVD			= 5;	// Virus Defs
var indexLS			= 6;	// License/Subscriptions
var indexLU			= 7;	// Auto-LiveUpdate
var itemCount		= 8;

var itemArray = null;		// Array of items
var curItem = null;			// Current item

var g_ProductName = null;

// Status item constaructor
function StatusItem(index, tr, bgImage, checkProtection, stateIcon, stateName, stateValue, hintTitle, helpId, hintBtnFunc, hintBtn2Func)
{
	this.index			= index;
	this.tr				= tr;
	this.bgImage		= bgImage;
	this.checkProtection= checkProtection;
	this.stateIcon		= stateIcon;
  	this.stateName		= stateName;	
	this.stateValue		= stateValue;
	this.hintTitle		= hintTitle.innerText;
	this.helpId			= helpId;
	this.hintBtnFunc	= hintBtnFunc;
	this.hintBtn2Func   = hintBtn2Func;
	
	// Defaults
	this.offsetTop		= -1;
	this.state			= stateNone;
	this.value			= "";
	this.hintHTML		= "";
	this.hintBtnText	= null;
	this.hintBtnHidden	= false;
	this.hintBtnEnabled	= true;
	this.hintBtnArg		= null;
	this.hintBtn2Text	= null;
	this.hintBtn2Hidden	= true;
	this.hintBtn2Enabled= false;
	this.hintBtn2Arg	= null;
	this.itemHidden		= false;
	this.hintOverridden	= false;
	this.hintBtnEnabledAlways = false;
}

function Initialize()
{
	// Create tabs definition
	
	itemArray = new Array(itemCount);

  	itemArray[indexAP] = new StatusItem( indexAP,	oItemTrAP,	oTabBgTop,		true,	oItemIconAP,	oItemNameAP,	oItemValueAP,	res.oHintTitleAP,	IDH_AP,	OnClickEnableAP, null );
  	itemArray[indexSW] = new StatusItem( indexSW, 	oItemTrSW,	oTabBgImage,	true,	oItemIconSW,	oItemNameSW,	oItemValueSW,	res.oHintTitleSW,	IDH_SW,	OnClickQuarantine, OnClickEnableSW );
  	itemArray[indexWP] = new StatusItem( indexWP,	oItemTrWP,	oTabBgImage,	true,	oItemIconWP,	oItemNameWP,	oItemValueWP,	res.oHintTitleWP,	IDH_WP,	OnClickEnableWP, null );
  	itemArray[indexEm] = new StatusItem( indexEm,	oItemTrEm,	oTabBgImage,	true,	oItemIconEm,	oItemNameEm,	oItemValueEm,	res.oHintTitleEm,	IDH_Em,	OnClickEnableEm, null );
  	itemArray[indexFS] = new StatusItem( indexFS,	oItemTrFS,	oTabBgImage,	true,	oItemIconFS,	oItemNameFS,	oItemValueFS,	res.oHintTitleFS,	IDH_FS,	OnClickScanNow, null );
  	itemArray[indexVD] = new StatusItem( indexVD,	oItemTrVD,	oTabBgImage,	false,	oItemIconVD,	oItemNameVD,	oItemValueVD,	res.oHintTitleVD,	IDH_VD,	OnClickRunLU, null );
  	itemArray[indexLS] = new StatusItem( indexLS,	oItemTrLS,	oTabBgImage,	false,	oItemIconLS,	oItemNameLS,	oItemValueLS,	res.oHintTitleLS,	IDH_LS,	null, null );
  	itemArray[indexLU] = new StatusItem( indexLU,	oItemTrLU,	oTabBgBottom,	true,	oItemIconLU,	oItemNameLU,	oItemValueLU,	res.oHintTitleLU,	IDH_LU,	OnClickEnableALU, null );
	
	panicArea = true;
	curItem = null;
	
	if (CreateMainInterfaces())
	{
		licenseType = axNAVLicense.GetLicenseType();
		trace("License type: " + licenseType);
		
		try
		{
		    iwpInstalled = axOptions.IWPIsInstalled != 0;
		    swInstalled = (axOptions.THREAT.NoThreatCat == 0); // kinda backwards
		}
		catch(NAVErr)
		{
		    axOptions.NAVError.LogAndDisplay(0);
		}

		// Set request status flags
		axNAVStatus.UseAP = 1; // Spyware is AP
		axNAVStatus.UseVirusDef = 1;
		axNAVStatus.UseALU = 1;
		axNAVStatus.UseEmail = 1;
		axNAVStatus.UseFullSystemScan = 1;
		axNAVStatus.UseLicensing = 1;

		if (iwpInstalled)
			axNAVStatus.UseIWP = 1;
			
		if (swInstalled)
		    axNAVStatus.UseSpyware = 1;
			
		switch(licenseType)
		{
		case DJSMAR_LicenseType_Retail:
		case DJSMAR_LicenseType_Trial:
		case DJSMAR_LicenseType_Unlicensed:
		case DJSMAR_LicenseType_ESD:
		case DJSMAR_LicenseType_Beta:
			axNAVStatus.UseVirusDefSubscription = 1;
			break;
			
		case DJSMAR_LicenseType_Rental:
			oCategorySubscriptionTitle.innerHTML = res.oCategorySubscriptionTitleFmt.innerText.replace(/%s/g, g_ProductName);
			break;
		}

		InitLayout();

		try
		{
			axNAVStatus.GetStatus(1);
		}
		catch (e)
		{
			NAVError(resErr.oERR_NO_STATUS_OBJECT);
		}
	}

	panicArea = false;
	
	trace("Status initialized");
}

function InitLayout()
{
	trace("Init layout");
	
	// Hide some items
	if (!iwpInstalled)
	{
		trace("IWP is not installed");
		
		itemArray[indexWP].itemHidden = true;
		oItemTrWP.style.display = "none";
		oSeparatorWP.style.display = "none";
	}
	
	if ( !swInstalled )
	{
		trace("SW is not installed");
		
		itemArray[indexSW].itemHidden = true;
		oItemTrSW.style.display = "none";
		oSeparatorSW.style.display = "none";
	}

	// Calculate tabs position skipping the first and the last.
	for (var i = 1; i < itemCount - 1; i++)
	{
		var item = itemArray[i];
		
		var y = 4;						// Tab backgound image offset
		var o = item.tr;
		for (var j = 0; j < 10; j++)	// 10 is used to stop an infinite loop, if tableStatusCategories is changed by mistake
		{
			y += o.offsetTop;
			if (o == tableStatusCategories)
			{
				item.offsetTop = y;
				break;
			}
			o = o.offsetParent;
		}
	}

	oStatusPane.style.visibility = "visible";
	layoutInitialized = true;
	
	trace("Layout initialized");
}

function OnLoad()
{
	trace("OnLoad");

	InitSymButton(oHintButton);
	oHintButton.Visibility("hidden");
	
	InitSymButton(oHintButton2);
	oHintButton2.Visibility("hidden");
		
	// Often used text
	textOn = res.oTextOn.innerText;
	textOff = res.oTextOff.innerText;
	textError = res.oTextError.innerText;
	g_ProductName = res.oNAV.innerText;
	
	window.onerror = UnspecifiedNAVError;	// Global error handling. This helps to eliminate the most of try-blocks.

	setTimeout("Initialize()", 10);
}

function CreateMainInterfaces()
{
	trace("Create MainInterfaces");
	
	try
	{
		if (axLauncher == null)
		{
			axLauncher = new ActiveXObject("Symantec.Norton.AntiVirus.AppLauncher");
			trace("axLauncher loaded");
		}
	}
	catch (e)
	{
		NAVError(resErr.oERR_NO_LAUNCHER);
		return false;
	}

	try
	{
		if (axAP == null)
		{
			axAP = new ActiveXObject("NAVAPSCR.ScriptableAutoProtect");
			trace("axAP loaded");
		}
	}
	catch (e)
	{
		NAVError(resErr.oERR_NO_APSCR);
		return false;
	}

	try
	{
		if (axNAVLicense == null)
		{
			axNAVLicense = new ActiveXObject("NAVLicense.NAVLicenseInfo");
			trace("axNAVLicense loaded");
			var sSuiteOwner = window.external.ObjectArg("DRM::SuiteOwner");
			trace(sSuiteOwner);
			axNAVLicense.OwnerSeed = sSuiteOwner;
			trace("Succeeded adding seed");	
			
			// Get product name from branding libraries
			g_ProductName = axNAVLicense.ProductName;
		}
	}
	catch (e)
	{
		NAVError(resErr.oERR_NO_LICENSE);
		return false;
	}
	
	try
	{
		if (axOptions == null)
		{
			axOptions = new ActiveXObject("Symantec.Norton.AntiVirus.NAVOptions");
			trace("axOptions loaded");
		}
	}
	catch (e)
	{
		NAVError(resErr.oERR_NO_OPTIONS);
		return false;
	}

	return true;
}

function CreateAxScanTasks()
{
	trace("Load axNAVScanTasks");

	try
	{
		if (axNAVScanTasks == null)
		{
			axNAVScanTasks = new ActiveXObject("Symantec.Norton.AntiVirus.ScanTasks");
			trace("axNAVScanTasks loaded");
		}
	}
	catch (e)
	{
		NAVError(resErr.oERR_NO_NAVTASKS);
		return false;
	}

	return true;
}

function OnStatusChanged(item)
{
trace("-> OnStatusChanged");
	var stateOverridden = false;
	
	if (item.checkProtection)
	{
		item.hintOverridden = false;
		if (navProtection != stateOK && item.state == stateOK && item.hintBtnHidden)
		{
			switch (navProtection)
			{
			default:
			case stateExpiredSub:
				item.hintHTML = res.oHintExpiredSub.innerHTML;
				if(item.index == indexSW)
				{
					item.hintBtn2Text = res.oBtnTextRenew.innerText;
					item.hintBtnText = res.oBtnTextRestore.innerText;
				}
				else
				{
					item.hintBtnText = res.oBtnTextRenew.innerText;
				}
				break;
				
			case stateOldUpdates:
				item.hintHTML = res.oHintOldUpdates.innerHTML;
				if(item.index == indexSW)
				{
					item.hintBtn2Text = res.oBtnTextRunLU.innerText;
					item.hintBtnText = res.oBtnTextRestore.innerText;
				}
				else
				{
					item.hintBtnText = res.oBtnTextRunLU.innerText;
				}
				break;
			}
						
			item.hintBtnHidden = false;
			if(item.index == indexSW)
			{
				item.hintBtn2Hidden = false;
				item.hintBtn2Enabled = true;
			}
			item.hintOverridden = true;
			stateOverridden = true;
		}
	}

	item.stateValue.innerText = item.value;
	item.stateValue.className = (item.state == stateOK || item.state == stateUpgraded)? "navStatItemValueOK" : (item.state == stateWarn? "navStatItemValueWarn" : "navStatItemValueUrgent");
	item.stateIcon.src = (stateOverridden || item.state == stateWarn)? "warning_wht.gif" : (item.state == stateOK? "check_wht.gif" : (item.state == stateUpgraded? "upgraded.png" : "urgent_wht.gif"));
	
	if (curItem != null)
		UpdateHint();
  		
  	UpdateItemStyle(item);
    trace("<- OnStatusChanged");
}
  
function UpdateItemStyle(item)
{
  	if (item == curItem)
  	{
  		item.tr.className = "navStatItemTRSelected";
  		item.stateName.className = "navTaskSelected";
  		item.stateName.hideFocus = true;
  	}
  	else
  	{
  		item.tr.className = "navStatItemTR";
  		item.stateName.className = "navTask";
  		item.stateName.hideFocus = false;
  	}
}

function SelectItem(index)
{
	trace("Select item: " + index);
	
	var item = itemArray[index];

	if (curItem == item || item.itemHidden)
		return;				// Selected already

	var bgImage = item.bgImage;

	if (curItem != null && curItem.bgImage != bgImage)
		curItem.bgImage.style.visibility = "hidden";
			
	var offsetTop = item.offsetTop;
	if (offsetTop >= 0)
		bgImage.style.top = offsetTop;
	bgImage.style.visibility = "visible";

  	var oldItem = curItem;
   	curItem = item;
  
  	if (oldItem != null)
  		UpdateItemStyle(oldItem);

  	UpdateItemStyle(curItem);
	
	UpdateHint();
}

function UpdateHint()
{
	oHintTitle.innerText = curItem.hintTitle;
	oHintText.innerHTML = curItem.hintHTML;
	oHintMoreInfo.style.visibility = curItem.helpId > 0? "visible" : "hidden";

	var btnVisible = curItem.hintBtnText != null && curItem.hintBtnText.length > 0 && !curItem.hintBtnHidden;
	if (btnVisible)
	{
		oHintButton.innerText = curItem.hintBtnText;
		oHintButton.Disabled(!(curItem.hintBtnEnabled && (productEnabled || curItem.hintBtnEnabledAlways)));
	}
	oHintButton.Visibility(btnVisible? "visible" : "hidden");

	var btnVisible2 = curItem.hintBtn2Text != null && curItem.hintBtn2Text.length > 0 && !curItem.hintBtn2Hidden;
	if (btnVisible2)
	{
		oHintButton2.innerText = curItem.hintBtn2Text;
		oHintButton2.Disabled(!(curItem.hintBtn2Enabled && (productEnabled || curItem.hintBtn2EnabledAlways)));
	}
	oHintButton2.Visibility(btnVisible2? "visible" : "hidden");

}

function UpdateSystemStatus()
{
	var navStatus = stateNone;
	var index = 0;
		
	for (var i = 0; i < itemCount; i++)
	{
		if (itemArray[i].itemHidden)
			continue;
			
		var itemState = itemArray[i].state;
		if (itemState == stateUpgraded)
			itemState = stateOK;
			
		// First OK or last of worst
		if (itemState > navStatus || (navStatus != stateOK && itemState == navStatus))
		{
			navStatus = itemState;
			index = i;
		}
	}

	oBody.style.backgroundImage = navStatus == stateOK? "url(statusBgOK.jpg)" : (navStatus == stateWarn? "url(statusBgWarn.jpg)" : "url(statusBgUrgent.jpg)");
	//Defect 1-4DK5C7
	/*
	var textObj = navStatus == stateOK? res.oSysStatusOK : (navStatus == stateWarn? res.oSysStatusWarn : res.oSysStatusUrgent);
	oSysStatusValue.innerText = textObj.innerText;
	oSysStatusValue.className = navStatus == stateOK? "navStatSysStatusOK" : (navStatus == stateWarn? "navStatSysStatusWarn" : "navStatSysStatusUrgent");
	oSysStatusIcon.src = navStatus == stateOK? "check_wht.gif" : (navStatus == stateWarn? "warning_wht.gif" : "urgent_red.gif");
	oSysStatusIcon.style.visibility = "visible";
	*/
	
	if (curItem == null)
		SelectItem(index);
}

function OnItemClick(index)
{
	SelectItem(index);
}

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
	GlobalErrorHandler(msgText, file_loc, line_no);		// defined in DisplayScriptError.js
	if (panicArea)
		Panic(textError, msgText);
	return true;
}

function NAVError(errObj)
{
	trace("*** NAV error");
	
	var e = errObj.innerText.split("|", 2);
	var errNumber = e[0];
	var errText = e[1];
	
	// Format error text with product name
	errText = errText.replace(/%s/g, g_ProductName);
	
	if (errText.length == 0)
		errText = resErr.oERR_DEFAULT.innerText;

	trace("*** Error: " + errNumber + " : " + errText);
		
	g_ErrorHandler.DisplayNAVError(errText, errNumber);		// g_ErrorHandler defined in DisplayScriptError.js
	if (panicArea)
		Panic(textError, errText);
}

function Panic(stateText, msg)
{
	trace("*** Panic: " + stateText + " -> " + msg);
	
	panicArea = false;

	// Set all items to ths Urgent state
	for (var i = 0; i < itemCount; i++)
	{
		var item = itemArray[i];

		item.state			= stateUrgent;
		item.value			= stateText;
		item.hintTitle		= stateText;
		item.helpId			= 0;
		item.hintHTML		= msg;
		item.hintBtnEnabled	= false;
		item.hintBtn2Enabled= false;
		
		OnStatusChanged(item);
	}

	if (!layoutInitialized)
		InitLayout();
	
	UpdateSystemStatus();
}

// -- Update status

// On StatusChangedEvent fired by axNAVStatus
function OnStatusChangedEvent()
{
	trace("-- Status Changed Event");
	
	if (!layoutInitialized)
		trace("*** Layout is not initialized");
	
	if (!UpdateLicenseStatus() || axNAVStatus.ProductLicenseState == DJSMAR00_LicenseState_Violated)
	{
		res.oHintLicenseViolated.innerText = res.oHintLicenseViolated.innerText.replace(/%s/g, g_ProductName);		
		Panic(res.oTextTampered.innerText, res.oHintLicenseViolated.innerText);
		return;
	}

	UpdateVirusDefStatus();	// This may effect AP, Email, SB, and Spyware status

	UpdateAPStatus();
	
	if ( swInstalled )
	    UpdateSWStatus();
	
	UpdateEmailStatus();

	if (iwpInstalled)
		UpdateWPStatus ();

	UpdateFullSystemScan ();
	
	UpdateALUStatus();

	UpdateSystemStatus();
	
	trace("-- Done with Status Changed");
}

function UpdateLicenseStatus()
{
	trace("- UpdateLicenseStatus");
	
	licenseType = axNAVStatus.ProductLicenseType;	// Update license type
	
	trace("License type: " + licenseType);
	
	navProtection = stateOK;
	
	setProtectionLevel = licenseType != DJSMAR_LicenseType_Rental;
	
	var item = itemArray[indexLS];
	item.hintBtnEnabledAlways = false;
	
	var result = true;
	var statusError = false;
	
	var nZone = axNAVStatus.ProductLicenseZone;	
	trace("License zone: " + nZone);
	
	var bViolated = ((nZone & DJSMAR_LicenseZone_Violated) == DJSMAR_LicenseZone_Violated);
	var bPreactivation = ((nZone & DJSMAR_LicenseZone_PreActivation) == DJSMAR_LicenseZone_PreActivation);
	var bKilled	= ((nZone & DJSMAR_LicenseZone_Killed) == DJSMAR_LicenseZone_Killed);
	var bExpired = ((nZone & DJSMAR_LicenseZone_Expired) == DJSMAR_LicenseZone_Expired);     		
	var bWarning = ((nZone & DJSMAR_LicenseZone_Warning) == DJSMAR_LicenseZone_Warning) || ((nZone & DJSMAR_LicenseZone_Trial) == DJSMAR_LicenseZone_Trial);
	var bGrace = ((nZone & DJSMAR_LicenseZone_Grace) == DJSMAR_LicenseZone_Grace);     		
	
	
	productEnabled = !(bExpired || bViolated || bKilled);
	
	trace("Product enabled: " + productEnabled);
	
	var itemName = null;
	
	if (bViolated)
	{
		// STATUS_CORRUPTED
		trace("STATUS_CORRUPTED");
		
		res.oNameNAV.innerText = g_ProductName;
		itemName = res.oNameNAV;

		item.state = stateUrgent;
		item.value = res.oTextTampered.innerText;
		item.hintTitle = g_ProductName;
		item.hintHTML = res.oHintLicenseViolated.innerHTML;
		item.helpId = 0;
		item.hintBtnHidden = true;
		
		result = false;
	}
    else if (bPreactivation)
    {
   		switch(licenseType)
   		{		
		case DJSMAR_LicenseType_OEM:
			{
				if (!bExpired)
				{		
					// This is for limited OEM flow (Preactivation purchased)
					// STATUS_OEM_PREACTIVATION
					trace("STATUS_OEM_PREACTIVATION");
					
					itemName = null;
					
					UpdateSubscriptionStatus(item);
				}
			}
			break;
		case DJSMAR_LicenseType_Retail:
		case DJSMAR_LicenseType_Unlicensed:
		case DJSMAR_LicenseType_ESD:
		case DJSMAR_LicenseType_Beta:
			{
				if (bExpired)
				{		
					// STATUS_RETAIL_EXPIRED
					trace("STATUS_RETAIL_EXPIRED");
					
					itemName = res.oNameRetailExpired;
					
					item.state = stateUrgent;
					item.value = res.oTextExpired.innerText;
					item.hintTitle = res.oTitleProductActivation.innerText;
					item.hintHTML = res.oHintRetailExpired.innerHTML.replace(/%s/g, g_ProductName);
					item.helpId = IDH_RETAIL_PREACTIVATION_EXPIRED;
					item.hintBtnHidden = false;
					item.hintBtnEnabled = true;
					item.hintBtnEnabledAlways = true;
					item.hintBtnText = res.oBtnTextActivate.innerText;
					item.hintBtnFunc = OnClickActivate;
					item.hintBtnArg = SYMALERT_RESULT_RESOLVELICENSE;	// see SymAlertStatic.js
				}
				else if (bWarning)
				{
					// STATUS_RETAIL_PREACTIVE
					trace("STATUS_RETAIL_PREACTIVE");

					var days = "???";
					try
					{
						days = axNAVLicense.LicenseDaysRemaining;
						trace("LicenseDaysRemaining = " + days);
					}
					catch (err)
					{
						NAVError(resErr.oERR_NO_LICENSE);
					}

					itemName = res.oNameRetail;
					
					item.state = stateWarn;
					
					if(days > 1)
					{
    					item.value = res.oTextDays.innerHTML.replace(/#/, days);
						item.hintHTML = res.oHintRetailDays.innerHTML.replace(/#/, days);
    				}
    			    else
    			    {
    			        item.value = res.oTextDay.innerHTML.replace(/#/, days);
						item.hintHTML = res.oHintRetailDay.innerHTML.replace(/#/, days);
    			    }
    			        
					item.hintTitle = res.oTitleRetail.innerText;
					item.hintHTML = item.hintHTML.replace(/%s/, g_ProductName);
					item.helpId = IDH_RETAIL_PREACTIVATION_TRIAL;
					item.hintBtnHidden = false;
					item.hintBtnEnabled = true;
					item.hintBtnEnabledAlways = true;
					item.hintBtnText = res.oBtnTextActivate.innerText;
					item.hintBtnFunc = OnClickActivate;
					item.hintBtnArg = SYMALERT_RESULT_RESOLVELICENSE;
				}
				else
				{
					statusError = true;
				}
			}
			break;
		
		case DJSMAR_LicenseType_Trial:
			{
				if (bExpired)
				{
					// STATUS_TRIALWARE_EXPIRED
					trace("STATUS_TRIALWARE_EXPIRED");

					itemName = res.oNameTrialExpired;
					
					item.state = stateUrgent;
					item.value = res.oTextExpired.innerText;
					item.hintTitle = res.oTitleTrialExpired.innerText;
					item.hintHTML = res.oHintTrialExpired.innerHTML;
					item.helpId = IDH_TRYBUY_PREACTIVATION_EXPIRED;
					item.hintBtnHidden = false;
					item.hintBtnEnabled = true;
					item.hintBtnEnabledAlways = true;
					item.hintBtnText = res.oBtnTextViewOptions.innerText;
					item.hintBtnFunc = OnClickActivate;
					item.hintBtnArg = SYMALERT_RESULT_RESOLVELICENSE;
				}
				else if (bWarning)
				{
					// STATUS_TRIALWARE_ACTIVE
					trace("STATUS_TRIALWARE_ACTIVE");
					
					var date = "???";
					var daysLeft = "???";
					try
					{
						date = axNAVLicense.LicenseExpirationDate;
						daysLeft = axNAVLicense.LicenseDaysRemaining;			
					}
					catch(err)
					{
						NAVError(resErr.oERR_NO_LICENSE);
					}			

					itemName = res.oNameTrial;
					
					item.state = stateWarn;
					if(daysLeft > 1)
    					item.value = res.oTextDays.innerHTML.replace(/#/, daysLeft);
    				else
    				    item.value = res.oTextDay.innerHTML.replace(/#/, daysLeft);
    					
					item.hintTitle = res.oTitleTrial.innerText;
					item.hintHTML = res.oHintTrial.innerHTML.replace(/#/, date);
					item.hintHTML = item.hintHTML.replace(/%s/, g_ProductName);
					item.helpId = IDH_TRYBUY_PREACTIVATION_TRIAL;
					item.hintBtnHidden = false;
					item.hintBtnEnabled = true;
					item.hintBtnEnabledAlways = true;
					item.hintBtnText = res.oBtnTextPurchase.innerText;
					item.hintBtnFunc = OnClickActivate;
					item.hintBtnArg = SYMALERT_RESULT_RESOLVELICENSE;
				}
				else
				{
					statusError = true;
				}
			}
			break;
			
		case DJSMAR_LicenseType_TryDie:
			{
				if (bExpired)
				{
					// STATUS_TRYDIE_EXPIRED
					trace("STATUS_TRYDIE_EXPIRED");
					
					itemName = res.oNameTrialExpired;
					
					item.state = stateUrgent;
					item.value = res.oTextExpired.innerText;
					item.hintTitle = res.oTitleTrialExpired.innerText;
					item.hintHTML = res.oHintTryDieExpired.innerHTML;
					item.helpId = 0;
					item.hintBtnHidden = true;
				}
				else if (bWarning)
				{
					// STATUS_TRYDIE_ACTIVE
					trace("STATUS_TRYDIE_ACTIVE");
					
					var date = "???";
					var daysLeft = "???";
					try
					{
						date = axNAVLicense.LicenseExpirationDate;	
						daysLeft = axNAVLicense.LicenseDaysRemaining;			
					}
					catch(err)
					{
						NAVError(resErr.oERR_NO_LICENSE);
					}			

					itemName = res.oNameTryDie;
					
					item.state = stateWarn;

					if(daysLeft > 1)
					{
						item.value = res.oTextDays.innerText.replace(/#/, daysLeft);
						item.hintHTML = res.oHintTryDieDays.innerHTML.replace(/#/, daysLeft);
					}
					else
					{
    					item.value = res.oTextDay.innerText.replace(/#/, daysLeft);					
    				    item.hintHTML = res.oHintTryDieDay.innerHTML.replace(/#/, daysLeft);
					}
					
					item.hintTitle = res.oTitleTryDie.innerText;
					item.helpId = 0;
					item.hintBtnHidden = true;
				}
				else
				{
					statusError = true;
				}
			}
			break;
		
		case DJSMAR_LicenseType_Rental:
			{
				if (bWarning)
				{
					if (bExpired)
					{					
						if(bGrace)
						{						
							// STATUS_RENTAL_GRACE_EXPIRED
							trace("STATUS_RENTAL_GRACE_EXPIRED");

							itemName = res.oNameRentalActivation;
							
							item.state = stateUrgent;
							item.value = res.oTextExpired.innerText;
							item.hintTitle = res.oTitleRentalActivation.innerText;
							item.hintHTML = res.oHintRetailExpired.innerHTML;
							item.hintHTML = item.hintHTML.replace(/%s/, g_ProductName);
							item.helpId = IDH_SCSS_PREACTIVATION_TRIAL_EXPIRED;
							item.hintBtnHidden = false;
							item.hintBtnEnabled = true;
							item.hintBtnEnabledAlways = true;
							item.hintBtnText = res.oBtnTextActivate.innerText;
							item.hintBtnFunc = OnClickActivate;
							item.hintBtnArg = -1;
						}
						else
						{
							// STATUS_RENTAL_TRIAL_EXPIRED
							trace("STATUS_RENTAL_TRIAL_EXPIRED");

							itemName = res.oNameRental;

							item.state = stateUrgent;
							item.value = res.oTextExpired.innerText;
							item.hintTitle = res.oTitleRentalExpired.innerText;
							item.hintHTML = res.oHintRentalExpired.innerHTML;
							item.helpId = IDH_SCSS_PREACTIVATION_TRIAL_EXPIRED;
							item.hintBtnHidden = false;
							item.hintBtnEnabled = true;
							item.hintBtnEnabledAlways = true;
							item.hintBtnText = res.oBtnTextViewOptions.innerText;
							item.hintBtnFunc = OnClickActivate;
							item.hintBtnArg = -1;
						}
					}
					else
					{
						if(bGrace)
						{						
							// STATUS_RENTAL_GRACE
							trace("STATUS_RENTAL_GRACE");
																	
							var days = "???";
							try
							{
								days = axNAVLicense.LicenseDaysRemaining;
							}
							catch (err)
							{
								NAVError(resErr.oERR_NO_LICENSE);
							}
					
							if(days > 1)
							{
    							item.value = res.oTextDays.innerHTML.replace(/#/, days);
								item.hintHTML = res.oHintRetailDays.innerHTML;							
    						}
    						else
    						{
    							item.value = res.oTextDay.innerHTML.replace(/#/, days);
								item.hintHTML = res.oHintRetailDay.innerHTML;							
    						}
    			        
							itemName = res.oNameRetail;
							item.state = stateWarn;
							item.hintTitle = res.oTitleRetail.innerText;
							item.hintHTML = item.hintHTML.replace(/%s/, g_ProductName);
							item.hintHTML = item.hintHTML.replace(/#/, days);							
							item.helpId = IDH_SCSS_PREACTIVATION_TRIAL;
							item.hintBtnHidden = false;
							item.hintBtnEnabled = true;
							item.hintBtnEnabledAlways = true;
							item.hintBtnText = res.oBtnTextActivate.innerText;
							item.hintBtnFunc = OnClickActivate;
							item.hintBtnArg = -1;
						}
						else
						{
							// STATUS_RENTAL_TRIAL
							trace("STATUS_RENTAL_TRIAL");
							
							var days = "???";
							try
							{
								days = axNAVLicense.LicenseDaysRemaining;
							}
							catch(err)
							{
								NAVError(resErr.oERR_NO_LICENSE);
							}			
													
							if(days < 11)
							{
								item.state = stateWarn;
							}
							else
							{
								item.state = stateOK;
							}

							if(days > 1)
							{
								item.hintHTML = res.oHintRentalDays.innerHTML.replace(/#/, days);
							}
							else
							{
								item.hintHTML = res.oHintRentalDay.innerHTML.replace(/#/, days);
							}
							
							itemName = res.oNameRental;
							
							item.value = res.oTextActive.innerText;
							item.hintTitle = res.oTitleRentalActive.innerText;			
							item.helpId = IDH_SCSS_PREACTIVATION_TRIAL;
							item.hintBtnHidden = false;
							item.hintBtnEnabled = true;
							item.hintBtnEnabledAlways = true;
							item.hintBtnText = res.oBtnTextViewOptions.innerText;
							item.hintBtnFunc = OnClickActivate;
							item.hintBtnArg = -1;
						}
					}					
				}			
				else
				{
					statusError = true;
				}
			}
			break;
			
		default:
			statusError = true;
			break;
   		}   	
   	}
   	else
   	{
		switch(licenseType)
		{
		case DJSMAR_LicenseType_Rental:
			{
				if (bKilled)
				{
					// STATUS_RENTAL_KILLED
					trace("STATUS_RENTAL_KILLED");

					itemName = res.oNameRental;
					
					item.state = stateUrgent;
					item.value = res.oTextCancelled.innerText;
					item.hintTitle = res.oTitleRentalCancelled.innerText;
					item.hintHTML = res.oHintRentalCancelled.innerText;				
					item.helpId = IDH_SCSS_KILLED;
					item.hintBtnHidden = false;
					item.hintBtnEnabled = true;
					item.hintBtnEnabledAlways = true;
					item.hintBtnText = res.oBtnTextViewOptions.innerText;
					item.hintBtnFunc = OnClickActivate;
					item.hintBtnArg = SYMALERT_RESULT_LAUNCH_ACTWIZ_KILL_PAGE;
				}
				else if (bExpired)
				{			
					// STATUS_RENTAL_EXPIRED
					trace("STATUS_RENTAL_EXPIRED");

					itemName = res.oNameRental;
					
					item.state = stateUrgent;
					item.value = res.oTextOutOfDate.innerText;
					item.hintTitle = res.oTitleRentalOutOfDate.innerText;					
					item.hintHTML = res.oHintRentalOutOfDate.innerHTML.replace(/%s/, g_ProductName);				
					item.helpId = IDH_SCSS_EXPIRED;
					item.hintBtnHidden = false;
					item.hintBtnEnabled = true;
					item.hintBtnEnabledAlways = true;
					item.hintBtnText = res.oBtnTextCheckStatus.innerText;
					item.hintBtnFunc = OnClickActivate;
					item.hintBtnArg = SYMALERT_RESULT_CONFIRM_SERVICE;
				}
				else if (bWarning)
				{
					// STATUS_RENTAL_WARNING
					trace("STATUS_RENTAL_WARNING");

					itemName = res.oNameRental;
					
					item.state = stateWarn;
					item.value = res.oTextAlert.innerText;
					item.hintTitle = res.oTitleRentalAlert.innerText;
					item.hintHTML = res.oHintRentalAlert.innerHTML.replace(/%s/, g_ProductName);				
					item.helpId = IDH_SCSS_WARNING;
					item.hintBtnHidden = false;
					item.hintBtnEnabled = true;
					item.hintBtnEnabledAlways = true;
					item.hintBtnText = res.oBtnTextCheckStatus.innerText;
					item.hintBtnFunc = OnClickActivate;
					item.hintBtnArg = SYMALERT_RESULT_CONFIRM_SERVICE;
				}
				else
				{
					// STATUS_RENTAL_ACTIVE
					trace("STATUS_RENTAL_ACTIVE");

					itemName = res.oNameRental;
					
					item.state = stateOK;
					item.value = res.oTextActive.innerText;
					item.hintTitle = res.oTitleRentalActive.innerText;
					item.hintHTML = res.oHintRentalActive.innerHTML.replace(/%s/, g_ProductName);				
					item.helpId = IDH_SCSS_ACTIVE;
					item.hintBtnHidden = true;
				}
			}
			break;
			
		case DJSMAR_LicenseType_OEM:
		case DJSMAR_LicenseType_Retail:
		case DJSMAR_LicenseType_Unlicensed:
		case DJSMAR_LicenseType_ESD:
		case DJSMAR_LicenseType_Trial:
		case DJSMAR_LicenseType_Beta:
			if (bExpired)
			{
				if (licenseType == DJSMAR_LicenseType_Beta)
				{
					// STATUS_TRYDIE_EXPIRED
					trace("STATUS_TRYDIE_EXPIRED");
					
					itemName = res.oNameTrialExpired;
					
					item.state = stateUrgent;
					item.value = res.oTextExpired.innerText;
					item.hintTitle = res.oTitleTrialExpired.innerText;
					item.hintHTML = res.oHintTryDieExpired.innerHTML;
					item.helpId = 0;
					item.hintBtnHidden = true;
				}
				else
				{
					statusError = true;
				}
			}
			else
			{
				// STATUS_RETAIL_ACTIVE
				trace("STATUS_RETAIL_ACTIVE");
				
				itemName = null;
				
				UpdateSubscriptionStatus(item);
			}
			break;	

		default:
			statusError = true;
			break;
		}
	}
	if (statusError)
	{
		// STATUS_ERROR
		trace("STATUS_ERROR");
		
		itemName = res.oNameRetailExpired;
		
		item.state = stateUrgent;
		item.value = res.oTextError.innerText;
		item.hintTitle = res.oTitleProductActivation.innerText;
		item.hintHTML = res.oHintLicenseError.innerHTML.replace(/%s/g, g_ProductName);
		item.helpId = 0;
		item.hintBtnHidden = true;
	}
	
	if(itemName != null)
	{
		oItemNameLS.innerText = itemName.innerText;
	}

	OnStatusChanged(item);

	trace("License status updated: " + result);
	
	return result;
}
				
function UpdateSubscriptionStatus(item)
{
	trace("- UpdateSubscriptionStatus");
	
	item.hintBtnHidden = true;
	item.hintBtnEnabled = true;
	item.hintBtnFunc = OnClickRenew;

	if(axNAVLicense.SubscriptionCanAutoRenew)
	{
		item.hintBtnText = res.oBtnTextVerifyStatus.innerText;
		item.helpId = IDH_SUB_AUTORENEW;		
	}
	else
	{
		item.hintBtnText = res.oBtnTextRenew.innerText;
		item.helpId = IDH_LS;
	}
	
	var subState = 0;	// UNKNOWN = 0, OK = 1, WARNING = 2, EXPIRED = 3
	var subStatus = 0;	// NotRunning = 2
	var subDate = "?";
	var subLeft = 0;
	var itemName = oNameDaysRemaining;
			
	try
	{
		subStatus = axNAVStatus.VirusDefSubStatus;
		subState =	axNAVStatus.VirusDefSubState;
		subDate =	axNAVStatus.VirusDefSubDate;
		subLeft =	axNAVStatus.VirusDefSubDaysLeft;
		
		trace("Subscription status: " + subStatus + " " + subState + " " + subDate + " " + subLeft);
	}
	catch (err)
	{
		NAVError(resErr.oERR_SUB_STATUS);
	}
				
	if (subStatus > 2 && subState != 0)
	{
		try
		{
			if (axNAVLicense.CanRenewSubscription)
			{
				// Display renew button if 
				// 1. Subscription is NON auto renewal
				// 2. Subscription is auto renewal and PEP says we can renew it.
				if(!axNAVLicense.SubscriptionCanAutoRenew || axNAVLicense.SubscriptionAutoRenewReady)
				{
					item.hintBtnHidden = false;
				}
			}
		}
		catch (err)
		{
			NAVError(resErr.oERR_NO_LICENSE);
		}
		
		if (subState == 3)
		{
			// Expired
			itemName = oNameSubscription;
			item.state = stateUrgent;
			
			if(axNAVLicense.SubscriptionCanAutoRenew)
			{
				item.hintHTML = oHintSubsExpiredAutoRenew.innerHTML.replace(/%s/, g_ProductName);					
				item.hintTitle = res.oTitleRetailOutOfDate.innerText;				
				item.value = res.oTextOutOfDate.innerText;				
			}
			else
			{
				item.hintHTML = res.oHintSubsExpired.innerHTML;			
				item.value = res.oTextExpired.innerText;
			}
					
			if (setProtectionLevel)
				navProtection = stateExpiredSub;
		}
		else
		{

			if ( subState == 2 )
			{
				// Warning
				item.state = stateWarn;
				item.value = subLeft;
				
				if(axNAVLicense.SubscriptionCanAutoRenew)
				{
					item.hintHTML = res.oHintSubsWarnAutoRenew.innerHTML.replace(/%s/, g_ProductName);	
					item.hintTitle = res.oTitleRetailAlert.innerText;				
				}
				else
				{
					if(subLeft > 1)
						item.hintHTML = res.oHintSubsWarnDays.innerHTML.replace(/#/, subLeft);
					else
						item.hintHTML = res.oHintSubsWarnDay.innerHTML.replace(/#/, subLeft);
				}
			}
			else
			{
				// OK
				item.state = stateOK;
				item.value = subLeft;

				if(axNAVLicense.SubscriptionCanAutoRenew)
					item.hintHTML = res.oHintSubsOK.innerHTML;
				else
					item.hintHTML = res.oHintSubsWarnDays.innerHTML.replace(/#/, subLeft);									
			}
		}
	}
	else
	{
		// Errors
		item.state = stateUrgent;
		item.value = subStatus == 2? res.oTextNotInstalled.innerText : textError;
		item.hintHTML = res.oHintSubsWarnDays.innerHTML.replace(/#/, subLeft);
		//item.hintHTML = res.oHintSubsOK.innerHTML;
	}
					
	oItemNameLS.innerText = itemName.innerText;
	if(subLeft >= 366)
	{
		item.hintBtnHidden = true;
	}
	OnStatusChanged(item);
}

function UpdateAPStatus()
{
	trace("-> UpdateAPStatus");

	var iAPStatus = statusNone;
	var bCanEnableAP = false;

	try
	{
		iAPStatus = axNAVStatus.APStatus;
		bCanEnableAP = axNAVStatus.APCanEnable;
		trace("AP status: " + iAPStatus);
	}
	catch (e)
	{
		NAVError(resErr.oERR_AP_STATUS);
	}
	
	var item = itemArray[indexAP];
	if(swInstalled == true)
	{
		item.hintHTML = res.oHintAP.innerHTML;
	}
	else
	{
		item.hintHTML = res.oHintAPNOAS.innerHTML;
	}
	item.hintBtnHidden = true;
	item.hintBtnArg = false;

	switch (iAPStatus)
	{
	case statusEnabled:
		item.state = stateOK;
		item.value = textOn;
		break;

	case statusNotRunning:
		item.hintBtnArg = true;
	case statusDisabled:
		item.state = stateUrgent;
		item.value = textOff;
		item.hintBtnText = res.oBtnTextTurnOn.innerText;
		item.hintBtnHidden = !bCanEnableAP;
		break;

	default :
		// Error
		item.state = stateUrgent;
		item.value = textError;
		break;
	}
    
    // Change AP
    OnStatusChanged(item);
    
    trace ("<- UpdateAPStatus");   
}

function UpdateSWStatus()
{
trace ("-> UpdateSWStatus");
    // Spyware
	var iSWStatus = statusNone;
	var bSpywareCat = false;
	var bCanEnableSpyware = false;
	try
	{
		bSpywareCat = axNAVStatus.SpywareCategory;
		bCanEnableSpyware = axNAVStatus.SpywareCanEnable;
	    trace("SW status: " + iSWStatus);
	    iSWStatus = statusEnabled; //code review item - adding a dummy for now, for the sake of error handling
	}
	catch (e)
	{
		NAVError(resErr.oERR_SW_STATUS);
	}

    trace ("SW item Array " + indexSW);	    
	var item = itemArray[indexSW];

    trace ("SW hintHTML");	    
	item.hintHTML = res.oHintSW.innerHTML;
	
	trace ("SW btn");	    
	// Button2 is "Turn on", Button is "Restore" (quarantine)
	
	item.hintBtn2Text = res.oBtnTextTurnOn.innerText;
	item.hintBtnText = res.oBtnTextRestore.innerText;

	
    trace ("SW switch " + bSpywareCat);	
    	
	//Defect 1-49EL9F: To maintain consistency with NSC and adhere to business rules:
	  //Spyware On - state = stateOK (Green)
	  //Spyware Off - state = stateWarn (Yellow)
	  //Adware not used in determining status
	  //Turning off spyware does not result in stateUrgent
	  
	  // If license is expired, tell the user Spyware is off.  
	  // In console, the feature proxy handles this for us.
	  licenseState = axNAVStatus.ProductLicenseState;
	  
	  if(iSWStatus != statusNone)
	  {// we could obtain status value from ccSettings	  
		if(bSpywareCat 
			&& (licenseState != DJSMAR00_LicenseState_EXPIRED)
			&& (licenseState != DJSMAR00_LicenseState_Violated))
		{
			iSWStatus = statusEnabled;
		}
		else
		{
			iSWStatus = statusDisabled;
		}
	  }
	  
	switch (iSWStatus)
	{
	case statusEnabled:
		item.state = stateOK;
		// Hide button2
		item.hintBtn2Hidden = true;
		item.hintBtn2Enabled = false;
	    
	    if(navProtection == stateOK)
	    {
			item.hintBtnHidden = false;
	    }
	    else
			item.hintBtnHidden = true;
	    item.hintBtnEnabled = true;
		
		// Category is on
		item.value = textOn;
    	break;

    // Category is off
	case statusDisabled:
	    // Show button1
	    item.hintBtnHidden = !bCanEnableSpyware;
	    item.hintBtnEnabled = true;
	    
		// Show button2 "Turn On"
		item.hintBtn2Hidden = !bCanEnableSpyware;
    	item.hintBtn2Enabled = true;
	
		item.state = stateWarn;   //YELLOW
		item.value = textOff;
    	break;
	default:
		// Error
	    // Show button1
	    item.hintBtnHidden = true;
	    item.hintBtnEnabled = true;
	    
	    // Show button2
	    item.hintBtn2Hidden = false;
	    item.hintBtn2Enabled = true;

		item.state = stateWarn;   //YELLOW
		item.value = textError;
    	break;
	}

    // Change SW
	OnStatusChanged(item);
    trace ("<- UpdateSWStatus");	
}

function UpdateEmailStatus()
{
	trace("- UpdateEmailStatus");
	
	var iEmailStatus = statusNone;
	var bPOP = false;
	var bSMTP = false;
	var bCanEnableEm = false;

	try
	{
		iEmailStatus = axNAVStatus.EmailStatus;
		bPOP = axNAVStatus.EmailPOP;
		bSMTP = axNAVStatus.EmailSMTP;
		bCanEnableEm = axNAVStatus.EmailCanEnable;
		trace("Em status: " + iEmailStatus);
	}
	catch (e)
	{
		NAVError(resErr.oERR_EMAIL_STATUS);
	}

	var itemName = res.oNameEmail;
	
	var item = itemArray[indexEm];

	if(swInstalled == true)
	{
		item.hintHTML = res.oHintEm.innerHTML;
	}
	else
	{
		item.hintHTML = res.oHintEmNOAS.innerHTML;
	}	
	item.hintBtnText = res.oBtnTextTurnOn.innerText;
	item.hintBtnHidden = true;

	switch (iEmailStatus )
	{
	case statusEnabled:
		item.state = stateOK;
		item.value = textOn;
		
		// Now figure out which title to use
		if (!bPOP || !bSMTP)
			itemName = bPOP? res.oNameEmailIncoming : res.oNameEmailOutgoing;
		break;

	case statusDisabled:
		item.state = stateWarn;
		item.value = textOff;
		item.hintBtnHidden = !bCanEnableEm;
		break;

	default:
		item.state = stateUrgent;
		item.value = textError;
		break;
	}

	oItemNameEm.innerText = itemName.innerText;
	
	OnStatusChanged(item);
}

function UpdateWPStatus()
{
	trace("- UpdateIWPStatus");
	
	var iIWPStatus = statusNone;
	var bCanEnableIWP = false;

	if (axOptions.IWPState == IWPStateYielding)
	{
		iIWPStatus = statusUpgraded;
	}
	else
	{
		try
		{
			iIWPStatus = axNAVStatus.IWPStatus;
			bCanEnableIWP = axOptions.IWPCanEnable;
			trace("IWP status: " + iIWPStatus);
		}
		catch (e)
		{
			NAVError(resErr.oERR_IWP_STATUS);
		}
	}

	var item = itemArray[indexWP];

	item.hintHTML = res.oHintWP.innerHTML;
	item.hintBtnText = res.oBtnTextTurnOn.innerText;
	item.hintBtnHidden = true;
	item.helpId	= IDH_WP;

	switch (iIWPStatus)
	{
	case statusUpgraded:
		item.state = stateUpgraded;
		item.value = res.oTextUpgraded.innerText;
		item.helpId	= IDH_WPUpgraded;
		item.hintHTML = res.oHintWPUpgraded.innerHTML;
		break;
		
	case statusEnabled:      
	case statusNotRunning:   // Yielding - OK
		item.state = stateOK;
		item.value = textOn;
		break;

	case statusNotInstalled:  // agent not running but should be - error
		item.state = stateUrgent;
		item.value = textOff;
		item.hintBtnHidden = false;
		break;

	case statusDisabled:
		item.state = stateUrgent;
		item.value = textOff;
		item.hintBtnHidden = !bCanEnableIWP;
		break;

	default:
		item.state = stateUrgent;
		item.value = textError;
		break;
	}

	OnStatusChanged(item);
}


function UpdateFullSystemScan()
{
	trace("- UpdateFullSystemScan");
	
	var lFSSStatus	= statusNone;
	var lScanAge	= 0;
	var strFSSDate	= "?";

	try
	{
		lFSSStatus = axNAVStatus.FullSystemScanStatus;
		lScanAge = axNAVStatus.FullSystemScanAge;
		strFSSDate = axNAVStatus.FullSystemScanDate;
		trace("FS status: " + lFSSStatus + ", Age: " + lScanAge);
	}
	catch (e)
	{
		NAVError(resErr.oERR_FSS_STATUS);
	}

	var item = itemArray[indexFS];
	
	// Format text that contains product name
	//res.oHintFS.innerText = res.oHintFS.innerText.replace(/%s/, g_ProductName);
	//Defect 1-44CM1PFormatting not required anymore, since new text proposed by usability does not contain product name
	
	item.hintHTML = res.oHintFS.innerHTML;
	item.hintBtnText = res.oBtnTextScanNow.innerText;
	item.hintBtnHidden = false;

	if (lFSSStatus == statusEnabled)
	{
		item.state = lScanAge < 90? stateOK : stateWarn;
		item.value = strFSSDate;
	}
	else
	{
		item.state = stateWarn;
		item.value = res.oTextNotCompleted.innerText;
	}

	if (navProtection != stateOK)
	{
		item.state = stateWarn;
	}

	OnStatusChanged(item);
}

function UpdateVirusDefStatus()
{
	trace("- UpdateVirusDefStatus");

	var defsOld = 14;		// days old before warning
	var defsReallyOld = 30;	// days old before urgent

	var iVirusDefStatus = statusNone;
	var defsAge = 0;
	var defsDate = "?";
	
	try
	{
		trace("Get VirusDefStatus");
		iVirusDefStatus = axNAVStatus.VirusDefStatus;
		
		trace("Get VirusDefAge");
		defsAge = axNAVStatus.VirusDefAge;
		
		trace("Get VirusDefDate");
		defsDate = axNAVStatus.VirusDefDate;
		
		trace("VD status: " + iVirusDefStatus + ", age: " + defsAge + ", date: " + defsDate);
	}
	catch (e)
	{
		NAVError(resErr.oERR_VIRUSDEF_STATUS);
	}

	var item = itemArray[indexVD];
	
	item.hintHTML = res.oHintVDWarn.innerHTML;
	item.hintBtnText = res.oBtnTextRunLU.innerText;
	item.hintBtnHidden = false;
		
	switch (iVirusDefStatus)
	{
	case statusEnabled:
		item.state = (defsAge > defsReallyOld)? stateUrgent :
					((defsAge > defsOld)? stateWarn : stateOK);
		item.value = defsDate;
		if (item.state == stateOK)
		{
			item.hintHTML = res.oHintVD.innerHTML;
			item.hintBtnHidden = true;
		}
		break;

	case statusNotAvailable:
		item.state = stateOK;
		item.value = res.oTextUpdating.innerText;
		item.hintBtnHidden = true;
		break;

	default:
		item.state = stateUrgent;
		item.value = textError;
		break;
	}
	
	if (item.state == stateUrgent && navProtection == stateOK)
		navProtection = stateOldUpdates;

	OnStatusChanged(item);
}

function UpdateALUStatus()
{
	trace("- UpdateALUStatus");
	
	var iALUStatus = statusNone;
	var bCanEnableLU = false;

	try
	{
		iALUStatus = axNAVStatus.ALUStatus;
		bCanEnableLU = axNAVStatus.ALUCanEnable;
		trace("ALU status: " + iALUStatus);
	}
	catch (NAVErr)
	{
		NAVError(resErr.oERR_ALU_STATUS);
	}

	var item = itemArray[indexLU];
	
	item.hintHTML = res.oHintLU.innerHTML;
	item.hintBtnText = res.oBtnTextTurnOn.innerText;
	item.hintBtnHidden = true;
		
	switch (iALUStatus)
	{
	case statusEnabled:
		item.state = stateOK;
		item.value = navProtection == stateExpiredSub? res.oTextHolding.innerText : textOn;
		break;

	case statusDisabled:
		item.value = textOff;
		
		if (navProtection == stateExpiredSub)
		{
			item.state = stateOK;	// Let OnStatusChanged to take case of this item
			break;
		}
		item.state = stateWarn;
		item.hintBtnHidden = !bCanEnableLU;
		break;

	case statusNotAvailable:
		// Feature not supported (no Task Scheduler)
		item.state = stateOK;
		item.value = textOff;
		break;

	default:
		item.state = stateUrgent;
		item.value = textError;
		break;
	}

	OnStatusChanged(item);
}

//----

function OnMoreInfo()
{
	trace("-- More Info: " + curItem.helpId);

	// if (curItem.hintOverridden)
	//	SelectItem(navProtection == stateExpiredSub? indexLS : indexVD);
	// else
	try
	{
	    axLauncher.LaunchHelp(curItem.helpId);
	}
	catch(NAVErr)
	{
	    axLauncher.NAVError.LogAndDisplay(0);
	    return;
	}
}

function OnButtonClick()
{
	trace("-- Button clicked");
	
	if (curItem.hintOverridden && curItem.index != indexSW)
	{
		trace("Hint overridden");
		
		if (navProtection == stateExpiredSub)
			OnClickRenew();
		else
			OnClickRunLU();
	} 
	else if (curItem.hintBtnFunc != null)
	{
		curItem.hintBtnFunc();
	}
}

function OnButton2Click()
{
	trace("-- Button2 clicked");
	
	if (curItem.hintOverridden)
	{
		trace("Hint overridden");
		
		if (navProtection == stateExpiredSub)
			OnClickRenew();
		else
			OnClickRunLU();
	} 
	else if (curItem.hintBtn2Func != null)
	{
		curItem.hintBtn2Func();
	}
}

function OnClickEnableAP()
{
	try
    {
	    axOptions.Load ();
	    axOptions.STARTUP.LoadVxD = 1;
	    axOptions.Save ();
	}
	catch(NAVErr)
	{
	    axOptions.NAVError.LogAndDisplay(0);
	    return;
	}
	
	try
	{
	    trace("Enable AP");
	    axAP.Enabled = 1;
	    axAP.Configure(true);
	}
	catch(NAVErr)
	{
	    // Log and display the AP Scriptable error we recieved while attempting to enable
	    // and return since we will not want to set the snooze period in this case
	    axAP.NAVError.LogAndDisplay(0);
	    return;
	}
		
	axOptions.SetSnoozePeriod(SnoozeAP, 0);
}

function OnClickEnableEm()
{
	trace("Enable EM");

    try
    {
	    axOptions.Load ();
	    axOptions.NAVEMAIL.ScanOutgoing = 1;
	    axOptions.NAVEMAIL.ScanIncoming = 1;
	    axOptions.Save ();
	}
	catch(NAVErr)
	{
	    axOptions.NAVError.LogAndDisplay(0);
	    return;
	}
	
	axOptions.SetSnoozePeriod(SnoozeEmailIn | SnoozeEmailOut, 0);
}

function OnClickEnableWP()
{
	trace("Enable WP");
    try
    {
	    axOptions.Load ();
	    axOptions.IWPUserWantsOn = 1;
	    axOptions.Save ();
	}
	catch(NAVErr)
	{
	    axOptions.NAVError.LogAndDisplay(0);
	    return;
	}
	
	axOptions.SetSnoozePeriod(SnoozeIWP, 0);
}

var canLaunchScan = true;
var oldCursor = null;

function OnClickScanNow()
{
	trace("Scan Now");
	if (canLaunchScan && CreateAxScanTasks())
	{
	    try
	    {
		    axNAVScanTasks.FullOrQuickScan();
		}
		catch(NAVErr)
		{
		    axNAVScanTasks.NAVError.LogAndDisplay(0);
		    return;
		}
		
		canLaunchScan = false;
		oldCursor = oHintButton.style.cursor;
		oHintButton.style.cursor = "wait";
		setTimeout(EnableScanLaunch, 2000);
	}
}

function EnableScanLaunch()
{
	trace("Scan launch enabled");
	oHintButton.style.cursor = oldCursor;
	canLaunchScan = true;
}

function OnClickRunLU()
{
	trace("Run LiveUpdate");
	try
	{
	    axLauncher.LaunchLiveUpdate();
	}
	catch(NAVErr)
	{
		axLauncher.NAVError.LogAndDisplay(0);
		return;
	}
}

function OnClickEnableALU()
{
	trace("Enable Auto-LU");

    try
    {
	    axOptions.Load ();
	    axOptions.LiveUpdate = 1;
	    axOptions.Save ();
	}
	catch(NAVErr)
	{
		axOptions.NAVError.LogAndDisplay(0);
		return;
	}
	
	axOptions.SetSnoozePeriod(SnoozeALU, 0);
}

function OnClickActivate()
{
	trace("Launch Activation Wizard");
	var HWNDPARENT_FIND = 1; // Ask NAVlicense to find the parent window, which is the integrator.
	
	try
	{
	    axNAVLicense.LaunchActivationWizardEx(0, curItem.hintBtnArg, HWNDPARENT_FIND);
	}
	catch(NAVErr)
	{
	    axNAVLicense.NAVError.LogAndDisplay(0);
	    return;
	}
}

function OnClickRenew()
{
	trace("Launch Subscription Wizard");
	try
	{
	    axNAVLicense.LaunchSubscriptionWizard(-1); // -1 == INVALID_HANDLE_VALUE
	}
	catch(NAVErr)
	{
	    axNAVLicense.NAVError.LogAndDisplay(0);
	    return;
	}
}

function OnClickEnableSW()
{
	trace("OnClickEnableSW");
	try
	{
	    axOptions.Load ();

        // Turn the spyware threat cat on
        axOptions.THREAT.Threat6 = 1;
        
	    axOptions.Save ();
	}
	catch(NAVErr)
	{
        axOptions.NAVError.LogAndDisplay(0);	
	    return;
	}
}

function OnClickQuarantine()
{
	trace("OnClickQuarantine");
	try
	{
	    var axLauncher = new ActiveXObject("Symantec.Norton.AntiVirus.AppLauncher");
	    axLauncher.LaunchQuarantine();	    
	}
	catch(NAVErr)
	{
	    axLauncher.NAVError.LogAndDisplay(0);
	    return;
	}
}
