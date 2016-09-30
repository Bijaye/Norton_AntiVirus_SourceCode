// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPI/VCS/instr2.cpv   1.1   21 May 1998 20:30:58   MKEATIN  $
//
// Description:
//
//  This source file contains routines for approximately half of the opcodes
//  supported by PAM's CPU emulator.
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVAPI/VCS/instr2.cpv  $
// 
//    Rev 1.1   21 May 1998 20:30:58   MKEATIN
// Changed pamapi.h to pamapi_l.h
//
//    Rev 1.0   21 May 1998 19:23:40   MKEATIN
// Initial revision.
//
//    Rev 1.36   11 Apr 1996 14:36:58   CNACHEN
// Fixed bug with ERS.  On INT 21, AH=40, CX=0 (truncate), a random value
// was being returned in AX instead of 0.
//
//    Rev 1.35   15 Mar 1996 13:27:20   CNACHEN
// Added INVALID_INDEX exclusions...
//
//    Rev 1.34   05 Mar 1996 14:01:28   CNACHEN
// Added error checking for repair stuff...
//
//    Rev 1.33   07 Feb 1996 13:32:52   CNACHEN
// Fixed 32-bit (ints) to (short ints)
//
//    Rev 1.32   02 Feb 1996 11:44:24   CNACHEN
// Added new dwFlags and revamped all exclusion checking...
//
//
//    Rev 1.31   01 Feb 1996 10:08:50   CNACHEN
// Made sure REPNZ SCAS/STOS/MOVS work during excluded regions when there are
// no_candidates_left...
//
//    Rev 1.30   31 Jan 1996 17:51:42   CNACHEN
// Changed code so it will properly perform REP * during getsig.
//
//
//    Rev 1.29   25 Jan 1996 14:15:52   DCHI
// Fixes to SHLD and SHRD.
//
//    Rev 1.28   23 Jan 1996 14:48:34   DCHI
// Properly deal with immunity counter.
//
//    Rev 1.27   22 Jan 1996 13:40:34   CNACHEN
// Added new fault support.
//
//
//    Rev 1.26   20 Dec 1995 12:19:32   CNACHEN
// Added 0F,AF, 69, 6B IMUL instructions...
//
//    Rev 1.25   19 Dec 1995 19:08:02   CNACHEN
// Added prefetch queue support!
//
//
//    Rev 1.24   14 Dec 1995 10:49:36   CNACHEN
// Fixed repair stuff...
//
//    Rev 1.23   14 Dec 1995 10:03:36   CNACHEN
// Fixed flags for interrupts (into) and initialization
//
//    Rev 1.22   13 Dec 1995 20:15:28   CNACHEN
// Fixed bug in INT calls so interrupt flag is disabled on int call...
//
//    Rev 1.21   13 Dec 1995 13:41:28   CNACHEN
// Changed LONG's to longs for NLM platform!
//
//    Rev 1.20   13 Dec 1995 11:58:06   CNACHEN
// All File and Memory functions now use #defined versions with PAM prefixes
//
//    Rev 1.19   13 Dec 1995 10:14:22   CNACHEN
// Fixed NLM problems...
//
//    Rev 1.18   12 Dec 1995 10:27:50   CNACHEN
// Fixed SAHF instruction.
//
//    Rev 1.17   20 Nov 1995 10:12:06   CNACHEN
//
//
//    Rev 1.16   20 Nov 1995 10:06:26   CNACHEN
// Fixed BT bug that James found.
//
//    Rev 1.15   16 Nov 1995 10:44:00   CNACHEN
// Fixed bug with RCR reg/mem, 1
//
//    Rev 1.14   15 Nov 1995 20:55:48   CNACHEN
// Fixed bug in SBB A?, IMMED instruction...
//
//    Rev 1.13   15 Nov 1995 20:40:30   CNACHEN
// Fixed parity problems..
//
//    Rev 1.12   15 Nov 1995 17:45:26   CNACHEN
// Added additional handling for GETSIG.
//
//    Rev 1.11   15 Nov 1995 17:26:24   DCHI
// AND all shift counts with 0x1F.
//
//    Rev 1.10   10 Nov 1995 09:58:50   CNACHEN
// Fixed exclusion on INT 21, AH > 7F
//
//    Rev 1.9   23 Oct 1995 10:57:08   CNACHEN
// Changed ARPL fault for repair to ARPL 0x90, 0xFF to avoid confusion...
//
//    Rev 1.8   19 Oct 1995 18:23:32   CNACHEN
// Initial revision... with comment header :)
//
//************************************************************************


#include "pamapi_l.h"


//#define BUILD_SET

/* 386 */

void oc01001reg(PAMLHANDLE hLocal, BYTE opcode)     /* dec 16 bit register */
{
    DWORD dwreg, dwscratch;
    WORD wSize;

    if (hLocal->CPU.op_size_over)
        wSize = DWORD_SIZE;
    else
        wSize = WORD_SIZE;

    dwreg = reg_value(hLocal, wSize, (WORD)(opcode & 0x7));  /* reg#=lw 3 bts */
    dwscratch = dwreg - 1;

    set_over_flag_sub(hLocal, wSize, dwreg, 1, 0);
    set_zero_flag(hLocal, wSize, dwscratch);
    set_sign_flag(hLocal, wSize, dwscratch);
	dwset_aux_carry_flag_sub(hLocal,dwreg,1,0);
    set_parity_flag(hLocal, wSize, dwscratch);

    // make sure to ignore all DEC modifications to index registers
    // for invalid index testing.

    hLocal->CPU.wIndexStatus |= VALID_INDEX_IGNORE;
    set_reg(hLocal, wSize, (WORD)(opcode & 0x7), dwscratch);
    hLocal->CPU.wIndexStatus &= ~(WORD)VALID_INDEX_IGNORE;
}


/* mult 2 32-bit #'s and get a 64-bit result in hi,lo */

void MUL_32(DWORD a, DWORD b, DWORD *hi, DWORD *lo)
{
	DWORD a_hi, a_lo, b_hi, b_lo;
	DWORD dwlow_sum, dwhi_sum, co_low;

	a_hi = a>>16;
	a_lo = a&0xFFFFU;

	b_hi = b>>16;
	b_lo = b&0xFFFFU;

	dwlow_sum = b_lo * a_lo + ((b_lo * a_hi) << 16);
	dwlow_sum += ((b_hi * a_lo) << 16);

	dwhi_sum  = ((b_lo * a_hi) >> 16) + ((b_hi * a_lo) >> 16);

	dwhi_sum += b_hi * a_hi;

	co_low = ((b_lo * a_hi) & 0xFFFFLU) +
			 ((b_hi * a_lo) & 0xFFFFLU) +
			 ((a_lo * b_lo) >> 16);

	co_low >>= 16;

	dwhi_sum += co_low;

	*hi = dwhi_sum;
	*lo = dwlow_sum;
}

long wwimul(short int a, short int b)
{
	long sign = 0, result;

    if (a < 0 && (WORD)a !=0x8000U)
	{
		sign = 1;
        a = -a;
	}

    if (b < 0 && (WORD)b != 0x8000U)
	{
		sign ^= 1;
		b = -b;
	}

	result = (long)a*b;
	if (sign)
		return(-result);

	return(result);
}

short int bbimul(signed char a, signed char b)
{
    short int sign = 0, result;

    if (a < 0 && (BYTE)a !=0x80U)
	{
		sign = 1;
        a = -a;
	}

	if (b < 0 && (BYTE)b != 0x80U)
	{
		sign ^= 1;
		b = -b;
	}

    result = (short int)a*b;
	if (sign)
		return(-result);

	return(result);
}


void dwimul(long a, long b, long *hi, long *lo)
{
	long sign = 0;

    if (a < 0 && (DWORD)a !=0x80000000L)
	{
		sign = 1;
        a = -a;
	}

    if (b < 0 && (DWORD)b != 0x80000000L)
	{
		sign ^= 1;
		b = -b;
	}

	MUL_32(a,b,(DWORD *)hi,(DWORD *)lo);

    if (sign)       // 2's complement them : hi and lo
    {
        *hi = ~*hi;
		*lo = ~*lo + 1;

        if (*lo == 0)
            (*hi) ++;
    }
}

/* 386 */

void oc1111011w(PAMLHANDLE hLocal, BYTE opcode)
{
    BYTE        amb, mem_type, bdiv, bquo;
    WORD        seg, off, reg;
    WORD        wdiv, wquo;
	short int   idiv, iquo;
    long        ldiv, lquo;
    DWORD       uldiv, ulquo;
    DWORD       dwvalue, dwnew;

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */

    switch (SEC_OP(amb))
    {
        case 0:                 /* 000: TEST reg/mem, immed */

            /* 386 */

            decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

            dwvalue = gen_get_value(hLocal,mem_type,seg,off);
            dwnew   = get_value_at_csip(hLocal,mem_type);

            dwvalue &= dwnew;

            set_sign_flag(hLocal,mem_type,dwvalue);
			set_zero_flag(hLocal,mem_type,dwvalue);
            set_parity_flag(hLocal, mem_type, dwvalue);

            hLocal->CPU.FLAGS.O = hLocal->CPU.FLAGS.C = 0;      /* by definition */
            break;


		case 1:

            //printf("??? instruction used!\n");

			// this is an ??? instruction, so remove operands from the
			// instruction stream and continue...

            decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			// this F6 XX00111W is a reserved intel opcode and always
			// skips over the next byte in the instruction stream!

            // F7 XX001??? skips over either WORD or DW

            get_value_at_csip(hLocal,mem_type);

			break;

		case 2:                         /* 010: NOT reg/mem */

            /* 386 */

            decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

            dwvalue = ~gen_get_value(hLocal,mem_type,seg,off);
            gen_put_value(hLocal,mem_type,seg,off,dwvalue);

            /* no flags affected */

			break;
        case 3:                         /* 011: NEG reg/mem */

            /* 386 */

            decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

            dwvalue = gen_get_value(hLocal, mem_type,seg,off);
            dwnew = -(long)dwvalue;
            gen_put_value(hLocal, mem_type,seg,off,dwnew);

            set_over_flag_sub(hLocal, mem_type, 0, dwvalue, 0);
            set_carry_flag_sub(hLocal, mem_type,0, dwvalue, 0);
			set_zero_flag(hLocal,mem_type,dwnew);
			set_sign_flag(hLocal,mem_type,dwnew);
            set_parity_flag(hLocal, mem_type, dwnew);
            break;

        case 4:                         /* 100: mult reg/mem */

            /* 386 */

            decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

            if (W_VALUE(opcode) == 0)   /* 8 bit */
            {
                wdiv = hLocal->CPU.preg.H.AL;
                wquo = bget_value(hLocal, mem_type,seg,off);

                hLocal->CPU.preg.X.AX = wdiv * wquo;

                if (hLocal->CPU.preg.H.AH == 0)
                    hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = FALSE;
                else
                    hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = TRUE;
            }
			else if (mem_type & WORD_SIZE)
            {                           /* 16 bit */
				ulquo = wget_value(hLocal, mem_type,seg,off);
                uldiv = ulquo * hLocal->CPU.preg.X.AX;

                ulquo = uldiv >> 16;
				hLocal->CPU.preg.X.DX = (WORD)(ulquo & 0xffff);
                hLocal->CPU.preg.X.AX = (WORD)(uldiv & 0xffff);

                if (hLocal->CPU.preg.X.DX == 0)
                    hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = FALSE;
                else
                    hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = TRUE;
            }
            else
            {                           /* 32 bit */
				ulquo = dwget_value(hLocal, mem_type,seg,off);
				uldiv = hLocal->CPU.preg.D.EAX;

                MUL_32(ulquo,
                       uldiv,
                       &(hLocal->CPU.preg.D.EDX),
                       &(hLocal->CPU.preg.D.EAX));

                if (hLocal->CPU.preg.D.EDX == 0)
                    hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = FALSE;
                else
                    hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = TRUE;
            }
            break;

		case 5:                         /* 101: imult reg/mem */

			/* 386 */

            decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);
            if (W_VALUE(opcode) == 0)   /* 8 bit */
            {
				bdiv = hLocal->CPU.preg.H.AL;
                bquo = bget_value(hLocal, mem_type,seg,off);

				hLocal->CPU.preg.X.AX = (WORD)bbimul(bdiv, bquo);

                if (bsign_status(hLocal->CPU.preg.H.AL) == 0 &&
                    hLocal->CPU.preg.H.AH == 0)
                    hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = FALSE;
                else if (bsign_status(hLocal->CPU.preg.H.AL) == 1 &&
                    hLocal->CPU.preg.H.AH == 0xff)
                    hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = TRUE;
            }
			else  if (mem_type & WORD_SIZE)
			{                           /* 16 bit */
				wquo = wget_value(hLocal, mem_type,seg,off);
				wdiv = hLocal->CPU.preg.X.AX;

				dwvalue = wwimul(wquo, wdiv);

				ulquo = dwvalue >> 16;
				hLocal->CPU.preg.X.DX = (WORD)(ulquo & 0xffffL);
				hLocal->CPU.preg.X.AX = (WORD)(dwvalue & 0xffffL);

				if (wsign_status(hLocal->CPU.preg.X.AX) == 0 &&
					hLocal->CPU.preg.X.DX == 0)
					hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = FALSE;
				else if (wsign_status(hLocal->CPU.preg.X.AX) == 1 &&
					hLocal->CPU.preg.X.DX == 0xffff)
					hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = TRUE;
			}
            else
            {
				ulquo = dwget_value(hLocal, mem_type,seg,off);
				uldiv = hLocal->CPU.preg.D.EAX;

				dwimul(ulquo,
					   uldiv,
					   (long *)&(hLocal->CPU.preg.D.EDX),
					   (long *)&(hLocal->CPU.preg.D.EAX));

				if (dsign_status(hLocal->CPU.preg.D.EAX) == 0 &&
					hLocal->CPU.preg.D.EDX == 0)
					hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = FALSE;
				else if (dsign_status(hLocal->CPU.preg.D.EAX) == 1 &&
					hLocal->CPU.preg.D.EDX == 0xffffffffL)
					hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = TRUE;
			}

			break;
		case 6:                     /* 110: DIV mem/reg */

            decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);
			if (W_VALUE(opcode) == 0)   /* 8 bit */
			{
				wdiv = hLocal->CPU.preg.X.AX;
				wquo = bget_value(hLocal, mem_type,seg,off);

				if (wquo == 0)
					return;             /* divide by 0 */

                hLocal->CPU.preg.H.AL = (BYTE)(wdiv / wquo);
                hLocal->CPU.preg.H.AH = (BYTE)(wdiv % wquo);

			}
			else if (mem_type & WORD_SIZE)
			{                           /* 16 bit */
                uldiv = ((DWORD)hLocal->CPU.preg.X.DX << 16);
				uldiv += hLocal->CPU.preg.X.AX;
				ulquo = wget_value(hLocal, mem_type,seg,off);

				if (ulquo == 0)
					return;             /* divide by 0 */

                hLocal->CPU.preg.X.AX = (WORD)(uldiv / ulquo);
                hLocal->CPU.preg.X.DX = (WORD)(uldiv % ulquo);
			}
			else
			{
			}
			break;

		case 7:                     /* 111: IDIV mem/reg */

            decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);
			if (W_VALUE(opcode) == 0)   /* 8 bit */
			{
				idiv = (short)hLocal->CPU.preg.X.AX;
				iquo = (short)(signed char)bget_value(hLocal, mem_type,seg,off);

				if (iquo == 0)
					return;             /* divide by 0 */

				hLocal->CPU.preg.H.AL = (BYTE)(idiv / iquo);
				hLocal->CPU.preg.H.AH = (BYTE)(idiv % iquo);

			}
			else if (mem_type & WORD_SIZE)
			{                           /* 16 bit */
                uldiv = ((DWORD)hLocal->CPU.preg.X.DX << 16);
                uldiv += hLocal->CPU.preg.X.AX;
                ldiv = (long)uldiv;
                lquo = (long)(signed short int)wget_value(hLocal, mem_type,seg,off);

                if (lquo == 0)
					return;             /* divide by 0 */

                hLocal->CPU.preg.X.AX = (WORD)(ldiv / lquo);
                hLocal->CPU.preg.X.DX = (WORD)(ldiv % lquo);
			}
			else
			{
			}

            break;
	}
}

