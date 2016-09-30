/*******************************************************************
*                                                                  *
* File name:     RCGoatList.cpp                                            *
*                                                                  *
* Description:   RC Goat list implementation                       *
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
* Function:      Implements goat list file that contain information *
*                about goat files used in replication              *
*                                                                  *
*******************************************************************/

#include "afxtempl.h"

#include "rcdefs.h"
#include "errors.h"
#include "RCFile.h"
#include "RCError.h"
#include "RCCommon.h"
#include "RCGoatList.h"

// constructor
RCGoatList::RCGoatList(RCCommon *Common) :
            Common(Common)
{
}

// find file entry corresponding to the goat in the goat list 
RCFile *RCGoatList::FindFileByCode(CString Code)
{
      POSITION pos = goatsList.GetHeadPosition();
       
      RCFile* pFile;
       while (pos != NULL)
       {
          pFile = goatsList.GetNext(pos);
          if (pFile->Code.CompareNoCase(Code) == 0)
            return pFile;
		
       }

       return NULL;
}
RCFile *RCGoatList::FindFileByRealName(CString FileName)
{
	POSITION pos = goatsList.GetHeadPosition();
    int i;
	RCFile *pFile;
	i = FileName.Find('.');

	CString FileNameNoExt = FileName.Left(i);
	
	while (pos != NULL)
	{
		CString strTmp;
		pFile = goatsList.GetNext(pos);
		i = pFile->RealName.Find('.');
 		strTmp = pFile->RealName.Left(i);

		if (FileNameNoExt.CompareNoCase(strTmp) == 0) 
			return pFile;
	}
	return NULL;
}


// find position of file in the collection class
POSITION RCGoatList::FindPosByCode(CString & Code)
{
	POSITION pos = goatsList.GetHeadPosition();
    POSITION prevpos =  pos; 
    RCFile* pFile;

       while (pos != NULL)
      {
		     
             pFile = goatsList.GetNext(pos);
             if (pFile->Code.CompareNoCase(Code) == 0)
	            return prevpos;
		      prevpos = pos;
       }

       return NULL;
}

// free the memory allocated for goat list entries
int RCGoatList::RemoveAll()
{
     POSITION position = goatsList.GetHeadPosition();
     POSITION pos1, pos2;
     RCFile* pFile;
    for (pos1 = goatsList.GetHeadPosition(); (pos2 = pos1) != NULL;)
    {
	pFile = goatsList.GetNext(pos1);
	goatsList.RemoveAt(pos2);
	delete pFile;
     }
    return 0;
}

// display all goat files - not used currently
int RCGoatList::Prnt()
{
     POSITION position = goatsList.GetHeadPosition();
     POSITION pos1, pos2;
     RCFile* pFile;
     for (pos1 = goatsList.GetHeadPosition(); (pos2 = pos1) != NULL;)
    {
	pFile = goatsList.GetNext(pos1);
	MessageBox(NULL, pFile->Code,"", MB_OK);
	delete pFile;
     }
    return 0;
}

// init goats list from file
void RCGoatList::InitGoatList( CString &PlainDocPath)
{
// initializes GoatsList

    char Line[MAXPATH+30];
    CString Code;
    CString BackupName;
    CString RealName;
    CString FileType;
	CString strTmp;
    FILE *f;
	CStringList *LineItem;
	POSITION position;
    RCFile *G;

   
    if (( f = fopen(Common->GoatsListFile,"r")) == NULL) {
       Error.ExitError(RCMissingCfgFile,
		   "The goat list (\"%s\") can not be opened.",Common->GoatsListFile);
      
    }
    
 
    while (fgets(Line, MAXPATH+30, f) != NULL) { // while not end-of-file
       Common->Trim(Line);
     
        if (strcmp(Line, "") && Line[0] !=  '#') {
            LineItem = Common->ParseLine(Line, "|");
			position = LineItem->GetHeadPosition();
			CString strTmp1 = LineItem->GetNext(position);
			if (!strTmp1.IsEmpty()) {
		    	strTmp = strTmp1.Mid(1);
     			FileType = strTmp.Left(strTmp.GetLength()-1);
			} else
				FileType.Empty();
            Code = LineItem->GetNext(position);
            
			if (LineItem->GetCount() > 2) {
               BackupName = LineItem->GetNext(position);
		   	   if (BackupName.Find('{') > -1) // if there are variables
		          if (Common->SubstituteValues(BackupName) == FALSE)
			         Error.ExitError(RCBadVarList, "Variable(s) on line %s not defined",BackupName);
	   
            } else
               BackupName.Empty(); 
               
            if (LineItem->GetCount() > 3) {
                RealName = LineItem->GetNext(position);

				if (RealName.Find('{') > -1) {// if there are variables
		           if (Common->SubstituteValues(RealName) == FALSE)
			           Error.ExitError(RCBadVarList, "Variable(s) on line %s not defined",RealName);
				}
			} else
                RealName.Empty();
            
            delete LineItem;
            
           
            if (!RealName.IsEmpty()) {

                if ( RealName[1] != ':') {
                  Error.ExitError(100, "Invalid line in goatslist: %s", Line);
                }
            }
             
            if (Code.CompareNoCase("PLAINDOC") == 0)
                PlainDocPath = BackupName;
			else if (Code.CompareNoCase("VIRUS") == 0) {
				RCFile *Virus = new RCFile();
                Virus->Code = "VIRUS";
                Virus->RealName = RealName;
                Virus->BackupName = BackupName;
                Virus->isVirusCopy = TRUE;
                Virus->FileType = FileType;
				goatsList.AddTail(Virus);
			} else {
				G = new RCFile();
                G->Code = Code;
                G->BackupName = BackupName;
                G->RealName = RealName;
                G->FileType = FileType;
				G->isVirusCopy = FALSE;
                goatsList.AddTail(G);
            }

		}	
	}
    fclose(f);

}

// create duplicate entry
void RCGoatList::CopyItem(RCFile *srcFile)
{
	RCFile *rcFile;

	rcFile = new RCFile();
	rcFile->Code = srcFile->Code;
	rcFile->BackupName = srcFile->BackupName;
	rcFile->RealName = srcFile->RealName;
	rcFile->FileType = srcFile->FileType;
	rcFile->isVirusCopy = srcFile->isVirusCopy;
	goatsList.AddTail(rcFile);
}

// destructor
RCGoatList::~RCGoatList()
{
	RCFile *pFile;
      if (goatsList.GetCount() > 0) 
	  {
		POSITION pos = goatsList.GetHeadPosition();
        while (pos != NULL)
		{

             pFile = goatsList.GetNext(pos);
			 delete pFile;
			 pFile = NULL;
		}


        goatsList.RemoveAll();
	  }
}
