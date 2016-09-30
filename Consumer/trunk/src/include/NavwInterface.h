////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NAVW_INTERFACE_H
#define NAVW_INTERFACE_H

// NAVW Return values depending on Scan Results
enum NAVWRESULT
{
	// Will be returned if no infections were found
	NAVWRESULT_NOINFECTIONS = 0,
	// Will be returned if infections were found and some remain
	NAVWRESULT_INFECTIONS_REMAIN,
	// Will be returned if infections were found and all infections have been removed/repaired
	NAVWRESULT_ALLINFECTIONS_CLEAN,
	// Will be returned if something unusual happened
	NAVWRESULT_ERROR
};

#endif