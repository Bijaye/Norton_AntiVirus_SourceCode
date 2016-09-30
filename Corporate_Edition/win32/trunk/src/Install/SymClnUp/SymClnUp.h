// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include <windows.h>
#include <stdlib.h>
#include <tchar.h>

#define SYMANTEC_KEY		"SOFTWARE\\Symantec"
#define SYMCLNUP_KEY		"SOFTWARE\\Symantec\\SymClnUp"
#define SYMCLNUP_FILES_KEY	"SOFTWARE\\Symantec\\SymClnUp\\Files"
#define SYMCLNUP_DIRS_KEY	"SOFTWARE\\Symantec\\SymClnUp\\Directories"

int APIENTRY WinMain(HINSTANCE,HINSTANCE,LPSTR,int);
int CleanUpDirectories();
int CleanUpFiles();
int CleanUpRegistry();
