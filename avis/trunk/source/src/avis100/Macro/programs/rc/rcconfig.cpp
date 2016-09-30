/*******************************************************************
*                                                                  *
* File name:     rcConfig.cpp                                            *
*                                                                  *
* Description:   Configuration initialization                      *
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
* Function:      Configure RC by parsing several configuration     *
*                files and variable list                           *
*                                                                  *
*******************************************************************/

#include "afxtempl.h"
#include <stdlib.h>
#include <string.h>
#include "rcdefs.h"
#include "rcerror.h"
#include "rccommon.h"
#include "rcconfig.h"
#include "errors.h"


// constructor
RCConfig::RCConfig(RCCommon *Common)
               : Common(Common)
{
        
       Common->nokillpopup = 0;
	   strcpy(VarListFile, "");
     
}

// initialize configuration variables
void RCConfig::InitConfig(char *commandline, CString &AppName, int &AppVersion)
{
	int argc;
    char *argv[MAXARGS];

	  argc = GetArgs(commandline, argv);
  	  ProcessParams(argc, argv );   

	  if (strcmp(VarListFile, "")) // if variable list is specified
	     GetVariables();
	  if (Common->VarList["GLOBALDIR"].IsEmpty() ||
		  Common->VarList["GLOBALNAME"].IsEmpty()) 
         Error.ExitError(RCBadVarList, "Error in variable list: GLOBALDIR and GLOBALNAME must be specified");

	  strcpy(Common->GlobalPath,Common->VarList["GLOBALDIR"]);
	  Common->AppendPath(Common->GlobalPath, Common->VarList["GLOBALNAME"]);
      GetConfig(AppName, AppVersion);
      GetLanguageSettings();
	
}

// parse variours configuration files
void RCConfig::GetConfig(CString &AppName, int &AppVersion)
{
    FILE * f;
    char Line [MAXPATH+51];
    CString  FieldCode;
    CString  FieldValue;
 
   CStringList *LineItem;

   // parse general configuration file (specified ofter -general parameter)
   if ((f = fopen(GenCfgFile, "r")) == NULL)
  {
     Error.ExitError(RCMissingCfgFile,"Unable to open the general config file (%s)",  GenCfgFile);
  }

   while (fgets(Line, MAXPATH+50, f) != NULL) { // while not end-of-file
   Common->Trim(Line);
    if (strcmp(Line, "") && Line[0] != '#') {
        LineItem = Common->ParseLine(Line, "|");
        POSITION position = LineItem->GetHeadPosition();
        FieldCode = LineItem->GetNext(position);
        FieldCode.MakeUpper();
        FieldValue = LineItem->GetNext(position);

		if (FieldValue.Find('{') > -1)  // if there are vars
 	      if (Common->SubstituteValues(FieldValue) == FALSE)
			   Error.ExitError(RCBadVarList, "Variable %s not defined",FieldValue);
	          
        if (FieldCode == "POPUPKILLERPATH")
                 Common->PopupKillerPath = FieldValue;
        else if (FieldCode ==   "POPUPKILLERTIMEOUT")      
                 Common->PopupKillerTimeOut = FieldValue;
        else if (FieldCode ==  "POPUPKILLERSLEEPTIME")
                 Common->PopupKillerSleepTime = FieldValue;
        else if (FieldCode == "SEEKAPPPATH")
                 Common->SeekAppPath = FieldValue;
        else if (FieldCode == "APPSETUPPATH")
                 Common->AppSetupPath = FieldValue;
          
    
      } //endif 
   }  // endwhile
 fclose (f);

       
     // read application config file specified on -appcfg parameter
   if ((f = fopen(AppCfgFile, "r")) == NULL)
  {
     Error.ExitError( RCMissingCfgFile,
	  "Unable to open the application config file (%s)", AppCfgFile);
	 delete LineItem;
     //PostQuitMessage(RCMissingCfgFile);
	 exit(RCMissingCfgFile);
  }

  while (fgets(Line, MAXPATH+30, f) != NULL) { // while not end-of-file
    Common->Trim(Line);
    if (strcmp(Line, "") && Line[0] != '#') {
        LineItem = Common->ParseLine(Line, "|");
        POSITION position = LineItem->GetHeadPosition();
        FieldCode = LineItem->GetNext(position);
        FieldCode.MakeUpper();
        FieldValue = LineItem->GetNext(position);
        
		if (FieldValue.Find('{') > -1)  // if there are vars
		  if (Common->SubstituteValues(FieldValue) == FALSE)
			   Error.ExitError(RCBadVarList, "Variable %s not defined",FieldValue);
	   
		if (FieldCode == "APPNAME") 
           AppName = FieldValue;
        else if(FieldCode == "APPVERSION")
		   AppVersion = atoi(FieldValue);
        else if (FieldCode == "APPPATH" )
           Common->AppPath = FieldValue;
		else if (FieldCode == "OLEID")
			Common->OleId = FieldValue;
        else if (FieldCode ==  "CLOSEWP" )
                Common->CloseWP = atol(FieldValue);
        else if (FieldCode == "EXITWP" )
                Common->ExitWP = atol(FieldValue);
        else if (FieldCode ==  "NEWWP" )
               Common->NewWP = atol(FieldValue);
        else if (FieldCode == "OPENWP" )
               Common->OpenWP = atol(FieldValue);
        else if (FieldCode == "SAVEWP" )
                Common->SaveWP = atol(FieldValue);
        else if (FieldCode ==  "SAVEASWP" )
                Common->SaveAsWP = atol(FieldValue);
		else if (FieldCode == "UNHIDEWP")
			    Common->UnhideWP = atol(FieldValue);
		else if (FieldCode == "SLIDESHOWWP")
			    Common->SlideShowWP = atol(FieldValue);
		else if (FieldCode == "OPENDLGCLASSNAME")
			    Common->OpenDlgClassName = FieldValue;
		else if (FieldCode == "SAVEASDLGCLASSNAME")
			    Common->SaveAsDlgClassName = FieldValue;
		else if (FieldCode == "DESKCLASSNAME")
			    Common->DeskClassName = FieldValue;
 		else if (FieldCode == "DOCCLASSNAME")
			    Common->DocClassName = FieldValue;
              
    } //endif
 }

  fclose (f);
  delete LineItem;     
  if (AppName != "Word" && AppName != "Excel" 
	  &&AppName != "PowerPoint" && AppName != "Access")
  {
      Error.ExitError( RCBadAppName,
	  "Invalid application name");
  
  }


}  // GetConfig


