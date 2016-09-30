// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/PROTO.H_v   1.5   04 May 1996 13:31:14   RSTANEV  $
//
// Description:
//
//  This source file contains prototypes for all functions which are called
//  by other files within the PAM system.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/PROTO.H_v  $
// 
//    Rev 1.5   04 May 1996 13:31:14   RSTANEV
// The callers of CacheInit() now have to pass pointer to a LONG which
// will be used as mutex.
//
//    Rev 1.4   04 Apr 1996 10:59:32   CNACHEN
// Changed prototypes for IN AL, CONST and IN AL, DX instructions.  Now
// pass in opcode as an argument so we can set the EAX/AX/AL register to
// zero.
//
//
//    Rev 1.3   04 Mar 1996 16:02:08   CNACHEN
// Added CACHE support.
//
//    Rev 1.2   27 Feb 1996 18:45:14   CNACHEN
// Removed COVER.CPP prototypes...
//
//    Rev 1.1   20 Feb 1996 11:27:26   CNACHEN
// Changed all LPSTRs to LPTSTRs.
//
//    Rev 1.0   01 Feb 1996 10:17:42   CNACHEN
// Initial revision.
//
//    Rev 1.14   22 Jan 1996 13:40:46   CNACHEN
// Added new fault support.
//
//
//    Rev 1.13   12 Jan 1996 12:05:38   CNACHEN
// Added floating point support (to skip over float instructions)
//
//    Rev 1.12   20 Dec 1995 12:19:34   CNACHEN
// Added 0F,AF, 69, 6B IMUL instructions...
//
//    Rev 1.11   19 Dec 1995 19:08:08   CNACHEN
// Added prefetch queue support!
//
//
//    Rev 1.10   15 Dec 1995 19:03:26   CNACHEN
// Added LOW_IN_RAM stuff...
//
//    Rev 1.9   14 Dec 1995 13:02:32   CNACHEN
// Added support for control relinquishing under NLM...
//
//    Rev 1.8   15 Nov 1995 20:15:08   CNACHEN
// Added several new aux carry functions...
//
//    Rev 1.7   26 Oct 1995 14:09:38   CNACHEN
// Added memory swapping instead of disk swapping if NULL is passed in for
// the swap-file filename.
//
//    Rev 1.6   19 Oct 1995 18:39:22   CNACHEN
// Initial revision... With comment headers... :)
//************************************************************************


/* CPU.C */

void reset_prefetch_queue(PAMLHANDLE hLocal);
void suspend_prefetch_queue(PAMLHANDLE hLocal);
void enable_prefetch_queue(PAMLHANDLE hLocal);
void reload_prefetch_queue(PAMLHANDLE hLocal);
BYTE fetch_byte_from_prefetch_queue(PAMLHANDLE hLocal);
void roll_back_prefetch(PAMLHANDLE hLocal);

