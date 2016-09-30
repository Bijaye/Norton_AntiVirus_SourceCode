// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/EMULATOR/VCS/instr.cpv   1.25   04 Apr 1996 11:00:28   CNACHEN  $
//
// Description:
//
//  This source file contains routines for approximately half of the opcodes
//  supported by PAM's CPU emulator.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/EMULATOR/VCS/instr.cpv  $
// 
//    Rev 1.25   04 Apr 1996 11:00:28   CNACHEN
// Changed functions for IN AL, CONST and IN AL, DX instructions.  Now
// pass in opcode as an argument so we can set the EAX/AX/AL register to
// zero.
// 
// 
//    Rev 1.24   04 Mar 1996 16:01:36   CNACHEN
// Added #IFDEF'd cache support.
// 
//    Rev 1.23   16 Feb 1996 13:59:06   CNACHEN
// Made sure REP'ed instructions return on error.
// 
//    Rev 1.22   07 Feb 1996 13:32:58   CNACHEN
// Fixed 32-bit (ints) to (short ints)
// 
//    Rev 1.21   02 Feb 1996 11:44:22   CNACHEN
// Added new dwFlags and revamped all exclusion checking...
// 
// 
//    Rev 1.20   24 Jan 1996 13:35:22   DCHI
// Fixed AAA, AAD, AAS, DAA, DAS.
// 
//    Rev 1.19   23 Jan 1996 14:47:52   DCHI
// Do nothing for repz, repnz instructions if no_exclude is true.
// 
//    Rev 1.18   18 Jan 1996 16:21:04   CNACHEN
// Fixed DAA and DAS bugs...
// 
//    Rev 1.17   12 Jan 1996 12:05:40   CNACHEN
// Added floating point support (to skip over float instructions)
// 
//    Rev 1.16   10 Jan 1996 11:12:10   CNACHEN
// Added comment about REPZ/REPNZ not being allowed in excluded interrupt or
// fault routines.
// 
//    Rev 1.15   10 Jan 1996 11:10:42   CNACHEN
// Oopsie!
// 
// 
//    Rev 1.13   20 Dec 1995 12:30:24   CNACHEN
// Fixed long->short int conversion warnings.
// 
//    Rev 1.12   20 Dec 1995 12:19:18   CNACHEN
// Added 0F,AF, 69, 6B IMUL instructions...
// 
//    Rev 1.11   19 Dec 1995 19:08:04   CNACHEN
// Added prefetch queue support!
// 
// 
//    Rev 1.10   14 Dec 1995 13:02:46   CNACHEN
// Added support for control relinquishing under NLM...
// 
//    Rev 1.9   14 Dec 1995 10:49:26   CNACHEN
// Fixed repair stuff...
// 
//    Rev 1.8   14 Dec 1995 10:03:50   CNACHEN
// Fixed flags for interrupts (into) and initialization
// 
//    Rev 1.7   13 Dec 1995 10:14:16   CNACHEN
// Fixed NLM problems...
// 
//    Rev 1.6   15 Nov 1995 20:40:32   CNACHEN
// Fixed parity problems..
// 
//    Rev 1.5   19 Oct 1995 18:23:38   CNACHEN
// Initial revision... with comment header :)
// 
//************************************************************************


#include "pamapi.h"

/*
    Notes: parity flag is not implemented anywhere
*/


/* 386 */

void oc000100dw(PAMLHANDLE hLocal, BYTE opcode)        /* ADC reg/mem, reg/mem */
{
	BYTE amb, mem_type;
	WORD seg, off, reg;
	DWORD dwscratch, dwreg, dwmem;

	/* if D = 0 then REG field is source and dest will be memory/register.
	   if D = 1 then dest is a register (specified by REG) and source may
	   be memory or another register */

	amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */
    decode_amb(hLocal,opcode, amb, &mem_type, &seg, &off, &reg);
	if (D_VALUE(opcode) == 0)
	{
        dwreg = reg_value(hLocal,mem_type,reg);
        dwmem = gen_get_value(hLocal,mem_type, seg,off);
        dwscratch = dwmem + dwreg + hLocal->CPU.FLAGS.C;
		gen_put_value(hLocal,mem_type,seg,off,dwscratch);
	}
	else			/* D == 1 */
	{
        dwreg = reg_value(hLocal,mem_type,reg);
        dwmem = gen_get_value(hLocal,mem_type,seg,off);
        dwscratch = dwmem + dwreg + hLocal->CPU.FLAGS.C;
        set_reg(hLocal,mem_type,reg,dwscratch);
    }

    set_over_flag_add(hLocal,mem_type,dwmem,dwreg,hLocal->CPU.FLAGS.C);
	set_carry_flag_add(hLocal,mem_type,dwmem,dwreg,hLocal->CPU.FLAGS.C);
    set_zero_flag(hLocal,mem_type,dwscratch);
	set_sign_flag(hLocal,mem_type,dwscratch);
    set_parity_flag(hLocal,mem_type,dwscratch);
}



/* 386 */

