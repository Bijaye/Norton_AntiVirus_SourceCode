////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Alert.js
// TRACE disabled
var	TRACE =	false;

if (TRACE)
{
	try
	{
		top.axVkTrace =	new	ActiveXObject("VkTools.VkTraceCC");
		top.axVkTrace.SetModuleName("NavScanJs");
	}
	catch (e)
	{
		TRACE =	false;
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
			TRACE =	false;
		}
	}
}


//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~
// HTML	pages that include Alert.js
//	must also include SymAlertStatic.js
//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

//
// Help	IDS
//
var	IDH_NAV_ALERT_WARNING_SUBSCRIPT	= 11561;
var	IDH_NAV_ALERT_EXPIRED_SUBSCRIPT	= 11560;
var	IDH_NAV_ALERT_EXPIRED_SUBSCRIPT_ONSCAN = 12100;
var	IDH_NAV_ALERT_EXPIRED_VIRUSDEFS	= 12150;
var	IDH_NAV_ALERT_EXPIRED_VIRUSDEFS_ONSCAN = 12151;
var	IDH_NAV_ALERT_ALU_DISABLED = 12152;

// ActiveX object
var	g_DefAlert = null;
var	g_NAVOptions = null;

// Licensing variables
var	g_bCanRenewSub = false;
var	g_CancelCode = 0;
var	g_LicenseDaysRemaining = 0;
var	g_szVendorName;

g_ModuleID = 3031;
g_ScriptID = 100;  // This uniquely	defines	this script	file to	the	error handler

var	CYCLE_SUBSEXPIRED	= "SECycle";
var	CYCLE_DEFSOUTOFDATE	= "DefsCycle";	
var	CYCLE_SCANSUBSEXPIRED	= "S_SECyc";
var	CYCLE_SCANDEFSOUTOFDATE	= "S_DCyc";

// NAV status returned by ActiveX
var	STATUS_DISABLED	= 0;	
var	STATUS_ENABLED = 1;

var	_1_DAY = 86400;	// in seconds

var	iTimerID;
var	bTimerRunning =	false;
// 2 minute	timeout
var	TIMEOUT	= 120000;

var	CANCELCODE_NOT_VENDORS_MEMBER		= 100;
var	CANCELCODE_VENDOR_CANCELLED_SERVICE	= 101;
var	CANCELCODE_USER_CANCELLED_SERVICE	= 102;
var	CANCELCODE_SERVICE_ON_HOLD			= 103;
var	CANCELCODE_SERVICE_TRANSFERRED		= 99;
var	CANCELCODE_SERVICE_DISCONTINUED		= 105;

var cltLicenseType_SOS			= (0x00000040);   // SOS is Rental model (SCSS)

var cltLicenseAttrib_Subscription_Renewable    	 =  (0x00020000);   // The subscription is currently in the renewable period

var POLICY_ID_QUERY_PRODUCT_INFORMATION	= (0xA0000021); // product info

// Vendor resource id in isResource.h
var	IDS_BRANDING_PRODUCT_NAME	= 5000;

// ========================================== 
//			Helper functions
// ========================================== 

function NAVError(errObj)
{
	trace("*** NAV error");
	
	var	e =	errObj.innerText.split("|",	2);
	var	errNumber =	e[0];
	var	errText	= e[1];
	if (errText.length == 0)
		errText	= resErr.oERR_DEFAULT.innerText;

	trace("*** Error: "	+ errNumber	+ "	: "	+ errText);
		
	g_ErrorHandler.DisplayNAVError(errText,	errNumber);		// g_ErrorHandler defined in DisplayScriptError.js
}

function CreateAxDefAlert()
{
	try
	{
		if(g_DefAlert == null)
		{
			g_DefAlert = new ActiveXObject ("DefAlert.SymNavDefAlert");
		}
	}
	catch(err)
	{
		NAVError(resErr.oERR_NO_DEFALERT);
		window.navigate("res://closeme.xyz");
		return false;
	}
	
	return true;
}
	

