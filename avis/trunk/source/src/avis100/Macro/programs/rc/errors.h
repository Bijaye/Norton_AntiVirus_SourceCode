/*******************************************************************
*                                                                  *
* File name:     errors.h                                          *
*                                                                  *
* Description:   RC error codes                                    *
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
* Function:      Contain return and error codes used in rc program *
*                                                                  *
*******************************************************************/
#ifndef ERRORS_H
#define ERRORS_H
const int RCOk = 0;
const int RCMissingCfgFile = 10;
const int RCAppStart = 20;
const int RCKillPopupStart = 30;
const int RCBadPath = 40;
const int RCTimeOut = 100;
const int RCDdeInitialize = 50;
const int RCBadAppName = 60;
const int RCFileNotFound = 70;
const int RCCmdFileError = 80;
const int RCBadVarList = 90;
const int RCNoVirusName = 100;
const int RCNoMemory = 999;

const int Err_OK = 0;
const int Err_App_Not_Running = 101;
const int Err_Macro_Running = 102;
const int Err_No_Doc_Open = 103;
const int Err_Active_Doc_Not_Match = 104;
const int Err_Target_Doc_Not_Active = 105;
const int Err_DDE = 106;
const int Err_Unknown = 107;
const int Err_TimeOut = 108;
const int Err_DlgBox_Timeout = 109;
const int Err_Unable_Read_File = 110;
const int Err_Missing_DlgBox = 111;
const int Err_DlgBox_Not_Close = 112;
const int Err_OleError = 113;
const int Err_DroppedFile = 114;
const int Err_No_Change = 0;
const int Err_Ext_Changed = 1;
const int Err_Not_Same_Names = 2;
const int Err_Virus_Name = 3;
#endif
