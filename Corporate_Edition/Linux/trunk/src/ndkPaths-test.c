// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Linux implementation of NDK File Path Handling functions

// ================== ndkPaths-test =====================================================

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h> // for __fpurge
#include <stdio_ext.h> // for __fpurge

#include <ndkIntegers.h>
#include <ndkTime.h>

#include <ndkPaths.h>
#include <dirent.h>
#include <sys/stat.h>

#include "ndkDebug.h"

// --------------------- Make Path ------------------------------------------------------

static void makePath() {
	wchar_t wsPath[100]; char sPath[100];

	// _wmakepath

	dbgNil("_wmakepath(wsPath,L\"c:\",L\"/usr/local/symantec\",L\"Gr\x00fc""\x00df""e\",L\"txt\")");
	_wmakepath(wsPath,L"c:",L"/usr/local/symantec",L"Gr\x00fc""\x00df""e",L"txt");
	dbgWcs("wsPath == \"%ls\"",wsPath);
	wfWcs("makePath","_wmakepath",L"/usr/local/symantec/Gr\x00fc""\x00df""e.txt",wsPath);

	dbgNil("_wmakepath(wsPath,L\"volume:\",L\"/usr/local/symantec/\",L\"Gr\x00fc""\x00df""e\",L\".txt\")");
	_wmakepath(wsPath,L"volume:",L"/usr/local/symantec/",L"Gr\x00fc""\x00df""e",L".txt");
	dbgWcs("wsPath == \"%ls\"",wsPath);
	wfWcs("makePath","_wmakepath",L"/usr/local/symantec/Gr\x00fc""\x00df""e.txt",wsPath);

	dbgNil("_wmakepath(wsPath,NULL,L\"\\usr/local\\symantec\",L\"Gr\x00fc""\x00df""e\",L\"u\x00df""r\")");
	_wmakepath(wsPath,NULL,L"\\usr/local\\symantec",L"Gr\x00fc""\x00df""e",L"u\x00df""r");
	dbgWcs("wsPath == \"%ls\"",wsPath);
	wfWcs("makePath","_wmakepath",L"/usr/local/symantec/Gr\x00fc""\x00df""e.u\x00df""r",wsPath);

	dbgNil("_wmakepath(wsPath,NULL,L\"\\usr/local\\symantec\\\",L\"Gr\x00fc""\x00df""e\",L\".u\x00df""r\")");
	_wmakepath(wsPath,NULL,L"\\usr/local\\symantec\\",L"Gr\x00fc""\x00df""e",L".u\x00df""r");
	dbgWcs("wsPath == \"%ls\"",wsPath);
	wfWcs("makePath","_wmakepath",L"/usr/local/symantec/Gr\x00fc""\x00df""e.u\x00df""r",wsPath);

	dbgNil("_wmakepath(wsPath,NULL,L\"\\u\x00df""r/local\\\x00fc""ber/unter\\Skr\x00e4""be\x00e5""n\",L\"Gr\x00fc""\x00df""e\",NULL)");
	_wmakepath(wsPath,NULL,L"\\u\x00df""r/local\\\x00fc""ber/unter\\Skr\x00e4""be\x00e5""n",L"Gr\x00fc""\x00df""e",NULL);
	dbgWcs("wsPath == \"%ls\"",wsPath);
	wfWcs("makePath","_wmakepath",L"/u\x00df""r/local/\x00fc""ber/unter/Skr\x00e4""be\x00e5""n/Gr\x00fc""\x00df""e",wsPath);

	dbgNil("_wmakepath(wsPath,NULL,L\"\\u\x00df""r/local\\\x00fc""ber/unter\\Skr\x00e4""be\x00e5""n/\",L\"rinah\",NULL)");
	_wmakepath(wsPath,NULL,L"\\u\x00df""r/local\\\x00fc""ber/unter\\Skr\x00e4""be\x00e5""n/",L"rinah",NULL);
	dbgWcs("wsPath == \"%ls\"",wsPath);
	wfWcs("makePath","_wmakepath",L"/u\x00df""r/local/\x00fc""ber/unter/Skr\x00e4""be\x00e5""n/rinah",wsPath);

	dbgNil("_wmakepath(wsPath,NULL,L\"/usr/local/symantec\",NULL,NULL)");
	_wmakepath(wsPath,NULL,L"/usr/local/symantec",NULL,NULL);
	dbgWcs("wsPath == \"%ls\"",wsPath);
	wfWcs("makePath","_wmakepath",L"/usr/local/symantec/",wsPath);

	dbgNil("_wmakepath(wsPath,NULL,NULL,L\"rinah\",NULL)");
	_wmakepath(wsPath,NULL,NULL,L"rinah",NULL);
	dbgWcs("wsPath == \"%ls\"",wsPath);
	wfWcs("makePath","_wmakepath",L"rinah",wsPath);

	dbgNil("_wmakepath(wsPath,NULL,NULL,NULL,NULL)");
	_wmakepath(wsPath,NULL,NULL,NULL,NULL);
	dbgWcs("wsPath == \"%ls\"",wsPath);
	wfWcs("makePath","_wmakepath",L"",wsPath);

	// _makepath

	dbgNil("_makepath(sPath,\"c:\",\"/usr/local/symantec\",\"Gr\x00fc""\x00df""e\",\"txt\")");
	_makepath(sPath,"c:","/usr/local/symantec","Gr\x00fc""\x00df""e","txt");
	dbgStr("sPath == \"%s\"",sPath);
	wfStr("makePath","_makepath","/usr/local/symantec/Gr\x00fc""\x00df""e.txt",sPath);

	dbgNil("_makepath(sPath,\"volume:\",\"/usr/local/symantec/\",\"Gr\x00fc""\x00df""e\",\".txt\")");
	_makepath(sPath,"volume:","/usr/local/symantec/","Gr\x00fc""\x00df""e",".txt");
	dbgStr("sPath == \"%s\"",sPath);
	wfStr("makePath","_makepath","/usr/local/symantec/Gr\x00fc""\x00df""e.txt",sPath);

	dbgNil("_makepath(sPath,NULL,\"\\usr/local\\symantec\",\"Gr\x00fc""\x00df""e\",\"u\x00df""r\")");
	_makepath(sPath,NULL,"\\usr/local\\symantec","Gr\x00fc""\x00df""e","u\x00df""r");
	dbgStr("sPath == \"%s\"",sPath);
	wfStr("makePath","_makepath","/usr/local/symantec/Gr\x00fc""\x00df""e.u\x00df""r",sPath);

	dbgNil("_makepath(sPath,NULL,\"\\usr/local\\symantec\\\",\"Gr\x00fc""\x00df""e\",\".u\x00df""r\")");
	_makepath(sPath,NULL,"\\usr/local\\symantec\\","Gr\x00fc""\x00df""e",".u\x00df""r");
	dbgStr("sPath == \"%s\"",sPath);
	wfStr("makePath","_makepath","/usr/local/symantec/Gr\x00fc""\x00df""e.u\x00df""r",sPath);

	dbgNil("_makepath(sPath,NULL,\"\\u\x00df""r/local\\\x00fc""ber/unter\\Skr\x00e4""be\x00e5""n\",\"Gr\x00fc""\x00df""e\",NULL)");
	_makepath(sPath,NULL,"\\u\x00df""r/local\\\x00fc""ber/unter\\Skr\x00e4""be\x00e5""n","Gr\x00fc""\x00df""e",NULL);
	dbgStr("sPath == \"%s\"",sPath);
	wfStr("makePath","_makepath","/u\x00df""r/local/\x00fc""ber/unter/Skr\x00e4""be\x00e5""n/Gr\x00fc""\x00df""e",sPath);

	dbgNil("_makepath(sPath,NULL,\"\\u\x00df""r/local\\\x00fc""ber/unter\\Skr\x00e4""be\x00e5""n/\",\"rinah\",NULL)");
	_makepath(sPath,NULL,"\\u\x00df""r/local\\\x00fc""ber/unter\\Skr\x00e4""be\x00e5""n/","rinah",NULL);
	dbgStr("sPath == \"%s\"",sPath);
	wfStr("makePath","_makepath","/u\x00df""r/local/\x00fc""ber/unter/Skr\x00e4""be\x00e5""n/rinah",sPath);

	dbgNil("_makepath(sPath,NULL,\"/usr/local/symantec\",NULL,NULL)");
	_makepath(sPath,NULL,"/usr/local/symantec",NULL,NULL);
	dbgStr("sPath == \"%s\"",sPath);
	wfStr("makePath","_makepath","/usr/local/symantec/",sPath);

	dbgNil("_makepath(sPath,NULL,NULL,\"rinah\",NULL)");
	_makepath(sPath,NULL,NULL,"rinah",NULL);
	dbgStr("sPath == \"%s\"",sPath);
	wfStr("makePath","_makepath","rinah",sPath);

	dbgNil("_makepath(sPath,NULL,NULL,NULL,NULL)");
	_makepath(sPath,NULL,NULL,NULL,NULL);
	dbgStr("sPath == \"%s\"",sPath);
	wfStr("makePath","_makepath","",sPath);
}

