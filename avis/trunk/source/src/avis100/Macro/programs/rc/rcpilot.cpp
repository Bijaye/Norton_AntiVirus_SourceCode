/*******************************************************************
*                                                                  *
* File name:     RCPilot.cpp                                       *
*                                                                  *
* Description:   RCPilot class implementation                      *
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
* Function:      Parse the command list and invoke appropriate     *
*                method in RCCmd class to process the command      *
*                                                                  *
*******************************************************************/

#include "afxtempl.h"
#include "afxdisp.h"
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <direct.h>

#include "rcdefs.h"
#include "errors.h"
#include "RCError.h"
#include "RCKeys.h"
#include "RCCommon.h"
#include "RCFile.h"
#include "RCGoatList.h"
#include "RCPilot.h"
#include "msword8.h"
#include "excel8.h"
#include "wb70en32.h"
#include "msppt8.h"
#include "msacc8.h"
#include "RCCmd.h"

// constructor
 RCPilot::RCPilot(RCCommon *rcCommon, 
	 char *ResultPath, CString AppName, int AppVersion) :
      Common(rcCommon),
	  ResultPath(ResultPath),
	  AppName(AppName),
	  AppVersion(AppVersion)
{

	GoatsList = new RCGoatList(Common);
	RegisteredGoats = new RCGoatList(Common);
}

