/*******************************************************************
*                                                                  *
* File name:     rcCmd.cpp                                         *
*                                                                  *
* Description:   RC command processor                              *
*                                                                  *
* Statement:     Licensed Materials - Property of IBM              *
*                (c) Copyright IBM Corp. 1999                      *
*                                                                  *
* Author:        Alla Segal                                        *
*                                                                  *
*                U.S. Government Users Restricted Rights - use,    *
*                duplication or disclosure restricted by GSA ADP   *
*                Schedule Contract with IBM Corp.                  *
*                                                                  *
*                                                                  *
********************************************************************
*                                                                  *
* Function:      Execute commands specified in the command list    *
*                file                                              *
*                                                                  *
*******************************************************************/

#include "afxtempl.h"
#include "afxdisp.h"
#include "afxwin.h"
#include "afxole.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <io.h>
#include "wb70en32.h"
#include "excel8.h"
#include "msword8.h"
#include "msppt8.h"
#include "msacc8.h"
#include "errors.h"
#include "rcdefs.h"
#include "RCError.h"
#include "RCCommon.h"
#include "RCFile.h"
#include "RCKeys.h"
#include "RCGoatList.h"
#include "RCPilot.h"
#include "RCKeys.h"
#include "RCCmd.h"


bool g_application_busy;  // I hate the fact it is global, but there seem no other way
/*
 * constructor
 */
