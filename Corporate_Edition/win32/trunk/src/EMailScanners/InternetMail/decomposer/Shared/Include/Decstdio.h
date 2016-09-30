// Decstdio.h : Decomposer I/O Interface
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DECSTDIO_H)
#define DECSTDIO_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined(_WINDOWS)
	#include <sys/utime.h>
	#include <io.h>
#else
	#include <utime.h>
	#include <unistd.h>
#endif

#if defined(SYM_LINUX)
	#define _S_IREAD	__S_IREAD
	#define _S_IWRITE	__S_IWRITE
#endif

#if defined(SYM_SOLARIS)
	#define _S_IREAD	S_IREAD
	#define _S_IWRITE	S_IWRITE
#endif

#if defined(SYM_FREEBSD)
	#define _S_IREAD	S_IREAD
	#define _S_IWRITE	S_IWRITE
#endif

#if defined(SYM_AIX)
	#define _S_IREAD	S_IREAD
	#define _S_IWRITE	S_IWRITE
#endif

//
// If temp file caching is turned on, then we will
// bypass the SDK by default.
//
#if defined(DEC_REUSE_TEMP_FILES)
	#if !defined(BYPASS_DECSDK)
		#define BYPASS_DECSDK 1
	#endif

	#include "dectmpf.h"
#endif

//
// NOTE: If BYPASS_DECSDK is defined, we still do not redefine dec_fprintf.  This
//       because dec_fprintf calls dec_fwrite itself and dec_fwrite is already
//       redefined below.
//
#if defined(BYPASS_DECSDK)
	#define dec_access(path, mode)					access(path, mode)
	#define dec_chmod(filename, pmode)				chmod(filename, pmode)
	#define dec_clearerr(stream)					clearerr(stream)
	#define dec_fclose(stream)						fclose(stream)
	#define dec_feof(stream)						feof(stream)
	#define dec_ferror(stream)						ferror(stream)
	#define dec_fflush(stream)						fflush(stream)
	#define dec_fgetc(stream)						fgetc(stream)
	#define dec_fgets(string, n, stream)			fgets(string, n, stream)
	#define dec_fopen(filename, mode)				fopen(filename, mode)
	#define dec_fputc(c, stream)					fputc(c, stream)
	#define dec_fputs(szstring, stream)				fputs(szstring, stream)
	#define dec_fread(buffer, size, count, stream)	fread(buffer, size, count, stream)
	#define dec_fseek(stream, offset, origin)		fseek(stream, offset, origin)
	#define dec_ftell(stream)						ftell(stream)

	#if defined(_WINDOWS) || defined(SYM_NLM)
		#define dec_ftruncate(stream, size)			chsize(fileno(stream), size)
	#else
		#define dec_ftruncate(stream, size)			ftruncate(fileno(stream), size)
	#endif

	#define dec_fwrite(buffer, size, count, stream) fwrite(buffer, size, count, stream)

	//
	// If temp file caching is turned on, use dec_rm instead
	//
	#if defined(DEC_REUSE_TEMP_FILES)
		#define dec_remove(path)					dec_rm(path)
	#else
		#define dec_remove(path)					remove(path)
	#endif

	#define dec_rename(oldname, newname)			rename(oldname, newname)
	#define dec_rewind(stream)						rewind(stream)
	#define dec_setvbuf(stream, buffer, mode, size) setvbuf(stream, buffer, mode, size)

	#if defined(_WINDOWS)
		#define dec_stat(path, buffer)				_stat(path, buffer)
		#define dec_utime(filename, times)			_utime(filename, times)
	#else
		#define dec_stat(path, buffer)				stat(path, buffer)
		#define dec_utime(filename, times)			utime(filename, times)
	#endif

	#define dec_ungetc(c, stream)					ungetc(c, stream)

	int		dec_fprintf(FILE *stream, const char *format, ...);
#else
	// C-language interface for Decomposer stream I/O functions.
	// These functions are defined exactly the same as the corresponding
	// POSIX functions so that modifying existing code to use these
	// functions from inside the Decomposer is just a matter of replacing
	// the calls with dec_ plus the same name.
	int		dec_access(const char *path, int mode);
	int		dec_chmod(const char *filename, int pmode);
	void	dec_clearerr(FILE *stream);
	int		dec_fclose(FILE *stream);
	int		dec_feof(FILE *stream);
	int		dec_ferror(FILE *stream);
	int		dec_fflush(FILE *stream);
	int		dec_fgetc(FILE *stream);
	char	*dec_fgets(char *string, int n, FILE *stream);
	FILE	*dec_fopen(const char *filename, const char *mode);
	int		dec_fputc(int c, FILE *stream);
	int		dec_fputs(const char *szstring, FILE *stream);
	size_t	dec_fread(void *buffer, size_t size, size_t count, FILE *stream);
	int		dec_fseek(FILE *stream, long offset, int origin);
	long	dec_ftell(FILE *stream);
	int		dec_ftruncate(FILE *stream, size_t size);
	size_t	dec_fwrite(const void *buffer, size_t size, size_t count, FILE *stream);
	int		dec_remove(const char *path);
	int		dec_rename(const char *oldname, const char *newname);
	void	dec_rewind(FILE *stream);
	int		dec_setvbuf(FILE *stream, char *buffer, int mode, size_t size);

	#if defined(_WINDOWS)
		int		dec_stat(const char *path, struct _stat *buffer);
		int		dec_utime(const char *filename, struct _utimbuf *times);
	#else
		int		dec_stat(const char *path, struct stat *buffer);
		int		dec_utime(const char *filename, struct utimbuf *times);
	#endif

	int		dec_ungetc(int c, FILE *stream);
	int		dec_fprintf(FILE *stream, const char *format, ...);

#endif	// BYPASS_DECSDK

#if defined(SYM_NLM)
	#include <nwconio.h>

	// including nwconio.h causes TRUE and FALSE to be undefined
	#if !defined(TRUE)
		#define TRUE 1
	#endif
	#if !defined(FALSE)
		#define FALSE 0
	#endif
	#define dec_printf ConsolePrintf
#else
	#define dec_printf printf
#endif // SYM_NLM

#endif	// DECSTDIO_H

