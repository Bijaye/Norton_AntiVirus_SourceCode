;;************************************************************************
;;
;; $Header:   S:/navzip/VCS/noldname.asv   1.0   06 Feb 1997 21:06:02   RFULLER  $
;;
;; Description:
;;      This file copied from Microsoft C 8.0.
;;      Because the greenleaf libraries access the non ANSI names for RTL
;;      functions oldnames is required.  However, this causes a conflict
;;      with DOSUI.LIB.  Specifically the _KBHit symbol, which has been
;;      commented out of this file. (Greenleaf currently uses several other
;;      functions, but not kbhit().)
;;
;;      The GLUV linker will resolve multiple func definitions if the
;;      prototypes are the same, but does not appear to allow conflicting
;;      aliases.
;;
;;************************************************************************
;; $Log:   S:/navzip/VCS/noldname.asv  $
;  
;     Rev 1.0   06 Feb 1997 21:06:02   RFULLER
;  Initial revision
;  
;     Rev 1.0   24 Jun 1996 14:04:22   JALLEE
;  Initial revision.
;;************************************************************************
        page    ,132
	title	oldnames  - maps ANSI-conformant names to old names
;***
;oldnames.asm - maps ANSI-conformant names to old names
;
;	Copyright (c) 1988-1992, Microsoft Corporation.  All rights reserved.
;
;Purpose:
;	Maps old, names of externals to their counterparts in new libraries,
;	thus allowing old objects to be linked with new libs.  This is
;	particularly useful for supporting old non-ANSI names in the new
;	ANSI compatible libs.
;
;*******************************************************************************

;
;	entries are all of the form:
;
;		ALIAS	<alias name> = <substitute name>
;		ie.	  old name   =	  new name
;
;	The alias directive is only available in MASM 6.0 and newer
;	(under the undocumented command line switch /Ba) and also
;	requires C 7.0 versions of the LINK and LIB.
;
;	The entries below parallel appropriate #defines in the specified
;	header files.
;


;	bios.h

	alias	<_int86>	= <__int86>
	alias	<_int86x>	= <__int86x>

;	conio.h

	alias	<_cgets>	= <__cgets>
	alias	<_cprintf>	= <__cprintf>
	alias	<_cputs>	= <__cputs>
	alias	<_cscanf>	= <__cscanf>
	alias	<_getch>	= <__getch>
	alias	<_getche>	= <__getche>
	alias	<_inp>		= <__inp>
	alias	<_inpw>		= <__inpw>
;        alias   <_kbhit>        = <__kbhit>
	alias	<_outp>		= <__outp>
	alias	<_outpw>	= <__outpw>
	alias	<_putch>	= <__putch>
	alias	<_ungetch>	= <__ungetch>

;	ctype.h

	alias	<_isascii>	= <___isascii>
	alias	<_toascii>	= <___toascii>
	alias	<_iscsymf>	= <___iscsymf>
	alias	<_iscsym>	= <___iscsym>

;	direct.h

	alias	<_chdir>	= <__chdir>
	alias	<_getcwd>	= <__getcwd>
	alias	<_mkdir>	= <__mkdir>
	alias	<_rmdir>	= <__rmdir>

;	dos.h

	alias	<_bdos>		= <__bdos>
	alias	<_dosexterr>	= <__dosexterr>
	alias	<_intdos>	= <__intdos>
	alias	<_intdosx>	= <__intdosx>
;	alias	<_int86>	= <__int86>	; already defined above
;	alias	<_int86x>	= <__int86x>	; already defined above
	alias	<_segread>	= <__segread>

;	io.h

	alias	<_access>	= <__access>
	alias	<_chmod>	= <__chmod>
	alias	<_chsize>	= <__chsize>
	alias	<_close>	= <__close>
	alias	<_creat>	= <__creat>
	alias	<_dup>		= <__dup>
	alias	<_dup2>		= <__dup2>
	alias	<_eof>		= <__eof>
	alias	<_filelength>	= <__filelength>
	alias	<_isatty>	= <__isatty>
	alias	<_locking>	= <__locking>
	alias	<_lseek>	= <__lseek>
	alias	<_mktemp>	= <__mktemp>
	alias	<_open>		= <__open>
	alias	<_read>		= <__read>
	alias	<_setmode>	= <__setmode>
	alias	<_sopen>	= <__sopen>
	alias	<_tell>		= <__tell>
	alias	<_umask>	= <__umask>
	alias	<_unlink>	= <__unlink>
	alias	<_write>	= <__write>

;	malloc.h

	alias	<_alloca>	= <__alloca>
	alias	<_halloc>	= <__halloc>
	alias	<_hfree>	= <__hfree>
	alias	<_stackavail>	= <__stackavail>

