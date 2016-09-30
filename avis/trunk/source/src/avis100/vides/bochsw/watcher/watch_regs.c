#include "watcher.h"

static void reportreg8(int index);
static void reportreadreg8(int index);
static void reportwritereg8(int index);
static void reportreg16(int index);
static void reportreadreg16(int index);
static void reportwritereg16(int index);
static void reportreg32(int index);
static void reportreadreg32(int index);
static void reportwritereg32(int index);
static void reportbasereg16(int index);
static void reportindexreg16(int index);

/*
typedef struct {
   char type; /* ADD, OR, ADC, SBB, AND, SUB, XOR, CMP, * /
   char src_opsize; /* OPb (byte), OPv (word/dword) * /
   char dest_opsize;
   char src_offset; /* 0, * /
   char dest_offset;
} Ins_prop_t;

#define ESCAPE -128
#define OPSIZE -127
#define ADSIZE -126
#define SEGES -16
#define SEGCS -15
#define SEGSS -14
#define SEGDS -13
#define SEGFS -12
#define SEGGS -11
#define NOP -1
#define ADD 0
#define OR 1
#define ADC 2
#define SBB 3
#define AND 4
#define SUB 5
#define XOR 6
#define CMP 7
#define MOV 8
#define MOVSEG 9
#define LEA 10
#define TEST 16
#define XCHG 17
#define DAA 24
#define DAS 25
#define AAA 26
#define AAS 27
#define PUSH 32
#define POP 33
#define PUSHA 34
#define POPA 35

#define NIL -1
#define Ob 0
#define Ov 1
#define Ow 2
#define Od 3

#define NIL -128
#define IMM -64
#define OFFSETv -63
#define ES -16
#define CS -15
#define SS -14
#define DS -13
#define EAX -8
#define OPCODE 0
#define RM 8
#define REG 11



struct Ins_prop_t instruction_properties[0x100] =
{
   { ADD, Ob, Ob, RM, REG },
   { ADD, Ov, Ov, RM, REG },
   { ADD, Ob, Ob, REG, RM },
   { ADD, Ov, Ov, REG, RM },
   { ADD, Ob, Ob, EAX, IMM },
   { ADD, Ov, Ov, EAX, IMM },
   { PUSH, NIL, Ow, ES, NIL },
   { POP, Ow, NIL, ES, NIL },
   { OR, Ob, Ob, RM, REG },
   { OR, Ov, Ov, RM, REG },
   { OR, Ob, Ob, REG, RM },
   { OR, Ov, Ov, REG, RM },
   { OR, Ob, Ob, EAX, IMM },
   { OR, Ov, Ov, EAX, IMM },
   { PUSH, NIL, Ow, CS, NIL },
   { ESCAPE, NIL, NIL, NIL, NIL },
   { ADC, Ob, Ob, RM, REG },
   { ADC, Ov, Ov, RM, REG },
   { ADC, Ob, Ob, REG, RM },
   { ADC, Ov, Ov, REG, RM },
   { ADC, Ob, Ob, EAX, IMM },
   { ADC, Ov, Ov, EAX, IMM },
   { PUSH, NIL, Ow, SS, NIL },
   { POP, Ow, NIL, SS, NIL },
   { SBB, Ob, Ob, RM, REG },
   { SBB, Ov, Ov, RM, REG },
   { SBB, Ob, Ob, REG, RM },
   { SBB, Ov, Ov, REG, RM },
   { SBB, Ob, Ob, EAX, IMM },
   { SBB, Ov, Ov, EAX, IMM },
   { PUSH, NIL, Ow, DS, NIL },
   { POP, Ow, NIL, DS, NIL },
   { AND, Ob, Ob, RM, REG },
   { AND, Ov, Ov, RM, REG },
   { AND, Ob, Ob, REG, RM },
   { AND, Ov, Ov, REG, RM },
   { AND, Ob, Ob, EAX, IMM },
   { AND, Ov, Ov, EAX, IMM },
   { SEGES, NIL, NIL, NIL, NIL },
   { DAA, NIL, NIL, NIL, NIL }, // ?
   { SUB, Ob, Ob, RM, REG },
   { SUB, Ov, Ov, RM, REG },
   { SUB, Ob, Ob, REG, RM },
   { SUB, Ov, Ov, REG, RM },
   { SUB, Ob, Ob, EAX, IMM },
   { SUB, Ov, Ov, EAX, IMM },
   { SEGCS, NIL, NIL, NIL, NIL },
   { DAS, NIL, NIL, NIL, NIL }, // ? 
   { XOR, Ob, Ob, RM, REG },
   { XOR, Ov, Ov, RM, REG },
   { XOR, Ob, Ob, REG, RM },
   { XOR, Ov, Ov, REG, RM },
   { XOR, Ob, Ob, EAX, IMM },
   { XOR, Ov, Ov, EAX, IMM },
   { SEGSS, NIL, NIL, NIL, NIL },
   { CMP, NIL, NIL, NIL, NIL }, // ? 
   { CMP, Ob, Ob, RM, REG },
   { CMP, Ov, Ov, RM, REG },
   { CMP, Ob, Ob, REG, RM },
   { CMP, Ov, Ov, REG, RM },
   { CMP, Ob, Ob, EAX, IMM },
   { CMP, Ov, Ov, EAX, IMM },
   { SEGDS, NIL, NIL, NIL, NIL },
   { AAS, NIL, NIL, NIL, NIL }, // ? 
   { INC, Ov, NIL, OPCODE, NIL },
   { INC, Ov, NIL, OPCODE, NIL },
   { INC, Ov, NIL, OPCODE, NIL },
   { INC, Ov, NIL, OPCODE, NIL },
   { INC, Ov, NIL, OPCODE, NIL },
   { INC, Ov, NIL, OPCODE, NIL },
   { INC, Ov, NIL, OPCODE, NIL },
   { INC, Ov, NIL, OPCODE, NIL },
   { DEC, Ov, NIL, OPCODE, NIL },
   { DEC, Ov, NIL, OPCODE, NIL },
   { DEC, Ov, NIL, OPCODE, NIL },
   { DEC, Ov, NIL, OPCODE, NIL },
   { DEC, Ov, NIL, OPCODE, NIL },
   { DEC, Ov, NIL, OPCODE, NIL },
   { DEC, Ov, NIL, OPCODE, NIL },
   { DEC, Ov, NIL, OPCODE, NIL },
   { PUSH, NIL, Ov, NIL, OPCODE },
   { PUSH, NIL, Ov, NIL, OPCODE },
   { PUSH, NIL, Ov, NIL, OPCODE },
   { PUSH, NIL, Ov, NIL, OPCODE },
   { PUSH, NIL, Ov, NIL, OPCODE },
   { PUSH, NIL, Ov, NIL, OPCODE },
   { PUSH, NIL, Ov, NIL, OPCODE },
   { PUSH, NIL, Ov, NIL, OPCODE },
   { POP, Ov, NIL, OPCODE, NIL },
   { POP, Ov, NIL, OPCODE, NIL },
   { POP, Ov, NIL, OPCODE, NIL },
   { POP, Ov, NIL, OPCODE, NIL },
   { POP, Ov, NIL, OPCODE, NIL },
   { POP, Ov, NIL, OPCODE, NIL },
   { POP, Ov, NIL, OPCODE, NIL },
   { POP, Ov, NIL, OPCODE, NIL },
   { PUSHA, NIL, NIL, NIL, NIL },
   { POPA, NIL, NIL, NIL, NIL },
   { BOUND, NIL, Ov, NIL, REG },
   { ARPL, Ow, Ow, RM, REG },
   { SEGFS, NIL, NIL, NIL, NIL },
   { SEGGS, NIL, NIL, NIL, NIL },
   { OPSIZE, NIL, NIL, NIL, NIL },
   { ADSIZE, NIL, NIL, NIL, NIL },
   { PUSH, NIL, Ov, NIL, IMM },
   { IMUL, Ov, Ov, REG, RM }, // 3 operands: what should I do? 
   { PUSH, NIL, Ob, NIL, IMM },
   { IMUL, Ob, Ob, REG, RM }, // 3 operands: what should I do? 
   { }, // ins, outs ? 
   { },
   { },
   { },
   { }, // cond jumps ? 
   { },
   { },
   { },
   { },
   { },
   { },
   { },
   { },
   { },
   { },
   { },
   { },
   { },
   { },
   { },
   { }, // immediate group 1 ? 
   { },
   { },
   { },
   { TEST, Ob, Ob, RM, REG },
   { TEST, Ov, Ov, RM, REG },
   { XCHG, Ob, Ob, RM, REG }, // xchg has 2 sources, 2 destinations ? 
   { XCHG, Ov, Ov, RM, REG },
   { MOV, Ob, Ob, RM, REG },
   { MOV, Ov, Ov, RM, REG },
   { MOV, Ob, Ob, REG, RM },
   { MOV, Ov, Ov, REG, RM },
   { MOV, Ow, Ow, RM, SEG },
   { LEA } // lea ? 
   { MOVSEG, Ow, Ow, REG, RM },
   { POP, Ov, NIL, RM, NIL },
   { NOP, NIL, NIL, NIL, NIL },
   { XCHG, Ov, Ov, EAX, OPCODE },
   { XCHG, Ov, Ov, EAX, OPCODE },
   { XCHG, Ov, Ov, EAX, OPCODE },
   { XCHG, Ov, Ov, EAX, OPCODE },
   { XCHG, Ov, Ov, EAX, OPCODE },
   { XCHG, Ov, Ov, EAX, OPCODE },
   { XCHG, Ov, Ov, EAX, OPCODE },
   { }, // CBW 
   { }, // CWD/CDQ 
   { }, // CALL Ap 
   { }, // WAIT 
   { }, // PUSHF 
   { }, // POPF 
   { }, // SAHF 
   { }, // LAHF 
   { MOV, Ob, Ob, EAX, OFFSETv },
   { MOV, Ov, Ov, EAX, OFFSETv },
   { MOV, Ob, Ob, OFFSETv, EAX },
   { MOV, Ov, Ov, OFFSETv, EAX },
   { }, // movs, cmps ? 
   { },
   { },
   { },
   { TEST, Ob, Ob, EAX, IMM },
   { TEST, Ov, Ov, EAX, IMM },
   { }, // stos, lods, scas ? 
   { },
   { },
   { },
   { },
   { },
   { MOV, Ob, Ob, OPCODE, IMM },
   { MOV, Ob, Ob, OPCODE, IMM },
   { MOV, Ob, Ob, OPCODE, IMM },
   { MOV, Ob, Ob, OPCODE, IMM },
   { MOV, Ob, Ob, OPCODE, IMM },
   { MOV, Ob, Ob, OPCODE, IMM },
   { MOV, Ob, Ob, OPCODE, IMM },
   { MOV, Ob, Ob, OPCODE, IMM },
   { MOV, Ov, Ov, OPCODE, IMM },
   { MOV, Ov, Ov, OPCODE, IMM },
   { MOV, Ov, Ov, OPCODE, IMM },
   { MOV, Ov, Ov, OPCODE, IMM },
   { MOV, Ov, Ov, OPCODE, IMM },
   { MOV, Ov, Ov, OPCODE, IMM },
   { MOV, Ov, Ov, OPCODE, IMM },
   { MOV, Ov, Ov, OPCODE, IMM },
// etc  

}
*/