// --------------------- Split Path -----------------------------------------------------

static void splitPath() {
	wchar_t wsDrive[10],wsDir[100],wsFname[50],wsExt[10];
	char		sDrive[10], sDir[100], sFname[50], sExt[10];

	// _wsplitpath

	dbgNil("_wsplitpath(L\"c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt\",NULL,NULL,NULL,NULL)");
	_wsplitpath(L"c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt",NULL,NULL,NULL,NULL);

	dbgNil("_wsplitpath(L\"c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt\",NULL,wsDir,wsFname,wsExt)");
	_wsplitpath(L"c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt",NULL,wsDir,wsFname,wsExt);
	dbgWcs("wsDir == \"%ls\"",wsDir);
	dbgWcs("wsFname == \"%ls\"",wsFname);
	dbgWcs("wsExt == \"%ls\"",wsExt);
	wfWcs("splitPath","_wsplitpath(wsDir)",L"/usr/local/symantec/",wsDir);
	wfWcs("splitPath","_wsplitpath(wsFname)",L"Gr\x00fc""\x00df""e",wsFname);
	wfWcs("splitPath","_wsplitpath(wsExt)",L".txt",wsExt);

	dbgNil("_wsplitpath(L\"c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt\",wsDrive,NULL,wsFname,wsExt)");
	_wsplitpath(L"c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt",wsDrive,NULL,wsFname,wsExt);
	dbgWcs("wsDrive == \"%ls\"",wsDrive);
	dbgWcs("wsFname == \"%ls\"",wsFname);
	dbgWcs("wsExt == \"%ls\"",wsExt);
	wfWcs("splitPath","_wsplitpath(wsDrive)",L"",wsDrive);
	wfWcs("splitPath","_wsplitpath(wsFname)",L"Gr\x00fc""\x00df""e",wsFname);
	wfWcs("splitPath","_wsplitpath(wsExt)",L".txt",wsExt);

	dbgNil("_wsplitpath(L\"c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt\",wsDrive,wsDir,NULL,wsExt)");
	_wsplitpath(L"c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt",wsDrive,wsDir,NULL,wsExt);
	dbgWcs("wsDrive == \"%ls\"",wsDrive);
	dbgWcs("wsDir == \"%ls\"",wsDir);
	dbgWcs("wsExt == \"%ls\"",wsExt);
	wfWcs("splitPath","_wsplitpath(wsDrive)",L"",wsDrive);
	wfWcs("splitPath","_wsplitpath(wsDir)",L"/usr/local/symantec/",wsDir);
	wfWcs("splitPath","_wsplitpath(wsExt)",L".txt",wsExt);

	dbgNil("_wsplitpath(L\"c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt\",wsDrive,wsDir,wsFname,NULL)");
	_wsplitpath(L"c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt",wsDrive,wsDir,wsFname,NULL);
	dbgWcs("wsDrive == \"%ls\"",wsDrive);
	dbgWcs("wsDir == \"%ls\"",wsDir);
	dbgWcs("wsFname == \"%ls\"",wsFname);
	wfWcs("splitPath","_wsplitpath(wsDrive)",L"",wsDrive);
	wfWcs("splitPath","_wsplitpath(wsDir)",L"/usr/local/symantec/",wsDir);
	wfWcs("splitPath","_wsplitpath(wsFname)",L"Gr\x00fc""\x00df""e",wsFname);

	dbgNil("_wsplitpath(L\"c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt\",wsDrive,wsDir,wsFname,wsExt)");
	_wsplitpath(L"c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt",wsDrive,wsDir,wsFname,wsExt);
	dbgWcs("wsDrive == \"%ls\"",wsDrive);
	dbgWcs("wsDir == \"%ls\"",wsDir);
	dbgWcs("wsFname == \"%ls\"",wsFname);
	dbgWcs("wsExt == \"%ls\"",wsExt);
	wfWcs("splitPath","_wsplitpath(wsDrive)",L"",wsDrive);
	wfWcs("splitPath","_wsplitpath(wsDir)",L"/usr/local/symantec/",wsDir);
	wfWcs("splitPath","_wsplitpath(wsFname)",L"Gr\x00fc""\x00df""e",wsFname);
	wfWcs("splitPath","_wsplitpath(wsExt)",L".txt",wsExt);

	dbgNil("_wsplitpath(L\"volume:/usr/local/symantec/Gr\x00fc""\x00df""e.txt\",wsDrive,wsDir,wsFname,wsExt)");
	_wsplitpath(L"volume:/usr/local/symantec/Gr\x00fc""\x00df""e.txt",wsDrive,wsDir,wsFname,wsExt);
	dbgWcs("wsDrive == \"%ls\"",wsDrive);
	dbgWcs("wsDir == \"%ls\"",wsDir);
	dbgWcs("wsFname == \"%ls\"",wsFname);
	dbgWcs("wsExt == \"%ls\"",wsExt);
	wfWcs("splitPath","_wsplitpath(wsDrive)",L"",wsDrive);
	wfWcs("splitPath","_wsplitpath(wsDir)",L"/usr/local/symantec/",wsDir);
	wfWcs("splitPath","_wsplitpath(wsFname)",L"Gr\x00fc""\x00df""e",wsFname);
	wfWcs("splitPath","_wsplitpath(wsExt)",L".txt",wsExt);

	dbgNil("_wsplitpath(L\"/usr/local/symantec/Gr\x00fc""\x00df""e.u\x00df""r\",wsDrive,wsDir,wsFname,wsExt)");
	_wsplitpath(L"/usr/local/symantec/Gr\x00fc""\x00df""e.u\x00df""r",wsDrive,wsDir,wsFname,wsExt);
	dbgWcs("wsDrive == \"%ls\"",wsDrive);
	dbgWcs("wsDir == \"%ls\"",wsDir);
	dbgWcs("wsFname == \"%ls\"",wsFname);
	dbgWcs("wsExt == \"%ls\"",wsExt);
	wfWcs("splitPath","_wsplitpath(wsDrive)",L"",wsDrive);
	wfWcs("splitPath","_wsplitpath(wsDir)",L"/usr/local/symantec/",wsDir);
	wfWcs("splitPath","_wsplitpath(wsFname)",L"Gr\x00fc""\x00df""e",wsFname);
	wfWcs("splitPath","_wsplitpath(wsExt)",L".u\x00df""r",wsExt);

	dbgNil("_wsplitpath(L\"/u\x00df""r/local/\x00fc""ber/unter/Skr\x00e4""be\x00e5""n/rinah\",wsDrive,wsDir,wsFname,wsExt)");
	_wsplitpath(L"/u\x00df""r/local/\x00fc""ber/unter/Skr\x00e4""be\x00e5""n/rinah",wsDrive,wsDir,wsFname,wsExt);
	dbgWcs("wsDrive == \"%ls\"",wsDrive);
	dbgWcs("wsDir == \"%ls\"",wsDir);
	dbgWcs("wsFname == \"%ls\"",wsFname);
	dbgWcs("wsExt == \"%ls\"",wsExt);
	wfWcs("splitPath","_wsplitpath(wsDrive)",L"",wsDrive);
	wfWcs("splitPath","_wsplitpath(wsDir)",L"/u\x00df""r/local/\x00fc""ber/unter/Skr\x00e4""be\x00e5""n/",wsDir);
	wfWcs("splitPath","_wsplitpath(wsFname)",L"rinah",wsFname);
	wfWcs("splitPath","_wsplitpath(wsExt)",L"",wsExt);

	dbgNil("_wsplitpath(L\"\\u\x00df""r/local\\\x00fc""ber/unter\\Skr\x00e4""be\x00e5""n\\rinah\",wsDrive,wsDir,wsFname,wsExt)");
	_wsplitpath(L"\\u\x00df""r/local\\\x00fc""ber/unter\\Skr\x00e4""be\x00e5""n\\rinah",wsDrive,wsDir,wsFname,wsExt);
	dbgWcs("wsDrive == \"%ls\"",wsDrive);
	dbgWcs("wsDir == \"%ls\"",wsDir);
	dbgWcs("wsFname == \"%ls\"",wsFname);
	dbgWcs("wsExt == \"%ls\"",wsExt);
	wfWcs("splitPath","_wsplitpath(wsDrive)",L"",wsDrive);
	wfWcs("splitPath","_wsplitpath(wsDir)",L"/u\x00df""r/local/\x00fc""ber/unter/Skr\x00e4""be\x00e5""n/",wsDir);
	wfWcs("splitPath","_wsplitpath(wsFname)",L"rinah",wsFname);
	wfWcs("splitPath","_wsplitpath(wsExt)",L"",wsExt);

	dbgNil("_wsplitpath(L\"rinah.exe\",wsDrive,wsDir,wsFname,wsExt)");
	_wsplitpath(L"rinah.exe",wsDrive,wsDir,wsFname,wsExt);
	dbgWcs("wsDrive == \"%ls\"",wsDrive);
	dbgWcs("wsDir == \"%ls\"",wsDir);
	dbgWcs("wsFname == \"%ls\"",wsFname);
	dbgWcs("wsExt == \"%ls\"",wsExt);
	wfWcs("splitPath","_wsplitpath(wsDrive)",L"",wsDrive);
	wfWcs("splitPath","_wsplitpath(wsDir)",L"",wsDir);
	wfWcs("splitPath","_wsplitpath(wsFname)",L"rinah",wsFname);
	wfWcs("splitPath","_wsplitpath(wsExt)",L".exe",wsExt);

	dbgNil("_wsplitpath(L\"rinah\",wsDrive,wsDir,wsFname,wsExt)");
	_wsplitpath(L"rinah",wsDrive,wsDir,wsFname,wsExt);
	dbgWcs("wsDrive == \"%ls\"",wsDrive);
	dbgWcs("wsDir == \"%ls\"",wsDir);
	dbgWcs("wsFname == \"%ls\"",wsFname);
	dbgWcs("wsExt == \"%ls\"",wsExt);
	wfWcs("splitPath","_wsplitpath(wsDrive)",L"",wsDrive);
	wfWcs("splitPath","_wsplitpath(wsDir)",L"",wsDir);
	wfWcs("splitPath","_wsplitpath(wsFname)",L"rinah",wsFname);
	wfWcs("splitPath","_wsplitpath(wsExt)",L"",wsExt);

	dbgNil("_wsplitpath(L\"\",wsDrive,wsDir,wsFname,wsExt)");
	_wsplitpath(L"",wsDrive,wsDir,wsFname,wsExt);
	dbgWcs("wsDrive == \"%ls\"",wsDrive);
	dbgWcs("wsDir == \"%ls\"",wsDir);
	dbgWcs("wsFname == \"%ls\"",wsFname);
	dbgWcs("wsExt == \"%ls\"",wsExt);
	wfWcs("splitPath","_wsplitpath(wsDrive)",L"",wsDrive);
	wfWcs("splitPath","_wsplitpath(wsDir)",L"",wsDir);
	wfWcs("splitPath","_wsplitpath(wsFname)",L"",wsFname);
	wfWcs("splitPath","_wsplitpath(wsExt)",L"",wsExt);

	// _splitpath

	dbgNil("_splitpath(\"c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt\",NULL,NULL,NULL,NULL)");
	_splitpath("c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt",NULL,NULL,NULL,NULL);

	dbgNil("_splitpath(\"c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt\",NULL,sDir,sFname,sExt)");
	_splitpath("c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt",NULL,sDir,sFname,sExt);
	dbgStr("sDir == \"%s\"",sDir);
	dbgStr("sFname == \"%s\"",sFname);
	dbgStr("sExt == \"%s\"",sExt);
	wfStr("splitPath","_splitpath(sDir)","/usr/local/symantec/",sDir);
	wfStr("splitPath","_splitpath(sFname)","Gr\x00fc""\x00df""e",sFname);
	wfStr("splitPath","_splitpath(sExt)",".txt",sExt);

	dbgNil("_splitpath(\"c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt\",sDrive,NULL,sFname,sExt)");
	_splitpath("c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt",sDrive,NULL,sFname,sExt);
	dbgStr("sDrive == \"%s\"",sDrive);
	dbgStr("sFname == \"%s\"",sFname);
	dbgStr("sExt == \"%s\"",sExt);
	wfStr("splitPath","_splitpath(sDrive)","",sDrive);
	wfStr("splitPath","_splitpath(sFname)","Gr\x00fc""\x00df""e",sFname);
	wfStr("splitPath","_splitpath(sExt)",".txt",sExt);

	dbgNil("_splitpath(\"c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt\",sDrive,sDir,NULL,sExt)");
	_splitpath("c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt",sDrive,sDir,NULL,sExt);
	dbgStr("sDrive == \"%s\"",sDrive);
	dbgStr("sDir == \"%s\"",sDir);
	dbgStr("sExt == \"%s\"",sExt);
	wfStr("splitPath","_splitpath(sDrive)","",sDrive);
	wfStr("splitPath","_splitpath(sDir)","/usr/local/symantec/",sDir);
	wfStr("splitPath","_splitpath(sExt)",".txt",sExt);

	dbgNil("_splitpath(\"c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt\",sDrive,sDir,sFname,NULL)");
	_splitpath("c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt",sDrive,sDir,sFname,NULL);
	dbgStr("sDrive == \"%s\"",sDrive);
	dbgStr("sDir == \"%s\"",sDir);
	dbgStr("sFname == \"%s\"",sFname);
	wfStr("splitPath","_splitpath(sDrive)","",sDrive);
	wfStr("splitPath","_splitpath(sDir)","/usr/local/symantec/",sDir);
	wfStr("splitPath","_splitpath(sFname)","Gr\x00fc""\x00df""e",sFname);

	dbgNil("_splitpath(\"c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt\",sDrive,sDir,sFname,sExt)");
	_splitpath("c:/usr/local/symantec/Gr\x00fc""\x00df""e.txt",sDrive,sDir,sFname,sExt);
	dbgStr("sDrive == \"%s\"",sDrive);
	dbgStr("sDir == \"%s\"",sDir);
	dbgStr("sFname == \"%s\"",sFname);
	dbgStr("sExt == \"%s\"",sExt);
	wfStr("splitPath","_splitpath(sDrive)","",sDrive);
	wfStr("splitPath","_splitpath(sDir)","/usr/local/symantec/",sDir);
	wfStr("splitPath","_splitpath(sFname)","Gr\x00fc""\x00df""e",sFname);
	wfStr("splitPath","_splitpath(sExt)",".txt",sExt);

	dbgNil("_splitpath(\"volume:/usr/local/symantec/Gr\x00fc""\x00df""e.txt\",sDrive,sDir,sFname,sExt)");
	_splitpath("volume:/usr/local/symantec/Gr\x00fc""\x00df""e.txt",sDrive,sDir,sFname,sExt);
	dbgStr("sDrive == \"%s\"",sDrive);
	dbgStr("sDir == \"%s\"",sDir);
	dbgStr("sFname == \"%s\"",sFname);
	dbgStr("sExt == \"%s\"",sExt);
	wfStr("splitPath","_splitpath(sDrive)","",sDrive);
	wfStr("splitPath","_splitpath(sDir)","/usr/local/symantec/",sDir);
	wfStr("splitPath","_splitpath(sFname)","Gr\x00fc""\x00df""e",sFname);
	wfStr("splitPath","_splitpath(sExt)",".txt",sExt);

	dbgNil("_splitpath(\"/usr/local/symantec/Gr\x00fc""\x00df""e.u\x00df""r\",sDrive,sDir,sFname,sExt)");
	_splitpath("/usr/local/symantec/Gr\x00fc""\x00df""e.u\x00df""r",sDrive,sDir,sFname,sExt);
	dbgStr("sDrive == \"%s\"",sDrive);
	dbgStr("sDir == \"%s\"",sDir);
	dbgStr("sFname == \"%s\"",sFname);
	dbgStr("sExt == \"%s\"",sExt);
	wfStr("splitPath","_splitpath(sDrive)","",sDrive);
	wfStr("splitPath","_splitpath(sDir)","/usr/local/symantec/",sDir);
	wfStr("splitPath","_splitpath(sFname)","Gr\x00fc""\x00df""e",sFname);
	wfStr("splitPath","_splitpath(sExt)",".u\x00df""r",sExt);

	dbgNil("_splitpath(\"/u\x00df""r/local/\x00fc""ber/unter/Skr\x00e4""be\x00e5""n/rinah\",sDrive,sDir,sFname,sExt)");
	_splitpath("/u\x00df""r/local/\x00fc""ber/unter/Skr\x00e4""be\x00e5""n/rinah",sDrive,sDir,sFname,sExt);
	dbgStr("sDrive == \"%s\"",sDrive);
	dbgStr("sDir == \"%s\"",sDir);
	dbgStr("sFname == \"%s\"",sFname);
	dbgStr("sExt == \"%s\"",sExt);
	wfStr("splitPath","_splitpath(sDrive)","",sDrive);
	wfStr("splitPath","_splitpath(sDir)","/u\x00df""r/local/\x00fc""ber/unter/Skr\x00e4""be\x00e5""n/",sDir);
	wfStr("splitPath","_splitpath(sFname)","rinah",sFname);
	wfStr("splitPath","_splitpath(sExt)","",sExt);

	dbgNil("_splitpath(\"\\u\x00df""r/local\\\x00fc""ber/unter\\Skr\x00e4""be\x00e5""n\\rinah\",sDrive,sDir,sFname,sExt)");
	_splitpath("\\u\x00df""r/local\\\x00fc""ber/unter\\Skr\x00e4""be\x00e5""n\\rinah",sDrive,sDir,sFname,sExt);
	dbgStr("sDrive == \"%s\"",sDrive);
	dbgStr("sDir == \"%s\"",sDir);
	dbgStr("sFname == \"%s\"",sFname);
	dbgStr("sExt == \"%s\"",sExt);
	wfStr("splitPath","_splitpath(sDrive)","",sDrive);
	wfStr("splitPath","_splitpath(sDir)","/u\x00df""r/local/\x00fc""ber/unter/Skr\x00e4""be\x00e5""n/",sDir);
	wfStr("splitPath","_splitpath(sFname)","rinah",sFname);
	wfStr("splitPath","_splitpath(sExt)","",sExt);

	dbgNil("_splitpath(\"rinah.exe\",sDrive,sDir,sFname,sExt)");
	_splitpath("rinah.exe",sDrive,sDir,sFname,sExt);
	dbgStr("sDrive == \"%s\"",sDrive);
	dbgStr("sDir == \"%s\"",sDir);
	dbgStr("sFname == \"%s\"",sFname);
	dbgStr("sExt == \"%s\"",sExt);
	wfStr("splitPath","_splitpath(sDrive)","",sDrive);
	wfStr("splitPath","_splitpath(sDir)","",sDir);
	wfStr("splitPath","_splitpath(sFname)","rinah",sFname);
	wfStr("splitPath","_splitpath(sExt)",".exe",sExt);

	dbgNil("_splitpath(\"rinah\",sDrive,sDir,sFname,sExt)");
	_splitpath("rinah",sDrive,sDir,sFname,sExt);
	dbgStr("sDrive == \"%s\"",sDrive);
	dbgStr("sDir == \"%s\"",sDir);
	dbgStr("sFname == \"%s\"",sFname);
	dbgStr("sExt == \"%s\"",sExt);
	wfStr("splitPath","_splitpath(sDrive)","",sDrive);
	wfStr("splitPath","_splitpath(sDir)","",sDir);
	wfStr("splitPath","_splitpath(sFname)","rinah",sFname);
	wfStr("splitPath","_splitpath(sExt)","",sExt);

	dbgNil("_splitpath(\"\",sDrive,sDir,sFname,sExt)");
	_splitpath("",sDrive,sDir,sFname,sExt);
	dbgStr("sDrive == \"%s\"",sDrive);
	dbgStr("sDir == \"%s\"",sDir);
	dbgStr("sFname == \"%s\"",sFname);
	dbgStr("sExt == \"%s\"",sExt);
	wfStr("splitPath","_splitpath(sDrive)","",sDrive);
	wfStr("splitPath","_splitpath(sDir)","",sDir);
	wfStr("splitPath","_splitpath(sFname)","",sFname);
	wfStr("splitPath","_splitpath(sExt)","",sExt);
}

