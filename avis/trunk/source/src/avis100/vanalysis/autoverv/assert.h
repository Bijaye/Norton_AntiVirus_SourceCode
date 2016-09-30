/*##################################################################*/
/*                                                                  */
/* Program name:  assert.h                                          */
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

#ifndef assert
#   if !defined(MYASSERT)
#      include <assert.h>
#   else
#      include <stdio.h>
#      include <stdlib.h>
#      include "break.h"
#      define assert(C) \
          {if (!(C)) {  \
             printf("*** assertion contradicted @ %s [%d]:\n   %s\n",__FILE__,__LINE__,#C); \
             BREAK();   \
             exit(7);   \
          }}
#   endif
#endif