;	math.h

	alias	<_HUGE> 	= <__HUGE>

	alias	<_cabs> 	= <__cabs>
	alias	<_dieeetomsbin> = <__dieeetomsbin>
	alias	<_dmsbintoieee> = <__dmsbintoieee>
	alias	<_fieeetomsbin> = <__fieeetomsbin>
	alias	<_fmsbintoieee> = <__fmsbintoieee>
	alias	<_hypot>	= <__hypot>
	alias	<_j0>		= <__j0>
	alias	<_j1>		= <__j1>
	alias	<_jn>		= <__jn>
	alias	<_y0>		= <__y0>
	alias	<_y1>		= <__y1>
	alias	<_yn>		= <__yn>

	alias	<_cabsl>	= <__cabsl>
	alias	<_hypotl>	= <__hypotl>

        ; PASCAL naming for MT libs
        alias   <CABS>          = <_CABS>
        alias   <HYPOT>         = <_HYPOT>
        alias   <J0>            = <_J0>
        alias   <J1>            = <_J1>
        alias   <JN>            = <_JN>
        alias   <Y0>            = <_Y0>
        alias   <Y1>            = <_Y1>
        alias   <YN>            = <_YN>

;       memory.h

        alias   <_memccpy>      = <__memccpy>
        alias   <_memicmp>      = <__memicmp>
        alias   <_movedata>     = <__movedata>

;       process.h

        alias   <_cwait>        = <__cwait>
        alias   <_execl>        = <__execl>
        alias   <_execle>       = <__execle>
        alias   <_execlp>       = <__execlp>
        alias   <_execlpe>      = <__execlpe>
        alias   <_execv>        = <__execv>
        alias   <_execve>       = <__execve>
        alias   <_execvp>       = <__execvp>
        alias   <_execvpe>      = <__execvpe>
        alias   <_getpid>       = <__getpid>
        alias   <_spawnl>       = <__spawnl>
        alias   <_spawnle>      = <__spawnle>
        alias   <_spawnlp>      = <__spawnlp>
        alias   <_spawnlpe>     = <__spawnlpe>
        alias   <_spawnv>       = <__spawnv>
        alias   <_spawnve>      = <__spawnve>
        alias   <_spawnvp>      = <__spawnvp>
        alias   <_spawnvpe>     = <__spawnvpe>
        alias   <_wait>         = <__wait>

;       search.h

        alias   <_lfind>        = <__lfind>
        alias   <_lsearch>      = <__lsearch>

;       setjmp.h

        alias   <_setjmp>       = <__setjmp>

;       stdlib.h

        alias   <_sys_errlist>  = <__sys_errlist>
        alias   <_sys_nerr>     = <__sys_nerr>
        alias   <_environ>      = <__environ>

        alias   <_ecvt>         = <__ecvt>
        alias   <_fcvt>         = <__fcvt>
        alias   <_gcvt>         = <__gcvt>
        alias   <_itoa>         = <__itoa>
        alias   <_ltoa>         = <__ltoa>
        alias   <_onexit>       = <__onexit>
        alias   <_putenv>       = <__putenv>
        alias   <_swab>         = <__swab>
        alias   <_ultoa>        = <__ultoa>

;       stdio.h

        alias   <_fcloseall>    = <__fcloseall>
        alias   <_fdopen>       = <__fdopen>
        alias   <_fgetchar>     = <__fgetchar>
        alias   <_fileno>       = <__fileno>
        alias   <_flushall>     = <__flushall>
        alias   <_fputchar>     = <__fputchar>
        alias   <_getw>         = <__getw>
        alias   <_putw>         = <__putw>
        alias   <_rmtmp>        = <__rmtmp>
        alias   <_tempnam>      = <__tempnam>
;       alias   <_unlink>       = <__unlink>    ; already defined above

;       string.h

;       alias   <_memccpy>      = <__memccpy>   ; already defined above
;       alias   <_memicmp>      = <__memicmp>   ; already defined above
;       alias   <_movedata>     = <__movedata>  ; already defined above
        alias   <_strcmpi>      = <__strcmpi>
        alias   <_stricmp>      = <__stricmp>
        alias   <_strdup>       = <__strdup>
        alias   <_strlwr>       = <__strlwr>
        alias   <_strnicmp>     = <__strnicmp>
        alias   <_strnset>      = <__strnset>
        alias   <_strrev>       = <__strrev>
        alias   <_strset>       = <__strset>
        alias   <_strupr>       = <__strupr>

;       time.h

        alias   <_daylight>     = <__daylight>
        alias   <_timezone>     = <__timezone>
        alias   <_tzname>       = <__tzname>
        alias   <_tzset>        = <__tzset>

;       sys\stat.h

        alias   <_fstat>        = <__fstat>
        alias   <_stat>         = <__stat>

;       sys\timeb.h

        alias   <_ftime>        = <__ftime>

;       sys\utime.h

        alias   <_utime>        = <__utime>

end