// Read the commands file and execute the specified commands
void RCPilot::ExecuteCommands()
{

     char *X, *Y;
     int ret;
    
     char TextLine[300];
     CString cmd;
     CString RealName;
     CStringList *Params;
	 RCFile *G;
     CString FirstFile;
     CString GoatCode;
     CString OldGoatCode;
     CString PrevCode;
     CString Position;
     CString fname;
     CString DirCode;
     FILE *commandsfile;
	 RCCmd *rcCmd;
     POSITION pos;
	 int count;

	 
    //remove files from the startup directory
    
    if ((commandsfile = fopen (Common->CommandsFile,"r")) == NULL)
    {
        Error.ExitError(RCMissingCfgFile,
			"Unable to open the commands file : %s.", Common->CommandsFile);
         exit(RCMissingCfgFile);
    }
        
    
       Params = NULL;
	   // initialize the goat list
       GoatsList->InitGoatList(PlainDocPath);
	 
	   // initialize the command processor
	   rcCmd = new RCCmd(GoatsList, Common, AppName, AppVersion);   

	   // read the commands file
       while (fgets(TextLine, MAXLINE-1, commandsfile) != NULL) {
		  Common->Trim(TextLine);  // get rid of leading/trailing blanks
          if (strcmp(TextLine, "") && TextLine[0] != '#' ) { // allow lines that start with # to be comments
                X = strchr(TextLine, '[');
                Y = strchr(TextLine, ']');
                if (Y != NULL) *Y = '\0';
                cmd = X+1;
                cmd.MakeUpper();                
           
                if (Y != NULL) Y++;
                if (Y != NULL && *Y != '\0') {
                    strcpy(TextLine, Y);
                    if (strchr(TextLine,'{') !=NULL) {// if a variable
                       if (Common->SubstituteValues(TextLine) == FALSE)
	  		               Error.ExitError(RCBadVarList, "Variable(s) on line %s not defined",TextLine);
					}
                }
				Params = Common->ParseLine(TextLine, "|");
                pos = Params->GetHeadPosition();
				count = Params->GetCount();
				GoatCode.Empty();
                if (cmd == "KOPEN") {
					
                    GoatCode = Params->GetNext(pos);
                    ret = rcCmd->FOpen(GoatCode, IPC_KS);
                    if (ret !=0 && ret != Err_Unable_Read_File && ret != Err_App_Not_Running) 
                      rcCmd->OLEFOpen (GoatCode);
                   
                    PrevCode = GoatCode;
                   // 'previous doc = ...
				
                } else if (cmd == "MOPEN") {
					GoatCode = Params->GetHead();
                    ret = rcCmd->FOpen(GoatCode, IPC_WM);
                    if (ret != 0 && ret !=  Err_Unable_Read_File && ret != Err_App_Not_Running) {
                      rcCmd->OLEFOpen (GoatCode);
                    }
                    PrevCode = GoatCode;
							
                } else if (cmd == "KSAVE") {
				
                    OldGoatCode =Params->GetNext(pos);
                    if (count == 2) 
                        GoatCode = Params->GetNext(pos);
                    else
                        GoatCode.Empty();
                    
                    ret = rcCmd->FSave(OldGoatCode, GoatCode, IPC_KS);
					if (ret != Err_OK &&
						ret != Err_App_Not_Running &&
						ret != Err_Target_Doc_Not_Active)  
					    if (GoatCode.IsEmpty())
						   ret = rcCmd->OLEFSave(OldGoatCode);
						else
                           ret = rcCmd->OLEFSaveAs(OldGoatCode, GoatCode);
                
                } else if (cmd == "MSAVE") {
							
                    OldGoatCode = Params->GetNext(pos);
                    if (count == 2) 
                        GoatCode = Params->GetNext(pos);
                    else
                        GoatCode.Empty();
                    
                    int ret = rcCmd->FSave(OldGoatCode, GoatCode, IPC_WM);
   					if (ret != Err_OK &&
						ret != Err_App_Not_Running &&
						ret != Err_Target_Doc_Not_Active)  
					    if (GoatCode.IsEmpty())
						   ret = rcCmd->OLEFSave(OldGoatCode);
						else
                           ret = rcCmd->OLEFSaveAs(OldGoatCode, GoatCode);
                } else if (cmd == "KSTARTSHOW") {
                    GoatCode =Params->GetNext(pos);
                    ret = rcCmd->StartShow(GoatCode, IPC_KS);
                } else if (cmd == "MSTARTSHOW") {
                    GoatCode =Params->GetNext(pos);        
				    ret = rcCmd->StartShow(GoatCode, IPC_WM);
				} else if (cmd == "KSHOW") {
					GoatCode = Params->GetNext(pos);
					ret = rcCmd->Show(GoatCode, IPC_KS);
				} else if (cmd == "MSHOW") {
					GoatCode = Params->GetNext(pos);
					ret = rcCmd->Show(GoatCode, IPC_WM);
                } else if (cmd == "CENTERLEFTCLICK") {//powerpoint only
                    GoatCode =Params->GetNext(pos);                    
                    ret = rcCmd->CenterLeftClick(GoatCode);
                } else if (cmd == "KSAVEAS") {
                    OldGoatCode = Params->GetNext(pos);
                    GoatCode = Params->GetNext(pos);
                    ret = rcCmd->FSaveAs(OldGoatCode, GoatCode, IPC_KS);
                    if (ret != Err_OK &&
						ret != Err_App_Not_Running &&
						ret != Err_Target_Doc_Not_Active)  
                      ret = rcCmd->OLEFSaveAs(OldGoatCode, GoatCode);
                							
                } else if (cmd == "MSAVEAS") {
                     OldGoatCode = Params->GetNext(pos);
                     GoatCode = Params->GetNext(pos);
                     ret = rcCmd->FSaveAs(OldGoatCode, GoatCode, IPC_WM);
                    if (ret != Err_OK &&
						ret != Err_App_Not_Running &&
						ret != Err_Target_Doc_Not_Active)  
                       ret = rcCmd->OLEFSaveAs(OldGoatCode, GoatCode);
											
                } else if (cmd == "KCLOSE") {
                    GoatCode =  Params->GetHead();
                    ret = rcCmd->FClose(GoatCode, IPC_KS);
                    if (ret != Err_OK && ret != Err_App_Not_Running &&
						 ret != Err_Target_Doc_Not_Active)  
                      ret = rcCmd->OLEFClose(GoatCode);
                    if (strcmp(GoatCode, "virus") && ret == 0)  
						ReplicantsList.AddTail(GoatCode);
						
                } else if (cmd == "MCLOSE") {
                    GoatCode = Params->GetHead();
                    ret = rcCmd->FClose(GoatCode, IPC_WM);
                    if (ret != Err_OK && ret != Err_App_Not_Running &&
						 ret != Err_Target_Doc_Not_Active)  
                      ret = rcCmd->OLEFClose(GoatCode);
                    if (strcmp(GoatCode, "virus") && ret == 0)  
						ReplicantsList.AddTail(GoatCode);
								
				} else if (cmd == "OLEOPEN") {
                    GoatCode = Params->GetHead();
                    ret = rcCmd->OLEFOpen(GoatCode);
                
                } else if  (cmd == "OLESAVE") {
                    GoatCode = Params->GetHead();
                    ret = rcCmd->OLEFSave(GoatCode);
                
                } else if (cmd == "OLESAVEAS") {
                     OldGoatCode = Params->GetNext(pos);
                     GoatCode = Params->GetNext(pos);
                     ret = rcCmd->OLEFSaveAs(OldGoatCode, GoatCode);
                } else if (cmd == "OLESAVEAS95") {
					OldGoatCode = Params->GetNext(pos);
                    GoatCode = Params->GetNext(pos);

					ret = rcCmd->OLEFSaveAsFormat(OldGoatCode,GoatCode, 95);
                } else if (cmd == "OLESAVEAS97") {
					OldGoatCode = Params->GetNext(pos);
                    GoatCode = Params->GetNext(pos);

					ret = rcCmd->OLEFSaveAsFormat(OldGoatCode,GoatCode, 97);

                } else if (cmd == "OLECLOSE") {
                    GoatCode = Params->GetHead();
                    ret = rcCmd->OLEFClose(GoatCode);
                    if (strcmp(GoatCode, "virus") && ret == 0)  
						ReplicantsList.AddTail(GoatCode);

                } else if (cmd == "KNEWDOC") {
                    GoatCode = Params->GetHead();
                    ret = rcCmd->FNew(GoatCode, IPC_KS, PlainDocPath);
                    if (ret !=Err_OK && ret != Err_App_Not_Running)   
						rcCmd->OLEFNew (GoatCode,PlainDocPath);
                
                } else if (cmd == "MNEWDOC") {
                    GoatCode = Params->GetHead();
                    ret = rcCmd->FNew(GoatCode, IPC_WM,PlainDocPath);
                    if (ret !=Err_OK && ret != Err_App_Not_Running)   
						rcCmd->OLEFNew (GoatCode, PlainDocPath);
                
                } else if (cmd == "OLENEWDOC") {
                    GoatCode = Params->GetHead();
                    ret = rcCmd->OLEFNew(GoatCode,PlainDocPath);
                    PrevCode = "plain";
                
                
                } else if (cmd == "STARTAPP") {
                    //  start the application
                     CString strTmp;
					 CString FirstFile;
					 FirstFile.Empty();
					 if (count >= 1) {
         				 strTmp = Params->GetNext(pos);
                         if (!strncmp(strTmp, "F:",2)) {
                            GoatCode = strTmp.Mid(2);
						    RCFile *G = GoatsList->FindFileByCode(GoatCode);
    
	                        if (G== NULL) {   // should not happen
		                       rcCmd->CloseApp(); // clean up  
	                           Error.ExitError(RCFileNotFound,"Error in OLEFOpen, file with code %s not found", GoatCode);
							}
							FirstFile = G->RealName;
                          }
					 } 
				     rcCmd->StartApp(FirstFile);

				} else if (cmd == "ADDGOAT") {
                     GoatCode = Params->GetNext(pos);
                     if (GoatCode.CompareNoCase("VIRUS") != 0) 
                        ReplicantsList.AddTail( GoatCode);

                } else if (cmd == "KCLOSEAPP" || cmd == "MCLOSEAPP") {
                    // closes word, copies the goats and normal.dot to the result directory
                    // updates the replicants list
                      rcCmd->CloseApp ();
					  KeepResults();
					                
                } else if (cmd == "SENDTEXT") {
					CString str1, str2;
					str1 = Params->GetNext(pos);
					str2 = Params->GetNext(pos);
                    rcCmd->TypeText (str1, str2);
                 } else if (cmd == "SLEEP") {
					int i;
                    CString str1 = Params->GetNext(pos); 
					if ((i =str1.Find('*')) >= 0) {
						CString strVal1 = str1.Left(i);
                        CString strVal2 = str1.Right(i+1);
						Sleep(atoi(strVal1) * atoi(strVal2));
					} else {
                      Sleep(atoi(str1));
                   } 
				} else if (cmd == "WAITAPPIDLE") {
                    CString str1 = Params->GetNext(pos); 
					rcCmd->WaitIdle(atoi(str1));

                } else if (cmd =="GOTO") {
                    GoatCode = Params->GetNext(pos);
                    Position = Params->GetNext(pos);
                    rcCmd->OLEGoto( GoatCode, Position);
                                
                } else if (cmd == "RESTORE") {
                    CString tempfile;
					CString strTmp;
					
                    GoatCode = Params->GetNext(pos);
                    G = GoatsList->FindFileByCode(GoatCode);
					if (G == NULL) { // error -- incorrect goats.lst
						             // or command file
						Error.ExitError(RCCmdFileError, "Error: incorrect command file");
					}
                    Common->SetAttr(G->RealName, 0);
                    unlink (G->RealName);
                    Common->GetFileDir(G->RealName, tempfile);
                    
					Common->GetFileName(G->RealName, strTmp);
					char *szTmp = new char[tempfile.GetLength()+
						             strTmp.GetLength() + 2];
					strcpy(szTmp, tempfile); // copy file dir
                    Common->AppendPath(szTmp, strTmp);
                    tempfile = szTmp;
					delete szTmp;
                    tempfile += ".dot";
                    unlink(tempfile);
                
                    if (Common->FileCopy(G->BackupName, G->RealName) == FALSE) {
                
                       Error.ReportError ("Unable to restore %s (%s) to (%s).", 
						     GoatCode, G->BackupName, G->RealName);
                   
                    }
                
                } else if (cmd == "CLEARDIRS") {
                    // 'clears the directories when the goats are stored when word is running
                    ClearRepDirs();
                
                } else if (cmd == "CLEAR") {
                     DirCode = Params->GetNext(pos);
                     if (DirCode == "startup")
                        Common->ClearDir (Common->VarList["STARTUPDIR"]);
                    else 
                      Error.ReportError("invalid directory code for CLEAR command: %s.",
					     DirCode);
                } else if (cmd == "GETLIST") {
				    CString strDir;
			        strDir = Params->GetNext(pos);
					GetList(strDir);
				} else if (cmd == "PICKUPFILES" || cmd == "DELETEFILES") {
					file_flags flags;
					CString strFlag = "";
					CString strFlag1 = "";
					flags.newfiles = 0;
					flags.ole2only = 0;
					CString filterStr = Params->GetNext(pos);
					if (count > 1)
					  strFlag = Params->GetNext(pos);

					if (count > 2) 
						strFlag1 = Params->GetNext(pos);
					
                    flags.newfiles = (strFlag == "NEW" || strFlag1 == "NEW");
					
					flags.ole2only = (strFlag == "OLE2" || strFlag1 == "OLE2");
					if (cmd == "PICKUPFILES")
					   PickupFiles(filterStr, flags);
                    else
				       DeleteFiles(filterStr, flags);
                } else {
                    // bad command
                    Error.ReportError("Unknown command in the commands file : %s.", 
						   cmd);
                }
				 if (Params->GetCount() > 0)
                    Params->RemoveAll();
                 delete Params;
               }			  
            
       }         //while
    fclose(commandsfile);
}


