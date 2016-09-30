/*******************************************************************
*                                                                  *
* File name:     rcconfig.h                                        *
*                                                                  *
* Description:   RCConfig class definition file                    *
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
* Function:      class definition for rc comnfiguration init       *
*                                                                  *
*******************************************************************/
#ifndef RCCONFIG_H
#define RCCONFIG_H
class RCConfig 
{
     private:
		 RCError Error;
		 RCCommon *Common;
         char GenCfgFile[MAXPATH];
         char VarListFile[MAXPATH]; 
         char AppCfgFile[MAXPATH];
    
      //   CString GlobalTemplateName;
         //CString TemplatesDir;
         
         void ProcessParams(int argc, char **argv);
          void GetConfig(CString &AppName, int &AppVersion);
          void GetLanguageSettings();
		  void GetVariables();

          int GetArgs (char *commandLine, char *argv[MAXARGS]);
          int SearchString (const char *string_to_search, const char **string_array,
                  const int array_size);
public:
          RCConfig(RCCommon *Common);
		  void InitConfig(char *commandLine, CString &AppName, int &AppVersion);
		            
};
#endif