void init_cpu_com(PAMLHANDLE hLocal, BYTE *buffer);
void init_cpu_exe(PAMLHANDLE hLocal, BYTE *buffer);
void init_cpu_sys(PAMLHANDLE hLocal, BYTE *buffer, WORD entry_num);
void global_close_cpu(char *data_file_path);
void local_close_cpu(PAMLHANDLE hLocal);
BYTE get_byte(PAMLHANDLE hLocal, WORD seg, WORD off);
WORD get_word(PAMLHANDLE hLocal, WORD seg, WORD off);
DWORD get_dword(PAMLHANDLE hLocal, WORD seg, WORD off);
BYTE get_byte_at_csip(PAMLHANDLE hLocal);
WORD get_word_at_csip(PAMLHANDLE hLocal);
DWORD get_dword_at_csip(PAMLHANDLE hLocal);
BYTE put_byte(PAMLHANDLE hLocal, WORD seg, WORD off, BYTE b);
WORD put_word(PAMLHANDLE hLocal, WORD seg, WORD off, WORD w);
DWORD put_dword(PAMLHANDLE hLocal, WORD seg, WORD off, DWORD dw);
WORD push_word(PAMLHANDLE hLocal, WORD w);
WORD pop_word(PAMLHANDLE hLocal);
DWORD push_value(PAMLHANDLE hLocal, WORD wMemType, DWORD dwValue);
DWORD pop_value(PAMLHANDLE hLocal, WORD wMemType);
DWORD push_dword(PAMLHANDLE hLocal, DWORD dw);
DWORD pop_dword(PAMLHANDLE hLocal);
WORD sign_extend_byte(BYTE b);
DWORD sign_extend_word(WORD w);
DWORD dsign_extend_byte(BYTE b);
void decode_amb(PAMLHANDLE hLocal, BYTE opcode, BYTE amb, BYTE *mem_type, WORD *seg, WORD *off, WORD *reg);
void decode_sib(PAMLHANDLE hLocal, BYTE amb, WORD *seg, WORD *off);
WORD seg_to_use(PAMLHANDLE hLocal, WORD seg);
WORD wreg_value(PAMLHANDLE hLocal, WORD reg_num);
DWORD dwreg_value(PAMLHANDLE hLocal, WORD reg_num);
BYTE breg_value(PAMLHANDLE hLocal, WORD reg_num);
//WORD wsign_status(WORD a);
//WORD bsign_status(BYTE a);
//WORD wzero_status(WORD a);
//WORD bzero_status(BYTE a);
DWORD sign_status(WORD wMemType, DWORD dwData);
WORD set_wreg(PAMLHANDLE hLocal, WORD reg_num, WORD value);
DWORD set_dwreg(PAMLHANDLE hLocal, WORD reg_num, DWORD value);
BYTE set_breg(PAMLHANDLE hLocal, WORD reg_num, BYTE value);
WORD set_sreg(PAMLHANDLE hLocal, WORD reg_num, WORD wval);
WORD get_sreg(PAMLHANDLE hLocal, WORD reg_num);
void bset_add_flags(PAMLHANDLE hLocal, BYTE a, BYTE b);
void wset_add_flags(PAMLHANDLE hLocal, WORD a, WORD b);
void set_seg_over(PAMLHANDLE hLocal, WORD seg_over);
void reset_seg_over(PAMLHANDLE hLocal);
void reset_rep_over(PAMLHANDLE hLocal);
void reset_32_bit_over(PAMLHANDLE hLocal);
int  global_init_cpu(char *data_file_path);
PAMSTATUS  local_init_cpu(PAMLHANDLE hLocal,HFILE hFile, WORD scan_flags, WORD entry_num, WORD *num_entry);
WORD wget_value(PAMLHANDLE hLocal, WORD mem_type, WORD seg, WORD off);
DWORD dwget_value(PAMLHANDLE hLocal, WORD mem_type, WORD seg, WORD off);
WORD wput_value(PAMLHANDLE hLocal, WORD mem_type, WORD seg, WORD off, WORD value);
DWORD dput_value(PAMLHANDLE hLocal, WORD mem_type, WORD seg, WORD off, DWORD value);
BYTE bget_value(PAMLHANDLE hLocal, WORD mem_type, WORD seg, WORD off);
BYTE bput_value(PAMLHANDLE hLocal, WORD mem_type, WORD seg, WORD off, BYTE value);
DWORD gen_get_value(PAMLHANDLE hLocal,WORD wMemType,WORD wSeg,WORD wOff);
DWORD gen_put_value(PAMLHANDLE hLocal, WORD wMemType, WORD wSeg, WORD wOff, DWORD dwValue);

DWORD reg_value(PAMLHANDLE hLocal, WORD wMemType, WORD wRegNum);
DWORD set_reg(PAMLHANDLE hLocal, WORD wMemType, WORD wRegNum, DWORD dwValue);
DWORD get_value_at_csip(PAMLHANDLE hLocal, WORD wMemType);

void decode_amb_32_off(PAMLHANDLE hLocal,
                       BYTE       opcode,
                       BYTE       amb,
                       BYTE       *mem_type,
                       DWORD      *off,
                       WORD       *reg);

