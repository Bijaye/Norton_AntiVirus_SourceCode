// We need this file to send keys to bochs

#include "..\Bochs-96\iodev\iodev.h"

// Here is a scan code table (incomplete)
// It is used to convert ASCII chars from the expert system to scan code for Bochs input

// NOT USED

// Use the (better) translation table in Bochs

#if 0
unsigned char scanCodeTab[256] = {
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 0
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 10
	0x39,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0x34,0, // 20
	0x0B,0x02,0x03,0x04, 0x05,0x06,0x07,0x08, 0x09,0x0A,0,0, 0,0,0,0, // 30

	// Only lowercase letters for now...
	0,0x1E,0x30,0x2E, 0x20,0x12,0x21,0x22, 0x23,0x17,0x24,0x25, 0x26,0x32,0x31,0x18, // 40
	0x19,0x10,0x13,0x1F, 0x14,0x16,0x2F,0x11, 0x2D,0x15,0x2C,0, 0x2B,0,0,0, // 50
	0,0x1E,0x30,0x2E, 0x20,0x12,0x21,0x22, 0x23,0x17,0x24,0x25, 0x26,0x32,0x31,0x18, // 60
	0x19,0x10,0x13,0x1F, 0x14,0x16,0x2F,0x11, 0x2D,0x15,0x2C,0, 0,0,0,0, // 70

	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 80
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 90
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // A0
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // B0

	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // C0
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // D0
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // E0
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // F0
};
#endif


//__inline static void copy_bytes(from, to, n)
//{
//  memcpy(to, from, n);
//}				


/*
 :******************************************************************
 :* UNIT-BODY lib.str2h
 :*
 :* TITLE         (/ conversion of a string to an integer /)
 :*
 :* PURPOSE       (/ convert the internal representation (binary) of an
 :*                  integer to a hexa decimal string                   /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-12-02
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*
 :*                                                               /)
 :*   DATA       (/
 :*                                                                /)
 :*
 :*                                                          END-DATA
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/
 :*                                Preliminary report on
 :*                      Advanced Security Audit trail Analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*
 :*                      Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre    (/ - 'm' is the address of a string representing an integer
 :*                         ___
 :*                'm' --->|_|_|
 :*                          |
 :*                         \|/
 :*                          |lg|integer|
 :*                           -----------
 :*
 :* 		- lg = 1 if this is a single byte 
 :*             - lg = 2 if this is a "short int"
 :*                  = 4 if this is a "int"
 :*             - integer is the binary representation of a short int or int
 :*
 :*                                                                     /)
 :*                                                             END-Pre
 :*   Post   (/ - the returned value is the hexa string
 :*                                                                 /)
 :*                                                             END-Post
 :*   RETURNS  "void *"   (/ the value of the integer            /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY lib.str2h
 :******************************************************************
*/
#define ASAXPATH "/home/swimmer/asax/sys/src"
#ifdef WIN32
  #include "asax.h"
#else
  #include "/home/swimmer/asax/sys/src/asax.h"
#endif
static int var_int;
void *str2h(m)
char *m;
{
#define ushort unsigned short

    hole **h; 
    short *sht;
    int lg;
    ushort *len;
    unsigned char *uc;
    char *s, *hexaStr;
    
    s = (char *) tmalloc(16, char);
    if(s == NULL) error1(0, 24, "tmalloc failure in str2h() of conv_lib.c");
    hexaStr = s + sizeof (ushort); /* skip the length */ 
    len = (ushort *) s;
    h = (hole **) m; 
    lg = lenstr(*h);
    m = (char *) *h;
    m += sizeof(ushort);
    switch(lg) {
	
	case 0:
		     strcpy(hexaStr, "VOID");
		     *len = (ushort) strlen(hexaStr);

		    return((void *) s);
	case 1:	    
		    uc = (unsigned char *) m;
		    sprintf(hexaStr, "%X", (int)*uc);
		    if (strlen(hexaStr)%2)
			sprintf(hexaStr, "0%X", *uc);
		    *len = (ushort) strlen(hexaStr);
		    return ((void *) s);

	case 2:
		    sht = (short *) m;
		    sprintf(hexaStr, "%02X", (int)*sht);
		    *len = (ushort) strlen(hexaStr);
		    return ((void *) s);

	case 4:
//		    copy_bytes(m, &var_int, sizeof(int));
		    memcpy(&var_int, m, sizeof(int));
		    sprintf(hexaStr, "%04X", var_int);
		    *len = (ushort) strlen(hexaStr);
		    return ((void *) s);
	default:
		    error1(0, 24, "str2h() of conv_lib.c");
		    return((void *) -1);
    }
}

