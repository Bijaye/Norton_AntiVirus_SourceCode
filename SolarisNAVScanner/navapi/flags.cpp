// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPI/VCS/flags.cpv   1.1   21 May 1998 20:30:54   MKEATIN  $
//
// Description:
//
//  Contains flag handling code for the CPU emulator.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVAPI/VCS/flags.cpv  $
// 
//    Rev 1.1   21 May 1998 20:30:54   MKEATIN
// Changed pamapi.h to pamapi_l.h
//
//    Rev 1.0   21 May 1998 19:23:38   MKEATIN
// Initial revision.
//
//    Rev 1.8   28 Feb 1996 18:41:18   CNACHEN
// Added comments part-way...
//
//    Rev 1.7   13 Dec 1995 10:13:12   CNACHEN
// Fixed unused variable problem for NLM...
//
//    Rev 1.6   15 Nov 1995 20:40:24   CNACHEN
// Fixed parity problems..
//
//    Rev 1.5   19 Oct 1995 18:23:08   CNACHEN
// Initial revision... with comment header :)
//
//************************************************************************

#include "pamapi_l.h"

//************************************************************************
// void dwset_carry_flag_add(hLocal, a, b, c)
//
// Purpose
//
//      This function is used to set the state of the carry flag when DWORD
//  operands are being added.  If the A+B+C results in a 32-bit carry out, then
//  hLocal->CPU.FLAGS.C = TRUE, else hLocal->CPU.FLAGS.C = FALSE.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      DWORD       a               First number to add
//      DWORD       b               Second number to add
//      DWORD       c               Carry in value
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/28/96 Carey created.
//************************************************************************

void dwset_carry_flag_add(PAMLHANDLE hLocal, DWORD a, DWORD b, DWORD c)
{
	DWORD temp;

	temp = (a&0x7FFFFFFFL) + (b&0x7FFFFFFFL) + c;

	// if we have a carry in to the high bit and either a or b has a high
	// bit set, then we have a carry out.  OR if we have both high bits set
	// on a and be then we have a carry out

	if ((temp & 0x80000000L & (a|b)) || (a&b&0x80000000L))
		hLocal->CPU.FLAGS.C = TRUE;
	else
		hLocal->CPU.FLAGS.C = FALSE;
}


//************************************************************************
// void wset_carry_flag_add(hLocal, a, b, c)
//
// Purpose
//
//      This function is used to set the state of the carry flag when WORD
//  operands are being added.  If the A+B+C results in a 16-bit carry out, then
//  hLocal->CPU.FLAGS.C = TRUE, else hLocal->CPU.FLAGS.C = FALSE.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD       a                First number to add
//      WORD       b                Second number to add
//      WORD       c                Carry in value
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/28/96 Carey created.
//************************************************************************

void wset_carry_flag_add(PAMLHANDLE hLocal, WORD a, WORD b, WORD c)
{
    DWORD temp = (DWORD)a + (DWORD)b + (DWORD)c;

	if (temp > 0xffff)
        hLocal->CPU.FLAGS.C = TRUE;
	else
        hLocal->CPU.FLAGS.C = FALSE;
}


//************************************************************************
// void bset_carry_flag_add(hLocal, a, b, c)
//
// Purpose
//
//      This function is used to set the state of the carry flag when BYTE
//  operands are being added.  If the A+B+C results in a 8-bit carry out, then
//  hLocal->CPU.FLAGS.C = TRUE, else hLocal->CPU.FLAGS.C = FALSE.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      BYTE       a                First number to add
//      BYTE       b                Second number to add
//      BYTE       c                Carry in value
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/28/96 Carey created.
//************************************************************************

void bset_carry_flag_add(PAMLHANDLE hLocal, BYTE a, BYTE b, BYTE c)
{
	WORD temp = (WORD)a + (WORD)b + (WORD)c;

	if (temp > 0xff)
        hLocal->CPU.FLAGS.C = TRUE;
	else
		hLocal->CPU.FLAGS.C = FALSE;
}


//************************************************************************
// void dwset_carry_flag_sub(hLocal, a, b, c)
//
// Purpose
//
//      This function is used to set the state of the carry flag when DWORD
//  operands are being subtracted.  If the A-B-C results in a 32-bit carry out,
//  then hLocal->CPU.FLAGS.C = TRUE, else hLocal->CPU.FLAGS.C = FALSE.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      DWORD       a               First number to subtract
//      DWORD       b               Second number to subtract
//      DWORD       c               Carry in value
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/28/96 Carey created.
//************************************************************************

