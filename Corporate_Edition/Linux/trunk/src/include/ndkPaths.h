// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Linux implementation of NDK File Path Handling functions

#ifndef _ndkPaths_h_
#define _ndkPaths_h_

#include "ndkStrings.h"

#ifdef __cplusplus
extern "C" {
#endif

#define _tmakepath TextMapping(_wmakepath,_makepath,_makepath)
void _wmakepath(wchar_t* path,const wchar_t* drive,
					 const wchar_t* dir,const wchar_t* fname,const wchar_t* ext);
void _makepath(char* path,const char* drive,
					const char* dir,const char* fname,const char* ext);

#define _tsplitpath TextMapping(_wsplitpath,_splitpath,_splitpath)
void _wsplitpath(const wchar_t* path,wchar_t* drive,
					  wchar_t* dir,wchar_t* fname,wchar_t* ext);
void _splitpath(const char* path,char* drive,
					 char* dir,char* fname,char* ext);

int eof(int fd);
long filelength(int fd);

#define tell(fd) lseek(fd,0,SEEK_CUR)

int kbhit();

#define gettch TextMapping(getwch,getch,getch)
#define getwch() getwc(stdin)
#define getch()  getc( stdin)

#define ungettch TextMapping(ungetwch,ungetch,ungetch)
#define ungetwch(c) ungetwc(c,stdin)
#define ungetch(c)  ungetc( c,stdin)

char* mkUnixPath(char* path);

#ifdef __cplusplus
}
#endif
	
#endif // _ndkPaths_h_
