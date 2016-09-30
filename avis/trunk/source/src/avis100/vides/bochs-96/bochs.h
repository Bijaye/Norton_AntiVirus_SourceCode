/*
Copyright Notice
================
BOCHS is Copyright 1994,1995,1996 by Kevin P. Lawton.

BOCHS is commercial software.

For more information, read the file 'LICENSE' included in the bochs
distribution.  If you don't have access to this file, or have questions
regarding the licensing policy, the author may be contacted via:

    US Mail:  Kevin Lawton
              528 Lexington St.
              Waltham, MA 02154

    EMail:    bochs@world.std.com
*/




#ifndef BX_BOCHS_H
#  define BX_BOCHS_H 1

#include <stdarg.h>
#include <stdio.h>

#include "config.h"
#include "..\bochsw\watcher\watcher.h"

#if USE_INLINE
#  define INLINE static inline
#else
#  define INLINE 
#endif /* USE_INLINE */



#ifdef WIN32
#define BOCHS_INI "bochs.ini"
#else
#define BOCHS_INI ".bochsrc"
#endif


/*typedef unsigned char  Boolean;*/
typedef unsigned Boolean;


/* maximum number of emulated devices allowed.  floppy, mda, etc...
   you can increase this to anything below 256 since an 8-bit handle
   is used for each device */
#define BX_MAX_IO_DEVICES 20

/* number of IRQ lines supported.  In an ISA PC there are two
   PIC chips cascaded together.  each has 8 IRQ lines, so there
   should be 16 IRQ's total */
#define BX_MAX_IRQS 16



#define BX_NO_IRQ  -1
#define BX_NO_FD   -1

#define BX_FD_EVENT_READ       1
#define BX_FD_EVENT_WRITE      2
#define BX_FD_EVENT_EXCEPTION  4


#define BX_FLOPPY_1_2    10 /* 1.2M  5.25" */
#define BX_FLOPPY_1_44   11 /* 1.44M 3.5" */
#define BX_FLOPPY_0_720  12 /* 720K  3.5" */
#define BX_FLOPPY_AUTO   13 /* auto sense later */
#define BX_FLOPPY_NONE   14 /* floppy not present */
// Fred
#define BX_FLOPPY_0_360  15 /* 360K  5.25" */

typedef struct {
  Bit32u  (* read_funct)(Bit32u addr, unsigned io_len);
  void    (* write_funct)(Bit32u addr, Bit32u value, unsigned io_len);
  char    *handler_name;
  Bit32u  start_addr;
  Bit32u  end_addr;
  int     irq;
  } bx_iodev_t;

typedef struct {
  Bit32u (* read_funct) (Bit32u addr, unsigned io_len);
  void   (* write_funct) (Bit32u addr, Bit32u value, unsigned io_len);
  char *        handler_name;                       /* name of device */
  int           irq;                                /* associated irq if any */
  } bx_io_handler_t;

typedef struct {
  Bit32u  period;
  Bit32u  remaining;
  Boolean active;
  Boolean continuous;
  Boolean triggered;
  void (* funct) (void);
  } bx_timer_t;

#define BX_MAX_TIMERS 10
#define BX_NULL_TIMER_HANDLE 10000 /* set uninitialized timer handles to this */

#define BX_MAX_REGISTERED_FDS  20


void bx_register_io_handlers(void);
int  bx_register_io_handler( bx_iodev_t );
void bx_register_int_vector(Bit8u interrupt, Bit8u* code, int code_size,
                            void (* funct)(int vector)
#ifdef WIN32DEBUG
			    ,int debug_code
#endif
			    );
int  bx_register_timer(void (*funct)(void), Bit32u useconds,
                       Boolean continous, Boolean active);
void bx_activate_timer(int timer_index, Boolean activate, Bit32u useconds);

void bx_add_input(int fd, int event_type, void (* funct)(void));


/* --- DEFINES --- */

/* segment register encoding */
#define BX_SEG_REG_ES    0
#define BX_SEG_REG_CS    1
#define BX_SEG_REG_SS    2
#define BX_SEG_REG_DS    3
#define BX_SEG_REG_FS    4
#define BX_SEG_REG_GS    5



#ifdef LITTLE_ENDIAN
#define BX_REG8L_OFFSET 0
#define BX_REG8H_OFFSET 1
#define BX_REG16_OFFSET 0
#else /* BIG_ENDIAN */
#define BX_REG8L_OFFSET 3
#define BX_REG8H_OFFSET 2
#define BX_REG16_OFFSET 2
#endif /* ifdef LITTLE_ENDIAN */

