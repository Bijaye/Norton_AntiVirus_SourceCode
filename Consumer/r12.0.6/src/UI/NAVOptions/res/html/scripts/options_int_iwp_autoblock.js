// options_int_iwp_autoblock.js

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

// Global variables
g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 110;  // This uniquely defines this script file to the error handler

var AddressElementName = "/*/Address";
var g_IntervalID = 0;

function OnHelp()
{
    try
    {
        window.external.Global.NAVOptions.Help(250000); // IWP_AUTOBLOCK
    }
    catch(err)
    {
        window.external.Global.NAVOptions.NAVError.LogAndDisplay(window.external.Global.g_HWND);
        return;
    }
}

function OnLoad()
{
    try
    {
		 //Defect 1-4B02QN
         // Enable scrolling for high-contrast mode
	     if( window.external.Global.NAVOptions.IsHighContrastMode )
	        body_int_iwp_autoblock.scroll = "auto";
		// Set title
		document.title = window.external.Global.NAVOptions.ProductName;
		
        with(options_int_iwp_autoblock)
        {
            // Get state of autoblock
            try
            {
                AutoBlock_Enabled.checked = window.external.Global.NAVOptions.AutoBlockEnabled;
            }
            catch(err)
            {
                window.external.Global.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
                parent.location.href = 'closeme.xyz';
                return;
            }
            
            // Setup the list control
            badList.SetTableClass("ListBoxClass");
            badList.OnClick = badList_OnClick;
            
            // Need to continuously check changes so we can update the UI accordingly.
            OnTimerClick();
            g_IntervalID = setInterval("OnTimerClick()", 1000);
            
        } // end with(options_int_iwp_autoblock)
    }
    catch(err)
    {
        g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
										document.frames("Errors").ERROR_IWP_LOAD_AUTOBLOCK_PAGE_ID.innerText);
    }
}

function InitListFromFirewall()
{
    with(options_int_iwp_autoblock)
    {
        var selSave = badList.GetCurSel();

        // Reset the list control
        badList.DeleteAllRows();
        badList.SetColumnCount(1);
        badList.SetColumnWidth(1, "100%");

        try
        {
            // Load the blocked list from the drivers
            var xmlDoc = BlockList.AddressList;
        }
        catch(err)
        {
            g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
										    document.frames("Errors").ERROR_IWP_AUTOBLOCK_GET_ADDRESSLIST_ID.innerText);
            return;
        }

        if (xmlDoc != null)
        {
            var blockList = xmlDoc.selectNodes(AddressElementName);            
            for (iItem = 0; iItem < blockList.length; iItem++)
            {
                try
                {
                    var ip = new ActiveXObject("ccFWSettg.IPItem");
                }catch(err)
                {
                    g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
										            document.frames("Errors").ERROR_IWP_AUTOBLOCK_CREATE_IPITEM_ID.innerText);
                    return;
                }
                ip.RawIP = blockList[iItem].nodeTypedValue;

                // Create the  list control row
                pRow = badList.InsertRow(-1);
                badList.SetCellTextPtr(pRow, 0, ip.GetAsString());
            }
        }
        else
        {
            g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
										    document.frames("Errors").ERROR_IWP_AUTOBLOCK_XMLDOCISNULL_ID.innerText);
        }

        // restore selected row if any
        if (selSave >= badList.GetRowCount())
            selSave = -1;
        badList.SetCurSel(selSave);

        // The Unblock and Unblock All buttons depend upon the contents of the list
        // so update them when the contents of the list change
        UpdateUnblockDisabledState();
        UpdateUnblockAllDisabledState();
    } // end with(options_int_iwp_autoblock)
}

function UpdateUnblockAllDisabledState()
{
    with(options_int_iwp_autoblock)
    {
        UnblockAll.disabled = (badList.GetRowCount() == 0);
    }
}

function UpdateUnblockDisabledState()
{
    with(options_int_iwp_autoblock)
    {
        Unblock.disabled = (badList.GetCurSel() < 0);
    }
}

function badList_OnClick()
{
    // The Unblock button depends upon the selection state of this list
    // so update them when the selection changes
    UpdateUnblockDisabledState();
}

function Unblock_OnClick()
{
    with(options_int_iwp_autoblock)
    {
        var nSel = badList.GetCurSel();
        if (nSel >= 0)
        {
            try
            {
                var ip = badList.GetCellText(nSel, 0);
                BlockList.ClearAddress(ip);
            }
            catch(err)
            {
                g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
										        document.frames("Errors").ERROR_IWP_AUTOBLOCK_CLEARADDRESS_ID.innerText);
            }
        }
    }
}

function UnblockAll_OnClick()
{
    with(options_int_iwp_autoblock)
    {
        try
        {
            BlockList.ClearAll();
        }
        catch(err)
        {
            g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR.innerText,
										    document.frames("Errors").ERROR_IWP_AUTOBLOCK_CLEARADDRESSALL_ID.innerText);
        }
    }
}

function OnClose()
{
    with(options_int_iwp_autoblock)
    {
        try
        {
            // Set state of autoblock
            window.external.Global.NAVOptions.AutoBlockEnabled = Math.abs(AutoBlock_Enabled.checked);
        }
        catch(err)
        {
            window.external.Global.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }
        
        parent.location.href = 'closeme.xyz';
    }
}

function OnTimerClick()
{
    InitListFromFirewall();
}

function OnUnload()
{
    if ( g_IntervalID != 0 )
    {
        clearInterval(g_IntervalID);
        g_IntervalID = 0;
    }
}
