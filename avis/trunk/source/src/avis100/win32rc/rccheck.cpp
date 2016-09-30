/*******************************************************************
*                                                                  *
* File name:     RCCheck.cpp                                       *
*                                                                  *
* Description:   RC checkup                                        *
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
* Function:                         *
*                                                                  *
*******************************************************************/
#include "stdafx.h"
#include "afxtempl.h"
#include <windows.h>                     
#include <stdlib.h>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include <share.h>

#include "defs.h"
#include "polymath.h"
#include "rcerror.h"
#include "rcfile.h"
#include "rcfilelist.h"
#include "RCCheck.h"


void RCCheck::Init(CString DirName, CStringList *pSecDirsList, CString WindowsDir, bool trace)
{
   m_GoatsDir = DirName;
   m_pGoatsDirs2 = pSecDirsList;
   m_WindowsDir = WindowsDir;
   m_trace = trace;
}

// destructor
RCCheck::~RCCheck()
{
	m_FileList.RemoveAll();
}

int RCCheck::InitFileList(bool after_reboot)
{
    poly            crc_key;
    PWB             *passwd;
    unsigned int i;

    char *password = "koshechka";

	/* Initialize CRC table */

   if(!(passwd = (PWB *) malloc(PWB_SIZE)))
   {
      PostQuitMessage(0); /* no memory */
	  return -1;
   }	  

   for(i=0; i<strlen(password); i++)
      passwd->db[i]=password[i];

   for(i=strlen(password); i<PWB_SIZE; i++)
      passwd->db[i]=0;

   crc_key = new_poly(passwd);
   m_crc_table = crc_init(crc_key);

   free(passwd);
   if (after_reboot) 
	 return ReadFilesInfo();
   else {
    if (m_GoatsDir != m_WindowsDir) //no need to process the same directory twice
	  ProcessDir(m_GoatsDir, 0, 1);
	POSITION pos;
	CString strDir;
	for (pos = m_pGoatsDirs2->GetHeadPosition(); pos!= NULL;)
	{
		strDir = m_pGoatsDirs2->GetNext(pos);
		if (strDir != m_WindowsDir)
			ProcessDir(strDir,0,1);
	}
    ProcessDir(m_WindowsDir, 1, 1); 
   }
   return SUCCESS;
}

void RCCheck::CheckFiles(CStringList *pNewFiles, CStringList *pChangedFiles)
{
    m_pNewFiles = pNewFiles;
	m_pChangedFiles = pChangedFiles;
    if (m_GoatsDir != m_WindowsDir) //no need to process the same directory twice
        ProcessDir(m_GoatsDir, 0, 0);

	POSITION pos;
	CString strDir;
	for (pos = m_pGoatsDirs2->GetHeadPosition(); pos!= NULL;)
	{
		strDir = m_pGoatsDirs2->GetNext(pos);
		if (strDir != m_WindowsDir)
			ProcessDir(strDir,0,1);
	}

	ProcessDir(m_WindowsDir, 1, 0);
}

