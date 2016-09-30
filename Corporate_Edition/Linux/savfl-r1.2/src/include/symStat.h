// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Linux implementation of NDK File Information functions

#ifndef SYM_STAT_H_INCLUDED
#define SYM_STAT_H_INCLUDED

#include <sys/types.h>
#include <time.h>
#include <malloc.h>

#include "ndkDefinitions.h"

#ifdef __cplusplus
extern "C"
{
#endif

#undef st_atime
#undef st_mtime
#undef st_ctime

	// All times are in seconds since the Epoch: 1/1/1970 12:00:00 am UTC

struct symStat {							//depot/Tools/NDK0403/nwsdk/include/nlm/sys/stat.h
	dev_t				st_dev;				// volume number
	ino_t				st_ino;				// directory entry number of the st_name
	unsigned short st_mode;				// emulated file mode
	unsigned long	st_nlink;			// count of hard links (always 1)
	unsigned long	st_uid;				// object id of owner
	unsigned long	st_gid;				// group-id (always 0)
	dev_t				st_rdev;				// device type (always 0)
	off_t				st_size;				// total file size -- files only
	time_t			st_atime;			// last access date -- files only
	time_t			st_mtime;			// last modify date and time
	time_t			st_ctime;			// POSIX: last status change time...
												// ...NetWare: creation date/time
	time_t			st_btime;			// last archived date and time
	unsigned long	st_attr;				// file attributes: FILE_ATTRIBUTE_DIRECTORY FILE_ATTRIBUTE_NORMAL
	size_t			st_blksize;			// block size for allocation -- files only
	size_t			st_blocks;			// count of blocks allocated to file
};

int symStat( const char* path,  struct symStat* buf);
int symFstat(int			 fildes,struct symStat* buf);

#ifdef __cplusplus
}
#endif

#endif // SYM_STAT_H_INCLUDED