RCCmd::RCCmd(RCGoatList *GoatsList, 
			   RCCommon *Common, CString AppName, int AppVersion)
               : GoatsList(GoatsList),
				 Common(Common),
				 AppName(AppName)
{
	 // initialize common OLE parameters
	   	covTrue =(short) TRUE;
        covFalse = (short) FALSE;
		covOptional = COleVariant((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
          
        // initialize class name for the target application
	 	if (AppName == "Word") {
	      MainAppWinName =   "OpusApp";	  // word main class name
		} else if (AppName == "PowerPoint") {
			MainAppWinName = "PP97FrameClass";
		} else if (AppName == "Access") { // access main class name
			MainAppWinName = "OMain";
		} else {
          MainAppWinName =  "XLMain"; // excel main class name
		}
}

/*
 * Start the program and initialize OLE
 * FirstFile - the name of the first file to open in the target application
 */
void RCCmd::StartApp(CString FirstFile)
{
	DWORD appprocessid; // id of the process to pass to killpopup
    
	// start the application with OLE
    // if the application was killed unexpectedly, it is possible
    // that the dispatch has not been released which will prevent it
    // to be created again, so we need to check for it 

	try {
   	 if (AppName == "Excel") {

		if (m_app_ex.m_lpDispatch != NULL) // dispatch has not been released
		    m_app_ex.ReleaseDispatch(); // release it now

		  // start Excel
	      if (!m_app_ex.CreateDispatch(Common->OleId))  {
	  	     AfxMessageBox("Couldn't start Excel");
		  } else {
	    	  m_app_ex.SetVisible(TRUE); // make application visible
			 // m_app_ex.GetProperty(
		  }
		} 	else if (AppName == "Word")   
		  if (Common->OleId == "Word.Application") // Word 97, need to get to Word.Basic
		{ 
			if (m_app_wdapp.m_lpDispatch != NULL) // dispatch has not been released
				m_app_wdapp.ReleaseDispatch();    // release it now
			// start word 
			if (!m_app_wdapp.CreateDispatch(Common->OleId)) {
				AfxMessageBox("Couldn't start Word");
			} else {
				m_app_wdapp.SetVisible(1); // make it visible
				m_app_wd = m_app_wdapp.GetWordBasic(); // get to WordBasic class
			}
		  } else {   // Word 95
    		if (m_app_wd.m_lpDispatch != NULL) // dispatch has not been released
				m_app_wd.ReleaseDispatch(); // release it now
			if (!m_app_wd.CreateDispatch(Common->OleId)) { // word 95
               AfxMessageBox("Couldn't start Word");
			} else {
			  m_app_wd.AppShow(COleVariant("Microsoft Word")); // make the application
			                                                   // visible
			}
		  } else if (AppName == "PowerPoint") {
			  if (m_app_pp.m_lpDispatch != NULL) //dispatch has not been released
				  m_app_pp.ReleaseDispatch(); // release it now
			  if (!m_app_pp.CreateDispatch(Common->OleId)) { // init OLE and start the application
				  AfxMessageBox("Couldn't start Power Point");
			  } else {
                  m_app_pp.SetVisible(TRUE);
			  }
		  } else if (AppName == "Access") {
			  if (m_app_acc.m_lpDispatch != NULL) // dispatch has not been released
				  m_app_acc.ReleaseDispatch(); // release it
			  if (!m_app_acc.CreateDispatch(Common->OleId)) { // init OLE and start pgm
				  AfxMessageBox("Couldn't start Access");
			  } else {
				  m_app_acc.SetVisible(TRUE);
			  }
		  }
	} catch  (COleException *pCE) {  // some error during OLE processing
            SCODE sc = pCE->m_sc;
			Error.ReportError("Failed to start application: OLE exception %d",SCODE_SEVERITY(sc));
			pCE->Delete();
	}  
   

	AppHwnd = FindWindow(MainAppWinName, NULL);
	CurrentHwnd = AppHwnd; // currently active window handle

    if (AppHwnd == NULL) { // this should never happen
       Error.ExitError(RCAppStart,"Unable to start application");
	   return;
	}

		 // get process id of the application
    GetWindowThreadProcessId(AppHwnd, &appprocessid);
		// get application handle
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, appprocessid);
         

		// start the popup killer
    Error.ReportError("kill: %d", Common->nokillpopup);
    char ShortPath[MAXPATH];

    GetShortPathName(Common->PopupKillerPath, ShortPath, MAXPATH);

    if (Common->nokillpopup == 0) {
         if  (!Common->isFileReadable(Common->PopupKillerPath)) {
           CloseApp(); // close the application  
           Error.ExitError(RCBadPath, "Bad path for the popup killer: %s", Common->PopupKillerPath);
         } else {
		     char LogPath[MAXPATH];
			 CString commandline;		
			 getcwd(LogPath, MAXPATH-1); // get current directory

             commandline.Format("%s -app %s -timeout %s -coma %s -labels %s -apppid %lx -rcpath %s",
                ShortPath, AppName, Common->PopupKillerTimeOut, 
                Common->PopupKillerSleepTime, Common->LangCfgFile, appprocessid, LogPath);

          
            Error.ReportError("running %s",commandline);
			
			// start the popup killer
            if (WinExec (commandline,SW_SHOWNOACTIVATE) < 32) {
               CloseApp(); // close the application
               Error.ExitError(RCKillPopupStart,"The command : %s failed.", 
				   commandline);
        
             }
         }
    }
		

    		        /*open the first file */
    if (!FirstFile.IsEmpty() && !Common->isFileReadable(FirstFile)) {
         Error.ReportError("Cannot access the file : %s", FirstFile);
         FirstFile.Empty();
	} else if (!FirstFile.IsEmpty()) { // first file is specified
         OLEFileOpen(FirstFile); // open the file
	}	
    SetForegroundWindow(AppHwnd);
   
}

/*
 * Maximize the application window. This is not used now, but may be needed
 * in future
 */
int  RCCmd::OLEDocMaximize()
{
   
	if (AppName != "Word") {
        Error.ReportError("DocMaximize command is only supported under Word");
		return Err_OK; // no need to stop
	}

    if (WaitForInputIdle(hProcess, Common->WaitIdleTime) != 0) //timed out
	{
		Error.ReportError("OLEDocMaximize failed: the application is busy");
		return Err_TimeOut;
	}
	try {
       m_app_wd.DocMaximize(COleVariant((short) 1));
	} catch (COleDispatchException *pCE) 
	{
		CString s;
		Error.ReportError("Error %d: %s",pCE->m_wCode,pCE->m_strDescription);
		pCE->Delete();
	}	catch (COleException *pCE) {
            SCODE sc = pCE->m_sc;
			Error.ReportError("OleDocMaximize failed: OLE exception %d",SCODE_SEVERITY(sc));
			pCE->Delete();
	}  catch (char *str) {
		Error.ReportError("caught some other exception during OleDocMaximize %s",str);
	}

	 return Err_OK;
}

/*
 *  Scroll to the end of the beginning of the document
 *  This command is only supported under Word
 */
int RCCmd::OLEGoto(CString GoatCode, CString Position)
{
      RCFile *G;
      short ret;
      CString CurrentDocName;
	  CString strTmpWName;
	 

	  if (AppName != "Word") {
        Error.ReportError("GOTO command is only supported under Word");
		return Err_OK; // no need to stop
	  }
    G = GoatsList->FindFileByCode(GoatCode);
	    
    ret = ReadCurrentDocName(CurrentDocName); //recode this one to use OLE
    Common->GetFileWName(G->RealName, strTmpWName); // could get full name here?
     
	 if (!isAppRunning()) // need a better way to see if we are running
		 return Err_App_Not_Running;
	 else  if (ret == Err_OK) 
        if (CurrentDocName.CompareNoCase(strTmpWName)==0) {
			try
			{
			   if (strcmp(Position, "START") == 0) {
                  m_app_wd.StartOfDocument(COleVariant((short) 0));
			   } else if (strcmp(Position,"END") == 0) {
                 m_app_wd.EndOfDocument(COleVariant((short)0));
			   }
            } catch (COleDispatchException *pCE)
			{
				CString s;
				Error.ReportError("Error %d: %s",pCE->m_wCode,pCE->m_strDescription);
				pCE->Delete();
			} catch (COleException *pCE) {
              SCODE sc = pCE->m_sc;
	          Error.ReportError("GoTo failed: OLE exception %d",SCODE_SEVERITY(sc));
			  pCE->Delete();
			}  catch (char *str) {
		       Error.ReportError("caught some other exception during GoTo %s",str);
			}
             
		}else
           return Err_Active_Doc_Not_Match;
     else {
    
       Error.ReportError("OLEGoto failed : error %d reading the active document name. (%s)",
            ret, GoatCode);
       return Err_Unknown;
	 }
  return Err_OK;
}

/*
 * Close the file via OLE
 * GoatCode - code of the file to be closed
 */
int RCCmd::OLEFClose(CString GoatCode)
{
    RCFile *G;
    CString CurrentDocName;
    int ret;
	CString strTmp;

   ret = WaitForInputIdle(hProcess, Common->WaitIdleTime);
   if (ret != 0) {
    Error.ReportError ("OLEFClose failed ; the application is busy.");
    return Err_TimeOut;
   }
 
   if (!isAppRunning()) {
    Error.ReportError("OLEFClose failed : the application is not running. (%s).",
	        GoatCode);
    return Err_App_Not_Running;
   }
    G = GoatsList->FindFileByCode(GoatCode);

    ret = ReadCurrentDocName(CurrentDocName);
	if (CurrentDocName.IsEmpty()) { //empty document name
		Error.ReportError("OLEFClose failed: the document is not active");
		return Err_Target_Doc_Not_Active;
	}
    Common->GetFileWName(G->RealName, strTmp);
  
	strTmp.MakeUpper();
	CurrentDocName.MakeUpper();
    if (ret == 0) 
		if (CheckActiveGoatNameChange(GoatCode) != Err_Not_Same_Names) { 
    	try
		{

		  if (AppName == "Excel") {       // Excel
			  _Workbook book = m_app_ex.GetActiveWorkbook();
	          book.Close(COleVariant((short)TRUE), covOptional, covOptional);
		  } else if (AppName == "PowerPoint") { // Power Point
             _Presentation pres = m_app_pp.GetActivePresentation();
			  pres.SetSaved(TRUE);
	          pres.Close();
		  } else if (AppName == "Access") { //Access
			  m_app_acc.CloseCurrentDatabase();
          } else { //word
              m_app_wd.FileClose(COleVariant((short) TRUE));
		  }
		} catch (COleDispatchException *pCE) {
            CString s;
			Error.ReportError("Error %d: %s",pCE->m_wCode,pCE->m_strDescription);
			pCE->Delete();
			return Err_OleError;
		} catch (COleException *pCE) {
            SCODE sc = pCE->m_sc;
			Error.ReportError("FileClose failed: OLE exception %d",SCODE_SEVERITY(sc));
			pCE->Delete();
		}  catch (char *str) {
		    Error.ReportError("caught some other exception during FileClose %s",str);
		}        
   } else {
        Error.ReportError(
             "OLEFClose failed : %s is active instead of %s. (goat : %s) failed.",
				   CurrentDocName,strTmp,GoatCode);
        return Err_Target_Doc_Not_Active;
    }
  
 else {
   
   Error.ReportError("OLEFClose failed : error %d reading the active document name. (%s)",
                   ret, GoatCode);
  return Err_Unknown;
 }
 // closed successfully - removed doc from currently opened list
 POSITION pos =FindDocOnList(G->RealName);
 if (pos !=NULL)
	 CurrentlyOpenedDocs.RemoveAt(pos); //removed it
 
 return Err_OK;
} 

/* 
 * Create new file via OLE
 * GoatCode - new file goat code
 * PlainDocPath - default path of the file
 * Note: for Microsoft access, the goat file must have full name specified
 */
int RCCmd::OLEFNew(CString GoatCode, CString PlainDocPath)
{
      RCFile *G;
      CString CurDocName;
	  int ret;

   ret = WaitForInputIdle(hProcess, Common->WaitIdleTime);
   if (ret != 0) {
    Error.ReportError ("OLEFNew failed ; the application is busy.");
    return Err_TimeOut;
   }
 
   if (!isAppRunning()) {
    Error.ReportError("OLEFNew failed : the application is not running. (%s).",
	        GoatCode);
    return Err_App_Not_Running;
   }

   // find the file entry
    G = GoatsList->FindFileByCode(GoatCode);
	// call OLE services to create the file
   	try
	{
	  if (AppName == "Excel") {  
		  // first need to get Workbooks
	  	_Workbook book;
        Workbooks m_books = m_app_ex.GetWorkbooks();
		// add a new workbook
	    book = m_books.Add(covOptional);
	  } else if (AppName == "PowerPoint") { //Power Point
 	  	_Presentation pres;   
		Presentations presentations;

         presentations = m_app_pp.GetPresentations(); // get handle to presentations
	     pres = presentations.Add((long) TRUE); // add a presentation
         Slides slides = pres.GetSlides(); // get handle to slides
    	 slides.Add((long) 1, ppLayoutBlank); // add a blank slide
	  } else if (AppName == "Access") {
		  CString strName = G->RealName;
		  if (strName.IsEmpty()) { // file name not specified
             Error.ReportError("OLENEW failed: goat file name is not specified in the goat list");
			 return Err_Active_Doc_Not_Match;
		  }
	      m_app_acc.NewCurrentDatabase(strName);

      } else { // Word
        m_app_wd.FileNewDefault();
	  }
	} catch (COleDispatchException *pCE) {
        CString s;
	    Error.ReportError("Error %d: %s",pCE->m_wCode,pCE->m_strDescription);
		pCE->Delete();
		return Err_OleError;
	} catch (COleException *pCE) {
            SCODE sc = pCE->m_sc;
			Error.ReportError("FileNew failed: OLE exception %d",SCODE_SEVERITY(sc));
			pCE->Delete();
	}  catch (char *str) {
		Error.ReportError("caught some other exception during FileNew %s",str);
	}

      ReadCurrentDocName (CurDocName);
      if (G != NULL) {
         G->BackupName = PlainDocPath;
		 if (AppName != "Access")   // access goat already has a real name
           G->RealName = CurDocName;
	  }

    // add document to list
    CurrentlyOpenedDocs.AddTail(G->RealName);
 
   return Err_OK; 
}

/*
 * Open the file via OLE
 * GoatCode - code of the file to be opened
 */
int RCCmd::OLEFOpen(CString GoatCode) 
{

    RCFile *G;
	int ret;

   ret = WaitForInputIdle(hProcess, Common->WaitIdleTime);
   if (ret != 0) {
    Error.ReportError ("OLEFOpen failed ; the application is busy.");
    return Err_TimeOut;
   }
 
   if (!isAppRunning()) {
    Error.ReportError("OLEFOpen failed : the application is not running. (%s).",
	        GoatCode);
    return Err_App_Not_Running;
   }

    G = GoatsList->FindFileByCode(GoatCode);
    
	if (G== NULL) {   // should not happen
		CloseApp(); // clean the application
	    Error.ExitError(RCFileNotFound,"Error in OLEFOpen, file with code %s not found", GoatCode);
	}

    if (!Common->isFileReadable(G->RealName)) {
        Error.ReportError("%s does not exist or cannot be opened (%s).",
	    	G->RealName, GoatCode);
        return Err_Unable_Read_File;
       
    }
	return OLEFileOpen(G->RealName);

}   

/* 
 * Open the file with OLE
 * FileName -- full path to the file to be opened
 */
int RCCmd::OLEFileOpen(CString FileName) 
{
	int ret;

	// wait till the application is ready to receive the commands
   ret = WaitForInputIdle(hProcess, Common->WaitIdleTime);
   if (ret != 0) {
    Error.ReportError ("OLEFileOpen failed ; the application is busy.");
    return Err_TimeOut;
   }
 
 
   // see if anyone killed the application under us
   if (!isAppRunning()) {
    Error.ReportError("OLEFileOpen failed : the application is not running.");
    return Err_App_Not_Running;
   }

   // use OLE to open the file
    try
	{
	  if (AppName == "Excel") {
	  	_Workbook book;
		// need to get pointer to Workbooks class first
        Workbooks m_books = m_app_ex.GetWorkbooks();
	    book = m_books.Open(FileName,
		    covOptional, covOptional, covOptional, covOptional,
		    covOptional, covOptional, covOptional, covOptional,
            covOptional, covOptional, covOptional, covOptional);

		// now need to get the Windows to make sure the application is visible
		// this is needed when we are opening the first file
		Windows wnds = book.GetWindows();
	    Window wnd = wnds.GetItem(COleVariant((short) 1));
	    if (wnd.GetVisible() != 1)  wnd.SetVisible(1);  // make it visible
	  } else if (AppName == "PowerPoint") {
        Presentations presentations = m_app_pp.GetPresentations();
 	  	_Presentation pres = presentations.Open(FileName,(long) FALSE, 
		                    (long) FALSE, (long) TRUE);
	  } else if (AppName == "Access") {
         m_app_acc.OpenCurrentDatabase(FileName, FALSE);
      } else { //word
		  if (Common->OleId == "Word.Application") { //Word 97
			  Documents docs = m_app_wdapp.GetDocuments();
			  _Document doc = docs.Open(COleVariant(FileName),
		              covOptional, covOptional, covOptional, covOptional,
		              covOptional, covOptional, covOptional, covOptional,
		              covOptional);
		  } else {   // Word 95
        	m_app_wd.FileOpen(COleVariant(FileName),
		    covOptional, covOptional, covOptional, covOptional,
		    covOptional, covOptional, covOptional, covOptional);
		  }
		 
	  }
	} catch (COleDispatchException *pCE) {
        CString s;
	    Error.ReportError("Error %d: %s",pCE->m_wCode,pCE->m_strDescription);
		pCE->Delete();
		return Err_OleError;
	} catch (COleException *pCE) {
            SCODE sc = pCE->m_sc;
			Error.ReportError("FileOpen failed: OLE exception %d",SCODE_SEVERITY(sc));
			pCE->Delete();
			return Err_OleError;
	}  catch (char *str) {
		Error.ReportError("caught some other exception during FileOpen %s",str);
	}

	// add doc to the list of those currently opened
	CurrentlyOpenedDocs.AddTail(FileName); 
	return Err_OK;
}   

/*
 * Save the file via OLE
 * GoatCode - code of the file in the goat list
 */
int RCCmd::OLEFSave(CString GoatCode) 
{
    RCFile *G;
    int ret;
    CString CurrentDocName;
	CString strTmp;

   ret = WaitForInputIdle(hProcess, Common->WaitIdleTime);
   if (ret != 0) {
    Error.ReportError ("OLESave failed ; the application is busy.");
    return Err_TimeOut;
   }
 
   if (!isAppRunning()) {
    Error.ReportError("OLESave failed : the application is not running. (%s).",
	        GoatCode);
    return Err_App_Not_Running;
   }
   if (AppName == "Access") { // we don't support save for Access
      Error.ReportError("OLESAVE failed : the command is not supported for Access");
	  return RCCmdFileError;
   }

   G = GoatsList->FindFileByCode(GoatCode);
   if (G == NULL) return 1; // should not happen ?
   ret = ReadCurrentDocName(CurrentDocName);
   Common->GetFileWName(G->RealName,strTmp);

   if (CheckActiveGoatNameChange(GoatCode) == Err_Not_Same_Names) {
	   Error.ReportError("OLESave failed: the active document is %s instead of %s (%s)",
		   CurrentDocName, G->RealName, GoatCode);
	   return Err_Not_Same_Names;
   }

   if (ret == Err_OK) {
    try
	{
	  if (AppName == "Excel") {
	  	_Workbook book = m_app_ex.GetActiveWorkbook();;
		book.Save();
	  } else if (AppName == "PowerPoint") {
        _Presentation pres = m_app_pp.GetActivePresentation();
		pres.Save();
      } else { //word
		   m_app_wd.FileSave();
	  }
	} catch (COleDispatchException *pCE) {
        CString s;
	    Error.ReportError("Error %d: %s",pCE->m_wCode,pCE->m_strDescription);
		pCE->Delete();
		return Err_OleError;
	} catch (COleException *pCE) {
            SCODE sc = pCE->m_sc;
			Error.ReportError("FileSave failed: OLE exception %d",SCODE_SEVERITY(sc));
			pCE->Delete();
	}  catch (char *str) {
		Error.ReportError("caught some other exception during FileSave %s",str);
	}	
   } else {
     Error.ReportError("OLEFSave failed : error %d reading the active document name. (%s)",
          ret, GoatCode);
     return Err_Unknown;
   }
   return Err_OK;
}

/*
 * process OLESAVEASFORMAT command - upconvert or downconvert samples
 * OldGoatCode - code of the goat to save
 * NewGoatCode - new goat code
 * Format - file format (can be 95 or 97)
 * supported for Excel only, we do not support downconversions for Word
 * while upconversions for Word are automatic
 */
int RCCmd::OLEFSaveAsFormat(CString OldGoatCode, CString NewGoatCode, int Format) 
{
    RCFile *OldG, *NewG;
    int ret;
    CString CurrentDocName;
	CString strTmp;
	CString filename;
	int iNameCheck;

	if (AppName != "Excel") {
	   Error.ReportError("OLEFSaveAsFormat failed; the command is only valid for Excel");
	   return Err_Unknown;
	}	   
   ret = WaitForInputIdle(hProcess, Common->WaitIdleTime);
   if (ret != 0) {
    Error.ReportError ("OLESaveAs95 failed ; the application is busy.");
    return Err_TimeOut;
   }
    
   if (!isAppRunning()) {
    Error.ReportError("OLESaveAs95 failed : the application is not running. (%s as %s).",
	        OldGoatCode, NewGoatCode);
    return Err_App_Not_Running;
   }

   OldG = GoatsList->FindFileByCode(OldGoatCode);
   NewG = GoatsList->FindFileByCode(NewGoatCode);
   if (OldG == NULL || NewG == NULL) 
	   Error.ExitError(RCCmdFileError,"OLESaveAs: Goat list error - goat code not found"); // should not happen ?

   filename = NewG->RealName;
   NewG->BackupName = OldG->BackupName;
   NewG->isVirusCopy = 0; //do not mark it as virus because we want it to be copied
                          //may make sense to add a flag for downconversion later
   ret = ReadCurrentDocName(CurrentDocName);

   iNameCheck = CheckActiveGoatNameChange(OldGoatCode);
   if ((iNameCheck == Err_Not_Same_Names) || (iNameCheck == Err_DroppedFile))
   {
	   Error.ReportError("OLESaveAs failed:the target doc is not active. (%s to %s).",
	       OldGoatCode, NewGoatCode);

        return Err_Target_Doc_Not_Active;
   }
   if (ret == Err_OK) {
     try {
	   _Workbook book = m_app_ex.GetActiveWorkbook();
	   if (Format == 95)
	     book.SaveAs(COleVariant(filename), COleVariant(xlExcel5) , covOptional, covOptional,
            covOptional, covOptional, _A_NORMAL, covOptional,
	        covOptional, covOptional, covOptional);
	   else if (Format == 97)
         book.SaveAs(COleVariant(filename), COleVariant(xlWorkbookNormal) , covOptional, covOptional,
            covOptional, covOptional, _A_NORMAL, covOptional,
	        covOptional, covOptional, covOptional);
	   else
         Error.ReportError("invalid format: only 95 or 97 are supported");

	 } catch (COleDispatchException *pCE) {
        CString s;
	    Error.ReportError("Error %d: %s",pCE->m_wCode,pCE->m_strDescription);
		pCE->Delete();
		return Err_OleError;
	 } catch (COleException *pCE) {
            SCODE sc = pCE->m_sc;
			Error.ReportError("FileSaveAs failed: OLE exception %d",SCODE_SEVERITY(sc));
			pCE->Delete();
	 }  catch (char *str) {
		Error.ReportError("caught some other exception during FileSaveAs %s",str);
	 }
   } else {
     Error.ReportError("OLESaveAs95 failed : error %d reading the active document name. (%s)",
          ret, OldGoatCode);
     return Err_Unknown;
   }
         // replace old name on currently opened list with the new one
    // closed successfully - removed doc from currently opened list
   POSITION pos = FindDocOnList(OldG->RealName);
   if (pos !=NULL)
 	 CurrentlyOpenedDocs.RemoveAt(pos); //removed it
   CurrentlyOpenedDocs.AddTail(NewG->RealName);
   return Err_OK;
}

/*
 *  saves and closes the current file in case unexpected file pops up by the virus
 */
int RCCmd::OLECurrentFileClose()
{

	  if (!isAppRunning())
		  return Err_App_Not_Running;
	  else 
	  {
	    try
		{
			if (AppName == "Excel") { //excel
			  _Workbook book = m_app_ex.GetActiveWorkbook();
	          book.Close(COleVariant((short)TRUE), covOptional, covOptional);
		  } else if (AppName == "PowerPoint") { // Power Point
			 _Presentation pres = m_app_pp.GetActivePresentation();
	         pres.Close();
		  } else if (AppName == "Access") { //Microsoft Access
			  m_app_acc.CloseCurrentDatabase();
          } else { //word
              m_app_wd.FileClose(COleVariant((short) 1)); // save the document
		  }
		} catch (COleDispatchException *pCE) {
            CString s;
			Error.ReportError("Error %d: %s",pCE->m_wCode,pCE->m_strDescription);
			pCE->Delete();
			return Err_OleError;
		} catch (COleException *pCE) {
            SCODE sc = pCE->m_sc;
			Error.ReportError("CurrentFileClose failed: OLE exception %d",SCODE_SEVERITY(sc));
			pCE->Delete();
		}  catch (char *str) {
		   Error.ReportError("caught some other exception during CurrentFileClose %s",str);
		}		
      }
     return Err_OK; 
}

/*
 * Perform the SaveAs command via OLE
 * OldGoatCode -- code of the old file to be saved as the file
 * with the NewGoatCode 
 */
int RCCmd::OLEFSaveAs(CString OldGoatCode, CString NewGoatCode) 
{
   RCFile * OldG, *NewG;
   int ret;
   CString CurrentDocName;

   CString strTmp;
	int iNameCheck;

   if (AppName == "Access") { // we don't support save for Access
      Error.ReportError("OLESAVE failed : the command is not supported for Access");
	  return RCCmdFileError;
   }

   ret = WaitForInputIdle(hProcess, Common->WaitIdleTime);
   if (ret != 0) {
    Error.ReportError ("OLEFSaveAs failed ; the application is busy.");
    return Err_TimeOut;
   }
 
   if (!isAppRunning()) {
    Error.ReportError("OLEFSaveAs failed : the application is not running.");
    return Err_App_Not_Running;
   }

   // Get file entries in the goat list
  NewG = GoatsList->FindFileByCode(NewGoatCode);
  OldG = GoatsList->FindFileByCode(OldGoatCode);
  Common->GetFileWName(OldG->RealName, strTmp);
  
  if (NewG == NULL || OldG == NULL) { // an unexpected error in the goat list
      Error.ReportError("OLEFSaveAs:Unknown error: either NewG or OldG is NULL");
	  return Err_Unknown; // should not happen
  }
  ret = ReadCurrentDocName(CurrentDocName);
  if (ret == Err_OK) {
		iNameCheck = CheckActiveGoatNameChange(OldGoatCode);
       if ((iNameCheck !=  Err_Not_Same_Names) && (iNameCheck !=  Err_DroppedFile)) {
		 try
		 {
	        if (AppName == "Excel") {
	  	       _Workbook book = m_app_ex.GetActiveWorkbook();;
			   book.SaveAs(COleVariant(NewG->RealName), covOptional, covOptional, covOptional,
                  covOptional, covOptional, _A_NORMAL, covOptional,
				  covOptional, covOptional, covOptional);
			} else if (AppName == "PowerPoint") {
			   _Presentation pres = m_app_pp.GetActivePresentation();
	           pres.SaveAs(NewG->RealName, ppSaveAsPresentation, (long) FALSE);
			} else { //word
		       m_app_wd.FileSaveAs(COleVariant(NewG->RealName),covOptional, covOptional, covOptional,
                  covOptional, covOptional, covOptional, covOptional,
				  covOptional, covOptional, covOptional);

			}
		 } catch (COleDispatchException *pCE) {
            CString s;
	        Error.ReportError("Error %d: %s",pCE->m_wCode,pCE->m_strDescription);
		    pCE->Delete();
		    return Err_OleError;
		 } catch (COleException *pCE) {
            SCODE sc = pCE->m_sc;
			Error.ReportError("FileSaveAs failed: OLE exception %d",SCODE_SEVERITY(sc));
			pCE->Delete();
		 }  catch (char *str) {
	 	     Error.ReportError("caught some other exception during FileSaveAs %s",str);
		 }		 
		  NewG->BackupName = OldG->BackupName;
		  		  // replace old name in currently opened list with the new one
		   // closed successfully - removed doc from currently opened list
           POSITION pos = FindDocOnList(OldG->RealName);
           if (pos !=NULL)
	           CurrentlyOpenedDocs.RemoveAt(pos); //removed it

           CurrentlyOpenedDocs.AddTail(NewG->RealName);

		   //mark the goat as being a copy of the virus sample if needed
		   NewG->isVirusCopy = OldG->isVirusCopy;

          return Err_OK;
       } else {
           Error.ReportError("OLEFSaveAs failed : %s is active instead of the target document. (%s)",
	          CurrentDocName, OldGoatCode);
           return Err_Target_Doc_Not_Active;
        }
  } else {
     Error.ReportError("OLEFSaveAs failed : error %d reading the active document name. (%s)",
         ret, OldGoatCode);
    return Err_Unknown;
  }
  return Err_Unknown; // shouldn't get here
}

/*
 *  process MCLOSE command - used WM_COMMAND or keyboard to open file
 *  GoatCode - code of the goat to open
 *  IPCmode - WM_COMMAND if IPC_WM; keyboard if IPC_KS
 */
int RCCmd::FClose(CString GoatCode, int IPCmode)
{
   int j, seconds;
   double totaltime;
   RCFile *G;
   char filename[MAXPATH];
   CString CurDocName;
   int ret;

  // check whether the process is busy
  ret = WaitForInputIdle(hProcess, Common->WaitIdleTime);
   if (ret != 0) {
    Error.ReportError ("Fclose failed ; the application is busy.");
    return Err_TimeOut;
   }
 
   //wait for unexpected popups to be killed by killpopup
   seconds = 60 * Common->TimeoutInterval;
   totaltime = 0;
   while (totaltime < seconds && isAppRunning()
	   && CurrentHwnd != GetLastActivePopup(CurrentHwnd)) {
    Sleep(Common->TimeoutInterval/2);
    totaltime += Common->TimeoutInterval/2;
   }
   // check if killpopup hasn't killed the application
   if (!isAppRunning()) {

    Error.ReportError("FClose failed : the application is not running. (%s).",
	        GoatCode);
    return Err_App_Not_Running;
   }
   if (totaltime >= seconds) { // if the popup is still there try to get rid of it
    SetForegroundWindow (CurrentHwnd); 
   
    HWND hwnd =GetLastActivePopup(CurrentHwnd);
    if  (SetForegroundWindow (hwnd)) {
       Keys.SendKeys("[ESC]", hwnd);  
	  Sleep(2*Common->TimeoutInterval);    // To replace DoEvents for now
    } 
		
    return Err_Unknown;
   }
 
  // find the file on the goat list
   G = GoatsList->FindFileByCode(GoatCode);
   if (G != NULL) // file is found
	   strcpy(filename, G->RealName);
   else {  // unexpected error in the goat list
	   Error.ReportError("FClose failed: Unknown error -- could not find %s in goat list",
		   GoatCode);
	   return Err_Unknown;   // should never happen
   }

   ret = ReadCurrentDocName(CurDocName); // get the name of the currently active doc
   if (ret == Err_OK) { // no problem with reading the name
	   	if (CurDocName.IsEmpty()) { //empty document name
		  Error.ReportError("FClose failed: the document is not active");
		  return Err_Target_Doc_Not_Active;
		}

		// Close the file
	   CString strTmp;
	   Common->GetFileWName(G->RealName, strTmp); // get name without the path
	   CurDocName.MakeUpper();
	   strTmp.MakeUpper();
	   if (CheckActiveGoatNameChange(GoatCode) != Err_Not_Same_Names) { // the expected document is open or dropped file

         if (IPCmode == IPC_KS) { // KCLOSE 
            SetForegroundWindow (AppHwnd);
            Keys.SendKeys(Common->CloseShortcut, AppHwnd); 
		 } else if (IPCmode == IPC_WM) { // MCLOSE
             PostMessage (CurrentHwnd, WM_COMMAND,  Common->CloseWP, 0);
		 }
    
    // wait until the file is closed
           j = 0;
	       seconds = 100 *Common->TimeoutInterval;  // some viruses take a lot of time to close
           while (isFileOpen(filename, TRUE) && j < seconds) {
               Sleep (Common->TimeoutInterval);
		       j+=Common->TimeoutInterval;
		   }
          
           if (j < seconds) {
			    // closed successfully - removed doc from currently opened list
               POSITION pos =CurrentlyOpenedDocs.Find(G->RealName);
               if (pos !=NULL)
	               CurrentlyOpenedDocs.RemoveAt(pos); //removed it

               return Err_OK;
           } else {
             Error.ReportError("FClose failed: %s is still present after timeout. (%s).",
		       filename, GoatCode);
             return Err_TimeOut;
		   }


	   } else {
         Error.ReportError("FClose failed: %s is the active document instead of %s. (%s).",
	      CurDocName, filename, GoatCode);
         return Err_Target_Doc_Not_Active;
	   }
   } else {
      Error.ReportError("FClose failed: error %d reading the active document name. (%s)",
         ret, GoatCode);
      return Err_Unknown;
   }

}

/*
 * process MNEW command - used WM_COMMAND or keyboard to open file
 *  GoatCode - code of the goat to open
 *  IPCmode - WM_COMMAND if IPC_WM; keyboard if IPC_KS
 *  PlainDocPath - default path for files
 */
int RCCmd::FNew(CString GoatCode, int IPCmode, CString PlainDocPath) 
{

    int Timeout, j;
    CString prev_doc;
	CString cur_doc;
    int ret;
	int rc;
	CString strAccessFileName;
	
    RCFile *G = GoatsList->FindFileByCode(GoatCode);
	if (AppName == "Access") { // goat name must have the RealName
		if (G->RealName.IsEmpty()) { 
			Error.ReportError("FNEW failed: goat's RealName must be specified for Access");
			return Err_Unknown;
		}
		strAccessFileName = G->RealName;
	}

    if (!isAppRunning()) { // application failed or was killed
      CloseApp(); // clean up OLE
      return Err_App_Not_Running;
	}

     ret = ReadCurrentDocName(prev_doc); // get the name of currently opened document
	 if (ret == Err_OK || ret == Err_No_Doc_Open) { 

      if (WaitForInputIdle (hProcess, Common->WaitIdleTime) != 0) {
		Error.ReportError("FNew failed: the application is busy");
		return Err_TimeOut;
	  }

	  SetForegroundWindow(CurrentHwnd);
      if (IPCmode == IPC_KS) {
         Keys.SendKeys (Common->NewShortcut, CurrentHwnd);

	  } else if (IPCmode == IPC_WM)
        PostMessage (AppHwnd, WM_COMMAND, Common->NewWP, 0);
    
       
      j = 0;
	  Timeout = 10*Common->TimeoutInterval;
      while (GetAppDlg(NewDlgClassName, Common->NewTitle) == NULL && j < Timeout) {
        Sleep (Common->TimeoutInterval);
        j+=Common->TimeoutInterval;
	   }
      if (!isAppRunning())
        return Err_App_Not_Running;
       HWND hwnd = GetAppDlg(NewDlgClassName, Common->NewTitle);  
    // check if the dialog box appears (some viruses do not display it)
	   if ( hwnd!= NULL) 
         if (WaitForInputIdle(hProcess, Common->WaitIdleTime) == 0) {
			 SetForegroundWindow(hwnd);
			if (!Common->NewMoving.IsEmpty())
				Keys.SendKeys(Common->NewMoving, hwnd);
			keybd_event(VK_RETURN,0,0,0);
        } else {
             Error.ReportError("Fnew failed: application busy after displaying the dialog box. (%s).",
 			        (LPCTSTR) GoatCode);
      return Err_TimeOut;
      
        }
       else
   		 Error.ReportError("Info: could not find New dialog");
		
	   if (AppName == "PowerPoint") { //need to select slide
         int i;
		 for (i=0; i<ppLayoutBlank-1;i++)   // get to blank presentation 
			 keybd_event(VK_RIGHT, 0,0,0);
		 keybd_event(VK_RETURN, 0,0,0);
	   } else if (AppName == "Access") { //need to send in the name
		   strAccessFileName += '~';
		   Keys.SendKeys(strAccessFileName, hwnd);
	   }
    // wait until the new file is open
    // normally, the title of the window when a new file is created is Microsoft Word - DocumentX or Microsoft Word - TemplateX
    // nevertheless, some viruses save the new files just after their creation with names like docX.doc/t
    j = 0;
    while (isAppRunning() && j < Timeout) {
       if (AppName == "Access" && isFileOpen(G->RealName, TRUE))
		   break; //for Access new files have names
	   else {
         ret = ReadCurrentDocName(cur_doc);
         if (ret == Err_OK)
          
           if (cur_doc.Left(Common->DocumentLabel.GetLength()).CompareNoCase(Common->DocumentLabel) == 0
			  || cur_doc.Left(Common->DocLabel.GetLength()).CompareNoCase(Common->DocLabel) == 0
              || cur_doc.Left(Common->TemplateLabel.GetLength()).CompareNoCase(Common->TemplateLabel)== 0)
            break;
	   }
       Sleep (Common->TimeoutInterval);
	   j+=Common->TimeoutInterval;
    }
    if (j >= Timeout) {
        Error.ReportError("FNew failed : could not detect any new document. (%s).",
		    GoatCode); 
        return Err_TimeOut;
        
    }
    
    if (!isAppRunning())
         return Err_App_Not_Running;
    ret = ReadCurrentDocName(cur_doc); 
    if (ret == Err_OK) {
    
	  if (G != NULL) {
	    G->BackupName = PlainDocPath;
        G->RealName = cur_doc;
	  }
      rc = Err_OK;
    } else {
      Error.ReportError("FNew failed : error %d reading the active document name. (%s)",
	      ret, GoatCode);
      rc = Err_Unknown;
    }
	} else {
     Error.ReportError("FNew failed : error %d reading the active document name. (%s)",
          ret, GoatCode);
      rc = Err_Unknown;
	}
	if (rc == Err_OK) {
		CurrentlyOpenedDocs.AddTail(cur_doc);
	}
	return rc;
}  

/*
 * process MOPEN command - used WM_COMMAND or keyboard to open file
 *  GoatCode - code of the goat to open
 *  IPCmode - WM_COMMAND if IPC_WM; keyboard if IPC_KS
 */
int RCCmd::FOpen(CString GoatCode, int IPCmode) 
{

  double j, totaltime;
  int seconds;
  CString dde_command;
  HWND  hwnd;
  CString openname,filename;
  RCFile *G;
  int ret;
   
 
  G = GoatsList->FindFileByCode(GoatCode);
  if (G== NULL)   {// should not happen, must be an error in the command file or the goat list
    CloseApp(); // close the application
	Error.ExitError(RCFileNotFound,"Error in FOpen, file with code %s not found", GoatCode);
  }
  filename = G->RealName;
  if (!Common->isFileReadable(G->RealName)){
	  Error.ReportError ("FOpen failed : can\'t open %s (goat : %s.)",
		  G->RealName, GoatCode);


    return Err_Unable_Read_File;
    
  }

   seconds = 40*Common->TimeoutInterval;
   totaltime = 0;
   while (totaltime < seconds && isAppRunning() 
	   && CurrentHwnd != GetLastActivePopup(CurrentHwnd)) {
    Sleep(Common->TimeoutInterval/4);
    totaltime +=Common->TimeoutInterval/4;
   }

   if (totaltime >= seconds) 
    return Err_TimeOut;

   if (!isAppRunning()) {
     Error.ReportError("FOpen failed : the application is not running. (%s)",
           GoatCode);
     return Err_App_Not_Running;
   }

   ret = WaitForInputIdle(hProcess, Common->WaitIdleTime);
   if (ret != 0) {
     Error.ReportError("FOpen failed : the application is busy. (%s)",
        GoatCode);
     return Err_Unknown;
   }

  // Send the command to display the Open dialog
   if (IPCmode == IPC_KS) {
	   if (!SetForegroundWindow(CurrentHwnd)) {
		   if (!isAppRunning()) {
              Error.ReportError("FOpen failed : the application is not running. (%s)",
			   GoatCode);
              return Err_App_Not_Running;
		   } else {
              Error.ReportError("FOpen failed : unable to set the application as foreground window. (%s)",
			    GoatCode);
              return Err_Unknown;
		   }
	   } else {
        Keys.SendKeys (Common->OpenShortcut, CurrentHwnd); // send keys
	   }
   } else if (IPCmode == IPC_WM)
          PostMessage (CurrentHwnd, WM_COMMAND, Common->OpenWP, 0);

 
  
   // give Word time to open the file
   j = 0;
   seconds = 10 * Common->TimeoutInterval;
   while (GetAppDlg(Common->OpenDlgClassName,Common->OpenTitle) == NULL
		   && j < seconds 
		   && isAppRunning()) {
         Sleep (Common->TimeoutInterval/4);
	  j +=  Common->TimeoutInterval/4;
   }
   
   if (!isAppRunning()) {
          Error.ReportError("FOpen failed : app is not running. (%s)",
	             GoatCode);
          return Err_App_Not_Running;
    
   } else if (j >= seconds) {
          Error.ReportError("FOpen failed : no dialog box appeared. (%s)",
	         GoatCode);
          return Err_Missing_DlgBox;
   }    
 

   // send the name of file to open to the dialog
   // if it is Access and the goat is the virus, just choose the first file
    openname =filename;     
    openname += '~';

    hwnd = GetAppDlg(Common->OpenDlgClassName, Common->OpenTitle);
    WaitForInputIdle (hProcess, Common->WaitIdleTime);
    if (!SetForegroundWindow(hwnd)) {
         Error.ReportError("Unable to set the open dlgbox as the foreground window.");
         return Err_Unknown;
	}
    Keys.SendKeys( openname, hwnd);
   
   // give the window time to close
    j = 0; 
    seconds = 10*Common->TimeoutInterval;
    while (GetAppDlg(Common->OpenDlgClassName, Common->OpenTitle) && j < seconds) {
        Sleep (Common->TimeoutInterval);
        j+=Common->TimeoutInterval;
	}
      
	// if the dialog is still there, cancel it
   if (j >= seconds) {
        Keys.SendKeys ("[ESC]", hwnd);
		Sleep(Common->TimeoutInterval);
        Error.ReportError( "FOpen failed : the dialog box does not close. (%s)",
	         GoatCode);
        return Err_DlgBox_Not_Close;
   }
	     
    
    // give Word time to open the file
    j = 0; 
    seconds = 7*Common->TimeoutInterval;
    while (!isFileOpen(filename, TRUE) && j < seconds) { 
       Sleep (Common->TimeoutInterval);
       j+=Common->TimeoutInterval;
	}
    Sleep(Common->TimeoutInterval); // wait a bit to make sure the complete file is loaded        

	if (j>=seconds && GoatCode.CompareNoCase("virus") == 0 
		&& AppName == "Excel") { //file may be hidden
		// file may be hidden
	  PostMessage (AppHwnd, WM_COMMAND, Common->UnhideWP, 0); //unhide
	  Sleep(Common->TimeoutInterval); //give it a bit of time
	  if (isFileOpen(filename, TRUE)) //try again
		  j = seconds - 1; //let's cheat a bit

    }
	 CurrentlyOpenedDocs.AddTail(filename);
     if (j < seconds) {
       // check for a possible name change and update real name if necessary

      CheckActiveGoatNameChange (GoatCode);
      return Err_OK;
	 } else if (isAppRunning()) {
         Error.ReportError("FOpen failed : %s was not opened. (%s)",
	        filename, GoatCode);
         return Err_TimeOut;
	 } else {
         Error.ReportError("FOpen failed : the application is not running. (%s)",
		    GoatCode);
         return Err_App_Not_Running;
	 }  
  
   return Err_Unknown; // Shouldn't get here
}

/*
 * process MSAVE command - used WM_COMMAND or keyboard to save file
 *  GoatCode - code of the goat to open
 *  IPCmode - WM_COMMAND if IPC_WM; keyboard if IPC_KS
 */
int RCCmd::FSave(CString GoatCode, CString NewGoatCode, int IPCmode) 
{

  int  isNew;
  long totaltime;
  int Timeout;
  HWND hwnd;
  char *s;
  RCFile *G, *NewG;
  CString CurrentDocName, filename;
  int j;
  int ret;

  if (!isAppRunning()) {
    Error.ReportError("FSave failed : the application is not running. (%s)",
       GoatCode);
    return Err_App_Not_Running;
  }

  if (AppName == "Access") { // we don't support save for Access
      Error.ReportError("FSAVE failed : the command is not supported for Access");
	  return RCCmdFileError;
  }

  ReadCurrentDocName(CurrentDocName);
  ret = CheckActiveGoatNameChange(GoatCode);

  G = GoatsList-> FindFileByCode(GoatCode);
  if ((ret == Err_Not_Same_Names) || (ret == Err_DroppedFile)) {
     Error.ReportError("FSave failed : the active document is: %s instead of %s. (%s)",
          CurrentDocName, G->RealName, GoatCode);
      return Err_Target_Doc_Not_Active;
  }
  if (G->isVirusCopy) {
     Error.ReportError("FSave warning: the active document is the virus. (%s)",
       GoatCode);
  }


   // Display Save dialog
    if (WaitForInputIdle(hProcess, Common->WaitIdleTime) != 0)
	{
		Error.ReportError("FSave failed: the application is busy");
		return Err_TimeOut;
	}

    if (IPCmode == IPC_KS) {
        SetForegroundWindow (CurrentHwnd);
        Keys.SendKeys (Common->SaveShortcut, AppHwnd);  //send keys to display Save dialog
	} else if (IPCmode == IPC_WM)
        PostMessage(CurrentHwnd, WM_COMMAND, Common->SaveWP, 0);
    
    Sleep(2*Common->TimeoutInterval); // give the command time to complete
    // check if the file has just been created
	 s = new char[G->RealName.GetLength()+1];
	 strcpy(s, G->RealName);
     if (strnicmp(s, (LPCTSTR)Common->DocumentLabel, Common->DocumentLabel.GetLength()) == 0
		 || strnicmp(s, Common->DocLabel, Common->DocLabel.GetLength()) == 0 
         || strnicmp(s, Common->TemplateLabel, Common->TemplateLabel.GetLength())==0) 
      isNew = 1;
    else
      isNew = 0;
    delete s;
    if (!isNew) {
        Timeout = 40*Common->TimeoutInterval;
		totaltime = 0;
        while (totaltime < Timeout && isAppRunning()
			&& CurrentHwnd != GetLastActivePopup(CurrentHwnd)) {
            totaltime += Common->TimeoutInterval;
			Sleep (Common->TimeoutInterval);
        }
        if (!isAppRunning()) {
            Error.ReportError("FSave failed : App is not running. (%s)",
			    GoatCode);
            return Err_App_Not_Running;
		}
        
              if (totaltime >= Timeout) {
			hwnd = GetLastActivePopup(CurrentHwnd);
            if (SetForegroundWindow(hwnd) != 0) {
                Keys.SendKeys ("[ESC]", hwnd); 
				Sleep(Common->TimeoutInterval);
            } else {
              Error.ReportError("FSave failed : unable to set App as foreground window. (%s)",
			     GoatCode);
              return Err_Unknown;
			}
		}
        
        Sleep(Common->TimeoutInterval); // wait a bit to make sure the complete file is completely written

        return Err_OK;
    } else {
        //a saveas dlgbox is going to appear
		if (NewGoatCode.IsEmpty()) { // will only happen if error in cmd file
			CloseApp(); // close the application
			Error.ExitError(RCCmdFileError,"Error: SaveAs filename is not specified. Check commands file");
		}
        NewG = GoatsList->FindFileByCode(NewGoatCode);
		if (NewG == NULL) {
          CloseApp(); // close the application
          Error.ExitError(RCCmdFileError, "FSave error: goat code %s not found", NewGoatCode);
		}
        filename = NewG->RealName;
        NewG->BackupName = G->BackupName;
        CString strTmpDir, strTmpFile;       
        Common->GetFileDir(filename, strTmpDir);
		Common->GetFileName(filename, strTmpFile);
		CString strTmp = strTmpDir;
		strTmp += "\\";
		strTmp += strTmpFile;
		CString saveasname(strTmp);
		saveasname += "~";
        
        j = 0; 
		Timeout = 10*Common->TimeoutInterval;
        while (GetAppDlg(Common->SaveAsDlgClassName, Common->SaveAsTitle) == NULL
			&& j < Timeout 
			&& isAppRunning() == TRUE) {
            Sleep (Common->TimeoutInterval);
			j+=Common->TimeoutInterval;
        }
        if (j >= Timeout) {
            Error.ReportError("FSave failed : the dialog box did not appear. (%s)",
			   GoatCode);
            return Err_Missing_DlgBox;
        } else if (!isAppRunning()) {
            Error.ReportError("FSave failed : App is not running. (%s)",
			    GoatCode);
            return Err_App_Not_Running;
        }
        hwnd = GetAppDlg(Common->SaveAsDlgClassName, Common->SaveAsTitle);
        if (SetForegroundWindow(hwnd) == 0) 
            if (!isAppRunning()) {
                Error.ReportError("FSave failed : App is not running. (%s)",
				   GoatCode);
                return Err_App_Not_Running;
            } else {
                Error.ReportError("FSave failed : unable to bring App to the foreground. (%s)",
				      GoatCode);
                return Err_Unknown;
            }
                   
        
        Keys.SendKeys(saveasname,hwnd); // send the new file name
        j = 0;
        while (GetAppDlg(Common->SaveAsDlgClassName, Common->SaveAsTitle) != NULL
			&& j < Timeout) {
            Sleep (Common->TimeoutInterval);
			j+=Common->TimeoutInterval;
        }
		// see if the dialog is still there
        if (j >= Timeout) {
             Keys.SendKeys ("[ESC]", hwnd); // get rid of the dialog
			 Sleep(2*Common->TimeoutInterval); // give time to process the keystrokes
             Error.ReportError("FSave failed : the dialog box does not close. (%s)",
			   GoatCode);
             return Err_DlgBox_Not_Close;
		}
                
        
		// wait for operation to complete - new file name should be displayed
        j = 0;
        while (isFileOpen(filename, TRUE) == FALSE && j < Timeout) {
             Sleep (Common->TimeoutInterval); 
			 j+=Common->TimeoutInterval;
        }
        if (j >= Timeout) {
             Error.ReportError ("FSave failed : failed to see the new file before timeout. (%s)",
			      GoatCode);
             return Err_TimeOut;
		}
                        
      	POSITION pos = FindDocOnList(G->RealName);
        if (pos !=NULL)
	       CurrentlyOpenedDocs.RemoveAt(pos); //removed it
	    CurrentlyOpenedDocs.AddTail(NewG->RealName);
 

        Sleep (Common->TimeoutInterval/2);
        CheckActiveGoatNameChange (NewGoatCode);
        return Err_OK;
    }
	return Err_Unknown; //shouldn't get here
}

/*
 * process MSAVEAS command - used WM_COMMAND or keyboard to save file
 *  GoatCode - code of the goat to open
 *  IPCmode - WM_COMMAND if IPC_WM; keyboard if IPC_KS
 */
int RCCmd::FSaveAs(CString OldGoatCode, CString NewGoatCode, int IPCmode) 
{

   int j;
   double totaltime;
   int Timeout, SleepTime;
   HWND hwnd;
   CString CurrentDocName;
   CString filename;
   RCFile *NewG, *OldG;
   int ret;
 

   if (AppName == "Access") { // we don't support save for Access
      Error.ReportError("FSAVEAS failed : the command is not supported for Access");
	  return RCCmdFileError;
   }

   ret = ReadCurrentDocName(CurrentDocName);
   if (ret == Err_OK) {

     j = CheckActiveGoatNameChange(OldGoatCode);
     if ((j == Err_Not_Same_Names) || (j == Err_DroppedFile)) {
        Error.ReportError ("FSaveAs failed: the target doc is not active. (%s to %s).",
	       OldGoatCode, NewGoatCode);
        return Err_Target_Doc_Not_Active;
	 }
       
     OldG = GoatsList->FindFileByCode(OldGoatCode);
	 NewG = GoatsList->FindFileByCode(NewGoatCode);
     if (NewG == NULL || OldG == NULL) {
       CloseApp(); // close the application
       Error.ExitError(RCCmdFileError, "FSaveAs error: one of the goat codes is not found");
	 }
     filename = NewG->RealName;
     NewG->BackupName = OldG->BackupName;
     NewG->isVirusCopy = OldG->isVirusCopy;
     if (OldG->isVirusCopy)
        Error.ReportError ("FSaveAs warning: the virus is the active document. (%s to %s).",
		     OldGoatCode, NewGoatCode);  
     
     Timeout = 10*Common->TimeoutInterval; 
     totaltime = 0;
	 SleepTime = Common->TimeoutInterval/5;  
     while (totaltime < Timeout && isAppRunning() && 
		 CurrentHwnd != GetLastActivePopup(CurrentHwnd)) {
        Sleep (SleepTime);
        totaltime += SleepTime;
	 }
   
	 // display the dialog
     if (!isAppRunning()) {
       Error.ReportError( "FSaveAs failed : App is not running (%s to %s).",
	     OldGoatCode, NewGoatCode);
       return Err_App_Not_Running;
	 }  else if (totaltime >= Timeout) {
       return Err_Unknown;
   
	 } else if (IPCmode == IPC_KS) {
	   if (SetForegroundWindow(CurrentHwnd) == 0) {
		   if (!isAppRunning()) {
              Error.ReportError("FSaveAs failed : App is not running. (%s to %s).",
			     OldGoatCode,NewGoatCode);
              return Err_App_Not_Running;
		   } else {
              Error.ReportError( "FSaveAs failed : could not bring App to the foreground. (%s to %s).",
			    OldGoatCode, NewGoatCode);
              return Err_Unknown;
		   }

	   } else {
          Keys.SendKeys(Common->SaveAsShortcut, CurrentHwnd);  // send shortcut to display
		                                              // the dialog
	   }
	 } else if (IPCmode == IPC_WM)
      PostMessage (CurrentHwnd, WM_COMMAND, Common->SaveAsWP, 0);
   
	 // send in the new file name
	 CString strTmpDir, strTmpFile;
	 Common->GetFileDir(filename, strTmpDir);
	 Common->GetFileName(filename, strTmpFile);
	 CString strTmp(strTmpDir);
	 strTmp += "\\";
	 strTmp += strTmpFile;
	 CString saveasname(strTmp);
     saveasname += "~";   // append ENTER key to the file name
     
	 // wait until the dialog is displayed
     Timeout = 10*Common->TimeoutInterval;   
     totaltime = 0;
     while (GetAppDlg(Common->SaveAsDlgClassName, Common->SaveAsTitle) == NULL
		 && totaltime < Timeout && isAppRunning()) {
        Sleep (Common->TimeoutInterval);
		totaltime += Common->TimeoutInterval;
	 }
    
	 // see if the dialog has appeared
	 if (totaltime >= Timeout) {
       Error.ReportError ("FSaveAs failed : the dialog box does not appear.(%s to %s).",
	    OldGoatCode, NewGoatCode);
       return Err_Missing_DlgBox;

	 } else if (!isAppRunning()) {
        Error.ReportError("FSaveAs failed : App is not running. (%s to %s).",
	     OldGoatCode, NewGoatCode);
        return Err_App_Not_Running;
	 }
  
         
     hwnd = GetAppDlg(Common->SaveAsDlgClassName, Common->SaveAsTitle);
     if (SetForegroundWindow(hwnd) == 0) {
  		 if (!isAppRunning()) {
           Error.ReportError("FSaveAs failed : App is not running. (%s to %s).",
		      OldGoatCode, NewGoatCode);
           return Err_App_Not_Running;
		 } else {
           Error.ReportError( "FSaveAs failed : could not bring App to the foreground. (%s to %s).",
		     OldGoatCode, NewGoatCode);
            return Err_Unknown;
		 }		 
 	 }
            
     Keys.SendKeys(saveasname, hwnd); // send in the name
         
	 // wait for the dialog to disappear
     j = 0;
	 Timeout = 30*Common->TimeoutInterval;  
	 
     while (GetAppDlg(Common->SaveAsDlgClassName, Common->SaveAsTitle) != NULL && 
		 j < Timeout) {
        Sleep (Common->TimeoutInterval);
		j+=Common->TimeoutInterval;
	 }
        
	 // if it is still there - get rid of it
     if (j >= Timeout) {
        Keys.SendKeys ("[ESC]", hwnd); // cancel the dialoog
		Sleep(2*Common->TimeoutInterval); 
        Error.ReportError("FSaveAs failed : the dialog box did not close. (%s to %s).",
	       OldGoatCode, NewGoatCode);
        return Err_DlgBox_Not_Close;
     }
   

	 // wait for the file to be open under the new name
      j = 0; 
	 Timeout = 10*Common->TimeoutInterval; 
 
     while (!isFileOpen(filename, TRUE) && j < Timeout) {
        Sleep (Common->TimeoutInterval);
		j+=Common->TimeoutInterval;
	 }

     if (j >= Timeout) {
       Error.ReportError("FSaveAs failed : could not detect the new file before timeout. (%s to %s).",
	      OldGoatCode, NewGoatCode);
       return Err_TimeOut;
     }
	 POSITION pos = FindDocOnList(OldG->RealName);
     if (pos !=NULL)
	    CurrentlyOpenedDocs.RemoveAt(pos); //removed it
	 CurrentlyOpenedDocs.AddTail(NewG->RealName);
 
     Sleep (Common->TimeoutInterval);
     CheckActiveGoatNameChange (NewGoatCode);
	  // replace old doc on the currently opened list with the new one

     return Err_OK;
   } else {
     Error.ReportError("FSaveAs failed : error %d reading the active document name. (%s)",
       ret,OldGoatCode);
          
     return Err_Unknown;
   }
   return Err_Unknown; //shouldn't get here
}

/*
 * process STARTSHOW command - used WM_COMMAND or keyboard to show presentation
 *  GoatCode - code of the goat to show
 *  IPCmode - WM_COMMAND if IPC_WM; keyboard if IPC_KS
 */
int RCCmd::StartShow(CString GoatCode, int IPCmode) 
{

  RCFile *G;
  CString CurrentDocName, filename;
  int ret;

  if (AppName != "PowerPoint") { // we support show for PowerPoint only
      Error.ReportError("StartShow failed : the command is supported for PowerPoint only");
	  return RCCmdFileError;
  }

  if (!isAppRunning()) {
    Error.ReportError("StartShow failed : the application is not running. (%s)",
       GoatCode);
    return Err_App_Not_Running;
  }

  ReadCurrentDocName(CurrentDocName);
  ret = CheckActiveGoatNameChange(GoatCode);

  if (ret == Err_Not_Same_Names) {
     G = GoatsList-> FindFileByCode(GoatCode);
     Error.ReportError("StartShow failed : the active document is: %s instead of %s. (%s)",
          CurrentDocName, G->RealName, GoatCode);
      return Err_Target_Doc_Not_Active;
  }


   // Display Save dialog
    WaitForInputIdle(hProcess, Common->WaitIdleTime);
    if (IPCmode == IPC_KS) {
        SetForegroundWindow (CurrentHwnd);
        Keys.SendKeys (Common->SlideShowShortcut, AppHwnd);  //send keys to display Save dialog
	} else if (IPCmode == IPC_WM)
        PostMessage(CurrentHwnd, WM_COMMAND, Common->SlideShowWP, 0);
    
    Sleep(2*Common->TimeoutInterval); // give the command time to complete


	WaitForInputIdle(hProcess, Common->WaitIdleTime);

    if (!isAppRunning()) {
        Error.ReportError("StartShow failed : App is not running. (%s)", GoatCode);
        return Err_App_Not_Running;
	}
      
	if (!IsViewingSlideShow())
	{
		Error.ReportError("StartShow failed : timeout or OLE error");
        return Err_Unknown;
	}

    return Err_OK;
}


/*
 * process LeftClick command
 *  GoatCode - code of the goat to click on
 *  IPCmode - WM_COMMAND if IPC_WM; keyboard if IPC_KS
 */
int RCCmd::CenterLeftClick(CString GoatCode) 
{

  RCFile *G;
  CString CurrentDocName, filename;
  int ret;
//  POINT p;

  if (!isAppRunning()) {
    Error.ReportError("CenterLeftClick failed : the application is not running. (%s)",
       GoatCode);
    return Err_App_Not_Running;
  }

  if (AppName != "PowerPoint") { // we support show for PowerPoint only
      Error.ReportError("CenterLeftClick failed : the command is supported for PowerPoint only");
	  return RCCmdFileError;
  }

  ReadCurrentDocName(CurrentDocName);
  ret = CheckActiveGoatNameChange(GoatCode);

  if (ret == Err_Not_Same_Names) {
     G = GoatsList-> FindFileByCode(GoatCode);
     Error.ReportError("CenterLeftClick failed : the active document is: %s instead of %s. (%s)",
          CurrentDocName, G->RealName, GoatCode);
      return Err_Target_Doc_Not_Active;
  }

	WaitForInputIdle(hProcess, 10 * Common->TimeoutInterval);

	//the mouse cursor does not appear on entering show mode if the position of the mouse does not move
	//macros are not triggered if the mouse cursor is not visible
    mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE, 16384, 16384, 0, 0);
    Sleep(Common->TimeoutInterval); // give the command time to complete
	WaitForInputIdle(hProcess, 5 * Common->TimeoutInterval);
    mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE, 32768, 32768, 0, 0);
    Sleep(Common->TimeoutInterval); // give the command time to complete
	WaitForInputIdle(hProcess, 5 * Common->TimeoutInterval);

    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	WaitForInputIdle(hProcess, Common->TimeoutInterval);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    Sleep(Common->TimeoutInterval); // give the command time to complete
	WaitForInputIdle(hProcess, 60 * Common->TimeoutInterval);
        
    return Err_OK;
}