function CreateAxNAVOptions()
{	
	try
	{
		if(g_NAVOptions	== null)
		{
			g_NAVOptions = new ActiveXObject("Symantec.Norton.AntiVirus.NAVOptions");
		}

	}
	catch(err)
	{
		NAVError(resErr.oERR_NO_OPTIONS);
		return false;
	}
	
	return true;
}

function DisableNotifyOption()
{
	days.disabled =	true;
}

function EnableNotifyOption()
{
	days.disabled =	false;
}

function DisableNotifyAndDontShowOptions()
{
	days.disabled =	true;
	btnDontShow.disabled = true;
}

function EnableNotifyAndDontShowOptions()
{
	days.disabled =	false;
	btnDontShow.disabled = false;
}

function OnHelpClick(TopicID)
{
	try
	{	
		g_DefAlert.LaunchHelp(TopicID);
	}
	catch(err)
	{
		g_DefAlert.NAVError.LogAndDisplay(0);
	}

	return false;
}

function OnClose()
{
	CancelTimer();
	location.href =	"res://closeme.xyz/";
}

function AlertOnLoad()
{
	window.returnValue = SYMALERT_RESULT_CANCEL;		
	
	if(!CreateAxDefAlert())
		return;
		
	try
	{
		if(g_DefAlert.IsHighContrast())	
		{
			// Enable scroll
			document.body.scroll = "yes";					
			
			// Hide	all	bitmaps	and	icons
			imgIcon.style.display = "none";
			btnOK.className	= "";
		}
		
		// product name.
		productName.innerText =	g_DefAlert.GetStringFromBrandingResource(IDS_BRANDING_PRODUCT_NAME, false);
	}
	catch(err)
	{
		g_DefAlert.NAVError.LogAndDisplay(0);
	}
}

function CancelTimer()
{
	// If the timer	is running
	if (bTimerRunning == true)
	{
		// cancel the timer
		clearInterval(iTimerID);
		bTimerRunning =	false;
	}
}

// ========================================== 
//			ALUDisabled	functions 
// ========================================== 

function ALUDisabledOnClickOK()
{
	if(btnEnableALU.checked	== true)
		window.returnValue = SYMALERT_RESULT_ENABLEALU;		
		
	OnClose();
}

// ========================================== 
//			DefsOutOfDates functions 
// ========================================== 