void decode_amb_norm(PAMLHANDLE hLocal,
                     BYTE       opcode,
                     BYTE       amb,
                     BYTE       *mem_type,
                     WORD       *seg,
                     WORD       *off,
					 WORD       *reg);

void decode_sib_off(PAMLHANDLE hLocal, BYTE amb, DWORD *off);

/* INSTR.C */

void oc000100dw(PAMLHANDLE hLocal, BYTE opcode);       /* ADC reg/mem, reg/mem */
void oc100000sw(PAMLHANDLE hLocal, BYTE opcode);       /* general - ADC */
void oc0001010w(PAMLHANDLE hLocal, BYTE opcode);       /* ADC accum, immed */

void oc000000dw(PAMLHANDLE hLocal, BYTE opcode);       /* ADD reg/mem, reg/mem */
void oc0000010w(PAMLHANDLE hLocal, BYTE opcode);       /* ADD accum, immed */

void oc11101000(PAMLHANDLE hLocal);              /* call near direct */
void oc11111111(PAMLHANDLE hLocal, BYTE opcode);       /* call/jmp */
void oc10011010(PAMLHANDLE hLocal);              /* call far direct */

void oc10011000(PAMLHANDLE hLocal);              /* cbd */

void oc11111000(PAMLHANDLE hLocal);              /* clc */

void oc11111100(PAMLHANDLE hLocal);              /* cld */

void oc11111010(PAMLHANDLE hLocal);              /* cli */

void oc11110101(PAMLHANDLE hLocal);              /* cmc */

void oc001110dw(PAMLHANDLE hLocal, BYTE opcode);       /* cmp reg/mem, reg/mem */

void oc0011110w(PAMLHANDLE hLocal, BYTE opcode);               /* cmp accum, immed */

void oc10011001(PAMLHANDLE hLocal);              /* cwd */

void oc11111110(PAMLHANDLE hLocal, BYTE opcode);       /* dec 8 bit reg/mem */

void oc01001reg(PAMLHANDLE hLocal, BYTE opcode);       /* dec 16 bit reg */

void oc1111011w(PAMLHANDLE hLocal, BYTE opcode);       /* general - div reg/mem, inc reg/mem */

void oc01000reg(PAMLHANDLE hLocal, BYTE opcode);       /* inc reg 16 */

void oc0111cccc(PAMLHANDLE hLocal, BYTE opcode);       /* jcond */

void oc11100011(PAMLHANDLE hLocal);              /* jcxz */

void oc11101011(PAMLHANDLE hLocal);              /* jmp short */

void oc11101010(PAMLHANDLE hLocal);             /* jmp far direct */

void oc11101001(PAMLHANDLE hLocal);             /* jmp near */

void oc10011111(PAMLHANDLE hLocal);             /* lahf */

void oc10011110(PAMLHANDLE hLocal);             /* sahf */

void oc11000101(PAMLHANDLE hLocal);             /* lds */

void oc10001101(PAMLHANDLE hLocal, BYTE opcode);       /* lea */

void oc11000100(PAMLHANDLE hLocal);             /* les */

void oc1010110w(PAMLHANDLE hLocal, BYTE opcode);       /* lods */

void oc11100010(PAMLHANDLE hLocal);              /* loop */

void oc11100001(PAMLHANDLE hLocal);              /* loopz */

void oc11100000(PAMLHANDLE hLocal);              /* loopnz */

void oc100010dw(PAMLHANDLE hLocal, BYTE opcode);       /* mov reg/mem, reg/mem */

void oc1011wreg(PAMLHANDLE hLocal, BYTE opcode);       /* mov reg, immed */

void oc1100011w(PAMLHANDLE hLocal, BYTE opcode);       /* general - mov reg/mem, immed */

void oc1010000w(PAMLHANDLE hLocal, BYTE opcode);       /* mov accum, mem */

void oc1010001w(PAMLHANDLE hLocal, BYTE opcode);       /* mov mem, accum */

void oc10001110(PAMLHANDLE hLocal, BYTE opcode);       /* mov sreg, reg/mem */