void dwset_carry_flag_sub(PAMLHANDLE hLocal, DWORD a, DWORD b, DWORD c)
{
	if (a < b)
	{
		hLocal->CPU.FLAGS.C = TRUE;
		return;
	}

	if (a == b && c)
		hLocal->CPU.FLAGS.C = TRUE;
	else
		hLocal->CPU.FLAGS.C = FALSE;

}


//************************************************************************
// void wset_carry_flag_sub(hLocal, a, b, c)
//
// Purpose
//
//      This function is used to set the state of the carry flag when WORD
//  operands are being subtracted.  If the A-B-C results in a 16-bit carry out,
//  then hLocal->CPU.FLAGS.C = TRUE, else hLocal->CPU.FLAGS.C = FALSE.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD       a                First number to subtract
//      WORD       b                Second number to subtract
//      WORD       c                Carry in value
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/28/96 Carey created.
//************************************************************************

void wset_carry_flag_sub(PAMLHANDLE hLocal, WORD a, WORD b, WORD c)
{
	if (a < b)
	{
		hLocal->CPU.FLAGS.C = TRUE;
		return;
	}

	if (a - b == 0 && c)
		hLocal->CPU.FLAGS.C = TRUE;
	else
		hLocal->CPU.FLAGS.C = FALSE;

}


//************************************************************************
// void bset_carry_flag_sub(hLocal, a, b, c)
//
// Purpose
//
//      This function is used to set the state of the carry flag when BYTE
//  operands are being subtracted.  If the A-B-C results in a 8-bit carry out,
//  then hLocal->CPU.FLAGS.C = TRUE, else hLocal->CPU.FLAGS.C = FALSE.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      BYTE        a               First number to subtract
//      BYTE        b               Second number to subtract
//      BYTE        c               Carry in value
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/28/96 Carey created.
//************************************************************************

void bset_carry_flag_sub(PAMLHANDLE hLocal, BYTE a, BYTE b, BYTE c)
{
	if (a < b)
	{
		hLocal->CPU.FLAGS.C = TRUE;
		return;
	}

	if (a - b == 0 && c)
		hLocal->CPU.FLAGS.C = TRUE;
	else
		hLocal->CPU.FLAGS.C = FALSE;
}

//************************************************************************
// void dwset_over_flag_add(hLocal, a, b, c)
//
// Purpose
//
//      This function is used to set the state of the overflow flag when DWORD
//  operands are being added.  If the A+B+C results in a 32-bit overflow then
//  hLocal->CPU.FLAGS.O = TRUE, else hLocal->CPU.FLAGS.O = FALSE.
//  NOTE: Overflow occurs when the sign bit of the result (A+B+C) is different
//  then the sign bit of both argument A and argument B, and
//  A&0x80000000 == B&0x80000000.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      DWORD       a               First number to add
//      DWORD       b               Second number to add
//      DWORD       c               Carry in value
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/28/96 Carey created.
//************************************************************************

void dwset_over_flag_add(PAMLHANDLE hLocal, DWORD a, DWORD b, DWORD c)     /* for ADC, b = b + CF */
{
	if ((a >> 31) != ((DWORD)(a+b+c)>>31))
	{
		hLocal->CPU.FLAGS.O = ((b >> 31) != ((DWORD)(a+b+c)>>31));
		return;
	}

	hLocal->CPU.FLAGS.O = FALSE;

}


//************************************************************************
// void wset_over_flag_add(hLocal, a, b, c)
//
// Purpose
//
//      This function is used to set the state of the overflow flag when WORD
//  operands are being added.  If the A+B+C results in a 16-bit overflow then
//  hLocal->CPU.FLAGS.O = TRUE, else hLocal->CPU.FLAGS.O = FALSE.
//  NOTE: Overflow occurs when the sign bit of the result (A+B+C) is different
//  then the sign bit of both argument A and argument B, and
//  A&0x8000 == B&0x8000.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD       a                First number to add
//      WORD       b                Second number to add
//      WORD       c                Carry in value
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/28/96 Carey created.
//************************************************************************

void wset_over_flag_add(PAMLHANDLE hLocal, WORD a, WORD b, WORD c)     /* for ADC, b = b + CF */
{
	if ((a >> 15) != ((WORD)(a+b+c)>>15))
	{
		hLocal->CPU.FLAGS.O = ((b >> 15) != ((WORD)(a+b+c)>>15));
		return;
	}

	hLocal->CPU.FLAGS.O = FALSE;
}