void oc100000sw(PAMLHANDLE hLocal, BYTE opcode)            /* general */
{
	BYTE amb, mem_type;
	WORD seg, off, reg;
	DWORD dwscratch, dwreg, dwmem;

	amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */

	switch (SEC_OP(amb))
	{
		case 0:							/* 000: ADD reg/mem, immed */

			/* 386 */

			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			if (S_VALUE(opcode) == 0)
				dwmem = get_value_at_csip(hLocal, mem_type);
			else
			{
                if (W_VALUE(opcode) == 0)
					dwmem = get_byte_at_csip(hLocal);
				else
					dwmem = dsign_extend_byte(get_byte_at_csip(hLocal));
			}

			dwreg = gen_get_value(hLocal,mem_type, seg,off);
			dwscratch = dwmem + dwreg;
			gen_put_value(hLocal,mem_type,seg,off,dwscratch);


			set_over_flag_add(hLocal,mem_type,dwmem,dwreg, 0);
			set_carry_flag_add(hLocal,mem_type,dwmem, dwreg, 0);
			set_zero_flag(hLocal,mem_type,dwscratch);
			set_sign_flag(hLocal,mem_type,dwscratch);
            set_parity_flag(hLocal,mem_type,dwscratch);

			break;
		case 1:							/* 001: OR reg/mem, immed */

			/* 386 */

			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			if (S_VALUE(opcode) == 0)
				dwmem = get_value_at_csip(hLocal, mem_type);
			else
			{
				if (W_VALUE(opcode) == 0)
					dwmem = get_byte_at_csip(hLocal);
				else
					dwmem = dsign_extend_byte(get_byte_at_csip(hLocal));
			}

			dwreg = gen_get_value(hLocal,mem_type, seg,off);
			dwscratch = dwmem | dwreg;
			gen_put_value(hLocal,mem_type,seg,off,dwscratch);

			set_zero_flag(hLocal,mem_type,dwscratch);
			set_sign_flag(hLocal,mem_type,dwscratch);
            set_parity_flag(hLocal,mem_type,dwscratch);
			hLocal->CPU.FLAGS.O = hLocal->CPU.FLAGS.C = FALSE;

			break;


        case 2:                         /* 010: ADC reg/mem, immed */

			/* 386 */

			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			if (S_VALUE(opcode) == 0)
				dwmem = get_value_at_csip(hLocal, mem_type);
			else
			{
				if (W_VALUE(opcode) == 0)
					dwmem = get_byte_at_csip(hLocal);
				else
					dwmem = dsign_extend_byte(get_byte_at_csip(hLocal));
			}

			dwreg = gen_get_value(hLocal,mem_type, seg,off);
			dwscratch = dwmem + dwreg + hLocal->CPU.FLAGS.C;
			gen_put_value(hLocal,mem_type,seg,off,dwscratch);


			set_over_flag_add(hLocal,
							  mem_type,
							  dwmem,
							  dwreg,
							  hLocal->CPU.FLAGS.C);
			set_carry_flag_add(hLocal,
							   mem_type,
							   dwmem,
							   dwreg,
							   hLocal->CPU.FLAGS.C);
			set_zero_flag(hLocal,mem_type,dwscratch);
			set_sign_flag(hLocal,mem_type,dwscratch);
            set_parity_flag(hLocal,mem_type,dwscratch);

			break;

		case 3:							/* 011: SBB reg/mem, immed */

			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			if (S_VALUE(opcode) == 0)
				dwmem = get_value_at_csip(hLocal, mem_type);
			else
			{
				if (W_VALUE(opcode) == 0)
					dwmem = get_byte_at_csip(hLocal);
				else
					dwmem = dsign_extend_byte(get_byte_at_csip(hLocal));
			}

			dwreg = gen_get_value(hLocal,mem_type, seg,off);
			dwscratch = dwreg - dwmem - hLocal->CPU.FLAGS.C;
			gen_put_value(hLocal,mem_type,seg,off,dwscratch);


			set_over_flag_sub(hLocal,
							  mem_type,
							  dwreg,
							  dwmem,
							  hLocal->CPU.FLAGS.C);
			set_carry_flag_sub(hLocal,
							   mem_type,
							   dwreg,
							   dwmem,
							   hLocal->CPU.FLAGS.C);
			set_zero_flag(hLocal,mem_type,dwscratch);
			set_sign_flag(hLocal,mem_type,dwscratch);
            set_parity_flag(hLocal,mem_type,dwscratch);

			break;


		case 4:							/* 100: AND reg/mem, immed */

			/* 386 */

			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			if (S_VALUE(opcode) == 0)
				dwmem = get_value_at_csip(hLocal, mem_type);
			else
			{
				if (W_VALUE(opcode) == 0)
					dwmem = get_byte_at_csip(hLocal);
				else
					dwmem = dsign_extend_byte(get_byte_at_csip(hLocal));
			}

			dwreg = gen_get_value(hLocal,mem_type, seg,off);
            dwscratch = dwmem & dwreg;
			gen_put_value(hLocal,mem_type,seg,off,dwscratch);

			/* flag stuff */

			set_sign_flag(hLocal,mem_type,dwscratch);
			set_zero_flag(hLocal,mem_type,dwscratch);
            set_parity_flag(hLocal,mem_type,dwscratch);

			/* set OF and CF to 0 regardless */

			hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = FALSE;

			/* by definition */

			break;

		case 5:							/* 101:	SUB reg/mem, immed */

			/* 386 */

			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

            if (S_VALUE(opcode) == 0)
				dwmem = get_value_at_csip(hLocal, mem_type);
			else
			{
				if (W_VALUE(opcode) == 0)
					dwmem = get_byte_at_csip(hLocal);
				else
					dwmem = dsign_extend_byte(get_byte_at_csip(hLocal));
			}

			dwreg = gen_get_value(hLocal,mem_type, seg,off);
            dwscratch = dwreg - dwmem;
			gen_put_value(hLocal,mem_type,seg,off,dwscratch);


            set_over_flag_sub(hLocal,mem_type,dwreg,dwmem,0);
			set_carry_flag_sub(hLocal,mem_type,dwreg,dwmem,0);
			set_zero_flag(hLocal,mem_type,dwscratch);
            set_sign_flag(hLocal,mem_type,dwscratch);
            set_parity_flag(hLocal,mem_type,dwscratch);

            break;

		case 6:					/* 110: XOR reg/mem, immed */

			/* 386 */

			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			if (S_VALUE(opcode) == 0)
				dwmem = get_value_at_csip(hLocal, mem_type);
			else
			{
				if (W_VALUE(opcode) == 0)
					dwmem = get_byte_at_csip(hLocal);
				else
					dwmem = dsign_extend_byte(get_byte_at_csip(hLocal));
			}

			dwreg = gen_get_value(hLocal,mem_type, seg,off);
			dwscratch = dwmem ^ dwreg;
			gen_put_value(hLocal,mem_type,seg,off,dwscratch);

			/* flag stuff */

			set_sign_flag(hLocal,mem_type,dwscratch);
			set_zero_flag(hLocal,mem_type,dwscratch);
            set_parity_flag(hLocal,mem_type,dwscratch);

			/* set OF and CF to 0 regardless */

			hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = FALSE;

			/* by definition */

			break;

		case 7:							/* 111: CMP reg/mem, immed */

			/* 386 */

			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

			if (S_VALUE(opcode) == 0)
				dwmem = get_value_at_csip(hLocal, mem_type);
			else
			{
				if (W_VALUE(opcode) == 0)
					dwmem = get_byte_at_csip(hLocal);
				else
					dwmem = dsign_extend_byte(get_byte_at_csip(hLocal));
			}

			dwreg = gen_get_value(hLocal,mem_type, seg,off);
			dwscratch = dwreg - dwmem;

			set_over_flag_sub(hLocal,mem_type,dwreg,dwmem,0);
			set_carry_flag_sub(hLocal,mem_type,dwreg,dwmem,0);
			set_zero_flag(hLocal,mem_type,dwscratch);
			set_sign_flag(hLocal,mem_type,dwscratch);
            set_parity_flag(hLocal,mem_type,dwscratch);

			break;
	}
}

/* 386 */

void oc0001010w(PAMLHANDLE hLocal, BYTE opcode)        /* ADC accum, immed */
{
	WORD wMemSize;
	DWORD dwscratch, dwreg, dwmem;

    if (W_VALUE(opcode) == 0)
		wMemSize = 0;
	else if (hLocal->CPU.op_size_over)
		wMemSize = DWORD_SIZE;
	else
		wMemSize = WORD_SIZE;

	/* REG_AL == REG_AX == REG_EAX */

    dwreg = reg_value(hLocal,wMemSize,REG_AL);
    dwmem = get_value_at_csip(hLocal,wMemSize);
    dwscratch = dwmem + dwreg + hLocal->CPU.FLAGS.C;

    set_reg(hLocal,wMemSize,REG_AL,dwscratch);

    set_over_flag_add(hLocal,wMemSize, dwmem,dwreg, hLocal->CPU.FLAGS.C);
	set_carry_flag_add(hLocal,wMemSize, dwmem, dwreg, hLocal->CPU.FLAGS.C);
    set_zero_flag(hLocal, wMemSize, dwscratch);
    set_sign_flag(hLocal, wMemSize, dwscratch);
    set_parity_flag(hLocal,wMemSize,dwscratch);
}

/* 386 */

void oc000000dw(PAMLHANDLE hLocal, BYTE opcode)   /* ADD reg/mem, reg/mem */
{
    BYTE amb, mem_type;
    WORD seg, off, reg;
    DWORD dwmem, dwreg, dwscratch;

	/* if D = 0 then REG field is source and dest will be memory/register.
	   if D = 1 then dest is a register (specified by REG) and source may
	   be memory or another register */

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */
    decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

    dwreg = reg_value(hLocal,mem_type,reg);
	dwmem = gen_get_value(hLocal,mem_type,seg,off);
	dwscratch = dwmem + dwreg;

    if (D_VALUE(opcode) == 0)
        gen_put_value(hLocal, mem_type,seg,off,dwscratch);
    else
		set_reg(hLocal,mem_type, reg, dwscratch);

    set_over_flag_add(hLocal,mem_type,dwmem,dwreg, 0);
	set_carry_flag_add(hLocal,mem_type,dwmem, dwreg, 0);
    set_zero_flag(hLocal,mem_type,dwscratch);
    set_sign_flag(hLocal,mem_type,dwscratch);
    set_parity_flag(hLocal,mem_type,dwscratch);
}

