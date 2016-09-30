/************************************************************************
 *                                                                      *
 * $Header:   I:/VCS/MSCSTR.H_V   1.4   06 May 1992 09:06:20   ED  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************ 
 * $Log:   I:/VCS/MSCSTR.H_V  $ *
 * 
 *    Rev 1.4   06 May 1992 09:06:20   ED
 * Masked the file from multiple includes
 * 
 *    Rev 1.3   13 Nov 1991 18:36:50   ED
 * Added all of the Microsoft _fstr functions
 * 
 *    Rev 1.2   06 Nov 1991 17:49:04   BARRY
 * Added _fstrnicmp
 * 
 *    Rev 1.1   16 Oct 1991 16:01:18   PETERD
 * Updated.
 ************************************************************************/

#ifndef INCL_MSCSTR
#define INCL_MSCSTR

/* Function definitions for strings */

void FAR * FAR	_fmemset(void FAR *, int, Word);
void FAR * FAR	_fmemcpy(void FAR *, const void FAR *, Word);
void FAR * FAR	_fmemmove(void FAR *, const void FAR *, Word);

Byte FAR * FAR _cdecl _fstrcat(Byte FAR *, const Byte FAR *);
Byte FAR * FAR _cdecl _fstrchr(const Byte FAR *, int);
int  FAR _cdecl       _fstrcmp(const Byte FAR *, const Byte FAR *);
int  FAR _cdecl       _fstricmp(const Byte FAR *, const Byte FAR *);
Byte FAR * FAR _cdecl _fstrcpy(Byte FAR *, const Byte FAR *);
Word FAR _cdecl       _fstrcspn(const Byte FAR *, const Byte FAR *);
Byte FAR * FAR _cdecl _fstrdup(const Byte FAR *);
Word FAR _cdecl       _fstrlen(const Byte FAR *);
Byte FAR * FAR _cdecl _fstrlwr(Byte FAR *);
Byte FAR * FAR _cdecl _fstrncat(Byte FAR *, const Byte FAR *, Word);
int  FAR _cdecl       _fstrncmp(const Byte FAR *, const Byte FAR *, Word);
int  FAR _cdecl       _fstrnicmp(const Byte FAR *, const Byte FAR *, Word);
Byte FAR * FAR _cdecl _fstrncpy(Byte FAR *, const Byte FAR *, Word);
Byte FAR * FAR _cdecl _fstrnset(Byte FAR *, int, Word);
Byte FAR * FAR _cdecl _fstrpbrk(const Byte FAR *, const Byte FAR *);
Byte FAR * FAR _cdecl _fstrrchr(const Byte FAR *, int);
Byte FAR * FAR _cdecl _fstrrev(Byte FAR *);
Byte FAR * FAR _cdecl _fstrset(Byte FAR *, int);
Word FAR _cdecl       _fstrspn(const Byte FAR *, const Byte FAR *);
Byte FAR * FAR _cdecl _fstrstr(const Byte FAR *, const Byte FAR *);
Byte FAR * FAR _cdecl _fstrtok(Byte FAR *, const Byte FAR *);
Byte FAR * FAR _cdecl _fstrupr(Byte FAR *);

#endif