void oc10001100(PAMLHANDLE hLocal, BYTE opcode);       /* mov reg/mem, sreg */

void oc1010010w(PAMLHANDLE hLocal, BYTE opcode);       /* movsb/movsw */

void oc0000110w(PAMLHANDLE hLocal, BYTE opcode);       /* or accum, immed */

void oc000010dw(PAMLHANDLE hLocal, BYTE opcode);       /* or reg/mem, reg/mem */

void oc01011reg(PAMLHANDLE hLocal, BYTE opcode);       /* pop 16 bit reg */

void oc10001111(PAMLHANDLE hLocal, BYTE opcode);               /* general - pop */

void oc00sreg111(PAMLHANDLE hLocal, BYTE opcode);      /* pop sreg */

void oc10011101(PAMLHANDLE hLocal);              /* popf */

void oc01010reg(PAMLHANDLE hLocal, BYTE opcode);       /* push 16 bit reg */

void oc00sreg110(PAMLHANDLE hLocal, BYTE opcode);      /* push sreg */

void oc10011100(PAMLHANDLE hLocal);              /* pushf */

void oc1101000w(PAMLHANDLE hLocal, BYTE opcode);       /* general - rcl */

void oc1100000w(PAMLHANDLE hLocal, BYTE opcode);   /* Rotates and shifts (i8) */

void oc1101001w(PAMLHANDLE hLocal, BYTE opcode);       /* general - rcl */

void oc11110010(PAMLHANDLE hLocal);              /* repnz */

void oc11110011(PAMLHANDLE hLocal);              /* repz */

void oc11000011(PAMLHANDLE hLocal);              /* ret near */

void oc11000010(PAMLHANDLE hLocal);              /* ret immed16 */

void oc11001011(PAMLHANDLE hLocal);              /* retf */

void oc11001010(PAMLHANDLE hLocal);              /* retf immed16 */

void oc000110dw(PAMLHANDLE hLocal, BYTE opcode);       /* SBB reg/mem, reg/mem */

void oc0001110w(PAMLHANDLE hLocal, BYTE opcode);       /* SBB accum, immed */

void oc001seg110(PAMLHANDLE hLocal, BYTE opcode);      /* SEG override! */

void oc11111001(PAMLHANDLE hLocal);              /* STC */

void oc11111101(PAMLHANDLE hLocal);              /* STD */

void oc11111011(PAMLHANDLE hLocal);              /* STI */

void oc1010101w(PAMLHANDLE hLocal, BYTE opcode);       /* STOS */

void oc001010dw(PAMLHANDLE hLocal, BYTE opcode);       /* SUB reg/mem, reg/mem */

void oc0010110w(PAMLHANDLE hLocal, BYTE opcode);       /* SUB accum, immed */

void oc1000011w(PAMLHANDLE hLocal, BYTE opcode);       /* XCHG reg/mem, reg */

void oc10010reg(PAMLHANDLE hLocal, BYTE opcode);       /* XCHG AX, reg */

void oc001100dw(PAMLHANDLE hLocal, BYTE opcode);       /* XOR reg/mem, reg/mem */

void oc0011010w(PAMLHANDLE hLocal, BYTE opcode);       /* XOR accum, immed */

void oc001000dw(PAMLHANDLE hLocal, BYTE opcode);       /* AND reg/mem, reg/mem */

void oc0010010w(PAMLHANDLE hLocal, BYTE opcode);       /* AND accum, immed */

void oc1000010w(PAMLHANDLE hLocal, BYTE opcode);       /* TEST reg, reg/mem */

void oc1010100w(PAMLHANDLE hLocal, BYTE opcode);       /* TEST accum, immed */

void oc11001100(PAMLHANDLE hLocal);              /* int 3 */

void oc11001101(PAMLHANDLE hLocal);              /* int general */

void oc11001111(PAMLHANDLE hLocal);              /* iret */

void oc1010111w(PAMLHANDLE hLocal, BYTE opcode);       /* scas */

