/*******************************************************************
*                                                                  *
* File name:     rcfile.cpp                                            *
*                                                                  *
* Description:   rcFile implementation                             *
*                controller for the macro viruses                  *
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
* Function:      Implements RCFile class for containing information*
*                about goat files                                  *
*                                                                  *
*******************************************************************/

#include "afxtempl.h"
#include "RCFile.h"

//implement simple copy constructor
RCFile:: RCFile( const RCFile& oldFile)
{
  Code = oldFile.Code;
  BackupName = oldFile.BackupName;
  RealName = oldFile.RealName;
  FileType = oldFile.FileType;  // DOC/GLOBAL/VIRUS
  isVirusCopy = oldFile.isVirusCopy;
  isLegal = oldFile.isLegal;
}