// get language-specific settings from language configuration file
void RCConfig::GetLanguageSettings()
{
   FILE * f;

   char Line [MAXPATH+30];
   CString FieldCode ; 
   CString OperationCode;
   CString FieldValue ;
   CStringList *LineItem;

    if ((f = fopen(Common->LangCfgFile, "r")) == NULL)
  {
     Error.ExitError(RCMissingCfgFile,
		 "Unable to open the language config file: %s", Common->LangCfgFile);
  }
  
   while (fgets(Line, MAXPATH+20, f) != NULL) { // while not end-of-file
	   Common->Trim(Line);
       if (strcmp(Line, "") && Line[0] != '#') {
        LineItem = Common->ParseLine(Line, "|");
		POSITION pos = LineItem->GetHeadPosition();
        FieldCode = LineItem->GetNext(pos);
        FieldCode.MakeUpper();
        OperationCode = LineItem->GetNext(pos);
        OperationCode.MakeUpper();
        FieldValue = LineItem->GetNext(pos);
                       
		if (FieldValue.Find('{') > -1)  // if there are vars
     		if (Common->SubstituteValues(FieldValue) == FALSE)
			   Error.ExitError(RCBadVarList, "Variable %s not defined",FieldValue);
	   
        if ( FieldCode =="T") {
            if (OperationCode == "SAVEAS" )
                Common->SaveAsTitle = FieldValue;
            else if (OperationCode == "OPEN")
                Common->OpenTitle = FieldValue;
            else if (OperationCode== "NEW")
                Common->NewTitle = FieldValue;
                   
        } else if (FieldCode == "S") {
             if (OperationCode == "SAVEAS")
                Common->SaveAsShortcut = FieldValue;
            else if (OperationCode == "SAVE" )
                Common->SaveShortcut = FieldValue;
            else if (OperationCode =="OPEN")
                Common->OpenShortcut = FieldValue;
            else if (OperationCode == "NEW")
                Common->NewShortcut = FieldValue;
            else if (OperationCode == "CLOSE")
                Common->CloseShortcut  =FieldValue;
            else if (OperationCode == "EXIT")
                Common->ExitShortcut = FieldValue;
            else if (OperationCode == "SLIDESHOW")
                Common->SlideShowShortcut = FieldValue;

           }   else if ( FieldCode == "C") {

            if (OperationCode == "FILESAVEAS")
                Common->SaveAsCommand = FieldValue;
            else if (OperationCode == "FILESAVE")
                Common->SaveCommand = FieldValue;
            else if (OperationCode == "FILEOPEN")
                Common->OpenCommand = FieldValue;
            else if (OperationCode == "FILENEWDEFAULT")
                Common->NewCommand = FieldValue;
            else if (OperationCode == "FILECLOSE")
                Common->CloseCommand = FieldValue;
            else if (OperationCode == "FILEEXIT")
                Common->ExitCommand = FieldValue;
            else if (OperationCode == "STARTOFDOCUMENT")
                Common->StartOfDocumentCommand = FieldValue;
            else if (OperationCode == "ENDOFDOCUMENT")
                Common->EndOfDocumentCommand = FieldValue;
            else if (OperationCode == "TOOLSOPTIONSSAVE")
                Common->ToolsOptionsSaveCommand = FieldValue;
            else if (OperationCode == "DOCMAXIMIZE")
                Common->DocMaximizeCommand = FieldValue;
           
        
        } else if  (FieldCode == "L") {
            if (OperationCode == "DOCUMENT")
                Common->DocumentLabel = FieldValue;
            else if (OperationCode == "DOC")
                Common->DocLabel = FieldValue;
            else if (OperationCode == "TEMPLATE")
                Common->TemplateLabel = FieldValue;

         
        } else if (FieldCode == "E" )
            if ( OperationCode == "NEWMOVING")
                Common->NewMoving = FieldValue;
         
          }

      
  }
  fclose (f);
  delete LineItem;
}