void oc1010011w(PAMLHANDLE hLocal, BYTE opcode);       /* cmpsX */

void oc10011110(PAMLHANDLE hLocal);              /* sahf */

void oc1010111w(PAMLHANDLE hLocal, BYTE opcode);       /* scasX */

void oc01100100(PAMLHANDLE hLocal);                     /* FS: prefix */

void oc01100101(PAMLHANDLE hLocal);                     /* GS: prefix */

void oc01100110(PAMLHANDLE hLocal);     /* (32-bit operand): prefix */

void oc01100111(PAMLHANDLE hLocal);     /* (32-bit address): prefix: */

void oc00001111(PAMLHANDLE hLocal);     /* extended opcode */


void oc00110111(PAMLHANDLE hLocal);     /* aaa */

void oc11010101(PAMLHANDLE hLocal);     /* aad */

void oc11010100(PAMLHANDLE hLocal);     /* aam */

void oc00111111(PAMLHANDLE hLocal);     /* aas */

void BSF(PAMLHANDLE hLocal);            /* BSF */

void BSR(PAMLHANDLE hLocal);            /* BSR */

void BSWAP(PAMLHANDLE hLocal, BYTE opcode);     /* BSWAP */

void BT(PAMLHANDLE hLocal, BYTE opcode, BYTE func);        /* BT */

void CMPXCHG(PAMLHANDLE hLocal, BYTE opcode);

void LGDT(PAMLHANDLE hLocal);

void LSREG(PAMLHANDLE hLocal, WORD wRegNum);

void MOVSX(PAMLHANDLE hLocal, BYTE opcode);
void MOVZX(PAMLHANDLE hLocal, BYTE opcode);

void POP_FS(PAMLHANDLE hLocal);

void POP_GS(PAMLHANDLE hLocal);

void PUSH_FS(PAMLHANDLE hLocal);

void PUSH_GS(PAMLHANDLE hLocal);

void oc00100111(PAMLHANDLE hLocal);             /* DAA */

void oc00101111(PAMLHANDLE hLocal);            /* DAS */

void oc11001000(PAMLHANDLE hLocal);             /* ENTER */

void oc1110010w(PAMLHANDLE hLocal, BYTE opcode);/* IN AL, IMMED */
void oc1110110w(PAMLHANDLE hLocal, BYTE opcode);/* IN AL, DX */


void oc0110110w(PAMLHANDLE hLocal, BYTE opcode);	/* INS */

void oc11001110(PAMLHANDLE hLocal);                 /* INTO */

void oc11001001(PAMLHANDLE hLocal);                 /* LEAVE */

void SETcc(PAMLHANDLE hLocal, BYTE opcode);     /* SETcc opcodes (0x0f XX) */

void OUT_(PAMLHANDLE hLocal, BYTE opcode);          /* all OUT instrs */
void OUTS(PAMLHANDLE hLocal, BYTE opcode);          /* all OUTS instrs */
void POPA(PAMLHANDLE hLocal);                       /* POPA/D */
void PUSH_IMMED(PAMLHANDLE hLocal, BYTE opcode);    /* PUSH immed */
void PUSH_ALL(PAMLHANDLE hLocal);                   /* PUSHA/D */
void SHLD(PAMLHANDLE hLocal, BYTE opcode);          /* SHLD */
void SHRD(PAMLHANDLE hLocal, BYTE opcode);          /* SHRD */
void XADD(PAMLHANDLE hLocal, BYTE opcode);          /* XADD */
void XLAT(PAMLHANDLE hLocal);                       /* XLAT */
void ARPL(PAMLHANDLE hLocal);                       /* ARPL -> INT */
void SETALC(PAMLHANDLE hLocal);                     /* SETALC */
void IMUL_69(PAMLHANDLE hLocal);                    /* IMUL */
void IMUL_6B(PAMLHANDLE hLocal);                    /* IMUL */
void IMUL_AF(PAMLHANDLE hLocal);                    /* IMUL */
void BOUND(PAMLHANDLE hLocal);                      /* BOUND */
void FLOAT_INSTR(PAMLHANDLE hLocal, BYTE byOpcode); /* FLOAT INSTRS */

