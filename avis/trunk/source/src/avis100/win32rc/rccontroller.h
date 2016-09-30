/*******************************************************************
*                                                                  *
* File name:     rcecheck.h                                         *
*                                                                  *
* Description:   RCCheck class definition file                     *
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
* Function:      class definition for replication controller       *
*                                                                  *
*******************************************************************/

#ifndef RCCONTROLLER_H
#define RCONTROLLER_H
class RCController {

  public:

     RCController(CString SampleName, CString GoatsDirs, CStringList *pGoatsDirs2,
		          bool trace, bool after_reboot,
				  CSocket *socket, CString strIdentifier);
     ~RCController();   

	 bool ReportResults(bool copy_results, CString strResultsDir);

	 int StandardReplication(int nFromCommandFile = 0);
	 int ProcessCommands(CString strCommandFile);

  private:
     CStringList m_GoatList; 
     CStringList m_ChangedFiles;
     CStringList m_ChangedGoats;
     CStringList m_NewFiles;
     char m_WindowsDir[MAXPATH];
     CString m_GoatsDir;
	 CStringList *m_pGoatsDirs2;
     CString m_SampleName;
     bool m_trace;
     CString m_strCommandFile;
     CString m_strResultsDir; // where to copy resulting replicants
     CString m_strVarList; //variable list if used
     CMapStringToString m_RegistryValues[8];
	  
	 CWinThread *m_pThread;
     int m_goats_infected;
	 bool m_after_reboot;
     RCError rcTrace;
     RCCheck rcCheck;
  	 CSocket *m_socket;
	 CString m_strIdentifier;

	 int Initialize();
     void GetFileName(CString filename, CString &result);
     int FileCopy (LPCTSTR src, LPCTSTR dst);
     void AppendPath(char *strFirst,LPCTSTR strSec);
     int InitGoatList(CString GoatsDir);
     void CopyAllGoats(CString strDst);
     int RunProgram(CString, int timeout_seconds, BOOL virus);
     int GoatsInfected();
     void ExecuteGoats(CString TmpDir);
     void CopyAllFiles(CString strSrc, CString strDst);
     void GetRegistryValues(CMapStringToString RegistryValues[]);
     bool CheckRegistryValues();
     bool IsRebootRequired(); 
     void GetValuesOfRegKey(CMapStringToString &KeyValues, HKEY hKey, char* path);
     void CopyResults(CString strResultsDir);
  	 void ReportProgress(CWin32rcDlg *dlg, CString strMsg,CString strType, CString strPhase);

    // bool WriteRegistryInfo(CString strCommand);


};
#endif
