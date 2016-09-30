/*----------------------------------------------------------------------------*/
/*                                                                            */
/* FILE:      DFEvalStatus.h                                                  */
/* FUNCTION:  To create a status object                                       */
/*                                                                            */
/* ROUTINES CONTAINED WITHIN FILE:                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* NOTES:                                                                     */
/*----------------------------------------------------------------------------*/
/* HISTORY: August, 1998 - SNK                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#ifndef DFEVALSTATUS_H
#define DFEVALSTATUS_H


class DFEvalStatus 
{
public:

	DFEvalStatus();
	DFEvalStatus(CString statusName, int trans, CString state );
 
    ~DFEvalStatus();
    CString GetStatus() { return status;}
	CString GetStrTransaction() { return strTransaction; }
	int GetTransaction() { return transaction; } 
 
   
 
private:

    CString  status; 
    CString  strTransaction; 
    int      transaction; 
   
    DFEvalStatus(const DFEvalStatus &);
    DFEvalStatus &operator =(const DFEvalStatus &);
 

};

typedef CTypedPtrList<CPtrList, DFEvalStatus*> DFEvalStatusList;

#endif 
