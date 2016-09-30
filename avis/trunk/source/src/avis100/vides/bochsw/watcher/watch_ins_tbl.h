#ifndef _WATCH_INS_TBL_H
#define _WATCH_INS_TBL_H

#include "watcher.h"
#include "..\..\Bochs-96\instr_proto.h"

extern int mod_rm_byte;
extern int mod;
extern int opcode;
extern int rm;

void code_escape(void);
void no_watch(void);
void illegal(void);
void nop(void);
void setalc(void);
void icebp(void);
void group1EbIb(void);
void group1EvIv(void);
void group1EvIb(void);
void group2EbIb(void);
void group2Eb1(void);
void group2EbCL(void);
void group2EvIb(void);
void group2Ev1(void);
void group2EvCL(void);
void group3Eb(void);
void group3Ev(void);
void group4(void);
void group5(void);
void group6(void);
void group7(void);
void group8(void);
void esc0(void);
void esc1(void);
void esc2(void);
void esc3(void);
void esc4(void);
void esc5(void);
void esc6(void);
void esc7(void);
void es_override(void);
void cs_override(void);
void ss_override(void);
void ds_override(void);
void fs_override(void);
void gs_override(void);
void opsize_prefix(void);
void addrsize_prefix(void);
void repne(void);
void rep(void);
void insb(void);
void insw(void);
void outsb(void);
void outsw(void);
void movsb(void);
void movsw(void);
void cmpsb(void);
void cmpsw(void);
void stosb(void);
void stosw(void);
void lodsb(void);
void lodsw(void);
void scasb(void);
void scasw(void);
void lock(void);
void loadalld(void);
void rdtsc(void);
void rdmsr(void);


typedef void (* Instr) (void);

extern Instr instructions[];
extern Instr instructions_escape[];
extern Instr watching_instructions[];
extern Instr watching_instructions_escape[];
extern Instr instructions_group5[];
extern Instr watching_instructions_group5[];


/* watched instructions prototypes */
void INT3_watch(void);
void INT_Ib_watch(void);
void IRET_watch(void);
void RETfar_watch(void);
void RETfar_Iw_watch(void);
void CALL_Ep_watch(void);
void CALL_Ap_watch(void);
void movsb_watch(void);
void movsw_watch(void);

void inalib_watch(void);
void ineaxib_watch(void);
void inaldx_watch(void);
void ineaxdx_watch(void);
void outibal_watch(void);
void outibeax_watch(void);
void outdxal_watch(void);
void outdxeax_watch(void);
void insb_watch(void);
void outsb_watch(void);
void insw_watch(void);
void outsw_watch(void);
void cmp_watch(void);



#endif /* _WATCH_INS_TBL_H */
