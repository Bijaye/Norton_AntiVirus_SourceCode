/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


#ifndef _SERVERTHREADS_H_
#define _SERVERTHREADS_H_

// 
// Main server listener thread.
// 
void IP_ListenerThread( DWORD dwData );
void SPX_ListenerThread( DWORD dwData );
void WorkerThread( DWORD dwData );
void ConfigWatchThread( DWORD dwData );
void ScanSamplesForAlerts( DWORD dwData );



#endif