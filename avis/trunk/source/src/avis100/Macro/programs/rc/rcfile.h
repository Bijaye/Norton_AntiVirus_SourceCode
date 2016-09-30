/*******************************************************************
*                                                                  *
* File name:     rcfile.h                                          *
*                                                                  *
* Description:   RCFile class definition file                      *
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
* Function:      class definition for goat list collection object  *
*                                                                  *
*******************************************************************/
#ifndef RCFILE_H
#define RCFILE_H

class RCFile {

public:
                  CString Code;
                  CString BackupName;
                  CString RealName;
                  CString FileType;    //**'DOC/GLOBAL(/startup?)
                  BOOL isVirusCopy;
                  BOOL isLegal;
				  RCFile() {}
	  
                  RCFile(const RCFile& );
    //RCFile &operator =(const RCFile &);
 
};

typedef CTypedPtrList<CPtrList, RCFile*> RCFileList;
#endif 