void RCPilot::ClearRepDirs()
{

	RCFile *G;

   POSITION pos = GoatsList->goatsList.GetHeadPosition();
    while(pos != NULL) {
		 G = GoatsList->goatsList.GetNext(pos);
         if (G->RealName.CompareNoCase(Common->GlobalPath) !=0&& G->Code.CompareNoCase("virus")!=0) {
			CString strTmp;
			Common->GetFileDir(G->RealName,strTmp);
			if (G->Code.Find("dropped") != 0) { // do not clear dropped files directory
              Common->ClearDir(strTmp );
			}
        }
	}
}

void RCPilot::KeepResults()
{
      CString Code;
      RCFile *G;
      CString Typ;
      CString filename;
      CString temppath;
	  CString strTmp;
       FILE *f;
       int copy_error =0;
	   CString tempcode;
       POSITION pos, goatpos;
	     // 'remove the possibly modified goats from the list of registered goats
       
    
    pos = ReplicantsList.GetHeadPosition();
    while (pos != NULL)
   {
		tempcode = ReplicantsList.GetNext(pos);
        goatpos = RegisteredGoats->FindPosByCode(tempcode);
		if (goatpos != NULL)
         RegisteredGoats->goatsList.RemoveAt(goatpos);
     
    }
    
    filename = ResultPath;
	filename+= "\\replic.lst";
    if ((f = fopen(filename,"w")) != NULL) {

      pos  = RegisteredGoats->goatsList.GetHeadPosition();
      while(pos != NULL) {
		  char ShortPath[MAXPATH];
		  G = RegisteredGoats->goatsList.GetNext(pos);
		  CString strTmp;
		 		  
	      Common->GetFileExt(G->RealName, strTmp);
		  if (!G->RealName.IsEmpty())
	                    temppath = ResultPath;
		  temppath += '\\' + G->Code + '.' + strTmp;
		  GetShortPathName(temppath, ShortPath, MAXPATH);  
		  G->FileType.MakeUpper();
              fprintf(f, "[%s]|%s|%s|%s\n",G->FileType, G->BackupName, ShortPath,G->RealName );
        
    } 
    pos = ReplicantsList.GetHeadPosition();
    while(pos != NULL) {
        Code = ReplicantsList.GetNext(pos);
        G = GoatsList->FindFileByCode(Code);
		if (G==NULL) { //should not happen
			Error.ExitError(RCFileNotFound,"Error: file with code %s is not found", Code);
		}	
        if (!G->isVirusCopy) {
            strTmp = ResultPath;
			strTmp += "\\" + Code +  ".";
			CString strTmpExt;
			Common->GetFileExt(G->RealName, strTmpExt);
		    strTmp += strTmpExt;
            temppath = ResultPath;
 			temppath += '\\'+G->Code + '.' + strTmpExt;

		    if (!Common->FileCopy(G->RealName, strTmp)) {

               // maybe a change of extension occured while closing the file
               Common->GetFileDir(G->RealName,strTmp);
               strTmp += "\\";
			   CString strTmpFile;
			   Common->GetFileName(G->RealName, strTmpFile);
               strTmp += strTmpFile +".dot";         
               if (! Common->FileCopy (strTmp, temppath)) {
                   
                   Error.ReportError("Missing file: %s", G->RealName);
                   copy_error = 1;
               }
            }
            if (!copy_error) {
                G->FileType.MakeUpper();
                RegisteredGoats->CopyItem(G);
                fprintf (f, "[%s]|%s|%s|%s\n",G->FileType, G->BackupName, temppath, G->RealName);
            }
        } else {
             Error.ReportError("%s is a copy of the virus and will be dismissed.",Code);
        }
    
   }
    fclose(f);
	if (ReplicantsList.GetCount() > 0)
       ReplicantsList.RemoveAll();
	}
}