/* 386 */

void oc01000reg(PAMLHANDLE hLocal, BYTE opcode)     /* inc 16 bit register */
{
    DWORD dwreg, dwscratch;
    WORD wMemType;

    if (hLocal->CPU.op_size_over)
        wMemType = DWORD_SIZE;
    else
        wMemType = WORD_SIZE;

    dwreg = reg_value(hLocal, wMemType, (WORD)(opcode & 0x7));  /* lower 3 bits is reg # */
    dwscratch = dwreg + 1;

    set_over_flag_add(hLocal,wMemType, dwreg, 1, 0);
    set_zero_flag(hLocal,wMemType,dwscratch);
    set_sign_flag(hLocal,wMemType,dwscratch);
	dwset_aux_carry_flag_add(hLocal,dwreg,1,0);
    set_parity_flag(hLocal, wMemType, dwscratch);

    // make sure to ignore all INC modifications to index registers
    // for invalid index testing.

    hLocal->CPU.wIndexStatus |= VALID_INDEX_IGNORE;
    set_reg(hLocal, wMemType, (WORD)(opcode & 0x7), dwscratch);
    hLocal->CPU.wIndexStatus &= ~(WORD)VALID_INDEX_IGNORE;
}

/* 386 */

void oc0111cccc(PAMLHANDLE hLocal, BYTE opcode)     /* short conditional jump */
{
    WORD woff;

    woff = sign_extend_byte(get_byte_at_csip(hLocal));

    switch (opcode)
    {
        case JA_SHORT:
            if (hLocal->CPU.FLAGS.C == FALSE && hLocal->CPU.FLAGS.Z == FALSE)
            {
                hLocal->CPU.IP += woff;
                reset_prefetch_queue(hLocal);
            }
            break;
        case JAE_SHORT:
            if (hLocal->CPU.FLAGS.C == FALSE)
            {
                hLocal->CPU.IP += woff;
                reset_prefetch_queue(hLocal);
            }
            break;
        case JC_SHORT:
            if (hLocal->CPU.FLAGS.C == TRUE)
            {
                hLocal->CPU.IP += woff;
                reset_prefetch_queue(hLocal);
            }
            break;
        case JBE_SHORT:
            if (hLocal->CPU.FLAGS.C == TRUE || hLocal->CPU.FLAGS.Z == TRUE)
            {
                hLocal->CPU.IP += woff;
                reset_prefetch_queue(hLocal);
            }
            break;
        case JZ_SHORT:
            if (hLocal->CPU.FLAGS.Z == TRUE)
            {
                hLocal->CPU.IP += woff;
                reset_prefetch_queue(hLocal);
            }
            break;
        case JG_SHORT:
            if (hLocal->CPU.FLAGS.Z == FALSE && hLocal->CPU.FLAGS.S == hLocal->CPU.FLAGS.O)
            {
                hLocal->CPU.IP += woff;
                reset_prefetch_queue(hLocal);
            }
            break;
        case JGE_SHORT:
            if (hLocal->CPU.FLAGS.S == hLocal->CPU.FLAGS.O)
            {
                hLocal->CPU.IP += woff;
                reset_prefetch_queue(hLocal);
            }
            break;
        case JL_SHORT:
            if (hLocal->CPU.FLAGS.S != hLocal->CPU.FLAGS.O)
            {
                hLocal->CPU.IP += woff;
                reset_prefetch_queue(hLocal);
            }
            break;
        case JLE_SHORT:
            if (hLocal->CPU.FLAGS.Z == TRUE || hLocal->CPU.FLAGS.S != hLocal->CPU.FLAGS.O)
            {
                hLocal->CPU.IP += woff;
                reset_prefetch_queue(hLocal);
            }
            break;
        case JNZ_SHORT:
            if (hLocal->CPU.FLAGS.Z == FALSE)
            {
                hLocal->CPU.IP += woff;
                reset_prefetch_queue(hLocal);
            }
            break;
        case JNO_SHORT:
            if (hLocal->CPU.FLAGS.O == FALSE)
            {
                hLocal->CPU.IP += woff;
                reset_prefetch_queue(hLocal);
            }
            break;
        case JO_SHORT:
            if (hLocal->CPU.FLAGS.O == TRUE)
            {
                hLocal->CPU.IP += woff;
                reset_prefetch_queue(hLocal);
            }
            break;
        case JS_SHORT:
            if (hLocal->CPU.FLAGS.S == TRUE)
            {
                hLocal->CPU.IP += woff;
                reset_prefetch_queue(hLocal);
            }
            break;
        case JNS_SHORT:
            if (hLocal->CPU.FLAGS.S == FALSE)
            {
                hLocal->CPU.IP += woff;
                reset_prefetch_queue(hLocal);
            }
            break;
        case JPE_SHORT:
            if (hLocal->CPU.FLAGS.P == TRUE)
            {
                hLocal->CPU.IP += woff;
                reset_prefetch_queue(hLocal);
            }
            break;
        case JPO_SHORT:
            if (hLocal->CPU.FLAGS.P == FALSE)
            {
                hLocal->CPU.IP += woff;
                reset_prefetch_queue(hLocal);
            }
            break;
    }


}

/* 386 */

void oc11100011(PAMLHANDLE hLocal)       /* jcxz */
{
    WORD woff;

    woff = sign_extend_byte(get_byte_at_csip(hLocal));

    if (hLocal->CPU.op_size_over)
    {
        if (hLocal->CPU.preg.D.ECX == 0)
        {
            hLocal->CPU.IP += woff;
            reset_prefetch_queue(hLocal);
        }
    }
    else
    {
        if (hLocal->CPU.preg.X.CX == 0)
        {
            hLocal->CPU.IP += woff;
            reset_prefetch_queue(hLocal);
        }
    }
}

/* 386 */

void oc11101011(PAMLHANDLE hLocal)       /* jmp disp8 (short direct) */
{
    WORD woff;

    woff = sign_extend_byte(get_byte_at_csip(hLocal));
    hLocal->CPU.IP += woff;

    reset_prefetch_queue(hLocal);
}

/* 386 - no change */

void oc11101010(PAMLHANDLE hLocal)       /* jmp far direct */
{
	WORD wip, wcs;

    wip = get_word_at_csip(hLocal);
    wcs = get_word_at_csip(hLocal);

    hLocal->CPU.IP = wip;
    hLocal->CPU.CS = wcs;

    reset_prefetch_queue(hLocal);
}

/* 386 OK */

void oc10011111(PAMLHANDLE hLocal)       /* lahf */
{
    BYTE flags = 0;

    if (hLocal->CPU.FLAGS.S == TRUE)
        flags |= 0x80;

    if (hLocal->CPU.FLAGS.Z == TRUE)
        flags |= 0x40;

    if (hLocal->CPU.FLAGS.C == TRUE)
        flags |= 1;

    if (hLocal->CPU.FLAGS.A == TRUE)
        flags |= 0x10;

    if (hLocal->CPU.FLAGS.P == TRUE)
        flags |= 4;

    hLocal->CPU.preg.H.AH = flags;
}


/* 386 OK */

void oc10011110(PAMLHANDLE hLocal)       /* sahf */
{
    BYTE flags = 0;

    flags = hLocal->CPU.preg.H.AH;

    hLocal->CPU.FLAGS.S = !!(flags & 0x80);

    hLocal->CPU.FLAGS.Z = !!(flags & 0x40);

    hLocal->CPU.FLAGS.C = !!(flags & 0x01);

    hLocal->CPU.FLAGS.A = !!(flags & 0x10);

    hLocal->CPU.FLAGS.P = !!(flags & 0x04);
}

/* 386 */

void oc11000101(PAMLHANDLE hLocal)                      /* lds */
{
    LSREG(hLocal,REG_DS);
}


/* 386 */

void oc10001101(PAMLHANDLE hLocal, BYTE opcode)                    /* lea */
{
    BYTE amb, mem_type;
    WORD seg, off, reg;
    DWORD dwoff;

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */

    if (hLocal->CPU.addr_size_over)
		decode_amb_32_off(hLocal, opcode, amb, &mem_type, &dwoff, &reg);
	else
	{
		decode_amb_norm(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);
		dwoff = (DWORD)off;
    }

    set_reg(hLocal, mem_type, reg, dwoff);
}

/* 386 */

void oc11000100(PAMLHANDLE hLocal)        /* les */
{
    LSREG(hLocal,REG_ES);
}


void oc1010110w(PAMLHANDLE hLocal, BYTE opcode)            /* LODS */
{
    DWORD dw;
    WORD  wMemSize, wSkip;

	if (hLocal->CPU.rep_prefix)
	{
		rep_instruction(hLocal, oc1010110w, opcode);
		return;
	}

    if (W_VALUE(opcode) == 0)
    {
        wMemSize = 0;
        wSkip = sizeof(BYTE);
    }
    else if (hLocal->CPU.op_size_over)
    {
        wMemSize = DWORD_SIZE;
        wSkip = sizeof(DWORD);
    }
    else
    {
        wMemSize = WORD_SIZE;
        wSkip = sizeof(WORD);
    }

    /* since all memory addressing is limited to 16 bit addresses, we can
       use X.SI instead of D.ESI */

    dw = gen_get_value(hLocal,
                       wMemSize,
					   seg_to_use(hLocal,hLocal->CPU.DS),
                       hLocal->CPU.ireg.X.SI);

    if (hLocal->CPU.FLAGS.D == 0)
        hLocal->CPU.ireg.X.SI += wSkip;
    else
        hLocal->CPU.ireg.X.SI -= wSkip;

    /* REG_AL == REG_AX == REG_EAX */

    set_reg(hLocal,wMemSize,REG_AL,dw);
}

/* 386 */

void oc11100010(PAMLHANDLE hLocal)               /* loop */
{
    WORD woff;

    /* NOTE: remember to compensate for dummy loops by subtracting out
             1 for loop back's of 2 bytes or less */

    woff = sign_extend_byte(get_byte_at_csip(hLocal));

    if (hLocal->CPU.addr_size_over)
    {
        hLocal->CPU.preg.D.ECX--;                /* decrement ECX */

        if (hLocal->CPU.preg.D.ECX != 0)
        {
            hLocal->CPU.IP += woff;

            reset_prefetch_queue(hLocal);
        }
    }
    else
    {
        hLocal->CPU.preg.X.CX--;                /* decrement CX */

        if (hLocal->CPU.preg.X.CX != 0)
        {
            hLocal->CPU.IP += woff;

            reset_prefetch_queue(hLocal);
        }
    }
}


/* 386 */

void oc11100001(PAMLHANDLE hLocal)               /* loopz */
{
    WORD woff;

    /* NOTE: remember to compensate for dummy loops by subtracting out
             1 for loop back's of 2 bytes or less */

    woff = sign_extend_byte(get_byte_at_csip(hLocal));

    if (hLocal->CPU.addr_size_over)
    {
        hLocal->CPU.preg.D.ECX--;                /* decrement ECX */

        if (hLocal->CPU.preg.D.ECX != 0 && hLocal->CPU.FLAGS.Z == TRUE)
        {
            hLocal->CPU.IP += woff;

            reset_prefetch_queue(hLocal);
        }
    }
    else
    {
        hLocal->CPU.preg.X.CX--;                /* decrement CX */

        if (hLocal->CPU.preg.X.CX != 0 && hLocal->CPU.FLAGS.Z == TRUE)
        {
            hLocal->CPU.IP += woff;

            reset_prefetch_queue(hLocal);
        }
    }
}

/* 386 */

void oc11100000(PAMLHANDLE hLocal)               /* loopnz */
{
    WORD woff;

    /* NOTE: remember to compensate for dummy loops by subtracting out
             1 for loop back's of 2 bytes or less */

    woff = sign_extend_byte(get_byte_at_csip(hLocal));

    if (hLocal->CPU.addr_size_over)
    {
        hLocal->CPU.preg.D.ECX--;                /* decrement ECX */

        if (hLocal->CPU.preg.D.ECX != 0 && hLocal->CPU.FLAGS.Z != TRUE)
        {
            hLocal->CPU.IP += woff;

            reset_prefetch_queue(hLocal);
        }
    }
    else
    {
        hLocal->CPU.preg.X.CX--;                /* decrement CX */

        if (hLocal->CPU.preg.X.CX != 0 && hLocal->CPU.FLAGS.Z != TRUE)
        {
            hLocal->CPU.IP += woff;

            reset_prefetch_queue(hLocal);
        }
    }
}

/* 386 */