__inline int checkreg8(int index)
{if(!reg_markers[index & 3]) return(index); else {reportreg8(index); return(index);}}

__inline int checkreg16(int index)
{if(!reg_markers[index]) return(index); else {reportreg16(index); return(index);}}

__inline int checkreg32(int index)
{if(!reg_markers[index]) return(index); else {reportreg32(index); return(index);}}

__inline int checkreadreg8(int index)
{if(!reg_markers[index & 3]) return(index); else {reportreadreg8(index); return(index);}}

__inline int checkreadreg16(int index)
{if(!reg_markers[index]) return(index); else {reportreadreg16(index); return(index);}}

__inline int checkreadreg32(int index)
{if(!reg_markers[index]) return(index); else {reportreadreg32(index); return(index);}}

__inline int checkwritereg8(int index)
{if(!reg_markers[index & 3]) return(index); else {reportwritereg8(index); return(index);}}

__inline int checkwritereg16(int index)
{if(!reg_markers[index]) return(index); else {reportwritereg16(index); return(index);}}

__inline int checkwritereg32(int index)
{if(!reg_markers[index]) return(index); else {reportwritereg32(index); return(index);}}

__inline int checkbasereg16(int index)
{if(!reg_markers[index]) return(index); else {reportbasereg16(index); return(index);}}

