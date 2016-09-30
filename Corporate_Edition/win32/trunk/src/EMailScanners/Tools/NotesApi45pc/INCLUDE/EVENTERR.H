// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef EVENT_ERR_DEFS
#define EVENT_ERR_DEFS

/*	Message codes for the Event Package */

#define ERR_EVENT_EXCEED_MEM	PKG_EVENT+1
	errortext(ERR_EVENT_EXCEED_MEM, 	"Warning: Cannot record event - cannot keep up with event occurrence rate!")
#define ERR_EVENT_DUPL_QUEUE	PKG_EVENT+2
	errortext(ERR_EVENT_DUPL_QUEUE, 	"A queue with that name already exists.")
#define ERR_EVENT_NOT_Q			PKG_EVENT+3
	errortext(ERR_EVENT_NOT_Q, 	"Not a known event queue.")
#define ERR_EVENT_EXCEED_QUOTA	PKG_EVENT+4
	errortext(ERR_EVENT_EXCEED_QUOTA, 	"Cannot enqueue event - quota exceeded!")
#define ERR_EVTQUEUE_EMPTY		PKG_EVENT+5
	errortext(ERR_EVTQUEUE_EMPTY, 	"No entries in this queue")
#define ERR_EVENT_NO_SUCH_QUEUE	PKG_EVENT+6
	errortext(ERR_EVENT_NO_SUCH_QUEUE, 	"No such queue")
#define ERR_EVENT_NOT_YOUR_Q 	PKG_EVENT+7
	errortext(ERR_EVENT_NOT_YOUR_Q, 	"Can only access your queue.")
#define ERR_EVENT_WRONG_VERSION	PKG_EVENT+8
	errortext(ERR_EVENT_WRONG_VERSION,"Wrong version of event package.")
#define ERR_EVENT_BAD_SEVERITY	PKG_EVENT+9
	errortext(ERR_EVENT_BAD_SEVERITY,"Invalid severity encountered.")
#define ERR_EVENT_NOT_INITIALIZED	PKG_EVENT+10
	errortext(ERR_EVENT_NOT_INITIALIZED,"Event support not initialized.")
#define ERR_EVENT_POOLFULL		PKG_EVENT+11
	errortext(ERR_EVENT_POOLFULL,"Insufficient memory - event pool is full.")
#define ERR_EVENT_SHUTDOWN		PKG_EVENT+12
	internaltext(ERR_EVENT_SHUTDOWN,"Shutdown requested")

/* Since this name space is so underutilized, it will share its top */
/* space with the briefcase.  So unlike most error ranges, PKG_EVENT */
/* can only range from 0 to 127.  PKG_BCASE goes from 128 to 256. */
#endif