void RCCheck::ProcessDir(CString DirName, BOOL check_subdirs, BOOL initialize)
{
   // initializes FilesList

    CString strTmp;
	poly newCrc;
    _fsize_t newSize;
    time_t newTimeStamp;



    if(DirName == "") 
        return;   // invalid parameter

     // Check if directory exists.
    if(access(DirName,00) != 0)
        return;
      
   //Search thru the directory and delete all its contents.

    struct _finddata_t tFileBlk;
    char szTmp[260]; // temporary for directory

    unsigned int nRet; // return code from findnext
    long hFile;

    strcpy(szTmp,DirName);
    strcat(szTmp,"\\*.*");
    
    hFile = _findfirst(szTmp,&tFileBlk);
    nRet = (hFile == -1) ? 1 : 0;


    while(nRet == 0 )
    {
                // skip . and .. entries

         strupr(tFileBlk.name);

         if (strcmp(tFileBlk.name,".") && strcmp(tFileBlk.name,".."))
         {
              if(tFileBlk.attrib & _A_SUBDIR )
              {
                          // subdirectory
				  if (check_subdirs) {
					  CString strNewDir;
					  strNewDir = DirName + '\\' + tFileBlk.name;
      				  ProcessDir(strNewDir,1,initialize);
				  }
                          //else - Nothing to do now.
               }
               else if (IncludedFile(tFileBlk.name)) 
               {
                    // not a subdirectory - add to file list if not already there
                   CString strName;
	               strName = DirName + '\\' + tFileBlk.name;
                   strName.MakeUpper();                       
				   RCFile *rcFile;
                   
				   
				   if (initialize || 
					    (rcFile = m_FileList.FindFileByRealName(strName)) == NULL) 
				   {
                      rcFile = new RCFile();
                      rcFile->Name = strName;
				      rcFile->size = tFileBlk.size;
                      rcFile->TimeStamp = tFileBlk.time_write;
                      rcFile->crc = GetCrc(strName);
					  if (initialize)
				        rcFile->possibly_infected = FALSE;
					  else  { //new file
                        if (m_trace)
                          rcTrace.ReportError("found new file %s\n", strName); 
                        rcFile->possibly_infected = TRUE; 
						m_pNewFiles->AddTail(strName);
					  }
                      m_FileList.AddTail(rcFile);
				   } else if (!rcFile->possibly_infected) { //check for changes, only bother if not already changed
					   newSize = tFileBlk.size;
					   newTimeStamp = tFileBlk.time_write;
					   newCrc = GetCrc(strName);
					      // for now let's not worry about the date, for if only
					      // the date is changed it is probably not a virus
                       if (rcFile->size != newSize || rcFile->crc != newCrc) {
						   rcFile->possibly_infected = TRUE;
                           if (m_trace)
						     rcTrace.ReportError("adding change file %s to list\n", strName);

						   m_pChangedFiles->AddTail(strName);
					   }
					   rcFile->size = newSize;
					   rcFile->TimeStamp = newTimeStamp;
					   rcFile->crc = newCrc;
				   }
                }
		 }
    
                nRet = _findnext(hFile, &tFileBlk);

	}

     _findclose(hFile);

     return;   

}

BOOL RCCheck::IncludedFile(CString FileName)
{
	CString strExt;

  
	int i = FileName.Find('.');
	strExt = FileName.Right(FileName.GetLength()-i);
	strExt.MakeUpper();
    if (strExt == ".EXE" ||
		strExt == ".VXD" ||
		strExt == ".COM" ||
		strExt == ".BIN" ||
		strExt == ".DLL" ||
		strExt == ".INI") // may include others later; include .INI to notice
		                  // new wininit.ini file
		return TRUE;
	else
		return FALSE;

}

poly RCCheck::GetCrc(CString FileName)
{
	HFILE handle;
	int   bytes;      /* Number of bytes read */
	int crc_bytes;
	byte  buf[513];
    int size = 512;
  
#define T_WINNT
     //open the file
#ifdef T_WINNT
   if (-1 == (handle = sopen(FileName, O_RDONLY|O_BINARY, SH_DENYNO)))
#else
   if (-1 == (handle = open(FileName, O_RDONLY|O_BINARY|SH_DENYNO)))
#endif
       return 0; // ignore crc if unable to open file

    // read first 512 bytes of file into the buffer
    //* for now, will only take crc of the first 512 bytes
     memset (buf, 0, size); // zero out 
	 errno = 0; 
     bytes = read(handle, buf, size);
     if (bytes<=0 && errno!=0) return 0; /* An error happened during read */
        if(bytes > 512)
         crc_bytes = 512;
      else
         crc_bytes = bytes;

	  close(handle);
      return crc_calc(m_crc_table, 0L, buf, crc_bytes); 
}

// save all file information into a file to use after reboot
void RCCheck::WriteFilesInfo()
{
	m_FileList.WriteAll();
}

int RCCheck::ReadFilesInfo()
{
	return m_FileList.ReadAll();
}