void RCPilot::GetList(CString strDir)
{
  CString strFile;
  char TName[MAXPATH];
  struct _finddata_t result;  // the structure returned from _findfirst/_findnext
  long hFile;
  int rc;

 
   Error.ReportError("Looking for files in %s",strDir);
   strcpy(TName,strDir);
   Common->AppendPath(TName, "*.*");

    hFile = _findfirst(TName, &result);
    rc = (hFile == -1) ? 1:0;
    while(!rc)
    {
      Error.ReportError("entered loop");
      if(strcmp(result.name,".") && strcmp(result.name,"..")) {
         strcpy(TName, strDir);
         Common->AppendPath(TName, result.name);
        
         Error.ReportError("Found %s in the directory %s.",TName, strDir); 
       
         GetListFiles.AddTail(TName);
        }
       rc = _findnext(hFile, &result);
    }

     _findclose(hFile);
}

void RCPilot::PickupFiles(CString strFilter, file_flags flags)
{
	CString strFileName;
  CString strDir;
  CString strFileFilter;
  CString GoatCode;
  char TName[MAXPATH];
  struct _finddata_t result;  // the structure returned from _findfirst/_findnext
  long hFile;
  int rc;
  int DroppedFiles;

    // Get the directory name from the filter
  int i = strFilter.ReverseFind('\\');
  strDir = strFilter.Left(i);
  strFileFilter = strFilter.Mid(i+1);
  strcpy(TName, strFilter);

  // Find first unused position for dropped files
  POSITION pos = GoatsList->goatsList.GetHeadPosition();
  DroppedFiles = 0;
  while( pos != NULL) {
      GoatCode.Format( "%s%d", "dropped", DroppedFiles+1);   
	  RCFile *goat = GoatsList->goatsList.GetNext(pos);
	  if ( goat->Code.CompareNoCase(GoatCode) == 0)
        DroppedFiles++;
  }
 
  Error.ReportError("Looking for files matching %s",TName);
  strcpy(TName, strFilter); //test - lets try to only search for matching files
  hFile = _findfirst(TName, &result);
  rc = (hFile == -1) ? 1:0;
  while(!rc)
  {
      Error.ReportError("entered loop");
      if(strcmp(result.name,".") && strcmp(result.name,"..")) {
         strcpy(TName, strDir);
         Common->AppendPath(TName, result.name);
        
         Error.ReportError("Found %s in the directory %s.",TName, strDir); 
       
         if (FileMatches(TName, flags)) {// if file matches the flags
		  
			DroppedFiles++;
            strcpy(TName, strDir);
            Common->AppendPath(TName, result.name);
        
            Error.ReportError("Adding %s to the goat list.",TName); 
            GoatCode.Format( "dropped%d", DroppedFiles);
		    RCFile *G = new RCFile();
		    G->Code = GoatCode;
            G->BackupName = PlainDocPath;
            G->RealName = TName;
            G->FileType = "dropped";
		    G->isVirusCopy = FALSE;
            GoatsList->goatsList.AddTail(G);
            ReplicantsList.AddTail (GoatCode);
		 }
       }
       rc = _findnext(hFile, &result);
    }

     _findclose(hFile);

}

