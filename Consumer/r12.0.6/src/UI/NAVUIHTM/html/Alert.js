// Alert.js
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


//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~
// HTML pages that include Alert.js
//  must also include SymAlertStatic.js
//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

//
// Help IDS
//
var IDH_NAV_ALERT_WARNING_SUBSCRIPT = 11561;
var IDH_NAV_ALERT_EXPIRED_SUBSCRIPT = 11560;
var IDH_NAV_ALERT_EXPIRED_SUBSCRIPT_ONSCAN = 12100;
var IDH_NAV_ALERT_EXPIRED_VIRUSDEFS = 12150;
var IDH_NAV_ALERT_EXPIRED_VIRUSDEFS_ONSCAN = 12151;
var IDH_NAV_ALERT_ALU_DISABLED = 12152;

var g_DefAlert = null;
var g_Launcher = null;
var g_NavStatus = null;
var g_NavLicense = null;
var g_NAVOptions = null;

g_ModuleID = 3031;
g_ScriptID = 100;  // This uniquely defines this script file to the error handler

var CYCLE_SUBSEXPIRED	= "SECycle";
var CYCLE_DEFSOUTOFDATE = "DefsCycle";	
var CYCLE_SCANSUBSEXPIRED	= "S_SECyc";
var CYCLE_SCANDEFSOUTOFDATE = "S_DCyc";

// NAV status returned by ActiveX
var STATUS_ENABLED = 3;
var STATUS_DISABLED	= 4;    

var _1_DAY = 86400; // in seconds

var iTimerID;
var bTimerRunning = false;
// 2 minute timeout
var TIMEOUT = 120000;

var CANCELCODE_NOT_VENDORS_MEMBER 		= 100;
var CANCELCODE_VENDOR_CANCELLED_SERVICE = 101;
var CANCELCODE_USER_CANCELLED_SERVICE 	= 102;
var CANCELCODE_SERVICE_ON_HOLD 			= 103;
var CANCELCODE_SERVICE_TRANSFERRED 		= 99;
var CANCELCODE_SERVICE_DISCONTINUED		= 105;

// Vendor resource id in NAVUIRES.
var IDS_AV_PRODUCT_NAME = 5000;
var IDS_AV_PRODUCT_NAME_VERSION = 5001;

// ========================================== 
//			Helper functions
// ========================================== 

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
		return false;
	}
	
	return true;
}

function CreateAxAppLauncher()
{	
	try
	{
		if(g_Launcher == null)
		{
			g_Launcher = new ActiveXObject ("Symantec.Norton.AntiVirus.AppLauncher");
		}
	}
	catch(err)
	{
		NAVError(resErr.oERR_NO_LAUNCHER);
		return false;
	}
	
	return true;
}
		
function CreateAxNavStatus()
{
	try
	{
		if(g_NavStatus == null)
		{
			g_NavStatus = new ActiveXObject("Status.NAVStatus");
		}
	}
	catch(err)
	{
		NAVError(resErr.oERR_NO_STATUS);
		return false;
	}		
	
	return true;
}

function CreateAxNavLicense()
{
	try
	{
		if(g_NavLicense == null)
		{
			g_NavLicense = new ActiveXObject("NAVLicense.NAVLicenseInfo");
		}
	}
	catch(err)
	{
		NAVError(resErr.oERR_NO_LICENSE);
		return false;
	}
	
	return true;
}

