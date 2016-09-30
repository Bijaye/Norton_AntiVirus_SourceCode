////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// options_othr_excl_main.js

g_ModuleID = 3023; // From ccModuleID.h
g_ScriptID = 128;  // This uniquely defines this script file to the error handler

// Disable Drag-and-drop support
document.ondragstart = function(){return false;}

var g_ThreatsByVID = null;

// Enable / Disable the buttons
function enable_anomaly_buttons()
{
   try
   {
     if (document.options_adv_exclusions_anomaly.AnomalyExclusions.options.selectedIndex < 0)
     {
        ButtonDisable(document.options_adv_exclusions_anomaly.Anomaly_Remove);
        ButtonDisable(document.options_adv_exclusions_anomaly.Anomaly_Details);
     }
     else
     {
        ButtonNormalize(document.options_adv_exclusions_anomaly.Anomaly_Remove);
        ButtonNormalize(document.options_adv_exclusions_anomaly.Anomaly_Details);
     }
   }
   catch (err)
   {
     g_ErrorHandler.DisplayException (err);
     return;
   }
}

// Remove an exclusion
function remove_anomaly_exclusion()
{
  try
  {
    with( document.options_adv_exclusions_anomaly )
    {
        // Remove all selected items from the anomaly exclusions
        var removeItems = new Array();
        for( i=0; i<AnomalyExclusions.options.length; i++ )
        {
            if( AnomalyExclusions.options[i].selected == true )
            {
                try
                {
                    parent.NAVOptions.RemoveAnomalyExclusionItem(i);
                    AnomalyExclusions.options[i] = null;
                    i--;
                }
                catch(err)
                {
                    parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
                    return;
                }
            }
        }
        
        // Re-set focus to the list
        AnomalyExclusions.focus();
      
        enable_anomaly_buttons();
        
     } // with( document.options_adv_exclusions_anomaly )
  }
  catch (err)
  {
    g_ErrorHandler.DisplayException (err);
    return;
  }
}

function anomaly_initialize2()
{
    try
    {
    
    // re-Initialize the list (after defaults are restored)
    document.options_adv_exclusions_anomaly.AnomalyExclusions.options.length = 0;

    // Set these HTML controls from the NAVOptions object
    try
    {
        var count = parent.NAVOptions.AnomalyExclusionCount;
    }
    catch(NAVErr)
    {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
    }

    if( g_ThreatsByVID == null )
    {
        try
        {
            // Get the object and assing the INAVComError object to it
            g_ThreatsByVID = parent.NAVOptions.THREATSBYVID;
            //initialize g_ThreatsByVID
            var threatCount = g_ThreatsByVID.count;
        }
        catch(NAVerr)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }
    }


    for (var i = 0; i < count; i++)
    {
        // Get the current item path and vid
        try
        {
            var itemName = parent.NAVOptions.AnomalyExclusionItemName(i);
            var itemVID = parent.NAVOptions.AnomalyExclusionItemVid(i);
        }
        catch(NAVErr)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }

        document.options_adv_exclusions_anomaly.AnomalyExclusions.options.length++;
        document.options_adv_exclusions_anomaly.AnomalyExclusions.options[i].text = itemName;
        document.options_adv_exclusions_anomaly.AnomalyExclusions.options[i].value = itemVID;
    }
        
    }
    catch(err)
    {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
    }

  document.all.Loading.style.display = 'none';
  document.all.WhatToExclude.style.display = 'block';
  
  // Set focus to first element
  document.options_adv_exclusions_anomaly.AnomalyExclusions.focus();

  enable_anomaly_buttons();

}

function anomaly_initialize()
{
  // Show the 'loading' part.
  document.all.WhatToExclude.style.display = 'none';
  document.all.Loading.style.display = 'block';
  setTimeout('anomaly_initialize2();', 10); // force render
}

function set_anomaly_defaults()
{
    try
    {
        parent.NAVOptions.SetAnomalyExclusionDefaults();
        this.Initialize();
    }
    catch(err)
    {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
    }
}

function add_update_anomaly_exclusion()
{
  with( document.options_adv_exclusions_anomaly )
  {

  try
  {
    window.external.Global = parent;
    
    try
    {
        var sww = new ActiveXObject('CcWebWnd.ccWebWindow');
    }
    catch (NAVErr)
    {
        window.external.Global = null;
        parent.g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR_REINSTALL.innerText,  
                                        document.frames("Errors").ERROR_CREATING_NAVWEBWINDOW_OBJECT_ID.innerText);
        return;
    }

    var NAVPath;

    try
    {
        NAVPath = parent.NAVOptions.NortonAntiVirusPath;
    }
    catch(err)
    {
        delete sww;
        window.external.Global = null;
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        return;
    }

    sww.SetIcon2('res://' + NAVPath + '\\Navopts.dll/201');
    var ItemsAdded = sww.showModalDialog('res://' + NAVPath + '\\navopts.loc/options_exclude_byvid_dlg.htm', 400, 400, null);
    delete sww;
    window.external.Global = null;

    // We can no longer rely on the array return value - as of IE7 beta 3, that functionality is broken
    // or removed.
    
    try
    {
        // Set these HTML controls from the NAVOptions object
        try
        {
            var count = parent.NAVOptions.AnomalyExclusionCount;
        }
        catch(NAVErr)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }

        for (var i = 0; i < count; i++)
        {
            // Get the current item path and vid
            try
            {
                var itemName = parent.NAVOptions.AnomalyExclusionItemName(i);
                var itemVID = parent.NAVOptions.AnomalyExclusionItemVid(i);
            }
            catch(NAVErr)
            {
                parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
            }

            if(i >= document.options_adv_exclusions_anomaly.AnomalyExclusions.options.length)
                document.options_adv_exclusions_anomaly.AnomalyExclusions.options.length++;

            document.options_adv_exclusions_anomaly.AnomalyExclusions.options[i].text = itemName;
            document.options_adv_exclusions_anomaly.AnomalyExclusions.options[i].value = itemVID;
        }       
    }
    catch(err)
    {
        parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
    }
    
    // Re-set focus to the list
    AnomalyExclusions.focus();
  }
  catch (err)
  {
    g_ErrorHandler.DisplayException (err);
    return;
  }

 } // with(document.options_adv_exclusions_anomaly)
}

