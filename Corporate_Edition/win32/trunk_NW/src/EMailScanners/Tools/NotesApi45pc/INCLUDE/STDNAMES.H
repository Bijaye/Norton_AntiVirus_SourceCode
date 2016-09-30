// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef __cplusplus
extern "C" {
#endif


#ifndef STD_NAME_DEFS
#define STD_NAME_DEFS

/*	Standard NSF Field Name/Value Definitions for the Notes product */

/* Very global field names */

#define	FIELD_TITLE "$TITLE"
#define	FIELD_FORM	"Form"
#define FIELD_TYPE_TYPE	"Type"
#define	FIELD_LINK	"$REF"
#define	FIELD_UPDATED_BY "$UpdatedBy"
#define FIELD_NAMELIST "$NameList"
#define FIELD_NAMED 	"$Name"
#define	FIELD_URL		"URL"				/* used when copying link to clipboard */
#define	FIELD_UNAME		"$UName"			/* Universal UNID/Name */
#define	FIELD_CREATED	"$Created"			/* Created date, if specified overrides UNID created date for @Created */
#define	DESIGN_CLASS "$Class"
#define	DESIGN_MODIFIED "$Modified"
#define	DESIGN_COMMENT FILTER_COMMENT_ITEM
#define	DESIGN_READERS "$Readers"			/* Text list of users allowed to read note */
#define FIELD_ANONYMOUS "$Anonymous"		/* Indicates an anonymous document. */
#define	DESIGN_UPDATERS "$Updaters"			/* Text list of users allowed to update the folder note */
#define FIELD_NOPURGE 	"$NoPurge"			/* Note should never be purged. */
#define	DESIGN_RETAIN_FIELDS "$RetainFields"	/* Text list of fields to retain in a
													design refresh */

/* Public Access definitions */
#define FIELD_PUBLICROLE 	"$P"			/* Reader List role name for public users */
#define FIELD_PUBLICACCESS 	"$PublicAccess"	/* Note has public access if ACL_FLAG_PUBLICREADER is set. */
#define FORM_FIELD_PUBLICACCESS 	"$FormPublicAccess"	/* Form Note has public access if ACL_FLAG_PUBLICREADER is set. */
#define	FIELD_PUBLICACCESS_ENABLED	'1'

#define ITEM_NOTEID "$$NoteID"	/* a NAMELookup item to be returned */

/*	Define Private note field name, and define the known types */

#define FIELD_PRIVATE_TYPE 	"$Private"

#define	FIELD_PRIVATE_TYPE_QUERY	'q'
#define	FIELD_PRIVATE_TYPE_AGENT	'a'
#define	FIELD_PRIVATE_TYPE_VIEW		'v'
#define	FIELD_PRIVATE_TYPE_FOLDER	'f'

/*	The following items are inserted into a note which cannot be saved to its
	Some special named notes. These values are stored in the FIELD_NAMED item to
	identify a named note. */

#define NAMEDNOTE_PRIVATEDESIGN		"$PrivateDesign"
#define NAMEDNOTE_PROFILE			"$Profile"

/*	The following items are inserted into a note which cannot be saved to its
	original database due to network failure.  When the failure can be
	remedied, the note is read back into memory and fixed up to point
	to correct database.  At that point the items are removed. */

#define PENDINGNAME					"NOTEPEND"
#define PENDINGSAVE_LINK_ITEM		"$ORIGLINK"
#define PENDINGSAVE_FILENAME_ITEM	"$ORIGFILE"

/*	Design flags */

#define	DESIGN_FLAGS "$Flags"

/*	Please keep these flags in alphabetic order (based on the flag itself) so that
	we can easily tell which flags to use next. Note that some of these flags apply
	to a particular NOTE_CLASS; others apply to all design elements. The comments
	indicate which is which. In theory, flags that apply to two different NOTE_CLASSes
	could overlap, but for now, try to make each flag unique. */

#define DESIGN_FLAG_ADD					'A'	/*	FORM: Indicates that a subform is in the add subform list */
#define	DESIGN_FLAG_BACKGROUND_FILTER 	'B'	/*	FILTER: Indicates FILTER_TYPE_BACKGROUND is asserted */
#define DESIGN_FLAG_NO_COMPOSE 			'C'	/*	FORM: Indicates a form that is used only for */
											/*		query by form (not on compose menu). */
#define DESIGN_FLAG_CALENDAR_VIEW		'c'	/*	VIEW: Indicates a form is a calendar style view. */
#define DESIGN_FLAG_NO_QUERY 			'D'	/* 	FORM: Indicates a form that should not be used in query by form */
#define DESIGN_FLAG_DEFAULT_DESIGN 		'd'	/* 	ALL: Indicates the default design note for it's class (used for VIEW) */
#define	DESIGN_FLAG_MAIL_FILTER 		'E'	/*	FILTER: Indicates FILTER_TYPE_MAIL is asserted */
#define DESIGN_FLAG_FOLDER_VIEW			'F'	/*	VIEW: This is a V4 folder view. */
#define DESIGN_FLAG_V4AGENT 			'f'	/*	FILTER: This is a V4 agent */
#define DESIGN_FLAG_VIEWMAP 			'G'	/*	VIEW: This is ViewMap/GraphicView/Navigator */
#define DESIGN_FLAG_OTHER_DLG			'H'	/*	ALL: Indicates a form that is placed in Other... dialog */
#define DESIGN_FLAG_V4PASTE_AGENT		'I'	/*	FILTER: This is a V4 paste agent */
#define DESIGN_FLAG_LOTUSSCRIPT_AGENT   'L' /*  FILTER: If its LOTUSSCRIPT */
#define	DESIGN_FLAG_QUERY_MACRO_FILTER	'M'	/*	FILTER: Stored FT query AND macro */
#define DESIGN_FLAG_NEW					'N'	/*  FORM: Indicates that a subform is listed when making a new form.*/
#define	DESIGN_FLAG_QUERY_V4_OBJECT 	'O'	/*	FILTER: Indicates V4 search bar query object - used in addition to 'Q' */
#define DESIGN_FLAG_PRIVATE_STOREDESK   'o' /*  VIEW: If Private_1stUse, store the private view in desktop */
#define	DESIGN_FLAG_PRESERVE 			'P'	/*	ALL: related to data dictionary */
#define DESIGN_FLAG_PRIVATE_1STUSE 		'p'	/* 	VIEW: This is a private copy of a private on first use view. */
#define	DESIGN_FLAG_QUERY_FILTER 		'Q'	/*	FILTER: Indicates full text query ONLY, no filter macro */
#define DESIGN_FLAG_AGENT_SHOWINSEARCH	'q'	/*	FILTER: Search part of this agent should be shown in search bar */
#define DESIGN_FLAG_REPLACE_SPECIAL		'R'	/*	SPECIAL: this flag is the opposite of DESIGN_FLAG_PRESERVE, used
												only for the 'About' and 'Using' notes + the icon bitmap in the icon note */
#define DESIGN_FLAG_V4BACKGROUND_MACRO 	'S'	/*	FILTER: This is a V4 background agent */
#define DESIGN_FLAG_SCRIPTLIB			's'	/*	FILTER: A database global script library note */
#define DESIGN_FLAG_VIEW_CATEGORIZED 	'T'	/* 	VIEW: Indicates a view that is categorized on the categories field */
#define DESIGN_FLAG_DATABASESCRIPT		't'	/*	FILTER: A database script note */
#define DESIGN_FLAG_SUBFORM 			'U'	/*	FORM: Indicates that a form is a subform.*/
#define DESIGN_FLAG_PRIVATE_IN_DB 		'V'	/* 	ALL: This is a private element stored in the database */
#define DESIGN_FLAG_V4AGENT_DATA		'X' /*  FILTER: This is a V4 agent data note */
#define DESIGN_FLAG_SUBFORM_NORENDER	'x'	/*	SUBFORM: indicates whether
												we should render a subform in
												the parent form					*/
#define DESIGN_FLAG_NO_MENU 			'Y'	/*	ALL: Indicates that folder/view/etc. should be hidden from menu. */
#define DESIGN_FLAG_MULTILINGUAL_PRESERVE_HIDDEN 'Z' /* ALL: Used to indicate design element was hidden */
											/*	before the 'Notes Global Designer' modified it. */
											/*	(used with the "!" flag) */
#define DESIGN_FLAG_MULTILINGUAL_ELEMENT '!'/*	ALL: Indicates this design element supports the */
											/*	'Notes Global Designer' multilingual addin */
#define DESIGN_FLAG_HIDE_FROM_V3		'3'	/*	ALL: notes stamped with this flag 
													will be hidden from V3 client */
#define DESIGN_FLAG_HIDE_FROM_V4		'4'	/*	ALL: notes stamped with this flag 
													will be hidden from V4 client */
#define DESIGN_FLAG_HIDE_FROM_V5		'5'	/*	ALL: notes stamped with this flag 
													will be hidden from V5 client */
#define DESIGN_FLAG_HIDE_FROM_V6		'6'	/*	ALL: notes stamped with this flag 
													will be hidden from V6 client */
#define DESIGN_FLAG_HIDE_FROM_V7		'7'	/*	ALL: notes stamped with this flag 
													will be hidden from V7 client */
#define DESIGN_FLAG_HIDE_FROM_V8		'8'	/*	ALL: notes stamped with this flag 
													will be hidden from V8 client */
#define DESIGN_FLAG_HIDE_FROM_V9		'9'	/*	ALL: notes stamped with this flag 
													will be hidden from V9 client */
#define DESIGN_FLAG_MUTILINGUAL_HIDE	'0'	/*	ALL: notes stamped with this flag 
													will be hidden from the client 
													usage is for different language
													versions of the design list to be
													hidden completely				*/


/* '+' = ANY of the flags, '-' = NONE of the flags, '*' = ALL of the flags 	*/
/* '(+-*' = a combination of the above.										*/
/*	 Example:  "(+AB-C*DE" = (A OR B) AND (NOT C) AND (D AND E)				*/
/* 		Note: be sure to have +-* placeholders even if no flags for some.	*/
/* 			  ie: "(+-C*DE"   = (NOT C) AND (D AND E).						*/
/*	 Note: "(+-Q*" is equivalent to "-Q" 									*/

#define	DFLAGPAT_V4SEARCHBAR "(+Qq-Bst5*"	/* display things editable at V4 search bar; version filtering */
#define	DFLAGPAT_SEARCHBAR "+QM-st5*" 		/* display things editable at search bar; version filtering */

#define DFLAGPAT_VIEWFORM "-FQMUGX40"  		/* display things editable with dialog box; version filtering */
#define DFLAGPAT_VIEWFORM_ALL_VERSIONS "-FQMUGX" /* display things editable with dialog box; no version filtering (for design) */

#define	DFLAGPAT_TOOLSRUNMACRO "-QXMBESIst5"	/* display things that are runnable; version filtering */
#define DFLAGPAT_AGENTSLIST	"-QXst"			/* display things that show up in agents list. No version filtering (for design) */
#define DFLAGPAT_PASTEAGENTS	"+I"		/* select only paste agents */
#define DFLAGPAT_SCRIPTLIB		 "+s"		/* display only database global script libraries */
#define DFLAGPAT_DATABASESCRIPT "+t"		/* display only database level script */

#define DFLAGPAT_SUBFORM "(+U-40*"				/* display only subforms; version filtering	*/

#define	DFLAGPAT_DBRUNMACRO "+BS"			/* run all background filters */
#define DFLAGPAT_COMPOSE "-C40"				/* display forms that belong in compose menu; version filtering */
#define DFLAGPAT_QUERYBYFORM "-D40"			/* display forms that appear in query by form; version filtering */
#define	DFLAGPAT_PRESERVE "+P"				/* related to data dictionary; no version filtering */
#define DFLAGPAT_SUBADD	"(+-40*UA"			/* subforms in the add subform list; no version filtering */
#define DFLAGPAT_SUBNEW "(+-40*UN"			/* subforms that are listed when making a new form.*/
#define DFLAGPAT_VIEW "-FG40"				/* display only views */
#define DFLAGPAT_NOTHIDDEN "-40"			/* design element is not hidden*/
#define DFLAGPAT_FOLDER "(+-04*F"			/* display only folders; version filtering */
#define DFLAGPAT_FOLDER_ALL_VERSIONS "*F"/* display only folders; no version filtering (for design) */
#define DFLAGPAT_CALENDAR "*c"				/* display only calendar-style views */

#define DFLAGPAT_VIEWS_AND_FOLDERS "-G40"	/* display only views and folder; version filtering */

#define DFLAGPAT_VIEWMAP "(+-04*G"			/* display only GraphicViews; version filtering */
#define DFLAGPAT_VIEWMAP_ALL_VERSIONS "*G"	/* display only GraphicViews; no version filtering (for design) */

#define DFLAGPAT_OTHER_DLG "(+-04*H"			/* display forms that belong in compose menu */
#define DFLAGPAT_CATEGORIZED_VIEW "(+-04*T"	/* display only categorized views */

#define DFLAGPAT_DEFAULT_DESIGN "+d"		/* detect default design note for it's class (used for VIEW) */

/* Compute-defined item names that are actually pseudo-items, requested
   by NIF, that cause Compute to return some info.  These are not actually
   in any document, and if they are, will probably not work properly. */

#define	FIELD_COMPUTE_READERS "$C1$"

/* Mail note item names */

#define MAIL_MESSAGETYPE_ITEM "MessageType"		/* Message type. */
#define MAIL_MESSAGE_TYPE_ITEM "$MessageType"	/* $MessageType item name */
#define MAIL_SENDTO_ITEM "SendTo"				/* SendTo item name */
#define MAIL_COPYTO_ITEM "CopyTo"				/* CopyTo item name */
#define MAIL_REPLYTO_ITEM		"ReplyTo"		/* ReplyTo, used for agents */
#define MAIL_RESPONSETO_ITEM	"ResponseTo"	/* ResponseTo, used for agents */
#define MAIL_FROM_ITEM "From"					/* From item name */
#define MAIL_FROMDOMAIN_ITEM "FromDomain"		/* From domain item name */
#define MAIL_SUBJECT_ITEM "Subject"				/* Subject item name */
#define MAIL_DISPLAY_SUBJECT_ITEM "DisplaySubject"	/* Used by reply form */
#define MAIL_COMPOSEDDATE_ITEM "ComposedDate"	/* Composed date item name */
#define MAIL_POSTEDDATE_ITEM "PostedDate"		/* Posted date item name */
#define MAIL_BODY_ITEM "Body"					/* Body item name */
#define MAIL_INTENDEDRECIPIENT_ITEM "IntendedRecipient" /* Intended recipient item */
#define MAIL_FAILUREREASON_ITEM "FailureReason"		/* Failure reason item */
#define MAIL_DEADFAILUREREASON_ITEM "DeadFailureReason"		/* DEAD Failure reason item */
#define MAIL_RECIPIENTS_ITEM "Recipients"		/* Recipients list item */
#define MAIL_ROUTINGSTATE_ITEM "RoutingState"	/* Routing state */
#define MAIL_SAVED_FORM_ITEM "MailSavedForm"	/* Delivery report saved form name item */
#define MAIL_BLINDCOPYTO_ITEM "BlindCopyTo"		/* Blind copy to item name */
#define MAIL_DELIVERYPRIORITY_ITEM "DeliveryPriority" /* Delivery priority item name */
#define MAIL_DELIVERYREPORT_ITEM "DeliveryReport"	/* Delivery report request item name */
#define MAIL_DELIVEREDDATE_ITEM "DeliveredDate"	/* Delivered date item name */
#define MAIL_DELIVERYDATE_ITEM "DeliveryDate"	/* Delivery date item name (Confirmation Reports only) */
#define MAIL_FORM_ITEM FIELD_FORM				/* Form name item */
#define MAIL_REPLY_FORM_ITEM	"ReplyForm"		/* Reply form, used for agents */
#define MAIL_CATEGORIES_ITEM "Categories"		/* Categories field */
#define MAIL_FROM_CATEGORIES_ITEM "FromCategories"	/* Sender's Categories field */
#define MAIL_ROUTE_SERVERS_ITEM "RouteServers"	/* List of servers routed thru */
#define MAIL_ROUTE_TIME_ITEM "RouteTimes"		/* List of TIMEDATE_PAIRS describing holding times */
#define MAIL_RECIP_GROUPS_EXP_ITEM "RecipientGroupsExpanded" /* List of recipient group names that have been expanded */
#define MAIL_RETURNRECEIPT_ITEM "ReturnReceipt"	/* Return receipt requested? */
#define MAIL_NAMED_ENCKEY_ITEM "NamedEncryptionKey" /* Named encryption key. */
#define MAIL_ROUTE_HOPS_ITEM "$Hops" 			/* Number of routing hops still allowed. */
#define MAIL_CORRELATION_ITEM "$Correlation" 	/* Arbitrary delivery report correlation value. */
#define MAIL_FORMAT_ITEM "MailFormat"			/* (E)ncapsulated */
												/* (T)ext */
												/* (B)oth */
#define MAIL_IMPORTANCE_ITEM "Importance"		/* Send Priority/Importance item */
#define MAIL_QUALRECIPIENTS_ITEM "QualRecipients"	/* Fully qualified Recipients list item */
#define MAIL_ORIGNALPATH_ITEM "OriginalPath"	/* Original routing path (copy of original message's FromDomain) */
#define MAIL_TRACE_ITEM "$Trace"				/* Identifies message as a trace file */
#define MAIL_ORIG_ITEM	"$Orig"					/* Global Id used for Billing */
#define MAIL_DELIVER_LOOPS_ITEM "$Loops" 		/* Number of forwarding loops still allowed. */

/*	Appointment form item names */

#define MAIL_APPT_BUSYNAME_ITEM 	"$BusyName"		/* Person/resource the appointment is for */
#define MAIL_APPT_STARTTIME_ITEM	"StartDateTime"	/* Start time of appointment */
#define MAIL_APPT_ENDTIME_ITEM		"EndDateTime"	/* End time of appointment */
#define MAIL_APPT_SEQUENCE_ITEM		"$Sequence"		/* Sequence # of updates */
#define MAIL_APPT_BUSYPRIORITY_ITEM "$BusyPriority"  /* Busy/Penciled-in/etc. */
#define MAIL_APPT_ATTENDEE_ITEM				"Attendees"	/* List of people for which the following items apply */
#define MAIL_APPT_ATTENDEE_STATE_ITEM		"ORGState"	/* Chairman, attendee, resource */
#define MAIL_APPT_ATTENDEE_STATUS_ITEM		"ORGStatus"	/* Deleted, etc. */
#define MAIL_APPT_ATTENDEE_OPTIONAL_ITEM	"OPTIONAL"	/* If TRUE, attendee is optional */


/*	Calendar profile form related */

/*	On a profile are eight fields that relate to when a users is available:

	The AvailableDays item corresponds to the checkboxes allowing the user
		to choose which days of the week they work.  The resulting item
		will be a textlist which contains the days of the week that they
		work.
	For the days of the week that the user does work, the time ranges are
		read and for each day and assimilated into one item in the busytime
		database that represents the days and time the user or resource
		is available for scheduling.
	The item is encoded as follows:
		DateTime.Lower.Date	- a day that corresponds to the day of the week
					  .Time - start time of an interval for that day the user works
				.Upper.Date - the same day as the Lower
					  .Time - end time of an interval for that day the user works

		These values will repeat for the various times during the day for
			every day of the week that the user works.

	(i.e.	The default configuration will be: (for Monday through Friday)

		A Monday,	9:00 AM - A Monday, 12:00 PM
		A Monday,	1:00 PM - A Monday, 5:00 PM)

	NOTE:	The encoding does not care which real date is used, it just
			determines what day of the week the date correspondes to..
*/

#define MAIL_CALENDAR_PROFILE_WORK_AVAILABLEDAYS_ITEM	"$AvailableDays"
#define MAIL_CALENDAR_PROFILE_OWNER_ITEM				"Owner"				/* Owner field in calendar profile form */
#define MAIL_CALENDAR_PROFILE_WORK_WEEKDAYPREFIX		"$Times"
#define MAIL_CALENDAR_PROFILE_WORK_WEEKDAYSUFFIX		""
#define MAIL_CALENDAR_PROFILE_WORK_SUNDAY_ITEM			"$Times1"
#define MAIL_CALENDAR_PROFILE_WORK_MONDAY_ITEM			"$Times2"
#define MAIL_CALENDAR_PROFILE_WORK_TUESDAY_ITEM			"$Times3"
#define MAIL_CALENDAR_PROFILE_WORK_WEDNESDAY_ITEM 		"$Times4"
#define MAIL_CALENDAR_PROFILE_WORK_THURSDAY_ITEM		"$Times5"	
#define MAIL_CALENDAR_PROFILE_WORK_FRIDAY_ITEM			"$Times6"
#define MAIL_CALENDAR_PROFILE_WORK_SATURDAY_ITEM		"$Times7"
#define MAIL_CALENDAR_PROFILE_ENABLE_ALARMS_ITEM		"EnableAlarms"

/*	Mail delegation profile form related */

#define MAIL_DELEGATION_PROFILE_OWNER_ITEM			"Owner"				
#define MAIL_DELEGATION_ALLOWBUSYACCESS_ITEM		"AllowBusyAccess"

/* Mail form names */

#define MAIL_MEMO_FORM "Memo"					/* Standard memo */
#define MAIL_REPLY_FORM "Reply"					/* Standard memo reply */
#define MAIL_REPLY_WITH_HISTORY_FORM "Reply With History"	/* Standard reply w/history memo */
#define MAIL_PHONEMESSAGE_FORM "Phone Message"	/* Phone message */
#define MAIL_DELIVERYREPORT_FORM "Delivery Report" /* Delivery report form name */
#define MAIL_NONDELIVERYREPORT_FORM "NonDelivery Report" /* Non-Delivery report form name */
#define MAIL_RETURNRECEIPT_FORM "Return Receipt" /* Return Receipt form name */
#define MAIL_DATABASEENTRY_FORM "Database Entry" /* mailin database entry form for libraries */
#define MAIL_TRACEREPORT_FORM "Trace Report"	 /* Trace report form name */
#define MAIL_CALENDAR_PROFILE_FORM "CalendarProfile"	/* Calendar profile form name */
#define MAIL_DELEGATION_PROFILE_FORM "DelegationProfile" /* Delegation profile form name */
#define MAIL_APPOINTMENT_FORM "Appointment"		/* C & S Appointment form name */
#define MAIL_NOTICE_FORM "Notice"				/* C & S Meeting Notice form name */
#define MAIL_RESOURCE_RESERVATION_FORM "Reservation" /* C & S Resource reservation form name */
#define MAIL_RESOURCE_PROFILE_FORM "Resource"	/* C & S Calendar resource profile form name */

/* Address Book - "Person" form item names */

#define MAIL_PERSON_FORM "Person"				/* Form and type name */
#define MAIL_FORMTYPE_ITEM "Type"				/* Form type */
#define MAIL_FULLNAME_ITEM 	"FullName"			/* Full name  */
#define MAIL_FIRSTNAME_ITEM "FirstName"			/* First name */
#define MAIL_LASTNAME_ITEM 	"LastName"			/* Last name */
#define MAIL_MIDDLEINITIAL_ITEM "MiddleInitial"	/* Middle initial */
#define MAIL_SHORTNAME_ITEM "ShortName"			/* Short name (for mail gateways) */
#define MAIL_OWNER_ITEM 	"Owner"				/* Entry owner name */
#define MAIL_MAILSYSTEM_ITEM "MailSystem"		/* Mail system item (keyword field: 1,2,3,4) */
#define MAIL_MAILSERVER_ITEM "MailServer"		/* Mail server name */
#define MAIL_MAILFILE_ITEM 	"MailFile"			/* Mail file name */
#define MAIL_MAILDOMAIN_ITEM "MailDomain"		/* Mail domain name */
#define MAIL_MAILADDRESS_ITEM "MailAddress"		/* Mail auto-forwarding address */
#define MAIL_PUBLICKEY_ITEM "PublicKey"			/* Public encryption key */
#define MAIL_CERTIFICATE_ITEM "Certificate"		/* Public encryption key */
#define MAIL_LOCATION_ITEM "Location"			/* Location field */
#define MAIL_COMMENT_ITEM "Comment"				/* Comment field */
#define MAIL_USERID_ATTACHMENT "UserID"			/* User ID attachment file name */
#define MAIL_SERVERID_ATTACHMENT "ServerID"		/* Server ID attachment file name */
#define MAIL_CALENDARDOMAIN_ITEM "CalendarDomain" /* User's calendar domain override */
#define MAIL_NETUSERNAME_ITEM "NetUserName"		 /* Network Acct Name (for dir synching) */

/* Address Book - Mail-in "Database" form item names.  Uses Person form
	fields: FullName, MailServer, MailFile, and MailDomain" */

#define MAIL_MAILINDATABASE_FORM "Database"		/* Form and type name */
#define MAIL_DESCRIPTION_ITEM 	"Description"	/* Mail-in db description item */

/* MAPI specific items */

#define MAIL_MAPI_MSGFLAGS_ITEM "$MapiMessageFlags" /* MAPI IMessage PR_MESSAGE_FLAGS Property */
#define MAIL_MAPI_MSGCLASS_ITEM "$MapiMessageClass" /* MAPI IMessage PR_MESSAGE_CLASS Property */
#define MAIL_MAPI_STATUS_ITEM "$MapiStatus" 		/* MAPI PR_MSG_STATUS, PR_STATUS Properties */
#define MAIL_MAPI_ACCESS_ITEM "$MapiAccess" 		/* MAPI PR_ACCESS, PR_ACCESS_LEVEL Properties */
#define MAIL_MAPI_TO_ADDRTYPE_ITEM "$MapiSendToAddrType"/* MAPI Recip PR_ADDRTYPE Property for To List */
#define MAIL_MAPI_CC_ADDRTYPE_ITEM "$MapiSendCcAddrType"/* MAPI Recip PR_ADDRTYPE Property for Cc List*/
#define MAIL_MAPI_BCC_ADDRTYPE_ITEM "$MapiSendBccAddrType"/* MAPI Recip PR_ADDRTYPE Property for Bcc List*/
#define MAIL_MAPI_TO_RESP_ITEM "$MapiSendToResponsibility"/* MAPI Recip PR_RESPONSIBILITY Property for To List */
#define MAIL_MAPI_CC_RESP_ITEM "$MapiSendCcResponsibility"/* MAPI Recip PR_RESPONSIBILITY Property for Cc List */
#define MAIL_MAPI_BCC_RESP_ITEM "$MapiSendBccResponsibility"/* MAPI Recip PR_RESPONSIBILITY Property for Bcc List */
#define MAIL_MAPI_TO_EID_ITEM "$MapiSendToEID"		/* MAPI Recip PR_ENTRYID Property for To List */
#define MAIL_MAPI_CC_EID_ITEM "$MapiSendCcEID"		/* MAPI Recip PR_ENTRYID Property for Cc List */
#define MAIL_MAPI_BCC_EID_ITEM "$MapiSendBccEID"	/* MAPI Recip PR_ENTRYID Property for Bcc List */
#define MAIL_MAPI_TO_OT_ITEM "$MapiSendToObjType"	/* MAPI Recip PR_OBJECT_TYPE Property for To List */
#define MAIL_MAPI_CC_OT_ITEM "$MapiSendCcObjType"	/* MAPI Recip PR_OBJECT_TYPE Property for Cc List */
#define MAIL_MAPI_BCC_OT_ITEM "$MapiSendBccObjType" /* MAPI Recip PR_OBJECT_TYPE Property for Bcc List */
#define MAIL_MAPI_TO_ADDR_ITEM "$MapiSendToAddr" 	/* MAPI Recip PR_EMAIL_ADDRESS Property for To List */
#define MAIL_MAPI_CC_ADDR_ITEM "$MapiSendCcAddr" 	/* MAPI Recip PR_EMAIL_ADDRESS Property for Cc List */
#define MAIL_MAPI_BCC_ADDR_ITEM "$MapiSendBccAddr" 	/* MAPI Recip PR_EMAIL_ADDRESS Property for Bcc List */
#define MAIL_MAPI_SENDER_EID_ITEM "$MapiSenderEID"	/* MAPI Sender PR_ENTRYID Property */
#define MAIL_MAPI_SENDER_ADDRTYPE_ITEM "$MapiSenderAddrType"   /* MAPI Sender PR_ADDRTYPE Property */
#define MAIL_MAPI_SENDER_ADDR_ITEM "$MapiSenderAddr"/* MAPI Sender PR_EMAIL_ADDR Property */
#define MAIL_MAPI_SENDER_SEARCHKEY_ITEM "$MapiSenderSearchKey" /* MAPI Sender PR_SEARCH_KEY Property */
#define MAIL_MAPI_SENDER_NAME_ITEM "$MapiSenderName"/* MAPI Sender PR_DISPLAY_NAME Property */

/*	Client Type  */

#define FIELD_CLIENTTYPE "ClientType"			/* Client Type - Full or Limited Notes */

/*	Standard Subform Item Name.	*/

#define SUBFORM_ITEM_NAME "$SubForms"

/*	Define field name common to Server, Person, and Certifier forms */

#define KFM_ITEM_CHANGE_REQUEST	"ChangeRequest"	

/*	Certifier and Cross-certificate Address Book lookup definitions */


#define	KFM_FORM_CERTIFIER			"Certifier"
#define	KFM_FORM_CROSSCERTIFICATE	"CrossCertificate"

#define KFM_ITEM_NAMECOMBO		"NameCombo"
#define KFM_ITEM_ORGCOMBO		"OrgCombo"
#define KFM_ITEM_ISSUEDTO		"IssuedTo"
#define KFM_ITEM_ISSUEDBY		"IssuedBy"
#define KFM_ITEM_CERTIFICATE	"Certificate"
												/* Some additional items are */
												/* the same as those defined */
												/* for the "Person" form */

#define KFM_ITEM_CERT_NOENC		"Certificate_NoEnc"	/* Public encryption key */
													/* to be used for signature */
													/* checking only and not for */
													/* sending encrypted mail */

#define KFM_ITEM_PUBLICKEY	 "PublicKey"		/* Public encryption key 	*/
												/* for flat names.			*/

#define CERTIFIERNAMESSPACE         "$Certifiers"
#define CERTIFIERSNAMESSPACE		"$Certifiers"
#define CROSSCERTBYROOTNAMESSPACE	"$CrossCertByRoot"
#define CROSSCERTBYNAMENAMESSPACE	"$CrossCertByName"

#define	KFM_CROSSCERT_ITEM_ISSUEDTO		0
#define	KFM_CROSSCERT_ITEM_ISSUEDBY		1
#define	KFM_CROSSCERT_ITEM_CERTIFICATE	2
#define	KFM_CROSSCERT_ITEM_CERT_NOENC	3
#define KFM_CROSSCERT_ITEM_NOTEID		4
#define	KFM_CROSSCERT_LOOKUPITEMCOUNT	5
#define	KFM_CROSSCERT_LOOKUPITEMS "IssuedTo\0IssuedBy\0Certificate\0Certificate_NoEnc\0$$NoteID"

#define	KFM_CERTIFIER_ITEM_ISSUEDTO		0
#define	KFM_CERTIFIER_ITEM_ISSUEDBY		1
#define	KFM_CERTIFIER_ITEM_CERTIFICATE	2
#define	KFM_CERTIFIER_ITEM_CERT_NOENC	3
#define	KFM_CERTIFIER_LOOKUPITEMCOUNT	4
#define	KFM_CERTIFIER_LOOKUPITEMS "IssuedTo\0IssuedBy\0Certificate\0Certificate_NoEnc"

/*	Lotus Notes Template Developer ID name.
 */
#define NOTES_TEMPLATE_DEVELOPER_ID "CN=Lotus Notes Template Development/O=Lotus Notes"


/*	Lookup items for Network Authentication check.
*/
#define	NETAUTH_ITEM_CERTIFICATE	0
#define	NETAUTH_ITEM_CERT_NOENC		1
#define	NETAUTH_ITEM_PUBLIC_KEY		2
#define	NETAUTH_ITEM_CHANGEREQUEST	3
#define	NETAUTH_LOOKUPITEMCOUNT		4
#define NETAUTH_LOOKUPITEMS			"Certificate\0Certificate_NoEnc\0PublicKey\0ChangeRequest"

#define NETAUTH_NAMESPACE_USERS			"($Users)"
#define	NETAUTH_NAMESPACE_SERVERS		"($Servers)"
#define	NETAUTH_NAMESPACE_CERTIFIERS	"($Certifiers)"
#define NETAUTH_NAMESPACE_XCERTBYROOT	"($CrossCertByRoot)"

/*  Lookup items for KFMGetPublicKey
*
*	NOTE: Code knows that the last view may not be present (in the case
*	of a Personal N&A book), so $Servers must be last and the code must
*	be changed if any others become optional.
*/

#define KFM_GETPUB_VIEWS		"($Users)\0($Certifiers)\0($Servers)"
#define KFM_GETPUB_NUMVIEWS		3
#define KFM_GETPUB_FIELDS		"Certificate\0Certificate_NoEnc\0PublicKey"
#define KFM_GETPUB_NUMFIELDS	3

/*  Lookup items for Profiles */

#define PROFILENAMESSPACE	"$Profiles"
#define PROFILENAME     "ProfileName"
#define PROFILES_ITEM "Profiles"

/* Address Book - "Server" form item names */

#define MAIL_SERVER_FORM "Server"				/* Form and type name */
#define MAIL_SERVERNAME_ITEM "ServerName"		/* Server name */
#define MAIL_DOMAINNAME_ITEM "Domain"			/* Domain server is in */
#define MAIL_NETWORKNAME_ITEM "Network"			/* Network server is on */
#define MAIL_ADMINNAME_ITEM "Administrator" 	/* Server's administrator(s) */
#define MAIL_SERVERTITLE_ITEM "ServerTitle"		/* Server title */
#define MAIL_SERVERPORTS_ITEM "Ports"			/* Enabled ports in server record */

/* Address Book - "X400Server" form item names */

#define MAIL_X400_SERVER_FORM "Server"				/* Form and type name */
#define MAIL_X400_SERVERNAME_ITEM "X400ServerName"		/* X400 MTA server name */
#define MAIL_X400_MTANAME_ITEM "X400MTAName"			/* X400 MTA name */
#define MAIL_X400_GLOBAL_DOMAIN_ITEM "X400GlobalDomain" /* Global domain identifier */ 
#define MAIL_X400_COUNTRY_ITEM "X400Country"			/* Country name */
#define MAIL_X400_ADMD_ITEM "X400ADMD"					/* Administration Management Domain name*/
#define MAIL_X400_PRMD_ITEM "X400Domain"				/* Private Domain name */
#define MAIL_X400_CREDENTIALS_ITEM "X400Credentials"	/* MTA credentials */
#define MAIL_X400_SUPP_APP_CONTEXTS_ITEM "X400SupportedApplicationContexts"	/* Supported app. contexts */
#define MAIL_X400_DEL_CONTENT_LENGTH_ITEM "X400DeliverContentLength"	/* Delivery content length */
#define MAIL_X400_DEL_CONTENT_TYPE_ITEM "X400DeliverContentTypes"	 	/* Delivery content types */
#define MAIL_X400_PSEL_ITEM "X400PSEL"					/* Presentation selector */
#define MAIL_X400_SSEL_ITEM "X400SSEL"					/* Session selector */
#define MAIL_X400_TSEL_ITEM "X400TSEL"					/* Transport selector */
#define MAIL_X400_IPADDRESS_ITEM "X400IPAddress" 		/* Network IP address for the MTA */
#define MAIL_X400_X121ADDRESS_ITEM "X400X121Address" 	/* X.121 Network address for the MTA */

/* Address Book - "Domain" form item names */

#define MAIL_DOMAINTYPE_ITEM "DomainType"					/* Domain type */
#define MAIL_DOMAINADJDOMAINNAME_ITEM "AdjacentDomainName" 	/* Adjacent domain name */
#define MAIL_DOMAINDOMAINNAME_ITEM "DomainName"				/* Domain name */
#define MAIL_DOMAINNEXTDOMAIN_ITEM "NextDomain"				/* Next domain to domain */
#define MAIL_DOMAINMAILSERVER_ITEM "MailServer"				/* Server containing mailbox */
#define MAIL_DOMAINMAILFILE_ITEM "MailFile"					/* File name of mailbox */
#define	MAIL_DOMAINALLOW_ITEM "AllowFromDomains"			/* Allow list */
#define	MAIL_DOMAINDENY_ITEM "DenyFromDomains"				/* Deny list */
#define MAIL_DOMAINCALENDARSERVER_ITEM "CalendarServer"		/* Calendar gateway server */
#define MAIL_DOMAINCALENDARSYSTEM_ITEM "CalendarSystem"		/* Calendar gateway system */

/* Address Book - "Connection" form item names */

#define MAIL_LOCAL_CONNECTION_FORM "Local"		/* Local Connection form name */
#define MAIL_REMOTE_CONNECTION_FORM "Remote"	/* Remote Connection form name */
#define MAIL_CONNECTION_TYPE "Connection"		/* Connection entry type */
#define MAIL_CONNECTION_TYPE_ITEM "ConnectionType" /* Connection type item */
#define MAIL_SOURCE_ITEM "Source"				/* Source computer name */
#define MAIL_SOURCEDOMAIN_ITEM "SourceDomain"	/* Source domain name */
#define MAIL_DESTINATION_ITEM "Destination"		/* Destination computer name */
#define MAIL_DESTINATIONDOMAIN_ITEM "DestinationDomain"	/* Destination domain name */
#define MAIL_PORTNAME_ITEM "PortName"			/* Port name */
#define MAIL_PHONENUMBER_ITEM "PhoneNumber" 	/* Phone number */
#define MAIL_COUNTRYCODE_ITEM "Country"		 	/* Phone number */
#define MAIL_AREACODE_ITEM "AreaCode"		 	/* Phone number */
#define MAIL_ENABLED_ITEM "Enabled"				/* Enabled/disabled item */
#define MAIL_TASKS_ITEM "Tasks"					/* Tasks item */
#define MAIL_INTERVAL_ITEM "Interval"			/* Interval item */
#define MAIL_SCHEDULE_ITEM "Schedule"			/* Schedule item */
#define MAIL_COST_ITEM "Cost"					/* Routing cost item */
#define MAIL_REPLICATION_TYPE_ITEM "RepType"	/* Replication type, e.g. pull/pull */
#define MAIL_LINKINFO_ITEM "LinkInfo"			/* For X.25 conn recs */
#define MAIL_PASSTHRUSERVER_ITEM "PassthruServer" /* For passthru conn recs */

/* Address Book - "Group" form item names */

#define MAIL_GROUP_FORM "Group"					/* Form and type name */
#define MAIL_LISTNAME_ITEM "ListName"			/* Distribution list name */
#define MAIL_LISTDESCRIPTION_ITEM "ListDescription"	/* Distribution list name */
#define MAIL_MEMBERS_ITEM "Members"				/* Members names name */
#define MAIL_LISTADMINISTRATOR_ITEM "Administrator" /* Group administrator */

/* Address Book - "Users" name space, items, and names */

#define USERNAMESSPACE "$Users"

#define MAIL_NAMELOOKUPITEMCOUNT 8
#define MAIL_NAMELOOKUPSIMPLEITEMCOUNT 6		/* Excludes PublicKey */
#define MAIL_NAMELOOKUPITEMS "FullName\0MailDomain\0MailAddress\0Location\0ListName\0Members\0PublicKey\0Certificate"
#define FULLNAMEITEM	0
#define DOMAINNAMEITEM	1
#define MAILADDRESSITEM	2
#define LOCATIONITEM	3
#define LISTNAMEITEM	4
#define MEMBERSITEM		5
#define PUBLICKEYITEM	6
#define CERTIFICATEITEM	7

#define MAIL_DELIVERYLOOKUPITEMCOUNT 10
#define MAIL_DELIVERYSIMPLEITEMCOUNT 6			/* Excludes PublicKey */
#define MAIL_DELIVERYLOOKUPITEMS "FullName\0MailAddress\0MailServer\0MailFile\0ShortName\0ListName\0PublicKey\0EncryptIncomingMail\0MailDomain\0Certificate"
#define MAIL_DELIVERYFULLNAMEITEM	 0
#define MAIL_DELIVERYMAILADDRESSITEM 1
#define MAIL_DELIVERYMAILSERVERITEM	 2
#define MAIL_DELIVERYMAILFILEITEM	 3
#define MAIL_DELIVERYSHORTNAMEITEM	 4
#define MAIL_DELIVERYLISTNAMEITEM	 5
#define MAIL_DELIVERYPUBLICINFO		 6
#define MAIL_DELIVERYENCRYPTINCOMING 7
#define MAIL_DELIVERYMAILDOMAINITEM	 8
#define MAIL_DELIVERYCERTIFICATEITEM 9


/* Address Book - "Domains" name space, items, and names */

#define DOMAINNAMESSPACE "$Domains"

#define MAIL_DOMAINLOOKUPITEMCOUNT 4
#define MAIL_DOMAINLOOKUPITEMS "DomainName\0NextDomain\0MailServer\0MailFile"
#define MAIL_DOMAINNAMEITEM 0
#define MAIL_DOMAINNEXTDOMAINITEM 1
#define MAIL_DOMAINMAILSERVERITEM 2
#define MAIL_DOMAINMAILFILEITEM 3

/* Address Book - "Servers" name space, items, and names */

#define SERVERNAMESSPACE "$Servers"
#define SERVERNAMESSPACE_1 "1\\$Servers"

#define MAIL_SERVERLOOKUPITEMCOUNT 3
#define MAIL_SERVERLOOKUPITEMS "ServerName\0Domain\0Network"
#define MAIL_SERVERNAMEITEM 0
#define MAIL_SERVERDOMAINITEM 1
#define MAIL_SERVERNETWORKITEM 2

/* Address Book - "$People" name space, items, and names */

#define MAIL_PEOPLENAMESPACE "$People"

#define MAIL_PEOPLELOOKUPITEMCOUNT 1
#define MAIL_PEOPLELOOKUPITEMS "FullName"
#define MAIL_PEOPLEFULLNAMEITEM 0

/* Address Book - "$Groups" name space, items, and names */

#define MAIL_GROUPSNAMESPACE "$Groups"

#define MAIL_GROUPSLOOKUPITEMCOUNT 1
#define MAIL_GROUPSLOOKUPITEMS "ListName"
#define MAIL_GROUPSGROUPNAMEITEM 0

/* Address Book - "$NameFieldLookup" name space, items, and names */

#define MAIL_NAMEFIELD_LOOKUP	"$NamesFieldLookup"

/* Address Book - Name field composite lookup */

#define MAIL_NAMEFIELD_COMPOSITE_NAMESSPACE	"$Users\0$Groups\0$ServersLookup"
#define NAMEFIELDCOMPOSITENAMESPACES 3

/* Address Book - Server lookup */

#define MAIL_SERVERSLOOKUP_NAMESSPACE	"$ServersLookup"



/*  Address Book - public views displayed by the admin subprogram */

#define ADMIN_GROUPS_VIEW 			"Groups"
#define ADMIN_PEOPLE_VIEW 			"People"
#define ADMIN_SERVERS_VIEW			"Servers"
#define ADMIN_CONFIGURATIONS_VIEW 	"Configurations"

/* Databases opened and displayed by the admin subprogram */

#define ADMIN_LOG_FILE				"log.nsf"
#define ADMIN_CATALOG_FILE			"catalog.nsf"
#define ADMIN_EVENTS_FILE			"statrep.nsf"
#define ADMIN_EVENT_CONFIG_FILE		"events4.nsf"
#define ADMIN_CERT_LOG_FILE			"certlog.nsf"

/*	Database containing UNAME database directory */

#define	UNAME_FILE					"unames.nsf"
	
/*  Address Book - "ServerAccess" namespace, items, and names */

#define ACCESS_GROUPS_NAMESPACE		"$ServerAccess"
#define ACCESS_GROUPS_NAMESPACE_1	"1\\$ServerAccess"

#define ACCESS_ITEM_LISTNAME		0
#define ACCESS_NUM_LOOKUP_ITEMS		1
#define ACCESS_LISTNAME_ITEM	"ListName"

/*  Address Book - $Users name space, to look up server access */

#define ACCESS_ITEM_COUNT	1
#define ACCESS_LOOKUP_ITEMS	"FullName"
#define ACCESS_ITEM_FULLNAME	0

/*  Address Book -- $Adminp name space, for admin proxy agent */
#define ADMINP_NAMESPACE "$Adminp"

/*  Address Book configuration parameters for the administration process */
#define ADMINP_CONFIG_MAX_THREADS	"AdminPMaxThreads" /* number */
#define ADMINP_CONFIG_INTERVAL	 	"AdminPInterval"   /* number */
#define ADMINP_CONFIG_DAILY_TIME	"AdminPDailyTime"  /* time   */
#define ADMINP_CONFIG_WEEK_DAYS		"AdminPWeekDays"   /* text list */
#define ADMINP_CONFIG_DELAYED_TIME  "AdminPDelayedTime"/* time */
#define ADMINP_CONFIG_DEL_MAIL_FILE "AdminPMailFileDelInt" /* number */

#define ADMINP_LOCAL_ADMIN			"LocalAdmin"
#define ADMINP_ALLOW_ACCESS			"AllowAccess"
#define ADMINP_CREATE_ACCESS		"CreateAccess"
#define ADMINP_REPLICA_ACCESS		"ReplicaAccess"
#define ADMINP_PT_ACCESS			"PTAccess"
#define ADMINP_PT_CLIENTS			"PTClients"
#define ADMINP_PT_CALLERS			"PTCallers"
#define ADMINP_PT_TARGETS			"PTTargets"
#define ADMINP_LIST_OWNER			"ListOwner"
#define ADMINP_PASSTHRU_SERVER		"PassthruServer"
#define ADMINP_SMPT_ADMINISTRATOR	"SMPTAdministrator"
#define ADMINP_GROUP_TYPE			"GroupType"
#define ADMINP_PRIVATE_LIST     	"PrivateList"
#define ADMINP_RESTRICTED_LIST		"RestrictedList"
#define ADMINP_UNRESTRICTED_LIST	"UnrestrictedList"


/* values for resource database */
#define ADMINP_RESOURCE_DOCUMENT	"Resource"
#define ADMINP_RESOURCE_NAME_ITEM	"ResourceName"

/* known values of fields in the proxy database document form */
#define AdminpDelete 							"0"
#define AdminpRenameInTheACL					"1"
#define AdminpCopyPublicKey 					"2"
#define AdminpStoreServerVersion				"3"
#define AdminpRenameServerInNAB 				"4"
#define AdminpRenameUserInNAB 					"5"
#define AdminpMoveUserInHier 					"6"
#define AdminpDeleteStats						"7"
#define AdminpInitiateNABChange					"8"
#define AdminpRecertServerInNAB					"9"
#define AdminpRecertUserInNAB					"10"
#define AdminpServerClusterAdd					"11"
#define AdminpServerClusterRemove				"12"
#define AdminpCreateReplicas					"13"
#define AdminpMoveReplicas						"14"
#define AdminpPendedDeleteForMove				"15"
#define AdminpDeleteInPersonDocs				"16"
#define AdminpDeleteInTheACL					"17"
#define AdminpDeleteInReadersAuthors			"18"
#define AdminpRenameInPersonDocs				"19"
#define AdminpRenameInReadersAuthors			"20"
#define AdminpDeleteMailFile					"21"
#define AdminpApproveMailFileInfo				"22"
#define AdminpDeleteUnlinkedMailFile			"23"
#define AdminpCreateMailFile					"24"
#define AdminpMonitorMovedReplica				"25"
#define AdminpDeleteChangeRequests				"26"
#define AdminpGetMailFileInfo					"27"
#define AdminpRequestDeleteMailFile				"28"
#define AdminpResourceAdd						"29"
#define AdminpResourceDelete					"30"
#define AdminpApproveResourceDelete				"31"
#define AdminpCreateReplicasCheckAccess			"32"
#define AdminpMoveReplicasCheckAccess			"33"
#define AdminpSetPasswordFields					"34"
#define AdminpUpdateUserPW						"35"
#define AdminpUpdateServerPW					"36"
#define AdminpSetMABField						"37"
	
#define AdminpDeleteWord 			 			0
#define AdminpRenameInTheACLWord 	 			1
#define AdminpCopyPublicKeyWord 	 			2
#define AdminpStoreServerVersionWord 			3
#define AdminpRenameServerInNABWord  			4
#define AdminpRenameUserInNABWord	 			5
#define AdminpMoveUserInHierWord 	 			6
#define AdminpDeleteStatsWord		 			7
#define AdminpInitiateNABChangeWord	 			8
#define AdminpRecertServerInNABWord	 			9
#define AdminpRecertUserInNABWord	 			10
#define AdminpServerClusterAddWord				11
#define AdminpServerClusterRemoveWord			12
#define AdminpCreateReplicasWord				13
#define AdminpMoveReplicasWord					14
#define AdminpPendedDeleteForMoveWord			15
#define AdminpDeleteInPersonDocsWord			16
#define AdminpDeleteInTheACLWord				17
#define AdminpDeleteInReadersAuthorsWord		18
#define AdminpRenameInPersonDocsWord			19
#define AdminpRenameInReadersAuthorsWord		20
#define AdminpDeleteMailFileWord				21
#define AdminpApproveMailFileInfoWord			22
#define AdminpDeleteUnlinkedMailFileWord		23
#define AdminpCreateMailFileWord				24
#define AdminpMonitorMovedReplicaWord			25
#define AdminpDeleteChangeRequestsWord			26
#define AdminpGetMailFileInfoWord				27
#define AdminpRequestDeleteMailFileWord			28
#define AdminpResourceAddWord					29
#define AdminpResourceDeleteWord				30
#define AdminpApproveResourceDeleteWord			31
#define AdminpCreateReplicasCheckAccessWord		32
#define AdminpMoveReplicasCheckAccessWord		33
#define AdminpSetPasswordFieldsWord				34
#define AdminpUpdateUserPWWord					35
#define AdminpUpdateServerPWWord				36
#define AdminpSetMABFieldWord					37

#define ADMINP 									"Adminp"
#define ADMINP_ALL_SERVERS						"*"
#define ADMINP_DELETE_MAIL_FILE_ON_HOME_SERVER  "1"
#define ADMINP_DELETE_MAIL_FILE_REPLICAS		"2"
#define ADMINP_DELETE_ONE_MAIL_FILE_REPLICA		"3"

/* the view we use from the proxy database */
#define ADMINP_MAIN_VIEW 						"All Requests by Action"
#define ADMINP_RESPONSES_VIEW					"($LoggedEvents)"
#define ADMINP_REQUESTS_VIEW					"($Requests)"
#define ADMINP_RENAME_IN_ACL_DELETE_VIEW		"($RenameInACLAndDeleteRequests)"

/* fields in the proxy database document form */

#define ADMINP_PROXY_DOCUMENT 					"AdminRequest"
#define ADMINP_DOC_SERVER_ITEM					"ProxyServer"
#define ADMINP_ACTION_ITEM						"ProxyAction"
#define ADMINP_DOC_SERVER_BUILD_NUMBER_ITEM		"ProxyServerBuildNumber"
#define ADMINP_DOC_SOURCE_SERVER_ITEM			"ProxySourceServer"
#define ADMINP_DOC_DEST_SERVER_ITEM				"ProxyDestinationServer"
#define ADMINP_DOC_CLUSTER_NAME_ITEM			"ProxyClusterName"
#define ADMINP_NAME_LIST_ITEM 					"ProxyNameList"
#define ADMINP_DOC_AUTHOR_ITEM 	  				"ProxyAuthor"
#define ADMINP_DOC_PROCESS_ITEM					"ProxyProcess"
#define ADMINP_DOC_TARGET_CERTIFIER_ITEM		"ProxyTargetCertifier"
#define ADMINP_DOC_CERTIFICATE_ITEM 			"ProxyCertificate"
#define ADMINP_DOC_CHANGE_REQUEST_ITEM			"ProxyChangeRequest"
#define ADMINP_DOC_CHANGE_SIGNATURE_ITEM		"ProxyChangeSignature"
#define ADMINP_DOC_REPLICA_ID_ITEM				"ProxyReplicaId"
#define ADMINP_DOC_DATABASE_PATH_ITEM			"ProxyDatabasePath"
#define ADMINP_DOC_DATABASE_SOURCE_PATH_ITEM	"ProxyDatabaseSourcePath"
#define ADMINP_DOC_DATABASE_NAME_ITEM			"ProxyDatabaseName"
#define ADMINP_DOC_COMMENTS_ITEM				"ProxyRequestComments"
#define ADMINP_DOC_DELETE_MAIL_FILE_ITEM		"ProxyDeleteMailfile"
#define ADMINP_DOC_HOME_SERVER_ITEM				"ProxyHomeServer"
#define ADMINP_DOC_EXECUTION_TIME_ITEM			"ProxyExecutionTime"
#define ADMINP_DOC_MAIL_SERVER_ITEM				"ProxySourceServer"
#define ADMINP_DOC_DATABASE_ACL_ITEM			"ProxyDatabaseACL"
#define ADMINP_DOC_DATABASE_CLASS_ITEM			"ProxyDatabaseTemplateName"
#define ADMINP_DOC_RESOURCE_SITE_ITEM			"ProxyResourceSite"
#define ADMINP_DOC_RESOURCE_TYPE_ITEM			"ProxyResourceType"
#define ADMINP_DOC_RESOURCE_CAPACITY_ITEM		"ProxyResourceCapacity"
#define ADMINP_DOC_RESOURCE_DESCRIPTION_ITEM	"ProxyResourceDescription"
#define ADMINP_DOC_RESOURCE_DOMAIN_ITEM			"ProxyResourceDomain"
#define ADMINP_DOC_PASS_CHECK_PASSWORD_ITEM		"ProxyPasswordCheck"
#define ADMINP_DOC_PASS_GRACE_PERIOD_ITEM		"ProxyPasswordGracePeriod"
#define ADMINP_DOC_PASS_CHANGE_INTERVAL_ITEM	"ProxyPasswordChangeInterval"
#define ADMINP_DOC_APPROVAL_FLAG_ITEM			"ApprovalFlag"
#define ADMINP_DOC_IMMEDIATE_FLAG_ITEM			"ProxyImmediateFlag"

/* fields in the proxy database log form */
#define ADMINP_PROXY_LOG 						"AdminLog"
#define ADMINP_LOG_ACTION_REQUESTOR_ITEM 		"ProxyActionRequestor"
#define ADMINP_LOG_SERVER_NAME_ITEM 			"ProxyServerName"	
#define ADMINP_LOG_ACTION_START_TIME_ITEM 		"ActionStartTime"
#define ADMINP_LOG_ACTION_END_TIME_ITEM			"ActionEndTime"
#define ADMINP_LOG_DB_LIST_ITEM					"ProxyDbList"
#define ADMINP_LOG_STATUS_ITEM					"ProxyStatus"
#define ADMINP_LOG_MODIFIED_RESPONSE_ITEM		"ModifiedFlag"
#define ADMINP_LOG_SHOW_MODIFIED_ITEM			"ShowModified"
#define ADMINP_LOG_OLD_NAME_ITEM				"OldName"
#define ADMINP_LOG_NEW_NAME_ITEM				"NewName"
#define ADMINP_LOG_ACTION_COMMENTS_ITEM			"ActionComments"
#define ADMINP_LOG_SHOW_ERROR					"ErrorFlag"
#define ADMINP_LOG_REPLICA_CREATION_TIME		"ReplicaCreationTime"
#define ADMINP_LOG_CREATED_DELETE_REQUEST		"DeleteRequestCreated"
#define ADMINP_LOG_SIGNED_NOTES					"ProxySignedDocs"
#define ADMINP_LOG_IN_PROGRESS					"AdminPInProgress"

/* name and address book fields and field values adminp needs */ 
#define ADMINP_NAB_SERVER_BUILD_NUMBER 			"ServerBuildNumber"
#define ADMINP_NAB_DENY_ACCESS_GROUP 			"3"

#define ADMINP_NAB_PASS_CHECK_PASSWORD			"CheckPassword"
#define ADMINP_NAB_PASS_GRACE_PERIOD			"PasswordGracePeriod"
#define ADMINP_NAB_PASS_CHANGE_INTERVAL			"PasswordChangeInterval"

#define ADMINP_NAB_MASTER_ADDRESS_BOOK			"MasterAddressBook"

#define ADMINP_SERVER_CONNECTION_FORM			"Server\\Connection"
#define ADMINP_NETWORK_CONNECTION_FORM			"Network Connection"
#define ADMINP_REMOTE_CONNECTION_X25_FORM		"Remote Connection (X25)"

/* address book stuff for resources used by adminp */
#define ADMINP_NAB_RESOURCE_FLAG_ITEM	"ResourceFlag"
#define ADMINP_NAB_DOCUMENT_ACCESS_ITEM "DocumentAccess"
#define ADMINP_NAB_RESOURCE_CAPACITY_ITEM	"ResourceCapacity"
#define ADMINP_NAB_RESOURCE_TYPE_ITEM	"ResourceType"
#define ADMINP_NAB_RESOURCE_DOMAIN_ITEM	"MailDomain"

#define RESOURCES_NAMESPACE 		"$Resources"
#define ROOMS_NAMESPACE				"$Rooms"

#define ADMINP_FLAG_SET				"1"
#define ADMINP_DOCUMENT_ACCESS_SET  "[NetModifier]"
#define ADMINP_ROOM					"1" 

/*	Address Book - "Connections" namespace */

#define SOURCE_WILDCARD "*"
#define CONNECTIONSNAMESPACE "$Connections"

#define NAME_CONNECTIONLOOKUPITEMCOUNT	8
#define NAME_CONNECTIONLOOKUPITEMS "Source\0SourceDomain\0Destination\0DestinationDomain\0PortName\0Tasks\0Enabled\0Cost"
#define NAME_CONNECTIONSRCITEM	0
#define NAME_CONNECTIONSRCDOMAINITEM 1
#define NAME_CONNECTIONDSTITEM	2
#define NAME_CONNECTIONDSTDOMAINITEM 3
#define NAME_CONNECTIONPORTITEM	4
#define NAME_CONNECTIONTASKSITEM 5
#define NAME_CONNECTIONENABLEDITEM 6
#define NAME_CONNECTIONCOSTITEM 7

#define DIALNAMELOOKUPITEMCOUNT	5
#define DIALNAMELOOKUPITEMS "Destination\0PhoneNumber\0PortName\0LinkInfo\0LinkScriptInfo\0CreditCardPrefix\0CreditCardSuffix"
#define DESTINATIONNAMEITEM		0
#define PHONENUMBERITEM			1
#define	PORTNAMEITEM			2
#define	LINKINFOITEM			3
#define	LINKSCRIPTITEM			4
#define CARDPREFIXITEM			5
#define CARDSUFFIXITEM			6

#define PT_NAMELOOKUPITEMCOUNT	3	/*	For Passthru */
#define PT_NAMELOOKUPITEMS "Destination\0PassthruServer\0Source"
#define PT_NAMEITEM_DST			0
#define PT_NAMEITEM_VIA			1
#define PT_NAMEITEM_SRC			2

/*	Address Book - "Connections" Application field values */

#define ROUTER_APPNAME	"Mail Routing"		/* Router application name */
#define X400_APPNAME	"X400 Mail Routing"	/* X.400 MTA application name */
#define CCMAIL_APPNAME	"ccMail Routing"	/* ccMail MTA application name */
#define SMTP_APPNAME	"SMTP Mail Routing"	/* SMTP MTA application name */
#define REPL_APPNAME 	"Replication"		/* Replicator application name */
#define RUNPGM_APPLNAME	"Run Program"		/* Run program application name */
#define WKSREP_APPNAME	"WksRep"			/* Workstation Rep/Briefcase application name */
#define WKSREPHI_APPNAME "WksRepHI"			/* High-priority Rep/Briefcase application name */
#define CLREPL_APPNAME	"Cluster Replication" /* Cluster Replicator application name */

/* Connection record items not defined above */

#define CONNREC_USAGE_PRIORITY_ITEM "ConnectionRecordFirst" /* "Normal" use connection record first */
#define CONNREC_OPTIONAL_NET_ADDRESS_ITEM "OptionalNetworkAddress" /* Optional network address field */
#define CONNREC_REMOTE_DTE_ADDRESS_ITEM "RemoteDTEAddress" /* DTE address for X.25 */
#define CONNREC_FCODE1_ITEM "fcode_1" /* X.25 facility code */
#define CONNREC_FCODE2_ITEM "fcode_2" /* X.25 facility code */
#define CONNREC_FCODE3_ITEM "fcode_3" /* X.25 facility code */
#define CONNREC_FCODE4_ITEM "fcode_4" /* X.25 facility code */
#define CONNREC_FVAL1_ITEM "fval_1" /* X.25 facility code value */
#define CONNREC_FVAL2_ITEM "fval_2" /* X.25 facility code value */
#define CONNREC_FVAL3_ITEM "fval_3" /* X.25 facility code value */
#define CONNREC_FVAL4_ITEM "fval_4" /* X.25 facility code value */
#define CONNREC_SCRIPTNAME_ITEM "ScriptName" /* Login script filename */
#define CONNREC_LINKSCRIPT_INFO_ITEM "LinkScriptInfo" /* Login script information */
#define CONNREC_SCRIPTARG1_ITEM "ScriptArg1" /* Login script argument */
#define CONNREC_SCRIPTARG2_ITEM "ScriptArg2" /* Login script argument */
#define CONNREC_SCRIPTARG3_ITEM "ScriptArg3" /* Login script argument */
#define CONNREC_SCRIPTARG4_ITEM "ScriptArg4" /* Login script argument */
#define CONNREC_WEEKDAYS_ITEM "WeekDays" /* Call schedule WeekDays item */
#define CONNREC_REP_PRIORITY_ITEM "RepPriority" /* Replication priority item */
#define CONNREC_REP_TYPE_ITEM "RepType" /* Replication type item */
#define CONNREC_REP_FILELIST_ITEM "Filenames" /* Replication filename list item */
#define CONNREC_REP_TIME_LIMIT_ITEM "TimeLimit" /* Replication time limit item */

/* Connection record types */

#define CONNRECTYPE_NETWORK '0'
#define CONNRECTYPE_REMOTE '1'
#define CONNRECTYPE_PASSTHRU '2'
#define CONNRECTYPE_REMOTE_LAN_SERVICE '5'
#define CONNRECTYPE_X25 '6'
#define CONNRECTYPE_HUNT_GROUP '9'

/* Network connection record item names */

#define CONNREC_TYPE 		"ConnectionType"
#define CONNREC_SOURCE 		"Source"
#define CONNREC_DESTINATION "Destination"
#define CONNREC_PORT 		"PortName"
#define CONNREC_ADDRESS 	"OptionalNetworkAddress"
#define CONNREC_COMMENTS 	"Comments"
#define CONNREC_ENABLED 	"Enabled"

/* Connection record precedence */
#define	CONNRECFIRST	'1'
#define	CONNRECLOW	'0'

/*	Address Book - "Connections" (and other views) Enabled field values */

#define	KWD_DISABLED 	"Disabled"				/* Obsolete */
#define	KWD_NO 			"No"					/* Obsolete */
#define VALUE_NO		'0'
#define VALUE_DISABLED	'0'
#define VALUE_ENABLED	'1'
#define VALUE_STARTUP	'2'

/*   Address Book views used by Mail Address in V3. */

#define PEOPLE_GROUPS_FLAT_VIEW      "($PeopleGroupsFlat)"

/*   Address Book view and column used by Mail Address in V4. */

#define PEOPLE_GROUPS_HIER_VIEW      	"($PeopleGroupsHier)"
#define MAIL_ADDRESS_VIEW				"($PeopleGroupsFlat)"
#define MAIL_ADDRESS_PICKLIST_COLUMN	2
#define MAIL_NAME_PICKLIST_COLUMN		3
#define MAIL_ADDRESS_HIER_COLUMN		3

/*	Address Book view used by ServerConfig (to refresh Notes.INI parameters). */

#define SERVER_CONFIG_NAMESPACE		"$ServerConfig"
#define SERVER_CONFIG_VIEW			"($ServerConfig)"
#define SERVER_ACCESS_VIEW			"($ServerAccess)"

/*	Address Book - Items and names used by name server */

#define NS_SERVERNAMEITEM			0
#define NS_SERVERTITLEITEM			1
#define NS_SERVERNETWORKITEM		2
#define NS_SERVERNETADDRESSESITEM	3
#define NS_SERVERPORTSITEM			4
#define NS_SERVERLOOKUPITEMCOUNT	5
#define NS_SERVERLOOKUPITEMS "ServerName\0ServerTitle\0Network\0NetAddresses\0Ports"

/*	Address Book - Items and names used by DbQuotaSet to verify access */

#define DBQUOTA_SERVERADMINITEM			0
#define DBQUOTA_SERVERLOOKUPITEMCOUNT	1
#define DBQUOTA_SERVERLOOKUPITEMS      "Administrator"

/*	Address Book - Items and names used by server during initialization */

#define INIT_SERVERNAMEITEM			0
#define INIT_SERVERTITLEITEM		1
#define INIT_SERVERADMINITEM		2

#define	INIT_SERVERCERTIFICATE		3
#define	INIT_SERVERCHANGEREQUEST	4

#define INIT_SERVERALLOWITEM		5
#define INIT_SERVERDENYITEM			6
#define INIT_SERVERCREATEITEM		7
#define INIT_SERVERREPLICAITEM		8

#define INIT_SERVERPTTARGETITEM		9
#define INIT_SERVERPTCLIENTITEM		10
#define INIT_SERVERPTCALLERITEM		11
#define INIT_SERVERPTACCESSITEM		12

#define INIT_SERVER_WHITELIST		13
#define INIT_SERVER_ANONYMOUSACCESS	14

#define INIT_SERVER_BUILDNUMBER		15
#define INIT_SERVERRESTRICTEDLIST	16
#define INIT_SERVERUNRESTRICTEDLIST	17

#define INIT_SERVER_CHECK_CLIENT_PW 18

#define INIT_SERVERLOOKUPITEMCOUNT	19
#define INIT_SERVERLOOKUPITEMS      \
"ServerName\0ServerTitle\0Administrator\0\
Certificate\0ChangeRequest\0\
AllowAccess\0DenyAccess\0CreateAccess\0ReplicaAccess\0\
PTTargets\0PTClients\0PTCallers\0PTAccess\0\
WhiteList\0AnonymousAccess\0\
ServerBuildNumber\0RestrictedList\0UnrestrictedList\0ServerCheckPasswords"

/* 
 * Address Book - Items and names used by Assist to verify agent execution 
 * access.
 */
                                        
#define ASSIST_PRIVATE_ITEM             0
#define ASSIST_RESTRICTED_ITEM          1
#define ASSIST_UNRESTRICTED_ITEM        2
#define ASSIST_START_ITEM               3
#define ASSIST_END_ITEM                 4
#define ASSIST_MAX_START_ITEM			5
#define ASSIST_MAX_END_ITEM				6

#define ASSIST_LOOKUP_AMGR_ITEM_COUNT   7
#define ASSIST_LOOKUP_AMGR_ITEMS        "PrivateList\0RestrictedList\0UnrestrictedList\0StartTime\0EndTime\0MaxStartTime\0MaxEndTime"

#define ASSIST_PRIVATE_LIST_NAME        "PRIVATE_LIST"
#define ASSIST_RESTRICTED_LIST_NAME     "RESTRICTED_LIST"
#define ASSIST_UNRESTRICTED_LIST_NAME   "UNRESTRICTED_LIST"

/* 
 * Address Book - Items and names used by Agent Manager to verify agent
 * execution access.
 */

#define AMGR_RELOAD_ITEM            		0
#define AMGR_NORMAL_START_ITEM             	1
#define AMGR_NORMAL_END_ITEM               	2
#define AMGR_NORMAL_CONCURRENT_TASKS_ITEM  	3
#define AMGR_NORMAL_MAX_EXECUTION_ITEM     	4
#define AMGR_NORMAL_PERCENT_TIME_ITEM      	5
#define AMGR_MAX_START_ITEM             	6
#define AMGR_MAX_END_ITEM               	7
#define AMGR_MAX_CONCURRENT_TASKS_ITEM  	8
#define AMGR_MAX_MAX_EXECUTION_ITEM     	9
#define AMGR_MAX_PERCENT_TIME_ITEM     		10
#define AMGR_CREATE_DB_ITEM         		11
#define AMGR_PRIVATE_ITEM           		12
#define AMGR_RESTRICTED_ITEM        		13
#define AMGR_UNRESTRICTED_ITEM      		14
#define AMGR_ALLOWACCESS_ITEM      			15
#define AMGR_DENYACCESS_ITEM      			16

#define AMGR_LOOKUP_ITEM_COUNT      17
#define AMGR_LOOKUP_ITEMS           \
"ReloadTime\0StartTime\0EndTime\0ConcurrentAgents\0MaxExecution\0\
PercentTime\0MaxStartTime\0MaxEndTime\0MaxConcurrentAgents\0\
MaxMaxExecution\0MaxPercentTime\0CreateAccess\0PrivateList\0\
RestrictedList\0UnrestrictedList\0AllowAccess\0DenyAccess"

/*	Address book - Items used by Calendaring & Scheduling for user lookup
	and domain lookup. */

#define SCHED_USERLOOKUPITEMCOUNT			6
#define SCHED_USERLOOKUPITEMS "FullName\0ListName\0MailAddress\0MailDomain\0MailServer\0CalendarDomain"
#define SCHED_USERLOOKUPFULLNAMEITEM		0
#define SCHED_USERLOOKUPLISTNAMEITEM		1
#define SCHED_USERLOOKUPMAILADDRESSITEM		2
#define SCHED_USERLOOKUPMAILDOMAINITEM		3
#define SCHED_USERLOOKUPMAILSERVERITEM		4
#define SCHED_USERLOOKUPCALENDARDOMAINITEM	5

#define SCHED_DOMAINLOOKUPITEMCOUNT					3
#define SCHED_DOMAINLOOKUPITEMS "DomainType\0CalendarServer\0CalendarSystem"
#define SCHED_DOMAINLOOKUPDOMAINTYPEITEM			0
#define SCHED_DOMAINLOOKUPCALENDARSERVERITEM		1
#define SCHED_DOMAINLOOKUPCALENDARSYSTEMITEM		2

#define SCHEDULE_MGR_LOOKUP_ITEM_COUNT			4
#define SCHEDULE_MGR_LOOKUP_ITEMS				"FullName\0MailServer\0MailFile\0Type"
#define SCHEDULE_MGR_LOOKUP_ITEM_MAILSERVER		1
#define SCHEDULE_MGR_LOOKUP_ITEM_MAILFILE		2
#define SCHEDULE_MGR_LOOKUP_ITEM_TYPE			3

/*	View note item names */

#define VIEW_TITLE_ITEM 				FIELD_TITLE				/* Title */
#define VIEW_COLLECTION_ITEM			"$Collection"			/* Contains object ID */
#define VIEW_CONTAINER_ITEM				"$ViewContainer"		/* Contains storage object ID */
#define VIEW_FORMULA_ITEM				"$Formula"				/* Selection formula buffer */
#define VIEW_FORMULA_TIME_ITEM			"$FormulaTV"			/* If present, formula is "time-relative" (@NOW-based) */
#define VIEW_CLASSES_ITEM 				"$FormulaClass" 		/* Formula note classes */
#define VIEW_COLLATION_ITEM				"$Collation"			/* Collation buffer */
#define VIEW_TOTALS_ITEM				"$Totals"				/* Subtotalling specification */
#define VIEW_VIEW_FORMAT_ITEM 			"$ViewFormat" 			/* View table format item */
#define VIEW_INDEX_ITEM 				"$Index"				/* Index disposition options */
#define VIEW_DESIGN_VER_ITEM_V2			"$DesignVersion"		/* Design note version, for V2 backward compatibility */
#define VIEW_DESIGN_VER_ITEM			"$Version"				/* Design note version */
#define VIEW_NOTEREF_ITEM 				FIELD_LINK				/* Note Reference item */
#define VIEW_CONFLICT_ITEM				"$Conflict"				/* Replication update conflict */
#define VIEW_FORM_FORMULA_ITEM			"$FormFormula"			/* Form Formula */
#define VIEW_COMMENT_ITEM				"$Comment"				/* View comment. */
#define VIEW_SELQUERY_ITEM				"$SelQuery"				/* View selection query object */
#define ITEM_NAME_REFOPT				"$RefOptions"			/* ALlow a ref to original 
																	note to be included.
																*/

/* Calendar style view items */

#define VIEW_CALENDAR_FORMAT_ITEM  		"$CalendarFormat"		/* Calendar View format item */


/* Folder-related view items */
/* Pre-build 126 items, can be removed before V4 ship - only exposed internally */
#define VIEW_UNID_SELECTION_ITEM		"$UNIDSelection"		/* List of UNID's which are in view. */
#define VIEW_USE_UNID_SELECTION_ITEM	"$UseUNIDSelection"		/* If present, uses UNID selection table. */

#define VIEW_FOLDER_OBJECT				"$FolderObject"			/*  If present, is ODS version of FOHEADER and 
																	set of entries, see dbfolder.h.  The view
																	may have additional items with suffixes
																	on this item name, e.g., $FolderObject1 */
#define VIEW_FOLDER_IDTABLE				"$FolderIDTable"		/*  If present, is ID table representing the
																	contents of the folder */

/*	Hidden view name prefix/postfix strings. */

#define HIDDEN_VIEW_NAME_PREFIX		"("
#define HIDDEN_VIEW_NAME_POSTFIX	")"

/* ViewMap note item names */

#define VIEWMAP_DATASET_ITEM	"$ViewMapDataset"	/* Contains ViewMap dataset data */
#define VIEWMAP_LAYOUT_ITEM	"$ViewMapLayout"	/* Contains layout objects */

/*	UName special fields */

#define	UNAME_PATH_ITEM			"Path"
#define	UNAME_SERVER_ITEM		"Server"
#define	UNAME_DB_ITEM			"Database"

/*	Actions */

#define ACTION_ITEM "$ACTIONS"

/* Filter note item names */

#define	FILTER_TYPE_ITEM		"$Type"
#define	FILTER_COMMENT_ITEM		"$Comment"
#define	FILTER_FORMULA_ITEM		"$Formula"		/*	Required */
#define	FILTER_FORMULA2_ITEM	"$Formula2"		/*	Second one, optional */
#define	FILTER_SCAN_ITEM		"$Scan"			/*	all in view, selected in view, all in db, unread in db	*/
#define	FILTER_HIGHLIGHT_ITEM	"$Highlight"	/*	'1' if hilites enabled, '0' to remove */
#define	FILTER_QUERY_ITEM		"$Query"		/*	Full text query string used to refine notes to do */
#define FILTER_OPERATION_ITEM	"$Operation"	/*	update, create new, select	*/
#define FILTER_PERIOD_ITEM		"$Period"		/*	hourly, daily, or weekly	*/
#define FILTER_LEFTTODO_ITEM	"$LeftToDo"		/*	Data structure describing what's left to do */
#define FILTER_MACHINE_ITEM		"$MachineName"	/*	name of machine on which this background filter may run */

/*	filter types	*/

#define FILTER_TYPE_MENU 		0
#define FILTER_TYPE_BACKGROUND	1
#define FILTER_TYPE_MAIL 		2
#define FILTER_TYPE_ONCE		3

/*	filter scan options	*/

#define FILTER_SCAN_ALL  		0
#define FILTER_SCAN_UNREAD 		1
#define FILTER_SCAN_VIEW 		2
#define FILTER_SCAN_SELECTED 	3
#define FILTER_SCAN_MAIL		4
#define FILTER_SCAN_NEW			5

#define FILTER_OP_UPDATE  	0
#define FILTER_OP_SELECT  	1
#define FILTER_OP_NEW_COPY	2

/*	filter periods	*/

#define	PERIOD_HOURLY	0
#define	PERIOD_DAILY	1
#define	PERIOD_WEEKLY	2
#define	PERIOD_DISABLED	3

/*	Query note item names. */

/*	If the user has typed in a query, it is stored in $SimpleQuery; If
	the user has created a query using the Query Builder, it is stored
	in the $Builder... fields. */

#define QUERY_SIMPLEQUERY_ITEM	"$SimpleQuery"	/*	simple query */
#define QUERY_BUILDERQUERY_ITEM	"$BuilderQuery"	/*	Query Builder: text list of */
												/*	different portions of query: */
												/*	any of these, all of these, */
												/*	none of these, author */

#define QUERY_BUILDERDATE_ITEM	"$BuilderDate"	/*	Date before or after */
#define QUERY_BUILDERFLAGS_ITEM	"$BuilderFlags"	/*	query builder flags */
#define QUERY_FIELDQUERY_ITEM	"$FieldQuery"	/*	query by form query */

#define QUERY_FIELDACTION_ITEM	"$FieldAction"	/*	action by form struct */
#define QUERY_FORMULAACTION_ITEM	"$FormulaAction"	/*	@function action formula */

#define QUERY_BUILDERFLAG_NEAR			'N'		/*	Apply near to 'all of these' */
#define QUERY_BUILDERFLAG_DATEBEFORE	'B'		/*	Before $BuilderDate (as opposed to after) */

/* 	V4 Search Bar queries */

#define QUERY_V4_OBJECTQUERY_ITEM	"$V4QueryObject" /* Query object built by the V4 Search Builder */
#define QUERY_V4_OPTIONS_ITEM		"$V4QueryOptions" /* Query options from the V4 Search Builder */

/*	V4 Assistants */

#define ASSIST_INFO_ITEM		"$AssistInfo"	/*	info (TYPE_ASSISTANT_INFO) */
#define ASSIST_TYPE_ITEM		"$AssistType"	/*	Type of assistant - related to action type */
#define ASSIST_QUERY_ITEM		"$AssistQuery"	/*	assistant query item */
#define ASSIST_ACTION_ITEM		"$AssistAction"	/*	assistant action item */
#define ASSIST_LASTRUN_ITEM		"$AssistLastRun"	/*	TimeDate of last run */
#define ASSIST_DOCCOUNT_ITEM	"$AssistDocCount"	/*	Number of docs run on last run */
#define ASSIST_RUNINFO_ITEM		"$AssistRunInfo"	/*	Run information object */
#define ASSIST_EXACTION_ITEM	"$AssistAction_Ex"	/*	assistant action item - extra data */
#define ASSIST_VERSION_ITEM		"$AssistVersion"	/*	TIMEDATE of when the agent design 
														(as opposed to enable/disable state)
														was changed */

/* Types for ASSIST_TYPE_ITEM - the following defines are used to determine
 * what type of assistant we are dealing with -- ie, what is the main
 * action type that the user has defined.  The formula ones are for
 * backwards compatability with V3.
 *  See ods.h for the SIG_ACTION_xxx values used for regular V4 generic agents.
 */

#define ASSIST_SIG_ACTION_NONE	 		   -1	/* No action defined */
/* formula, update only -FILTER_OP_UPDATE 	0 */
/* formula, select only -FILTER_OP_SELECT  	1 */
/* formula, new copy    -FILTER_OP_NEW_COPY 2 */

/*	The default search form and default action forms are used by query by form
	and action by form. */

#define DEFAULT_QUERY_BY_FORM		"$DefaultSearch"
#define DEFAULT_ACTION_BY_FORM		"$DefaultAction"

/* Misc Flags */

#define ASSIST_FLAG_ENABLED			'E'
#define ASSIST_FLAG_NEWCOPY			'N'
#define ASSIST_FLAG_HIDDEN			'H'
#define ASSIST_FLAG_PRIVATE			'P'
#define ASSIST_FLAGS_ITEM		"$AssistFlags"	/*	Misc flags */

#define ASSIST_MAIL_ITEM		"$AssistMail"	/*	If this field exists in a mail note, it means that */
												/*	mail message was created by an agent. */

#define ASSIST_TRIGGER_ITEM		"$AssistTrigger"	/*	(Read-only) Assistant's trigger type */
#define FORM_SCRIPT_OBJECT		"$$FormScript_O"	/* Script object code item */
#define SUBFORM_ITEM_NAME "$SubForms"

/* These were moved here from \edit\dmem.h */
#define FORM_SCRIPT_ITEM_NAME "$$FormScript"
#define	DOC_SCRIPT_ITEM		"$Script"
#define DOC_ACTION_ITEM		"$$FormAction"
#define DOC_SCRIPT_NAME		"$$ScriptName"


/* These were moved here from \edit\dmem.h */
#define FORM_SCRIPT_ITEM_NAME "$$FormScript"
#define	DOC_SCRIPT_ITEM		"$Script"
#define DOC_ACTION_ITEM		"$$FormAction"
#define DOC_SCRIPT_NAME		"$$ScriptName"

/* View Script items */
#define VIEW_SCRIPT_NAME		"$ViewScript"
#define VIEW_SCRIPT_OBJECT		"$ViewScript_O"
#define VIEW_GLOBAL_SCRIPT_NAME		"$ViewGlobalScript"
#define VIEW_GLOBAL_SCRIPT_OBJECT	"$ViewGlobalScript_O"
#define VIEW_ACTION_ITEM_NAME	"$ViewAction" 	/* Formula for view script event */
#define VIEW_ACTION_EVENT_COUNT	11	/* ie, $ViewAction0-$ViewAction10 - one per view script event that can contain formulas */

/* Database Script items */
#define DB_SCRIPT_DESIGN_NAME	"Database Script"
#define DB_SCRIPT_NAME		"$DBScript"
#define DB_SCRIPT_OBJECT	"$DBScript_O"
#define DB_ACTION_ITEM_NAME	"$DBAction"

/* Script Library items */
#define SCRIPTLIB_ITEM_NAME	"$ScriptLib"
#define SCRIPTLIB_OBJECT	"$ScriptLib_O"

/* Replication formula note item names */

#define REPLFORMULA_SERVER_ITEM		FIELD_TITLE
#define REPLFORMULA_VERSION_ITEM	"$ReplVersion"
#define REPLFORMULA_SOURCE_SERVERS	"$ReplSrcServers"
#define REPLFORMULA_NOTECLASS_ITEM	"$ReplClassMasks"
#define REPLFORMULA_FORMULA_ITEM 	"$ReplFormula"
#define REPLFORMULA_VIEW_ITEM		"$ReplView"	/* V4 feature; sel repl by view */
#define REPLFORMULA_PRIVATE_FOLDER_ITEM	"$ReplPrivateFolder" 	/* V4 feature; sel repl by private folder */
#define REPLFORMULA_FIELDS_ITEM		"$ReplFields"	/* V4.5 feature; sel repl by fields */

#define FIELD_NAME_DELIMITER ';'
#define VIEW_NAME_DELIMITER ';'
#define VIEW_NAME_DELIMETER VIEW_NAME_DELIMITER /* (misspelled!) */

/* Special replication flags */
 
#define REPLFLAGS_ITEM 	"$ReplFlags"
#define REPL_FLAG_NOREPL	'n'

/* Icon note item names */

#define DESK_ICON_ITEM "IconBitmap"
#define LSECFLAGS_ITEM 	"$LSECFlags"
#define DEFAULT_NAVIGATOR_ITEM "$DefaultNavigator"
#define ICON_TEMPLATEMODTIME_ITEM "$TemplateModTime"
#define ICON_TEMPLATESERVERNAME_ITEM "$TemplateServerName"
#define ICON_TEMPLATEFILENAME_ITEM "$TemplateFileName"

/*	Character flags for replicated local security property ($LSECFlags field
	in the icon note */

#define CHFLAG_SET_LOCALSEC		's'			/* should set local access control */
#define CHFLAG_CLEAR_LOCALSEC	'c'			/* should clear local access control */
#define CHFLAG_COMPACT_LOCALSEC 'p'			/* compact should set local access control */

/* 	character flags for the "replicated file header" ($Flags field in the 
	icon note) */

#define	ICON_FLAGS	DESIGN_FLAGS

/* icon flags */
#define CHFLAG_LAUNCH_APP			'a'
#define CHFLAG_ADDRESS_BOOK_TYPE	'b'
#define CHFLAG_SHOW_CHANGED_POLICY	'c'
#define CHFLAG_LAUNCH_DOCLINK		'd'
#define CHFLAG_RESTRICT_STORED_FORMS 'f'
#define CHFLAG_LIBRARY_TYPE			'g'
#define CHFLAG_DEFERRED_LOADED_IMAGES 'i'
#define CHFLAG_PERSONALJOURNAL_TYPE	'j'
#define CHFLAG_LAUNCH_NAVIGATOR		'l'
#define CHFLAG_SEARCH_SCOPE_TYPE	'm'
#define CHFLAG_NEVER_SHOW_POLICY	'n'
#define CHFLAG_ALWAYS_SHOW_POLICY	'p'
#define CHFLAG_SOLO_NAVIGATOR		's'
#define CHFLAG_OBSOLETE_DONTUSE		't'	
										/* 'refresh' flag from build 130-133 */
										/* has been superceded by a flag in */
										/* ICON_FLAGS_NOREFRESH, below.  */
										/* don't use this one anymore... */
#define CHFLAG_NO_SHOW_VIEW			'v'

#define CHFLAG_PANE_BIGFOLDER		'w'	/* preview pane orientation; mutually exclusive with below */
#define CHFLAG_PANE_SMALLVIEW		'x' /* preview pane orientation; mutually exclusive with above*/
#define CHFLAG_PANE_MAXIMIZED		'y'	/* preview pane size */
										
/* 	unlike the $Flags field, this field doesn't get
	refreshed/replaced on a design refresh/replace 
	and they don't inherit from parent templates
*/
#define ICON_FLAGS_NOREFRESH	"$FlagsNoRefresh"

#define CHFLAG_NR_ADV_TEMPLATE			't'

/* Special replication flags */
 
#define REPLFLAGS_ITEM 	"$ReplFlags"
#define REPL_FLAG_NOREPL	'n'

/* Auto-merge replication conflicts */

#define ITEM_CONFLICT_ACTION 	"$ConflictAction"	/* enable auto-merge conflicts if 1 */
#define 	CONFLICT_AUTOMERGE	'1'

/* Form note item names */

#define	ITEM_NAME_TEMPLATE "$Body"			/* form item to hold form CD */
#define	ITEM_NAME_DOCUMENT "$Info"			/* document header info */
#define	ITEM_NAME_TEMPLATE_NAME FIELD_TITLE	/* form title item */
#define	ITEM_NAME_FORMLINK "$FormLinks"		/* form link table */
#define	ITEM_NAME_FIELDS "$Fields"			/* field name table */
#define ITEM_NAME_FORMPRIVS "$FormPrivs"	/* form privileges */
#define ITEM_NAME_FORMUSERS "$FormUsers"	/* text list of users allowed to use the form */

/* Special form flags */						
 
#define ITEM_NAME_KEEP_PRIVATE "$KeepPrivate"	
#define PRIVATE_FLAG_YES	'1'				/* $KeepPrivate = TRUE  force disabling of printing, mail forwarding and edit copy */
#define PRIVATE_FLAG_YES_RESEND	'2'			/* $KeepPrivate = TRUE  same as PRIVATE_FLAG_YES except allow resend */

#define ITEM_NAME_BACKGROUNDGRAPHIC "$Background"	
#define ITEM_NAME_PAPERCOLOR "$PaperColor"	

#define ITEM_NAME_AUTO_EDIT_NOTE "$AutoEditMode"	
#define AUTO_EDIT_FLAG_YES	'1'				/* $AutoEditMode = TRUE  force edit mode on open regardless of Form flag */

#define ITEM_NAME_HIDE_SCROLL_BARS 	"$HideScrollBars"
#define WINDOW_SCROLL_BARS_NONE 	'1'
#define WINDOW_SCROLL_BARS_HORZ 	'2'
#define WINDOW_SCROLL_BARS_VERT 	'3'



#define ITEM_NAME_VERSION_OPT "$VersionOpt"	/* Over-ride the Form flags for versioning. */
#define VERSION_FLAG_NONE	'0'				/* $Version = 0, None */
#define VERSION_FLAG_MURESP	'1'				/* $Version = 1, Manual - Update becomes response */
#define VERSION_FLAG_AURESP	'2'				/* $Version = 2, Auto   - Update becomes response */
#define VERSION_FLAG_MUPAR	'3'				/* $Version = 3, Manual - Update becomes parent */
#define VERSION_FLAG_AUPAR	'4'				/* $Version = 4, Auto   - Update becomes parent */
#define VERSION_FLAG_MUSIB	'5'				/* $Version = 5, Manual - Update becomes sibling */
#define VERSION_FLAG_AUSIB	'6'				/* $Version = 6, Auto   - Update becomes sibling */


/* Document note item names */

#define	ITEM_NAME_TEMPLATE_USED FIELD_FORM	/* form name used to create note, user-visible */
#define ITEM_NAME_NOTEREF FIELD_LINK		/* optional reference to another note */
#define ITEM_NAME_VERREF "$VERREF"			/* optional reference to master version note */
#define	ITEM_NAME_LINK "$Links"				/* note link table */
#define ITEM_NAME_REVISIONS "$Revisions"	/* Revision history */
#define ITEM_NAME_CONFLICT VIEW_CONFLICT_ITEM /* Replication update conflict */
#define ITEM_NAME_AUTHORS "$Authors"		/* text list of users allowed to modify document */

/* Document and form note item names, all items are optional */

#define	ITEM_NAME_FONTS "$Fonts"			/* font table */
#define ITEM_NAME_HEADER "$Header"			/* print page header */
#define ITEM_NAME_FOOTER "$Footer"			/* print page footer */
#define ITEM_NAME_HFFLAGS "$HFFlags"		/* header/footer flags */
	#define	HFFLAGS_NOPRINTONFIRSTPAGE	'1'		/* suppress printing header/footer on first page */
#define ITEM_NAME_WINDOWTITLE "$WindowTitle"/* window title */
#define	ITEM_NAME_ATTACHMENT "$FILE" 		/* file attachment, MUST STAY UPPER-CASE BECAUSE IT'S SIGNED! */

/* Document note Sign/Seal item names */

#define	ITEM_NAME_NOTE_SIGNATURE "$Signature"
#define	ITEM_NAME_NOTE_SIG_PREFIX "$Sig_"	/*	Prefix for multiple signatures. */
#define	ITEM_NAME_NOTE_SEAL "$Seal"
#define	ITEM_NAME_NOTE_SEALDATA "$SealData"
#define ITEM_NAME_NOTE_SEALNAMES "SecretEncryptionKeys"
#define ITEM_NAME_NOTE_SEALUSERS "PublicEncryptionKeys"
#define	ITEM_NAME_NOTE_FORCESIGN "Sign"
#define	ITEM_NAME_NOTE_FORCESEAL "Encrypt"
#define	ITEM_NAME_NOTE_FORCEMAIL "MailOptions"
#define	ITEM_NAME_NOTE_FORCESAVE "SaveOptions"
#define	ITEM_NAME_NOTE_FORCESEALSAVED "EncryptSaved"
#define	ITEM_NAME_NOTE_MAILSAVE	"MailSaveOptions"
#define	ITEM_NAME_NOTE_FOLDERADD "FolderOptions"

/* Group expansion item and legal values */

#define	ITEM_NAME_NOTE_GROUPEXP	 "ExpandPersonalGroups"	/* For backward compatibility */
#define ITEM_NAME_NOTE_EXPANDGROUPS	 "$ExpandGroups"
#define MAIL_DONT_EXPAND_GROUPS			'0'
#define MAIL_EXPAND_LOCAL_GROUPS		'1'
#define MAIL_EXPAND_PUBLIC_GROUPS		'2'
#define MAIL_EXPAND_LOCAL_AND_PUBLIC_GROUPS	'3'

/* Search term highlights item name prefix.  An item name is 
	concatenated to this; e.g. $Highlights_Body.  */

#define ITEM_NAME_HIGHLIGHTS	"$Highlights_"

/* Import/Export document item names */

#define IMPORT_BODY_ITEM_NAME "Body"
#define IMPORT_FORM_ITEM_NAME FIELD_FORM
#define NEW_FORM_ITEM_NAME FIELD_FORM

/*	Help index note item names */

#define ITEM_OBJECT	"$Section"
#define	HELP_VIEW_NAME "(All)"

/*	Help view names */

#define HELP_VIEW_TOC "Top10"		/*	Table of contents view */
#define HELP_VIEW_MSG "MSG"		/*	Messages view */
#define HELP_VIEW_ATFUNC "REF"	/*	@Functions view */
#define HELP_VIEW_INDEX "IDX"	/*	Index view */
#define HELP_VIEW_NEWS "NEWS" 	/* Release Notes view */
#define HELP_VIEW_V3CMD "CLASSIC" 	/* V3 Menu Command Equivalents */

/* Help navigator names */

#define HELP_NAV_HOW "Help Topics - How"
#define HELP_NAV_INDEX "Help Topics - Index"
#define HELP_NAV_MSG "Help Topics - Trouble"
#define HELP_NAV_ATFUNC "Help Topics - Tell Me"
#define HELP_NAV_KBD	"Help Topics - Tell Me"

/*	Catalog form names */

#define NOTEFILE_FORM_NAME			"Notefile"
#define CONNECTION_FORM_NAME		"Connection"

/*	Catalog view names */
#define	CATALOG_VIEW_REPLICA_ID "Databases by _Replica ID"

/*	Catalog document note item names */

#define CATALOG_ITEM_SERVER		 	"Server"
#define CATALOG_ITEM_REMOTE_SERVER 	"RemoteServer"
#define CATALOG_ITEM_PATHNAME 		"Pathname"
#define CATALOG_ITEM_TITLE  		"Title"
#define CATALOG_ITEM_REPLICAID  	"ReplicaID"
#define CATALOG_ITEM_POLICY_DATE 	"PolicyDate"
#define CATALOG_ITEM_LICENSEID		"LicenseID"
#define CATALOG_ITEM_MANAGERS		"Managers"
#define CATALOG_ITEM_BODY			"Body"
#define CATALOG_ITEM_VIEW_INDEX		"View"
#define CATALOG_ITEM_CATEGORY1		"Category1"
#define CATALOG_ITEM_CATEGORY2		"Category2"
#define CATALOG_ITEM_CATEGORY3		"Category3"
#define CATALOG_ITEM_CATEGORY4		"Category4"
#define CATALOG_ITEM_CATEGORY5		"Category5"
#define CATALOG_ITEM_CAT			MAIL_CATEGORIES_ITEM

/*	Multi DB searching document forms, item names, and item values */

#define MULTIDB_FORM_CONFIG			"ConfigurationForm"
#define MULTIDB_FORM_ENTRY			"Database"
#define MULTIDB_ITEM_ACL			"ACL"
#define MULTIDB_ITEM_CATEGORIES		"DatabaseCategories"
#define MULTIDB_ITEM_CONFIGLINK		"ConfigLink"
#define MULTIDB_ITEM_DOMAIN			"Domain"
#define MULTIDB_ITEM_INDEXOPTIONS	"IndexOptions"
#define MULTIDB_ITEM_PATHNAME		"Pathname"
#define MULTIDB_ITEM_REPLICAID		"ReplicaID"
#define MULTIDB_ITEM_SCOPE			"Scope"
#define MULTIDB_ITEM_SERVER			"Server"
#define MULTIDB_ITEM_TITLE			"Title"
#define MULTIDB_ITEM_VIEWCHOICE		"DbViews"
#define MULTIDB_ITEM_VIEWLIST		"DatabaseViews"
#define MULTIDB_INDEX_NONE			"0"
#define MULTIDB_INDEX_SUMMARY		"1"
#define MULTIDB_INDEX_FULLNOTE		"2"
#define MULTIDB_INDEX_ATTACHMENT	"3"
#define MULTIDB_SCOPE_DATABASE		"0"
#define MULTIDB_SCOPE_DIRECTORY		"1"
#define MULTIDB_SCOPE_SERVER		"2"
#define MULTIDB_SCOPE_DOMAIN		"3"
#define	MULTIDB_VIEW_REPLICAID 		"$ReplicaID"

/*	Library document note item names */

#define LIBRARY_ITEM_AUTHOR			"Author"
#define LIBRARY_ITEM_SERVER		 	"Server"
#define LIBRARY_ITEM_PATHNAME 		"Pathname"
#define LIBRARY_ITEM_TITLE  		"Title"
#define LIBRARY_ITEM_REPLICAID  	"ReplicaID"
#define LIBRARY_ITEM_ABSTRACT		"Abstract"
#define LIBRARY_PATHNAME			"LibraryPathName"
#define LIBRARY_TITLE				"LibraryTitle"
#define LIBRARY_SENDTO				"SendTo"
#define LIBRARY_LIBRARIAN			"Librarian"

/* Library Form Names */
#define LIBRARIAN_FORM_NAME			"Librarian"

/* Library View Names */
#define LIBRARY_REPID_VIEW		   	"($RepID)"	
#define LIBRARY_LIBRARIAN_VIEW		"($Librarians)"	

/* Disk directory search item names and values */

#define DBDIR_PATH_ITEM 		"$Path"		 		/* Path name */
#define DBDIR_TYPE_ITEM 		"$Type"				/* Type item */
#define DBDIR_TYPE_ITEM_DIRECTORY "$DIR"			/* Directory type name */
#define DBDIR_TYPE_ITEM_NOTEFILE "$NOTEFILE"		/* Notefile type name */
#define DBDIR_TYPE_ITEM_OLDNOTEFILE "$OLDNOTEFILE"
#define DBDIR_INFO_ITEM 		"$Info"				/* Info item */
#define DBDIR_LENGTH_ITEM		"$Length"
#define DBDIR_MODIFIED_ITEM		"$Modified"
#define DBDIR_PROPERTIES_ITEM 	"$Properties"
#define DBDIR_LINK_ITEM 		"$LinkInfo"
#define DBDIR_DIRECTORY_LINK 	"$DIRLINK"			/* Directory Link */
#define DBDIR_DATABASE_LINK 	"$DBLINK"			/* Database Link */

/* Form names for log database. */

#define	LOG_EVENT_FORM 		"Events"
#define	LOG_REPLICA_FORM 	"Replication"
#define	LOG_MAIL_FORM 		"Mail Routing"
#define	LOG_SESSION_FORM 	"Session"
#define LOG_USAGE_FORM		"Activity"
#define LOG_SIZE_FORM		"Size"
#define LOG_PHONE_FORM		"Phone Call"
#define LOG_PHONE_IN_FORM	"Phone Call - Incoming"
#define LOG_PHONE_OUT_FORM	"Phone Call - Outgoing"

/*	Item names used by the log facility itself or commonly used
	by different packages that call the log facility. */

#define LOG_ITEM_STARTTIME			"StartTime"
#define LOG_ITEM_ENDTIME			"FinishTime"
#define LOG_ITEM_BODY				"Body"
#define LOG_ITEM_SERVER 			"Server"
#define LOG_ITEM_PORT				"Port"
#define LOG_ITEM_EVENTS				"Events"
#define LOG_ITEM_PATHNAME			"Pathname"
#define LOG_ITEM_TITLE				"Title"
#define LOG_ITEM_CAT				MAIL_CATEGORIES_ITEM

/* Item names for replicator notes in log file. */

#define REPLICA_ITEM_SOURCE_SERVER	"SourceServer"
#define REPLICA_ITEM_INITIATED_BY	"InitiatedBy"

/* Item names for session notes in log file. */

#define SESSION_ITEM_USERNAME		"UserName"
#define SESSION_ITEM_READS			"Reads"
#define SESSION_ITEM_WRITES			"Writes"
#define SESSION_ITEM_SENT_FROM		"SentFromServer"
#define SESSION_ITEM_SENT_TO		"SentToServer"
#define SESSION_ITEM_TRANSACTIONS	"Transactions"

/* Item names for escrow log */

#define	ESCROW_SENDTO				"Escrow Agent"

#define	ESCROW_USER					"User"
#define	ESCROW_SERVER				"Server"
#define	ESCROW_CERTIFIER			"Certifier"

#define	ESCROW_LOCATION_ITEM		"Location"
#define	ESCROW_COMMENT_ITEM			"Comment"
	
/* Item names for database activity note in log file */

#define ACTIVITY_ITEM_MANAGERS		"Managers"
#define ACTIVITY_ITEM_VIEWTITLES	"ViewTitles"
#define ACTIVITY_ITEM_VIEWSIZES		"ViewSizes"
#define ACTIVITY_ITEM_DISKSPACE		"DiskSpace"
#define ACTIVITY_ITEM_DISKUSAGE		"DiskUsage"
#define ACTIVITY_ITEM_PERIOD		"Period"
#define ACTIVITY_ITEM_PERIOD_USES	"PeriodUses"
#define ACTIVITY_ITEM_PERIOD_READS	"PeriodReads"
#define ACTIVITY_ITEM_PERIOD_WRITES	"PeriodWrites"
#define ACTIVITY_ITEM_DAY_USES		"DayUses"
#define ACTIVITY_ITEM_DAY_READS		"DayReads"
#define ACTIVITY_ITEM_DAY_WRITES	"DayWrites"
#define ACTIVITY_ITEM_WEEK_USES		"WeekUses"
#define ACTIVITY_ITEM_WEEK_READS	"WeekReads"
#define ACTIVITY_ITEM_WEEK_WRITES	"WeekWrites"
#define ACTIVITY_ITEM_MONTH_USES	"MonthUses"
#define ACTIVITY_ITEM_MONTH_READS	"MonthReads"
#define ACTIVITY_ITEM_MONTH_WRITES	"MonthWrites"

/* Item names for unet notes in log file. */
#define UNIXNET_ITEM_BODY	"UnixBody"

#if 0

/* Address Book - "X400Server" form item names */

#define MAIL_X400_SERVER_FORM "Server"				/* Form and type name */
#define MAIL_X400_SERVERNAME_ITEM "ServerName"		/* X400 MTA name */
#define MAIL_X400_GLOBAL_DOMAIN_ITEM "GlobalDomain" /* Global domain identifier */ 
#define MAIL_X400_COUNTRY_ITEM "Country"			/* Country name */
#define MAIL_X400_ADMD_ITEM "ADMD"					/* Administration Management Domain name*/
#define MAIL_X400_PRMD_ITEM "Domain"				/* Private Domain name */
#define MAIL_X400_CREDENTIALS_ITEM "Credentials"	/* MTA credentials */
#define MAIL_X400_SUPP_APP_CONTEXTS_ITEM "SupportedApplicationsContext"	/* Supported app. contexts */
#define MAIL_X400_DEL_CONTENT_LENGTH_ITEM "DeliveryContentLength" 	/* Delivery content length */
#define MAIL_X400_DEL_CONTENT_TYPE_ITEM "DeliveryContentTypes"	 	/* Delivery content types */
#define MAIL_X400_PSEL_ITEM "PSEL"					/* Presentation selector */
#define MAIL_X400_SSEL_ITEM "SSEL"					/* Session selector */
#define MAIL_X400_TSEL_ITEM "TSEL"					/* Transport selector */
#define MAIL_X400_NADDRESSES_ITEM "NAddresses"		/* Network addresses */

#endif



/*	Public Queue Names */

#define	TASK_QUEUE_PREFIX	"MQ$"			/* Prepended to "addin" task */
											/* name to form task's queue name */

#define SERVER_QUEUE_NAME	"_SERVER"						/* DB Server */
#define REPL_QUEUE_NAME		TASK_QUEUE_PREFIX"REPLICATOR"	/* Replicator */
#define ROUTER_QUEUE_NAME	TASK_QUEUE_PREFIX"ROUTER"		/* Mail Router */
#define UPDATE_QUEUE_NAME 	TASK_QUEUE_PREFIX"INDEXER"		/* Index views & full text process */
#define LOGIN_QUEUE_NAME 	TASK_QUEUE_PREFIX"LOGIN"		/* Login Process */
#define EVENT_QUEUE_NAME	TASK_QUEUE_PREFIX"EVENT"		/* Event process */
#define REPORT_QUEUE_NAME	TASK_QUEUE_PREFIX"REPORTER"		/* Report process */
#define CLREPL_QUEUE_NAME	TASK_QUEUE_PREFIX"CLREPL"		/* Cluster Replicator */
#define FIXUP_QUEUE_NAME	TASK_QUEUE_PREFIX"FIXUP"		/* Fixup */
#define COLLECT_QUEUE_NAME  TASK_QUEUE_PREFIX"COLLECTOR"	/* Collector*/
#define ALARM_QUEUE_NAME	TASK_QUEUE_PREFIX"ALARMS"		/* Alarms Cache daemon */



/*  Names for server access lists and other stuff from Name & address book*/

#define SERVER_TITLE_NAME			"SERVER_TITLE"
#define ALLOW_ACCESS_NAME			"ALLOW_ACCESS"
#define DENY_ACCESS_NAME		  	"DENY_ACCESS"
#define CREATE_FILE_ACCESS_NAME		"CREATE_FILE_ACCESS"
#define CREATE_REPLICA_ACCESS_NAME	"CREATE_REPLICA_ACCESS"
#define ADMIN_ACCESS_NAME			"ADMIN_ACCESS"
#define ALLOW_PASSTHRU_TARGET_NAME	"ALLOW_PASSTHRU_TARGETS"
#define ALLOW_PASSTHRU_CLIENT_NAME	"ALLOW_PASSTHRU_CLIENTS"
#define ALLOW_PASSTHRU_CALLER_NAME	"ALLOW_PASSTHRU_CALLERS"
#define ALLOW_PASSTHRU_ACCESS_NAME	"ALLOW_PASSTHRU_ACCESS"
#define ALLOW_RESTRICTED_LOTUSCRIPT_NAME	"ALLOW_RESTRICTED_LOTUSSCRIPT"
#define ALLOW_UNRESTRICTED_LOTUSCRIPT_NAME	"ALLOW_UNRESTRICTED_LOTUSSCRIPT"

/*	Names for folder pane. */

#define CATEGORIES_AS_FOLDERS		"CategoriesAsFolders"

/*	Location information */

#define LOCATION_DEFAULT	"Location"
#define LOCATION_TYPE		"LocationType"
#define LOCATION_TRAVELPROMPT "TravelPrompt"
#define LOCATION_FORMTYPE	"Location"
#define LOCATION_VIEW		"($Locations)"
#define LOCATION_NAME		"Name"
#define LOCATION_SOURCE		"Source"
#define LOCATION_USERID		"UserID"
#define LOCATION_TIMEZONE	"TimeZone"
#define LOCATION_DST		"DST"
#define LOCATION_DOMAIN		"Domain"
#define LOCATION_MAILSERVER	"MailServer"
#define LOCATION_MAILFILE	"MailFile"
#define LOCATION_MAILFILE_LASTSEQ	"$MailFileSeq"
#define LOCATION_DEFPASSTHRU "DefaultPassthruServer"
#define LOCATION_OUTSIDELINE "OutsideLine"
#define LOCATION_CARDPREFIX	"CallPre"	/* Number to dial to get carriers dial tone (10ATT or 1800...) */
#define LOCATION_CARDSUFFIX	"Suffix"	/* Number to identify user;s account (i.e. User's Home Phone + PIN) */
#define LOCATION_INTERNATIONALPREFIX	"InternationalPrefix"
#define LOCATION_LONGDISTANCEPREFIX		"LongDistancePrefix"
#define LOCATION_COUNTRY	"Country"
#define LOCATION_AREACODE	"AreaCode"
#define LOCATION_MAILTYPE	"MailType"
#define LOCATION_MAILTHRESHOLD		"MailThreshold"
#define LOCATION_TCPNAMESERVER		"TcpNameServer"
#define LOCATION_TCPNAMESERVERHOST	"TcpNameServerHost"
#define LOCATION_SCHEDULE	"Enabled"
#define LOCATION_NDSNAMESERVER			"NDSNameServer"
#define LOCATION_NDSNAMESERVERADDRESS	"NDSNameServerAddress"
#define LOCATION_SOCKSPROXY 			"Proxy_SOCKs"
#define LOCATION_SSLPROXY 				"Proxy_SSL"
#define LOCATION_NOTESHTTPPROXY			"Proxy_NotesHttp"
#define LOCATION_IMAGES		"Images"

		/* InterNotes/Web Location Items */

#define LOCATION_INTERNOTES					"InterNotes"
#define LOCATION_WEBLOCALDB					"WebDbName"
#define LOCATION_WEBSERVERDB				"WebDbName"
#define LOCATION_WEBRETRIEVERLOCATION		"WebLocation"
#define LOCATION_WEBUPDATECACHE				"WebUpdateCache"
#define LOCATION_WEBRETRIEVERLOGGING		"WebLogLevel"
#define LOCATION_WEBRETRIEVER				"WebRetriever"
#define LOCATION_WEBRETRIEVERS				"WebRetrievers"
#define LOCATION_WEBRETRIEVERPATH			"WebRetrieverPath"
#define LOCATION_WEBHTTPPROXY				"Proxy_HTTP"
#define LOCATION_WEBFTPPROXY				"Proxy_FTP"
#define LOCATION_WEBGOPHERPROXY				"Proxy_Gopher"
#define LOCATION_WEBNOPROXY					"No_Proxy"

#define LOCATION_SSLSITECERT				"SSLSiteCerts"

		/* Java Location Items */

#define LOCATION_JAVASECUREDOMAINLIST		"JavaSecureDomain"
#define LOCATION_JAVAALLOWINSECURE			"JavaAllowInSecureDomain"
#define LOCATION_JAVAALLOWINOTHER			"JavaAllowInOtherDomains"
#define LOCATION_JAVATRUSTPROXY				"JavaTrustHTTPProxy"

/* Prior to build 125, in the location record, the ports that were listed
	were those that were enabled.  Beginning in build 125, the ports that
	are listed (internally by a different item) are those that are disabled.
	The location form will however present the enabled ports by selecting the
	ports which are not specifed in the disabled item.  When saved, it is
	inverted back to mean the ports that are disabled.  In this manner,
	newly added ports will be enabled for all locations (by default). */

#define LOCATION_AUTONETCONFIG				"AutoNetConfig"
#define LOCATION_PORTS			"PortName"	  		/* Prior to 125 */
#define LOCATION_DISABLEDPORTS	"DisabledPorts"		/* 125 and later */
#define LOCATION_NAMELOOKUPPREF	"NameLookupPref"
#define LOCATION_EXHAUSTIVENAMELOOKUP	"ExhaustiveNameLookup"
#define LOCATION_MAIL_ADDR_FILE "MailAddressFile"	/* Last N&A Book used at this location */
#define LOCATION_VERSION	"$LocationVersion"
#define LOCATION_BCASELIST	"$BCaseList"
#define LOCATION_DIALING_SERVERS "$DialingServers"
#define LOCATION_DIALING_RULES   "$DialingRules"
#define LOCATION_STACKFILENAMES	"$StackFileNames"	/* Filename of topmost
														replica icon at this
														location for this
														'stack', */

#define LOCATION_SAVED_SERVER	"$SavedServers"		/* Server name */
#define LOCATION_SAVED_PORT		"$SavedPorts"		/* Port server reached on */
#define LOCATION_SAVED_ADDRESS	"$SavedAddresses"	/* Server address on this port */
#define LOCATION_SAVED_DATE		"$SavedDate"		/* Julian Date */
#define LOCATION_RLAN_IDLE_TIMEOUT	"RLANIdleTimeout"	/* timeout for RLAN hangup */

/* The following field was introduced in build 133 so that new icons added
	to the desktop get bubbled to the top of the stack if appropriate.  New
	icons should be at the top of the stack if:

	the DB icon is mew, is for a server DB, the location has a LAN port and
		the current top of the stack is a local database.
	the DB icon is mew, is for a local DB, the location has no LAN port and
		the current top of the stack is a server database.
*/
#define LOCATION_STACKCHECKED		"$StackChecked"		



/*	Fields used to store the list of folders into which a document is to be
	saved.  Note that @Command([ChooseFolders]) reads and writes these fields,
	so you do not normally need to manipulate them in a document.

	Note also that to a user, there are only two sets of folders: Shared and
	Private, with the distinction that some Private folders are stored in the
	databse, and some on the workstation in the desktop file (when the user
	cannot store them in the database due to access restrictions). */

#define FOLDERS_SHARED			"$Folders"			  	/*	List of shared folders. */
#define FOLDERS_PRIVATE			"$PrivateFolders"	  	/*	List of private folders (in database). */
#define FOLDERS_PRIVATE_LOCAL	"$LocalPrivateFolders"	/*	List of private folders (in desktop). */

/* Item names of fields in Mail template: */

#define MAILREPORT_ITEM	"$DeliveryReport"		/* TBD */


/*	Standard folder names. */

#define FOLDER_INBOX			"($Inbox)"
#define FOLDER_DRAFTS			"($Drafts)"
#define FOLDER_SENT				"($Sent)"
#define FOLDER_ALL				"($All)"
#define FOLDER_CALENDAR			"($Calendar)"
#define FOLDER_TODO				"($ToDo)"
#define FOLDER_MEETINGS			"($Meetings)"
#define FOLDER_TRASH			"($Trash)"
#define FOLDER_PROFILE			"($Profiles)"
#define FOLDER_ALARMS			"($Alarms)"

#define FOLDER_INBOX_NAME		"$Inbox"
#define FOLDER_DRAFTS_NAME		"$Drafts"
#define FOLDER_SENT_NAME		"$Sent"
#define FOLDER_ALL_MAIL			"$All"
#define FOLDER_CALENDAR_NAME	"$Calendar"
#define FOLDER_TODO_NAME		"$ToDo"
#define FOLDER_MEETINGS_NAME	"$Meetings"
#define FOLDER_TRASH_NAME		"$Trash"
#define FOLDER_PROFILE_NAME		"$Profiles"

/*	Router Shared Mail Object Store names */

#define SHARED_MAIL_NAME			"SHARED_MAIL"				/* config variable for shared mail */
#define ROUTER_OBJECT_STORE_NAME	"mailobj.nsf"				/* object store link file used by the router */
#define ROUTER_OBJECT_STORE_DBNAME	"mailobj1.nsf"				/* object store database created by the router */

/* Cluster Name and Address Book lookup definitions */

#define CLUSTERS_CLUSTER_VIEW_1		"1\\$Clusters"
#define CLUSTERS_CLUSTER_VIEW		"$Clusters"
#define CLUSTERS_CLUSTER_ITEM_NAME	"ClusterName"	
#define CLUSTERS_CLUSTER_ITEM		0

#define CLUSTERS_SERVERS_VIEW_1		"1\\$Servers"
#define CLUSTERS_SERVERS_VIEW		"$Servers"
#define CLUSTERS_SERVER_ITEM_NAME	"ServerName"	
#define CLUSTERS_SERVER_ITEM		0

#define CLUSTERS_CLREPID_ITEM_NAME	"ClRepID"
#define CLUSTERS_CLREPID_ITEM		0

/* Cluster Database Directory definitions */

#define CLUSTERS_SERVER_VIEW		"$Server"
#define CLUSTERS_PATHNAME_VIEW		"$Pathname"
#define CLUSTERS_REPID_VIEW			"$ReplicaID"

#define CLUSTERS_SERVER_FIELD		"Server"
#define CLUSTERS_REPID_FIELD		"ReplicaID"
#define CLUSTERS_PATHNAME_FIELD		"PathName"
#define CLUSTERS_TITLE_FIELD  		"Title"
#define CLUSTERS_OUTOFSERVICE_FIELD	"DatabaseOutOfService"
#define CLUSTERS_REPL_FIELD			"ClusterReplicate"
#define CLUSTERS_FAILBYPATH_FIELD	"OnlyFailoverbyPathname"
#define CLUSTERS_DELETE_FIELD		"MarkedForDelete"

#define CLUSTERS_DBDIR_NAME				"cldbdir.nsf"
#define CLUSTERS_DBDIR_TEMPLATE_NAME	"cldbdir4.ntf"

/* Values for CLUSTERS_OUTOFSERVICE_FIELD */
#define CLUSTERS_IN_SERVICE 		"0"
#define CLUSTERS_OUT_OF_SERVICE 	"1"

/* Values for CLUSTERS_CLREPL_FIELD */
#define CLUSTERS_CLUSTER_REPLICATE_ON	"1"
#define CLUSTERS_CLUSTER_REPLICATE_OFF	"0"


/*	Last specified path and username for alternate mail logon dialog. */

#define ALT_MAIL_LAST_PATH			"AltMailLastPath"
#define ALT_MAIL_LAST_NAME			"AltMailLastName"

/*	Convert Utility Constants. */

#define ITEM_NAME_CONVERT_FORM	FIELD_FORM
#define ITEM_NAME_CONVERT_DATE	"ConvertedDate"
#define ITEM_NAME_HIDDEN_DOC	"HiddenDocument"

/*	Names stored as UNKs if a note has rarely used item names. */

#define	RARELY_USED_NAME		"$RarelyUsedName"
#define	RARELY_USED_TABLE		"$RarelyUsedTable"

#define AGENT_HSCRIPT_ITEM		"$AgentHScript"
#define AGENT_HSCRIPTOBJ_ITEM	"$AgentHScript_O"

/*	Local schedule retrieval defintions */

#define	LOCSCHED_FORM_NAME		"LocalSchedSettings"
#define LOCSCHED_PROFILE_NAME	"LocalSchedSettings"

/*	Fields in local schedule database (busytime) for local 
	schedule synchronization via replicator page. */


#define	ITEM_LOCSCHED_LIST			"LocalSchedList"
#define ITEM_LOCSCHED_DAYSTOFETCH	"LocalSchedDaysToFetch"
#define ITEM_LOCSCHED_SUPPRESS		"LocalSchedSuppressMinutes"



#endif /* STD_NAME_DEFS */


#ifdef __cplusplus
}



#endif
