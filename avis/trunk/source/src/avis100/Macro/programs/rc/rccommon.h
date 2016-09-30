/*******************************************************************
*                                                                  *
* File name:     rccommon.h                                        *
*                                                                  *
* Description:   RCCommon class definition file                    *
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
* Function:     class definition for common methods container      *
*                                                                  *
*******************************************************************/
#ifndef RCCOMMON_H
#define RCCOMMON_H
//Replaces some global declarations and utility functions
class RCCommon {
   public:
       RCCommon();
       CMapStringToString VarList;

	   char LangCfgFile[MAXPATH];
       int nokillpopup;
	   int TimeoutInterval;
	   int WaitIdleTime;
       char  GlobalPath[MAXPATH];
       CString DocMaximizeCommand;
	   CString SaveAsCommand;
       CString SaveCommand;
       CString OpenCommand;
       CString CloseCommand;
       CString ExitCommand;
       CString NewCommand;
       CString StartOfDocumentCommand;
       CString EndOfDocumentCommand;
       CString ToolsOptionsSaveCommand;
	   CString DocumentLabel;
       CString DocLabel;
       CString TemplateLabel;
       CString NewMoving;

	   CString OpenDlgClassName;
	   CString SaveAsDlgClassName;
	   CString DeskClassName;
	   CString DocClassName;
	   CString OleId;
       CString SeekAppPath;
	   CString AppSetupPath;
	   CString AppPath;

	   CString CommandsFile;
       CString GoatsListFile;
      
        CString PopupKillerPath;
       CString PopupKillerTimeOut;
       CString PopupKillerSleepTime;

       BOOL DBCSMode;

		   //WPARAMS to call application menus using windows messages
       long SaveAsWP;
       long SaveWP;
       long OpenWP;
       long CloseWP;
       long NewWP;
       long ExitWP;
	   long UnhideWP;
	   long SlideShowWP;


		  // Shortcuts to use keystroke commands
       CString SaveAsShortcut; 
       CString SaveShortcut;
       CString OpenShortcut;
       CString CloseShortcut;
       CString NewShortcut ;
       CString ExitShortcut; 
       CString SlideShowShortcut; 

       CString SaveAsTitle;
       CString OpenTitle;
       CString NewTitle;

       CStringList *ParseLine(char * Line, LPCTSTR Sep );
	   int FileCopy (LPCTSTR src, LPCTSTR dst);
	   void SetAttr(LPCTSTR file_name, BOOL readonly);
       void AppendPath(char *strFirst,LPCTSTR strSec);
       void ClearDir(LPCTSTR DirName);
       void GetFileName(CString filename, CString& result) ;
       void GetFileDir(CString filename, CString& result);
	   void GetFileExt(CString filename, CString& result) ;
       void GetFileWName(CString filename, CString& result) ;
       BOOL isFileReadable(CString filename);
	   void Trim(char *Line);
	   BOOL SubstituteValues(CString &strVar);
	   BOOL SubstituteValues(char *szVar);
};
#endif
