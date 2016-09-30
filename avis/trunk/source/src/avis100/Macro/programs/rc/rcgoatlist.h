#ifndef RCGOATLIST_H
#define RCGOATLIST_H

class RCGoatList
{
/*******************************************************************
*                                                                  *
* File name:     rcgoatlist.h                                      *
*                                                                  *
* Description:   RCGoatList class definition file                  *
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

     RCGoatList(RCCommon *Common);
     ~RCGoatList();
     RCFile* FindFileByCode(CString Code);
	 POSITION FindPosByCode(CString & Code);
	 RCFile *RCGoatList::FindFileByRealName(CString FileName);
     Prnt();
	 void InitGoatList(CString &PlainDocPath);
    int RemoveAll();
	 RCFileList goatsList;
	 void CopyItem(RCFile *srcItem);
private:
	 RCCommon *Common;
	 RCError Error;
	
	 
};
#endif
