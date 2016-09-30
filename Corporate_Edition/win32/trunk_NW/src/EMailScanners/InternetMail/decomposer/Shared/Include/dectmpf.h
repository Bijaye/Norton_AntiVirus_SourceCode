// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/* Temporary file reuse support for decomposers. On AS/400, the decomposers
are significantly slowed by the time spent deleting and creating temp files.
These operations are slow on the AS/400 IFS. This support allows us to keep
temp files around and reuse them instead of deleting and re-creating. It
results in a significant performance improvement on AS/400. */

#ifndef _DECTMPF_H
#define _DECTMPF_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEC_REUSE_TEMP_FILES
	void dec_purge_tfn_cache(void);
	void dec_release_name(const char *name);
	int dec_rm(const char *fname);
#endif // defined DEC_REUSE_TEMP_FILES

int dec_get_tempname(const char *dir, const char *ext, char *name_out);

#ifdef __cplusplus
}
#endif

#endif
