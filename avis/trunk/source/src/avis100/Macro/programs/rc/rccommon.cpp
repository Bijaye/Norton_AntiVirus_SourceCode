/*******************************************************************
*                                                                  *
* File name:     RCCommon.cpp                                      *
*                                                                  *
* Description:   Common functions for the RC program               *
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
* Function:      Encapsulates common functions and variables of    *
*                the RC program to help eliminate the need for     *
*                global variables                                  *
*                                                                  *
*******************************************************************/

#include "afxtempl.h"
#include <windows.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <io.h>
#include <errno.h>
#include <direct.h>
#include "rcdefs.h"
#include "errors.h"
#include "RCCommon.h"

// constructor
RCCommon::RCCommon()
{
   DBCSMode = FALSE;
   TimeoutInterval = 500;
   WaitIdleTime = 5000;
}

// breaks Line in tokens separated by separator Sep
CStringList *RCCommon::ParseLine(char * Line, LPCTSTR Sep )
{
   CStringList *LineItem;
   int position;
   CString token;
       
   LineItem = new CStringList;
   CString templine(Line);
   templine.TrimLeft();
   templine.TrimRight();
   try {
	   while (!templine.IsEmpty()) {
        position = templine.Find(Sep);  
        if (position != -1) {
          token = templine.Left( position);
          templine = templine.Mid(position+1);
        } else {
          token = templine;
          templine.Empty();
        }
        LineItem->AddTail(token);
       } 
  }
  catch (CMemoryException& exc){
    PostQuitMessage(RCNoMemory);
  }
  return LineItem;
}

// Copies src file to dst file in binary mode
int RCCommon::FileCopy (LPCTSTR src, LPCTSTR dst)
{
 int numItems;
 FILE *fp_src, *fp_dst;
 char *buffr;  // Use 16k buffer for faster file copy

  buffr = (char *) malloc (BUFF_SIZE);

  if (buffr == NULL) {
    return FALSE;
  }

  if ((fp_src = fopen (src,"rb"))==NULL)
  {
    free (buffr);
    return FALSE;
  }
  if ((fp_dst = fopen (dst,"wb"))==NULL)
  {
     free (buffr);
    return FALSE;
  }


  while ((numItems = fread (buffr, 1, BUFF_SIZE, fp_src)) == BUFF_SIZE)
  {
        fwrite (buffr, 1, BUFF_SIZE, fp_dst);
   }


  fwrite (buffr, 1, numItems, fp_dst);

  free (buffr);

  fflush (fp_dst);
//  setftime (fileno (fp_dst), &ft);

  fclose (fp_src);

  fclose (fp_dst);

  return TRUE;

}

// sets attributes for a file
void RCCommon::SetAttr(LPCTSTR file_name, BOOL readonly)
{
//   unsigned attrib;
   readonly = FALSE;
   struct stat buf;

   if (stat(file_name, &buf) == 0 ) {
      if (!readonly && !(buf.st_mode&S_IWRITE)) {
         chmod(file_name, S_IWRITE );
         
       }
      
      if (readonly)  chmod(file_name, S_IREAD );
 
   } /* endif */

  
} /*SetAttr*/
/*-----------------------------------------------------------------------------
Function Name:  void AppendPath(char * strFirst, char * strSec)

Input Parameters:
        strFirst: Specifies the first path to which second path is to be appended.
        strSecond: Specifies the path to be appenced to lpszFirst

Output Parameters:
        strFirst: Containst he resultant path on return.

Return Value:
        None

Description:
        This function combines two paths and takes care of trailing
        slashes in both of them. It ensures that when the two
        paths are appended, there is a single slash separating
        them. For ex: if first path has a trailing slash and
        second path has a starting slash, it ensures to remove
        one of the slashes. Similarly, if first path does not
        have a trailing slash and second path does not have
        a starrting slash, a single slash is appended to the first
         path before appending the second path.

-----------------------------------------------------------------------------*/

// Appends sec path to first and takes care of trailing slashes.
void RCCommon::AppendPath(char *strFirst,LPCTSTR strSec)
{
        if(strFirst == NULL || strSec == NULL) return;

        int nLen1 = strlen(strFirst);

        // If last char in First is not a slash and first char
        // in Sec is not a slash, append slash.

        if(strFirst[nLen1-1] != '\\' && strSec[0] != '\\')
                strcat(strFirst,"\\");

        // If both of then have a slash, remove one of them.

        else if (strFirst[nLen1-1] == '\\' && strSec[0] == '\\')
                strFirst[nLen1-1] = 0;

        strcat(strFirst,strSec);
}