/*
 * Run the slide show (Powerpoint only)
 *  GoatCode - code of the goat to view
 *  IPCmode - WM_COMMAND if IPC_WM; keyboard if IPC_KS
 */
int RCCmd::Show(CString GoatCode, int IPCmode)
{
	int retc;
	/* start the show first */
     if ((retc = StartShow(GoatCode, IPCmode)) != Err_OK)
	 {
         Error.ReportError("Unable to start the show");
		 return retc;
	 }

	 int number_slides = GetSlideCount(); // get number of slides
	 int i;
	 for (i=0;i<number_slides;i++) {
       retc = CenterLeftClick(GoatCode);
	   if (retc != Err_OK) return retc;
      
	 }
     return Err_OK;     
}

/*
 * Process the WAITAPPIDLE command 
 */
void RCCmd::WaitIdle(int waittime)
{
    WaitForInputIdle(hProcess, waittime);
}

/*
 * determine if the specified file is currently opened
 * filename - full name of the file
 * AddDot - TRUE -- need to add . to name before comparizon
 */
BOOL RCCmd::isFileOpen(CString filename, BOOL AddDot) 
{
   CString AppTitle;
   CString s;
   CString CurDocName;
   int ret;

   if (!isAppRunning()) 
        return FALSE;
       
    Common->GetFileName(filename,s);
    
    ret = ReadCurrentDocName(CurDocName);
	if (ret == Err_Macro_Running) {
      Sleep (Common->TimeoutInterval/2);
      ret = ReadCurrentDocName(CurDocName);
    }

    if (AddDot == TRUE) 
       s += ".";

    s.MakeUpper();
	//if returned document length exceed s length then
	// the returned document may contain full name with
	// extension. In this case we need to add . to s and
	// truncate the document name to length of s+1 to allow
	// for the dot, then add dot. This is to make sure that
	// the comparisons work correctly when current document
	// name contains s
    CurDocName = CurDocName.Left(s.GetLength());
      	
	CurDocName.MakeUpper();
    if (ret == Err_OK && CurDocName == s) 
      return TRUE;
    else 
      return FALSE;
	
    
}

