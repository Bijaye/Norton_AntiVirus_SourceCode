var MainFrame = window.parent;
// var bAgreedToEULA = false;

// Licensing state
var DJSMAR00_LicenseState_TRIAL		= 0;
var DJSMAR00_LicenseState_PURCHASED = 1;
// Force Override
var DJSMAR00_QueryMode_ForceOverride = 7;

function OnLoad()
{
	document.body.tabIndex=-1;
	// Disable the Next button
	// window.parent.frames("bottom").NextButton.disabled = true;
	if (MainFrame.bAgreedToEULA)
	{
		AgreeEULAForm.AgreeEULA.checked = true;
		OnAgreeEULAClick();
	}
	else
	{
		OnDisagreeEULAClick();
	}

	EULAText.innerText = EULAIFrame.document.body.innerText;
    EULAIFrame.focus ();

}

function OnAgreeEULAClick()
{
	// Enable the Next button
	window.parent.frames("bottom").NextButton.disabled = false;
	window.parent.frames("bottom").NextButton.focus();	
	MainFrame.bAgreedToEULA = true;
}

function OnDisagreeEULAClick()
{
	// Disable the Next button
	window.parent.frames("bottom").NextButton.disabled = true;
	MainFrame.bAgreedToEULA = false;
}

function OnUnLoad()
{
	if ( MainFrame.bAgreedToEULA == true )
	{
		try
		{
			// Set EULA flag
			window.parent.frames("contents").CfgWizMgrObj.AgreedToEULA = true;
					
			// Reset trial date on CTO only (expensive call)
			if( MainFrame.g_lOEM == 2 )
			{
				var NAVLicense;
				
				NAVLicense = new ActiveXObject("NAVLicense.NAVLicenseInfo");
				// this license is expried until DJSMAR00_QueryMode_ForceOverride has been called.
    			NAVLicense.GetLicenseStateEx( DJSMAR00_QueryMode_ForceOverride );
    			
    			// refresh the variables with the new license info
    			MainFrame.g_iLicenseState = window.external.ObjectArg("DRM::LicenseState") = NAVLicense.GetLicenseState();
    			MainFrame.g_iLicenseZone = window.external.ObjectArg("DRM::LicenseZone") = NAVLicense.GetLicenseZone();
				MainFrame.g_bLicenseValid = ((MainFrame.g_iLicenseState == DJSMAR00_LicenseState_TRIAL) || (MainFrame.g_iLicenseState == DJSMAR00_LicenseState_PURCHASED));
    			MainFrame.g_iDaysRemaining = window.external.ObjectArg("DRM::RemainingDays") = NAVLicense.LicenseDaysRemaining;
    		}
    		
    		// Enable subscription.
    		window.parent.frames("contents").OnAgreedToEULA();
		}
		catch(err)
		{
			var msg = document.frames("Errors").document.all.ERR_INTERNAL_ERROR.innerText;
			var id = document.frames("Errors").document.all.ERR_ID_NO_LICENSE.innerText;
			g_ErrorHandler.DisplayNAVError (msg, id);
		}
	}
}