#define BX_REGISTER_REF 10
#define BX_MEMORY_REF   11

#define BX_8BIT_REG_AL  0
#define BX_8BIT_REG_CL  1
#define BX_8BIT_REG_DL  2
#define BX_8BIT_REG_BL  3
#define BX_8BIT_REG_AH  4
#define BX_8BIT_REG_CH  5
#define BX_8BIT_REG_DH  6
#define BX_8BIT_REG_BH  7

#define BX_16BIT_REG_AX 0
#define BX_16BIT_REG_CX 1
#define BX_16BIT_REG_DX 2
#define BX_16BIT_REG_BX 3
#define BX_16BIT_REG_SP 4
#define BX_16BIT_REG_BP 5
#define BX_16BIT_REG_SI 6
#define BX_16BIT_REG_DI 7

#define BX_32BIT_REG_EAX 0
#define BX_32BIT_REG_ECX 1
#define BX_32BIT_REG_EDX 2
#define BX_32BIT_REG_EBX 3
#define BX_32BIT_REG_ESP 4
#define BX_32BIT_REG_EBP 5
#define BX_32BIT_REG_ESI 6
#define BX_32BIT_REG_EDI 7

/* Hooman: these were changed (AL -> AL_) and replaced by the ones I wrote a little further,
to keep trak of register modifications */

#define AL_ (*bx_8bit_gen_reg[BX_8BIT_REG_AL])
#define AH_ (*bx_8bit_gen_reg[BX_8BIT_REG_AH])
#define BL_ (*bx_8bit_gen_reg[BX_8BIT_REG_BL])
#define BH_ (*bx_8bit_gen_reg[BX_8BIT_REG_BH])
#define CL_ (*bx_8bit_gen_reg[BX_8BIT_REG_CL])
#define CH_ (*bx_8bit_gen_reg[BX_8BIT_REG_CH])
#define DL_ (*bx_8bit_gen_reg[BX_8BIT_REG_DL])
#define DH_ (*bx_8bit_gen_reg[BX_8BIT_REG_DH])

#define AX_ (*bx_16bit_gen_reg[BX_16BIT_REG_AX])
#define BX_ (*bx_16bit_gen_reg[BX_16BIT_REG_BX])
#define CX_ (*bx_16bit_gen_reg[BX_16BIT_REG_CX])
#define DX_ (*bx_16bit_gen_reg[BX_16BIT_REG_DX])
#define SP_ (*bx_16bit_gen_reg[BX_16BIT_REG_SP])
#define BP_ (*bx_16bit_gen_reg[BX_16BIT_REG_BP])
#define SI_ (*bx_16bit_gen_reg[BX_16BIT_REG_SI])
#define DI_ (*bx_16bit_gen_reg[BX_16BIT_REG_DI])

#define EAX_ bx_cpu.eax
#define ECX_ bx_cpu.ecx
#define EDX_ bx_cpu.edx
#define EBX_ bx_cpu.ebx
#define ESP_ bx_cpu.esp
#define EBP_ bx_cpu.ebp
#define ESI_ bx_cpu.esi
#define EDI_ bx_cpu.edi
#define EIP_ bx_cpu.eip

#define BX_READ_8BIT_REG_(index)  (*bx_8bit_gen_reg[index])
#define BX_READ_16BIT_REG_(index) (*bx_16bit_gen_reg[index])
#define BX_READ_32BIT_REG_(index) (*bx_32bit_gen_reg[index])


#define BX_READ_16BIT_BASE_REG_(var, index) {\
  var = *bx_16bit_base_reg[index];\
  }

#define BX_READ_16BIT_INDEX_REG_(var, index) {\
  var = *bx_16bit_index_reg[index];\
  }

#define BX_WRITE_8BIT_REG_(index, val) {\
  *bx_8bit_gen_reg[index] = val; \
  }
#define BX_WRITE_16BIT_REG_(index, val) {\
  *bx_16bit_gen_reg[index] = val; \
  }
#define BX_WRITE_32BIT_REG_(index, val) {\
  *bx_32bit_gen_reg[index] = val; \
  }