// ** STD EXCL **

// Main page object
function AnomalyExclusionPage()
{

  var MB_YESNO = 0x00000004;
  var MB_ICONEXCLAMATION    = 0x00000030;
  var ID_YES = 6;
  var ID_NO = 7;
  try
  {
    // Set missing methods to default implementations
    this.Terminate = function() {}

    this.Default   = function()
    {
        try
        {
            window.external.Global = parent;
            
            try
            {
                var sww = new ActiveXObject('CcWebWnd.ccWebWindow');
            }
            catch (NAVErr)
            {
                window.external.Global = null;
                parent.g_ErrorHandler.DisplayNAVError (document.frames("Errors").ERROR_INTERNAL_ERROR_REINSTALL.innerText,  
                                                document.frames("Errors").ERROR_CREATING_NAVWEBWINDOW_OBJECT_ID.innerText);
                return;
            }

            var an = sww.MsgBox(document.all.DefaultsWarning.innerText, parent.NAVOptions.ProductName, MB_YESNO|MB_ICONEXCLAMATION);
            delete sww;
            window.external.Global = null;

            if(an == ID_YES)
            {
                parent.NAVOptions.SetAnomalyExclusionDefaults();
                this.Initialize();
            }
        }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }
    }  // end of Default()

    this.Validate  = function() { return true; }

    // Provide help for this page
    this.Help = function()
    {
        try
        {
            parent.NAVOptions.Help(250130); // #define NAVW_Options_Signature_Exclusions        250130
        }
        catch(err)
        {
            parent.NAVOptions.NAVError.LogAndDisplay(parent.g_HWND);
        }
    }

    // Initialize the from with the options value
    this.Initialize =  function()
    {
       // Enable scrolling for high-contrast mode
        if( parent.NAVOptions.IsHighContrastMode )
        {
            body_options_adv_exclusions_securityrisks.scroll = "auto";
            document.options_adv_exclusions_anomaly.Anomaly_Create.style.border = '1px solid white';
            document.options_adv_exclusions_anomaly.Anomaly_Remove.style.border = '1px solid white';
            document.options_adv_exclusions_anomaly.Anomaly_Details.style.border = '1px solid white';
       }
     
        anomaly_initialize();
    }  // end of Initialize()

      // Initialize this page
      this.Initialize();      
  }
  catch (err)
  {
    g_ErrorHandler.DisplayException (err);
    return;
  }
}

function OnClickAnomalyDetails()
{
    try
    {
        with(document.options_adv_exclusions_anomaly)
        {
            if( AnomalyExclusions.options.selectedIndex < 0 )
                return;
                
            DisplayVIDDetails(AnomalyExclusions.options[AnomalyExclusions.selectedIndex].value);
        }
    }
    catch(err)
    {
        g_ErrorHandler.DisplayException (err);
    }
}

function OnDoubleClickAnomalyList()
{
    try
    {
        // Make sure something was double-clicked on
        if( window.event.srcElement.value == 0 )
        {
            return;
        }
        
        DisplayVIDDetails(window.event.srcElement.value);
    }
    catch(err)
    {
        g_ErrorHandler.DisplayException (err);
    }
    
    return true;
}

function DisplayVIDDetails(ulVid)
{
    try
    {
        // Get the IThreatsByVID interface from the navoptions object
        // if necessary
        if( g_ThreatsByVID == null )
        {
            try
            {
                // Get the object and assing the INAVComError object to it
                g_ThreatsByVID = parent.NAVOptions.THREATSBYVID;
            }
            catch(NAVerr)
            {
                return;
            }
        }
        
        g_ThreatsByVID.DisplayDetails(ulVid, parent.NAVOptions.HWND);
    }
    catch(err)
    {
        g_ErrorHandler.DisplayException (err);
    }
}

function sortSelect(obj)
{
  try
  {
    if (obj == null || obj.options == null) 
        return;
        
    var o = new Array();

    for (var i=0; i<obj.options.length; i++)
    {
        o[o.length] = new Option( obj.options[i].text, obj.options[i].value, obj.options[i].defaultSelected, obj.options[i].selected) ;
    }
    
    if (o.length == 0)
        return;

    o = o.sort( 
        function(a,b)
        { 
            if ((a.text+"") < (b.text+""))
                return -1;
            if ((a.text+"") > (b.text+""))
                return 1;
                
            return 0;
        } 
        );

    for (var i=0; i<o.length; i++)
    {
        obj.options[i] = new Option(o[i].text, o[i].value, o[i].defaultSelected, o[i].selected);
    }
  }
  catch(err)
  {
    g_ErrorHandler.DisplayException (err);
  }
}