/*
 * process SENDTEXT command - currently only used in Word command files
 * though will also work in Excel
 *    GoatCode - code of the goat where we need to send text
 *     Txt - text to be sent
 */
int RCCmd::TypeText(CString GoatCode, CString Txt) 
{
  RCFile *G;
  CString s;
  CString CurDocName, TargetDocName;
  int ret;

   G = GoatsList->FindFileByCode(GoatCode);
   Common->GetFileName(G->RealName, TargetDocName);
   CString strTmp; 
   Common->GetFileExt(G->RealName,strTmp);
   if (!strTmp.IsEmpty())
     TargetDocName = TargetDocName + "." + strTmp;
   
   SetForegroundWindow (CurrentHwnd);

   ret = ReadCurrentDocName(CurDocName);
   if (ret == Err_OK) {
	   if (CurDocName.CompareNoCase(TargetDocName) == 0) { 
           Keys.SendKeys (Txt,CurrentHwnd); // send the specified text to the dialog
           Sleep(2*Common->TimeoutInterval); // wait for the dialog to process the keys
           WaitForInputIdle(hProcess, 2000);
           return Err_OK;
	   } else {
         Error.ReportError("TypeText failed : the active doc is %s instead of %s.(%s)",
	            CurDocName, TargetDocName, GoatCode);
         return Err_Target_Doc_Not_Active;
       }
   } else {
      Error.ReportError("TypeText failed : error %d reading the active document name. (%s)",
        ret, GoatCode);
      return Err_Unknown;
   }
}