// process input parameters
void RCConfig::ProcessParams(int argc, char **argv)
{
        
    CString Line;
    CString FieldCode;
    CString FieldValue;
    int i;

   
   for (i =0;  i <= argc; i++)  { // for all parameters
      strupr(argv[i]);
      if (!strcmp(argv[i], "-LOG")) ; // ignore - always need the log
      else if (!strcmp(argv[i], "-VERBOSE") ) Error.SetVerbose(1);
      else if (!strcmp(argv[i] ,"-NOKILLPOPUP")) Common->nokillpopup = 1;
      else if (!strcmp(argv[i] ,"-GENERAL" )) {
         i++;
         strcpy(GenCfgFile, argv[i]);
	  } else if (!strcmp(argv[i], "-VARIABLES")) {
         i++;
		 strcpy(VarListFile, argv[i]);
      } else if (!strcmp(argv[i],"-LANGUAGE")) {
          i++;
         strcpy(Common->LangCfgFile,  argv[i]);
      }   else if (!strcmp(argv[i], "-APPCFG")) {
          i++;
         strcpy(AppCfgFile, argv[i]);
      } else if (!strcmp(argv[i], "-COMMANDS")) {
          i++;
         Common->CommandsFile = argv[i];
      } else if (!strcmp(argv[i], "-GOATS")) {
          i++;
          Common->GoatsListFile = argv[i];
      } else if (!strcmp(argv[i], "-2BYTE")) {
              i++;
             Common->DBCSMode = TRUE;
      } else if (!strcmp(argv[i], "-TIMEOUT")) { // sleep interval to use in waits
		                         // for the commands to complete
		  i++; 
		  Common->TimeoutInterval = atoi(argv[i]);
	  } else if (!strcmp(argv[i], "-WAITIDLETIME")) {
		  i++;
		  Common->WaitIdleTime = atoi(argv[i]);
	  } else  //*** bad parameter ignore for now, also ignore
		     // the rest of line
		  break;
      
      
   }     // for   


}

// Converts Windows CommanLine into argc, argv arguments
int RCConfig::GetArgs (char *commandLine, char *argv[MAXARGS])
{
  
  //int index;
  int count=0;
  char *p;
  char temp[MAXPATH];
  int length;
  char *q;

  p = commandLine;
  while (*p != '\0' && *p == ' ') p++; //skip blanks
  while (*p != '\0')
  {
     length = 0;
     q = temp;
     while (*p != '\"' && *p != ' ' && *p != '\0' && length < MAXPATH)
     {
       *q++ = *p++;

       length++;
     }
     if (*p == '\"')  // found a quote
     {
       p++;        // skip it
       while (*p != '\"' && *p != '\0' && length < MAXPATH) // copy till end of quote
       {
        *q++ = *p++;

        length++;
       }
           if (*p == '\"') p++;
      }
      *q = '\0';
      argv[count] = (char *) calloc(1, strlen(temp)+1);
      if (argv[count] == NULL) return 0;
      strcpy(argv[count++], temp);
      while (*p != '\0' && *p == ' ') p++; //skip blanks
     }
   return count-1;

}

// get variables from the variable list
void RCConfig::GetVariables()
{
   FILE * f;

   char Line [MAXPATH+30];
   CString strVarName ; 
   CString strVarValue;
   CStringList *LineItem;

    if ((f = fopen(VarListFile, "r")) == NULL)
  {
      Error.ExitError(RCBadVarList,"Unable to open variable list file: %s",VarListFile);
  }
  
   while (fgets(Line, MAXPATH+20, f) != NULL) { // while not end-of-file
	   Common->Trim(Line);
       if (strcmp(Line, "") && Line[0] != '#') {
          LineItem = Common->ParseLine(Line, "=");
	      if (LineItem->GetCount() < 2) // if less than two parameters specified
		  {
		    Error.ExitError(RCBadVarList,"Incorrect line %s in variable list",Line);
		 	return;
		  }
		  POSITION pos = LineItem->GetHeadPosition();
          strVarName = LineItem->GetNext(pos);
          strVarName.MakeUpper();
          strVarValue = LineItem->GetNext(pos);
          Common->VarList[strVarName] = strVarValue;                  
       }
        
      
  }
  fclose (f);
  delete LineItem;
}
