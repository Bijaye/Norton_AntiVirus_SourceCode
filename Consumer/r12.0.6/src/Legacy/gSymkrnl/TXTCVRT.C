/*********************************************************************/
/* Copyright 1996 Symantec, Peter Norton Computing Group             */
/*********************************************************************/
/*                                                                   */
/* $Header:   S:/SYMKRNL/VCS/txtcvrt.c_v   1.10   03 Oct 1996 17:10:26   JOUYANG  $ */
/*                                                                   */
/* Description: Ansi to Oem and Oem to Ansi conversions for DOS      */
/*                                                                   */
/* Contains:                                                         */
/*                                                                   */
/* See Also:                                                         */
/*                                                                   */
/*********************************************************************/
/* $Log:   S:/SYMKRNL/VCS/txtcvrt.c_v  $ */
// 
//    Rev 1.10   03 Oct 1996 17:10:26   JOUYANG
// Added codepage 1250 to 852 and 852 to 1250 conversion
// 
//    Rev 1.9   19 Sep 1996 21:30:04   JOUYANG
// Changed array tables to Windows version of the equivilent functions
// 
//    Rev 1.8   19 Sep 1996 14:55:40   JOUYANG
// Added 0's to characters 255 for both arrays of Ansi to Oem and Oem 
// to Ansi
// 
//    Rev 1.7   19 Sep 1996 13:38:12   SPAGLIA
// Added Buff() functions
//
//    Rev 1.6   19 Sep 1996 01:01:28   JOUYANG
// Replaced code with current new updated one
//
//    Rev 1.5   18 Sep 1996 23:38:46   JOUYANG
// Optimized code for Ansi to Oem and vice versa.  
// Also changed to 2 parameters
//
//    Rev 1.4   17 Sep 1996 12:07:14   JOUYANG
// Changed from reading from TEXT file to internal Array declaration
//
//    Rev 1.3   16 Sep 1996 18:16:32   JOUYANG
// Retain the original character code if code is not found in array
//
//    Rev 1.2   16 Sep 1996 18:09:14   JOUYANG
// text file Ansi to Oem update
//
//    Rev 1.1   16 Sep 1996 17:51:06   JOUYANG
// Ansi to Oem update
//
//    Rev 1.0   16 Sep 1996 17:23:24   JOUYANG
// Initial revision.
//
/*********************************************************************/

#include "platform.h"
#include "xapi.h"
#include "stddos.h"

/*********************************************************************/
/*                                                                   */
/*  Contains:(Note: these 2 functions were created to run DOS agents)*/
/*                                                                   */
/*  DOS_AnsiToOem() - takes the pointer of a given ANSI string and   */
/*		      returns the OEM equivalence of the pointer to			 */
/*		      that given string  									 */
/*                                                                   */
/*  DOS_OemToAnsi() - takes the pointer of a given OEM string and    */
/*		      returns the ANSI equivalence of the pointer to		 */
/*		      that given string 									 */
/*********************************************************************/


/*********************************************************************/
/*  The Code Pages for conversions are predefined before the actual  */
/*  functions                                                        */
/*********************************************************************/

// Ansi To Oem Code Pages //

// Codepage for conversion from 1252 to 437 defined in Win95 //

static unsigned char array_437[128] =
    {
    63, 63, 44, 159, 44, 46, 43, 216, 94, 37, 83, 60, 79, 63, 63, 63, 
    63, 96, 39, 34, 34, 7, 45, 45, 126, 84, 115, 62, 111, 63, 63, 89, 
    255, 173, 155, 156, 15, 157, 221, 21, 34, 99, 166, 174, 170, 45, 
    114, 95, 248, 241, 253, 51, 39, 230, 20, 250, 44, 49, 167, 175, 
    172, 171, 95, 168, 65, 65, 65, 65, 142, 143, 146, 128, 69, 144, 
    69, 69, 73, 73, 73, 73, 68, 165, 79, 79, 79, 79, 153, 120, 79, 85, 
    85, 85, 154, 89, 95, 225, 133, 160, 131, 97, 132, 134, 145, 135, 
    138, 130, 136, 137, 141, 161, 140, 139, 100, 164, 149, 162, 147, 
    111, 148, 246, 111, 151, 163, 150, 129, 121, 95, 152
    };

// Codepage for conversion from 1250 to 852 defined in Win95 //

static unsigned char array_852[128] =
	{
	63, 63, 39, 63, 34, 7, 197, 197, 63, 37, 230, 60, 151, 155, 166, 
	141, 63, 39, 39, 34, 34, 7, 45, 45, 63, 116, 231, 62, 152, 156, 
	167, 171, 255, 243, 244, 157, 207, 164, 124, 245, 249, 99, 184, 
	174, 170, 240, 82, 189, 248, 43, 242, 136, 239, 117, 20, 7, 247, 
	165, 173, 175, 149, 241, 150, 190, 232, 181, 182, 198, 142, 145, 
	143, 128, 172, 144, 168, 211, 183, 214, 215, 210, 209, 227, 213, 
	224, 226, 138, 153, 158, 252, 222, 233, 235, 154, 237, 221, 225,
	234, 160, 131, 199, 132, 146, 134, 135, 159, 130, 169, 137, 216,
	161, 140, 212, 208, 228, 229, 162, 147, 139, 148, 246, 253, 133,
	163, 251, 129, 236, 238, 250
	};


