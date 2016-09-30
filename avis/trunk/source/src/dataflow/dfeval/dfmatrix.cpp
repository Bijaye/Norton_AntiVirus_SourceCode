/*----------------------------------------------------------------------------*/
/*                                                                            */
/* FILE:      DFMatrix.cpp                                                    */
/* FUNCTION:  To create the Matrix                                            */
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
#include "afxwin.h"
#include "afxtempl.h"
#include <iostream>
#include <fstream>
#include <ios>
#include <strstream>
#include "DFEvalStatus.h"
#include "DFEvalState.h"
#include "DFMatrix.h"
#include "DFEvalError.h"
/*----------------------------------------------------------------------------*/
/* Procedure name:      DFMatrix                                              */
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
DFMatrix::DFMatrix():
  tptr(NULL),
  tptrmap(NULL)
{
}

/*----------------------------------------------------------------------------*/
/* Procedure name:      DFMatrix                                              */
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
DFMatrix::~DFMatrix()
{
   if (tptr)
      delete tptr;
   if (tptrmap)
      delete tptrmap;

	DFEvalState* stateObject;

	while (stateList.GetCount() != 0) {
		stateObject = stateList.GetTail();
		stateList.RemoveTail();
		delete stateObject;
	}

}   
/*----------------------------------------------------------------------------*/
/* Procedure name:      Init                                                  */
/* Description:         To initialize the Matrix                              */
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
int DFMatrix::Init()
{

	char evalPath[1024];


    HINSTANCE hInst = AfxGetInstanceHandle();
    (void) GetModuleFileName(hInst, evalPath, 1024);
    char *p = strrchr(evalPath, '\\');
    *p = '\0';
	AppendPath(evalPath, EVALUATORDATA);
   
	std::ifstream ifile(evalPath, std::ios::in);
	if (!ifile) {
	  return(FILE_OPEN_ERROR);
	}
   

	std::ostrstream inStore;
	const len = 512;
	char inBuf[len];
	int wordCnt = 0;
//	while ( inStore && ifile >> setw(len) >> inBuf ) {
	while ( inStore && ifile >> inBuf ) {
        ++ wordCnt;
		inStore << inBuf << std::ends;
	}
	
	char *ptr = inStore.str();
	tptr = ptr;

	char **words = new char* [wordCnt];

 	for (int i = 0; i < wordCnt; i++) {
		words[i] = ptr;
		ptr += strlen(ptr) + 1;
    }

	// Create State List
	DFEvalState *stateObject; 
	int mappingNumber = 1;

	for (i = 0; i < wordCnt; i++) {
		if ( i % 7 == 0 ) {
		   if ( mappingNumber != 0 ) { 
               //char *state = _strupr(words[i]); 

			   char *type = _strupr(words[i+1]);
			   char *group = _strupr(words[i+2]);
			   //char *status1 = _strupr(words[i+5]);
			   //char *status2 = _strupr(words[i+6]);

			   stateObject = FindObjectWithState(words[i]);
			   if (stateObject == NULL) {
			      stateObject = new DFEvalState(words[i], mappingNumber, type, group, words[i+5], words[i+6]);
                  stateList.AddTail(stateObject);			   
		     	  mappingNumber++;
				  
			   }
		   }
		}
	}

    // Create status collections inside state objects 
	DFEvalStatus *statusObject;
	for (i = 0; i < wordCnt ; i++) {
		if ( i % 7 == 0 ) {
		   int mappingNumber = FindMappingNumber(words[i]);
		   if ( mappingNumber != 0 ) { 
			   DFEvalState *stateObject = FindObjectWithState(mappingNumber);
			   if (stateObject != NULL) {
                   statusObject = stateObject->FindObjectWithStatus(words[i + 3]);
				   if (statusObject == NULL) {
				     mappingNumber = FindMappingNumber(words[i + 4]);					 
					 if(mappingNumber != 0) {
						  statusObject = new DFEvalStatus(words[i+3], mappingNumber, words[i + 4]);
						  stateObject->statusList.AddTail(statusObject);
					 }
                   } 
			   }    			   
			   
		   }
		}
	}
	ifile.close();
	if (!words)
		delete [] words;
//	SeeStateCollection();
	return 0;

}
/*----------------------------------------------------------------------------*/
/* Procedure name:      FindObjectWithState                                   */
/* Description:         To  find a state object in the collection stateList   */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              DFEvalState * obj                                     */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
DFEvalState* DFMatrix::FindObjectWithState(int state)
{
		POSITION pos;


		pos = stateList.GetHeadPosition();
	    int i = 0;
		DFEvalState* evalState;
		while (pos != NULL)
		{
			evalState = stateList.GetNext(pos);
			int stateInObject  = evalState->GetState();
			if (stateInObject == state)
               return evalState;
		
		}

		return NULL;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      FindObjectWithState                                   */
/* Description:         To find an object with state                          */
/*                                                                            */
/* Input:               char *state - state                                   */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              DFEvalState * obj                                     */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
DFEvalState* DFMatrix::FindObjectWithState(char *state)
{
		POSITION pos;


		pos = stateList.GetHeadPosition();
	    int i = 0;
		DFEvalState* evalState;
		while (pos != NULL)
		{
			evalState = stateList.GetNext(pos);
			CString stateInObject  = evalState->GetStrState();
			if (stateInObject == CString(state))
               return evalState;
		
		}

		return NULL;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      FindMappingNumber                                     */
/* Description:         To find a mapping number for a state                  */
/*                                                                            */
/* Input:               char *state - state                                   */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              mapping number                                        */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFMatrix::FindMappingNumber(char  *state)
{

		POSITION pos;


		pos = stateList.GetHeadPosition();
	    int i = 0;
		DFEvalState* evalState;
        //char *stateUpper = _strupr(state); 
		while (pos != NULL)
		{
			evalState = stateList.GetNext(pos);
			CString stateInObject  = evalState->GetStrState();
			if (stateInObject == state)
                  return evalState->GetState();
		
		}


    return 0; 
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      FindMappingState                                      */
/* Description:         To initialize the Matrix                              */
/*                                                                            */
/* Input:               int state                                             */
/*                      char *buffer                                          */
/* Output:              None                                                  */
/* Return:              None                                                  */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
//void DFMatrix::FindMappingState(int  state,  char *buffer)
//{
//	char buffer1[10];
//	_itoa(state, buffer1, 10);
//		
//	for ( int i = 0; i < numberOfStates ; i++) {
//	   if ( i % 2 == 0) {
//		   if (_stricmp(stateWords[i + 1], buffer1) == 0) { 
//			   strcpy(buffer, stateWords[ i ]);
//			   break;
//           }
//			 
//	   } 
//    }
//    return; 
//}
/*----------------------------------------------------------------------------*/
/* Procedure name:      SeeStateCollection                                    */
/* Description:         To write the matrix to a file                         */
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
//int DFMatrix::SeeStateCollection()
//{
//   char *path;
//   char evalPath[256];
//
//   if (( path = getenv("DATAPATH")) == NULL) {
//         RecordErrorToEvalLog(SET_DATAPATH_ERROR, NULL);
//         return(SET_DATAPATH_ERROR); 
//   }
//   strcpy(evalPath, path);
//   AppendPath(evalPath, MATRIX);
//
//	   ostrstream outStore;
//       ofstream ofile( "d:\\evaluator\\debug\\statetest.map", ios::out);
//   ofstream ofile( evalPath, ios::out );
//   if (!ofile) {
//      RecordErrorToEvalLog(FILE_OPEN_ERROR, evalPath );
//	  return(FILE_OPEN_ERROR);
//   } 
//
//
//   char buffer[20]; 
//
//   POSITION pos;
//
//
//   pos = stateList.GetHeadPosition();
//   int j = 0;
//   DFEvalState* stateObject;
//   while (pos != NULL)
//   {
//		stateObject = stateList.GetNext(pos);
//		ofile << "State:" << stateObject->GetState() <<  "\n";
//		ofile << ":" << stateObject->GetStateType() <<  "\n";
//
//		int stateInObject  = stateObject->GetState();
//		POSITION posStatus;
//        posStatus = stateObject->statusList.GetHeadPosition();
//	    int j = 0;
//	    DFEvalStatus* statusObject;
//	    while (posStatus != NULL)
//		{
//		    statusObject =  stateObject->statusList.GetNext(posStatus);
//	        FindMappingState(statusObject->GetTransaction(), buffer);
//			
//	        ofile << "Status:"<< statusObject->GetStatus() << '\n' <<  "Transaction: " << statusObject->GetStrTransaction() <<  '(' << statusObject->GetTransaction() << ')' <<'\n';
//
//           
//		
//		}
//
//	   ofile << "--------------" << "\n";
//
//           
//		
//	}
//    ofile.close(); 
//	return 0;
//}

/*----------------------------------------------------------------------------*/
/* Procedure name:      GetNextStateStr                                       */
/* Description:         Get next state as a string                            */
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
CString DFMatrix::GetNextStateStr(CString &strState, CString currentStatus )
{
	    //strState.MakeUpper();
		//currentStatus.MakeUpper();
        int currentState = FindMappingNumber( (LPTSTR) (LPCTSTR)strState);

	   	POSITION pos = stateList.GetHeadPosition();
	    int j = 0;
		DFEvalState* stateObject;
		while (pos != NULL)
		{
			stateObject = stateList.GetNext(pos);
			
			int stateInObject  = stateObject->GetState();
			if (stateInObject == currentState) {
			   POSITION posStatus;
	           posStatus = stateObject->statusList.GetHeadPosition();
	           int j = 0;
		       DFEvalStatus* statusObject;
		       while (posStatus != NULL)
			   {
			       statusObject =  stateObject->statusList.GetNext(posStatus);
				   CString strStatus = statusObject->GetStatus();
			       //strStatus.MakeUpper();
				   //currentStatus.MakeUpper();
				   if (strStatus.CompareNoCase(currentStatus) == 0) {
                        return statusObject->GetStrTransaction();

				   }	
		    
			   }
			}    
		}

  		return CString();
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      GetNextStateInt                                       */
/* Description:         Get Next State as an int                              */
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
int DFMatrix::GetNextStateInt(int currentState, CString currentStatus )
{
	  
        //currentStatus.MakeUpper(); 
	   	POSITION pos = stateList.GetHeadPosition();
	    int j = 0;
		DFEvalState* stateObject;
		while (pos != NULL)
		{
			stateObject = stateList.GetNext(pos);
			
			int stateInObject  = stateObject->GetState();
			if (stateInObject == currentState) {
			   POSITION posStatus;
	           posStatus = stateObject->statusList.GetHeadPosition();
	           int j = 0;
		       DFEvalStatus* statusObject;
		       while (posStatus != NULL)
			   {
			       statusObject =  stateObject->statusList.GetNext(posStatus);
				   CString strStatus = statusObject->GetStatus();
			       //strStatus.MakeUpper();
				   //currentStatus.MakeUpper();
				   if (strStatus == currentStatus) {
                        return statusObject->GetTransaction();

				   }	
		    
			   }
			}    
		}

  		return 0;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      AppendPath                                            */
/* Description:         To append one path to another one                     */
/*                                                                            */
/* Input:               lpszFirst: Specifies the first path to which second   */ 
/*                                   path is to be appended.                  */
/*                        lpszSecond: Specifies the path to be appenced to    */ 
/*						            lpszFirst                                 */ 
/*                                                                            */
/*                                                                            */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              pszFirst: Containst he resultant path on return.      */
/*                                                                            */
/* Global References:   None                                                  */
/* Notes: This function combines two paths and takes care of trailing         */
/*          slashes in both of them. It ensures that when the two             */ 
/*          paths are appended, there is a single slash separating            */
/*          them. For ex: if first path has a trailing slash and              */
/*          second path has a starting slash, it ensures to remove            */
/*          one of the slashes. Similarly, if first path does not             */
/*          have a trailing slash and second path does not have               */
/*          a starrting slash, a single slash is appended to the first        */
/*          path before appending the second path.                            */ 
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
void DFMatrix::AppendPath(char *lpszFirst,char *lpszSec)
{
        if(lpszFirst == NULL || lpszSec == NULL) return;

        int nLen1 = strlen(lpszFirst);

        // If last char in First is not a slash and first char
        // in Sec is not a slash, append slash.
                                        
        if(lpszFirst[nLen1-1] != '\\' && lpszSec[0] != '\\')                                                                                           
               strcat(lpszFirst,"\\");
        else if(lpszFirst[nLen1-1] == '\\' && lpszSec[0] == '\\')                                                   
                lpszFirst[nLen1-1] = 0;

        strcat(lpszFirst,lpszSec);
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      GetStateType                                          */
/* Description:         To get the type of a state                            */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              state type                                            */
/* Return:              None                                                  */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
DFEvalState::StateType DFMatrix::GetStateType(int state)
{
		POSITION pos;
       
		pos = stateList.GetHeadPosition();
	    int i = 0;
		DFEvalState* evalState;
		while (pos != NULL)
		{
			evalState = stateList.GetNext(pos);
			int stateInObject  = evalState->GetState();
			if (stateInObject == state)
               return evalState->GetStateType();
		
		}
        return DFEvalState::UNDEFINED; 


}
/*----------------------------------------------------------------------------*/
/* Procedure name:      GetStateName                                          */
/* Description:         To get the state name                                 */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              state type                                            */
/* Return:              None                                                  */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
CString DFMatrix::GetStateName(int state)
{
		POSITION pos;
       
		pos = stateList.GetHeadPosition();
	    int i = 0;
		DFEvalState* evalState;
		while (pos != NULL)
		{
			evalState = stateList.GetNext(pos);
			int stateInObject  = evalState->GetState();
			if (stateInObject == state)
               return evalState->GetStrState();
		
		}
        return CString(""); 
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      IsStateBusy                                           */
/* Description:         To get the state name                                 */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              state type                                            */
/* Return:              None                                                  */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
BOOL DFMatrix::IsStateBusy(CString state)
{
		POSITION pos;
       
		pos = stateList.GetHeadPosition();
	    int i = 0;
		DFEvalState* evalState;
		while (pos != NULL)
		{
			evalState = stateList.GetNext(pos);
			CString strState  = evalState->GetStrState();
			if (strState == state)
               return evalState->IsResUsed();
		
		}
        return FALSE; 
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      IsAtLeastOneStateFree                                 */
/* Description:         To determine the status of the processint line        */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/* Return:              TRUE - there is at least one free state; FALSE -      */
/*                      otherwise                                             */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
BOOL DFMatrix::IsAtLeastOneStateFree()
{
		POSITION pos;
       
		pos = stateList.GetHeadPosition();
	    int i = 0;
		DFEvalState* evalState;
		while (pos != NULL)
		{
			evalState = stateList.GetNext(pos);
			if ( !evalState->IsResUsed() )
				return TRUE;
		
		}
        return FALSE; 
}

/*----------------------------------------------------------------------------*/
/* Procedure name:      SetState                                              */
/* Description:         To set the state to be busy                           */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              state type                                            */
/* Return:              None                                                  */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
void DFMatrix::SetState(CString state, BOOL busy)
{
		POSITION pos;
       
		pos = stateList.GetHeadPosition();
	    int i = 0;
		DFEvalState* evalState;
		while (pos != NULL)
		{
			evalState = stateList.GetNext(pos);
			CString strState  = evalState->GetStrState();
			if (strState == state) {
               evalState->SetResUsed(busy);
			   return;
            }
		
		}
        return; 
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      FreeStates                                            */
/* Description:         To free the states                                    */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              state type                                            */
/* Return:              None                                                  */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
void DFMatrix::FreeStates()
{
		POSITION pos;
       
		pos = stateList.GetHeadPosition();
	    int i = 0;
		DFEvalState* evalState;
		while (pos != NULL)
		{
			evalState = stateList.GetNext(pos);
			evalState->SetResUsed(FALSE);
		        		
		}
        return; 
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      GetProcessingStatus                                   */
/* Description:         To get processing status                              */
/*                                                                            */
/* Input:               CString state                                         */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              CString processingStatus                              */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
CString DFMatrix::GetProcessingStatus(CString state)
{
		    DFEvalState *stateObj = FindObjectWithState((LPTSTR) (LPCTSTR) state);
            return stateObj->GetProcessingStatus(); 
}			
/*----------------------------------------------------------------------------*/
/* Procedure name:      GetEndStatus                                          */
/* Description:         To get end status                                     */
/*                                                                            */
/* Input:               CString state                                         */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              CString endStatus                                     */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
CString DFMatrix::GetEndStatus(CString state)
{
		    DFEvalState *stateObj = FindObjectWithState((LPTSTR) (LPCTSTR) state);
            return stateObj->GetEndStatus(); 
}			
/*----------------------------------------------------------------------------*/
/* Procedure name:      GetStateGroup                                         */
/* Description:         To get state group                                    */
/*                                                                            */
/* Input:               CString state                                         */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              CString processingStatus                              */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
DFEvalState::StateGroup DFMatrix::GetStateGroup(CString state)
{
		    DFEvalState *stateObj = FindObjectWithState((LPTSTR) (LPCTSTR) state);
			if (stateObj)
				return stateObj->GetStateGroup(); 
			else 
				return DFEvalState::NONE;
}			


/*----------------------------------------------------------------------------*/
/* Procedure name:      FindFirstStateInGroup                                 */
/* Description:         To get first state in a group                         */
/*                                                                            */
/* Input:               group                                                 */                                                                   
/* Output:              None                                                  */
/* Return:              CString state                                         */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
CString DFMatrix::FindFirstStateInGroup(DFEvalState::StateGroup group)
{
		POSITION pos;


		pos = stateList.GetHeadPosition();
	    int i = 0;
		DFEvalState* evalState;
		while (pos != NULL)
		{
			evalState = stateList.GetNext(pos);
			DFEvalState::StateGroup groupInObject  = evalState->GetStateGroup();
			if (groupInObject == CString(group))
               return evalState->GetStrState();
		
		}

		return CString();
}
