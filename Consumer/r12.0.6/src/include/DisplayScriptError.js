// Script for displaying error messages.
//

window.onerror = GlobalErrorHandler;

var E_FAIL = -214746259;
var E_ACCESSDENIED = -2147024891;
var g_ModuleID = 0; // Clients *must* set this
var g_ScriptID = 0; // Cleints *must* set this too
var m_NAVException = new Object;    // Is this new error from an exception?
m_NAVException.caught = false;

function GlobalErrorHandler (msg, file_loc, line_no)
{
    if ( m_NAVException.caught == false )
    {
        m_NAVException.Message = msg;
    }
    
    // Give a unique error ID for this script file and line number
    var strID = g_ScriptID.toString() + line_no.toString();
    m_NAVException.ErrorID = parseInt(strID);

    DisplayNAVError ( m_NAVException.Message + " : " + unescape(file_loc) + " : " + line_no, m_NAVException.ErrorID );
    m_NAVException.Message = 0;
    m_NAVException.ErrorID = 0;
    m_NAVException.caught = false;

	return true;    // so the basic one isn't displayed
}

function DisplayException ( exception )
{
    m_NAVException.caught = true;
    m_NAVException.Message = exception.name;
    if( exception.description )
        m_NAVException.Message = m_NAVException.Message + " : " + exception.description;
    
    // throw this up to the global handler. This is so we can get the URL and line number
    throw exception;
}

function DisplayNAVError ( Message, ErrorID )
{
	var errMessage = Message;
	
	try
	{	
		// Format message with product name		
		var axLicense = new ActiveXObject("NAVLicense.NAVLicenseInfo");
		errMessage = Message.replace(/%s/g, axLicense.ProductName);
	}
	catch(err)
	{
		errMessage = Message.replace(/%s/g, "Norton AntiVirus");
	}
	
    try 
    {
		var NAVError = new ActiveXObject ("NAVError.NAVCOMError"); 
        NAVError.ModuleID = g_ModuleID;
        NAVError.ErrorID = ErrorID;
        NAVError.Message = errMessage;
        NAVError.LogAndDisplay (0);
    }
    catch (exception)
    {
        alert ( "Norton AntiVirus : "+g_ModuleID+""+ErrorID+" : "+ Message );
    }
}

var g_ErrorHandler = new Object;
g_ErrorHandler.DisplayNAVError = DisplayNAVError;
g_ErrorHandler.DisplayException = DisplayException;