// --------------------- End Of File ----------------------------------------------------

static void endOfFile() {
	int fd,r;

	dbgInt("fd = open(\"/etc/hosts\",O_RDONLY) -> %i",fd = open("/etc/hosts",O_RDONLY));

	dbgInt("eof(fd) -> %i",r = eof(fd)); wfInt("endOfFile","eof()",0,r);
	dbgInt("lseek(fd,0,SEEK_END) -> %i",lseek(fd,0,SEEK_END));
	dbgInt("eof(fd) -> %i",r = eof(fd)); wfInt("endOfFile","eof()",1,r);

	dbgInt("close(fd) -> %i",close(fd));
}

// --------------------- File Length ----------------------------------------------------

static void fileLength() {
	int fd;

	dbgInt("fd = open(\"/etc/hosts\",O_RDONLY) -> %i",fd = open("/etc/hosts",O_RDONLY));

	dbgLng("filelength(fd) -> %d",filelength(fd));

	dbgInt("close(fd) -> %i",close(fd));
}

// --------------------- Detect Keyboard Hit --------------------------------------------

static void detectKeyboardHit() {
	int r;

	// TODO __fpurge is nonstandard and not portable
	dbgNil("Don't hit the keyboard now !"); if (kbhit()) __fpurge(stdin);
	dbgInt("kbhit() -> %i",r = kbhit()); wfInt("detectKeyboardHit","kbhit()",0,r);

	if (dbgVerbose) {
		dbgNil("Enter one character and end with \"Enter\" !"); dbgAwait(3 * 1000);
		dbgInt("kbhit() -> %i",r = kbhit()); wfInt("detectKeyboardHit","kbhit()",1,r);
		putc(getc(stdin),stdout); putc(getc(stdin),stdout);
	}
}