void oc100010dw(PAMLHANDLE hLocal, BYTE opcode)        /* MOV reg/mem, reg/mem */
{
    BYTE amb, mem_type;
    WORD seg, off, reg;
    DWORD dwmem, dwreg;

    /* if D = 0 then REG field is source and dest will be memory/register.
       if D = 1 then dest is a register (specified by REG) and source may
       be memory or another register */

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */
    decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

    if (D_VALUE(opcode) == 0)
    {
        dwreg = reg_value(hLocal,mem_type, reg);
        gen_put_value(hLocal,mem_type,seg,off,dwreg);
    }
    else            /* D == 1 */
    {
        dwmem = gen_get_value(hLocal,mem_type,seg,off);
        set_reg(hLocal,mem_type,reg,dwmem);
    }
}


/* 386 */

void oc1011wreg(PAMLHANDLE hLocal, BYTE opcode)        /* mov reg, immed */
{

    if ((opcode & 0x8) == 0)            /* w bit =0 means 8 bit*/
        set_breg(hLocal, (BYTE)(opcode & 0x7),get_byte_at_csip(hLocal));
    else
    {                                   /* word/dword registers */
        if (hLocal->CPU.op_size_over)
            set_dwreg(hLocal, (WORD)(opcode & 0x7),get_dword_at_csip(hLocal));
        else
            set_wreg(hLocal, (WORD)(opcode & 0x7),get_word_at_csip(hLocal));
    }
}

/* 386 */

void oc1100011w(PAMLHANDLE hLocal, BYTE opcode)            /* MOV reg/mem, immediate */
{
    BYTE amb, mem_type;
    WORD seg, off, reg;

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */
    decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

    gen_put_value(hLocal,
                  mem_type,
                  seg,
                  off,
                  get_value_at_csip(hLocal,mem_type));
}


/* 386 */

void oc1010000w(PAMLHANDLE hLocal, BYTE opcode)        /* mov accum, [wimmed] */
{
    WORD woff;

    woff = get_word_at_csip(hLocal);      /* offset */

    if (W_VALUE(opcode) == 0)       /* byte */
		hLocal->CPU.preg.H.AL =
			get_byte(hLocal, seg_to_use(hLocal, hLocal->CPU.DS),woff);
	else
    {
        if (hLocal->CPU.op_size_over)
            hLocal->CPU.preg.D.EAX =
                get_dword(hLocal, seg_to_use(hLocal, hLocal->CPU.DS),woff);
        else

            hLocal->CPU.preg.X.AX =
                get_word(hLocal, seg_to_use(hLocal, hLocal->CPU.DS),woff);
    }
}

/* 386 */

void oc1010001w(PAMLHANDLE hLocal, BYTE opcode)        /* mov mem, accum */
{
    WORD woff;

    woff = get_word_at_csip(hLocal);      /* offset */

    if (W_VALUE(opcode) == 0)       /* byte */
		put_byte(hLocal, seg_to_use(hLocal, hLocal->CPU.DS),woff,hLocal->CPU.preg.H.AL);
	else
    {
        if (hLocal->CPU.op_size_over)
            put_dword(hLocal,
                      seg_to_use(hLocal,hLocal->CPU.DS),woff,
                      hLocal->CPU.preg.D.EAX);
        else
            put_word(hLocal, seg_to_use(hLocal, hLocal->CPU.DS),woff,hLocal->CPU.preg.X.AX);
    }
}


/* 386 */

void oc10001110(PAMLHANDLE hLocal, BYTE opcode)        /* MOV sreg, reg/mem */
{
    BYTE amb, mem_type;
    WORD seg, off, reg;

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */
    decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

    set_sreg(hLocal,(BYTE)reg,wget_value(hLocal, (BYTE)mem_type,seg,off));
}


/* 386 */

void oc10001100(PAMLHANDLE hLocal, BYTE opcode)        /* MOV reg/mem, sreg */
{
    BYTE amb, mem_type;
    WORD seg, off, reg;

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */
    decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

    wput_value(hLocal, (BYTE)mem_type,seg,off,get_sreg(hLocal, (BYTE)reg));
}


/* 386 */

