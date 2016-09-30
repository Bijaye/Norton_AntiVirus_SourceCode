/*----------------------------------------------------------------------------*/
/*                                                                            */
/* FILE:      DFEvalState.h                                                   */
/* FUNCTION:  To create a state object                                        */
/*                                                                            */
/* ROUTINES CONTAINED WITHIN FILE:                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* NOTES:                                                                     */
/* Notes: There are two input files: Evaluator.dat and State.map              */
/*                                                                            */
/* Evaluator.dat  (as for today, 08/12/98)                                    */
/* ---------------                                                            */
/*  Begin Initial Wait Classifier                                             */
/*  Classifier Processing Success GIRC                                        */
/*  Classifier Processing Retry Classifier                                    */
/*  Classifier Processing Fail DeferredQueue                                  */ 
/*  GIRC Processing Success Replication                                       */
/*  GIRC Processing Fail DeferredQueue                                        */ 
/*  Replication Processing Success GMRC                                       */
/*  Replication Processing Retry Replication                                  */
/*  Replication Processing Fail DeferredQueue                                 */
/*  GMRC Processing Success Extraction                                        */
/*  GMRC Processing Retry Replication                                         */
/*  GMRC Processing Fail DeferredQueue                                        */ 
/*  Extraction Processing Success DefCompiler                                 */
/*  Extraction Processing Retry Extraction                                    */
/*  Extraction Processing Fail DeferredQueue                                  */
/*  DefCompiler Processing Success UnitTest                                   */
/*  DefCompiler Processing Retry DeferredQueue                                */ 
/*  DefCompiler Processing Fail DeferredQueue                                 */ 
/*  UnitTest Processing Success End                                           */
/*  UnitTest Processing Retry DeferredQueue                                   */
/*  UnitTest Processing Fail DeferredQueue                                    */
/*  DeferredQueue Processing Success GNSFD                                    */
/*  DeferredQueue Processing Fail End                                         */
/*  End Final Null Null                                                       */
/*  GNSFD Transit Null Null                                                   */ 
/*                                                                            */
/*  State.map (as for today, 08/12/98)                                        */
/*  -------------------                                                       */
/*  Begin 1                                                                   */
/*  Classifier  2                                                             */
/*  GIRC 3                                                                    */
/*  Replication 4                                                             */ 
/*  GMRC 5                                                                    */
/*  Extraction 6                                                              */
/*  DefCompiler 7                                                             */ 
/*  UnitTest 8                                                                */ 
/*  DeferredQueue 9                                                           */
/*  End 10                                                                    */
/*  GNSFD 11                                                                  */
/*                                                                            */
/*  The Evaluator.dat and State.map files are located in a directory defined  */
/*  by env variable DATAPATH.                                                 */
/*                                                                            */
/*  Output file matrix.data contains  the built matrix                        */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* HISTORY:  August, 1998                                                     */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#ifndef DFEVALSTATE_H
#define DFEVALSTATE_H


class DFEvalState 
{
public:
	enum  StateType {UNDEFINED, INITIAL, PROCESSING, FINAL, SCHEDULED}; 
	enum  StateGroup {NONE, ANALYSIS, BUILD, FULLBUILD, DEFER, ARCHIVE, SCAN, STOP, END, IMPORT, UPDATE, UNDEFER, COLLECT}; 
	DFEvalState();   
 	~DFEvalState();

	DFEvalState(CString stringState, int number, CString stateType, CString stateGroup, CString startStatus, CString status );
	DFEvalStatus* FindObjectWithStatus(CString status);  

	StateType   GetStateType() {return type; }
	StateGroup  GetStateGroup() {return group; }
	int GetState() { return state;}
    CString GetStrState() { return strState; } 
    CString GetProcessingStatus() { return processingStatus; } 
    CString GetEndStatus() { return endStatus; } 
 	StateType GetDefinedStateType(CString stateType);
	StateGroup GetDefinedStateGroup(CString stateGroup);
	void SetResUsed( BOOL flag) { IsUsed = flag;} 
    BOOL IsResUsed() { return IsUsed; } 

            
    DFEvalStatusList statusList;

private:

	int        state; 
	CString    strState;
	StateType  type;
	StateGroup group;
	CString    processingStatus;
    CString    endStatus;
	BOOL       IsUsed; 


    DFEvalState(const DFEvalState &);
    DFEvalState &operator =(const DFEvalState &);
 

};

typedef CTypedPtrList<CPtrList, DFEvalState*> DFEvalStateList;


#endif 
