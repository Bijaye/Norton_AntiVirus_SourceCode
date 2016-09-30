// Copyright 1992 Symantec, Peter Norton Group                          
//**********************************************************************
//                                                                      
// $Header:   S:/include/VCS/fuzyload.h_v   1.0   06 Feb 1997 20:52:02   RFULLER  $
//                                                                      
// Description: Prototypes for loadfile.c
//                                                                      
// See Also:                                                            
//                                                                      
//**********************************************************************
// $Log:   S:/include/VCS/fuzyload.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:52:02   RFULLER
// Initial revision
// 
//    Rev 1.0   16 Apr 1996 19:26:20   TSmith
// Initial revision.
// 
//    Rev 1.0   15 Aug 1994 16:14:54   GDZIECI
// Baseline Mario from Vishnu with Win32 awk changes applied
// 
//    Rev 1.1   16 Sep 1993 22:13:52   JMILLARD
// return error from InitializeFuzzyTables
// 
//    Rev 1.0   13 Apr 1993 22:19:08   JMILLARD
// Initial revision.
// 
/************************************************************************/

#ifndef _FUZYDATA_H
#define _FUZYDATA_H

//  .h files required to include this .h file
                                                
#ifndef INCL_PLATFORM
#error platform.h MUST be included before fuzydata.h
#endif

//**********************************************************************
//
//  defines
//
//**********************************************************************
         
#define LOAD_ERROR_NEGATIVE_LARGE   0
#define LOAD_ERROR_NEGATIVE_MEDIUM  1
#define LOAD_ERROR_NEGATIVE_SMALL   2
#define LOAD_ERROR_ZERO             3
#define LOAD_ERROR_POSITIVE_SMALL   4
#define LOAD_ERROR_POSITIVE_MEDIUM  5
#define LOAD_ERROR_POSITIVE_LARGE   6

#define LOAD_ERROR_VECTOR_SIZE      7


#define LOAD_CHANGE_NEGATIVE_LARGE  0
#define LOAD_CHANGE_NEGATIVE_MEDIUM 1
#define LOAD_CHANGE_NEGATIVE_SMALL  2
#define LOAD_CHANGE_ZERO            3
#define LOAD_CHANGE_POSITIVE_SMALL  4
#define LOAD_CHANGE_POSITIVE_MEDIUM 5
#define LOAD_CHANGE_POSITIVE_LARGE  6

#define LOAD_CHANGE_VECTOR_SIZE     7


#define DELAY_CHANGE_NEGATIVE_LARGE     0
#define DELAY_CHANGE_NEGATIVE_MEDIUM    1  
#define DELAY_CHANGE_NEGATIVE_SMALL     2
#define DELAY_CHANGE_ZERO               3
#define DELAY_CHANGE_POSITIVE_SMALL     4
#define DELAY_CHANGE_POSITIVE_MEDIUM    5
#define DELAY_CHANGE_POSITIVE_LARGE     6

#define DELAY_CHANGE_VECTOR_SIZE        7


// input/output domain definitions

#define LOAD_DOMAIN_MIN     -100
#define LOAD_DOMAIN_MAX      100

#define DELAY_CHANGE_MIN    -100
#define DELAY_CHANGE_MAX     100


//**********************************************************************
//
//  Typedefs
//
//**********************************************************************
         



//**********************************************************************
//
//  Prototypes
//
//**********************************************************************
         
WORD InitializeFuzzyTables( VOID );

VOID FreeFuzzyTables( VOID );

SDWORD GetNewDelay( LPDWORD  lpdwDelay,
                    DWORD    dwTargetLoad,
                    WORD     wServerLoad,
                    SWORD    swServerLoadChange );

#endif  // _FUZYDATA_H