/* Hooman: there */
#define AL (*bx_8bit_gen_reg[checkreg8(BX_8BIT_REG_AL)])
#define AH (*bx_8bit_gen_reg[checkreg8(BX_8BIT_REG_AH)])
#define BL (*bx_8bit_gen_reg[checkreg8(BX_8BIT_REG_BL)])
#define BH (*bx_8bit_gen_reg[checkreg8(BX_8BIT_REG_BH)])
#define CL (*bx_8bit_gen_reg[checkreg8(BX_8BIT_REG_CL)])
#define CH (*bx_8bit_gen_reg[checkreg8(BX_8BIT_REG_CH)])
#define DL (*bx_8bit_gen_reg[checkreg8(BX_8BIT_REG_DL)])
#define DH (*bx_8bit_gen_reg[checkreg8(BX_8BIT_REG_DH)])

#define AX (*bx_16bit_gen_reg[checkreg16(BX_16BIT_REG_AX)])
#define BX (*bx_16bit_gen_reg[checkreg16(BX_16BIT_REG_BX)])
#define CX (*bx_16bit_gen_reg[checkreg16(BX_16BIT_REG_CX)])
#define DX (*bx_16bit_gen_reg[checkreg16(BX_16BIT_REG_DX)])
#define SP (*bx_16bit_gen_reg[checkreg16(BX_16BIT_REG_SP)])
#define BP (*bx_16bit_gen_reg[checkreg16(BX_16BIT_REG_BP)])
#define SI (*bx_16bit_gen_reg[checkreg16(BX_16BIT_REG_SI)])
#define DI (*bx_16bit_gen_reg[checkreg16(BX_16BIT_REG_DI)])
#define IP (*((Bit16u *) (((Bit8u *) &bx_cpu.eip) + BX_REG16_OFFSET)))


#define EAX (*bx_32bit_gen_reg[checkreg32(BX_32BIT_REG_EAX)])
#define ECX (*bx_32bit_gen_reg[checkreg32(BX_32BIT_REG_ECX)])
#define EDX (*bx_32bit_gen_reg[checkreg32(BX_32BIT_REG_EDX)])
#define EBX (*bx_32bit_gen_reg[checkreg32(BX_32BIT_REG_EBX)])
#define ESP (*bx_32bit_gen_reg[checkreg32(BX_32BIT_REG_ESP)])
#define EBP (*bx_32bit_gen_reg[checkreg32(BX_32BIT_REG_EBP)])
#define ESI (*bx_32bit_gen_reg[checkreg32(BX_32BIT_REG_ESI)])
#define EDI (*bx_32bit_gen_reg[checkreg32(BX_32BIT_REG_EDI)])
#define EIP bx_cpu.eip

#define BX_READ_8BIT_REG(index)  (*bx_8bit_gen_reg[checkreadreg8(index)])
#define BX_READ_16BIT_REG(index) (*bx_16bit_gen_reg[checkreadreg16(index)])
#define BX_READ_32BIT_REG(index) (*bx_32bit_gen_reg[checkreadreg32(index)])

#define BX_READ_16BIT_BASE_REG(var, index) {\
  var = *bx_16bit_base_reg[checkbasereg16(index)];\
  }

#define BX_READ_16BIT_INDEX_REG(var, index) {\
  var = *bx_16bit_index_reg[checkindexreg16(index)];\
  }

#define BX_WRITE_8BIT_REG(index, val) {\
  *bx_8bit_gen_reg[checkwritereg8(index)] = val; \
  }
#define BX_WRITE_16BIT_REG(index, val) {\
  *bx_16bit_gen_reg[checkwritereg16(index)] = val; \
  }
#define BX_WRITE_32BIT_REG(index, val) {\
  *bx_32bit_gen_reg[checkwritereg32(index)] = val; \
  }
/* end Hooman */


#define BX_DECODE_MODRM(mod_rm_byte, mod, reg, rm) \
  {\
  mod = (Bit8u) (mod_rm_byte) >> 6; \
  reg = (Bit8u) ((mod_rm_byte) >> 3) & 0x07; \
  rm  = (Bit8u) (mod_rm_byte) & 0x07; \
  }

#if BX_CPU < 2
#  define BX_HANDLE_EXCEPTION() 
#else
#  define BX_HANDLE_EXCEPTION() { if (bx_cpu.errno) return; }
#endif



#define TF bx_cpu.eflags.tf
#define IF bx_cpu.eflags.if_
#define DF bx_cpu.eflags.df

#if 1 /* riad */
#define CF bx_cpu.eflags.cf
#endif

