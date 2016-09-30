#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include <windows.h>

/*
BOOL DumpSubMenu(HMENU hMenu)
{
int nMenuItemCount;
int nSubMenuItemCount;
int nInd;
HMENU hSubMenu;
MENUITEMINFO MiiMenuItemInfo;

MiiMenuItemInfo.cbsize = sizeof(MENUITEMINFO);
MiiMenuItemInfo.fMask = MIIM_ID;
 
  nMenuItemCount = GetMenuItemCount(hMenu);

  for( nInd = 0; nInd < nMenuItemCount, nInd++)
  {
    nSubMenuItemCount = GetMenuItemCount(hSubMenu);
    if (!GetMenuItemInfo(hSubMenu, (UINT) nInd, FALSE, &MiiMenuItemInfo))
	{
		//signal the error
	}
	else
    {
		//print the item

	}
  }
return TRUE;
}
*/



BOOL GetMenuNameLength(HMENU hMenu, int * length)
{

}

BOOL GetMenuName(HMENU hMenu, char * Name, int length)
{

}


BOOL DumpMenu(HMENU hMenu)
{
int nMenuItemCount;
int nSubMenuItemCount;
int nInd;
int nInd2;
HMENU hSubMenu;
MENUITEMINFO MiiMenuItemInfo;
char * s;

s = (char *) malloc (100);
 
  nMenuItemCount = GetMenuItemCount(hMenu);
  for( nInd = 0; nInd < nMenuItemCount; nInd++)
  {
	hSubMenu = GetSubMenu(hMenu, nInd);
	nSubMenuItemCount = GetMenuItemCount(hSubMenu);
    for( nInd2 = 0; nInd2 < nSubMenuItemCount; nInd2++)
	{
      MiiMenuItemInfo.cbSize = sizeof(MENUITEMINFO);
	  MiiMenuItemInfo.fMask = MIIM_ID|MIIM_TYPE;
	  MiiMenuItemInfo.dwTypeData = 0;
	  MiiMenuItemInfo.cch = 0;
      if (!GetMenuItemInfo(hSubMenu, (UINT) nInd2, TRUE, &MiiMenuItemInfo))
	  {
		//signal the error
	  }
	  else
	  {
		//print the item
        if (MiiMenuItemInfo.fType == MFT_STRING)
		{
			MiiMenuItemInfo.cbSize = sizeof(MENUITEMINFO);
			MiiMenuItemInfo.fMask = MIIM_ID|MIIM_TYPE|MIIM_SUBMENU|MIIM_STATE|MIIM_DATA;
			MiiMenuItemInfo.dwTypeData = s;
			MiiMenuItemInfo.cch = 99;
	        GetMenuItemInfo(hSubMenu, (UINT) nInd2, TRUE, &MiiMenuItemInfo);
			printf("%s : %d\n", (char *) MiiMenuItemInfo.dwTypeData, MiiMenuItemInfo.wID);
		}
	  }
	}
  }
  free(s);
  return TRUE;
}


void main(int argc, char * argv[])
{
HWND hWnd = 0;
HMENU hMenu;
int i;
int mode;

  if (argc ==1) {
    fprintf(stderr, "usage : commands -hwnd|-class arguments\n");
    fprintf(stderr, "dumps the commands associated to menus for a window\n");
    fprintf(stderr, "-hwnd : take the argument as the hex handle of the window to be examined\n");
    fprintf(stderr, "-class : take the argument as the classnames of the window to be examined (you have to specify the whole descendance)\n");
    fprintf(stderr, "-class does not work properly\n");
    exit(100);
  }

  if (stricmp(argv[1], "-hwnd") ==0)
	  sscanf (argv[2], "%x", &hWnd);
  else
    for (i = 2; i < argc; i++)
	{
      hWnd = FindWindowEx(hWnd, 0, argv[i], NULL);
	}

  //if (hWnd) hMenu = GetSystemMenu(hWnd, FALSE);
  if (hWnd)
  {
	  hMenu = GetMenu(hWnd);
      if (hMenu) DumpMenu(hMenu);
  }
  if (hWnd)
  {
	  hMenu = GetSystemMenu(hWnd, FALSE);
      if (hMenu) DumpMenu(hMenu);
  }
}