/*
 *  Determine the handle of the specified dialog
 *    ClassName - class name of the dialog
 *    Title - dialog title
 */
HWND RCCmd::GetAppDlg(CString ClassName, CString Title) 
{
   HWND hwnd;
   HWND PrevChild;

   
   PrevChild = NULL;
   while ((hwnd = FindWindowEx(NULL, PrevChild, ClassName, Title))!=0) {
      
      if (GetParent(hwnd) == CurrentHwnd) 
            return hwnd;
           
      PrevChild = hwnd;
   }
   return NULL;
}

/*
 * Determine the name of the document displayed by the application
 * Output: CurrentDocName - name of the document
 */
int RCCmd::ReadCurrentDocName(CString &CurrentDocName)
{
   CString s;

   if (WaitForInputIdle(hProcess, Common->WaitIdleTime) != 0) {
      Error.ReportError("The application is busy, maybe unable to determine name");
	  WaitForInputIdle(hProcess, 2000); //give it a bit more time
   }	  
   
   try {
     if (AppName == "Excel") { //excel
	  _Workbook book = m_app_ex.GetActiveWorkbook();
      CurrentDocName = book.GetName();
	 } else if (AppName == "PowerPoint") {
		_Presentation pres = m_app_pp.GetActivePresentation();
	    CurrentDocName = pres.GetName();
	 } else if (AppName == "Word") { //word
      CString s = m_app_wd.FileName(COleVariant((short) 0));
	  Common->GetFileWName(s,CurrentDocName); /*for now, in future might just compare full names */
	 } else { // Microsoft Access
	   LPDISPATCH lpCurrentDb = m_app_acc.CurrentDb(); // get pointer to current db object

	   DISPID disp;   //Disp id of the Name property
	   HRESULT hr;

	   OLECHAR FAR* szFunction = OLESTR("Name");  // convert property name to OLESTR
	   // get disp id of the property
	    hr = lpCurrentDb->GetIDsOfNames(IID_NULL, &szFunction, 1, 
		                 LOCALE_SYSTEM_DEFAULT, &disp);
	   if (FAILED(hr))  // unable to get the id
	   {
	 	  Error.ReportError("Unable to determine document name: GetIDsOfNames failed");
	   } else {    // get value of the property
		COleDispatchDriver cCurrentDb; // OLE interface class for lpCurrentDb IDispatch
		cCurrentDb.AttachDispatch(lpCurrentDb, TRUE); // attach to db IDISPATCH ptr
		CString strFullDocName; // full name of the current db
		cCurrentDb.GetProperty(disp,VT_BSTR,(void *) &strFullDocName); // get name
		Common->GetFileWName(strFullDocName, CurrentDocName);
	   }
	 }
	 Error.ReportError( "Active Document: %s", CurrentDocName);
   } catch (COleDispatchException *pCE)
   {
		if ((AppName == "PowerPoint" || AppName == "Access") && pCE->m_wCode == 0) // no active presentation
		{
			CurrentDocName = "";
			Error.ReportError("Active Document: "); // no active document
			return Err_No_Doc_Open;
		}
	   	CString s;

		Error.ReportError("Error %d: %s",pCE->m_wCode,pCE->m_strDescription);
		pCE->Delete();
		return Err_OleError;
	} catch (COleException *pCE) {
            SCODE sc = pCE->m_sc;
			Error.ReportError("ReadCurrentDocName failed: OLE exception %d",SCODE_SEVERITY(sc));
			pCE->Delete();
	}  catch (char *str) {
		Error.ReportError("caught some other exception during ReadCurrentDocName %s",str);
	}   
	if (AppName == "Word"  && strstr(Common->OpenDlgClassName, "9.0")) { // get current application handle
		  HWND hwnd;
          HWND PrevChild;
          char temp[100];
	  
          CString strTst = CurrentDocName + " - Microsoft Word";
          PrevChild = 0;
  //        while ((hwnd = FindWindowEx(AppHwnd, PrevChild, MainAppWinName, NULL))!=0) {
		  while((hwnd = FindWindowEx(0,PrevChild,MainAppWinName, 0)) != 0) {
          
             GetWindowText(hwnd, temp, 100);
			 if (strcmpi(temp, strTst) == 0) // current document
				 break;
             PrevChild = hwnd;
		  }
          if (hwnd == NULL)
			  CurrentHwnd = AppHwnd;
		  else
			  CurrentHwnd = hwnd;
	} else {
		CurrentHwnd = AppHwnd;
	}
   return Err_OK;
}