void *byte2h(m)
char *m;
{
    ushort *len;
    char *s, *hexaStr;
    unsigned char i;

	i = * (unsigned char *) m;
    // Allocate the space for the string
	s = (char *) tmalloc(16, char);
    if(s == NULL) error1(0, 24, "tmalloc failure in str2h() of conv_lib.c");
    hexaStr = s + sizeof (ushort); /* skip the length */ 
    len = (ushort *) s;

	sprintf (hexaStr, "%02X", i);
	*len = (ushort) strlen (hexaStr);
	return (void *) s;
}

void *word2h(m)
char *m;
{
    ushort *len;
    char *s, *hexaStr;
    unsigned short i;

	i = * (unsigned short *) m;
    // Allocate the space for the string
	s = (char *) tmalloc(16, char);
    if(s == NULL) error1(0, 24, "tmalloc failure in str2h() of conv_lib.c");
    hexaStr = s + sizeof (ushort); /* skip the length */ 
    len = (ushort *) s;

	sprintf (hexaStr, "%04X", i);
	*len = (ushort) strlen (hexaStr);
	return (void *) s;
}

void *int2h(m)
char *m;
{
    ushort *len;
    char *s, *hexaStr;
    unsigned int i;

	i = * (unsigned int *) m;
    // Allocate the space for the string
	s = (char *) tmalloc(16, char);
    if(s == NULL) error1(0, 24, "tmalloc failure in int2h() of conv_lib.c");
    hexaStr = s + sizeof (ushort); /* skip the length */ 
    len = (ushort *) s;

	sprintf (hexaStr, "%X", i);
	*len = (ushort) strlen (hexaStr);
	return (void *) s;
}

// Function sending a key to Bochs
void *sendscancode(m)
char *m;
{
	bx_keybd_gen_scancode(*(unsigned char *)m);
	return (void *) 0;
}

// Functions sending text to Bochs
void *sendtxt(m)
char *m;
{
	// m is a pointer to a pointer to the string (the string starts with the string length)
	int len;
	unsigned char *str;
	unsigned char *c;
	hole **h;

	h = (hole **) m;
	len = lenstr(*h);
	str = (*(unsigned char **) m)+2;

	for (c=str ; c<str+len ; c++)
		//bx_keybd_gen_scancode(scanCodeTab[*c]);
		if (shifted_ascii[*c&0x7F]) {
			bx_keybd_gen_scancode(0x36);
			bx_keybd_gen_scancode(ascii_to_scancode[*c&0x7F]);		// press
			bx_keybd_gen_scancode(ascii_to_scancode[*c&0x7F]|0x80);	// release
			bx_keybd_gen_scancode(0xB6);
		} else {
			bx_keybd_gen_scancode(ascii_to_scancode[*c&0x7F]);		// press
			bx_keybd_gen_scancode(ascii_to_scancode[*c&0x7F]|0x80);	// release
		}
	return (void *) 0;
}

void *sendln(m)
char *m;
{
	sendtxt(m);
	bx_keybd_gen_scancode(0x1C); // Scan code for return
	bx_keybd_gen_scancode(0x9C);
	return (void *) 0;
}

#ifdef XXX
void *append(m)
char *m;
{
        hole **sp1;
        char *getExtStrRepr(), *s, *s0, *s1, *s2;
        unsigned short *sh;
        int l1, l2;

        sp1 = (hole **) m++; 
        l1 = strlen(s1 = getExtStrRepr(*sp1++));
        l2 = strlen(s2 = getExtStrRepr(*sp1));
        s = s0 = tmalloc(l1 + l2 + 2, char);
        sh = (unsigned short *) s0;
        *sh++ = l1 + l2 + 2;
        s0 = (char *) sh;
        while (*s1 != ' ' && *s1) {
                *s0 = *s1++;
                s0++;
        } 
        strcpy(s0, s2);
        return( (void *) s);
}
#endif
