/*******************************************************************
*                                                                  *
* File name:     rccmd.h                                           *
*                                                                  *
* Description:   RCCmd class definition file                       *
*                                                                  *
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
* Function:      class definition for rc command processor         *
*                                                                  *
*******************************************************************/
#ifndef RCCMD_H
#define RCCMD_H
class RCCmd {
   public:
	   RCCmd (RCGoatList *rcGoatsList,RCCommon *rcCommon, 
		   CString AppName, int AppVersion);
       int   OLEDocMaximize();
       int OLEGoto(CString GoatCode, CString Position);
       int OLEFClose(CString GoatCode) ;
	   int OLECurrentFileClose();
	   int OLEFNew(CString GoatCode, CString PlainDocPath);
       int OLEFOpen(CString GoatCode);
	   int OLEFileOpen(CString FileName);
       int OLEFSave(CString GoatCode);
       int OLEFSaveAs(CString OldGoatCode, CString NewGoatCode) ;
	   int RCCmd::OLEFSaveAsFormat(CString GoatCode, CString NewGoatCode, int Format);
       int FClose(CString GoatCode, int IPCmode);
       int FNew(CString GoatCode, int IPCmode, CString PlainDocPath);
       int FOpen(CString GoatCode, int IPCmode);
       int FSave(CString GoatCode, CString NewGoatCode, int IPCmode) ;
      int FSaveAs(CString OldGoatCode, CString NewGoatCode, int IPCmode);
       int StartShow(CString GoatCode, int IPCmode) ;
       int CenterLeftClick(CString GoatCode) ;
	   int Show(CString GoatCode, int IPCmode);
	  int TypeText(CString GoatCode, CString Txt);
	  int ReadCurrentDocName(CString &CurrentDocName);
	  void StartApp(CString FirstFile);
	  void CloseApp();
	  int CloseUnexpectedFile();
	  bool MaybeDroppedFile();
	  bool isAppRunning();
	  void WaitIdle(int waittime);
      long GetSlideCount();
	  bool IsViewingSlideShow();
  private:
	  COleVariant covTrue, covFalse, covOptional;
	  _Application m_app_ex; // Excel application
	  WordBasic m_app_wd; //Word application
	  _WApplication m_app_wdapp;
	  _PPTApplication m_app_pp;
	  _ACCApplication m_app_acc;
      HWND AppHwnd; 
	  HWND CurrentHwnd;
	  HANDLE hProcess;

      RCError Error;
      RCGoatList *GoatsList;
	  RCCommon *Common;
      RCKeys Keys;
      CString AppName;
	  CString OpenDlgClassName;
      CString SaveAsDlgClassName; 
      CString DeskClassName; 
      CString DocClassName;
	  CString MainAppWinName;
	  CStringList CurrentlyOpenedDocs;

      BOOL  isFileOpen(CString filename, BOOL AddDot);
	  HWND GetAppDlg(CString ClassName, CString Title);
	  int CheckActiveGoatNameChange(CString GoatCode);
	  POSITION FindDocOnList(CString DocFullName);
	  bool isApplicationBusy();

};
#endif
