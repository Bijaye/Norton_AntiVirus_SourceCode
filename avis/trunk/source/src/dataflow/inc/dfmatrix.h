/*----------------------------------------------------------------------------*/
/*                                                                            */
/* FILE:      DFMatrix.h                                                      */
/* FUNCTION:  To create a matrix object                                       */
/*                                                                            */
/* ROUTINES CONTAINED WITHIN FILE:                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* NOTES:                                                                     */
/* Notes: There is one input file: Evaluator.dat                              */
/* Output file matrix.data contains the built matrix                          */
/*----------------------------------------------------------------------------*/
/* HISTORY: August, 1998   SNK                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#ifndef DFMATRIX_H
#define DFMATRIX_H

// file names
#define EVALUATORDATA  "avisdfev.dat"  // Evaluator's file (State-Status)
#define LOGFILE        "evaluator.log" // Evaluator's log file
#define MATRIX         "matrix.dat"    // Test-Matrix file

class DFMatrix 
{
public:

    DFMatrix();
    ~DFMatrix();
  	int Init();
	CString GetNextStateStr(CString &strState, CString currentStatus);
	int GetNextStateInt(int currentState, CString currentStatus );
	int FindMappingNumber(char  *state);
	void FindMappingState(int  state, char *buffer);
    DFEvalState* FindObjectWithState(int state);
    DFEvalState* FindObjectWithState(char *state);

	void AppendPath(char *lpszFirst,char *lpszSec);
	DFEvalState::StateType DFMatrix::GetStateType(int state);
    CString GetStateName(int state);
	CString GetProcessingStatus(CString state);	
	CString GetEndStatus(CString state);
	DFEvalState::StateGroup GetStateGroup(CString state);
	CString GetReplDriverName();
    int GetNumberOfStates() { return stateList.GetCount(); }	
	void SetState(CString state, BOOL busy);
	BOOL IsStateBusy(CString state);
    void FreeStates(); 
	BOOL IsAtLeastOneStateFree();
	CString FindFirstStateInGroup(DFEvalState::StateGroup group);

 private:
   	int numberOfStates; 
	DFEvalStateList stateList;
    char **stateWords;
    char *tptr;
	char *tptrmap;



    DFMatrix(const DFMatrix &);
    DFMatrix &operator =(const DFMatrix &);
 

};



#endif 