// see if dropped file is opened unexpectedly and close it
int RCCmd::CloseUnexpectedFile()
{
   CString s;
   if (WaitForInputIdle(hProcess, Common->WaitIdleTime) != 0) 
	   Error.ReportError("the application is busy, may be unable to close file");
  // get full name of currently opened file 
   if (AppName == "Access") { //access viruses do not drop files
	   return Err_OK;
   }
   try {
     if (AppName == "Excel") { //excel
	  _Workbook book = m_app_ex.GetActiveWorkbook();
      s = book.GetFullName();
	 } else if (AppName == "PowerPoint") { // Power Point
	  _Presentation pres = m_app_pp.GetActivePresentation();
	  s = pres.GetFullName();
	 } else { //word
      s = m_app_wd.FileName(COleVariant((short) 0));
	 }
   } catch (COleDispatchException *pCE)
   {
		CString s;
		Error.ReportError("Error %d: %s",pCE->m_wCode,pCE->m_strDescription);
		pCE->Delete();
		return Err_OleError;
	} catch (COleException *pCE) {
            SCODE sc = pCE->m_sc;
			Error.ReportError("Unable to get file name: OLE exception %d",SCODE_SEVERITY(sc));
			pCE->Delete();
	}  catch (char *str) {
		Error.ReportError("caught some other exception getting file name (OLE) %s",str);
	}   
   if (s.IsEmpty()) return Err_OK;

   // see if the file should indeed be opened
            
   // see if file is unexpected and is not listed as dropped
   POSITION pos = FindDocOnList(s);
   if (pos == NULL) { //file not found on list
      Error.ReportError("closing unexpected file %s\n", s);
	  OLECurrentFileClose();
   }	  
  
   return Err_OK;
}

