
#ifndef _SERVERTHREADS_H_
#define _SERVERTHREADS_H_

// 
// Main server listener thread.
// 
void IP_ListenerThread( DWORD dwData );
void SPX_ListenerThread( DWORD dwData );
void WorkerThread( DWORD dwData );
void ConfigWatchThread( DWORD dwData );




#endif