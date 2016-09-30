// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

// Random.h

#ifndef __SCSCOMMS_RANDOM_H__
#define __SCSCOMMS_RANDOM_H__


#include "Random.h"


/**
 * GetRandomData:
 *
 * Get random data.  This will almost certainly be a platform specific implementation.
 *
 * @param buffer A buffer into which to copy random bytes
 * @param buffer_lenth Length of the buffer and number of bytes requested
 */
void GetRandomData( BYTE* buffer, unsigned long buffer_length );


#endif // __SCSCOMMS_RANDOM_H__