// --------------------- Read Directory -------------------------------------------------

static char* sDosDate(char* sDate,WORD wDate) {
	sprintf(sDate,"%i/%i/%i",
			  GET_MONTH_FROM_DATE(wDate),
			  GET_DAY_FROM_DATE(	 wDate),
			  GET_YEAR_FROM_DATE( wDate) + 1980);
	return sDate;
}
static char* sDosTime(char* sTime,WORD wTime) {
	sprintf(sTime,"%i:%02i:%02i",
			  GET_HOUR_FROM_TIME(	 wTime),
			  GET_MINUTE_FROM_TIME(	 wTime),
			  GET_BISECOND_FROM_TIME(wTime) * 2);
	return sTime;
}
static char* sDosDateTime(char* sDateTime,DWORD dDateTime) {
	char sDate[11],sTime[9]; strcpy(sDateTime,"");
	if (dDateTime) {
		strcat(sDateTime,sDosDate(sDate,dDateTime >> 16));
		strcat(sDateTime," ");
		strcat(sDateTime,sDosTime(sTime,dDateTime & 0xFFFF));
	}
	return sDateTime;
}

static void readDirectory() {
	DIR* pDIR; char s[21];

	dbgPtr("pDIR = opendir(\"/boot\") -> %u\n",pDIR = opendir("/boot"));

	while (readdir(pDIR)) {
		switch (pDIR->d_attr) {
      case _A_NORMAL: dbgStr("d_attr      == %s","_A_NORMAL"); break;
      case _A_SUBDIR: dbgStr("d_attr      == %s","_A_SUBDIR"); break;
      default:        dbgInt("d_attr      == %u",pDIR->d_attr);
      }
      dbgStr("d_date      == %s",sDosDate(s,pDIR->d_date));
      dbgStr("d_time      == %s",sDosTime(s,pDIR->d_time));
      dbgLng("d_size      == %d",pDIR->d_size);
      dbgStr("d_cdatetime == %s",sDosDateTime(s,pDIR->d_cdatetime));
      dbgStr("d_adatetime == %s",sDosDateTime(s,pDIR->d_adatetime));
      dbgStr("d_name      == \"%s\"\n",pDIR->d_name);
	}
	dbgInt("closedir(pDIR) -> %i",closedir(pDIR));
}

