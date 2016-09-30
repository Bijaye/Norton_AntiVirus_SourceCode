// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef __cplusplus
extern "C" {
#endif


/* Distinguished name handling package. */

#ifndef DNAME_DEFS
#define DNAME_DEFS


#define DN_DELIM '/'				/* Component delimiter between RDN's */
#define	DN_DELIM_RDN '+'			/* Component delimiter within an RDN */
#define	DN_DELIM_RDN_ABBREV '+'		/* Display component delimiter within an RDN (when abbreviating)*/
#define DN_TYPE_DELIM '='			/* Type name delimiter */
#define DN_OUNITS 4					/* Maximum number org units */

/* Distinguished name parsing result data structure */

typedef struct {
	DWORD Flags;					/* Parsing flags */
#define DN_NONSTANDARD      0x0001	/* Name includes non-standard components */
									/*  Ie., contains unrecognized labels */
#define DN_NONDISTINGUISHED 0x0002	/* Non-distinguished name */
									/*  Ie., contains no delimiters or labeled attributes */
#define DN_CN_OU_RDN		0x0008	/* CN plus OU are relative distinguished name */
#define DN_O_C_RDN			0x0010	/* O plus C are relative distinguished name */
#define DN_NONABBREV		0x0020	/* Name includes components that cannot be abbreviated */
									/*  E.g., G, I, S, Q, P, A */

	WORD CLength;					/* Country name length */
	char far *C;					/* Country name pointer */
	WORD OLength;					/* Organization name length */
	char far *O;					/* Organization name pointer */
	WORD OULength[DN_OUNITS];		/* Org Unit name lengths */
									/*  OULength[0] is rightmost org unit */
	char far *OU[DN_OUNITS];		/* Org unit name pointers */
									/*  OU[0] is rightmost org unit */
	WORD CNLength;					/* Common name length */
	char far *CN;					/* Common name pointer */
	WORD DomainLength;				/* Domain name length */
	char far *Domain;				/* Domain name pointer */

/*	Original V3 structure ended here.  The following fields were added in V4 */

	WORD PRMDLength;				/* Private management domain name length */
	char far *PRMD;					/* Private management domain name pointer */
	WORD ADMDLength;				/* Administration management domain name length */
	char far *ADMD;					/* Administration management domain name pointer */
	WORD GLength;					/* Given name length */
	char far *G;					/* Given name name pointer */
	WORD SLength;					/* Surname length */
	char far *S;					/* Surname pointer */
	WORD ILength;					/* Initials length */
	char far *I;					/* Initials pointer */
	WORD QLength;					/* Generational qualifier (e.g., Jr) length */
	char far *Q;					/* Generational qualifier (e.g., Jr) pointer */
	} DN_COMPONENTS;


/* Functions */

#define DN_ABBREV_INCLUDEALL 0x00000001 /* Include all component types, even when same as template */

STATUS LNPUBLIC DNAbbreviate(DWORD Flags, char far *TemplateName, char far *InName,
								char far *OutName, WORD OutSize, WORD far *OutLength);
STATUS LNPUBLIC DNCanonicalize(DWORD Flags, char far *TemplateName, char far *InName,
								char far *OutName, WORD OutSize, WORD far *OutLength);

STATUS LNPUBLIC DNParse(DWORD Flags, char far *TemplateName, 
							char far *InName, DN_COMPONENTS far *Comp, WORD CompSize);

#endif

#ifdef __cplusplus
}
#endif