/* 386 */

void oc0000010w(PAMLHANDLE hLocal, BYTE opcode)        /* ADD accum, immed */
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

	dwreg = reg_value(hLocal,wMemSize, REG_AL);
    dwmem = get_value_at_csip(hLocal,wMemSize);
    dwscratch = dwmem + dwreg;
    set_reg(hLocal,wMemSize,REG_AL,dwscratch);

    set_over_flag_add(hLocal,wMemSize,dwmem,dwreg, 0);
	set_carry_flag_add(hLocal,wMemSize,dwmem, dwreg, 0);
    set_zero_flag(hLocal,wMemSize,dwscratch);
    set_sign_flag(hLocal,wMemSize,dwscratch);
    set_parity_flag(hLocal,wMemSize,dwscratch);
}


/* 386 */

void oc001000dw(PAMLHANDLE hLocal, BYTE opcode)            /* AND reg/mem, reg/mem */
{
	BYTE amb, mem_type;
    WORD seg, off, reg;
    DWORD dwreg, dwmem, dwscratch;

	/* if D = 0 then REG field is source and dest will be memory/register.
	   if D = 1 then dest is a register (specified by REG) and source may
	   be memory or another register */

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */

    decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

	dwreg = reg_value(hLocal,mem_type, reg);
    dwmem = gen_get_value(hLocal,mem_type,seg,off);
    dwscratch = dwmem & dwreg;

	if (D_VALUE(opcode) == 0)
        gen_put_value(hLocal, mem_type,seg,off,dwscratch);
    else
        set_reg(hLocal,mem_type, reg, dwscratch);


	/* set OF and CF to 0 regardless */

    hLocal->CPU.FLAGS.O = hLocal->CPU.FLAGS.C = 0;      /* by definition */

	set_sign_flag(hLocal,mem_type,dwscratch);
	set_zero_flag(hLocal,mem_type,dwscratch);
    set_parity_flag(hLocal,mem_type,dwscratch);
}


/* 386 */

void oc0010010w(PAMLHANDLE hLocal, BYTE opcode)        /* AND accum, immed */
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

	dwreg = reg_value(hLocal,wMemSize, REG_AL);
	dwmem = get_value_at_csip(hLocal,wMemSize);
	dwscratch = dwmem & dwreg;
	set_reg(hLocal,wMemSize,REG_AL,dwscratch);

	set_sign_flag(hLocal,wMemSize,dwscratch);
	set_zero_flag(hLocal,wMemSize,dwscratch);
    set_parity_flag(hLocal,wMemSize,dwscratch);

	/* set OF and CF to 0 regardless */

	hLocal->CPU.FLAGS.O = hLocal->CPU.FLAGS.C = 0;      /* by definition */
}

/* 386 - no overrides possible */

void oc11101000(PAMLHANDLE hLocal)               /* call near direct */
{
	WORD disp;

    disp = get_word_at_csip(hLocal);        /* find where we're going */
    push_word(hLocal, hLocal->CPU.IP);      /* save offset of start of next instr */
    hLocal->CPU.IP += disp;                 /* set IP where to jump */

    reset_prefetch_queue(hLocal);

    // now check for exclusions...

    if ((short int)disp > 128 || (short int)disp < -128)
        exclude_item(hLocal,CALL128);
}



/* 386 */

void oc11111111(PAMLHANDLE hLocal, BYTE opcode)  /* call/inc/dec instructions */
{
	BYTE amb, mem_type;
	WORD seg, off, reg, wnew_ip, wnew_cs, wip, wcs, wval;
    DWORD dwval, dwscratch;

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */

	switch (SEC_OP(amb))
	{
		case 0:							/* 000: inc 16 bit operand */

			/* 386 */

			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);
			dwval = gen_get_value(hLocal,mem_type,seg,off);

			dwscratch = dwval + 1;

			set_over_flag_add(hLocal,mem_type, dwval, 1, 0);
			set_zero_flag(hLocal,mem_type,dwscratch);
			set_sign_flag(hLocal,mem_type,dwscratch);
			dwset_aux_carry_flag_add(hLocal,dwval,1,0);
			set_parity_flag(hLocal,mem_type,dwscratch);

			gen_put_value(hLocal,mem_type,seg,off,dwscratch);
			break;

		case 1:							/* 001: dec 16 bit operand */

			/* 386 */

			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);
			dwval = gen_get_value(hLocal,mem_type,seg,off);

			dwscratch = dwval - 1;

			set_over_flag_sub(hLocal, mem_type, dwval, 1, 0);
			set_zero_flag(hLocal,mem_type,dwscratch);
			set_sign_flag(hLocal,mem_type,dwscratch);
			dwset_aux_carry_flag_sub(hLocal,dwval,1,0);
			set_parity_flag(hLocal,mem_type,dwscratch);

			gen_put_value(hLocal,mem_type,seg,off,dwscratch);
			break;

		case 2:							/* 010: call near indirect */

			/* 386 */

			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);
            wnew_ip = wget_value(hLocal,mem_type,seg,off);
            push_word(hLocal, hLocal->CPU.IP);      /* save offset of start of next instr */
            hLocal->CPU.IP = wnew_ip;

            reset_prefetch_queue(hLocal);

			break;
		case 3:							/* 011: call far indirect */

            /* 386 */

            decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);
            wnew_ip = wget_value(hLocal,mem_type,seg,off);     /* offset */
            wnew_cs = wget_value(hLocal,
                                 mem_type,
                                 seg,
                                 (WORD)(off + sizeof(WORD)));
            push_word(hLocal, hLocal->CPU.IP);
            hLocal->CPU.CS = wnew_cs;
            hLocal->CPU.IP = wnew_ip;       /* if CS:IP > fileimage then this executes out of stack temp file */

            reset_prefetch_queue(hLocal);

			break;

		case 4:							/* 100: jmp reg/mem near indirect */
			/* 386 */

            decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);
            wval = wget_value(hLocal,mem_type,seg,off);
            hLocal->CPU.IP = wval;

            reset_prefetch_queue(hLocal);

			break;
		case 5:							/* 101: jmp far indirect */

            /* 386 - no changes (this does funky stuff when prefixed by 66/7) */

            decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);
            wip = wget_value(hLocal,mem_type,seg,off);
            wcs = wget_value(hLocal,mem_type,seg,(WORD)(off+ sizeof(WORD)));
            hLocal->CPU.IP = wip;
            hLocal->CPU.CS = wcs;

            reset_prefetch_queue(hLocal);

			break;
		case 6:							/* 110: push reg/mem */

            /* 386 */

            decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);
            dwval = gen_get_value(hLocal,mem_type,seg,off);
            push_value(hLocal, mem_type,dwval);
			break;
        case 7:

            // invalid case; causes crash in real mode...

            if (!(hLocal->dwFlags &
                 (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
                hLocal->dwFlags |= LOCAL_FLAG_NO_CAND_LEFT;
            break;
    }
}

/* 386 - funky stuff happens when prefixed on real machine */

void oc10011010(PAMLHANDLE hLocal)               /* call far direct */
{
	WORD ip_val, cs_val;

    ip_val = get_word_at_csip(hLocal);
    cs_val = get_word_at_csip(hLocal);
    push_word(hLocal, hLocal->CPU.IP);              /* where to return to */
    push_word(hLocal, hLocal->CPU.CS);
    hLocal->CPU.CS = cs_val;                /* if new CS:IP > file size, this */
    hLocal->CPU.IP = ip_val;                /* executes data in the temp stack */

    reset_prefetch_queue(hLocal);

									/* file */
}

/* 386 */

void oc10011000(PAMLHANDLE hLocal)               /* CBD - no flags */
{
    if (hLocal->CPU.op_size_over)
		hLocal->CPU.preg.D.EAX = sign_extend_word(hLocal->CPU.preg.X.AX);
    else
        hLocal->CPU.preg.X.AX = sign_extend_byte(hLocal->CPU.preg.H.AL);
}

/* 386 */

void oc11111000(PAMLHANDLE hLocal)               /* CLC - clear carry flag */
{
	hLocal->CPU.FLAGS.C = FALSE;
}

/* 386 */

void oc11111100(PAMLHANDLE hLocal)               /* CLD - clear carry flag */
{
	hLocal->CPU.FLAGS.D = FALSE;
}


/* 386 */

void oc11111010(PAMLHANDLE hLocal)               /* CLI - clear carry flag */
{
	hLocal->CPU.FLAGS.I = FALSE;
}

/* 386 */

void oc11110101(PAMLHANDLE hLocal)               /* CMC - complement carry flag */
{
	hLocal->CPU.FLAGS.C = !hLocal->CPU.FLAGS.C;
}


/* 386 */

void oc001110dw(PAMLHANDLE hLocal, BYTE opcode)        /* CMP reg/mem, reg/mem */
{
	BYTE amb, mem_type;
	WORD seg, off, reg;
	DWORD dwreg, dwmem, dwscratch;

	/* if D = 0 then REG field is source and dest will be memory/register.
	   if D = 1 then dest is a register (specified by REG) and source may
	   be memory or another register */

	amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */
	decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);

	dwreg = reg_value(hLocal,mem_type, reg);
	dwmem = gen_get_value(hLocal,mem_type,seg,off);

	if (D_VALUE(opcode) == 0)
	{
		dwscratch = dwmem - dwreg;

		set_over_flag_sub(hLocal, mem_type, dwmem, dwreg, 0);
		set_carry_flag_sub(hLocal, mem_type, dwmem, dwreg, 0);
		set_zero_flag(hLocal,mem_type,dwscratch);
		set_sign_flag(hLocal,mem_type,dwscratch);
        set_parity_flag(hLocal,mem_type,dwscratch);
	}
	else			/* D == 1 */
	{
		dwscratch = dwreg - dwmem;

		set_carry_flag_sub(hLocal, mem_type, dwreg, dwmem, 0);
		set_over_flag_sub(hLocal, mem_type, dwreg,dwmem, 0);
		set_zero_flag(hLocal,mem_type,dwscratch);
		set_sign_flag(hLocal,mem_type,dwscratch);
        set_parity_flag(hLocal,mem_type,dwscratch);
	}
}

