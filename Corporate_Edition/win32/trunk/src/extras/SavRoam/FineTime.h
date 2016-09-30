// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef __FINE_TIME_H_
#define __FINE_TIME_H_

enum FineLinearTimeModes { TICKS, MILLISECONDS, DECIMILLISECONDS, CENTIMILLISECONDS, MICROSECONDS };

void SetFineLinearTimeMode( int nMode );
DWORD ElapsedFineLinearTime( DWORD dwStartTime, DWORD dwEndTime );
DWORD GetFineLinearTime( VOID );

                                              
#endif // __FINE_TIME_H_


