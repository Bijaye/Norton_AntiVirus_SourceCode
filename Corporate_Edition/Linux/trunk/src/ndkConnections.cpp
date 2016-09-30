//ndk(Connections) stubs
#include <ndkFunctions.h>

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

LONG AllocateBlockOfTasks(LONG numberWanted) 
{
	return 0;
}

int GetConnectionInformation (WORD connectionNumber, char *objectName , WORD *objectType, long *objectID, BYTE *loginTime)  
{
	return ERROR_SUCCESS;
}

WORD GetConnectionNumber (void)
{
	return 0;
}

int GetInternetAddress (WORD connectionNumber, char *networkNumber, char *nodeAddress)
{
	return ERROR_SUCCESS;
}

int GetMaximumNumberOfStations (void)
{
	return 0;
}
int ReturnBlockOfTasks(LONG startingTask, LONG numberOfTasks)
{
	return ERROR_SUCCESS;
}
int ReturnConnection(LONG connectionNumber)
{
	return ERROR_SUCCESS;
}
int SendBroadcastMessage (char *message, WORD *connectionList, BYTE *resultList, WORD connectionCount) 
{
	return ERROR_SUCCESS;
}
LONG SetCurrentConnection(LONG connectionNumber)
{
	return 0;
}
LONG SetCurrentTask(LONG taskNumber) 
{
	return 0;
}
 