/*
 * checks if the name or the extension of the active document has changed
 * and updates goatslist if necessary
 */
int RCCmd::CheckActiveGoatNameChange(CString GoatCode) 
{

  RCFile *G;
  CString CurrentDocName;
  CString GoatName, currentGoatName;
  CString GoatExt, currentGoatExt;
  int ret;

  //get rid of unexpectedly opened file if any is open:
  // make an exception for files dropped in startup directory or known
  // dropped files
  if (!MaybeDroppedFile()) //if not knowingly opened file
	  CloseUnexpectedFile();

  // compute the necessary data first
   G = GoatsList->FindFileByCode(GoatCode);
   if (G == NULL)
	   ret = Err_Unknown; // should not happen
   Common->GetFileName(G->RealName, GoatName);
   Common->GetFileExt(G->RealName,GoatExt);

// do the comparison
   ret = ReadCurrentDocName(CurrentDocName);
   CurrentDocName.MakeUpper();
   Common->GetFileName(CurrentDocName, currentGoatName);
   Common->GetFileExt(CurrentDocName, currentGoatExt);
  
   GoatName.MakeUpper();
   GoatExt.MakeUpper();
   if (currentGoatName != GoatName) {
	     if (MaybeDroppedFile())
		   ret = Err_DroppedFile;
		 else
		   ret = Err_Not_Same_Names;
   } else if (currentGoatExt == GoatExt) {
       ret = Err_No_Change;
   } else {
 		CString newRealName;
        Common->GetFileDir(G->RealName, newRealName);
		char temp[MAXPATH];
		strcpy(temp, newRealName);
        Common->AppendPath(temp, CurrentDocName);

        G->RealName = temp; 
         ret = Err_Ext_Changed;
   }

  
  return ret;
}