// Oem To Ansi Code Pages //

// Codepage for conversion from 437 to 1252 defined in Win95 //

static unsigned char array_1252[128] =
    {
    199, 252, 233, 226, 228, 224, 229, 231, 234, 235, 232, 239, 238, 
    236, 196, 197, 201, 230, 198, 244, 246, 242, 251, 249, 255, 214, 
    220, 162, 163, 165, 80, 131, 225, 237, 243, 250, 241, 209, 170, 
    186, 191, 173, 172, 189, 188, 161, 171, 187, 166, 166, 166, 166, 
    166, 166, 166, 43, 43, 166, 166, 43, 43, 43, 43, 43, 43, 45, 45, 
    43, 45, 43, 166, 166, 43, 43, 45, 45, 166, 45, 43, 45, 45, 45, 45, 
    43, 43, 43, 43, 43, 43, 43, 43, 166, 95, 166, 166, 175, 97, 223, 
    71, 112, 83, 115, 181, 116, 70, 84, 79, 100, 56, 102, 101, 110, 
    61, 177, 61, 61, 40, 41, 247, 152, 176, 183, 183, 118, 110, 178, 
    166, 160
    };

// Codepage for conversion from 852 to 1250 defined in Win95 //

static unsigned char array_1250[128]=
	{
	199, 252, 233, 226, 228, 249, 230, 231, 179, 235, 213, 245, 238, 
	143, 196, 198, 201, 197, 229, 244, 246, 188, 190, 140, 156, 214,
	220, 141, 157, 163, 215, 232, 225, 237, 243, 250, 165, 185, 142, 
	158, 202, 234, 172, 159, 200, 186, 171, 187, 45, 45, 45, 45, 43,
	193, 194, 204, 170, 166, 166, 172, 45, 175, 191, 172, 76, 43, 84,
	43, 166, 43, 195, 227, 76, 45, 166, 84, 166, 61, 43, 164, 240, 208,
	207, 203, 239, 210, 205, 206, 236, 45, 45, 45, 45, 222, 217, 45, 
	211, 223, 212, 209, 241, 242, 138, 154, 192, 218, 224, 219, 253,
	221, 254, 180, 173, 189, 178, 161, 162, 167, 247, 184, 176, 168, 
	255, 251, 216, 248, 166, 160
	};

static int CodePage = 0;

LPSTR SYM_EXPORT WINAPI DOS_AnsiToOemBuff(LPSTR str1, LPSTR str2, UINT length)
    {
    unsigned char ch, *p = NULL;
    UINT count;
    int i;

	if (CodePage == 0)
		CodePage = DOSGetCodePage();
 
	if (CodePage == 437)	// selecting codepages
    	p = array_437;
    else if (CodePage == 852)
    	p = array_852;
							// returns same string if no codepage found 
	if (p == NULL)
	{	
		if(str2 != str1) 
		memcpy(str2, str1, length);
    }
    
    else
    {
    for (count = 0;  count < length;  count++)
        {
        ch = *(str1 + count);

        if (ch < 128)  // lower ascii no conversion necessary //
            *(str2 + count) = ch;

        else		   // high ascii using look up table //
            {
            i = ch - 128;
			*(str2 + count) = p[i];
		  	}
		}
    }
	return str2;
    }


LPSTR SYM_EXPORT WINAPI DOS_AnsiToOem(LPSTR str1, LPSTR str2)
    {

    UINT length;

    length = STRLEN(str1);
    DOS_AnsiToOemBuff(str1, str2, length);

    *(str2 + length) = '\0';
    return (str2);
    }


LPSTR SYM_EXPORT WINAPI DOS_OemToAnsiBuff(LPSTR str1, LPSTR str2, UINT length)
    {
    unsigned char ch, *p = NULL;
    UINT count;
    int i;
 
	if (CodePage == 0)
		CodePage = DOSGetCodePage();
 
	if (CodePage == 437)	// selecting codepages
    	p = array_1252;
    else if (CodePage == 852)
    	p = array_1250;
							// returns same string if no codepage found 
    if (p == NULL)
	{	
		if(str2 != str1) 
		memcpy(str2, str1, length);
    }
    
	else
    {
    for (count = 0;  count < length;  count++)
        {
        ch = *(str1 + count);

        if (ch < 128)  // lower ascii no conversion necessary //
            *(str2 + count) = ch;

        else           // high ascii using look up table //
            {
            i = ch - 128;
			*(str2 + count) = p[i];
		  	}
		}
    }
	return str2;
    }


LPSTR SYM_EXPORT WINAPI DOS_OemToAnsi(LPSTR str1, LPSTR str2)
    {

    UINT length;

    length = STRLEN(str1);
    DOS_OemToAnsiBuff(str1, str2, length);

    *(str2 + length) = '\0';
    return (str2);
    }


