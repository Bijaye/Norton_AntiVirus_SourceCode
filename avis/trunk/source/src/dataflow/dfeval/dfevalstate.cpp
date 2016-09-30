/*----------------------------------------------------------------------------*/
/*                                                                            */
/* FILE:      DFEvalState.cppp                                                */
/* FUNCTION:  To create a state object                                        */
/*                                                                            */
/* ROUTINES CONTAINED WITHIN FILE:                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* NOTES:                                                                     */
/*----------------------------------------------------------------------------*/
/* HISTORY: AUGUST, 1998  SNK                                                 */
/*----------------------------------------------------------------------------*/
#include "afxtempl.h"

#ifndef DFEVALSTATUS_H
   #include "DFEvalStatus.h"
#endif


#ifndef DFEVALSTATE_H
   #include "DFEvalState.h"
#endif


/*----------------------------------------------------------------------------*/
/* Procedure name:      DFEvalState                                           */
/* Description:         Destructor                                            */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              None                                                  */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
DFEvalState::~DFEvalState()
{
    // JALAN: Tried to free the memory used by statusObjects in the
    // statusList. It does remove some of the memory leaks but the
    // application gives error after running for some time. I don't 
    // think that there is any problem in freeing the memory now but
    // we will wait and check the application with Bounds Checker.
    /*
	POSITION pos;

	pos = statusList.GetHeadPosition();
	int i = 0;
	DFEvalStatus* statusObject;
	while (pos != NULL)
	{
		statusObject = statusList.GetNext(pos);
        if (statusObject)
            delete statusObject;
	}
    */
	DFEvalStatus* statusObject;

	while (statusList.GetCount() != 0) {
		statusObject = statusList.GetTail();
		statusList.RemoveTail();
		delete statusObject;
	}
				
}	
/*----------------------------------------------------------------------------*/
/* Procedure name:      DFEvalState                                           */
/* Description:         Constructor                                           */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              None                                                  */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
DFEvalState::DFEvalState():
  IsUsed(FALSE)

{
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      DFEvalState                                          */
/* Description:         Constructor                                           */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              None                                                  */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
DFEvalState::DFEvalState(CString stringState, int number, CString stateType, CString stateGroup, CString startStatus, CString status)
{
//	stringState.MakeUpper();
	stateType.MakeUpper();
	stateGroup.MakeUpper();

	strState = stringState;
	state = number;
	type = GetDefinedStateType(stateType);
	group = GetDefinedStateGroup(stateGroup);
	processingStatus = startStatus;
	endStatus = status;
	IsUsed = FALSE;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      FindObjectWithStatus                                  */
/* Description:         To find a status object in the 'Status' collection    */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              pointer to DFEvalStatus object                        */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/

DFEvalStatus* DFEvalState::FindObjectWithStatus(CString status)
{
		POSITION pos;

        status.MakeUpper();        
		pos = statusList.GetHeadPosition();
	    int i = 0;
		DFEvalStatus* statusObject;
		while (pos != NULL)
		{
			statusObject = statusList.GetNext(pos);
			CString statusInObject  = statusObject->GetStatus();
			if (statusInObject == status)
               return statusObject;
		
		}

		return NULL;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      GetDefinedStateType                                   */
/* Description:         To get a type of the state                            */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              state type: INITIAL, PROCESSING, FINAL, TRANSIT,      */
/*                       OR UNDEFINED                                         */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
DFEvalState::StateType DFEvalState::GetDefinedStateType(CString stateType)
{
   stateType.MakeUpper();
   
   if (stateType == CString("INITIAL"))
	   return INITIAL;

   if (stateType == CString("PROCESSING"))
	   return PROCESSING;

   if (stateType == CString("FINAL"))
	   return FINAL;

   return UNDEFINED;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      GetDefinedStateGroup                                  */
/* Description:         To get a type of the group                            */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              state type: ANALYIS, BUILD, FULLDEFBUILD, DEFER,      */
/*                      ARCHIVE, SCAN, STOP, NONE                             */  
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
DFEvalState::StateGroup DFEvalState::GetDefinedStateGroup(CString stateGroup)
{
   stateGroup.MakeUpper();
   
   if (stateGroup == CString("ANALYSIS"))
	   return ANALYSIS;

   if (stateGroup == CString("BUILD"))
	   return BUILD;

   if (stateGroup == CString("FULLBUILD"))
	   return FULLBUILD;

   if (stateGroup == CString("DEFER"))
	   return DEFER;

   if (stateGroup == CString("ARCHIVE"))
	   return ARCHIVE;

   if (stateGroup == CString("STOP"))
	   return STOP;

   if (stateGroup == CString("SCAN"))
	   return SCAN;

   if (stateGroup == CString("IMPORT"))
	   return IMPORT;

   if (stateGroup == CString("UPDATE"))
	   return UPDATE;

   if (stateGroup == CString("UNDEFER"))
	   return UNDEFER;

   if (stateGroup == CString("COLLECT"))
	   return COLLECT;

   return NONE;
}