static void readDirectoryWild() {
	DIR* pDIR; char s[21];

	dbgPtr("pDIR = opendir(\"/boot/*.*\") -> %u\n",pDIR = opendir("/boot/*.*"));

	while (readdir(pDIR)) {
		switch (pDIR->d_attr) {
		case _A_NORMAL: dbgStr("d_attr      == %s","_A_NORMAL"); break;
		case _A_SUBDIR: dbgStr("d_attr      == %s","_A_SUBDIR"); break;
		default:        dbgInt("d_attr      == %u",pDIR->d_attr);
		}
		dbgStr("d_date      == %s",sDosDate(s,pDIR->d_date));
		dbgStr("d_time      == %s",sDosTime(s,pDIR->d_time));
		dbgLng("d_size      == %d",pDIR->d_size);
		dbgStr("d_cdatetime == %s",sDosDateTime(s,pDIR->d_cdatetime));
		dbgStr("d_adatetime == %s",sDosDateTime(s,pDIR->d_adatetime));
		dbgStr("d_name      == \"%s\"\n",pDIR->d_name);
	}
	dbgInt("closedir(pDIR) -> %i",closedir(pDIR));
}

// --------------------- Get File Info --------------------------------------------------

static char* sDateTime(char* sTime,time_t tTime) {
	struct tm dateTime; gmtime_r(&tTime,&dateTime);
	sprintf(sTime,"%i/%i/%i %i:%02i:%02i",
			  dateTime.tm_mon + 1, 
			  dateTime.tm_mday,    
			  dateTime.tm_year + 1900,
			  dateTime.tm_hour,    
			  dateTime.tm_min,     
			  dateTime.tm_sec);
	return sTime;
}
static void dbgStat(struct stat* pStat) {
    char s[21];
    
    dbgInt("st_dev     == %u",pStat->st_dev);
    dbgInt("st_ino     == %u",pStat->st_ino);
    dbgStr("st_mode    == 0%s",itoa(pStat->st_mode,s,8));
    dbgInt("st_nlink   == %u",pStat->st_nlink);
    dbgInt("st_uid     == %u",pStat->st_uid);
    dbgInt("st_gid     == %u",pStat->st_gid);
    dbgInt("st_rdev    == %u",pStat->st_rdev);
    dbgInt("st_size    == %u",pStat->st_size);
    dbgStr("st_atime   == %s",sDateTime(s,pStat->st_atime));
    dbgStr("st_mtime   == %s",sDateTime(s,pStat->st_mtime));
    dbgStr("st_ctime   == %s",sDateTime(s,pStat->st_ctime));
    dbgInt("st_blksize == %u",pStat->st_blksize);
    dbgInt("st_blocks  == %u\n",pStat->st_blocks);
}