//************************************************************************
// void bset_over_flag_add(hLocal, a, b, c)
//
// Purpose
//
//      This function is used to set the state of the overflow flag when BYTE
//  operands are being added.  If the A+B+C results in a 8-bit overflow then
//  hLocal->CPU.FLAGS.O = TRUE, else hLocal->CPU.FLAGS.O = FALSE.
//  NOTE: Overflow occurs when the sign bit of the result (A+B+C) is different
//  then the sign bit of both argument A and argument B, and
//  A&0x80 == B&0x80.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      BYTE       a                First number to add
//      BYTE       b                Second number to add
//      BYTE       c                Carry in value
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/28/96 Carey created.
//************************************************************************

void bset_over_flag_add(PAMLHANDLE hLocal, BYTE a, BYTE b, BYTE c)     /* for ADC, b = b + CF */
{
	if ((a >> 7) != ((BYTE)(a+b+c)>>7))
	{
		hLocal->CPU.FLAGS.O = ((b >> 7) != ((BYTE)(a+b+c)>>7));
		return;
	}

	hLocal->CPU.FLAGS.O = FALSE;
}


//************************************************************************
// void dwset_over_flag_sub(hLocal, a, b, c)
//
// Purpose
//
//      This function is used to set the state of the overflow flag when DWORD
//  operands are being added.  If the A+B+C results in a 32-bit overflow then
//  hLocal->CPU.FLAGS.O = TRUE, else hLocal->CPU.FLAGS.O = FALSE.
//  NOTE: Overflow occurs when the sign bit of the result (A-B-C) is different
//  then the sign bit of both argument A and argument B, and
//  A&0x80000000 == B&0x80000000.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      DWORD       a               First number to subtract
//      DWORD       b               Second number to subtract
//      DWORD       c               Carry in value
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/28/96 Carey created.
//************************************************************************

void dwset_over_flag_sub(PAMLHANDLE hLocal, DWORD a, DWORD b, DWORD c)     /* for ADC, b = b + CF */
{
	if ((a >> 31) != ((DWORD)(a-b-c)>>31))
	{
		hLocal->CPU.FLAGS.O = ((b >> 31) == ((DWORD)(a-b-c)>>31));
		return;
	}

	hLocal->CPU.FLAGS.O = FALSE;
}


//************************************************************************
// void wset_over_flag_sub(hLocal, a, b, c)
//
// Purpose
//
//      This function is used to set the state of the overflow flag when WORD
//  operands are being added.  If the A+B+C results in a 16-bit overflow then
//  hLocal->CPU.FLAGS.O = TRUE, else hLocal->CPU.FLAGS.O = FALSE.
//  NOTE: Overflow occurs when the sign bit of the result (A-B-C) is different
//  then the sign bit of both argument A and argument B, and
//  A&0x8000 == B&0x8000.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD       a                First number to subtract
//      WORD       b                Second number to subtract
//      WORD       c                Carry in value
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/28/96 Carey created.
//************************************************************************

void wset_over_flag_sub(PAMLHANDLE hLocal, WORD a, WORD b, WORD c)     /* for ADC, b = b + CF */
{
	if ((a >> 15) != ((WORD)(a-b-c)>>15))
	{
		hLocal->CPU.FLAGS.O = ((b >> 15) == ((WORD)(a-b-c)>>15));
		return;
	}

	hLocal->CPU.FLAGS.O = FALSE;
}


//************************************************************************
// void bset_over_flag_sub(hLocal, a, b, c)
//
// Purpose
//
//      This function is used to set the state of the overflow flag when BYTE
//  operands are being added.  If the A+B+C results in a 8-bit overflow then
//  hLocal->CPU.FLAGS.O = TRUE, else hLocal->CPU.FLAGS.O = FALSE.
//  NOTE: Overflow occurs when the sign bit of the result (A-B-C) is different
//  then the sign bit of both argument A and argument B, and
//  A&0x80 == B&0x80.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      BYTE       a                First number to subtract
//      BYTE       b                Second number to subtract
//      BYTE       c                Carry in value
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/28/96 Carey created.
//************************************************************************

void bset_over_flag_sub(PAMLHANDLE hLocal, BYTE a, BYTE b, BYTE c)     /* for ADC, b = b + CF */
{
	if ((a >> 7) != ((BYTE)(a-b-c)>>7))
	{
		hLocal->CPU.FLAGS.O = ((b >> 7) == ((BYTE)(a-b-c)>>7));
		return;
	}

	hLocal->CPU.FLAGS.O = FALSE;
}

void set_over_flag_add(PAMLHANDLE hLocal,
                       WORD wMemType,
					   DWORD a,
					   DWORD b,
					   DWORD c)
{
    if (wMemType & WORD_SIZE)
		wset_over_flag_add(hLocal,(WORD)a,(WORD)b,(WORD)c);
	else if (wMemType & DWORD_SIZE)
		dwset_over_flag_add(hLocal,a,b,c);
	else
		bset_over_flag_add(hLocal,(BYTE)a,(BYTE)b,(BYTE)c);
}

