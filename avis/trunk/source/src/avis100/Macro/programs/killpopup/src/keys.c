/*******************************************************************
*                                                                  *
* File name:     keys.c                                            *
*                                                                  *
* Description:   keystroke processor                               *
*                                                                  *
*                                                                  *
* Statement:     Licensed Materials - Property of IBM              *
*                (c) Copyright IBM Corp. 1999                      *
*                                                                  *
*                                                                  *
*                U.S. Government Users Restricted Rights - use,    *
*                duplication or disclosure restricted by GSA ADP   *
*                Schedule Contract with IBM Corp.                  *
*                                                                  *
*                                                                  *
********************************************************************
*                                                                  *
* Function:      Process keystrokes and send them to application   *
*                                                                  *
*******************************************************************/
#include <string.h>
#include <malloc.h>

#include <windows.h>
#include <winuser.h>

struct KEY
{
	BYTE VirCode;
	BYTE KBState;
	BYTE UoD; //depressed or pressed
};
typedef struct KEY KS;


BYTE GetVK(char * keys)
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

		}
		else
		{
			VK = 0xFF & VkKeyScan(keys[0]);
			strcpy(keys,keys+1);
		}

	}
return VK;
}



KS * GetKeySeq(char * keys)
{
int i = 0;
KS * K;
BYTE VK;
BOOL control = 0;
BOOL menu = 0;

char * keyscopy;

	keyscopy = (char *) calloc(strlen(keys) + 1,sizeof(char));
	strcpy(keyscopy,keys);

	K = (KS*) calloc (2 * strlen(keys) +1, sizeof(KS));

	while(keyscopy[0] !=0)
	{
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

		if ((VK !=VK_CONTROL)&&(VK !=VK_MENU))
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
		}	
	}

	free(keyscopy);

return K;
}


int sendkeyseq(KS * keys,HWND winhwnd)
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


int SendKeys(char * keys,HWND winhwnd)
{
KS * K;

	K = GetKeySeq(keys);

	sendkeyseq(K,winhwnd);
	free(K);
    return 0;
}