void rep_instruction(PAMLHANDLE hLocal,
                     void (*string_func)(PAMLHANDLE, BYTE),
                     BYTE opcode);
void repnz_instruction(PAMLHANDLE hLocal,
                       void (*string_func)(PAMLHANDLE, BYTE),
                       BYTE opcode);
void repz_instruction(PAMLHANDLE hLocal,
                      void (*string_func)(PAMLHANDLE, BYTE),
					  BYTE opcode);

int rep_cmp_instr(BYTE opcode);




void    large_switch(PAMLHANDLE hLocal, BYTE opcode);

/* EXEC.C */

DWORD   interpret(PAMLHANDLE hLOCAL, WORD *found_string);
WORD    search_buffers_for_string(PAMLHANDLE hLocal);
void    instruction_fault(PAMLHANDLE hLocal,BYTE byFaultNum);
BOOL    CacheCheckPresence(PAMLHANDLE hLocal);

/* FLAGS.C */

void set_aux_carry_flag(PAMLHANDLE hLocal, DWORD a, DWORD b, DWORD c);
void set_carry_flag_add(PAMLHANDLE hLocal, WORD wMemType, DWORD a, DWORD b, DWORD c);
void dwset_carry_flag_add(PAMLHANDLE hLocal, DWORD a, DWORD b, DWORD c);
void dwset_aux_carry_flag_add(PAMLHANDLE hLocal, DWORD a, DWORD b, DWORD c);
void dwset_aux_carry_flag_sub(PAMLHANDLE hLocal, DWORD a, DWORD b, DWORD c);
void wset_carry_flag_add(PAMLHANDLE hLocal, WORD a, WORD b, WORD c);
void bset_carry_flag_add(PAMLHANDLE hLocal, BYTE a, BYTE b, BYTE c);
void dwset_carry_flag_sub(PAMLHANDLE hLocal, DWORD a, DWORD b, DWORD c);
void wset_carry_flag_sub(PAMLHANDLE hLocal, WORD a, WORD b, WORD c);
void bset_carry_flag_sub(PAMLHANDLE hLocal, BYTE a, BYTE b, BYTE c);
void set_carry_flag_sub(PAMLHANDLE hLocal,WORD wMemType,DWORD a,DWORD b,DWORD c);
void set_over_flag_add(PAMLHANDLE hLocal, WORD wMemType, DWORD a, DWORD b, DWORD c);
void dwset_over_flag_add(PAMLHANDLE hLocal, DWORD a, DWORD b, DWORD c);
void wset_over_flag_add(PAMLHANDLE hLocal, WORD a, WORD b, WORD c);
void bset_over_flag_add(PAMLHANDLE hLocal, BYTE a, BYTE b, BYTE c);
void set_over_flag_sub(PAMLHANDLE hLocal, WORD wMemType, DWORD a, DWORD b, DWORD c);
void dwset_over_flag_sub(PAMLHANDLE hLocal, DWORD a, DWORD b, DWORD c);
void wset_over_flag_sub(PAMLHANDLE hLocal, WORD a, WORD b, WORD c);
void bset_over_flag_sub(PAMLHANDLE hLocal, BYTE a, BYTE b, BYTE c);

void set_zero_flag(PAMLHANDLE hLocal, WORD wMemType, DWORD dwValue);
void set_sign_flag(PAMLHANDLE hLocal, WORD wMemType, DWORD dwValue);
void set_parity_flag(PAMLHANDLE hLocal, WORD wMemType, DWORD dwValue);

/* opcodele.c */

int opcodelen(char *buffer);
int ret_table(int i,int j);


/* page.c */

PAMSTATUS global_init_paging(LPTSTR data_file_path, PAMLHANDLE hLocal);
void global_close_paging(PAMLHANDLE hLocal);
void local_init_paging(PAMLHANDLE hLocal);
void local_close_paging(PAMLHANDLE hLocal);
BYTE locate_swap_page(PAMLHANDLE hLocal);
void    page_write_byte(PAMLHANDLE, BYTE, DWORD, DWORD, DWORD, HFILE);

