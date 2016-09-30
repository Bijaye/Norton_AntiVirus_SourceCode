// Copyright 1995 Symantec Corporation                                  
//***************************************************************************
// 
// $Header:   S:/NAVEX/VCS/nlmload.h_v   1.2   21 Nov 1996 17:15:02   AOONWAL  $ 
// 
// Description:                                                         
//  Header for NLM NAVEX.
// 
// See Also:                                                            
// 
//*************************************************************************** 
// $Log:   S:/NAVEX/VCS/nlmload.h_v  $ 
// 
//    Rev 1.2   21 Nov 1996 17:15:02   AOONWAL
// No change.
// 
//    Rev 1.1   29 Oct 1996 12:59:56   AOONWAL
// No change.
// 
//    Rev 1.0   18 Oct 1995 11:43:44   CNACHEN
// Initial revision.
//*************************************************************************** 

#if !defined (_NLMLOAD_H)
   #define  _NLMLOAD_H


typedef struct {

   EXPORT_TABLE_TYPE  *prExportTable;
   VOID               (*lpFunc)(VOID);
   int                nStatus;

} INIT_PACKET_TYPE;


#endif      // .. _NLMLOAD_H