function DefsOutOfDateOnLoad()
{	
	window.returnValue = SYMALERT_RESULT_CANCEL;		

	if(!CreateAxDefAlert())
		return;
	
	try
	{
		if(g_DefAlert.IsHighContrast())	
		{
			// Enable scroll
			document.body.scroll = "yes";					
			
			// Hide	all	bitmaps	and	icons
			imgIcon.style.display = "none";
			btnOK.className	= "";
		}
					
		// If ALU is disabled, show	"Enable	ALU" checkbox.
		if(STATUS_DISABLED == g_DefAlert.ALUStatus)	
		{
			if(g_DefAlert.OptionsChangeAllowed)
			{
				// Show "Enable ALU" option
				optEnableALU.style.display = "";
			}
		}

		// Format the heading	
		spanHeadline.innerText = spanHeadline.innerText.replace(/#/, g_DefAlert.DefinitionsDate);
			
		g_DefAlert.SetAlertDword(CYCLE_DEFSOUTOFDATE, _1_DAY);		
	}
	catch(err)
	{
		g_DefAlert.NAVError.LogAndDisplay(0);
	}
}

function DefsOutOfDateOnClickOK()
{
	// Update the DefsOutOfDate	cycle anyway in	case the user
	//	selects	LU but then	cancels	it later.

	try
	{
		g_DefAlert.SetAlertDword(CYCLE_DEFSOUTOFDATE, (days.value *	_1_DAY));			
	}
	catch(err)
	{
		g_DefAlert.NAVError.LogAndDisplay(0);
	}

	if(btnLU.checked ==	true)
	{	
		if((optEnableALU.style.display == "") && btnEnableALU.checked)
		{			
			window.returnValue = SYMALERT_RESULT_LAUNCHLU_AND_ENABLE_ALU;
		}
		else
		{
			window.returnValue = SYMALERT_RESULT_LAUNCHLU;
		}
	}
	else if((optEnableALU.style.display == "") && btnEnableALU.checked)
	{
		window.returnValue = SYMALERT_RESULT_ENABLEALU;		
	}
		
	OnClose();
}

// ========================================== 
//	   Scanner DefsOutOfDates functions	
// ========================================== 

function Scan_DefsOutOfDateOnLoad()
{
	window.returnValue = SYMALERT_RESULT_CANCEL;	
	
	// Start the timer to close	the	window in 2	minutes
	iTimerID = setInterval("OnClose()",	TIMEOUT);
	bTimerRunning =	true;
	
	if(!CreateAxDefAlert() || !CreateAxNAVOptions())
		return;

	try
	{
		if(g_DefAlert.IsHighContrast())	
		{
			// Enable scroll
			document.body.scroll = "yes";					
			
			// Hide	all	bitmaps	and	icons
			imgIcon.style.display = "none";
			btnOK.className	= "";
		}
					
		// If ALU is disabled, show	"Enable	ALU" checkbox.
		if(STATUS_DISABLED == g_DefAlert.ALUStatus) 
		{
			if(g_DefAlert.OptionsChangeAllowed)
			{
				// Show "Enable ALU" option
				optEnableALU.style.display = "";
			}
		}
		
		// Format heading
		spanHeadline.innerText = spanHeadline.innerText.replace(/#/, g_DefAlert.DefinitionsDate);
			
		g_DefAlert.SetAlertDword(CYCLE_SCANDEFSOUTOFDATE, 0);		
	}
	catch(err)
	{
		g_DefAlert.NAVError.LogAndDisplay(0);
	}
}

function Scan_DefsOutOfDateOnClickOK()
{
	CancelTimer();
	
	try
	{
		if(btnLU.checked ==	true)
		{	
			if((optEnableALU.style.display == "") && btnEnableALU.checked)
			{			
				window.returnValue = SYMALERT_RESULT_LAUNCHLU_AND_ENABLE_ALU;
			}
			else
			{
				window.returnValue = SYMALERT_RESULT_LAUNCHLU;
			}			
		}
		else if((optEnableALU.style.display == "") && btnEnableALU.checked)
		{
			window.returnValue = SYMALERT_RESULT_ENABLEALU;		
		}
		
		if(btnLU.checked ==	false)
		{
			g_DefAlert.SetAlertDword(CYCLE_SCANDEFSOUTOFDATE, (days.value *	_1_DAY));			
		}		
	}
	catch(err)
	{
		g_DefAlert.NAVError.LogAndDisplay(0);
	}
	
	OnClose();
}

// ========================================== 
//			Subscription Warning
// ========================================== 

function SubWarningOnLoad()
{
	window.returnValue = SYMALERT_RESULT_CANCEL;		

	if(!CreateAxDefAlert() || !CreateAxNAVOptions())
		return;
		
	try
	{
		GetProductAndLicensingData();
		
		if(g_DefAlert.IsHighContrast())	
		{
			// Enable scroll
			document.body.scroll = "yes";					
			
			// Hide	all bitmaps	and icons
			imgIcon.style.display = "none";
			btnOK.className	= "";
		}

		productName.innerText =	g_DefAlert.GetStringFromBrandingResource(IDS_BRANDING_PRODUCT_NAME, false);
		
		if(g_NAVOptions.THREAT_NoThreatCat)
		{
			details_nospyware.style.display = "";
			details_spyware.style.display = "none";
		}			
		
		// Set days	left and renewal date.		
		daysLeft.innerText = g_nSubDaysRemaining;

		// Check for Admin right.			
		if(!g_bCanRenewSub)
		{
			// Display text	"Admin rights required!"
			spanRenew.style.display = "none";	
			btnRenew.checked = false;			
			spanAdminRights.style.display = "";	
		}
	}
	catch(err)
	{
		g_DefAlert.NAVError.LogAndDisplay(0);
	}	
}

function SubWarningOnClickOK()
{
	if(btnRenew.checked	== true)
		window.returnValue = SYMALERT_RESULT_RENEWSUBS;		

	OnClose();
}

// ========================================== 
//			Subscription Expired
// ========================================== 

function SubExpiredOnLoad()
{
	// Assume users	cancel the alert
	window.returnValue = SYMALERT_RESULT_CANCEL;		

	if(!CreateAxDefAlert() || !CreateAxNAVOptions())
		return;
	
	try
	{
		GetProductAndLicensingData();
		
		if(g_DefAlert.IsHighContrast())	
		{
			// Enable scroll
			document.body.scroll = "yes";					
			
			// Hide	all	bitmaps	and	icons
			imgIcon.style.display = "none";
			btnOK.className	= "";
		}

		// Set product name.				
		productName.innerText =	g_DefAlert.GetStringFromBrandingResource(IDS_BRANDING_PRODUCT_NAME, false);
		
		// Display appropriate text	if there is	nospyware feature
		if(g_NAVOptions.THREAT_NoThreatCat)
		{
			headline_nospyware.style.display = "";
			headline_spyware.style.display = "none";
		}
		
		// Assume they selected	"Notify	me in 1	day".	
		g_DefAlert.SetAlertDword(CYCLE_SUBSEXPIRED,	_1_DAY);		

		if(g_nSubDaysRemaining < 0)
		{
			var	nDaysExpired = Math.abs(g_nSubDaysRemaining);
			if(nDaysExpired	>= 30)
			{
				spanDontShowAgain.style.display =	"";
				btnDontShow.disabled = true;
				days15.selected	= true;
				days1.selected = false;
			}		
		}
		if(!g_bCanRenewSub)
		{
			spanRenew.style.display =	"none";	
			btnRenew.checked = false;			
			spanAdminRights.style.display = "";	
		}
	}
	catch(err)
	{
		g_DefAlert.NAVError.LogAndDisplay(0);
	}	
}

function SubExpiredOnClickOK()
{
	if(btnRenew.checked	== true)
	{
		window.returnValue = SYMALERT_RESULT_RENEWSUBS;			
	}
	else
	{
		try
		{
			if(btnDontShow.checked == true)
			{
				g_DefAlert.SetAlertDword(CYCLE_SUBSEXPIRED,	-1);		
			}
			else
			{
				g_DefAlert.SetAlertDword(CYCLE_SUBSEXPIRED,	days.value * _1_DAY);		
			}			
		}
		catch(err)
		{
			g_DefAlert.NAVError.LogAndDisplay(0);
		}			
	}

	OnClose();
}

// ========================================== 
//		Scanner	Subscription Expired
// ========================================== 

function Scan_SubExpiredOnLoad()
{
	// Set default users' selections - Assume users	cancel the alert
	window.returnValue = SYMALERT_RESULT_CANCEL;	
	
	// Start the timer to close	the	window in 2	minutes
	iTimerID = setInterval("OnClose()",	TIMEOUT);
	bTimerRunning =	true;
	
	if(!CreateAxDefAlert() || !CreateAxNAVOptions())
		return;
		
	try
	{
		GetProductAndLicensingData();

		productName.innerText =	g_DefAlert.GetStringFromBrandingResource(IDS_BRANDING_PRODUCT_NAME, false);
		
		if(g_DefAlert.IsHighContrast())	
		{
			// Enable scroll
			document.body.scroll = "yes";					
			
			// Hide	all	bitmaps	and	icons
			imgIcon.style.display = "none";
			btnOK.className	= "";
		}
					
		// Display appropriate text	if there is	nospyware feature
		if(g_NAVOptions.THREAT_NoThreatCat)
		{
			headline_nospyware.style.display = "";
			headline_spyware.style.display = "none";
		}
		
		// Set display cycle to	0 days so it appears again in case the user	dismiss	alert.
		g_DefAlert.SetAlertDword(CYCLE_SCANSUBSEXPIRED,	0);	
		
		// If subscriptions	has	expired	for	30 days,
		// display option "Don't show this again".
		if(g_nSubDaysRemaining < 0)
		{
			var nDaysExpired = Math.abs(g_nSubDaysRemaining);
			if(nDaysExpired	>= 30)
			{
				spanDontShowAgain.style.display =	"";
				btnDontShow.disabled = true;
				days15.selected	= true;
				days1.selected = false;
			}				
		}

		if(!g_bCanRenewSub)
		{
			spanRenew.style.display =	"none";	
			btnRenew.checked = false;		
			spanAdminRights.style.display = "";	
		}
	}
	catch(err)
	{
		g_DefAlert.NAVError.LogAndDisplay(0);
	}
}

function Scan_SubExpiredOnClickOK()
{
	CancelTimer();
	
	if(btnRenew.checked	== true)
	{
		window.returnValue = SYMALERT_RESULT_RENEWSUBS;			
	}
	else
	{
		try
		{
			if(btnDontShow.checked == true)
			{
				g_DefAlert.SetAlertDword(CYCLE_SCANSUBSEXPIRED,	-1);		
			}
			else
			{
				g_DefAlert.SetAlertDword(CYCLE_SCANSUBSEXPIRED,	days.value * _1_DAY);		
			}
		}
		catch(err)
		{
			g_DefAlert.NAVError.LogAndDisplay(0);
		}
	}

	OnClose();
}


// 
// PostActivation Expired Killed Alert
// 
function LicensingCancelledOnLoad()
{
	window.returnValue = SYMALERT_RESULT_CANCEL;		

	if(!CreateAxDefAlert())
		return;
		
	try
	{		
		GetProductAndLicensingData();

		var szProductName = g_DefAlert.GetStringFromBrandingResource(IDS_BRANDING_PRODUCT_NAME, false);

		// Set product name.
		rsnProductName.innerText = szProductName;
		optUninstallProductName.innerText =	szProductName;
		
		// System in high contrast mode, 
		if(g_DefAlert.IsHighContrast())	
		{
			// Enable scroll.
			document.body.scroll = "yes";			
						
			// Hide	all	bitmaps	and	icons.
			imgIcon.style.display = "none";
			btnCancel.className	= "";
			btnOK.className	= "";
		}
		
		switch(g_CancelCode)
		{
		// 100
		case CANCELCODE_NOT_VENDORS_MEMBER:	
			rsnNotAMember.style.display	= "";
			optUpsell.style.display	= "";		
			optUninstall.style.display = "";
			optReactivate.style.display = "";
			btnUpsell.checked =	true;
		break;
			
		// 101
		case CANCELCODE_VENDOR_CANCELLED_SERVICE:
			rsnVendorCancelledService.style.display	= "";
			rsnVendorCancelledService_VendorName.innerText = g_szVendorName;
			optContactVendor.style.display = "";
			optContactVendor_VendorName.innerText =	g_szVendorName;
			optReactivate.style.display	= "";			
			optUpsell.style.display	= "";
			optUninstall.style.display = "";		
			btnUpsell.checked =	true;
		break;
		
		// 102
		case CANCELCODE_USER_CANCELLED_SERVICE:
			rsnUserCancelledService.style.display =	"";
			optReactivate.style.display	= "";
			optReactivateOnResolved.style.display = "none"
			optReactivateOnResubscribe.style.display = "";
			optUpsell.style.display	= "";
			optUninstall.style.display = "";
			btnUpsell.checked =	true;
		break;
		
		// 99
		case CANCELCODE_SERVICE_TRANSFERRED:
			rsnServiceTransferred.style.display	= "";				
			optReactivate.style.display	= "";
			optReactivateOnResolved.style.display = "none";
			optReactivateOnTransfer.style.display="";
			optUninstall.style.display = "";
			btnReactivate.checked =	true;
		break;			
		
		// 105
		case CANCELCODE_SERVICE_DISCONTINUED:	
			rsnServiceDiscontinued.style.display = "";
			rsnServiceDiscontinued_VendorName.innerText = g_szVendorName;			
			rsnServiceDiscontinued_VendorName.style.display	= "";
			optUpsell.style.display	= "";
			optUninstall.style.display = "";
			btnUpsell.checked =	true;
		break;			
		
		// 103 and Unknown
		case CANCELCODE_SERVICE_ON_HOLD:
		default:
			rsnServiceOnHold.style.display = "";				
			rsnServiceOnHold_VendorName.innerText =	g_szVendorName;				
			optContactVendor.style.display = "";			
			optContactVendor_VendorName.innerText =	g_szVendorName;
			optReactivate.style.display	= "";
			optUpsell.style.display	= "";
			optUninstall.style.display = "";
			btnUpsell.checked =	true;
		break;
		}					
	}
	catch(err)
	{
		g_DefAlert.NAVError.LogAndDisplay(0);
	}	
}

function LicensingCancelledOnClickOK()
{
	// Must	check to see if	a particular button	is displayed before
	//	checking to	see	if user	selected it; otherwise,	an exception
	//	will be	thrown.
	if(typeof btnContactVendor != "undefined" && btnContactVendor.checked == true)
	{
		// Launch ReEnableURL
		window.returnValue = SYMALERT_RESULT_LAUNCH_SERVICE_REACTIVATION_WEBSITE;		
	}
	else if(typeof btnReactivate !=	"undefined"	&& btnReactivate.checked ==	true)
	{
		// Launch ActWiz screen	30A
		window.returnValue = SYMALERT_RESULT_ACTIVATE_SERVICE;		
	}
	else if(typeof btnUpsell !=	"undefined"	&& btnUpsell.checked ==	true)
	{
		window.returnValue = SYMALERT_RESULT_LAUNCH_UPSELL_WEBSITE;		
	}
	else if(typeof btnUninstall	!= "undefined" && btnUninstall.checked == true)
	{
		window.returnValue = SYMALERT_RESULT_UNINSTALL;		
	}
	
	OnClose();
}


// ========================================== 
//			Subscription AutoRenew
// ========================================== 

function SubAutoRenewOnLoad()
{
	window.returnValue = SYMALERT_RESULT_CANCEL;		

	if(!CreateAxDefAlert() || !CreateAxNAVOptions())
		return;
		
	try
	{
		if(g_DefAlert.IsHighContrast())	
		{
			// Enable scroll
			document.body.scroll = "yes";					
			
			// Hide	all bitmaps and	icons
			imgIcon.style.display = "none";
			btnOK.className	= "";
		}
		
		var szProductName = g_DefAlert.GetStringFromBrandingResource(IDS_BRANDING_PRODUCT_NAME, false);

		// Display appropriate text if there is	nospyware feature
		if(g_NAVOptions.THREAT_NoThreatCat)
		{
			headline_nospyware.style.display = "";
			headline_spyware.style.display = "none";
		}
	}
	catch(err)
	{
		g_DefAlert.NAVError.LogAndDisplay(0);
	}	
}

// ========================================== 
//		Scanner	Subscription Expired AutoRenew
// ========================================== 

function Scan_SubExpiredAutoRenew()
{
	// Set default users' selections - Assume users	cancel the alert
	window.returnValue = SYMALERT_RESULT_CANCEL;	
	
	// Start the timer to close	the	window in 2	minutes
	iTimerID = setInterval("OnClose()",	TIMEOUT);
	bTimerRunning =	true;
	
	if(!CreateAxDefAlert() || !CreateAxNAVOptions())
		return;
		
	try
	{
		if(g_DefAlert.IsHighContrast())	
		{
			// Enable scroll
			document.body.scroll = "yes";					
			
			// Hide	all	bitmaps	and	icons
			imgIcon.style.display = "none";
			btnOK.className	= "";
		}
		
		var	szProductName =	g_DefAlert.GetStringFromBrandingResource(IDS_BRANDING_PRODUCT_NAME, false); 
				
		if(g_NAVOptions.THREAT_NoThreatCat)
		{
			body_spyware.style.display = "none";
			body_nospyware.style.display = "";
		}									
	}
	catch(err)
	{
		g_DefAlert.NAVError.LogAndDisplay(0);
	}
}

function Scan_SubExpiredAutoRenewOnClickOK()
{
	CancelTimer();
	
	if(btnRenew.checked	== true)
	{
		window.returnValue = SYMALERT_RESULT_RENEWSUBS;			
	}

	OnClose();
}

function SubWarningAutoRenewOnLoad()
{
	window.returnValue = SYMALERT_RESULT_CANCEL;		

	if(!CreateAxDefAlert())
		return;
		
	try
	{
		GetProductAndLicensingData();
		
		if(g_DefAlert.IsHighContrast())	
		{
			// Enable scroll
			document.body.scroll = "yes";					
			
			// Hide	all bitmaps	and icons
			imgIcon.style.display = "none";
			btnOK.className	= "";
		}

		productName.innerText =	g_DefAlert.GetStringFromBrandingResource(IDS_BRANDING_PRODUCT_NAME, false);
	}
	catch(err)
	{
		g_DefAlert.NAVError.LogAndDisplay(0);
	}	
}

//	Gets the Product and Licensing Data
//	Call this function before using	any	of Product / Licensing data
// ====================================================================
function GetProductAndLicensingData()
{
	//Use the CLT Com wrapper to get Product and Subscription information.
	try
	{
		var	dwException	= 0;
		//Product PEP GUID that	is passed in as	an argument
		var	aCLTWrapProperties = null;
		var	CLTWrapper;
		var pepGuid = "{BC8D3EAF-F864-4d4b-AB4D-B3D0C32E2840}";

		CLTWrapper = new ActiveXObject("Symantec.CLTWrap2.CLTDataProvider");
		var	lCookie	= CLTWrapper.InitQueryPolicy(pepGuid, "", 1, POLICY_ID_QUERY_PRODUCT_INFORMATION);
		CLTWrapper.QueryPolicy(lCookie);
		
		
		// =======================================================================
		// Get License and Subscription	info
		// =======================================================================
		
		CLTWrapper.Reset(lCookie);
		lCookie	= CLTWrapper.Initialize(pepGuid, "");
		var	pScript2 = new ActiveXObject("Scripting.Dictionary");
		pScript2.Add("0", "clt::RemainingDays"); //nSubDaysRemaining
		pScript2.Add("1", "clt::LicenseAttribs"); //CanRenewSubscription
		pScript2.Add("2", "clt::LicenseType");
		var	pGetArray2 = pScript2.Items();
		
		// look	up properties
		aCLTWrapProperties = CLTWrapper.GetArrayFromArray(lCookie, pGetArray2);
		
		g_nSubDaysRemaining	= aCLTWrapProperties.getItem(0);
		g_bCanRenewSub = ((aCLTWrapProperties.getItem(1) & cltLicenseAttrib_Subscription_Renewable)	== cltLicenseAttrib_Subscription_Renewable);
		g_LicenseType = aCLTWrapProperties.getItem(2);
		
	
		// =======================================================================
		// Get License type	specific data
		// =======================================================================
		CLTWrapper.Reset(lCookie);
		lCookie	= CLTWrapper.Initialize(pepGuid, "");
		var	pScript3 = new ActiveXObject("Scripting.Dictionary");
		
		//Get SOS data
		if(cltLicenseType_SOS == g_LicenseType)
		{
			pScript3.Add("0", "clt::RemainingDays");
			pScript3.Add("1", "clt::SCSSVendorName");
			pScript3.Add("2", "clt::SCSSCancellationCode");
		}
		//Get non SOS data
		else
		{
			pScript3.Add("0", "clt::RemainingDays");
		}
			
		var	pGetArray3 = pScript3.Items();
		aCLTWrapProperties = CLTWrapper.GetArrayFromArray(lCookie, pGetArray3);
		
		//Set the days remaining
		g_LicenseDaysRemaining = aCLTWrapProperties.getItem(0);
		
		
		//Set the SOS specific data
		if(cltLicenseType_SOS == g_LicenseType)
		{
			g_szVendorName = aCLTWrapProperties.getItem(1);
			g_CancelCode = aCLTWrapProperties.getItem(2);
		}
	
	}
	catch(err)
	{
		NAVError(resErr.oERR_NO_LICENSE);
		window.navigate("res://closeme.xyz");
		return;		
	}
	
	return;		
}