/* 
 * check if the currently active file could be dropped by the virus
 * this will happen if 1. the file name is not on the goat list, but
 * is located in the startup directory
 * 2. the file was added to the goat list under the code "dropped#"
 */
bool RCCmd::MaybeDroppedFile()
{
  CString s;
  RCFile *G;

  if (AppName == "Access")  // access viruses do not drop files
	  return FALSE;
  // get full name of the currently active file
  WaitForInputIdle(hProcess, Common->WaitIdleTime);
  try {
     if (AppName == "Excel") { //excel
	    _Workbook book = m_app_ex.GetActiveWorkbook();
         s = book.GetFullName();
	 } else if (AppName == "PowerPoint") {
		 _Presentation pres = m_app_pp.GetActivePresentation();
		 s = pres.GetFullName();
	 } else { //word
        s = m_app_wd.FileName(COleVariant((short) 0));
	}
  } catch (COleDispatchException *pCE)
  {
     CString s;
	 Error.ReportError("Error %d: %s",pCE->m_wCode,pCE->m_strDescription);
	 pCE->Delete();
	 return Err_OleError;
  } catch (COleException *pCE) {
     SCODE sc = pCE->m_sc;
	 Error.ReportError("Error getting file name: OLE exception %d",SCODE_SEVERITY(sc));
		pCE->Delete();
		return FALSE;
  }  catch (char *str) {
		Error.ReportError("caught some other exception while getting file name",str);
		return FALSE;
  }	  
   G = GoatsList->FindFileByRealName(s);
   
   if (G == NULL) // file is not on the list
   {
	  CString strPath;
	  Common->GetFileDir(s, strPath); // get directory name
	  if (strPath.CompareNoCase(Common->VarList["STARTUPDIR"]) == 0) // startup directory
           return TRUE;
   }
   else if (G->Code.Find("dropped") == 0) // file is on the list as dropped file
       return TRUE;
   
   return FALSE;

}
/*
 * check if application is running, clean up OLE if it is not
 */
bool RCCmd::isAppRunning() 
{
   unsigned long exitcode;
   if (GetExitCodeProcess(hProcess, &exitcode))
      if (exitcode == STILL_ACTIVE) return TRUE;
      else {
        // clean up  
          COleDispatchDriver *m_app;
       if (AppName== "Excel")
     	  m_app = &m_app_ex;
	   else if (AppName == "PowerPoint")
		  m_app = &m_app_pp;
	   else if (AppName == "Access")
		  m_app = &m_app_acc;
       else
	      m_app = &m_app_wd;

        m_app->ReleaseDispatch();  
	    return FALSE;
	  } 

   else {
	   int rc = GetLastError();
       Error.ReportError("Error trying to get the exit code of the application.");
          return TRUE;
   } 
}

/*
 * Close the application
 */
void RCCmd::CloseApp()
{
   COleDispatchDriver *m_app;
       
   if (AppName== "Excel")
	   m_app = &m_app_ex;
   else if (AppName == "PowerPoint")
	   m_app = &m_app_pp;
   else if (AppName == "Access")
	   m_app = &m_app_acc;
   else
	   m_app = &m_app_wd;

   if (isAppRunning())   // FExit seems to take too much time with OLE
   {
	   try {
	     if (AppName == "Excel") {
     	   m_app_ex.Quit();
         } else if (AppName == "PowerPoint") {
		   m_app_pp.Quit();
		 } else if (AppName == "Access") {
		   m_app_acc.Quit(acSaveYes);
		 } else {
	       m_app_wd.AppClose(COleVariant("Microsoft Word"));
		 }
	   } catch (COleDispatchException *pCE) 
	   {
		CString s;
		Error.ReportError("Error %d: %s",pCE->m_wCode,pCE->m_strDescription);
		pCE->Delete();
        TerminateProcess (hProcess, 1);
	   } catch (COleException *pCE) {
            SCODE sc = pCE->m_sc;
			Error.ReportError("Quit failed: OLE exception %d",SCODE_SEVERITY(sc));
			pCE->Delete();
	   }  catch (char *str) {
		Error.ReportError("caught some other exception during Quit %s",str);
	   }	   
	   m_app->ReleaseDispatch();
   } else {
     m_app->ReleaseDispatch();  
	 return;
   }
   Sleep(2*Common->TimeoutInterval); // in case of unexpected popup
   if (isAppRunning()) // failed to close
	   TerminateProcess(hProcess, 1);
   Sleep(Common->TimeoutInterval);
}

// Find document on list of open documents
// Use this instead of CStringList Find() function
// because we want case-insenstive compare
// and because docs with different extensions match
POSITION RCCmd::FindDocOnList(CString DocFullName)
{
	
	POSITION pos, prev_pos; 
	CString strTmp, strDocNoExt, strTmpNoExt;

 
	int i = DocFullName.ReverseFind('.');
	if (i != -1) 
		strDocNoExt = DocFullName.Left(i);
	else
		strDocNoExt = DocFullName;
	for (pos = CurrentlyOpenedDocs.GetHeadPosition(); pos != NULL;)
	{
		
	   prev_pos = pos;
       strTmp = CurrentlyOpenedDocs.GetNext(pos);

	   int i = strTmp.ReverseFind('.'); 
       if (i != -1) 
		   strTmpNoExt = strTmp.Left(i);
	   else
	       strTmpNoExt = strTmp;
	   if (strTmpNoExt.CompareNoCase(strDocNoExt) == 0)
	      return prev_pos;
	}
	return NULL;
}

/*
* Determine if the application is busy
*/
bool RCCmd::isApplicationBusy()
{

	CString CurrentDocName;
	g_application_busy = FALSE; // assume not busy at the moment
    ReadCurrentDocName(CurrentDocName);
	return g_application_busy;
}

/*
* Get the number of slides in the current presentation
*/
long RCCmd::GetSlideCount()
{
    _Presentation pres = m_app_pp.GetActivePresentation();
	Slides slides = pres.GetSlides();
    return slides.GetCount();
}

/*
* Check if Power Point is in a View mode
* return TRUE if it is in a View mode, FALSE if it is not
*   or if we are unable to determine it
*/
bool RCCmd::IsViewingSlideShow()
{
	// first get the presentation
	_Presentation pres;
	try {
     pres = m_app_pp.GetActivePresentation();
    } catch (COleDispatchException *pCE)  // OLE dispatch error
	{
         
        Error.ReportError("Error %d: %s",pCE->m_wCode,pCE->m_strDescription);
		pCE->Delete();
		return FALSE;
	}	catch (COleException *pCE) { // OLE error
            SCODE sc = pCE->m_sc;
			Error.ReportError("OLE exception %d",SCODE_SEVERITY(sc));
			pCE->Delete();
			return FALSE;
	}  catch (char *str) { // some other error
		Error.ReportError("exception during OLE call: %s", str);
		return FALSE;
	}

	
	// now see if slide show is running
	// if it is running GetSlideShowWindow should return a pointer
	// otherwise 
	try {
	  LPDISPATCH slideshow = pres.GetSlideShowWindow();
	}  catch (COleDispatchException *pCE) 
	{
          
        Error.ReportError("OLE return code %d: %s",pCE->m_wCode,pCE->m_strDescription);
		pCE->Delete();
		return FALSE;
	}	catch (COleException *pCE) {
            SCODE sc = pCE->m_sc;
			Error.ReportError("OLE exception %d",SCODE_SEVERITY(sc));
			pCE->Delete();
			return FALSE;
	}  catch (char *str) {
		Error.ReportError("exception during OLE call: %s", str);
		return FALSE;
	}
	
	return TRUE;
}