BYTE    page_read_byte(PAMLHANDLE hLocal, DWORD address, DWORD local_start,
                       DWORD local_address, HFILE source_file);
PAMSTATUS page_read_sector(PAMLHANDLE hLocal, DWORD address, DWORD local_start,
                       DWORD local_address, HFILE source_file, BYTE *buffer);
BYTE    access_page(PAMLHANDLE hLocal, DWORD address, DWORD local_start, DWORD local_address,
                    HFILE source_file);

void insert_resident_page(PAMLHANDLE hLocal, DWORD address, BYTE *sector);
void reset_page(BYTE *buffer, WORD fill_word);

/* exclude.c */

PAMSTATUS context_init_exclude(PAMLHANDLE hLocal);
PAMSTATUS context_close_exclude(PAMLHANDLE hLocal);

PAMSTATUS global_init_exclude(HFILE hStream,
                              DWORD dwStartOff,
                              exclude_type *exclude_info);
void local_init_exclude(PAMLHANDLE hLocal);
void global_close_exclude(exclude_type *exclude_info);
void exclude_item(PAMLHANDLE hLocal, WORD exclude_num);
void exclude_exe_entry_point(PAMLHANDLE hLocal, exeheader *h);
void exclude_int(PAMLHANDLE hLocal, BYTE int_num);
void exclude_com_iter(DWORD iteration);
void exclude_image_size(PAMLHANDLE hLocal, DWORD file_size);

/* search.c */

WORD find_sig(PAMGHANDLE hGlobal, BYTE *buffer, WORD buff_len);
PAMSTATUS global_init_search(HFILE hFile, DWORD dwOffset, PAMGHANDLE pPAMG);
void global_close_search(PAMGHANDLE hGlobal);


/* SIGEX.C */

PAMSTATUS PExcludeInit(ExcludeContext *hExcludePtr, LPTSTR lpFileName);
PAMSTATUS PExcludeTest(ExcludeContext hExclude,LPBYTE lpEntryBuffer);
PAMSTATUS PExcludeClose(ExcludeContext hExclude);


/* REPAIR.C */

WORD found_repair_sig(PAMLHANDLE        hLocal,
                      BYTE              byOpcode,
                      sig_list_type     *lpSigList,
                      WORD              *lpwSigNum);

WORD  repair_interpret( PAMLHANDLE          hLocal,
                        sig_list_type       *lpSigList,
                        WORD                *lpwSigNum);

PAMSTATUS load_repair_info(PAMLHANDLE hLocal,
                           DATAFILEHANDLE hDataFile,
                           WORD wVirusID,
                           sig_list_type *sig_list);


PAMSTATUS copy_bytes_to_vm(PAMLHANDLE   hLocal,
                           WORD         wNumBytes,
                           DWORD        dwStartOff,
                           HFILE        hFile,
                           WORD         wStartCS,
                           WORD         wStartIP,
                           WORD         *lpwBytesRead);

PAMSTATUS copy_bytes_from_vm(PAMLHANDLE   hLocal,
                             WORD         wNumBytes,
                             DWORD        dwStartOff,
                             HFILE        hFile,
                             WORD         wStartSEG,
                             WORD         wStartOFF,
                             WORD         *lpwBytesWritten);

PAMSTATUS load_repair_code(PAMLHANDLE hLocal,
                           DATAFILEHANDLE hDataFile,
                           sig_list_type *lpSigList);



/* CACHE.C */

void CacheInit(LPCACHE lpCache,
               LPLONG  lplMutex);

void CacheInsertItem(LPCACHE            lpCache,
                     LPCPU_STATE_T      lpEntry);

CACHESTATUS CacheLookupItem(LPCACHE         lpCache,
                            LPCPU_STATE_T   lpCPUState);

void StoreCPUStateIntoTemp(PAMLHANDLE hLocal, LPCPU_STATE_T lpTempState);