#define IOPL bx_cpu.eflags.iopl
#define CPL  bx_cpu.cs.selector.rpl

#define BX_UD_EXCEPTION   6
#define BX_NM_EXCEPTION   7
#define BX_DF_EXCEPTION   8
#define BX_TS_EXCEPTION  10
#define BX_NP_EXCEPTION  11
#define BX_SS_EXCEPTION  12
#define BX_GP_EXCEPTION  13
#define BX_PF_EXCEPTION  14
#define BX_MF_EXCEPTION  16
#define BX_AC_EXCEPTION  17


#define BX_READ    10
#define BX_WRITE   11

#ifndef BX_MEMORY_C
extern Boolean bx_enable_a20;
#endif


/* --- TYPEDEFS --- */

typedef struct {
  /* 31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16
   * ==|==|=====|==|==|==|==|==|==|==|==|==|==|==|==
   *  0| 0| 0| 0| 0| 0| 0| 0| 0| 0|ID|VP|VF|AC|VM|RF
   *
   * 15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0
   * ==|==|=====|==|==|==|==|==|==|==|==|==|==|==|==
   *  0|NT| IOPL|OF|DF|IF|TF|SF|ZF| 0|AF| 0|PF| 1|CF
   */
  
  Boolean cf;
  Boolean bit1;
  Bit8u   pf_byte;  /* PF derived from last result byte when needed */
  Boolean bit3;
  Boolean af;
  Boolean bit5;
  Boolean zf;
  Boolean sf;
  Boolean tf;
  Boolean if_;
  Boolean df;
  Boolean of;
#if BX_CPU >= 2
  Bit8u   iopl;
  Boolean nt;
#endif
  Boolean bit15;
#if BX_CPU >= 3
  Boolean rf;
  Boolean vm;
#endif
#if BX_CPU >= 4
  Boolean ac;
#endif
  } bx_flags_reg_t;


#if BX_CPU >= 2
typedef struct {
#if BX_CPU >= 3
  Boolean pg; /* paging */
#endif

#if BX_CPU >= 4
  /* uncomment initialization in main.c */
  Boolean cd; /* cache disable */
  Boolean nw; /* no write-through */
  Boolean am; /* alignment mask */
  Boolean wp; /* write-protect */
  Boolean ne; /* numerics exception */
#endif
  /*Boolean et;???*/ /* extension type */

  /* reserved bit = Extension type */
  Boolean ts; /* task switched */
  Boolean em; /* emulate math coprocessor */
  Boolean mp; /* monitor coprocessor */
  Boolean pe; /* protected mode enable */
  } bx_cr0_t;
#endif


typedef struct { /* bx_selector_t */
  Bit16u value;   /* the 16bit value of the selector */
#if BX_CPU >= 2
    /* the following fields are extracted from the value field in protected
       mode only.  They're used for sake of efficiency */
  Bit16u index;   /* 13bit index extracted from value in protected mode */
  Bit8u  ti;      /* table indicator bit extracted from value */
  Bit8u  rpl;     /* RPL extracted from value */
#endif
  } bx_selector_t;



