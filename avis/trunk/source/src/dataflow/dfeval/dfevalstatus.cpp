/*----------------------------------------------------------------------------*/
/*                                                                            */
/* FILE:      DFEvalStatus.cppp                                               */
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
/* HISTORY: AUGUST, 1998  SNK                                                                  */
/*----------------------------------------------------------------------------*/
#include "afxtempl.h"

#include "DFEvalStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THDF_FILE
static char THDF_FILE[] = __FILE__;
#endif



/*----------------------------------------------------------------------------*/
/* Procedure name:      DFEvalStatus                                          */
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
DFEvalStatus::~DFEvalStatus()
{

}
/*----------------------------------------------------------------------------*/
/* Procedure name:      DFEvalStatus                                          */
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
DFEvalStatus::DFEvalStatus()
{
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      DFEvalStatus                                          */
/* Description:         Constructor                                           */
/*                                                                            */
/* Input:               CString statusName                                    */
/*                      int trans (next state as an 'int')                    */ 
/*                      CString state (next state as a string)                */
/* Output:              None                                                  */
/* Return:              None                                                  */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
DFEvalStatus::DFEvalStatus( CString statusName, int trans, CString state )
{
   status = statusName;
   transaction = trans;
   strTransaction = state;

}