__inline int checkindexreg16(int index)
{if(!reg_markers[index]) return(index); else {reportindexreg16(index); return(index);}}

static void reportreg8(int index)
{
   return;
}

static void reportreadreg8(int index)
{
   return;
}

static void reportwritereg8(int index)
{
   return;
}

static void reportreg16(int index)
{
   return;
}

static void reportreadreg16(int index)
{
   return;
}

static void reportwritereg16(int index)
{
   return;
}

static void reportreg32(int index)
{

   report_tag = REG32_ACCESS;
   nadf_record[41].object = NULL;
   nadf_record[42].object = NULL;
   nadf_record[43].object = NULL;
   nadf_record[44].object = NULL;
   submit_record(nadf_record);   
   return;
}

static void reportreadreg32(int index)
{
   report_tag = REG32_ACCESS;
   nadf_record[41].object = NULL;
   nadf_record[42].object = NULL;
   nadf_record[43].object = NULL;
   nadf_record[44].object = NULL;
   submit_record(nadf_record);   
   return;
}

static void reportwritereg32(int index)
{
   report_tag = REG32_ACCESS;
   nadf_record[41].object = NULL;
   nadf_record[42].object = NULL;
   nadf_record[43].object = NULL;
   nadf_record[44].object = NULL;
   submit_record(nadf_record);   
   return;
}

static void reportbasereg16(int index)
{
   return;
}

static void reportindexreg16(int index)
{
   return;
}