typedef struct {
  Boolean valid;         /* 0 = invalid, 1 = valid */
  Boolean p;             /* present */
  Bit8u   dpl;           /* descriptor privilege level 0..3 */
  Boolean segment;       /* 0 = system/gate, 1 = data/code segment */
  Bit8u   type;          /* For system & gate descriptors, only
                          *  0 = invalid descriptor (reserved)
                          *  1 = 286 available Task State Segment (TSS)
                          *  2 = LDT descriptor
                          *  3 = 286 busy Task State Segment (TSS)
                          *  4 = 286 call gate
                          *  5 = task gate
                          *  6 = 286 interrupt gate
                          *  7 = 286 trap gate
                          *  8 = (reserved)
                          *  9 = 386 available TSS
                          * 10 = (reserved)
                          * 11 = 386 busy TSS
                          * 12 = 386 call gate
                          * 13 = (reserved)
                          * 14 = 386 interrupt gate
                          * 15 = 386 trap gate */
  union {
  struct {
    Boolean executable;    /* 1=code, 0=data or stack segment */
    Boolean c_ed;          /* for code: 1=conforming,
                              for data/stack: 1=expand down */
    Boolean r_w;           /* for code: readable?, for data/stack: writeable? */
    Boolean a;             /* accessed? */
    Bit32u  base;          /* base address: 286=24bits, 386=32bits */
    Bit32u  limit;         /* limit: 286=16bits, 386=20bits */
    Bit32u  limit_scaled;  /* for efficiency, this contrived field is set to
                            * limit for byte granular, and
                            * (limit << 12) | 0xfff for page granular seg's
                            */
#if BX_CPU >= 3
    Boolean g;             /* granularity: 0=byte, 1=4K (page) */
    Boolean d_b;           /* default size: 0=16bit, 1=32bit */
    Boolean avl;           /* available for use by system */
#endif
    } segment;
  struct {
    Bit8u   word_count;    /* 5bits (0..31) #words to copy from caller's stack
                            * to called procedure's stack.  (call gates only)*/
    Bit16u  dest_selector;
    Bit16u  dest_offset;
    } gate286;
#if BX_CPU >= 3
  struct {
    Bit8u   dword_count;   /* 5bits (0..31) #dwords to copy from caller's stack
                            * to called procedure's stack.  (call gates only)*/
    Bit16u  dest_selector;
    Bit32u  dest_offset;
    } gate386;
#endif
  struct {
    Bit32u  base;          /* 24 bit 286 TSS base  */
    Bit16u  limit;         /* 16 bit 286 TSS limit */
    } tss286;
#if BX_CPU >= 3
  struct {
    Bit32u  base;          /* 32 bit 386 TSS base  */
    Bit32u  limit;         /* 20 bit 386 TSS limit */
    Boolean g;             /* granularity: 0=byte, 1=4K (page) */
    Boolean avl;           /* available for use by system */
    } tss386;
#endif
  struct {
    Bit32u  base;  /* 286=24 386+ =32 bit LDT base */
    Bit16u  limit; /* 286+ =16 bit LDT limit */
    } ldt;
    } u;

  } bx_descriptor_t;

typedef struct {
  bx_selector_t          selector;
  bx_descriptor_t  cache;
  } bx_segment_reg_t;


#if BX_CPU < 2
  /* no GDTR or IDTR register in an 8086 */
#else
typedef struct {
  Bit32u                 base;      /* base address: 24bits=286,32bits=386 */
  Bit16u                 limit;     /* limit, 16bits */
  } bx_global_segment_reg_t;
#endif


typedef struct {
  /* general register set */
  Bit32u eax; /* accumulator */
  Bit32u ebx; /* base */
  Bit32u ecx; /* count */
  Bit32u edx; /* data */
  Bit32u ebp; /* base pointer */
  Bit32u esi; /* source index */
  Bit32u edi; /* destination index */
  Bit32u esp; /* stack pointer */

  /* status and control flags register set */
  bx_flags_reg_t eflags; /* status and control flags */
  Boolean inhibit_interrupts;

  Bit32u eip;    /* instruction pointer */
#if BX_CPU > 0
  /* so that we can back up when handling faults, exceptions, etc.
     we need to store the value of the instruction pointer, before
     each fetch/execute cycle.
   */
  Bit32u prev_eip;
#endif

  bx_segment_reg_t *segment_override;

  /* user segment register set */
  bx_segment_reg_t  cs; /* code segment register */
  bx_segment_reg_t  ss; /* stack segment register */
  bx_segment_reg_t  ds; /* data segment register */
  bx_segment_reg_t  es; /* extra segment register */
  bx_segment_reg_t  fs; /* extra segment register */
  bx_segment_reg_t  gs; /* extra segment register */

  /* system segment registers */
#if BX_CPU >= 2
  bx_global_segment_reg_t gdtr; /* global descriptor table register */
  bx_global_segment_reg_t idtr; /* interrupt descriptor table register */
#endif
  bx_segment_reg_t        ldtr; /* interrupt descriptor table register */
  bx_segment_reg_t        tr;   /* task register */


  /* debug registers 0-7 (unimplemented) */
#if BX_CPU >= 3
  Bit32u dr0;
  Bit32u dr1;
  Bit32u dr2;
  Bit32u dr3;
  Bit32u dr4;
  Bit32u dr5;
  Bit32u dr6;
  Bit32u dr7;
#endif

  /* TR3 - TR7 (Test Register 3-7), unimplemented */
#if 0
  Bit32u tr3;
  Bit32u tr4;
  Bit32u tr5;
  Bit32u tr6;
  Bit32u tr7;
#endif

  /* Control registers */
#if BX_CPU >= 2
  bx_cr0_t  cr0;
  Bit32u    cr3;
#endif

  Boolean EXT; /* 1 if processing external interrupt or exception
                * or if not related to current instruction,
                * 0 if current CS:IP caused exception */
  unsigned errno;   /* signal exception during instruction emulation */
  Boolean INTR;
#if BX_CPU >= 3
  Boolean is_32bit_opsize;
  Boolean is_32bit_addrsize;
#endif
  } bx_cpu_t;


