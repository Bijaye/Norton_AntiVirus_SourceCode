// Copyright 1995 Symantec Corporation                                  
//***************************************************************************
// 
// $Header:   S:\navfamly\vcs\nlmload.h_v   1.2   29 Nov 1995 18:02:38   GDZIECI  $ 
// 
// Description:                                                         
//      Header file for autoload/unload NLM used by NAVEX.
// 
// See Also:                                                            
// 
//*************************************************************************** 
// $Log:   S:\navfamly\vcs\nlmload.h_v  $ 
// 
//    Rev 1.2   29 Nov 1995 18:02:38   GDZIECI
// Checking in from VIRLIB 6.
// 
//    Rev 1.1.2.0   24 Oct 1995 14:12:46   JJOHNSON
// Branch base for version QAK6
// 
//    Rev 1.1   24 Oct 1995 13:55:02   MLELE
// Added header info.
//*************************************************************************** 

#if !defined (_NLMLOAD_H)
   #define  _NLMLOAD_H


typedef struct {

   EXPORT_TABLE_TYPE  *prExportTable;
   VOID               (*lpFunc)(VOID);
   int                nStatus;

} INIT_PACKET_TYPE;


#endif      // .. _NLMLOAD_H