/* 386 */

void oc0011110w(PAMLHANDLE hLocal, BYTE opcode)                /* CMP accum, immed */
{
	DWORD dwscratch, dwreg, dwmem;
	WORD wMemSize;

	if (W_VALUE(opcode) == 0)       /* byte op, AL = AL + IMMED + Cflag */
		wMemSize = 0;
    else if (hLocal->CPU.op_size_over)
		wMemSize = DWORD_SIZE;
	else
		wMemSize = WORD_SIZE;

	/* REG_AL == REG_AX == REG_EAX */

	dwreg = reg_value(hLocal,wMemSize, REG_AL);
	dwmem = get_value_at_csip(hLocal,wMemSize);
	dwscratch = dwreg - dwmem;

	set_carry_flag_sub(hLocal, wMemSize, dwreg, dwmem, 0);
	set_over_flag_sub(hLocal, wMemSize, dwreg,dwmem, 0);
	set_zero_flag(hLocal,wMemSize,dwscratch);
	set_sign_flag(hLocal,wMemSize,dwscratch);
    set_parity_flag(hLocal,wMemSize,dwscratch);
}

/* 386 */

void oc10011001(PAMLHANDLE hLocal)               /* CWD, CDQ - no flags */
{
    if (hLocal->CPU.op_size_over)
    {
        if (dsign_status(hLocal->CPU.preg.D.EAX))
            hLocal->CPU.preg.D.EDX = 0xffffffffL;
        else
            hLocal->CPU.preg.D.EDX = 0;
    }
    else
    {
        if (wsign_status(hLocal->CPU.preg.X.AX))
            hLocal->CPU.preg.X.DX = 0xffffU;
        else
            hLocal->CPU.preg.X.DX = 0;
    }
}

/* 386 */

