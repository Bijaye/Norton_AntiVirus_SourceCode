/*******************************************************************
*                                                                  *
* File name:     rcpilot.h                                         *
*                                                                  *
* Description:   RCPilot class definition file                     *
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
* Function:      class definition for rc pilot                     *
*                                                                  *
*******************************************************************/
#ifndef RCPILOT_H
#define RCPILOT_H

typedef struct {
	unsigned int newfiles : 1;
	unsigned int ole2only : 1;
} file_flags;

class RCPilot {
   private:
       //list of the codes of the goats kept for the results
      CStringList  ReplicantsList;
	  CStringList GetListFiles; // list of files in the startup
	                               // directory
      RCGoatList *RegisteredGoats;
      RCGoatList *GoatsList;
	  RCError Error;
	  RCCommon *Common;
	  CString AppName;
	  int AppVersion;
      char *ResultPath;
	  CString PlainDocPath;
	  CString MainAppWinName;
 
	  void ClearRepDirs();
      void  AddStartupTemplates();
	  void GetList(CString strDir);
	  void KeepResults();
   public:
	  RCPilot(RCCommon *rcCommon, 
		  char* ResultPath, CString AppName, int AppVersion);
	  ~RCPilot(); 
      void ExecuteCommands();
      void PickupFiles(CString strFilter, file_flags flags);
	  void DeleteFiles(CString strFilter, file_flags flags);
	  BOOL FileMatches(char *filename, file_flags flags);

};
#endif