function CreateAxNAVOptions()
{	
	try
	{
		if(g_NAVOptions == null)
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
	days.disabled = true;
}

function EnableNotifyOption()
{
	days.disabled = false;
}

function EnableSSCCheckBox()
{
	if(days.value == "14")
	{
		btnSSC.disabled = false;
		btnSSC.disabled = false;
	}
	else
	{
		btnSSC.disabled = true;
		btnSSC.disabled = true;	
		btnSSC.checked = true;		
	}	
}

function DisableNotifyAndDontShowOptions()
{
	days.disabled = true;
	btnDontShow.disabled = true;
}

function EnableNotifyAndDontShowOptions()
{
	days.disabled = false;
	btnDontShow.disabled = false;
}

function OnHelpClick(TopicID)
{
	if(CreateAxAppLauncher())
	{
		try
		{	
			g_Launcher.LaunchHelp(TopicID);
		}
		catch(err)
		{
			g_Launcher.NAVError.LogAndDisplay(0);
		}
	}

	return false;
}

function OnClose()
{
    CancelTimer();
	location.href = "res://closeme.xyz/";
}

function AlertOnLoad()
{
	window.returnValue = SYMALERT_RESULT_CANCEL;		
	
	if(CreateAxDefAlert())
	{		
		try
		{
			if(g_DefAlert.IsHighContrast()) 
			{
				// Enable scroll
				document.body.scroll = "yes";					
				
				// Hide all bitmaps and icons
				tableTop.style.backgroundImage = "";
				imgAlertIcon.style.display = "none";
				btnOK.className = "";
			}
			
			// Set title and product name.		
			var szProductName = g_DefAlert.GetStringFromBrandingResource(IDS_AV_PRODUCT_NAME, false);
			document.title = szProductName;
			productName.innerText = szProductName;
		}
		catch(err)
		{
			g_DefAlert.NAVError.LogAndDisplay(0);
		}
	}
}

function CancelTimer()
{
	// If the timer is running
	if (bTimerRunning == true)
	{
		// cancel the timer
		clearInterval(iTimerID);
		bTimerRunning = false;
	}
}

// ========================================== 
//			ALUDisabled functions 
// ========================================== 

function ALUDisabledOnClickOK()
{
	if(btnEnableALU.checked == true)
		window.returnValue = SYMALERT_RESULT_ENABLEALU;		
		
	OnClose();
}

// ========================================== 
//			DefsOutOfDates functions 
// ========================================== 

function DefsOutOfDateOnLoad()
{	
	window.returnValue = SYMALERT_RESULT_CANCEL;		
	
	if(CreateAxDefAlert())
	{		
		try
		{
			if(g_DefAlert.IsHighContrast()) 
			{
				// Enable scroll
				document.body.scroll = "yes";					
				
				// Hide all bitmaps and icons
				tableTop.style.backgroundImage = "";
				imgAlertIcon.style.display = "none";
				btnOK.className = "";
			}
			
			if(g_DefAlert.ShowSSC)
			{
				// Show the checkbox..but disable it by default..
				btnSSC.style.display = "";	
				lblSSC.style.display = "";
				btnSSC.disabled = true;
				btnSSC.disabled = true;			
				
			}
				
			document.title = g_DefAlert.GetStringFromBrandingResource(IDS_AV_PRODUCT_NAME, false);
			
			g_DefAlert.SetAlertDword(CYCLE_DEFSOUTOFDATE, _1_DAY);		
		}
		catch(err)
		{
			g_DefAlert.NAVError.LogAndDisplay(0);
		}
	}
	
	if(CreateAxNavStatus())
	{
		try
		{
			g_NavStatus.UseALU = 1;
			g_NavStatus.GetStatus(1);
									
			if(g_NavStatus.ALUStatus == STATUS_DISABLED) 
			{
				optEnableALU.style.display = "";
			}
		}
		catch(err)
		{
			g_NavStatus.NAVError.LogAndDisplay(0);
		}		
	}		
}

function DefsOutOfDateOnClickOK()
{
	// Update the DefsOutOfDate cycle anyway in case the user
	//  selects LU but then cancels it later.

	try
	{
		g_DefAlert.SetAlertDword(CYCLE_DEFSOUTOFDATE, (days.value * _1_DAY));			
	}
	catch(err)
	{
		g_DefAlert.NAVError.LogAndDisplay(0);
	}

	if(btnLU.checked == true)
	{	
		if(btnEnableALU.checked == true)
		{			
			window.returnValue = SYMALERT_RESULT_LAUNCHLU_AND_ENABLE_ALU;
		}
		else
		{
			window.returnValue = SYMALERT_RESULT_LAUNCHLU;
		}
	}
	else if(btnEnableALU.checked)
	{
		window.returnValue = SYMALERT_RESULT_ENABLEALU;		
	}
		
	// If check box is shown and enabled write to WMI
	if(g_DefAlert.ShowSSC && (false == btnSSC.disabled))
		g_DefAlert.WriteToWMI(true == btnSSC.checked);

	OnClose();
}

// ========================================== 
//	   Scanner DefsOutOfDates functions 
// ========================================== 

function Scan_DefsOutOfDateOnLoad()
{
	window.returnValue = SYMALERT_RESULT_CANCEL;	
	
    // Start the timer to close the window in 2 minutes
    iTimerID = setInterval("OnClose()", TIMEOUT);
	bTimerRunning = true;
	
	if(CreateAxDefAlert()
		&& CreateAxNAVOptions())
	{		
		try
		{
			if(g_DefAlert.IsHighContrast()) 
			{
				// Enable scroll
				document.body.scroll = "yes";					
				
				// Hide all bitmaps and icons
				tableTop.style.backgroundImage = "";
				imgAlertIcon.style.display = "none";
				btnOK.className = "";
			}
				
			if(g_DefAlert.ShowSSC)
			{
				// Show the checkbox..but disable it by default..
				optSSE.style.display = "";				
				btnSSC.style.display = "";	
				lblSSC.style.display = "";
				btnSSC.disabled = true;
				btnSSC.disabled = true;			
				
			}
			
			document.title = g_DefAlert.GetStringFromBrandingResource(IDS_AV_PRODUCT_NAME, false);
			
			g_DefAlert.SetAlertDword(CYCLE_SCANDEFSOUTOFDATE, 0);		
		}
		catch(err)
		{
			g_DefAlert.NAVError.LogAndDisplay(0);
		}
	}		
	
	if(CreateAxNavStatus())
	{
		try
		{
			g_NavStatus.UseALU = 1;
			g_NavStatus.GetStatus(1);
									
			if(g_NavStatus.ALUStatus == STATUS_DISABLED) 
			{
				optEnableALU.style.display = "";
			}
		}
		catch(err)
		{
			g_NavStatus.NAVError.LogAndDisplay(0);
		}		
	}		
}

function Scan_DefsOutOfDateOnClickOK()
{
    CancelTimer();
	
	try
	{
		if(btnLU.checked == true)
		{	
			if(btnEnableALU.checked == true)
			{			
				window.returnValue = SYMALERT_RESULT_LAUNCHLU_AND_ENABLE_ALU;
			}
			else
			{
				window.returnValue = SYMALERT_RESULT_LAUNCHLU;
			}
		}
		else if(btnEnableALU.checked)
		{
			window.returnValue = SYMALERT_RESULT_ENABLEALU;		
		}
		
		if(btnLU.checked == false)
		{
			g_DefAlert.SetAlertDword(CYCLE_SCANDEFSOUTOFDATE, (days.value * _1_DAY));			
		}		
	}
	catch(err)
	{
		g_DefAlert.NAVError.LogAndDisplay(0);
	}
	
	// If check box is shown and enabled write to WMI
	if(g_DefAlert.ShowSSC && (false == btnSSC.disabled))
		g_DefAlert.WriteToWMI(true == btnSSC.checked);
	
	OnClose();
}

// ========================================== 
//			Subscription Warning
// ========================================== 

function SubWarningOnLoad()
{
	window.returnValue = SYMALERT_RESULT_CANCEL;		

	if(CreateAxDefAlert()
		&& CreateAxNavLicense()
		&& CreateAxNAVOptions())
	{		
		try
		{
			if(g_DefAlert.IsHighContrast()) 
			{
				// Enable scroll
				document.body.scroll = "yes";					
				
				// Hide all bitmaps and icons
				tableTop.style.backgroundImage = "";
				imgAlertIcon.style.display = "none";
				btnOK.className = "";
			}
				
			document.title = g_DefAlert.GetStringFromBrandingResource(IDS_AV_PRODUCT_NAME, false);
			sProductNameVersion.innerText = g_DefAlert.GetStringFromBrandingResource(IDS_AV_PRODUCT_NAME_VERSION, false);
			
			if(g_NAVOptions.THREAT.NoThreatCat)
			{
				details2_nospyware.style.display = "";
				details2_spyware.style.display = "none";
			}			
		}
		catch(err)
		{
			g_DefAlert.NAVError.LogAndDisplay(0);
		}	

		try
		{				
			// Set days left and renewal date.		
			daysLeft.innerText = g_NavLicense.SubscriptionRemaining;
		}
		catch(err)
		{
			g_NavLicense.NAVError.LogAndDisplay(0);
		}	
	
		try
		{
			// Check for Admin right.			
			if(g_NavLicense.CanRenewSubscription == false)
			{
				// Display text "Admin rights required!"
				table2_row_RenewNow.style.display = "none";	
				table2_row_RenewLater.style.display = "none";
				btnRenew.checked = false;			
				span_AdminRights.style.display = "";	
			}
		}
		catch(err)
		{
			g_NavLicense.NAVError.LogAndDisplay(0);
		}
	}	
}

function SubWarningOnClickOK()
{
	if(btnRenew.checked == true)
		window.returnValue = SYMALERT_RESULT_RENEWSUBS;		

	OnClose();
}

// ========================================== 
//			Subscription Expired
// ========================================== 

function SubExpiredOnLoad()
{
	// Assume users cancel the alert
	window.returnValue = SYMALERT_RESULT_CANCEL;		

	if(CreateAxDefAlert()
		&& CreateAxNavLicense()
		&& CreateAxNAVOptions())
	{		
		try
		{
			if(g_DefAlert.IsHighContrast()) 
			{
				// Enable scroll
				document.body.scroll = "yes";					
				
				// Hide all bitmaps and icons
				tableTop.style.backgroundImage = "";
				imgAlertIcon.style.display = "none";
				btnOK.className = "";
			}

			// Set product name and title.				
			document.title = g_DefAlert.GetStringFromBrandingResource(IDS_AV_PRODUCT_NAME, false);
			sProductNameVersion.innerText = g_DefAlert.GetStringFromBrandingResource(IDS_AV_PRODUCT_NAME_VERSION, false);
			
			// Display appropriate text if there is nospyware feature
			if(g_NAVOptions.THREAT.NoThreatCat)
			{
				headline_nospyware.style.display = "";
				headline_spyware.style.display = "none";
				body_nospyware.style.display = "";
				body_spyware.style.display = "none";
			}
			
			// Assume they selected "Notify me in 1 day".	
			g_DefAlert.SetAlertDword(CYCLE_SUBSEXPIRED, _1_DAY);		

			var nDaysLeft = 0;
			nDaysLeft = g_DefAlert.GetVirusDefSubDaysLeft();
			
			if(nDaysLeft < 0)
			{
				var nDaysExpired = Math.abs(nDaysLeft);
				if(nDaysExpired >= 30)
				{
					lblDontShow.style.display = "";
					btnDontShow.style.display = "";
					btnDontShow.disabled = true;
					days15.selected = true;
					days1.selected = false;
				}		
			}
		}
		catch(err)
		{
			g_DefAlert.NAVError.LogAndDisplay(0);
		}	
		
		try
		{	
			if(g_NavLicense.CanRenewSubscription == false)
			{
				table2_row_RenewNow.style.display = "none";	
				table2_row_RenewLater.style.display = "none";
				table2_row_DontShowAgain.style.display = "none";
				btnRenew.checked = false;			
				span_AdminRights.style.display = "";	
			}
		}
		catch(err)
		{
			g_NavLicense.NAVError.LogAndDisplay(0);
		}
	}					
}

function SubExpiredOnClickOK()
{
	if(btnRenew.checked == true)
	{
		window.returnValue = SYMALERT_RESULT_RENEWSUBS;			
	}
	else
	{
		try
		{
			if(btnDontShow.checked == true)
			{
				g_DefAlert.SetAlertDword(CYCLE_SUBSEXPIRED, -1);		
			}
			else
			{
				g_DefAlert.SetAlertDword(CYCLE_SUBSEXPIRED, days.value * _1_DAY);		
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
	// Set default users' selections - Assume users cancel the alert
	window.returnValue = SYMALERT_RESULT_CANCEL;	
	
	// Start the timer to close the window in 2 minutes
	iTimerID = setInterval("OnClose()", TIMEOUT);
	bTimerRunning = true;
	
	if(CreateAxDefAlert()
		&& CreateAxNavLicense()
		&& CreateAxNAVOptions())
	{		
		try
		{
			document.title = g_DefAlert.GetStringFromBrandingResource(IDS_AV_PRODUCT_NAME, false);
			sProductNameVersion.innerText = g_DefAlert.GetStringFromBrandingResource(IDS_AV_PRODUCT_NAME_VERSION, false);
			sProductNameVersion_nospware.innerText = sProductNameVersion.innerText;
			
			if(g_DefAlert.IsHighContrast()) 
			{
				// Enable scroll
				document.body.scroll = "yes";					
				
				// Hide all bitmaps and icons
				tableTop.style.backgroundImage = "";
				imgAlertIcon.style.display = "none";
				btnOK.className = "";
			}
						
			// Display appropriate text if there is nospyware feature
			if(g_NAVOptions.THREAT.NoThreatCat)
			{
				headline_nospyware.style.display = "";
				headline_spyware.style.display = "none";
				body_nospyware.style.display = "";
				body_spyware.style.display = "none";
				body2_nospyware.style.display = "";
				body2_spyware.style.display = "none";
			}
			
			// Set display cycle to 0 days so it appears again in case the user dismiss alert.
			g_DefAlert.SetAlertDword(CYCLE_SCANSUBSEXPIRED, 0);	
			
			// If subscriptions has expired for 30 days,
			// display option "Don't show this again".
			var nDaysLeft = g_DefAlert.GetVirusDefSubDaysLeft();
			if(nDaysLeft < 0)
			{
				var nDaysExpired = Math.abs(nDaysLeft);
				if(nDaysExpired >= 30)
				{
					lblDontShow.style.display = "";
					btnDontShow.style.display = "";
					btnDontShow.disabled = true;
					days15.selected = true;
					days1.selected = false;
				}				
			}
		}
		catch(err)
		{
			g_DefAlert.NAVError.LogAndDisplay(0);
			return;
		}
		
		try
		{
			// Check for admin rights.
			if(g_NavLicense.CanRenewSubscription == false)
			{
				table2_row_RenewNow.style.display = "none";	
				table2_row_RenewLater.style.display = "none";
				table2_row_DontShowAgain.style.display = "none";
				btnRenew.checked = false;		
				span_AdminRights.style.display = "";	
			}
		}
		catch(err)
		{
			g_NavLicense.NAVError.LogAndDisplay(0);
		}
	}
}

function Scan_SubExpiredOnClickOK()
{
    CancelTimer();
	
	if(btnRenew.checked == true)
	{
		window.returnValue = SYMALERT_RESULT_RENEWSUBS;			
	}
	else
	{
		try
		{
			if(btnDontShow.checked == true)
			{
				g_DefAlert.SetAlertDword(CYCLE_SCANSUBSEXPIRED, -1);		
			}
			else
			{
				g_DefAlert.SetAlertDword(CYCLE_SCANSUBSEXPIRED, days.value * _1_DAY);		
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
// PostActivation Expired and Warning Alerts
// 

function LicensingOnClickOK()
{
	if(btnReactivate.checked == true)
	{
		// Launch ActWiz screen 30B
		window.returnValue = SYMALERT_RESULT_CONFIRM_SERVICE;		
	}
	
	OnClose();
}

// 
// Preactivation Expired Alert
// 
function LicensingPreActiveExpiredOnClickOK()
{
	if(btnReactivate.checked == true)
	{
		// Launch ActWiz screen 29C or 30A
		//  depends on startup mode
		window.returnValue = SYMALERT_RESULT_SIGNUP_SERVICE;		
	}
	
	OnClose();
}

// 
// PostActivation Expired Killed Alert
// 
function LicensingCancelledOnLoad()
{
	window.returnValue = SYMALERT_RESULT_CANCEL;		

	if(CreateAxDefAlert()
		&& CreateAxNavLicense())
	{		
		var szVendorName;
		var szProductName;
		
		try
		{		
			// Set title and product name.
			szProductName = g_DefAlert.GetStringFromBrandingResource(IDS_AV_PRODUCT_NAME, false);
			szVendorName = g_NavLicense.VendorName;
			
			document.title = szProductName;
			rsnProductName.innerText = szProductName;
			optUninstallProductName.innerText = szProductName;
			
			// System in high contrast mode, 
			if(g_DefAlert.IsHighContrast()) 
			{
				// Enable scroll.
				document.body.scroll = "yes";			
							
				// Hide all bitmaps and icons.
				tableTop.style.backgroundImage = "";
				imgAlertIcon.style.display = "none";
				btnCancel.className = "";
				btnOK.className = "";
			}
		}
		catch(err)
		{
			g_DefAlert.NAVError.LogAndDisplay(0);
		}	
					
		try
		{			
			switch(g_NavLicense.CancelCode)
			{
			// 100
			case CANCELCODE_NOT_VENDORS_MEMBER:	
				rsnNotAMember.style.display = "";
				optUpsell.style.display = "";		
				optUninstall.style.display = "";
				btnUpsell.checked = true;
				sReactivate.innerHTML = "";		
				sContactVendor.innerHTML = "";
			break;
				
			// 101
			case CANCELCODE_VENDOR_CANCELLED_SERVICE:
				rsnVendorCancelledService.style.display = "";
				rsnVendorCancelledService_VendorName.innerText = szVendorName;
				optContactVendor.style.display = "";
				optContactVendor_VendorName.innerText = szVendorName;
				optContactVendor_VendorName.style.display = "";
				optReactivate.style.display = "";			
				optReactivateOnResolved.style.display = "";
				optUpsell.style.display = "";
				optUninstall.style.display = "";		
				btnUpsell.checked = true;
			break;
			
			// 102
			case CANCELCODE_USER_CANCELLED_SERVICE:
				rsnUserCancelledService.style.display = "";
				optReactivate.style.display = "";
				optReactivateOnResubscribe.style.display = "";
				optUpsell.style.display = "";
				optUninstall.style.display = "";
				btnUpsell.checked = true;
				sContactVendor.innerHTML = "";
			break;
			
			// 99
			case CANCELCODE_SERVICE_TRANSFERRED:
				rsnServiceTransferred.style.display = "";				
				optReactivate.style.display = "";
				optReactivateOnTransfer.style.display="";
				optUninstall.style.display = "";
				btnReactivate.checked = true;
				sUpsell.innerHTML = "";
				sContactVendor.innerHTML = "";
			break;			
			
			// 105
			case CANCELCODE_SERVICE_DISCONTINUED:	
				rsnServiceDiscontinued.style.display = "";
				rsnServiceDiscontinued_VendorName.innerText = szVendorName;			
				rsnServiceDiscontinued_VendorName.style.display = "";
				optUpsell.style.display = "";
				optUninstall.style.display = "";
				btnUpsell.checked = true;
				sReactivate.innerHTML = "";
				sContactVendor.innerHTML = "";
			break;			
			
			// 103 and Unknown
			case CANCELCODE_SERVICE_ON_HOLD:
			default:
				rsnServiceOnHold.style.display = "";				
				rsnServiceOnHold_VendorName.innerText = szVendorName;				
				optContactVendor.style.display = "";			
				optContactVendor_VendorName.innerText = szVendorName;
				optContactVendor_VendorName.style.display = "";
				optReactivate.style.display = "";
				optReactivateOnResolved.style.display = "";
				optUpsell.style.display = "";
				optUninstall.style.display = "";
				btnUpsell.checked = true;
			break;
			}		
			
		}
		catch(err)
		{
			g_NavLicense.NAVError.LogAndDisplay(0);
		}
	}
}

function LicensingCancelledOnClickOK()
{
	// Must check to see if a particular button is displayed before
	//  checking to see if user selected it; otherwise, an exception
	//  will be thrown.
	if(typeof btnContactVendor != "undefined" && btnContactVendor.checked == true)
	{
		// Launch ReEnableURL
		window.returnValue = SYMALERT_RESULT_LAUNCH_SERVICE_REACTIVATION_WEBSITE;		
	}
	else if(typeof btnReactivate != "undefined" && btnReactivate.checked == true)
	{
		// Launch ActWiz screen 30A
		window.returnValue = SYMALERT_RESULT_ACTIVATE_SERVICE;		
	}
	else if(typeof btnUpsell != "undefined" && btnUpsell.checked == true)
	{
		window.returnValue = SYMALERT_RESULT_LAUNCH_UPSELL_WEBSITE;		
	}
	else if(typeof btnUninstall != "undefined" && btnUninstall.checked == true)
	{
		window.returnValue = SYMALERT_RESULT_UNINSTALL;		
	}
	
	OnClose();
}

function AlertRentalPreactiveGraceOnLoad()
{
	AlertOnLoad();

	if(CreateAxNavLicense())
	{
		var nDaysRemaining = g_NavLicense.LicenseDaysRemaining;
		
		span_daysRemaining.innerText = nDaysRemaining;
		
		// Display singular or plural version of "days"
		if(nDaysRemaining == 1)
		{
			span_days.style.display = "none";
			span_day.style.display = "";
		}
	}
}


// ========================================== 
//			Subscription AutoRenew
// ========================================== 

function SubAutoRenewOnLoad()
{
	window.returnValue = SYMALERT_RESULT_CANCEL;		

	if(CreateAxDefAlert())
	{		
		try
		{
			if(g_DefAlert.IsHighContrast()) 
			{
				// Enable scroll
				document.body.scroll = "yes";					
				
				// Hide all bitmaps and icons
				tableTop.style.backgroundImage = "";
				imgAlertIcon.style.display = "none";
				btnOK.className = "";
			}
				
			var szProductName = g_DefAlert.GetStringFromBrandingResource(IDS_AV_PRODUCT_NAME, false);
			document.title = szProductName;
			productName.innerText = szProductName;			
		}
		catch(err)
		{
			g_DefAlert.NAVError.LogAndDisplay(0);
		}	
	}	
}

// ========================================== 
//		Scanner	Subscription Expired AutoRenew
// ========================================== 

function Scan_SubExpiredAutoRenew()
{
	// Set default users' selections - Assume users cancel the alert
	window.returnValue = SYMALERT_RESULT_CANCEL;	
	
	// Start the timer to close the window in 2 minutes
	iTimerID = setInterval("OnClose()", TIMEOUT);
	bTimerRunning = true;
	
	if(CreateAxDefAlert()
		&& CreateAxNAVOptions())
	{		
		try
		{
			if(g_DefAlert.IsHighContrast()) 
			{
				// Enable scroll
				document.body.scroll = "yes";					
				
				// Hide all bitmaps and icons
				tableTop.style.backgroundImage = "";
				imgAlertIcon.style.display = "none";
				btnOK.className = "";
			}
			
			var szProductName = g_DefAlert.GetStringFromBrandingResource(IDS_AV_PRODUCT_NAME, false); 
			document.title = szProductName;
			productName_spyware.innerText = szProductName;
			
			if(g_NAVOptions.THREAT.NoThreatCat)
			{
				body_spyware.style.display = "none";
				body_nospyware.style.display = "";
				productName_nospyware.innerText = szProductName;				
			}									
		}
		catch(err)
		{
			g_DefAlert.NAVError.LogAndDisplay(0);
			return;
		}
	}
}

function Scan_SubExpiredAutoRenewOnClickOK()
{
    CancelTimer();
	
	if(btnRenew.checked == true)
	{
		window.returnValue = SYMALERT_RESULT_RENEWSUBS;			
	}

	OnClose();
}