#if BX_CPU >= 2
#  define bx_real_mode() (!bx_cpu.cr0.pe)
#endif

#if BX_CPU == 2
#  define bx_protected_mode() (bx_cpu.cr0.pe)
#endif

#if BX_CPU >= 3
#  if BX_SUPPORT_V8086_MODE
#    define bx_v8086_mode() (bx_cpu.eflags.vm)
#    define bx_protected_mode() (bx_cpu.cr0.pe && !bx_cpu.eflags.vm)
#  else
#    define bx_v8086_mode() (0)
#    define bx_protected_mode() (bx_cpu.cr0.pe)
#  endif
#endif


/* --- EXTERNS --- */

extern Bit8u  *bx_8bit_gen_reg[8];
extern Bit16u *bx_16bit_gen_reg[8];
extern Bit32u *bx_32bit_gen_reg[8];

extern Bit16u *bx_16bit_base_reg[8];
extern Bit16u *bx_16bit_index_reg[8];


#ifndef BX_MAIN_C
extern FILE *bx_logfd;
extern volatile Boolean bx_async_event;
extern volatile Boolean bx_fd_event;
#endif

extern Boolean bx_single_step_event;


extern Boolean bx_parity_lookup[256];

#define BX_REPE_PREFIX  10
#define BX_REPNE_PREFIX 11

extern Boolean bx_rep_prefix;



#if BX_EXTERNAL_ENVIRONMENT==0
extern bx_cpu_t bx_cpu;
#endif


typedef struct {
  Boolean floppy;
  Boolean keyboard;
  Boolean video;
  Boolean disk;
  Boolean pit;
  Boolean pic;
  Boolean bios;
  Boolean cmos;
  Boolean protected;
  Boolean a20;
  Boolean interrupts;
  Boolean exceptions;
  Boolean unsupported;
  Boolean temp;
  Boolean reset;
  Boolean debugger;
  Boolean mouse;
  Boolean io;
  Boolean xms;
  Boolean v8086;
  Boolean paging;
  Boolean creg;
  Boolean dreg;
  } bx_debug_t;

typedef struct {
  char path[512];
  unsigned type;
  } bx_floppy_options;

typedef struct {
  char path[512];
  } bx_disk_options;

typedef struct {
  char *path;
  } bx_rom_options;

typedef struct {
  bx_floppy_options floppya;
  bx_floppy_options floppyb;
  bx_disk_options   diskc;
  bx_rom_options    rom;
  char              bootdrive[2];
  unsigned long     hga_update_interval;
  } bx_options_t;

extern bx_options_t bx_options;

#ifndef BX_MAIN_C
extern bx_debug_t bx_dbg;
#endif

#include "memory.h"
#include "lazy_flags.h"

/* --- PROTOTYPES --- */

void bx_decode_exgx(unsigned *reg_index, Bit32u *rm_addr,
    unsigned *rm_type, bx_segment_reg_t **rm_seg_reg);







void bx_printf(char *fmt, ...);
void bx_panic(char *fmt, ...);

void   bx_load_seg_reg(bx_segment_reg_t *seg, Bit16u new_value);

void bx_virtual_block_write(bx_segment_reg_t *seg, Bit16u offset,
  Bit32u length, void *data);
void bx_virtual_block_read(bx_segment_reg_t *seg, Bit16u offset,
  Bit32u length, void *data);



Bit16u bx_read_segment_register(Bit32u index);
void bx_write_segment_register(Bit32u index, Bit16u value);

void bx_push_16(Bit16u value);
void bx_push_32(Bit32u value);
void bx_pop_16(Bit16u *value);
void bx_pop_32(Bit32u *value);
void bx_write_flags(Bit16u flags, Boolean change_IOPL, Boolean change_IF);
void bx_write_eflags(Bit32u flags, Boolean change_IOPL, Boolean change_IF,
                     Boolean change_VM, Boolean change_RF);
