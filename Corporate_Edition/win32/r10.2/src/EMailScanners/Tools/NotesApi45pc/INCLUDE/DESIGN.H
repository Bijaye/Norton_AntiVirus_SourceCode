// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

/* Database Design Definitions */

#ifndef DESIGN_DEFS
#define DESIGN_DEFS

/*	Identifier for different types of design elements and macros
	for working with them. Used in the Folder API (foldman.h) */

typedef DWORD DESIGN_TYPE;

#define DESIGN_TYPE_SHARED				0	   	/* Note is shared (always located in the database) */
#define DESIGN_TYPE_PRIVATE_DATABASE	1	   	/* Note is private and is located in the database */

#endif /* DESIGN_DEFS */


