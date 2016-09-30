// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef __cplusplus
extern "C" {
#endif


/* Mail file definitions */

#ifndef MAIL_DEFS
#define MAIL_DEFS

#ifndef STD_NAME_DEFS
#include "stdnames.h"
#endif

/* Mail note item table indexes (see Message Mailing functions in mailserv.h) */

#define MAIL_SENDTO_ITEM_NUM 0					/* SendTo */
#define	MAIL_COPYTO_ITEM_NUM 1					/* CopyTo */
#define MAIL_FROM_ITEM_NUM 2					/* From */
#define MAIL_SUBJECT_ITEM_NUM 3					/* Subject */
#define MAIL_COMPOSEDDATE_ITEM_NUM 4			/* Composed date */
#define MAIL_POSTEDDATE_ITEM_NUM 5				/* Posted date */
#define MAIL_BODY_ITEM_NUM 6					/* Body */
#define MAIL_INTENDEDRECIPIENT_ITEM_NUM	7		/* Intended recipient */
#define MAIL_FAILUREREASON_ITEM_NUM 8			/* Failure reason */
#define MAIL_RECIPIENTS_ITEM_NUM 9				/* Recipients list */
#define MAIL_ROUTINGSTATE_ITEM_NUM 10			/* Routing state */
#define MAIL_FORM_ITEM_NUM 11					/* Form name */
#define MAIL_SAVED_FORM_ITEM_NUM 12				/* Delivery report saved form name */
#define MAIL_BLINDCOPYTO_ITEM_NUM 13			/* Blind copy to */
#define MAIL_DELIVERYPRIORITY_ITEM_NUM 14		/* Delivery priority */
#define MAIL_DELIVERYREPORT_ITEM_NUM 15			/* Delivery report request  */
#define MAIL_DELIVEREDDATE_ITEM_NUM 16			/* Delivered date */
#define MAIL_DELIVERYDATE_ITEM_NUM 17			/* Delivery date (Confirmation Reports only) */
#define MAIL_CATEGORIES_ITEM_NUM 18				/* Categories */
#define MAIL_FROMDOMAIN_ITEM_NUM 19				/* From domain name */
#define MAIL_SENDTO_LIST_ITEM_NUM 20			/* SendTo TEXT_LIST*/
#define MAIL_RECIPIENTS_LIST_ITEM_NUM 21		/* Recipients TEXT_LIST*/
#define MAIL_RECIP_GROUPS_EXP_ITEM_NUM 22		/* List of recipient group names that have been expanded */
#define MAIL_RETURNRECEIPT_ITEM_NUM 23			/* Return receipt requested? */
#define MAIL_ROUTE_HOPS_ITEM_NUM 24				/* Routing hop count */
#define MAIL_CORRELATION_ITEM_NUM 25			/* Arbitrary delivery report correlation value. */
#define MAIL_ORIGINALPATH_ITEM_NUM 26			/* Original routing path (copy of original message's FromDomain) */
#define MAIL_DELIVER_LOOPS_ITEM_NUM 27			/* Forwarding loop count */
#define MAIL_DEAD_FAILUREREASON_ITEM_NUM 28		

#define MAIL_ITEMS 29							/* Number of items */
#define MAIL_TO_ITEMS 3							/* Number of To items */

/* Network domain name parameters */

#define MAIL_DOMAIN_DELIMITER "@"				/* Domain name delimiter string */

#define MAIL_DOMAIN_DELIMITER_NOSPACES 1		/* NOTE: Set this to 1 to revert to padding @'s with spaces */

#if MAIL_DOMAIN_DELIMITER_NOSPACES
#define MAIL_DOMAIN_DELIMITER_SPACES "@"		/* NOTE: Spaces removed in Build 145 because they break SMTP compatibility */
#else
#define MAIL_DOMAIN_DELIMITER_SPACES " @ "		/* Domain name delimiter string */
#endif

#define MAIL_DOMAIN_DELIMITER_CHAR '@'			/* Domain name delimiter character */
#define MAIL_SERVER_DELIMITER_CHAR ':'			/* Server delimiter character within domain specifier (xxx @ LOTUS:CPD-QA_SERVER) */
#define MAXRECIPIENTNAME 256					/* Maximum recipient name length */
#define MAXORIGLEN 12							/* Maximum $Orig logging string length */
#define MAXMESSAGEIDSTRLEN 50					/* Maximum logging message id string length */

/* Mail router mailbox file */

#define MAILBOX_NAME "mail.box"
#define MAILBOX_TEMPLATE_NAME "mailbox.ntf"
#define MAIL_TEMPLATE_NAME "mail45.ntf"
#define ADDRESSBOOK_NAME "names.nsf"
#define PUBLIC_ADDRESSBOOK_TEMPLATE_NAME "pubnames.ntf"
#define PERSONAL_ADDRESSBOOK_TEMPLATE_NAME "pernames.ntf"

/* SMTP MTA mailbox file */
#define SMTPMAILBOX_NAME "smtp.box"

/* X.400 MTA mailbox file */
#define X400MAILBOX_NAME "x400.box"
#endif

#ifdef __cplusplus
}
#endif

