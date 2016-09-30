/*##################################################################*/
/*                                                                  */
/* Program name:  newhandler.h                                      */
/*                                                                  */
/* Package name:  autoverv.exe                                      */
/*                                                                  */
/* Description:                                                     */
/*                                                                  */
/*                                                                  */
/* Statement:     Licensed Materials - Property of IBM              */
/*                (c) Copyright IBM Corp. 1999                      */
/*                                                                  */
/* Author:        Andy Raybould                                     */
/*                                                                  */
/*                U.S. Government Users Restricted Rights - use,    */
/*                duplication or disclosure restricted by GSA ADP   */
/*                Schedule Contract with IBM Corp.                  */
/*                                                                  */
/*                                                                  */
/*##################################################################*/

#ifndef NEWHANDLER_H
#define NEWHANDLER_H

void* setNewHandler (void);

unsigned long setHeapLimit (unsigned long NewLimit);

//--- for C-style allocation --------------------------------------------------

void mem_err(void);


#endif