void set_over_flag_sub(PAMLHANDLE hLocal,
                       WORD wMemType,
                       DWORD a,
                       DWORD b,
                       DWORD c)
{
    if (wMemType & WORD_SIZE)
		wset_over_flag_sub(hLocal,(WORD)a,(WORD)b,(WORD)c);
    else if (wMemType & DWORD_SIZE)
        dwset_over_flag_sub(hLocal,a,b,c);
	else
		bset_over_flag_sub(hLocal,(BYTE)a,(BYTE)b,(BYTE)c);
}


void dwset_aux_carry_flag_sub(PAMLHANDLE hLocal, DWORD a, DWORD b, DWORD c)
{
	a &= 0xf;
	b &= 0xf;
	c &= 0xf;

	if (a < b)
	{
		hLocal->CPU.FLAGS.A = TRUE;
		return;
	}

	if (a == b && c)
		hLocal->CPU.FLAGS.A = TRUE;
	else
		hLocal->CPU.FLAGS.A = FALSE;

}

void dwset_aux_carry_flag_add(PAMLHANDLE hLocal, DWORD a, DWORD b, DWORD c)
{
	a &= 0xf;
	b &= 0xf;
	c &= 0xf;

	if (a+b+c > 0xf)
		hLocal->CPU.FLAGS.A = TRUE;
	else
		hLocal->CPU.FLAGS.A = FALSE;
}



void set_carry_flag_add(PAMLHANDLE hLocal,
					   WORD wMemType,
					   DWORD a,
					   DWORD b,
					   DWORD c)
{
	if (wMemType & WORD_SIZE)
		wset_carry_flag_add(hLocal,(WORD)a,(WORD)b,(WORD)c);
	else if (wMemType & DWORD_SIZE)
		dwset_carry_flag_add(hLocal,a,b,c);
	else
		bset_carry_flag_add(hLocal,(BYTE)a,(BYTE)b,(BYTE)c);

	dwset_aux_carry_flag_add(hLocal, a, b, c);
}

void set_carry_flag_sub(PAMLHANDLE hLocal,
					   WORD wMemType,
					   DWORD a,
					   DWORD b,
					   DWORD c)
{
	if (wMemType & WORD_SIZE)
		wset_carry_flag_sub(hLocal,(WORD)a,(WORD)b,(WORD)c);
	else if (wMemType & DWORD_SIZE)
		dwset_carry_flag_sub(hLocal,a,b,c);
	else
		bset_carry_flag_sub(hLocal,(BYTE)a,(BYTE)b,(BYTE)c);

	dwset_aux_carry_flag_sub(hLocal, a, b, c);
}


void set_zero_flag(PAMLHANDLE hLocal, WORD wMemType, DWORD dwValue)
{
    if (wMemType & WORD_SIZE)
		hLocal->CPU.FLAGS.Z = !(dwValue & 0xFFFFU);
    else if (wMemType & DWORD_SIZE)
		hLocal->CPU.FLAGS.Z = !dwValue;
	else
		hLocal->CPU.FLAGS.Z = !(dwValue & 0xFF);
}

void set_sign_flag(PAMLHANDLE hLocal, WORD wMemType, DWORD dwValue)
{
    if (wMemType & WORD_SIZE)
        hLocal->CPU.FLAGS.S = !!(dwValue & 0x8000U);
    else if (wMemType & DWORD_SIZE)
        hLocal->CPU.FLAGS.S = !!(dwValue & 0x80000000L);
    else
        hLocal->CPU.FLAGS.S = !!(dwValue & 0x80);
}

#define PAM_PARITY_EVEN 1
#define PAM_PARITY_ODD  0

int nibble_parity[16] = {PAM_PARITY_EVEN, PAM_PARITY_ODD, PAM_PARITY_ODD, PAM_PARITY_EVEN,
						 PAM_PARITY_ODD, PAM_PARITY_EVEN, PAM_PARITY_EVEN, PAM_PARITY_ODD,
						 PAM_PARITY_ODD, PAM_PARITY_EVEN, PAM_PARITY_EVEN, PAM_PARITY_ODD,
						 PAM_PARITY_EVEN, PAM_PARITY_ODD, PAM_PARITY_ODD, PAM_PARITY_EVEN};

void set_parity_flag(PAMLHANDLE hLocal, WORD wMemType, DWORD dwValue)
{
    (void)wMemType;

	dwValue ^= (dwValue >> 4);
	dwValue &= 0xFLU;

	hLocal->CPU.FLAGS.P = nibble_parity[(WORD)dwValue];
}
