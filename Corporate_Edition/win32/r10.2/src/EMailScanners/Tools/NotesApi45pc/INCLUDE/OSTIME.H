// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef __cplusplus
extern "C" {
#endif

/*	OS time/date package */

#ifndef TIMEDATE_DEFS
#define TIMEDATE_DEFS

void LNPUBLIC	OSCurrentTIMEDATE (TIMEDATE far *retTimeDate);
void LNPUBLIC	OSCurrentTimeZone (int far *retZone, int far *retDST);


#endif

#ifdef __cplusplus
}
#endif

