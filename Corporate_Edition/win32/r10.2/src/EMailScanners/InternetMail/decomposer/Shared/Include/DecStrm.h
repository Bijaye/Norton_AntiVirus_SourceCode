//------------------------------------------------------------------------
// DecStrm.h
//
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) Symantec, Corp. 2001, 2005. All rights reserved.
//------------------------------------------------------------------------

#ifndef DECSTRM_H
#define DECSTRM_H

/////////////////////////////////////////////////////////////////////////////
// Class CDecSDKIO

class CDecSDKIO :
	public IDecIO
{
public:
	CDecSDKIO();
	~CDecSDKIO();

	virtual int		Dec_access(const char *path, int mode);
	virtual int		Dec_chmod(const char *filename, int pmode);
	virtual void	Dec_clearerr(FILE *stream);
	virtual int		Dec_fclose(FILE *stream);
	virtual int		Dec_feof(FILE *stream);
	virtual int		Dec_ferror(FILE *stream);
	virtual int		Dec_fflush(FILE *stream);
	virtual int		Dec_fgetc(FILE *stream);
	virtual char	*Dec_fgets(char *string, int n, FILE *stream);
	virtual FILE	*Dec_fopen(const char *filename, const char *mode);
	virtual int		Dec_fputc(int c, FILE *stream);
	virtual int		Dec_fputs(const char *szstring, FILE *stream);
	virtual size_t	Dec_fread(void *buffer, size_t size, size_t count, FILE *stream);
	virtual int		Dec_fseek(FILE *stream, long offset, int origin);
	virtual long	Dec_ftell(FILE *stream);
	virtual int		Dec_ftruncate(FILE *stream, size_t size);
	virtual size_t	Dec_fwrite(const void *buffer, size_t size, size_t count, FILE *stream);
	virtual int		Dec_remove(const char *path);
	virtual int		Dec_rename(const char *oldname, const char *newname);
	virtual void	Dec_rewind(FILE *stream);
	virtual int		Dec_setvbuf(FILE *stream, char *buffer, int mode, size_t size);
#if defined(_WINDOWS)
	virtual int		Dec_stat(const char *path, struct _stat *buffer);
	virtual int		Dec_utime(const char *filename, struct _utimbuf *times);
#else
	virtual int		Dec_stat(const char *path, struct stat *buffer);
	virtual int		Dec_utime(const char *filename, struct utimbuf *times);
#endif
	virtual int		Dec_ungetc(int c, FILE *stream);
	virtual int		Dec_fprintf(FILE *stream, const char *format, ...);

	virtual void	SetFileSystem(IDecFiles *pfs);

private:
	IDecFiles	*m_pfs;
};

#endif // DECSTRM_H
