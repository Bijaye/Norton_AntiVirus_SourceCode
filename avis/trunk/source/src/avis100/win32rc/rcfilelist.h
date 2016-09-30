#ifndef RCFILELIST_H
#define RCFILELIST_H

class RCFileList
{
/*******************************************************************
*                                                                  *
* File name:     rcfilelist.h                                      *
*                                                                  *
* Description:   RCFileList class definition file                  *
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
* Function:      class definition for rc goat list                 *
*                                                                  *
*******************************************************************/
public:
// the list of the goats
//initialized in accordance with the file Goat.lst
//then updated to take into account the renameing made by viruses

	RCFileList() {};
     ~RCFileList();
     RCFile* FindFileByCode(CString Code);
	 POSITION FindPosByCode(CString & Code);
	 RCFile *RCFileList::FindFileByRealName(CString FileName);
	 void AddTail(RCFile *pNewFile);
     Prnt();
    int RemoveAll();
	 void CopyItem(RCFile *srcItem);
	 void WriteAll();
	 int ReadAll();
private:
	 RCError Error;
 	 RCFList filesList;

	 
};
#endif