Bit16u bx_read_flags(void);
Bit32u bx_read_eflags(void);
void bx_decode(void);

void bx_init_memory(int memsize);
void bx_set_interrupt_vector(int interrupt, Bit32u address);
void bx_invoke_interrupt(int interrupt);


void  bx_outp(Bit16u addr, Bit32u value, unsigned io_len);
Bit32u bx_inp(Bit16u addr, unsigned io_len);
void  bx_set_INTR(Boolean value);

#if USE_DEBUGGER > 0
void bx_debug(bx_segment_reg_t *seg, Bit32u offset);
#else
#  define bx_debug(seg, offset)
#endif

char *bx_strseg(bx_segment_reg_t *seg);

void bx_interrupt(Bit8u vector, Boolean is_INT, Boolean is_error_code);

#if BX_CPU >= 2
void bx_exception(unsigned vector, Bit16u error_code, Boolean is_INT);
void bx_task_gate(Bit16u selector, Boolean link);
#endif


int bx_int_number(bx_segment_reg_t *seg);

void bx_parse_descriptor(Bit32u dword1, Bit32u dword2, bx_descriptor_t *temp);
void bx_load_cs(bx_selector_t *selector,
  bx_descriptor_t *descriptor, Bit8u cpl);
void bx_load_ss(bx_selector_t *selector, bx_descriptor_t *descriptor,
  Bit8u cpl);
void bx_parse_selector(Bit16u raw_selector, bx_selector_t *selector);
void bx_load_ldtr(bx_selector_t *selector, bx_descriptor_t *descriptor);
void bx_fetch_raw_descriptor(bx_selector_t *selector,
                        Bit32u *dword1, Bit32u *dword2, Bit8u exception);
Boolean bx_fetch_raw_descriptor2(bx_selector_t *selector,
                        Bit32u *dword1, Bit32u *dword2);

Boolean bx_can_push(bx_descriptor_t *descriptor, Bit32u esp, Bit32u bytes);
Boolean bx_can_pop(Bit32u bytes);

void bx_reset_cpu(void);

void bx_validate_seg_regs(void);
void bx_set_enable_a20(Bit8u value);
Boolean bx_get_enable_a20(void);

void bx_start_timers(void);
Bit32u bx_get_timer(int index);

void bx_enter_protected_mode(void);

void bx_jump_protected(Bit16u cs, Bit32u disp32);
void bx_call_protected(Bit16u cs, Bit32u disp32);
void bx_return_protected(Bit16u pop_bytes);
void bx_iret_protected(void);

#define BX_TASK_FROM_JUMP         10
#define BX_TASK_FROM_CALL_OR_INT  11
#define BX_TASK_FROM_IRET         12

void bx_task_switch16(bx_selector_t *selector,
                      bx_descriptor_t *descriptor, unsigned source,
                      Bit32u dword1, Bit32u dword2);

void bx_task_switch32(bx_selector_t *selector,
                      bx_descriptor_t *descriptor, unsigned source,
                      Bit32u dword1, Bit32u dword2);

void bx_enable_paging(void);
void bx_disable_paging(void);
void bx_CR3_change(Bit32u value32);
void bx_stack_return_to_v86(Bit32u new_eip, Bit32u raw_cs_selector,
                            Bit32u flags32);
void bx_stack_return_from_v86(void);
void bx_int_from_v86_mode(bx_descriptor_t *gate_descriptor,
                          bx_selector_t   *cs_selector,
                          bx_descriptor_t *cs_descriptor);

#if BX_EXTERNAL_ENVIRONMENT==0
#  include "fetch.h"
#else
#  include "external_interface.h"
#endif

#ifdef WIN32DEBUG

#define H_BIOS_POST	1
#define H_BIOS_DEFAULT	2
#define H_NULL		3
#define H_BIOSLOADER	4

#define H_BIOSINT1A	5
#define H_CMOSRTC	6
#define H_FLOPPYINT13	7
#define H_HGAINT10	8
#define H_INT09		9
#define H_INT16		10
#define H_PARALLELINT	11
#define H_PITINT	12
#define H_SERIALINT	13
#define H_BIOSINT14	14
#define H_BIOSINT12	15
#define H_BIOSINT11	16
#define H_BIOSINT15	17
#define H_BIOSINT17	18

#endif

#ifdef WIN32
  #include <io.h>
  #include <string.h>
#endif

#endif  /* BX_BOCHS_H */