void oc11111110(PAMLHANDLE hLocal, BYTE opcode)        /* general */
{
	BYTE amb, mem_type;
	WORD seg, off, reg;
	BYTE bval, bscratch;

    amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */

    switch (SEC_OP(amb))
	{
		case 0:						/* 000: inc 8 bit */
            decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);
            bval = bget_value(hLocal,mem_type,seg,off);

			bscratch = bval + 1;

            bset_over_flag_add(hLocal,bval, 1, 0);
            bset_zero_flag(bscratch);
            bset_sign_flag(bscratch);
			dwset_aux_carry_flag_add(hLocal,bval,1,0);
			set_parity_flag(hLocal,FALSE,bscratch);     // FALSE=BYTE_SIZE

			bput_value(hLocal,mem_type,seg,off,bscratch);
			break;

		case 1:						/* 001: dec 8 bit */
			decode_amb(hLocal, opcode, amb, &mem_type, &seg, &off, &reg);
			bval = bget_value(hLocal,mem_type,seg,off);

			bscratch = bval - 1;

			bset_over_flag_sub(hLocal, bval, 1, 0);
			bset_zero_flag(bscratch);
			bset_sign_flag(bscratch);
			dwset_aux_carry_flag_sub(hLocal,bval,1,0);
            set_parity_flag(hLocal,FALSE,bscratch);     // FALSE=BYTE_SIZE

            bput_value(hLocal,mem_type,seg,off,bscratch);
			break;

        default:
            /* the other opcodes cause a fault and stop execution */

            if (!(hLocal->dwFlags &
                 (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
                hLocal->dwFlags |= LOCAL_FLAG_NO_CAND_LEFT;

            break;
    }
}


/* 386 */

void oc1010011w(PAMLHANDLE hLocal, BYTE opcode)       /* cmps* */
{
    DWORD dw1, dw2;
    WORD wMemSize, wSkip;

	if (hLocal->CPU.rep_prefix)
	{
		rep_instruction(hLocal, oc1010011w, opcode);
		return;
	}

    if (W_VALUE(opcode) == 0)
    {
        wSkip = sizeof(BYTE);
		wMemSize = 0;
    }
	else if (hLocal->CPU.op_size_over)
    {
        wSkip = sizeof(DWORD);
		wMemSize = DWORD_SIZE;
    }
	else
    {
        wMemSize = WORD_SIZE;
        wSkip = sizeof(WORD);
    }

    dw1 = gen_get_value(hLocal,
                        wMemSize,
                        hLocal->CPU.DS,
                        hLocal->CPU.ireg.X.SI);  /* DS:SI default */

	dw2 = gen_get_value(hLocal,
						wMemSize,
						hLocal->CPU.ES,
						hLocal->CPU.ireg.X.DI);  /* ES:DI default */

    set_carry_flag_sub(hLocal, wMemSize, dw1, dw2, 0);
    set_over_flag_sub(hLocal, wMemSize, dw1,dw2, 0);
    set_zero_flag(hLocal, wMemSize, dw1-dw2);
    set_sign_flag(hLocal, wMemSize, dw1-dw2);
    set_parity_flag(hLocal,wMemSize,dw1-dw2);

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

/* new 386 stuff */

void oc01100110(PAMLHANDLE hLocal)      /* (32-bit operand): prefix */
{
    hLocal->CPU.op_size_over = TRUE;
    hLocal->CPU.reset_seg = FALSE;
}

void oc01100111(PAMLHANDLE hLocal)      /* (32-bit address): prefix: */
{
    hLocal->CPU.addr_size_over = TRUE;
    hLocal->CPU.reset_seg = FALSE;
}


/* REP stuff */

int rep_cmp_instr(BYTE opcode)
{
    switch (opcode)
    {
        case oc10100110b:
        case oc10100111b:
        case oc10101110b:
        case oc10101111b:
            return(TRUE);

        default:

            return(FALSE);
    }
}


//////////////////////////////////////////////////////////////////////////////
// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
//
// REP prefixes should NOT be used inside of EXCLUDED interrupt or fault
// routines.
//
//////////////////////////////////////////////////////////////////////////////

void rep_instruction(PAMLHANDLE hLocal,
                     void (*string_func)(PAMLHANDLE, BYTE),
                     BYTE opcode)
{
    if (hLocal->CPU.rep_prefix == PREFIX_REPZ)
        repz_instruction(hLocal, string_func, opcode);
    else
        repnz_instruction(hLocal, string_func, opcode);
}

void repnz_instruction(PAMLHANDLE hLocal,
                       void (*string_func)(PAMLHANDLE, BYTE),
                       BYTE opcode)
{
    BYTE    test_z;

#ifdef USE_CACHE

    DWORD   dwCacheCheckIter =
                hLocal->hGPAM->config_info.dwCacheCheckIter;

#endif

    hLocal->CPU.rep_prefix = 0;

    test_z = rep_cmp_instr(opcode);

    if (hLocal->CPU.addr_size_over)
    {
        while (hLocal->CPU.preg.D.ECX != 0)
        {
            string_func(hLocal, opcode);
            hLocal->CPU.preg.D.ECX--;            /* one less to do */

            if (hLocal->dwFlags & LOCAL_FLAG_ERROR)
                return;

            if (hLocal->dwFlags & LOCAL_FLAG_IMMUNE)
            {
                hLocal->CPU.immune_iteration++;

                if (hLocal->CPU.immune_iteration >=
                    hLocal->CPU.max_immune_iteration)
                    return;
            }
            else
            {
                hLocal->CPU.iteration++;

#ifdef USE_CACHE

                // Check cpu cache

                if (hLocal->CPU.iteration == dwCacheCheckIter &&
                    CacheCheckPresence(hLocal) == TRUE)
                {
                    // Reset the prefetch queue because upon
                    // returning to the interpretation loop,
                    // the call to get_byte_at_csip() may fetch
                    // a modified byte from the prefetch queue
                    // and thus set wNextRequest to the next
                    // prefetch queue size overwriting the
                    // assignment below of PREFETCH_NO_REQUEST.

                    reset_prefetch_queue(hLocal);
                    hLocal->CPU.prefetch.wNextRequest = PREFETCH_NO_REQUEST;
                    hLocal->dwFlags |= LOCAL_FLAG_NO_CAND_LEFT;
                    return;
                }

#endif // USE_CACHE

                if (hLocal->CPU.iteration >= hLocal->CPU.max_iteration)
                    return;
            }

#ifdef SYM_NLM
            if (hLocal->dwFlags & LOCAL_FLAG_IMMUNE)
            {
                if (hLocal->CPU.immune_iteration >
                    hLocal->CPU.next_nlm_immune_iter_stop)
                {
                    // relinquish control to other NLM's on NLM platforms

                    hLocal->CPU.next_nlm_immune_iter_stop +=
                        NLM_ITER_CHECK_POINT;

                    PAMRelinquishControl();
                }
            }
            else
            {
                if (hLocal->CPU.iteration > hLocal->CPU.next_nlm_iter_stop)
                {
                    // relinquish control to other NLM's on NLM platforms

                    hLocal->CPU.next_nlm_iter_stop += NLM_ITER_CHECK_POINT;
                    PAMRelinquishControl();
                }
            }
#endif

            
            if (test_z && hLocal->CPU.FLAGS.Z == TRUE)
                return;
        }
    }
    else
    {
        while (hLocal->CPU.preg.X.CX != 0)
        {
            string_func(hLocal, opcode);
            hLocal->CPU.preg.X.CX--;            /* one less to do */

            if (hLocal->dwFlags & LOCAL_FLAG_ERROR)
                return;

            if (hLocal->dwFlags & LOCAL_FLAG_IMMUNE)
            {
                hLocal->CPU.immune_iteration++;

                if (hLocal->CPU.immune_iteration >=
                    hLocal->CPU.max_immune_iteration)
                    return;
            }
            else
            {
                hLocal->CPU.iteration++;

#ifdef USE_CACHE

                // Check cpu cache

                if (hLocal->CPU.iteration == dwCacheCheckIter &&
                    CacheCheckPresence(hLocal) == TRUE)
                {
                    // Reset the prefetch queue because upon
                    // returning to the interpretation loop,
                    // the call to get_byte_at_csip() may fetch
                    // a modified byte from the prefetch queue
                    // and thus set wNextRequest to the next
                    // prefetch queue size overwriting the
                    // assignment below of PREFETCH_NO_REQUEST.

                    reset_prefetch_queue(hLocal);
                    hLocal->CPU.prefetch.wNextRequest = PREFETCH_NO_REQUEST;
                    hLocal->dwFlags |= LOCAL_FLAG_NO_CAND_LEFT;
                    return;
                }

#endif // USE_CACHE

                if (hLocal->CPU.iteration >= hLocal->CPU.max_iteration)
                    return;
            }

#ifdef SYM_NLM
            if (hLocal->dwFlags & LOCAL_FLAG_IMMUNE)
            {
                if (hLocal->CPU.immune_iteration >
                    hLocal->CPU.next_nlm_immune_iter_stop)
                {
                    // relinquish control to other NLM's on NLM platforms

                    hLocal->CPU.next_nlm_immune_iter_stop +=
                        NLM_ITER_CHECK_POINT;

                    PAMRelinquishControl();
                }
            }
            else
            {
                if (hLocal->CPU.iteration > hLocal->CPU.next_nlm_iter_stop)
                {
                    // relinquish control to other NLM's on NLM platforms

                    hLocal->CPU.next_nlm_iter_stop += NLM_ITER_CHECK_POINT;
                    PAMRelinquishControl();
                }
            }
#endif

            if (test_z && hLocal->CPU.FLAGS.Z == TRUE)
                return;
        }
    }
}



void repz_instruction(PAMLHANDLE hLocal,
                      void (*string_func)(PAMLHANDLE, BYTE),
					  BYTE opcode)                              /* repz */
{
    BYTE    test_z;

#ifdef USE_CACHE

    DWORD   dwCacheCheckIter =
                hLocal->hGPAM->config_info.dwCacheCheckIter;

#endif // USE_CACHE

    hLocal->CPU.rep_prefix = 0;

    test_z = rep_cmp_instr(opcode);

    if (hLocal->CPU.addr_size_over)
    {
        while (hLocal->CPU.preg.D.ECX != 0)
        {
            string_func(hLocal, opcode);
            hLocal->CPU.preg.D.ECX--;           /* one less to do */

            if (hLocal->dwFlags & LOCAL_FLAG_ERROR)
                return;

            if (hLocal->dwFlags & LOCAL_FLAG_IMMUNE)
            {
                hLocal->CPU.immune_iteration++;

                if (hLocal->CPU.immune_iteration >=
                    hLocal->CPU.max_immune_iteration)
                    return;
            }
            else
            {
                hLocal->CPU.iteration++;

#ifdef USE_CACHE

                // Check cpu cache

                if (hLocal->CPU.iteration == dwCacheCheckIter &&
                    CacheCheckPresence(hLocal) == TRUE)
                {
                    // Reset the prefetch queue because upon
                    // returning to the interpretation loop,
                    // the call to get_byte_at_csip() may fetch
                    // a modified byte from the prefetch queue
                    // and thus set wNextRequest to the next
                    // prefetch queue size overwriting the
                    // assignment below of PREFETCH_NO_REQUEST.

                    reset_prefetch_queue(hLocal);
                    hLocal->CPU.prefetch.wNextRequest = PREFETCH_NO_REQUEST;
                    hLocal->dwFlags |= LOCAL_FLAG_NO_CAND_LEFT;
                    return;
                }

#endif // USE_CACHE

                if (hLocal->CPU.iteration >= hLocal->CPU.max_iteration)
                    return;
            }

#ifdef SYM_NLM
            if (hLocal->dwFlags & LOCAL_FLAG_IMMUNE)
            {
                if (hLocal->CPU.immune_iteration >
                    hLocal->CPU.next_nlm_immune_iter_stop)
                {
                    // relinquish control to other NLM's on NLM platforms

                    hLocal->CPU.next_nlm_immune_iter_stop +=
                        NLM_ITER_CHECK_POINT;

                    PAMRelinquishControl();
                }
            }
            else
            {
                if (hLocal->CPU.iteration > hLocal->CPU.next_nlm_iter_stop)
                {
                    // relinquish control to other NLM's on NLM platforms

                    hLocal->CPU.next_nlm_iter_stop += NLM_ITER_CHECK_POINT;
                    PAMRelinquishControl();
                }
            }
#endif



			if (test_z && hLocal->CPU.FLAGS.Z == FALSE)
				return;
		}
	}
	else
	{
		while (hLocal->CPU.preg.X.CX != 0)
		{
			string_func(hLocal, opcode);
			hLocal->CPU.preg.X.CX--;            /* one less to do */

            if (hLocal->dwFlags & LOCAL_FLAG_ERROR)
                return;

            if (hLocal->dwFlags & LOCAL_FLAG_IMMUNE)
            {
                hLocal->CPU.immune_iteration++;

                if (hLocal->CPU.immune_iteration >=
                    hLocal->CPU.max_immune_iteration)
                    return;
            }
            else
            {
                hLocal->CPU.iteration++;

#ifdef USE_CACHE

                // Check cpu cache

                if (hLocal->CPU.iteration == dwCacheCheckIter &&
                    CacheCheckPresence(hLocal) == TRUE)
                {
                    // Reset the prefetch queue because upon
                    // returning to the interpretation loop,
                    // the call to get_byte_at_csip() may fetch
                    // a modified byte from the prefetch queue
                    // and thus set wNextRequest to the next
                    // prefetch queue size overwriting the
                    // assignment below of PREFETCH_NO_REQUEST.

                    reset_prefetch_queue(hLocal);
                    hLocal->CPU.prefetch.wNextRequest = PREFETCH_NO_REQUEST;
                    hLocal->dwFlags |= LOCAL_FLAG_NO_CAND_LEFT;
                    return;
                }

#endif // USE_CACHE

                if (hLocal->CPU.iteration >= hLocal->CPU.max_iteration)
                    return;
            }

#ifdef SYM_NLM
            if (hLocal->dwFlags & LOCAL_FLAG_IMMUNE)
            {
                if (hLocal->CPU.immune_iteration >
                    hLocal->CPU.next_nlm_immune_iter_stop)
                {
                    // relinquish control to other NLM's on NLM platforms

                    hLocal->CPU.next_nlm_immune_iter_stop +=
                        NLM_ITER_CHECK_POINT;

                    PAMRelinquishControl();
                }
            }
            else
            {
                if (hLocal->CPU.iteration > hLocal->CPU.next_nlm_iter_stop)
                {
                    // relinquish control to other NLM's on NLM platforms

                    hLocal->CPU.next_nlm_iter_stop += NLM_ITER_CHECK_POINT;
                    PAMRelinquishControl();
                }
            }
#endif



			if (test_z && hLocal->CPU.FLAGS.Z == FALSE)
				return;
		}
	}
}


/* 386 */

/* AAA */

void oc00110111(PAMLHANDLE hLocal)
{
    if ((hLocal->CPU.preg.H.AL & 0xF) > 9 || hLocal->CPU.FLAGS.A)
    {
		BYTE alCarry = hLocal->CPU.preg.H.AL > 0xF9;

		hLocal->CPU.preg.H.AL = (hLocal->CPU.preg.H.AL + 6) & 0xF;
		hLocal->CPU.preg.H.AH += 1 + alCarry;
		hLocal->CPU.FLAGS.A = hLocal->CPU.FLAGS.C = TRUE;
    }
    else
    {
        hLocal->CPU.FLAGS.A = hLocal->CPU.FLAGS.C = FALSE;
		hLocal->CPU.preg.H.AL &= 0xF;
    }
}

/* 386 */

/* AAD */

void oc11010101(PAMLHANDLE hLocal)
{
    BYTE b;

    b = get_byte_at_csip(hLocal);

    hLocal->CPU.preg.H.AL = (hLocal->CPU.preg.H.AH * b) +
        hLocal->CPU.preg.H.AL;

    hLocal->CPU.preg.H.AH = 0;

    // FALSE = BYTE operand...

    set_sign_flag(hLocal,FALSE,hLocal->CPU.preg.H.AL);
    set_zero_flag(hLocal,WORD_SIZE,hLocal->CPU.preg.X.AX);
    set_parity_flag(hLocal,WORD_SIZE,hLocal->CPU.preg.X.AX);

}

/* 386 */

/* AAM */

void oc11010100(PAMLHANDLE hLocal)
{
	BYTE b;

	b = get_byte_at_csip(hLocal);

	if (b)
	{
		hLocal->CPU.preg.H.AH = (hLocal->CPU.preg.H.AL / b);
		hLocal->CPU.preg.H.AL %= b;

		set_sign_flag(hLocal,WORD_SIZE,hLocal->CPU.preg.X.AX);
		set_zero_flag(hLocal,WORD_SIZE,hLocal->CPU.preg.X.AX);
        set_parity_flag(hLocal,WORD_SIZE,hLocal->CPU.preg.X.AX);
	}
}



/* 386 */

/* AAS */

void oc00111111(PAMLHANDLE hLocal)
{
	if ((hLocal->CPU.preg.H.AL & 0xF) > 9 || hLocal->CPU.FLAGS.A)
	{
		BYTE alBorrow = hLocal->CPU.preg.H.AL < 6;

		hLocal->CPU.preg.H.AL = (hLocal->CPU.preg.H.AL - 6) & 0xF;
		hLocal->CPU.preg.H.AH -= 1 + alBorrow;
		hLocal->CPU.FLAGS.A = hLocal->CPU.FLAGS.C = TRUE;
	}
	else
	{
		hLocal->CPU.FLAGS.A = hLocal->CPU.FLAGS.C = FALSE;
		hLocal->CPU.preg.H.AL &= 0xF;
	}
}

/* DAA */

void oc00100111(PAMLHANDLE hLocal)
{
    BYTE newAL = hLocal->CPU.preg.H.AL;

	if ((newAL & 0x0F) > 9 || hLocal->CPU.FLAGS.A == TRUE)
	{
		newAL += 6;
        hLocal->CPU.FLAGS.A = TRUE;
	}
    else
        hLocal->CPU.FLAGS.A = FALSE;

	if ((newAL & 0xF0) > 0x90 || hLocal->CPU.FLAGS.C == TRUE)
	{
		newAL += 0x60;
        hLocal->CPU.FLAGS.C = TRUE;
	}
    else
        hLocal->CPU.FLAGS.C = FALSE;

    set_parity_flag(hLocal,FALSE,newAL);    // FALSE = BYTE_SIZE
    set_zero_flag(hLocal,FALSE,newAL);
    set_sign_flag(hLocal,FALSE,newAL);

	hLocal->CPU.preg.H.AL = newAL;	// Save new AL
}


/* DAS */

void oc00101111(PAMLHANDLE hLocal)
{
    BYTE tmpAL = hLocal->CPU.preg.H.AL;

    if ((tmpAL & 0x0f) > 9 || hLocal->CPU.FLAGS.A == TRUE)
    {
        hLocal->CPU.FLAGS.A = TRUE;
		hLocal->CPU.preg.H.AL = hLocal->CPU.preg.H.AL - 0x6;
	}
    else
        hLocal->CPU.FLAGS.A = FALSE;

    if (tmpAL > 0x9f || hLocal->CPU.FLAGS.C == TRUE)
	{
		hLocal->CPU.preg.H.AL = hLocal->CPU.preg.H.AL - 0x60;
        hLocal->CPU.FLAGS.C = TRUE;
    }
    else
        hLocal->CPU.FLAGS.C = FALSE;


    set_parity_flag(hLocal,FALSE,hLocal->CPU.preg.H.AL);    // FALSE = BYTE_SIZE
    set_zero_flag(hLocal,FALSE,hLocal->CPU.preg.H.AL);
    set_sign_flag(hLocal,FALSE,hLocal->CPU.preg.H.AL);
}


/* 386 */

/* ENTER */

void oc11001000(PAMLHANDLE hLocal)
{
    WORD wStack, wLevel, i;
    DWORD dwFramePtr;

    wStack = get_word_at_csip(hLocal);
    wLevel = get_byte_at_csip(hLocal) & 0x1F;       /* level %= 32 */

    if (hLocal->CPU.op_size_over)
    {
		push_dword(hLocal,hLocal->CPU.ireg.D.EBP);
		dwFramePtr = hLocal->CPU.ireg.D.ESP;
	}
	else
	{
		push_word(hLocal,hLocal->CPU.ireg.X.BP);
		dwFramePtr = hLocal->CPU.ireg.X.SP;
    }

    if (wLevel > 0)
    {
		for (i=1;i<=wLevel-1;i++)
        {
			if (hLocal->CPU.op_size_over)
			{
				hLocal->CPU.ireg.D.EBP -= sizeof(DWORD);
				push_dword(hLocal, hLocal->CPU.ireg.D.EBP);
			}
			else
			{
				hLocal->CPU.ireg.X.BP -= sizeof(WORD);
				push_word(hLocal, hLocal->CPU.ireg.X.BP);
			}
		}

		if (hLocal->CPU.op_size_over)
			push_dword(hLocal,dwFramePtr);
		else
			push_word(hLocal,(WORD)dwFramePtr);
	}

    if (hLocal->CPU.op_size_over)
        hLocal->CPU.ireg.D.EBP = dwFramePtr;
    else
        hLocal->CPU.ireg.X.BP = (WORD)dwFramePtr;

    if (hLocal->CPU.addr_size_over)
		hLocal->CPU.ireg.D.ESP -= wStack;
	else
		hLocal->CPU.ireg.X.SP -= wStack;
}

/* in AL/AX, immed(8) */

void oc1110010w(PAMLHANDLE hLocal, BYTE opcode)
{
    get_byte_at_csip(hLocal);               // port #

    if (W_VALUE(opcode) == 0)               // byte
    {
        hLocal->CPU.preg.H.AL = 0;
    }
    else
    {
        if (hLocal->CPU.op_size_over)       // dword
            hLocal->CPU.preg.D.EAX = 0;
        else
            hLocal->CPU.preg.X.AX = 0;      // word
    }
}


/* in AL,DX (AX,DX, EAX, DX) */

void oc1110110w(PAMLHANDLE hLocal, BYTE opcode)
{
    if (W_VALUE(opcode) == 0)               // byte
    {
        hLocal->CPU.preg.H.AL = 0;
    }
    else
    {
        if (hLocal->CPU.op_size_over)       // dword
            hLocal->CPU.preg.D.EAX = 0;
        else
            hLocal->CPU.preg.X.AX = 0;      // word
    }
}

/* INSB/INSW/INSD */

void oc0110110w(PAMLHANDLE hLocal, BYTE opcode)
{
	WORD wSkip;

	if (hLocal->CPU.rep_prefix)
	{
		rep_instruction(hLocal, oc0110110w, opcode);
		return;
	}

	if (W_VALUE(opcode) == 0)
		wSkip = sizeof(BYTE);
	else if (hLocal->CPU.op_size_over)
		wSkip = sizeof(DWORD);
	else
		wSkip = sizeof(WORD);

    // update pointers but don't write anything...

    if (hLocal->CPU.FLAGS.D == 0)
        hLocal->CPU.ireg.X.DI += wSkip;
    else
        hLocal->CPU.ireg.X.DI -= wSkip;

}

/* INTO */

void oc11001110(PAMLHANDLE hLocal)
{
	WORD off,seg;

    if (hLocal->CPU.FLAGS.O)
    {
        off = get_word(hLocal, 0,0x4*VECT_SIZE);
        seg = get_word(hLocal, 0,0x4*VECT_SIZE+sizeof(WORD));

        hLocal->CPU.op_size_over = FALSE;       // don't want 32 bit PUSHFD!

        oc10011100(hLocal);                     /* pushf */

        hLocal->CPU.FLAGS.I = FALSE;            // turn off interrups
        hLocal->CPU.FLAGS.T = FALSE;            // turn off single step

        push_word(hLocal, hLocal->CPU.CS);
        push_word(hLocal, hLocal->CPU.IP);

        hLocal->CPU.CS = seg;
        hLocal->CPU.IP = off;

        reset_prefetch_queue(hLocal);

    }
}


/* LEAVE */

void oc11001001(PAMLHANDLE hLocal)
{
    if (hLocal->CPU.addr_size_over)
        hLocal->CPU.ireg.D.ESP = hLocal->CPU.ireg.D.EBP;
    else
        hLocal->CPU.ireg.X.SP = hLocal->CPU.ireg.X.BP;

    if (hLocal->CPU.op_size_over)
        hLocal->CPU.ireg.D.EBP = pop_dword(hLocal);
    else
        hLocal->CPU.ireg.X.BP = pop_word(hLocal);
}


/* OUT - just remove bytes from instruction stream */

void OUT_(PAMLHANDLE hLocal, BYTE opcode)
{
    if (opcode == OUT_8_AL || opcode == OUT_8_AX)
        get_byte_at_csip(hLocal);
}


/* 386 */

void OUTS(PAMLHANDLE hLocal, BYTE opcode)       /* outs* */
{
	WORD wSkip;

	if (hLocal->CPU.rep_prefix)
	{
        rep_instruction(hLocal, OUTS, opcode);
		return;
	}

    if (W_VALUE(opcode) == 0)
		wSkip = sizeof(BYTE);
	else if (hLocal->CPU.op_size_over)
		wSkip = sizeof(DWORD);
	else
		wSkip = sizeof(WORD);

	if (hLocal->CPU.FLAGS.D == 0)
		hLocal->CPU.ireg.X.SI += wSkip;
    else
        hLocal->CPU.ireg.X.SI -= wSkip;
}


void PUSH_IMMED(PAMLHANDLE hLocal, BYTE opcode)
{
	if (opcode == PUSH_I8_OPCODE)
	{
		if (hLocal->CPU.op_size_over)
			push_dword(hLocal,get_byte_at_csip(hLocal));
		else
			push_word(hLocal,get_byte_at_csip(hLocal));
	}
	else
	{
		if (hLocal->CPU.op_size_over)
			push_dword(hLocal,get_dword_at_csip(hLocal));
		else
			push_word(hLocal,get_word_at_csip(hLocal));
	}
}



void XLAT(PAMLHANDLE hLocal)
{
    hLocal->CPU.preg.H.AL =
            get_byte(hLocal,
					 seg_to_use(hLocal, hLocal->CPU.DS),
                     (WORD)(hLocal->CPU.preg.X.BX + hLocal->CPU.preg.H.AL));
}


void SETALC(PAMLHANDLE hLocal)
{
    // this is different on a pentium

    if (hLocal->CPU.FLAGS.C)
		hLocal->CPU.preg.H.AL = 0xFF;
	else
		hLocal->CPU.preg.H.AL = 0;

    /* no parity to worry about! */
}


void IMUL_69(PAMLHANDLE hLocal)
{
    short int isrc;
	long lvalue, lsrc, lhi, llo;
    BYTE amb, mem_type;
    WORD seg, off, reg;

	amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */

	// TRUE means always 16/32 bit stuff

	decode_amb(hLocal,TRUE, amb, &mem_type, &seg, &off, &reg);

	if (mem_type & WORD_SIZE)
	{                           /* 16 bit */
		long wwimul(short int,short int);

        isrc = (short int)gen_get_value(hLocal, mem_type,seg,off);

        lvalue = wwimul(isrc, (short int)get_word_at_csip(hLocal));

		set_reg(hLocal,mem_type,reg,(WORD)(lvalue&0xFFFFU));

		// result must fit entirely within WORD register for 0 C,O...

        if ((long)(short int)(lvalue & 0xFFFFU) == lvalue)
			hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = FALSE;
		else
			hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = TRUE;
	}
	else
	{
		void dwimul(long a, long b, long *hi, long *lo);

		lsrc = gen_get_value(hLocal, mem_type,seg,off);

		dwimul(lsrc,
               (long)get_dword_at_csip(hLocal),
               (long *)&lhi,
               (long *)&llo);

        set_reg(hLocal,mem_type,reg,(DWORD)llo);

        // result must fit entirely within a DWORD register for 0 C,O

        if (lhi == 0 || lhi == -1)
            hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = FALSE;
        else
            hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = TRUE;
    }


}

void IMUL_6B(PAMLHANDLE hLocal)
{
	short int isrc;
	long lvalue, lsrc, lhi, llo;
	BYTE amb, mem_type, byImmed;
	WORD seg, off, reg;

	amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */

	// TRUE means always 16/32 bit stuff

	decode_amb(hLocal,TRUE, amb, &mem_type, &seg, &off, &reg);

	byImmed = get_byte_at_csip(hLocal);

	if (mem_type & WORD_SIZE)
	{                           /* 16 bit */
		long wwimul(short int,short int);

        isrc = (short int)gen_get_value(hLocal, mem_type,seg,off);

        lvalue = wwimul(isrc, (short int)sign_extend_byte(byImmed));

		set_reg(hLocal,mem_type,reg,(WORD)(lvalue&0xFFFFU));

		// result must fit entirely within WORD register for 0 C,O...

        if ((long)(short int)(lvalue & 0xFFFFU) == lvalue)
			hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = FALSE;
		else
			hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = TRUE;
	}
	else
	{
		void dwimul(long a, long b, long *hi, long *lo);

		lsrc = gen_get_value(hLocal, mem_type,seg,off);

		dwimul(lsrc,
			   (long)dsign_extend_byte(byImmed),
               (long *)&lhi,
               (long *)&llo);

        set_reg(hLocal,mem_type,reg,(DWORD)llo);

        // result must fit entirely within a DWORD register for 0 C,O

        if (lhi == 0 || lhi == -1)
            hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = FALSE;
        else
            hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = TRUE;
    }

}

void IMUL_AF(PAMLHANDLE hLocal)
{
    short int isrc, idest;
	long lvalue, lsrc, ldest, lhi, llo;
	BYTE amb, mem_type;
	WORD seg, off, reg;

	amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */

	// TRUE means always 16/32 bit stuff

	decode_amb(hLocal,TRUE, amb, &mem_type, &seg, &off, &reg);

	if (mem_type & WORD_SIZE)
	{                           /* 16 bit */
		long wwimul(short int,short int);

        isrc = (short int)gen_get_value(hLocal, mem_type,seg,off);
        idest = (short int)reg_value(hLocal,mem_type,reg);

		lvalue = wwimul(idest, isrc);

		set_reg(hLocal,mem_type,reg,(WORD)(lvalue&0xFFFFU));

		// result must fit entirely within WORD register for 0 C,O...

        if ((long)(short int)(lvalue & 0xFFFFU) == lvalue)
			hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = FALSE;
		else
			hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = TRUE;
	}
	else
	{
		void dwimul(long a, long b, long *hi, long *lo);

		lsrc = gen_get_value(hLocal, mem_type,seg,off);
		ldest = reg_value(hLocal,mem_type,reg);

		dwimul(ldest,
			   lsrc,
               (long *)&lhi,
               (long *)&llo);

        set_reg(hLocal,mem_type,reg,(DWORD)llo);

        // result must fit entirely within a DWORD register for 0 C,O

        if (lhi == 0 || lhi == -1)
            hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = FALSE;
        else
            hLocal->CPU.FLAGS.C = hLocal->CPU.FLAGS.O = TRUE;
    }
}


void BOUND(PAMLHANDLE hLocal)
{
	BYTE amb, mem_type;
	WORD seg, off, reg;

	amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */

	// TRUE means always 16/32 bit stuff

    decode_amb(hLocal,TRUE, amb, &mem_type, &seg, &off, &reg);
}


void FLOAT_INSTR(PAMLHANDLE hLocal, BYTE byOpcode)
{
    BYTE amb, mem_type;
	WORD seg, off, reg;

    (void)byOpcode;

	amb = get_byte_at_csip(hLocal);           /* get mod/reg/r/m byte */

    if (REG_VALUE(amb) >= 0xc0)               /* this is a float 2-byte */
        return;

    // TRUE means always 16/32 bit stuff

    decode_amb(hLocal,TRUE, amb, &mem_type, &seg, &off, &reg);
}

