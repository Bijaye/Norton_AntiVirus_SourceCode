// Copyright 2004 Symantec Corporation. All rights reserved.

#ifndef _ndkConnections_h_
#define _ndkConnections_h_

#include "ndkDefinitions.h"

#ifdef __cplusplus
extern "C" {
#endif

/*connections*/
//AllocateBlockOfTasks
//GetConnectionInformation
//GetConnectionNumber
//GetInternetAddress
//GetMaximumNumberOfStations
//ReturnBlockOfTasks
//ReturnConnection
//SendBroadcastMessage
//SetCurrentConnection
//SetCurrentTask
LONG AllocateBlockOfTasks(LONG numberWanted);
int GetConnectionInformation(WORD connectionNumber, char *objectName , WORD *objectType, long *objectID, BYTE *loginTime);  
WORD GetConnectionNumber(void);
int GetInternetAddress(WORD connectionNumber, char *networkNumber, char *nodeAddress);
int GetMaximumNumberOfStations (void); 
int ReturnBlockOfTasks(LONG startingTask, LONG numberOfTasks);
int ReturnConnection(LONG connectionNumber);
int SendBroadcastMessage(char *message, WORD *connectionList, BYTE *resultList, WORD connectionCount); 
LONG SetCurrentConnection(LONG connectionNumber);
LONG SetCurrentTask(LONG taskNumber);

#ifdef __cplusplus
}
#endif
	
#endif // _ndkConnections_h_
