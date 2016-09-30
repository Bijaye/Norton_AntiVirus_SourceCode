/*******************************************************************
*                                                                  *
* File name:     RCFileList.cpp                                            *
*                                                                  *
* Description:   RC File list implementation                       *
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
#include "stdafx.h"
#include "afxtempl.h"

#include <iostream>
#include <fstream>
#include <ios>

#include <direct.h>
#include "defs.h"
#include "polymath.h"
#include "RCFile.h"
#include "RCError.h"
#include "RCFileList.h"


RCFile *RCFileList::FindFileByRealName(CString FileName)
{
	POSITION pos = filesList.GetHeadPosition();
	RCFile *pFile;

	
	while (pos != NULL)
	{
		CString strTmp;
		pFile = filesList.GetNext(pos);
 		strTmp = pFile->Name;

		if (FileName.CompareNoCase(strTmp) == 0) 
			return pFile;
	}
	return NULL;
}



// free the memory allocated for goat list entries
int RCFileList::RemoveAll()
{
     POSITION position = filesList.GetHeadPosition();
     POSITION pos1, pos2;
     RCFile* pFile;
    for (pos1 = filesList.GetHeadPosition(); (pos2 = pos1) != NULL;)
    {
	pFile = filesList.GetNext(pos1);
	filesList.RemoveAt(pos2);
	delete pFile;
     }
    return SUCCESS;
}


void RCFileList::AddTail(RCFile *pNewFile)
{
	filesList.AddTail(pNewFile);
}

void RCFileList::WriteAll()
{
   std::ofstream *dbFile;
   dbFile = new std::ofstream;
   dbFile->open(DBFILENAME, std::ios::out | std::ios::app);

   POSITION pos;

   for (pos = filesList.GetHeadPosition(); pos != NULL;)
   {
		RCFile *rcFile = filesList.GetNext(pos);
		char name[MAXPATH];
		GetShortPathName(rcFile->Name, name, MAXPATH);
		*dbFile << name <<' '<< rcFile->crc << ' ' 
			<< rcFile->size << ' ' << rcFile->TimeStamp 
			<< ' '  << rcFile->possibly_infected << '\n';
		dbFile->flush();
	}
	dbFile->close();
	delete dbFile;
}

int RCFileList::ReadAll()
{
    WIN32_FIND_DATA FindFileData;
	std::ifstream dbFile(DBFILENAME, std::ios::in);
    if (dbFile.fail()) return FAILURE;
	//dbFile.open(DBFILENAME, std::ios::in );
	if (!dbFile) return FAILURE;
	while (!dbFile.eof())
	{
		RCFile *rcFile = new RCFile();
		char name[MAXPATH];
		char longname[MAXPATH];
        
		dbFile >> name >> rcFile->crc >> rcFile->size 
			>> rcFile->TimeStamp >> rcFile->possibly_infected;
		
		//need to convert name to long name
		while (strchr(name, '~'))    //need to expand the name
		{
			//***here need to parse name, then do FindFirst for every
			//directory that has ~ 
			char part1[MAXPATH];
			char part2[MAXPATH];
			char tempname[MAXPATH];
			strcpy (tempname, name);
			
			char *p = strchr(tempname, '~');
			char *q = strchr(p, '\\');
			if (q != NULL) {
				strcpy(part2, q);
				*q = '\0';
			} else
				strcpy(part2,"");
            
			strcpy (part1, tempname);
			p = strrchr(part1, '\\');
			if (p !=NULL) *p = '\0';
			FindFirstFile(tempname, &FindFileData);
			strcpy(name, part1);
			strcat(name, "\\");
			strcat(name, FindFileData.cFileName);
			strcat(name, part2);

		}
		rcFile->Name = name;
		filesList.AddTail(rcFile);
	}
	dbFile.close();
	delete dbFile;
	unlink(DBFILENAME);
	return SUCCESS;
}
// destructor
RCFileList::~RCFileList()
{
	RCFile *pFile;
      if (filesList.GetCount() > 0) 
	  {
		POSITION pos = filesList.GetHeadPosition();
        while (pos != NULL)
		{

             pFile = filesList.GetNext(pos);
			 delete pFile;
			 pFile = NULL;
		}


        filesList.RemoveAll();
	  }
}
