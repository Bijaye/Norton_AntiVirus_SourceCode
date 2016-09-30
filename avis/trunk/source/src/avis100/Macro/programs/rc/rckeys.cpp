/*******************************************************************
*                                                                  *
* File name:     rckeys.cpp                                        *
*                                                                  *
* Description:   Keystroke processor                               *
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
* Function:      Implements RCKey class that processes keystrokes  *
*                to ge sent to target application                  *
*                                                                  *
*******************************************************************/

#include "afxtempl.h"
#include <windows.h>
#include <winuser.h>
#include <string.h>
#include <malloc.h>

#include "RCKeys.h"

BYTE RCKeys::GetVK(char *keys)
{
char * pos;
char key[10];
BYTE VK;

	if (keys[0] ==0)
	{
		return 0;
	}
	else
	{
		if (keys[0] == '[')
		{
			//special caracter
			pos = strchr(keys,']');
			
			strncpy(key,keys+1,pos - keys -1);
			strupr(key);
			strcpy(keys,pos+1);

			if (strcmp(key,"CTRL") ==0)
			{
				VK = VK_CONTROL;
			}
			else if (strcmp(key,"ALT") ==0)
			{
				VK = VK_MENU;
			}
			else if (strcmp(key,"ENTER") ==0)
			{
				VK = VK_RETURN;
			}
			else if (strcmp(key,"ESC") ==0)
			{
				VK = VK_ESCAPE;
			}
			else if (strcmp(key,"TAB") ==0)
			{
				VK = VK_TAB;
			}
			else if (strcmp(key,"BACK") ==0)
			{
				VK = VK_BACK;
			}
			else if (strcmp(key,"DEL") ==0)
			{
				VK = VK_DELETE;
			}
			else if (strcmp(key,"INS") ==0)
			{
				VK = VK_INSERT;
			}
			else if (strcmp(key,"F4") ==0)
			{
				VK = VK_F4;
			}
			else if (strcmp(key,"LEFT") ==0)
			{
				VK = VK_LEFT;
			}
			else if (strcmp(key,"RIGHT") ==0)
			{
				VK = VK_RIGHT;
			}
			else if (strcmp(key,"HOME") ==0)
			{
				VK = VK_HOME;
			}
			else if (strcmp(key,"END") ==0)
			{
				VK = VK_END;
			}

		}
		else
		{
           if (strcmp(keys, "~") == 0)
			   VK= VK_RETURN; 
		   else {
		     VK = 0xFF & VkKeyScan(keys[0]);
		   }
            strcpy(keys,keys+1);
		   
		}

	}
return VK;
}



KS * RCKeys::GetKeySeq(LPCTSTR keys)
{
int i = 0;
KS * K;
BYTE VK;
BOOL control = 0;
BOOL shift = 0;
BOOL menu = 0;

char * keyscopy;

	keyscopy = (char *) calloc(strlen(keys) + 1,sizeof(char));
	strcpy(keyscopy,keys);

	K = (KS*) calloc (3 * strlen(keys) +1, sizeof(KS));

	while(keyscopy[0] !=0)
	{
		if (keyscopy[0] == ':' || keyscopy[0] >= 'A' && keyscopy[0] <= 'Z')
		{	
			(*(K+i)).VirCode = VK_SHIFT;
	  	    (*(K+i)).KBState = 0;
		    (*(K+ i++)).UoD = 0;
			shift = 1;
		} 

	    VK = GetVK(keyscopy);
		(*(K+i)).VirCode = VK;
		(*(K+i)).KBState = 0;
		(*(K+ i++)).UoD = 0;


		if (VK ==VK_CONTROL)
		{
			control = 1;
		}

		if (VK ==VK_MENU)
		{
			menu = 1;
		}

		if ((VK !=VK_CONTROL)&&(VK !=VK_MENU)&& (VK != VK_SHIFT))
		{
			(K+i)->VirCode = VK;
			(K+i)->KBState = 0;
			(K+i)->UoD = 1;			
			++i;

			if (control)
			{
				(*(K+ i)).VirCode = VK_CONTROL;
				(*(K+ i)).KBState = 0;
				(*(K+ i++)).UoD = 1;			
				control = 0;
			}
			if (menu)
			{
				(*(K+i)).VirCode = VK_MENU;
				(*(K+i)).KBState = 0;
				(*(K+ i++)).UoD = 1;			
				menu = 0;
			}	
			if (shift)
			{
				(*(K+ i)).VirCode = VK_SHIFT;
				(*(K+ i)).KBState = 0;
				(*(K+ i++)).UoD = 1;			
				shift = 0;
			}

		}	
	}

	free(keyscopy);

return K;
}


int RCKeys::sendkeyseq(KS * keys,HWND winhwnd)
{
//KBDEV key;
BYTE VK;
DWORD flag;

int i = 0;

	while((VK = (*(keys+i)).VirCode) !=0)
	{
		if ((*(keys+i)).UoD ==0) flag = 0;
		if ((*(keys+i)).UoD ==1) flag = KEYEVENTF_KEYUP;
		++i;

		if (SetForegroundWindow(winhwnd) !=0)
		
			keybd_event(VK,0,flag,0);
	}	

	return 0;
}

int RCKeys::SendKeys(LPCTSTR keys,HWND winhwnd)
{
KS * K;

	K = GetKeySeq(keys);

	sendkeyseq(K,winhwnd);
	free(K);
    return 0;
}