// remove all files from a directory
void RCCommon::ClearDir(LPCTSTR DirName)
{

    if(DirName == NULL)
           return;   // invalid parameter

     // Check if directory exists.
      if(access(DirName,00) != 0)
        return;

     //Search thru the directory and delete all its contents.

        struct _finddata_t tFileBlk;

        char szTmp[MAXPATH]; // temporary for directory
        char szTmpBuf[MAXPATH]; // temporary for directory
        unsigned int nRet; // return code from findnext
        long hFile;

        strcpy(szTmp,DirName);
        strcpy(szTmpBuf,DirName);
        AppendPath(szTmp,"\\*.*");

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
                          //Nothing to do now.
                  }
                  else
                  {
                         // not a subdirectory
                         strcpy(szTmp,szTmpBuf);
                         AppendPath(szTmp,tFileBlk.name);
                          SetAttr(tFileBlk.name,0); // in case file is read-only
                         remove(szTmp);
                  }
                }

                nRet = _findnext(hFile, &tFileBlk);

        }

        _findclose(hFile);

        return;
}

/*
 * get file name without either the directory or the extension
 * filename - full name of the file
 * result - resulting name
 */
void RCCommon::GetFileName(CString filename, CString &result) 
{
   char *p, *s;

   char *temp;

   temp = new char[filename.GetLength() +1];
   strcpy(temp,filename);
	

   s = strrchr(temp,'\\');
   p = NULL;
   if (s == NULL) s = temp;
   else s++;
   if (s != NULL) p = strrchr(s, '.');

   if (p != NULL) *p = '\0';
   result = s;

   delete temp;

}

/*
 * extract file directory from its full name
 * filename - full name of the file
 * result - resulting directory name
 */
void RCCommon::GetFileDir(CString filename, CString& result) 
{ 
   char *p, *s;

   s = new char[strlen(filename) +1];
   strcpy(s, filename);
   p = strrchr(s, '\\');
    
   if (p != NULL) *p = '\0';

   result = s;
   delete s;
}

/*
 * extract file extension without either the directory or the extension
 * filename - full name of the file
 * result - resulting extension
 */
void RCCommon::GetFileExt(CString filename, CString& result) 
{
   char *p, *s;
     
    s = new char[strlen(filename) +1];
    strcpy(s, filename);
    p = strrchr(filename, '.');
      
    if (p == NULL)
        strcpy(s, "");
    else
        strcpy(s, p+1);

     result = s; 
	 delete s;
}

/*
 * get file name with extension without the directory 
 * filename - full name of the file
 * result - resulting name
 */

void RCCommon::GetFileWName(CString filename, CString & result) 
{
	CString s, q, t;
	
    GetFileName(filename,s);
	GetFileExt(filename,q);

    if (!q.IsEmpty()) {
        t = s + "." + q;
		
		result = t;
	} else 
		result = s;  

}

/*
 * determine if the file is readable
 * filename - full name of the file
 */

BOOL RCCommon::isFileReadable(CString filename) 
{
      FILE *f;
  
       if ((f = fopen(filename,"r")) == NULL)
            return FALSE;
       else {
           fclose(f);
           return(TRUE);
       }
}

// Remove leading and trailing whitespace from line
void RCCommon::Trim(char *Line)
{
	CString strTmp(Line);
    strTmp.TrimLeft();
	strTmp.TrimRight();
	strcpy(Line,strTmp);
} 

//Substitute value for the variable name in string
BOOL RCCommon::SubstituteValues(CString &strLine)
{
	CString strTmp;
	CString strVar;
	
	int start, end;

	while ((start = strLine.Find('{')) > -1) {
	  if (start > 0) // not at first position
		strTmp = strLine.Left(start); // copy first part
      else
	 	strTmp.Empty();
      
      end = strLine.Find('}'); // find end of var
	  strVar = strLine.Mid(start+1, end-start-1);
	  if (VarList[strVar].IsEmpty())  // no such variable defined
	      return FALSE;
	   else 
	      strTmp += VarList[strVar]; // substituted variable

	   if (end < strLine.GetLength()-1)
	      strTmp += strLine.Mid(end+1);
	   
	   strLine = strTmp;
	}

	return TRUE;
}

// substitute values for variable names in szLine
BOOL RCCommon::SubstituteValues(char *szLine)
{
	CString strTmp(szLine);
	if (SubstituteValues(strTmp) == TRUE) 
		strcpy(szLine, strTmp);
	else
		return FALSE;
	return TRUE;
}