static void getFileInfo() {
	struct stat fileInfo,fdInfo; int rc,fd;

	dbgInt("stat(\"/etc/hosts\",&fileInfo) -> %i",rc = stat("/etc/hosts",&fileInfo));
	wfInt("getFileInfo","stat()",0,rc);
	dbgStat(&fileInfo);

	dbgInt("fd = open(\"/etc/hosts\",O_RDONLY) -> %i\n",fd = open("/etc/hosts",O_RDONLY));
	dbgInt("fstat(fd,&fdInfo) -> %i",rc = fstat(fd,&fdInfo));
	wfInt("getFileInfo","fstat()",0,rc); dbgStat(&fdInfo);
	dbgInt("close(fd) -> %i",close(fd));
}

// --------------------- main -----------------------------------------------------------

int main(int argc,char* argv[]) {
	if (dbgInit(argc,argv,"Paths")) {
		dbgStr("NWLsetlocale(LC_CTYPE,\"en_US\") -> \"%s\"",
				 NWLsetlocale(LC_CTYPE,"en_US.UTF-8"));
		
		dbgNL("makePath()");				makePath();
		dbgNL("splitPath()");			splitPath();
		dbgNL("endOfFile()");			endOfFile();
		dbgNL("fileLength()");			fileLength();
		dbgNL("detectKeyboardHit()"); detectKeyboardHit();
		dbgNL("readDirectory()");		readDirectory();
		dbgNL("readDirectoryWild()");		readDirectoryWild();
		dbgNL("getFileInfo()");			getFileInfo();
	}
	return dbgFinal();
}