void RCPilot::DeleteFiles(CString strFilter, file_flags flags)
{
  char TName[MAXPATH];
  CString strDir;   // directory
  CString strFileFilter;
  struct _finddata_t result;  // the structure returned from _findfirst/_findnext
  long hFile;
  int rc;

  // Get the directory name from the filter
     // Get the directory name from the filter
  int i = strFilter.ReverseFind('\\');
  strDir = strFilter.Left(i);
  strFileFilter = strFilter.Mid(i+1);
  strcpy(TName, strFilter);

  Error.ReportError("Looking for files matching %s",TName);

  hFile = _findfirst(TName, &result);
  rc = (hFile == -1) ? 1:0;
  while(!rc)
  {
      Error.ReportError("entered loop");
      if(strcmp(result.name,".") && strcmp(result.name,"..")) {
         strcpy(TName, strDir);
         Common->AppendPath(TName, result.name);
        
         Error.ReportError("Found %s in the directory %s.",result.name, strDir); 
       
         if (FileMatches(TName, flags)) {// if file matches the flags
            Error.ReportError("Deleting %s",TName); 
			unlink(TName);

		 }
       }
       rc = _findnext(hFile, &result);
    }

     _findclose(hFile);
}

BOOL RCPilot::FileMatches(char *TName, file_flags flags)
{
   CString strTemp;


   // check if new file
   if (flags.newfiles) { //if only matching new files
      POSITION pos = GetListFiles.GetHeadPosition();
      while(pos != NULL) {
		  strTemp = GetListFiles.GetNext(pos);
          if (strTemp == TName) // if same name
			  return FALSE; //not a new file
	  }
   }

   // see if looking for OLE files only
   if (flags.ole2only)  {
	   //need to read file, if the first 8 bytes are 
	   // D0 CF 11 E0 A1 B1 1A E1 - then it is OLE2
	   HFILE handle;
	   BYTE mask[] = {0xD0, 0xCF, 0x11, 0xE0,
		   0xA1, 0xB1, 0x1A, 0xE1};
       BYTE buf[513];
	   if ((handle = open(TName, O_RDONLY|O_BINARY))== -1)
	   {
		   Error.ReportError("Unable to read the file %s\n", TName);
		   return FALSE;
	   }
       if (8 < read(handle, buf, 512)) {
		   return FALSE; // not an ole file -- too short
	   }
	   if (memcmp(buf, mask, 8) != 0)
		   return FALSE;    // not an ole file
             
       else
		   return TRUE; // ole file
   }
   return TRUE; // flags not specified -- all files match   
}


RCPilot::~RCPilot()
{
	delete RegisteredGoats; 
	
	delete GoatsList;
}
