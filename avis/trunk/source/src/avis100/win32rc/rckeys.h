/*******************************************************************
*                                                                  *
* File name:     rckeys.h                                          *
*                                                                  *
* Description:   RCKeys class definition file                      *
*                                                                  *
*                                                                  *
* Statement:     Licensed Materials - Property of IBM              *
*                (c) Copyright IBM Corp. 1999                      *
*                                                                  *
* Author:        Alla Segal                                        *
*                                                                  *
*                U.S. Government Users Restricted Rights - use,    *
*                duplication or disclosure restricted by GSA ADP   *
*                Schedule Contract with IBM Corp.                  *
*                                                                  *
*                                                                  *
********************************************************************
*                                                                  *
* Function:      class definition for rc keystrokes processor      *
*                                                                  *
*******************************************************************/
#ifndef RCKEYS_H
#define RCKEYS_H

struct KEY
	{
    BYTE VirCode;
    BYTE KBState;
    BYTE UoD; //depressed or pressed
	};
    typedef struct KEY KS;

class RCKeys {
   private:
    
    BYTE GetVK(char * keys);
    KS * GetKeySeq(LPCTSTR keys);
    int sendkeyseq(KS * keys,HWND winhwnd);
   public:
     RCKeys() {};
     int SendKeys(LPCTSTR keys,HWND winhwnd);
	 char * ConvertText(CString Keys);
};
#endif