void oc1010010w(PAMLHANDLE hLocal, BYTE opcode)        /* movs */
{
    WORD  wMemSize, wSkip;
    DWORD dw;

	if (hLocal->CPU.rep_prefix)
	{
		exclude_item(hLocal, REP_MOVS);                 /* exclude the rep movs */

#ifndef BUILD_SET

        // only return if we're not in immunity mode of some sort

        if ((hLocal->dwFlags & LOCAL_FLAG_NO_CAND_LEFT) &&
            !(hLocal->dwFlags & (LOCAL_FLAG_IMMUNE |
                                 LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
			return;
#endif

		rep_instruction(hLocal, oc1010010w, opcode);
		return;
	}

    if (W_VALUE(opcode) == 0)
    {
        wMemSize = 0;
        wSkip = sizeof(BYTE);
    }
    else if (hLocal->CPU.op_size_over)
    {
        wMemSize = DWORD_SIZE;
        wSkip = sizeof(DWORD);
    }
    else
    {
        wMemSize = WORD_SIZE;
        wSkip = sizeof(WORD);
    }

    /* since all memory addressing is limited to 16 bit addresses, we can
       use X.SI instead of D.ESI */

    dw = gen_get_value(hLocal,
                       wMemSize,
                       seg_to_use(hLocal, hLocal->CPU.DS),
                       hLocal->CPU.ireg.X.SI);              /* DS:SI default */

    gen_put_value(hLocal,
                  wMemSize,
                  hLocal->CPU.ES,
                  hLocal->CPU.ireg.X.DI,dw);                /* always to ES:DI */

    if (hLocal->CPU.FLAGS.D == 0)
    {
        hLocal->CPU.ireg.X.SI += wSkip;
        hLocal->CPU.ireg.X.DI += wSkip;
    }
    else
    {
        hLocal->CPU.ireg.X.SI -= wSkip;
        hLocal->CPU.ireg.X.DI -= wSkip;
    }
}

/* 386 */

void oc000010dw(PAMLHANDLE hLocal, BYTE opcode)            /* OR reg/mem, reg/mem */
{
	BYTE amb, mem_type;
	WORD seg, off, reg;
	DWORD dwscratch, dwreg, dwmem;

    /* if D = 0 then REG field is source and dest will be memory/register.
       if D = 1 then dest is a register (specified by REG) and source may
       be memory or another register */

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */
    decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

    dwreg = reg_value(hLocal,mem_type,reg);
    dwmem = gen_get_value(hLocal,mem_type,seg,off);

    dwscratch = dwmem | dwreg;

    if (D_VALUE(opcode) == 0)
        gen_put_value(hLocal,mem_type,seg,off,dwscratch);
    else
        set_reg(hLocal, mem_type, reg, dwscratch);

    /* set OF and CF to 0 regardless */

    hLocal->CPU.FLAGS.O = hLocal->CPU.FLAGS.C = 0;      /* by definition */

    /* flag stuff */

    set_sign_flag(hLocal,mem_type,dwscratch);
    set_zero_flag(hLocal,mem_type,dwscratch);
    set_parity_flag(hLocal, mem_type, dwscratch);
}


/* 386 */

void oc0000110w(PAMLHANDLE hLocal, BYTE opcode)        /* OR accum, immed */
{
    DWORD dwscratch, dwreg, dwmem;
    WORD wMemSize;

    if (W_VALUE(opcode) == 0)
        wMemSize = 0;
    else if (hLocal->CPU.op_size_over)
        wMemSize = DWORD_SIZE;
    else
        wMemSize = WORD_SIZE;

    /* REG_AL = REG_AX = REG_EAX */

    dwreg = reg_value(hLocal, wMemSize, REG_AL);
    dwmem = get_value_at_csip(hLocal,wMemSize);

    dwscratch = dwmem | dwreg;

	set_reg(hLocal, wMemSize, REG_AL,dwscratch);

    set_sign_flag(hLocal, wMemSize, dwscratch);
    set_zero_flag(hLocal, wMemSize, dwscratch);
    set_parity_flag(hLocal, wMemSize, dwscratch);

    /* set OF and CF to 0 regardless */

    hLocal->CPU.FLAGS.O = hLocal->CPU.FLAGS.C = 0;      /* by definition */
}


/* 386 */
/* pop to 16/32 bit register */

void oc01011reg(PAMLHANDLE hLocal, BYTE opcode)
{
    DWORD dw;
    WORD wMemSize;

    if (hLocal->CPU.op_size_over)
		wMemSize = DWORD_SIZE;
    else
        wMemSize = WORD_SIZE;

	dw = pop_value(hLocal,wMemSize);
    set_reg(hLocal, wMemSize, (WORD)(opcode & 0x7),dw);  /* set proper register */
}

/* 386 */

/* pop into memory (d)word */

// this function is not supported (via the LIVT) for ERS overlay or foundation
// functions.

void oc10001111(PAMLHANDLE hLocal, BYTE opcode)
{
	BYTE amb, mem_type;
	WORD seg, off, reg;
    DWORD dwVal;

    if (hLocal->CPU.op_size_over)
        mem_type = DWORD_SIZE;
    else
        mem_type = WORD_SIZE;

    dwVal = pop_value(hLocal,mem_type);

    // make sure that if we are doing a POP [ESP+???], that we first pop
    // our value from the stack, update the ESP, and then finally we
    // do the addressing afterwards.

	amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */

	decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

    gen_put_value(hLocal, mem_type,seg,off,dwVal);
}

/* 386 */

void oc00sreg111(PAMLHANDLE hLocal, BYTE opcode)           /* pop sreg */
{
	WORD wMemSize;

	if (hLocal->CPU.op_size_over || hLocal->CPU.addr_size_over)
		wMemSize = DWORD_SIZE;
	else
		wMemSize = WORD_SIZE;

    set_sreg(hLocal, (BYTE)((opcode >> 3) & 0x7), (WORD)pop_value(hLocal,wMemSize));
}


/* 386 */

void oc10011101(PAMLHANDLE hLocal)                   /* popf */
{
    WORD w;

    if (hLocal->CPU.op_size_over)
        w = (WORD)pop_dword(hLocal) & 0xFFFFU;
    else
        w = pop_word(hLocal);

    if (w & 0x01)
        hLocal->CPU.FLAGS.C = TRUE;
    else
        hLocal->CPU.FLAGS.C = FALSE;

    if (w & 0x04)
        hLocal->CPU.FLAGS.P = TRUE;
    else
        hLocal->CPU.FLAGS.P = FALSE;

    if (w & 0x10)
        hLocal->CPU.FLAGS.A = TRUE;
    else
        hLocal->CPU.FLAGS.A = FALSE;

    if (w & 0x40)
        hLocal->CPU.FLAGS.Z = TRUE;
    else
        hLocal->CPU.FLAGS.Z = FALSE;

    if (w & 0x80)
        hLocal->CPU.FLAGS.S = TRUE;
    else
        hLocal->CPU.FLAGS.S = FALSE;

    if (w & 0x100)
        hLocal->CPU.FLAGS.T = TRUE;
    else
        hLocal->CPU.FLAGS.T = FALSE;

    if (w & 0x200)
        hLocal->CPU.FLAGS.I = TRUE;
    else
        hLocal->CPU.FLAGS.I = FALSE;

    if (w & 0x400)
        hLocal->CPU.FLAGS.D = TRUE;
    else
        hLocal->CPU.FLAGS.D = FALSE;

    if (w & 0x800)
        hLocal->CPU.FLAGS.O = TRUE;
    else
        hLocal->CPU.FLAGS.O = FALSE;
}


void oc01010reg(PAMLHANDLE hLocal, BYTE opcode)            /* push 16 bit register */
{
    push_word(hLocal, wreg_value(hLocal, (WORD)(opcode & 0x7)));
}

void oc00sreg110(PAMLHANDLE hLocal, BYTE opcode)           /* push sreg */
{
    push_word(hLocal, get_sreg(hLocal, (BYTE)((opcode >> 3) & 0x7)));
}

/* 386 */

void oc10011100(PAMLHANDLE hLocal)                   /* pushf(d) */
{
    WORD w = 0;

    w |= 2;                                         /* 486: always set! */

    if (hLocal->CPU.FLAGS.C)
        w |= 0x1;

    if (hLocal->CPU.FLAGS.P)
        w |= 0x4;

    if (hLocal->CPU.FLAGS.A)
        w |= 0x10;

    if (hLocal->CPU.FLAGS.Z)
        w |= 0x40;

    if (hLocal->CPU.FLAGS.S)
        w |= 0x80;

    if (hLocal->CPU.FLAGS.T)
        w |= 0x100;

    if (hLocal->CPU.FLAGS.I)
        w |= 0x200;

    if (hLocal->CPU.FLAGS.D)
        w |= 0x400;

    if (hLocal->CPU.FLAGS.O)
        w |= 0x800;

    if (hLocal->CPU.op_size_over)
        push_dword(hLocal, (DWORD)w);
    else
        push_word(hLocal, w);
}







/* 386 */

void oc1101000w(PAMLHANDLE hLocal, BYTE opcode)            /* general purpose */
{
    BYTE amb, mem_type;
    WORD seg, off, reg;
    DWORD dw, dwc, dwlow;

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */

    switch (SEC_OP(amb))
	{
        case 0:                         /* 000: ROL reg/mem, 1 */
            decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

            dw  = gen_get_value(hLocal,mem_type,seg,off);
            dwc = sign_status(mem_type, dw);
            dw <<= 1;
            dw |= dwc;
			gen_put_value(hLocal,mem_type,seg,off,dw);

            if (sign_status(mem_type,dw) != dwc)
                hLocal->CPU.FLAGS.O = TRUE;         /* overflow! */
            else
                hLocal->CPU.FLAGS.O = FALSE;

            hLocal->CPU.FLAGS.C = (WORD)dwc;               /* thru carry */
            break;

		case 1:                         /* 001: ROR reg/mem, 1 */

            decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

            dw  = gen_get_value(hLocal,mem_type,seg,off);
			dwc = sign_status(mem_type, dw);

			dwlow= dw&1;
			dw >>= 1;

			if (dwlow)
			{
				if (mem_type & WORD_SIZE)
					dw |= 0x8000U;
				else if (mem_type & DWORD_SIZE)
					dw |= 0x80000000L;
				else
					dw |= 0x80;
			}

			gen_put_value(hLocal,mem_type,seg,off,dw);

			if (sign_status(mem_type,dw) != dwc)
				hLocal->CPU.FLAGS.O = TRUE;         /* overflow! */
			else
				hLocal->CPU.FLAGS.O = FALSE;

            hLocal->CPU.FLAGS.C = (WORD)dwlow;
			break;

		case 2:                         /* 010: RCL reg/mem, 1 */
			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			dw = gen_get_value(hLocal, mem_type, seg, off);
			dwc = sign_status(mem_type, dw);
			dw <<= 1;
			dw |= hLocal->CPU.FLAGS.C;

			gen_put_value(hLocal,mem_type,seg,off,dw);

			if (sign_status(mem_type, dw) != dwc)
				hLocal->CPU.FLAGS.O = TRUE;         /* overflow! */
			else
				hLocal->CPU.FLAGS.O = FALSE;

            hLocal->CPU.FLAGS.C = (WORD)dwc;               /* thru carry */
			break;

		case 3:                         /* 011: RCR reg/mem, 1 */

			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

            dw = gen_get_value(hLocal, mem_type, seg, off);
			dwc = sign_status(mem_type,dw);
			dwlow = dw & 1;
			dw >>= 1;

			if (hLocal->CPU.FLAGS.C)
			{
				if (mem_type & WORD_SIZE)
					dw |= 0x8000U;
				else if (mem_type & DWORD_SIZE)
					dw |= 0x80000000L;
				else
					dw |= 0x80;
			}

			gen_put_value(hLocal, mem_type,seg,off,dw); /* store value */

			if (sign_status(mem_type,dw) != dwc)
				hLocal->CPU.FLAGS.O = TRUE;         /* overflow! */
			else
				hLocal->CPU.FLAGS.O = FALSE;

            hLocal->CPU.FLAGS.C = (WORD)dwlow;             /* thru carry */
			break;

		case 4:                                 /* 100: SAL reg/mem, 1 */

		case 6:									// 6 is invalid (SHL)

			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			dw = gen_get_value(hLocal, mem_type,seg,off);
			dwc = sign_status(mem_type,dw);
			dw <<= 1;
			gen_put_value(hLocal,mem_type,seg,off,dw);

			if (sign_status(mem_type,dw) != dwc)
				hLocal->CPU.FLAGS.O = TRUE;         /* overflow! */
			else
				hLocal->CPU.FLAGS.O = FALSE;

            hLocal->CPU.FLAGS.C = (WORD)dwc;

			set_zero_flag(hLocal,mem_type,dw);
            set_sign_flag(hLocal,mem_type,dw);
            set_parity_flag(hLocal, mem_type, dw);

			break;

		case 5:                 /* 101: SHR reg/mem, 1 */

			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			dw = gen_get_value(hLocal,mem_type,seg,off);
			dwc = sign_status(mem_type,dw);
			dwlow = dw & 1;
			dw >>= 1;

			gen_put_value(hLocal, mem_type,seg,off,dw); /* store value */

			if (dwc != sign_status(mem_type,dw))
				hLocal->CPU.FLAGS.O = TRUE;
			else
				hLocal->CPU.FLAGS.O = FALSE;

            hLocal->CPU.FLAGS.C = (WORD)dwlow;

			set_zero_flag(hLocal,mem_type,dw);
			set_sign_flag(hLocal,mem_type,dw);
            set_parity_flag(hLocal, mem_type, dw);

			break;

//		case 6:
//
//			//printf("??? instruction used (2)\n");
//
//			// this is an ??? instruction and we need to remove the
//			// WORD operand from the instruction stream to continue
//			// executing properly.  decode_amb does this.
//
//			printf("What does this invalid instruction do? CSN\n");
//
//			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);
//			break;


		case 7:                 /* 111: SAR reg/mem, 1 */

			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			dw = gen_get_value(hLocal, mem_type, seg, off);
			dwc = sign_status(mem_type,dw);
			dwlow = dw & 1;
			dw >>= 1;
			if (dwc)
			{
				if (mem_type & WORD_SIZE)
					dw |= 0x8000U;
				else if (mem_type & DWORD_SIZE)
					dw |= 0x80000000L;
				else
					dw |= 0x80;
			}

			gen_put_value(hLocal, mem_type,seg,off,dw); /* store value */

			hLocal->CPU.FLAGS.O = FALSE;
            hLocal->CPU.FLAGS.C = (WORD)dwlow;

            set_zero_flag(hLocal,mem_type,dw);
            set_sign_flag(hLocal,mem_type,dw);
            set_parity_flag(hLocal, mem_type, dw);

			break;
	}
}

/* 386 */

void oc1101001w(PAMLHANDLE hLocal, BYTE opcode)            /* general purpose */
{
	BYTE amb, mem_type;
	WORD seg, off, reg;
	DWORD dw, dwc, dwcf, dwor;
	BYTE rot_count, orig_c;

	amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */

	switch (SEC_OP(amb))
	{
		case 0:                         /* 000: ROL reg/mem, cl */
			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			dw = gen_get_value(hLocal, mem_type, seg, off);
            rot_count = hLocal->CPU.preg.H.CL & 0x1F;
            orig_c = (BYTE)sign_status(mem_type,dw);

			while (rot_count)
			{
				dwc = sign_status(mem_type, dw);    /* obtain high bit */
				dw <<= 1;                        	/* shift over by 1 */
				dw |= dwc;                    		/* put high bit in low bit */
				rot_count--;                		/* one less rot */
			}

			gen_put_value(hLocal, mem_type,seg,off,dw); /* store value */

			if (sign_status(mem_type,dw) != orig_c)
				hLocal->CPU.FLAGS.O = TRUE;         /* overflow! */
			else
				hLocal->CPU.FLAGS.O = FALSE;

            hLocal->CPU.FLAGS.C = (WORD)dwc;
			break;

        case 1:                         /* 001: ROR reg/mem, cl */
			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			dw = gen_get_value(hLocal, mem_type,seg,off);
            rot_count = hLocal->CPU.preg.H.CL & 0x1F;
            orig_c = (BYTE)sign_status(mem_type,dw);

			if (mem_type & WORD_SIZE)
				dwor = 0x8000U;
			else if (mem_type & DWORD_SIZE)
				dwor = 0x80000000L;
			else
				dwor = 0x80;

			while (rot_count)
			{
				dwc = dw & 1;          	/* obtain low bit */
				dw >>= 1;               /* shift over by 1 */
				if (dwc == TRUE)
					dw |= dwor;         /* put carry bit in high bit */
				rot_count--;            /* one less rot */
			}

			gen_put_value(hLocal, mem_type,seg,off,dw); /* store value */

			if (sign_status(mem_type,dw) != orig_c)
				hLocal->CPU.FLAGS.O = TRUE;         /* overflow! */
			else
				hLocal->CPU.FLAGS.O = FALSE;

            hLocal->CPU.FLAGS.C = (WORD)dwc;
			break;

		case 2:                         /* 010: RCL reg/mem, cl */
			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			dw = gen_get_value(hLocal, mem_type,seg,off);
            rot_count = hLocal->CPU.preg.H.CL & 0x1F;
			dwcf = hLocal->CPU.FLAGS.C;
            orig_c = (BYTE)sign_status(mem_type, dw);

			while (rot_count)
			{
				dwc = sign_status(mem_type,dw);	/* obtain high bit */
				dw <<= 1;                       /* shift over by 1 */
				dw |= dwcf;                    	/* put carry bit in low bit */
				dwcf = dwc;                    	/* set carry flag to high bit */
				rot_count--;                	/* one less rot */
			}

			gen_put_value(hLocal, mem_type,seg,off,dw); /* store value */

			if (sign_status(mem_type,dw) != orig_c)
				hLocal->CPU.FLAGS.O = TRUE;         	/* overflow! */
			else
				hLocal->CPU.FLAGS.O = FALSE;

            hLocal->CPU.FLAGS.C = (WORD)dwcf;           /* thru carry */
			break;

        case 3:                         /* 011: RCR reg/mem, cl */
			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			dw = gen_get_value(hLocal, mem_type,seg,off);
            rot_count = hLocal->CPU.preg.H.CL & 0x1F;
			dwcf = hLocal->CPU.FLAGS.C;
            orig_c = (BYTE)sign_status(mem_type,dw);

			if (mem_type & WORD_SIZE)
				dwor = 0x8000U;
			else if (mem_type & DWORD_SIZE)
				dwor = 0x80000000L;
			else
				dwor = 0x80;

			while (rot_count)
			{
				dwc = dw & 1;               /* obtain low bit */
				dw >>= 1;                   /* shift over by 1 */
				if (dwcf == TRUE)
					dw |= dwor;          	/* put carry bit in high bit */
				dwcf = dwc;                 /* set carry flag to high bit */
				rot_count--;                /* one less rot */
			}

			gen_put_value(hLocal, mem_type,seg,off,dw); /* store value */

			if (sign_status(mem_type,dw) != orig_c)
				hLocal->CPU.FLAGS.O = TRUE;         /* overflow! */
			else
				hLocal->CPU.FLAGS.O = FALSE;

            hLocal->CPU.FLAGS.C = (WORD)dwcf;       /* thru carry */
			break;

		case 4:                     /* SAL reg/mem, cl */

		case 6:						// invalid opcode has same effect

			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);


			dw = gen_get_value(hLocal, mem_type,seg,off);
            rot_count = hLocal->CPU.preg.H.CL & 0x1F;
            orig_c = (BYTE)sign_status(mem_type,dw);

			if (!rot_count)
				return;

			while (rot_count)
			{
				dwc = sign_status(mem_type,dw);  	/* obtain high bit */
				dw <<= 1;                        	/* shift over by 1 */
				rot_count--;                		/* one less rot */
			}

			gen_put_value(hLocal, mem_type,seg,off,dw); /* store value */

			if (sign_status(mem_type,dw) != orig_c)
				hLocal->CPU.FLAGS.O = TRUE;         /* overflow! */
			else
				hLocal->CPU.FLAGS.O = FALSE;

            hLocal->CPU.FLAGS.C = (WORD)dwc;

			set_zero_flag(hLocal,mem_type,dw);
			set_sign_flag(hLocal,mem_type,dw);
            set_parity_flag(hLocal, mem_type, dw);

			break;

		case 5:                 /* 101: SHR reg/mem, cl */

			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			dw = gen_get_value(hLocal, mem_type,seg,off);
            rot_count = hLocal->CPU.preg.H.CL & 0x1F;
            orig_c = (BYTE)sign_status(mem_type,dw);

			if (!rot_count)
				return;

			while (rot_count)
			{
				dwc = dw & 1;				/* obtain low bit */
				dw >>= 1;                   /* shift over by 1 */
				rot_count--;                /* one less rot */
			}

			gen_put_value(hLocal, mem_type,seg,off,dw); /* store value */

			if (sign_status(mem_type,dw) != orig_c)
				hLocal->CPU.FLAGS.O = TRUE;
			else
				hLocal->CPU.FLAGS.O = FALSE;

            hLocal->CPU.FLAGS.C = (WORD)dwc;

			set_zero_flag(hLocal,mem_type,dw);
			set_sign_flag(hLocal,mem_type,dw);
            set_parity_flag(hLocal, mem_type, dw);
			break;

//		case 6:
//
//			// ??? instruction.   Just remove bytes from the instruction
//			// stream an continue!
//
//			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);
//			break;

		case 7:                 /* 111: SAR reg/mem, cl */

			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			dw = gen_get_value(hLocal, mem_type,seg,off);
            rot_count = hLocal->CPU.preg.H.CL & 0x1F;
            orig_c = (BYTE)sign_status(mem_type,dw);

			if (mem_type & WORD_SIZE)
				dwor = 0x8000U;
			else if (mem_type & DWORD_SIZE)
				dwor = 0x80000000L;
			else
				dwor = 0x80;

			if (!rot_count)
				return;

			while (rot_count)
			{
				dwc = dw & 1;        		/* obtain low bit */
				dw >>= 1;                   /* shift over by 1 */
				if (orig_c == TRUE)
					dw |= dwor;             /* put carry bit in high bit */
				rot_count--;                /* one less rot */
			}

			gen_put_value(hLocal, mem_type,seg,off,dw); /* store value */

			hLocal->CPU.FLAGS.O = FALSE;
            hLocal->CPU.FLAGS.C = (WORD)dwc;

			set_sign_flag(hLocal,mem_type,dw);
			set_zero_flag(hLocal,mem_type,dw);
            set_parity_flag(hLocal, mem_type, dw);

			break;
	}
}


/* 386 */

void oc1100000w(PAMLHANDLE hLocal, BYTE opcode)  /* Rotates and shifts (i8) */
{
	BYTE amb, mem_type;
	WORD seg, off, reg;
	DWORD dw, dwc, dwcf, dwor;
	BYTE rot_count, orig_c;

	amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */

	switch (SEC_OP(amb))
	{
        case 0:                         /* 000: ROL reg/mem, imm */
			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			dw = gen_get_value(hLocal, mem_type, seg, off);
            rot_count = get_byte_at_csip(hLocal) & 0x1F;
            orig_c = (BYTE)sign_status(mem_type,dw);

			while (rot_count)
			{
				dwc = sign_status(mem_type, dw);    /* obtain high bit */
				dw <<= 1;                        	/* shift over by 1 */
				dw |= dwc;                    		/* put high bit in low bit */
				rot_count--;                		/* one less rot */
			}

			gen_put_value(hLocal, mem_type,seg,off,dw); /* store value */

			if (sign_status(mem_type,dw) != orig_c)
				hLocal->CPU.FLAGS.O = TRUE;         /* overflow! */
			else
				hLocal->CPU.FLAGS.O = FALSE;

            hLocal->CPU.FLAGS.C = (WORD)dwc;
			break;

        case 1:                         /* 001: ROR reg/mem, imm */
			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			dw = gen_get_value(hLocal, mem_type,seg,off);
            rot_count = get_byte_at_csip(hLocal) & 0x1F;
            orig_c = (BYTE)sign_status(mem_type,dw);

			if (mem_type & WORD_SIZE)
				dwor = 0x8000U;
			else if (mem_type & DWORD_SIZE)
				dwor = 0x80000000L;
			else
				dwor = 0x80;

			while (rot_count)
			{
				dwc = dw & 1;          	/* obtain low bit */
				dw >>= 1;               /* shift over by 1 */
				if (dwc == TRUE)
					dw |= dwor;         /* put carry bit in high bit */
				rot_count--;            /* one less rot */
			}

			gen_put_value(hLocal, mem_type,seg,off,dw); /* store value */

			if (sign_status(mem_type,dw) != orig_c)
				hLocal->CPU.FLAGS.O = TRUE;         /* overflow! */
			else
				hLocal->CPU.FLAGS.O = FALSE;

            hLocal->CPU.FLAGS.C = (WORD)dwc;
			break;

		case 2:                         /* 010: RCL reg/mem, immed */
			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			dw = gen_get_value(hLocal, mem_type,seg,off);
            rot_count = get_byte_at_csip(hLocal) & 0x1F;
			dwcf = hLocal->CPU.FLAGS.C;
            orig_c = (BYTE)sign_status(mem_type, dw);

			while (rot_count)
			{
				dwc = sign_status(mem_type,dw);	/* obtain high bit */
				dw <<= 1;                       /* shift over by 1 */
				dw |= dwcf;                    	/* put carry bit in low bit */
				dwcf = dwc;                    	/* set carry flag to high bit */
				rot_count--;                	/* one less rot */
			}

			gen_put_value(hLocal, mem_type,seg,off,dw); /* store value */

			if (sign_status(mem_type,dw) != orig_c)
				hLocal->CPU.FLAGS.O = TRUE;         	/* overflow! */
			else
				hLocal->CPU.FLAGS.O = FALSE;

            hLocal->CPU.FLAGS.C = (WORD)dwcf;           /* thru carry */
			break;

        case 3:                         /* 011: RCR reg/mem, imm */
			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			dw = gen_get_value(hLocal, mem_type,seg,off);
            rot_count = get_byte_at_csip(hLocal) & 0x1F;
			dwcf = hLocal->CPU.FLAGS.C;
            orig_c = (BYTE)sign_status(mem_type,dw);

			if (mem_type & WORD_SIZE)
				dwor = 0x8000U;
			else if (mem_type & DWORD_SIZE)
				dwor = 0x80000000L;
			else
				dwor = 0x80;

			while (rot_count)
			{
				dwc = dw & 1;               /* obtain low bit */
				dw >>= 1;                   /* shift over by 1 */
				if (dwcf == TRUE)
					dw |= dwor;          	/* put carry bit in high bit */
				dwcf = dwc;                 /* set carry flag to high bit */
				rot_count--;                /* one less rot */
			}

			gen_put_value(hLocal, mem_type,seg,off,dw); /* store value */

			if (sign_status(mem_type,dw) != orig_c)
                hLocal->CPU.FLAGS.O = TRUE;             /* overflow! */
			else
				hLocal->CPU.FLAGS.O = FALSE;

            hLocal->CPU.FLAGS.C = (WORD)dwcf;           /* thru carry */
			break;

        case 4:                     /* SAL reg/mem, imm */

		case 6:

			// case 6 is an invalid opcode which works the same as case 4

			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);


			dw = gen_get_value(hLocal, mem_type,seg,off);
            rot_count = get_byte_at_csip(hLocal) & 0x1F;
            orig_c = (BYTE)sign_status(mem_type,dw);

			if (!rot_count)
				return;

			while (rot_count)
			{
				dwc = sign_status(mem_type,dw);  	/* obtain high bit */
				dw <<= 1;                        	/* shift over by 1 */
				rot_count--;                		/* one less rot */
			}

			gen_put_value(hLocal, mem_type,seg,off,dw); /* store value */

			if (sign_status(mem_type,dw) != orig_c)
				hLocal->CPU.FLAGS.O = TRUE;         /* overflow! */
			else
				hLocal->CPU.FLAGS.O = FALSE;

            hLocal->CPU.FLAGS.C = (WORD)dwc;

            set_zero_flag(hLocal,mem_type,dw);
            set_sign_flag(hLocal,mem_type,dw);
            set_parity_flag(hLocal, mem_type, dw);
			break;

        case 5:                 /* 101: SHR reg/mem, imm */

			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			dw = gen_get_value(hLocal, mem_type,seg,off);
            rot_count = get_byte_at_csip(hLocal) & 0x1F;
            orig_c = (BYTE)sign_status(mem_type,dw);

			if (!rot_count)
				return;

			while (rot_count)
			{
				dwc = dw & 1;				/* obtain low bit */
				dw >>= 1;                   /* shift over by 1 */
				rot_count--;                /* one less rot */
			}

			gen_put_value(hLocal, mem_type,seg,off,dw); /* store value */

			if (sign_status(mem_type,dw) != orig_c)
				hLocal->CPU.FLAGS.O = TRUE;
			else
				hLocal->CPU.FLAGS.O = FALSE;

            hLocal->CPU.FLAGS.C = (WORD)dwc;

			set_zero_flag(hLocal,mem_type,dw);
			set_sign_flag(hLocal,mem_type,dw);
            set_parity_flag(hLocal, mem_type, dw);

			break;

//		case 6:
//
//			// ??? instruction.   Just remove bytes from the instruction
//			// stream an continue!
//
//			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);
//
//			// remove one byte for the immediate value
//
//			get_byte_at_csip(hLocal);
//			break;

        case 7:                 /* 111: SAR reg/mem, imm */

			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			dw = gen_get_value(hLocal, mem_type,seg,off);
            rot_count = get_byte_at_csip(hLocal) & 0x1F;
            orig_c = (BYTE)sign_status(mem_type,dw);

			if (mem_type & WORD_SIZE)
				dwor = 0x8000U;
			else if (mem_type & DWORD_SIZE)
				dwor = 0x80000000L;
			else
				dwor = 0x80;

			if (!rot_count)
				return;

			while (rot_count)
			{
				dwc = dw & 1;        		/* obtain low bit */
				dw >>= 1;                   /* shift over by 1 */
				if (orig_c == TRUE)
					dw |= dwor;             /* put carry bit in high bit */
				rot_count--;                /* one less rot */
			}

			gen_put_value(hLocal, mem_type,seg,off,dw); /* store value */

			hLocal->CPU.FLAGS.O = FALSE;
            hLocal->CPU.FLAGS.C = (WORD)dwc;
            set_sign_flag(hLocal,mem_type,dw);
            set_parity_flag(hLocal, mem_type, dw);

			break;
	}
}




/* 386 */

void oc11110010(PAMLHANDLE hLocal)           /* repnz */
{
    hLocal->CPU.rep_prefix = PREFIX_REPNZ;
    hLocal->CPU.reset_seg = FALSE;
}


/* 386 */

void oc11110011(PAMLHANDLE hLocal)           /* repz */
{
    hLocal->CPU.rep_prefix = PREFIX_REPZ;
    hLocal->CPU.reset_seg = FALSE;
}

/* 386 */

void oc11000011(PAMLHANDLE hLocal)           /* ret near */
{
    hLocal->CPU.IP = pop_word(hLocal);        /* return address is absolute IP */

    reset_prefetch_queue(hLocal);
}

/* 386 */

void oc11000010(PAMLHANDLE hLocal)           /* ret immed16 */
{
    WORD w;

    w = get_word_at_csip(hLocal);
    hLocal->CPU.IP = pop_word(hLocal);        /* return address is absolute IP */
	hLocal->CPU.ireg.X.SP += w;               /* remove local args */

    reset_prefetch_queue(hLocal);
}

/* 386 */

void oc11001011(PAMLHANDLE hLocal)           /* retf */
{
	hLocal->CPU.IP = pop_word(hLocal);        /* return address IP */
    hLocal->CPU.CS = pop_word(hLocal);        /* return address CS */

    reset_prefetch_queue(hLocal);
}

/* 386 */

void oc11001010(PAMLHANDLE hLocal)           /* retf immed16 */
{
    WORD w;

    w = get_word_at_csip(hLocal);

    hLocal->CPU.IP = pop_word(hLocal);        /* return address IP */
    hLocal->CPU.CS = pop_word(hLocal);        /* return address CS */

    hLocal->CPU.ireg.X.SP += w;              /* remove local args */

    reset_prefetch_queue(hLocal);
}

/* 386 */

void oc000110dw(PAMLHANDLE hLocal, BYTE opcode)            /* SBB reg/mem, reg/mem */
{
    BYTE amb, mem_type;
    WORD seg, off, reg;
    DWORD dwscratch, dwreg, dwmem;

    /* if D = 0 then REG field is source and dest will be memory/register.
       if D = 1 then dest is a register (specified by REG) and source may
       be memory or another register */

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */
    decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

    dwreg = reg_value(hLocal, mem_type, reg);
    dwmem = gen_get_value(hLocal, mem_type,seg,off);

    if (D_VALUE(opcode) == 0)
    {
		dwscratch = dwmem - dwreg - hLocal->CPU.FLAGS.C;
		gen_put_value(hLocal,mem_type,seg,off,dwscratch);

		set_over_flag_sub(hLocal,mem_type,dwmem,dwreg, hLocal->CPU.FLAGS.C);
		set_carry_flag_sub(hLocal,mem_type, dwmem, dwreg,hLocal->CPU.FLAGS.C);
	}
	else            /* D == 1 */
	{
		dwscratch = dwreg - dwmem - hLocal->CPU.FLAGS.C;
		set_reg(hLocal, mem_type, reg, dwscratch);

		set_over_flag_sub(hLocal, mem_type, dwreg,dwmem, hLocal->CPU.FLAGS.C);
		set_carry_flag_sub(hLocal, mem_type, dwreg, dwmem, hLocal->CPU.FLAGS.C);
	}

	set_zero_flag(hLocal, mem_type, dwscratch);
	set_sign_flag(hLocal, mem_type, dwscratch);
    set_parity_flag(hLocal, mem_type, dwscratch);
}

/* 386 */

void oc0001110w(PAMLHANDLE hLocal, BYTE opcode)        /* SBB accum, immed */
{
    DWORD dwscratch, dwreg, dwmem;
    WORD wMemSize;

    if (W_VALUE(opcode) == 0)
        wMemSize = 0;
    else if (hLocal->CPU.op_size_over)
        wMemSize = DWORD_SIZE;
    else
        wMemSize = WORD_SIZE;

    /* REG_AL == REG_AX == REG_EAX */

    dwreg = reg_value(hLocal, wMemSize, REG_AL);
    dwmem = get_value_at_csip(hLocal,wMemSize);
    dwscratch = dwreg - dwmem - hLocal->CPU.FLAGS.C;
    set_reg(hLocal, wMemSize, REG_AL,dwscratch);

    set_over_flag_sub(hLocal, wMemSize, dwreg, dwmem, hLocal->CPU.FLAGS.C);
    set_carry_flag_sub(hLocal,wMemSize, dwreg, dwmem, hLocal->CPU.FLAGS.C);
    set_zero_flag(hLocal, wMemSize, dwscratch);
    set_sign_flag(hLocal, wMemSize, dwscratch);
    set_parity_flag(hLocal, wMemSize, dwscratch);
}

/* 386 */

void oc001seg110(PAMLHANDLE hLocal, BYTE opcode)        /* SEG override! */
{
    set_seg_over(hLocal, (WORD)((opcode >> 3) & 0x3) ); /* get seg # */
    hLocal->CPU.reset_seg = FALSE;     /* only with seg-override! */
}

/* 386 */

/* FS: */

void oc01100100(PAMLHANDLE hLocal)     /* SEG override! */
{
	set_seg_over(hLocal, FS_OVER);     /* get seg # */
	hLocal->CPU.reset_seg = FALSE;     /* only with seg-override! */
}

/* 386 */

/* GS: */

void oc01100101(PAMLHANDLE hLocal)     /* SEG override! */
{
    set_seg_over(hLocal, GS_OVER);     /* get seg # */
    hLocal->CPU.reset_seg = FALSE;     /* only with seg-override! */
}

/* 386 */

void oc11111001(PAMLHANDLE hLocal)           /* STC */
{
    hLocal->CPU.FLAGS.C = TRUE;
}

/* 386 */

void oc11111101(PAMLHANDLE hLocal)           /* STD */
{
    hLocal->CPU.FLAGS.D = TRUE;
}

/* 386 */

void oc11111011(PAMLHANDLE hLocal)           /* STI */
{
    hLocal->CPU.FLAGS.I = TRUE;
}

/* 386 */

void oc1010101w(PAMLHANDLE hLocal, BYTE opcode)    /* stos */
{
    WORD wMemSize, wSkip;

	if (hLocal->CPU.rep_prefix)
	{
		exclude_item(hLocal,REP_STOS);			   /* exclude rep stos's */

#ifndef BUILD_SET

        // only return if we're not in immunity mode of some sort

        if ((hLocal->dwFlags & LOCAL_FLAG_NO_CAND_LEFT) &&
            !(hLocal->dwFlags & (LOCAL_FLAG_IMMUNE |
                                 LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
            return;

#endif

		rep_instruction(hLocal, oc1010101w, opcode);
		return;
	}

    if (W_VALUE(opcode) == 0)
    {
        wMemSize = 0;
        wSkip = sizeof(BYTE);
    }
    else if (hLocal->CPU.op_size_over)
    {
        wMemSize = DWORD_SIZE;
        wSkip = sizeof(DWORD);
    }
    else
    {
        wMemSize = WORD_SIZE;
        wSkip = sizeof(WORD);
    }

    gen_put_value(hLocal,
                  wMemSize,
                  hLocal->CPU.ES,
                  hLocal->CPU.ireg.X.DI,
                  hLocal->CPU.preg.D.EAX);          /* always to ES:DI */

    if (hLocal->CPU.FLAGS.D == 0)
        hLocal->CPU.ireg.X.DI += wSkip;
    else
        hLocal->CPU.ireg.X.DI -= wSkip;
}


/* 386 */

void oc001010dw(PAMLHANDLE hLocal, BYTE opcode)            /* SUB reg/mem, reg/mem */
{
    BYTE amb, mem_type;
    WORD seg, off, reg;
    DWORD dwreg, dwmem, dwscratch;

    /* if D = 0 then REG field is source and dest will be memory/register.
       if D = 1 then dest is a register (specified by REG) and source may
       be memory or another register */

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */
    decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

    dwreg = reg_value(hLocal, mem_type, reg);
    dwmem = gen_get_value(hLocal, mem_type,seg,off);

    if (D_VALUE(opcode) == 0)
    {
        dwscratch = dwmem - dwreg;

        gen_put_value(hLocal, mem_type,seg,off,dwscratch);

        set_over_flag_sub(hLocal, mem_type, dwmem,dwreg, 0);
        set_carry_flag_sub(hLocal, mem_type, dwmem, dwreg, 0);
    }
    else            /* D == 1 */
    {
        dwscratch = dwreg - dwmem;
        set_reg(hLocal, mem_type, reg, dwscratch);

        set_over_flag_sub(hLocal, mem_type, dwreg,dwmem, 0);
        set_carry_flag_sub(hLocal, mem_type, dwreg, dwmem, 0);
    }

    set_zero_flag(hLocal, mem_type, dwscratch);
    set_sign_flag(hLocal, mem_type, dwscratch);
    set_parity_flag(hLocal, mem_type, dwscratch);
}

/* 386 */

void oc0010110w(PAMLHANDLE hLocal, BYTE opcode)        /* sub accum, immed */
{
    DWORD dwscratch, dwreg, dwmem;
    WORD wMemSize;

    if (W_VALUE(opcode) == 0)
        wMemSize = 0;
    else if (hLocal->CPU.op_size_over)
        wMemSize = DWORD_SIZE;
    else
        wMemSize = WORD_SIZE;

    /* REG_AL = REG_AX = REG_EAX */

	dwreg = reg_value(hLocal, wMemSize, REG_AX);
	dwmem = get_value_at_csip(hLocal,wMemSize);
    dwscratch = dwreg - dwmem;
	set_reg(hLocal, wMemSize, REG_AL, dwscratch);

	set_over_flag_sub(hLocal, wMemSize, dwreg,dwmem, 0);
	set_carry_flag_sub(hLocal, wMemSize, dwreg, dwmem, 0);
	set_zero_flag(hLocal, wMemSize, dwscratch);
	set_sign_flag(hLocal, wMemSize, dwscratch);
    set_parity_flag(hLocal, wMemSize, dwscratch);
}


/* 386 */

void oc1000011w(PAMLHANDLE hLocal, BYTE opcode)        /* XCHG reg/mem, reg */
{
    BYTE amb, mem_type;
    WORD seg, off, reg;
    DWORD dwreg, dwmem;

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */
    decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

    dwreg = reg_value(hLocal, mem_type, reg);
    dwmem = gen_get_value(hLocal, mem_type,seg,off);

    set_reg(hLocal,mem_type,reg,dwmem);
    gen_put_value(hLocal, mem_type,seg,off,dwreg);
}


/* 386 */

void oc10010reg(PAMLHANDLE hLocal, BYTE opcode)        /* XCHG AX, reg */
{
    WORD w;
    DWORD dw;

    opcode &= 0x7;      /* obtain reg to swap with */

    if (hLocal->CPU.op_size_over)
    {
		dw = dwreg_value(hLocal, opcode);
        set_dwreg(hLocal, opcode, hLocal->CPU.preg.D.EAX);
        hLocal->CPU.preg.D.EAX = dw;
    }
    else
    {
		w = wreg_value(hLocal, opcode);
        set_wreg(hLocal, opcode, hLocal->CPU.preg.X.AX);
        hLocal->CPU.preg.X.AX = w;
    }
}


/* 386 */

void oc001100dw(PAMLHANDLE hLocal, BYTE opcode)            /* XOR reg/mem, reg/mem */
{
    BYTE amb, mem_type;
    WORD seg, off, reg;
    DWORD dwreg, dwmem, dwscratch;

    /* if D = 0 then REG field is source and dest will be memory/register.
       if D = 1 then dest is a register (specified by REG) and source may
       be memory or another register */

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */
    decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

	dwreg = reg_value(hLocal, mem_type, reg);
	dwmem = gen_get_value(hLocal, mem_type,seg,off);
	dwscratch = dwmem ^ dwreg;

    if (D_VALUE(opcode) == 0)
        gen_put_value(hLocal, mem_type,seg,off,dwscratch);
    else            /* D == 1 */
        set_reg(hLocal, mem_type, reg, dwscratch);

    /* set OF and CF to 0 regardless */

    hLocal->CPU.FLAGS.O = hLocal->CPU.FLAGS.C = 0;      /* by definition */

    /* flag stuff */

    set_sign_flag(hLocal,mem_type,dwscratch);
    set_zero_flag(hLocal,mem_type,dwscratch);
    set_parity_flag(hLocal, mem_type, dwscratch);
}

/* 386 */

void oc0011010w(PAMLHANDLE hLocal, BYTE opcode)        /* XOR accum, immed */
{
    DWORD dwscratch, dwreg, dwmem;
    WORD wMemSize;

    if (W_VALUE(opcode) == 0)
        wMemSize = 0;
    else if (hLocal->CPU.op_size_over)
        wMemSize = DWORD_SIZE;
    else
        wMemSize = WORD_SIZE;

    /* REG_AL == REG_AX == REG_EAX */

    dwreg = reg_value(hLocal, wMemSize, REG_AL);
    dwmem = get_value_at_csip(hLocal,wMemSize);
    dwscratch = dwmem ^ dwreg;
    set_reg(hLocal,wMemSize, REG_AL,dwscratch);

    set_sign_flag(hLocal,wMemSize,dwscratch);
    set_zero_flag(hLocal,wMemSize,dwscratch);
    set_parity_flag(hLocal, wMemSize, dwscratch);

    /* set OF and CF to 0 regardless */

    hLocal->CPU.FLAGS.O = hLocal->CPU.FLAGS.C = 0;      /* by definition */
}

/* 386 */

void oc11101001(PAMLHANDLE hLocal)       /* jmp near direct */
{
    WORD woff;

    woff = get_word_at_csip(hLocal);
    hLocal->CPU.IP += woff;

    reset_prefetch_queue(hLocal);
}

/* 386 */

void oc1000010w(PAMLHANDLE hLocal, BYTE opcode)        /* test reg, reg/mem */
{
    BYTE amb, mem_type;
    WORD seg, off, reg;
    DWORD dwscratch, dwreg, dwmem;

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */
    decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

    dwreg = reg_value(hLocal, mem_type, reg);
    dwmem = gen_get_value(hLocal, mem_type,seg,off);

    dwscratch = dwreg & dwmem;

    set_sign_flag(hLocal, mem_type, dwscratch);
    set_zero_flag(hLocal, mem_type, dwscratch);
    set_parity_flag(hLocal, mem_type, dwscratch);

    hLocal->CPU.FLAGS.O = hLocal->CPU.FLAGS.C = 0;      /* by definition */
}

/* 386 */

void oc1010100w(PAMLHANDLE hLocal, BYTE opcode)            /* TEST accum, immed */
{
    DWORD dwscratch, dwreg, dwmem;
    WORD wMemSize;

    if (W_VALUE(opcode) == 0)
        wMemSize = 0;
    else if (hLocal->CPU.op_size_over)
        wMemSize = DWORD_SIZE;
    else
        wMemSize = WORD_SIZE;

    /* REG_AL == REG_AX == REG_EAX */

    dwreg = reg_value(hLocal, wMemSize, REG_AL);
    dwmem = get_value_at_csip(hLocal,wMemSize);
    dwscratch = dwmem & dwreg;

    set_sign_flag(hLocal, wMemSize, dwscratch);
    set_zero_flag(hLocal, wMemSize, dwscratch);
    set_parity_flag(hLocal, wMemSize, dwscratch);

    /* set OF and CF to 0 regardless */

    hLocal->CPU.FLAGS.O = hLocal->CPU.FLAGS.C = 0;      /* by definition */
}


/* 386 */
/* int 3 */

void oc11001100(PAMLHANDLE hLocal)
{
	WORD seg, off;

    if (!(hLocal->dwFlags & LOCAL_FLAG_REPAIR_REPAIR))
    {
        off = get_word(hLocal, 0,0x3*VECT_SIZE);
        seg = get_word(hLocal, 0,0x3*VECT_SIZE+sizeof(WORD));

        hLocal->CPU.op_size_over = FALSE;       // don't want 32 bit PUSHFD!

        oc10011100(hLocal);                     /* pushf */

        hLocal->CPU.FLAGS.I = FALSE;          // disable ints!
        hLocal->CPU.FLAGS.T = FALSE;          // diable trap flag

        push_word(hLocal, hLocal->CPU.CS);
        push_word(hLocal, hLocal->CPU.IP);

        hLocal->CPU.CS = seg;
        hLocal->CPU.IP = off;

        reset_prefetch_queue(hLocal);
    }
    else
    {
        // we're in our repair state, use LIVT (local interrupt vector table)
        // the acronym sounds more impressive, doesn't it?
        //
        // The local IVT is located at F000:0 to F000:200 and is an array
        // full of WORDS as opposed to DWORDS.

        off = get_word(hLocal, REPAIR_SEG, 0x3*sizeof(WORD));

        hLocal->CPU.op_size_over = FALSE;       // don't want 32 bit PUSHFD!

        oc10011100(hLocal);                     /* pushf */

        hLocal->CPU.FLAGS.I = FALSE;          // disable ints!
        hLocal->CPU.FLAGS.T = FALSE;          // diable trap flag

        push_word(hLocal, hLocal->CPU.CS);
        push_word(hLocal, hLocal->CPU.IP);

        hLocal->CPU.CS = REPAIR_SEG;
        hLocal->CPU.IP = off;

        reset_prefetch_queue(hLocal);
    }
}

/* 386 */
/* int general */

void oc11001101(PAMLHANDLE hLocal)
{
    WORD seg, off;
	BYTE int_num;

#ifdef BUILD_SET

    static BYTE ints[256] = {0};
    static BYTE ints_used[256] = {0};
#endif


    hLocal->CPU.op_size_over = FALSE;       // don't want 32 bit PUSHFD!

	int_num = get_byte_at_csip(hLocal);

	// perform exclusions!
    // only exclude first 128 interrupts function #'s

    if (int_num == 0x21 && hLocal->CPU.preg.H.AH < 0x80)
        exclude_item(hLocal, (WORD)(INT_21_BASE + hLocal->CPU.preg.H.AH));

#ifdef BUILD_SET

	if (int_num == 0x21 && ints[hLocal->CPU.preg.H.AH] == 0)
	{
		printf("INT 21, AH = %02X\n",hLocal->CPU.preg.H.AH);
		ints[hLocal->CPU.preg.H.AH] = 1;
	}

    if ((0x05 == int_num || 0x10 == int_num || 0x12 == int_num ||
        0x13 == int_num || 0x16 == int_num || 0x17 == int_num ||
        0x1A == int_num || 0x20 == int_num || 0x25 == int_num ||
        0x26 == int_num || 0x27 == int_num || 0x2F == int_num ||
        0x33 == int_num || 0x67 == int_num) &&
        ints_used[int_num] == FALSE)
    {
        ints_used[int_num] = TRUE;
        printf("Special INT %02X generated.\n",int_num);
    }

#endif

	exclude_int(hLocal, int_num);

    if (!(hLocal->dwFlags & LOCAL_FLAG_REPAIR_REPAIR))
    {
        off = get_word(hLocal, 0,(WORD)(int_num*VECT_SIZE));
        seg = get_word(hLocal, 0,(WORD)(int_num*VECT_SIZE+sizeof(WORD)));

        oc10011100(hLocal);                   /* pushf */

        hLocal->CPU.FLAGS.I = FALSE;          // disable ints!
        hLocal->CPU.FLAGS.T = FALSE;          // diable trap flag

        push_word(hLocal, hLocal->CPU.CS);
        push_word(hLocal, hLocal->CPU.IP);

        hLocal->CPU.CS = seg;
        hLocal->CPU.IP = off;

        reset_prefetch_queue(hLocal);
    }
    else
    {
        // we're in our repair state, use LIVT (local interrupt vector table)
        // the acronym sounds more impressive, doesn't it?
        //
        // The local IVT is located at F000:0 to F000:200 and is an array
        // full of WORDS as opposed to DWORDS.

        off = get_word(hLocal, REPAIR_SEG, (WORD)(int_num*sizeof(WORD)));

        oc10011100(hLocal);                 /* pushf */

        hLocal->CPU.FLAGS.I = FALSE;          // disable ints!
        hLocal->CPU.FLAGS.T = FALSE;          // diable trap flag

        push_word(hLocal, hLocal->CPU.CS);
        push_word(hLocal, hLocal->CPU.IP);

        hLocal->CPU.CS = REPAIR_SEG;
        hLocal->CPU.IP = off;

        reset_prefetch_queue(hLocal);
    }
}

/* 386 */

void oc11001111(PAMLHANDLE hLocal)       /* iret */
{
    WORD ip, cs;

    hLocal->CPU.op_size_over = FALSE;       // don't want 32 bit POPFD!

    if (hLocal->dwFlags & LOCAL_FLAG_IMMUNE)
    {
        hLocal->dwFlags &= ~(DWORD)LOCAL_FLAG_IMMUNE;

#ifdef SYM_NLM
        PAMRelinquishControl();
#endif
    }

    ip = pop_word(hLocal);
    cs = pop_word(hLocal);

    oc10011101(hLocal);           /* pop flags */

    hLocal->CPU.IP = ip;
    hLocal->CPU.CS = cs;

    enable_prefetch_queue(hLocal);
    reset_prefetch_queue(hLocal);
}


/* 386 */
/* scas */

void oc1010111w(PAMLHANDLE hLocal, BYTE opcode)
{
    DWORD dw1,dw2;
	WORD wMemSize, wSkip;

    if (hLocal->CPU.rep_prefix)
	{
        exclude_item(hLocal,REP_SCAS);             /* exclude rep scas's */

#ifndef BUILD_SET

        // only return if we're not in immunity mode of some sort

        if ((hLocal->dwFlags & LOCAL_FLAG_NO_CAND_LEFT) &&
            !(hLocal->dwFlags & (LOCAL_FLAG_IMMUNE |
                                 LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
        return;

#endif

        rep_instruction(hLocal, oc1010111w, opcode);
		return;
    }


    if (W_VALUE(opcode) == 0)
    {
        wMemSize = 0;
        wSkip = sizeof(BYTE);
    }
    else if (hLocal->CPU.op_size_over)
    {
        wMemSize = DWORD_SIZE;
        wSkip = sizeof(DWORD);
    }
    else
    {
        wMemSize = WORD_SIZE;
        wSkip = sizeof(WORD);
    }

    /* REG_AL == REG_AX == REG_EAX */


	dw1 = reg_value(hLocal, wMemSize, REG_AL);
    dw2 = gen_get_value(hLocal,
						wMemSize,
                        hLocal->CPU.ES,
                        hLocal->CPU.ireg.X.DI);    /* ES:DI default */

    set_carry_flag_sub(hLocal, wMemSize, dw1, dw2, 0);
    set_over_flag_sub(hLocal, wMemSize, dw1,dw2, 0);
    set_zero_flag(hLocal, wMemSize, dw1-dw2);
    set_sign_flag(hLocal, wMemSize, dw1-dw2);
    set_parity_flag(hLocal, wMemSize, dw1-dw2);

    if (hLocal->CPU.FLAGS.D == 0)
        hLocal->CPU.ireg.X.DI += wSkip;
    else
        hLocal->CPU.ireg.X.DI -= wSkip;
}


/* extended 386 stuff */

void oc00001111(PAMLHANDLE hLocal)
{
	BYTE opcode;
	WORD wOffset;

	opcode = get_byte_at_csip(hLocal);

	switch (opcode)
	{
		case JA_NEAR:
			wOffset = get_word_at_csip(hLocal);
			if (hLocal->CPU.FLAGS.C == FALSE && hLocal->CPU.FLAGS.Z == FALSE)
            {
				hLocal->CPU.IP += wOffset;

                reset_prefetch_queue(hLocal);
            }
			break;
		case JAE_NEAR:
			wOffset = get_word_at_csip(hLocal);
			if (hLocal->CPU.FLAGS.C == FALSE)
            {
				hLocal->CPU.IP += wOffset;

                reset_prefetch_queue(hLocal);
            }
			break;
		case JC_NEAR:
			wOffset = get_word_at_csip(hLocal);
			if (hLocal->CPU.FLAGS.C == TRUE)
            {
				hLocal->CPU.IP += wOffset;

                reset_prefetch_queue(hLocal);
            }
			break;
		case JBE_NEAR:
			wOffset = get_word_at_csip(hLocal);
			if (hLocal->CPU.FLAGS.C == TRUE || hLocal->CPU.FLAGS.Z == TRUE)
            {
				hLocal->CPU.IP += wOffset;

                reset_prefetch_queue(hLocal);
            }
            break;
		case JZ_NEAR:
			wOffset = get_word_at_csip(hLocal);
			if (hLocal->CPU.FLAGS.Z == TRUE)
            {
				hLocal->CPU.IP += wOffset;

                reset_prefetch_queue(hLocal);
            }
			break;
		case JG_NEAR:
			wOffset = get_word_at_csip(hLocal);
			if (hLocal->CPU.FLAGS.Z == FALSE && hLocal->CPU.FLAGS.S == hLocal->CPU.FLAGS.O)
            {
				hLocal->CPU.IP += wOffset;

                reset_prefetch_queue(hLocal);
            }
			break;
		case JGE_NEAR:
			wOffset = get_word_at_csip(hLocal);
			if (hLocal->CPU.FLAGS.S == hLocal->CPU.FLAGS.O)
            {
				hLocal->CPU.IP += wOffset;

                reset_prefetch_queue(hLocal);
            }
			break;
		case JL_NEAR:
			wOffset = get_word_at_csip(hLocal);
			if (hLocal->CPU.FLAGS.S != hLocal->CPU.FLAGS.O)
            {
				hLocal->CPU.IP += wOffset;

                reset_prefetch_queue(hLocal);
            }
			break;
		case JLE_NEAR:
			wOffset = get_word_at_csip(hLocal);
			if (hLocal->CPU.FLAGS.Z == TRUE || hLocal->CPU.FLAGS.S != hLocal->CPU.FLAGS.O)
            {
				hLocal->CPU.IP += wOffset;

                reset_prefetch_queue(hLocal);
            }
			break;
		case JNZ_NEAR:
			wOffset = get_word_at_csip(hLocal);
			if (hLocal->CPU.FLAGS.Z == FALSE)
            {
				hLocal->CPU.IP += wOffset;

                reset_prefetch_queue(hLocal);
            }
			break;
		case JNO_NEAR:
			wOffset = get_word_at_csip(hLocal);
			if (hLocal->CPU.FLAGS.O == FALSE)
            {
				hLocal->CPU.IP += wOffset;

                reset_prefetch_queue(hLocal);
            }
			break;
		case JO_NEAR:
			wOffset = get_word_at_csip(hLocal);
			if (hLocal->CPU.FLAGS.O == TRUE)
            {
				hLocal->CPU.IP += wOffset;

                reset_prefetch_queue(hLocal);
            }
			break;
		case JS_NEAR:
			wOffset = get_word_at_csip(hLocal);
			if (hLocal->CPU.FLAGS.S == TRUE)
            {
				hLocal->CPU.IP += wOffset;

                reset_prefetch_queue(hLocal);
            }
			break;
		case JNS_NEAR:
            wOffset = get_word_at_csip(hLocal);
			if (hLocal->CPU.FLAGS.S == FALSE)
            {
				hLocal->CPU.IP += wOffset;

                reset_prefetch_queue(hLocal);
            }
			break;
        case JPE_NEAR:
            wOffset = get_word_at_csip(hLocal);
            if (hLocal->CPU.FLAGS.P == TRUE)
            {
				hLocal->CPU.IP += wOffset;

                reset_prefetch_queue(hLocal);
            }
            break;
        case JPO_NEAR:
            wOffset = get_word_at_csip(hLocal);
            if (hLocal->CPU.FLAGS.P == FALSE)
            {
				hLocal->CPU.IP += wOffset;

                reset_prefetch_queue(hLocal);
            }
            break;

		case BSF_OPCODE:
			BSF(hLocal);
			break;

		case BSR_OPCODE:
			BSR(hLocal);
			break;

        case BSWAP_OPCODE:
            BSWAP(hLocal,opcode);
			break;

        case BT_IMMED_OPCODE:
            BT(hLocal, opcode, BT_DETERMINE);
            break;

        case BT_REG_OPCODE:
            BT(hLocal, opcode, BT_TEST);
            break;

        case BTC_REG_OPCODE:
            BT(hLocal, opcode, BT_CARRY);
            break;

        case BTR_REG_OPCODE:
            BT(hLocal, opcode, BT_RESET);
            break;

        case BTS_REG_OPCODE:
            BT(hLocal, opcode, BT_SET);
            break;

        case CLTS_OPCODE:
            /* clear CR0 - this basically fetches the byte after 0x0F */
            break;

        case CMPXCHGB_OPCODE:
        case CMPXCHGW_OPCODE:
            CMPXCHG(hLocal,opcode);
            break;

        case INVD_OPCODE:
            /* invalidate CACHE - basically fetches byte */
            break;

        case LGDT_OPCODE:
            LGDT(hLocal);
            break;

        case LSS_OPCODE:
            LSREG(hLocal,REG_SS);
            break;

        case LFS_OPCODE:
            LSREG(hLocal,REG_FS);
            break;

        case LGS_OPCODE:
            LSREG(hLocal,REG_GS);
            break;

        case MOVSX8_OPCODE:
        case MOVSX16_OPCODE:
            MOVSX(hLocal,opcode);
            break;

        case MOVZX8_OPCODE:
        case MOVZX16_OPCODE:
            MOVZX(hLocal,opcode);
            break;

        case POP_FS_OPCODE:
            POP_FS(hLocal);
            break;

        case POP_GS_OPCODE:
            POP_GS(hLocal);
            break;

        case PUSH_FS_OPCODE:
            PUSH_FS(hLocal);
            break;

        case PUSH_GS_OPCODE:
            PUSH_GS(hLocal);
            break;

        case  SETA_OPCODE:
        case  SETAE_OPCODE:
        case  SETB_OPCODE:
        case  SETBE_OPCODE:
        case  SETE_OPCODE:
        case  SETG_OPCODE:
        case  SETGE_OPCODE:
        case  SETL_OPCODE:
        case  SETLE_OPCODE:
        case  SETNE_OPCODE:
        case  SETNO_OPCODE:
        case  SETNP_OPCODE:
        case  SETNS_OPCODE:
        case  SETO_OPCODE:
        case  SETP_OPCODE:
        case  SETS_OPCODE:
            SETcc(hLocal,opcode);
            break;

        case SHLD_CL:
        case SHLD_I8:
            SHLD(hLocal,opcode);
			break;

		case SHRD_CL:
		case SHRD_I8:
			SHRD(hLocal,opcode);
			break;

        case WBINVD_OPCODE:

            /* 2 byte instr */

            break;

        case XADD8_OPCODE:
        case XADD16_OPCODE:
            XADD(hLocal, opcode);
            break;

        case IMUL_AF_OPCODE:
            IMUL_AF(hLocal);
            break;
    }
}


void BSF(PAMLHANDLE hLocal)
{
	BYTE amb, mem_type, opcode;
    WORD seg, off, reg;
	DWORD dwsecond, temp;

	opcode = 1;

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */
    decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

    dwsecond = gen_get_value(hLocal, mem_type, seg, off);

    if (0 == dwsecond)
    {
        hLocal->CPU.FLAGS.Z = TRUE;
        return;
    }

    temp = 0;
    hLocal->CPU.FLAGS.Z = FALSE;

    while ((dwsecond & 1) == 0)
    {
        temp++;
        dwsecond >>= 1;
    }

    set_reg(hLocal, mem_type,reg,temp);
}


/* 386 */

void BSR(PAMLHANDLE hLocal)
{
	BYTE amb, mem_type, opcode;
    WORD seg, off, reg;
    DWORD dwsecond, temp, mask;

	opcode = 1;

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */
    decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

    if (mem_type & DWORD_SIZE)
    {
        mask = 0x80000000L;
        temp = 31;
    }
    else
    {
        mask = 0x8000U;
        temp = 15;
    }

    dwsecond = gen_get_value(hLocal, mem_type, seg, off);

    if (0 == dwsecond)
    {
        hLocal->CPU.FLAGS.Z = TRUE;
        return;
    }

    hLocal->CPU.FLAGS.Z = FALSE;

    while ((dwsecond & mask) == 0)
    {
        temp--;
        dwsecond <<= 1;
    }

    set_reg(hLocal, mem_type,reg,temp);
}

/* 386 */

void BSWAP(PAMLHANDLE hLocal, BYTE opcode)
{
    DWORD dwreg, dwtemp;

    dwreg = reg_value(hLocal, DWORD_SIZE, (WORD)(opcode & 0x7));
    dwtemp = (dwreg >> 24) | ((dwreg >> 8) & 0xFF00U) |
             (dwreg << 24) | ((dwreg << 8) & 0x00FF0000L);

    set_reg(hLocal,DWORD_SIZE,(WORD)(opcode & 0x7), dwtemp);
}

/* 386 */

void BT(PAMLHANDLE hLocal, BYTE opcode, BYTE byFunc)      /* BT */
{
    BYTE amb, mem_type;
	WORD seg, off, reg;
	DWORD dwreg, dwmem;

	/* pass 1 in for the opcode field to specify WORD/DWORD operands always */

	amb = get_byte_at_csip(hLocal);

	decode_amb(hLocal, 1, amb, &mem_type, &seg, &off, &reg);

	if (BT_DETERMINE == byFunc)
		byFunc = REG_VALUE(amb);

	dwmem = gen_get_value(hLocal,mem_type,seg,off);

    if (BT_IMMED_OPCODE != opcode)
		dwreg = reg_value(hLocal,mem_type,reg);
	else
		dwreg = get_byte_at_csip(hLocal);       // 8 bit local parameter

	dwreg &= 0x1F;

	hLocal->CPU.FLAGS.C = !!(dwmem & ((DWORD)1 << dwreg));

	switch (byFunc)
    {
        case BT_CARRY:
            dwmem ^= ((DWORD)1 << dwreg);
            break;
        case BT_RESET:
            dwmem &= ~((DWORD)1 << dwreg);
            break;
        case BT_SET:
            dwmem |= ((DWORD)1 << dwreg);
            break;
        case BT_TEST:
            return;

        /* the other cases will cause a crash */
    }

    gen_put_value(hLocal,mem_type,seg,off,dwmem);
}


void CMPXCHG(PAMLHANDLE hLocal, BYTE opcode)
{
	BYTE amb;

    (void)opcode;

	amb = get_byte_at_csip(hLocal);

    /* invalid OPCODE in real mode/V86 */
}


/* LGDT, LMSW */

void LGDT(PAMLHANDLE hLocal)
{
	BYTE amb, mem_type;
	WORD seg, off, reg;

	/* just fetch the operands */
	/* use 1 for opcode - always word */

	amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */
	decode_amb(hLocal,1, amb, &mem_type, &seg, &off, &reg);
}


void LSREG(PAMLHANDLE hLocal, WORD wRegNum)
{
    BYTE amb, mem_type;
	WORD seg, off, reg, wSize, wval;
	DWORD dwval;

	amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */

    /* opcode is always WORD/DWORD, pass 1 in */

    decode_amb(hLocal, 1, amb, &mem_type, &seg, &off, &reg);

	if (hLocal->CPU.op_size_over)
	{
		wSize = sizeof(DWORD);
		mem_type = DWORD_SIZE;
	}
	else
	{
		wSize = sizeof(WORD);
		mem_type = WORD_SIZE;
	}

	dwval = gen_get_value(hLocal, mem_type,seg,off);
    set_reg(hLocal, mem_type,reg,dwval);

    wval = wget_value(hLocal, mem_type,seg,(WORD)(off+wSize));

    set_sreg(hLocal,(BYTE)wRegNum,wval);
}


void MOVSX(PAMLHANDLE hLocal, BYTE opcode)
{
	BYTE amb, mem_type;
	WORD seg, off, reg;
	DWORD dwval;

	amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */

    /* opcode is always WORD/DWORD, pass 1 in */

    decode_amb(hLocal, 1, amb, &mem_type, &seg, &off, &reg);

    if (opcode == MOVSX8_OPCODE)
		dwval = dsign_extend_byte(bget_value(hLocal,mem_type,seg,off));
	else
		dwval = sign_extend_word(wget_value(hLocal,mem_type,seg,off));

	set_reg(hLocal,mem_type,reg,dwval);
}


void MOVZX(PAMLHANDLE hLocal, BYTE opcode)
{
	BYTE amb, mem_type;
	WORD seg, off, reg;
	DWORD dwval;

	amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */

	/* opcode is always WORD/DWORD, pass 1 in */

	decode_amb(hLocal, 1, amb, &mem_type, &seg, &off, &reg);

	if (opcode == MOVZX8_OPCODE)
		dwval = bget_value(hLocal,mem_type,seg,off);
	else
		dwval = wget_value(hLocal,mem_type,seg,off);

	set_reg(hLocal,mem_type,reg,dwval);
}


void POP_FS(PAMLHANDLE hLocal)
{
    if (hLocal->CPU.op_size_over)
        hLocal->CPU.FS = (WORD)pop_dword(hLocal);
    else
        hLocal->CPU.FS = pop_word(hLocal);
}

void POP_GS(PAMLHANDLE hLocal)
{
    if (hLocal->CPU.op_size_over)
        hLocal->CPU.GS = (WORD)pop_dword(hLocal);
    else
        hLocal->CPU.GS = pop_word(hLocal);
}


void POPA(PAMLHANDLE hLocal)
{
    if (!hLocal->CPU.op_size_over)
    {
        hLocal->CPU.ireg.X.DI = pop_word(hLocal);
        hLocal->CPU.ireg.X.SI = pop_word(hLocal);
        hLocal->CPU.ireg.X.BP = pop_word(hLocal);

        hLocal->CPU.ireg.X.SP += sizeof(WORD);

		hLocal->CPU.preg.X.BX = pop_word(hLocal);
		hLocal->CPU.preg.X.DX = pop_word(hLocal);
		hLocal->CPU.preg.X.CX = pop_word(hLocal);
		hLocal->CPU.preg.X.AX = pop_word(hLocal);
	}
	else
	{
		hLocal->CPU.ireg.D.EDI = pop_dword(hLocal);
		hLocal->CPU.ireg.D.ESI = pop_dword(hLocal);
		hLocal->CPU.ireg.D.EBP = pop_dword(hLocal);

		hLocal->CPU.ireg.D.ESP += sizeof(DWORD);

		hLocal->CPU.preg.D.EBX = pop_dword(hLocal);
		hLocal->CPU.preg.D.EDX = pop_dword(hLocal);
		hLocal->CPU.preg.D.ECX = pop_dword(hLocal);
		hLocal->CPU.preg.D.EAX = pop_dword(hLocal);

	}
}


void PUSH_FS(PAMLHANDLE hLocal)
{
    if (hLocal->CPU.op_size_over)
        push_dword(hLocal,hLocal->CPU.FS);
    else
        push_word(hLocal,hLocal->CPU.FS);
}

void PUSH_GS(PAMLHANDLE hLocal)
{
    if (hLocal->CPU.op_size_over)
        push_dword(hLocal,hLocal->CPU.GS);
    else
        push_word(hLocal,hLocal->CPU.GS);
}


void PUSH_ALL(PAMLHANDLE hLocal)
{
    DWORD temp;

    if (!hLocal->CPU.op_size_over)
    {
        temp = hLocal->CPU.ireg.X.SP;

        push_word(hLocal,hLocal->CPU.preg.X.AX);
        push_word(hLocal,hLocal->CPU.preg.X.CX);
        push_word(hLocal,hLocal->CPU.preg.X.DX);
        push_word(hLocal,hLocal->CPU.preg.X.BX);
        push_word(hLocal,(WORD)temp);
        push_word(hLocal,hLocal->CPU.ireg.X.BP);
        push_word(hLocal,hLocal->CPU.ireg.X.SI);
        push_word(hLocal,hLocal->CPU.ireg.X.DI);

	}
	else
	{
        temp = hLocal->CPU.ireg.D.ESP;

        push_dword(hLocal,hLocal->CPU.preg.D.EAX);
        push_dword(hLocal,hLocal->CPU.preg.D.ECX);
        push_dword(hLocal,hLocal->CPU.preg.D.EDX);
        push_dword(hLocal,hLocal->CPU.preg.D.EBX);
        push_dword(hLocal,temp);
        push_dword(hLocal,hLocal->CPU.ireg.D.EBP);
        push_dword(hLocal,hLocal->CPU.ireg.D.ESI);
        push_dword(hLocal,hLocal->CPU.ireg.D.EDI);
    }

}


void SETcc(PAMLHANDLE hLocal, BYTE opcode)     /* short conditional jump */
{
    BYTE amb, mem_type;
    WORD seg, off, reg;
    BYTE byResult = 0;

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */
    decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

    switch (opcode)
    {
        case SETA_OPCODE:
            if (hLocal->CPU.FLAGS.C == FALSE && hLocal->CPU.FLAGS.Z == FALSE)
                byResult = TRUE;
            break;
        case SETAE_OPCODE:
            if (hLocal->CPU.FLAGS.C == FALSE)
                byResult = TRUE;
            break;
        case SETB_OPCODE:
            if (hLocal->CPU.FLAGS.C == TRUE)
                byResult = TRUE;
            break;
        case SETBE_OPCODE:
            if (hLocal->CPU.FLAGS.C == TRUE || hLocal->CPU.FLAGS.Z == TRUE)
                byResult = TRUE;
                break;
        case SETE_OPCODE:
            if (hLocal->CPU.FLAGS.Z == TRUE)
                byResult = TRUE;
            break;
        case SETG_OPCODE:
            if (hLocal->CPU.FLAGS.Z == FALSE && hLocal->CPU.FLAGS.S == hLocal->CPU.FLAGS.O)
                byResult = TRUE;
            break;
        case SETGE_OPCODE:
            if (hLocal->CPU.FLAGS.S == hLocal->CPU.FLAGS.O)
                byResult = TRUE;
            break;
        case SETL_OPCODE:
            if (hLocal->CPU.FLAGS.S != hLocal->CPU.FLAGS.O)
                byResult = TRUE;
            break;
        case SETLE_OPCODE:
            if (hLocal->CPU.FLAGS.Z == TRUE || hLocal->CPU.FLAGS.S != hLocal->CPU.FLAGS.O)
                byResult = TRUE;
            break;
        case SETNE_OPCODE:
            if (hLocal->CPU.FLAGS.Z == FALSE)
                byResult = TRUE;
            break;
        case SETNO_OPCODE:
            if (hLocal->CPU.FLAGS.O == FALSE)
                byResult = TRUE;
            break;
        case SETO_OPCODE:
            if (hLocal->CPU.FLAGS.O == TRUE)
                byResult = TRUE;
            break;
        case SETS_OPCODE:
            if (hLocal->CPU.FLAGS.S == TRUE)
                byResult = TRUE;
            break;
        case SETNS_OPCODE:
            if (hLocal->CPU.FLAGS.S == FALSE)
                byResult = TRUE;
            break;
        case SETNP_OPCODE:
            if (hLocal->CPU.FLAGS.P == FALSE)
                byResult = TRUE;
            break;
        case SETP_OPCODE:
            if (hLocal->CPU.FLAGS.P == TRUE)
                byResult = TRUE;
            break;

	}

	/* only allow byte operands */

	mem_type &= ~(WORD_SIZE | DWORD_SIZE);

	gen_put_value(hLocal,mem_type,seg,off,byResult);
}



void SHLD(PAMLHANDLE hLocal, BYTE opcode)
{
    WORD wShift, wSize;
    BYTE amb, mem_type;
	WORD seg, off, reg;
    DWORD dwInBits, dwBase, dwOrigValue;

    /* always WORD operands */

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */
    decode_amb(hLocal, 1, amb, &mem_type, &seg, &off, &reg);

    if (hLocal->CPU.op_size_over)
        wSize = 32;
    else
        wSize = 16;

    if (opcode == SHLD_CL)
        wShift = hLocal->CPU.preg.H.CL & 0x1F;
    else
        wShift = get_byte_at_csip(hLocal) & 0x1F;

    if (wShift == 0 || wShift > wSize)
        return;                             // nop or invalid shift

    dwInBits = reg_value(hLocal,mem_type,reg);
    dwOrigValue = dwBase = gen_get_value(hLocal,mem_type,seg,off);

    hLocal->CPU.FLAGS.C = !!(((DWORD)1 << (wSize - wShift)) & dwBase);

    dwBase <<= wShift;
    dwInBits >>= (wSize - wShift);
    dwBase |= dwInBits;

    if (wShift == 1 &&
        sign_status(mem_type,dwBase) !=
        sign_status(mem_type,dwOrigValue))
        hLocal->CPU.FLAGS.O = TRUE;
    else
        hLocal->CPU.FLAGS.O = FALSE;

	set_zero_flag(hLocal, mem_type, dwBase);
	set_sign_flag(hLocal, mem_type, dwBase);
    set_parity_flag(hLocal, mem_type, dwBase);

	gen_put_value(hLocal,mem_type,seg,off,dwBase);
}

void SHRD(PAMLHANDLE hLocal, BYTE opcode)
{
    WORD wShift, wSize;
    BYTE amb, mem_type;
	WORD seg, off, reg;
    DWORD dwInBits, dwBase, dwOrigValue;

    /* always WORD operands */

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */
    decode_amb(hLocal, 1, amb, &mem_type, &seg, &off, &reg);

    if (hLocal->CPU.op_size_over)
        wSize = 32;
    else
        wSize = 16;

	if (opcode == SHRD_CL)
        wShift = hLocal->CPU.preg.H.CL & 0x1F;
    else
        wShift = get_byte_at_csip(hLocal) & 0x1F;

    if (wShift == 0 || wShift > wSize)
        return;                             // nop or invalid shift

    dwInBits = reg_value(hLocal,mem_type,reg);
    dwOrigValue = dwBase = gen_get_value(hLocal,mem_type,seg,off);

    hLocal->CPU.FLAGS.C = !!(((DWORD)1 << (wShift - 1)) & dwBase);

    dwBase >>= wShift;
    dwInBits <<= (wSize - wShift);
    dwBase |= dwInBits;

    if (wShift == 1 &&
        sign_status(mem_type,dwBase) !=
        sign_status(mem_type,dwOrigValue))
        hLocal->CPU.FLAGS.O = TRUE;
    else
        hLocal->CPU.FLAGS.O = FALSE;

    set_zero_flag(hLocal, mem_type, dwBase);
	set_sign_flag(hLocal, mem_type, dwBase);
    set_parity_flag(hLocal, mem_type, dwBase);

    gen_put_value(hLocal,mem_type,seg,off,dwBase);
}



void XADD(PAMLHANDLE hLocal, BYTE opcode)
{
    BYTE amb, mem_type;
	WORD seg, off, reg;
    DWORD dwsrc, dwdest, dwtemp;

    /* always WORD operands */

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */
	decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

    dwdest = gen_get_value(hLocal,mem_type,seg,off);
    dwsrc = reg_value(hLocal,mem_type,reg);

	set_over_flag_add(hLocal,mem_type, dwsrc, dwdest, 0);
	set_carry_flag_add(hLocal,mem_type, dwsrc, dwdest, 0);

	dwtemp = dwsrc + dwdest;
	dwsrc = dwdest;
	dwdest = dwtemp;

	set_zero_flag(hLocal,mem_type,dwdest);
	set_sign_flag(hLocal,mem_type,dwdest);
    set_parity_flag(hLocal, mem_type, dwdest);

    gen_put_value(hLocal,mem_type,seg,off,dwdest);
    set_reg(hLocal,mem_type,reg,dwsrc);
}


void ARPL(PAMLHANDLE hLocal)
{
    WORD        wNumBytes;
    DWORD       dwOffset;
    PAMSTATUS   pamStatus;

    // this is used to fault into PAM for INT 21 calls...
    // treat this as a 1-byte instruction...

    if ((hLocal->dwFlags & LOCAL_FLAG_REPAIR_REPAIR) &&
        hLocal->CPU.CS == REPAIR_SEG)
	{
        // check for proper ARPL repair signature of 90 FF...

        if (get_byte(hLocal,hLocal->CPU.CS,hLocal->CPU.IP) == 0x90 &&
            get_byte(hLocal,hLocal->CPU.CS,hLocal->CPU.IP+1) == 0xFF)
        {
            hLocal->CPU.IP += 2;
            reset_prefetch_queue(hLocal);
        }
		else
		{
			return;
		}

        switch (hLocal->CPU.preg.H.AH)
        {
            case REPAIR_READ_FUNC:

                // find out where we are now in our repair file

                dwOffset = PAMFileSeek(hLocal->hRepairFile, 0, SEEK_CUR);

                if (dwOffset == (DWORD)-1)
                    hLocal->dwFlags |= LOCAL_FLAG_FILE_ERROR;

                pamStatus = copy_bytes_to_vm(hLocal,
                                             hLocal->CPU.preg.X.CX,
                                             dwOffset,
                                             hLocal->hRepairFile,
                                             hLocal->CPU.DS,
                                             hLocal->CPU.preg.X.DX,
                                             &wNumBytes);

                if (pamStatus != PAMSTATUS_OK)
                    hLocal->dwFlags |= LOCAL_FLAG_FILE_ERROR;

                hLocal->CPU.preg.X.AX = wNumBytes;

                hLocal->CPU.FLAGS.C = FALSE;

                break;

            case REPAIR_WRITE_FUNC:

                // find out where we are now in our repair file

                dwOffset = PAMFileSeek(hLocal->hRepairFile, 0, SEEK_CUR);

                if (dwOffset == (DWORD)-1)
                    hLocal->dwFlags |= LOCAL_FLAG_FILE_ERROR;

				if (0 == hLocal->CPU.preg.X.CX)
				{
					// truncate case

                    if (PAMFileWrite(hLocal->hRepairFile,&dwOffset,0) != 0)
                        hLocal->dwFlags |= LOCAL_FLAG_FILE_ERROR;

                    // make sure to indicate that we're writing 0 bytes...

                    wNumBytes = 0;
				}
				else
				{
                    pamStatus = copy_bytes_from_vm(hLocal,
                                                   hLocal->CPU.preg.X.CX,
                                                   dwOffset,
                                                   hLocal->hRepairFile,
                                                   hLocal->CPU.DS,
                                                   hLocal->CPU.preg.X.DX,
                                                   &wNumBytes);

                    if (pamStatus != PAMSTATUS_OK)
                        hLocal->dwFlags |= LOCAL_FLAG_FILE_ERROR;

				}

                hLocal->CPU.preg.X.AX = wNumBytes;
                hLocal->CPU.FLAGS.C = FALSE;

                break;

            case REPAIR_SEEK_FUNC:

				dwOffset = ((DWORD)hLocal->CPU.preg.X.CX << 16) +
						   hLocal->CPU.preg.X.DX;

                dwOffset = PAMFileSeek(hLocal->hRepairFile,
                                      dwOffset,
                                      hLocal->CPU.preg.H.AL);

                if (dwOffset == (DWORD)-1)
                    hLocal->dwFlags |= LOCAL_FLAG_FILE_ERROR;

                hLocal->CPU.preg.X.AX = (WORD)(dwOffset & 0xFFFFU);
                hLocal->CPU.preg.X.DX = (WORD)(dwOffset >> 16);

                hLocal->CPU.FLAGS.C = FALSE;

                break;

            case REPAIR_TERMINATE_FUNC:
                hLocal->repair_result =
                    hLocal->CPU.preg.H.AL | REPAIR_DONE_BIT;
                break;

            case REPAIR_WRITE_THRU_ON:
                hLocal->dwFlags |= LOCAL_FLAG_WRITE_THRU;
				break;

            case REPAIR_WRITE_THRU_OFF:
                hLocal->dwFlags &= ~(DWORD)LOCAL_FLAG_WRITE_THRU;
                break;
        }
    }

}
