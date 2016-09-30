








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

#define BIOS_C 1

#include "iodev.h"
#include "..\..\bochsw\watcher\watcher.h"


#if BX_EXTERNAL_ENVIRONMENT==0
static void bx_bios_default_int_handler(int vector);
static void bx_bios_bootstrap_loader(int vector);
static void bx_bios_int11_handler(int vector);
static void bx_bios_int12_handler(int vector);
static void bx_bios_int15_handler(int vector);
static void bx_bios_int17_handler(int vector);

static void bx_bios_post(int vector);

static char bios_date[] = "07/03/95";
static char bios_copyright[] = "(c) 1994,1995,1996 Kevin P. Lawton";
#endif

static Bit32u bx_port92_io_read_handler(Bit32u address, unsigned io_len);
static void  bx_port92_io_write_handler(Bit32u address, Bit32u value,
                                        unsigned io_len);


#if 0
static Bit8u typical_int_code[] = { 
  0x0F, /* 2-byte opcode for PSEUDO-INT */
  0x1F,
  0x00, /* set interrupt vector no here */
  0xFB, /* STI */
  0xCA, /* RETfar Iw (+2) */
  0x02,
  0x00
  };
#endif

Bit8u bx_isr_code_iret[4] = {
  0x0F, 0x1F, 0x00,         /* PSEUDO-INT  xyz,  IRET */
  0xCF
  };

Bit8u bx_isr_code_cf[23] = { 
  0x0F, /* 2-byte opcode for PSEUDO-INT */
  0x1F,
  0x00, /* set interrupt vector no here */
  0x72, 0x09,               /* jc   CARRY_SET       */
                            /* CARRY CLEAR:         */
  0x55,                     /* push bp              */
  0x8b, 0xec,               /* mov BP, SP           */
  0x80, 0x66, 0x06, 0xfe,   /* and SS:[BP][06], #fe */
  0x5d,                     /* pop BP               */
  0xcf,                     /* iret                 */

                            /* CARRY_SET:           */
  0x55,                     /* push bp              */
  0x8b, 0xec,               /* mov BP, SP           */
  0x80, 0x4e, 0x06, 0x01,   /* or SS:[BP][06], #01  */
  0x5d,                     /* pop BP               */
  0xcf,                     /* iret                 */
  };


Bit8u iret_int_code[1] = { 
 0xcf                       /* iret                 */
 };

#if BX_EXTERNAL_ENVIRONMENT==0
static Bit8u bootstrap_int_code[] = { 
  0x0F,                   /* 2-byte opcode for PSEUDO-INT */
  0x1F,
  0x19,                   /* INT 19h = bootstrap loader code */
  0xf4 /* halt */
  };
#endif

#if BX_EXTERNAL_ENVIRONMENT==0
static Bit8u bios_post_code[] = { 
  0x0F, 0x1F, 0xff, /* pseudo-int ffh: bios post */
                    /* ss:sp set from [0:467] in pseudo-int handler */

  0xb0, 0x0f,       /* mov AL, #0f */
  0xe6, 0x70,       /* outp 70h, AL : select CMOS register Fh */
  0xb0, 0x00,       /* mov AL, #00 */
  0xe6, 0x71,       /* outp 71h, AL : shutdown action is normal */

#if 0
  0xb0, 0x20,       /* mov al, #0x20 */
  0xe6, 0x20,       /* out 0x20, al    ;send EOI to PIC */
#endif

  0x07,             /* pop es */
  0x1f,             /* pop ds */
  0x61,             /* popa */
  0xcf,             /* iret */
  0xf4              /* halt */
  };
#endif

/* set the main memory size BIOS data item (in Kbytes)
 * subtract 1k for extended BIOS area
 * report only base memory, not extended mem
 */
static Bit16u base_memory_in_k = 640;
/*static Bit16u base_memory_in_k = 640 - 1;  -1 for extended BIOS data area */
static Bit16u extended_memory_in_k = (BX_PHY_MEM_MEGS - 1) * 1024;

extern bx_options_t bx_options;


  void
bx_iodev_init_hardware(void)
{
  bx_iodev_t  io_device;

  /* initialize IO devices on a hardware level; no BIOS hooks done here */

  /* --- system hardware ---*/
  io_device.read_funct   = bx_port92_io_read_handler;
  io_device.write_funct  = bx_port92_io_write_handler;
  io_device.handler_name = "System Control";
  io_device.start_addr   = 0x0092;
  io_device.end_addr     = 0x0092;
  io_device.irq          = BX_NO_IRQ;
  bx_register_io_handler(io_device);

  /*--- CMOS RAM & RTC ---*/
  bx_init_cmos_hardware();

#if BX_SUPPORT_VGA
  /*--- VGA adapter ---*/
  bx_init_vga_hardware();
#else
  /*--- HGA adapter ---*/
  bx_init_hga_hardware();
#endif

  /*--- 8259A PIC ---*/
  bx_init_pic_hardware();

  /*--- 8254 PIT ---*/
  bx_init_pit_hardware();

  /*--- 8237 DMA controller ---*/
  bx_init_dma_hardware();

  /*--- 8042 keyboard controller ---*/
  bx_init_keyboard_hardware();

  /*--- HARD DRIVE ---*/
  bx_init_hard_drive_hardware();

  /*--- PARALLEL PORT ---*/
  bx_init_parallel_hardware();

  /*--- SERIAL PORT ---*/
  bx_init_serial_hardware();

  /*--- FLOPPY DRIVE CONTROLLER ---*/
  bx_init_floppy_hardware();

  /*--- PS/2 MOUSE ---*/
  bx_init_mouse_hardware();

  bx_cmos_reg[0x15] = base_memory_in_k;
  bx_cmos_reg[0x16] = (base_memory_in_k >> 8);
  bx_cmos_reg[0x17] = extended_memory_in_k;
  bx_cmos_reg[0x18] = (extended_memory_in_k >> 8);
  bx_cmos_reg[0x30] = extended_memory_in_k;
  bx_cmos_reg[0x31] = (extended_memory_in_k >> 8);

  /* now perform checksum of CMOS memory */
  bx_checksum_cmos();
}


#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_post_setup(void)
{
  int i;
  Bit8u jump_to_post[] = { 0xea, 0x00, 0x00, 0x00, 0x00 };
  

  /* register pseudo-interrupt with given code addr, no-set intr */
#ifdef WIN32DEBUG2
  bx_printf("*** post_setup() : about to register on BIOS_POST [%08lX]\n", bx_bios_post);
#endif
  bx_register_int_vector((Bit8u) 0xff,
    (Bit8u *) NULL, 0, bx_bios_post
#ifdef WIN32DEBUG
      ,H_BIOS_POST
#endif
    );
#ifdef WIN32DEBUG2
  bx_printf("*** post_setup() : done\n");
#endif

  /* write post code to memory */
  for (i=0; i<sizeof(bios_post_code); i++) {
    bx_access_physical(BX_POST_CODE + i, 1, BX_WRITE,
      &bios_post_code[i]);
    }

  jump_to_post[1] = (BX_POST_CODE & 0xff);
  jump_to_post[2] = (BX_POST_CODE >> 8) & 0xff;
  jump_to_post[3] = 0;
  jump_to_post[4] = (BX_POST_CODE >> 12) & 0xf0;

  for (i=0; i<sizeof(jump_to_post); i++) {
    bx_access_physical(0xffff0 + i, 1, BX_WRITE,
      &jump_to_post[i]);
    }
}



  void
bx_jmp_post(void)
{
  Bit16u ip, cs;


  /* change CS:IP to beginning of POST */
  ip = (Bit16u) (BX_POST_CODE & 0xFFFF);
  cs = (Bit16u) ((BX_POST_CODE & 0xF0000) >> 4);

  bx_cpu.eip = ip;
  bx_load_seg_reg(&bx_cpu.cs, cs);
}


  static void
bx_bios_post(int vector)
{
  Bit32u phy_addr;
  Bit16u equip_word;
  Bit8u  zero8, manufact_test;
  Bit32u zero32;
  int    interrupt;
  int    i;
  Bit8u  sys_model_id, sys_submodel_id;
  Bit16u warm_boot_flag;

  static Boolean been_here = 0;
  Bit16u boot_flag;
  Bit16u ip, cs, sp, ss, es_value, ds_value;
  Bit8u bios_config_table[10];

#ifdef WIN32DEBUG2
  bx_printf("*** in bios_post() [%02X]\n", vector);
#endif

  if (bx_dbg.reset)
    bx_printf("bios_post: entering\n");

  bx_access_physical(0x472, 2, BX_READ, &boot_flag);
  if (bx_dbg.reset)
    bx_printf("bios_post: boot_flag=%04x\n",
      (unsigned) boot_flag);
  if (boot_flag == 0x1234) {
    if (bx_dbg.reset)
      bx_panic("bios_post(): skipping memory checks. :^)\n");
    }

  if (been_here) {
    Bit8u shutdown_status;
#ifdef WIN32DEBUG2
    bx_printf("*** bios_post() : been here\n");
#endif
    if (bx_dbg.reset)
      bx_printf("bios post: been here\n");

    bx_outp(0x70, 0x0f, 1);
    shutdown_status = bx_inp(0x71, 1);
    switch (shutdown_status) {
      case 0x9: /* return to BIOS extended memory block move */
        if (bx_dbg.reset)
          bx_printf("bios_post(): shutdown_status=9, returning\n");

        bx_access_physical(0x467, 2, BX_READ, &sp);
        bx_access_physical(0x469, 2, BX_READ, &ss);
        if (bx_dbg.reset)
          bx_printf("SS:SP ? from 0x467:0x469 = %04x:%04x\n",
            (unsigned) ss, (unsigned) sp);

        bx_cpu.esp = sp;
        bx_load_seg_reg(&bx_cpu.ss, ss);
        bx_access_physical( (ss<<4) + sp,     2, BX_READ, &es_value);
        bx_access_physical( (ss<<4) + sp + 2, 2, BX_READ, &ds_value);
        if (es_value != ds_value)
          bx_panic("BIOS_POST: ES(%04x) != DS(%04x)\n",
          (unsigned) es_value, (unsigned) ds_value);
        return;

      case 0x0a:
        /* jump to dword pointer at 40:67
         * no EOI issued
         * POST will disable interrupts & NMI before jump made
         * stack set to SS:SP=0:400h
         */
        if (bx_dbg.reset)
          bx_printf("bios_post(): shutdown_status=0xA\n");

        bx_access_physical(0x467, 2, BX_READ, &ip);
        bx_access_physical(0x469, 2, BX_READ, &cs);

        bx_cpu.esp = 0x0400;
        bx_load_seg_reg(&bx_cpu.ss, 0x0000);

        bx_cpu.eip = ip;
        bx_load_seg_reg(&bx_cpu.cs, cs);
        bx_cpu.eflags.if_ = 0;
        return;
        break;

      default: bx_panic("bios_post(): unknown shutdown status"
                            " %u\n", (unsigned) shutdown_status);
      }

    bx_panic("bios_boostrap_loader(): don't know how to resume after"
      "shutdown\n  when boot_flag = %u", (unsigned) boot_flag);
    return;
    }

  been_here = 1;

  zero8 = 0;
  zero32 = 0;

#ifdef WIN32DEBUG2
  bx_printf("*** bios_post() : about to loop over interrupts\n");
#endif
  /* ??? changed ff to fe, recode! */
  for (interrupt=0x00; interrupt <= 0xfe; interrupt++) {
#ifdef WIN32
    if ((interrupt >= 0x60)  &&  (interrupt <= 0x67))
#else
    if (interrupt >= 0x60  &&  interrupt <= 67)
#endif
    {
#ifdef WIN32DEBUG2
      bx_printf("SKIP INTERRUPT=%02X\n", interrupt);
#endif
      continue;
    }
    bx_isr_code_iret[2] = interrupt;
    bx_register_int_vector((Bit8u) interrupt,
      bx_isr_code_iret, sizeof(bx_isr_code_iret),
      bx_bios_default_int_handler
#ifdef WIN32DEBUG
      ,H_BIOS_DEFAULT
#endif      
      );
    }

  /* disable EMS */
  bx_access_physical(0x67 * 4, 4, BX_WRITE, &zero32);


  /* zero out BIOS data area (0x400 .. 0x500 inclusive) */
  for (phy_addr = 0x400; phy_addr <= 0x500 ; phy_addr++) {
    bx_access_physical(phy_addr, 1, BX_WRITE, &zero8);
    }

//#ifdef AUDIT_ENABLED

/* some viruses insist upon seeing null int vectors else they
won't infect */

  for (interrupt=0x60; interrupt <=0xfe; interrupt++)
      bx_access_physical(interrupt*4, 4, BX_WRITE, &zero32);

//#endif

  bx_access_physical(0x410, 2, BX_READ, &equip_word);
  equip_word |= 0x01; /* boot diskette drive installed */
  bx_access_physical(0x410, 2, BX_WRITE, &equip_word);

  /* set the manufacturing test BIOS data item to normal operation */
  manufact_test = 0; /* normal operation */
  bx_access_physical(0x412, 1, BX_WRITE, &manufact_test);

  bx_access_physical(0x413, 2, BX_WRITE, &base_memory_in_k);

  warm_boot_flag = 0x0000; /* 0x1234 = warm boot, skip memory tests after
                            * a Ctrl-Alt-Delete sequence */
  bx_access_physical(0x472, 2, BX_WRITE, &warm_boot_flag);

  /* INT 12h = Memory Size */
  bx_isr_code_iret[2] = 0x12;
  bx_register_int_vector(0x12,
      bx_isr_code_iret, sizeof(bx_isr_code_iret),
      bx_bios_int12_handler
#ifdef WIN32DEBUG
      ,H_BIOSINT12
#endif      
      );

  /* INT 11h = Equipment Configuration */
  bx_isr_code_iret[2] = 0x11;
  bx_register_int_vector(0x11,
      bx_isr_code_iret, sizeof(bx_isr_code_iret),
      bx_bios_int11_handler
#ifdef WIN32DEBUG
      ,H_BIOSINT11
#endif      
      );

  bx_isr_code_cf[2] = 0x15;
  bx_register_int_vector(0x15,
      bx_isr_code_cf, sizeof(bx_isr_code_cf),
      bx_bios_int15_handler
#ifdef WIN32DEBUG
      ,H_BIOSINT15
#endif      
      );

  bx_isr_code_iret[2] = 0x17;
  bx_register_int_vector(0x17,
      bx_isr_code_iret, sizeof(bx_isr_code_iret),
      bx_bios_int17_handler
#ifdef WIN32DEBUG
      ,H_BIOSINT17
#endif      
      );

  bx_register_int_vector(0x1c,
      iret_int_code, sizeof(iret_int_code),
      NULL
#ifdef WIN32DEBUG
      ,H_NULL
#endif      
      );

#if BX_CPU < 2
  /* ??? */
  sys_model_id    = 0xFC; /* PC XT */
  sys_model_id    = 0xFB; /* PC XT */
  sys_model_id    = 0xFF; /* PC XT */
  sys_model_id    = 0xFA; /* PC XT */
  sys_model_id    = 0xFE; /* PC XT */
#else
  sys_model_id = 0xFC; /* AT, 286 XT, and most 286 to Pentium clones */
#endif

  sys_submodel_id = 0x00; /* submodel 0 ? */

  bios_config_table[0] = 0x08; /* 8 bytes following this word */
  bios_config_table[1] = 0x00;
  bios_config_table[2] = sys_model_id;
  bios_config_table[3] = sys_submodel_id;
  bios_config_table[4] = 0x01; /* BIOS revision number */
  bios_config_table[5] = (0 << 7) | /* DMA channel 3 not used by hard disk */
                         (1 << 6) | /* 1=2 interrupt controllers present */
                         (0 << 5) | /* 1=RTC present */
#ifdef BX_CALL_INT15_4F
                         (1 << 4) | /* 1=BIOS calls int 15h, 4Fh every key */
#else
                         (0 << 4) | /* 0=BIOS doesn't call int 15h, 4Fh every key */
#endif
                         (0 << 3) | /* 1=wait for extern event supported */
                         (0 << 2) | /* no extended BIOS data area used */
                         (0 << 1);  /* AT bus */
#if BX_CPU < 2
  bios_config_table[6] = 0x00; /* int16h funct 9 not supported */
#else
  bios_config_table[6] = 0x00; /* int16h funct 9 not supported */
                               /* typmatic capabilities */
#endif
  bios_config_table[7] = 0x00;
  bios_config_table[8] = 0x00;
  bios_config_table[9] = 0x00;

  for (i=0; i<10; i++) {
    bx_access_physical(BX_BIOS_CONFIG_TABLE + i, 1, BX_WRITE,
      &bios_config_table[i]);
    }

  bx_access_physical(0xFFFFE, 1, BX_WRITE, &sys_model_id);

  for (i=0; i<strlen(bios_date); i++) {
    bx_access_physical(0xFFFF5 + i, 1, BX_WRITE, &bios_date[i]);
    }

  for (i=0; i<strlen(bios_copyright); i++) {
    bx_access_physical(BX_COPYRIGHT_LOC + i, 1, BX_WRITE, &bios_copyright[i]);
    }


#if BX_SUPPORT_VGA
  /*--- VGA adapter ---*/
  bx_init_vga_bios();
#else
  /*--- HGA adapter ---*/
  bx_init_hga_bios();
#endif

  /*--- 8259A PIC ---*/
  bx_init_pic_bios();

  /*--- 8254 PIT ---*/
  bx_init_pit_bios();

  /*--- 8237 DMA controller ---*/
  bx_init_dma_bios();

  /*--- 8042 keyboard controller ---*/
  bx_init_keyboard_bios();

  /*--- CMOS RAM & RTC ---*/
  bx_init_cmos_bios();

  /*--- HARD DRIVE ---*/
  bx_init_hard_drive_bios();

  /*--- PARALLEL PORT ---*/
  bx_init_parallel_bios();

  /*--- SERIAL PORT ---*/
  bx_init_serial_bios();

  /*--- FLOPPY DRIVE CONTROLLER ---*/
  bx_init_floppy_bios();

  /*--- PS/2 MOUSE ---*/
  bx_init_mouse_bios();


  /* bootstrap */
  bx_register_int_vector((Bit8u) 0x19,
      bootstrap_int_code, sizeof(bootstrap_int_code),
      bx_bios_bootstrap_loader
#ifdef WIN32DEBUG
      ,H_BIOSLOADER
#endif      
      );


{
  Bit16u cs, ip;
  /* start off CS:IP at bootstrap loader code */
  bx_access_physical(4 * 0x19,     2, BX_READ, &ip);
  bx_cpu.eip = ip;
  bx_access_physical(4 * 0x19 + 2, 2, BX_READ, &cs);
  bx_load_seg_reg(&bx_cpu.cs, cs);

  bx_printf("bios_post starting at bootstrap: CS=%04x, IP=%04x\n", (unsigned) cs,
    (unsigned) ip);
}

  bx_outp(0x21, 0x00, 1); /* master pic: all IRQ's unmasked */
  bx_outp(0xa1, 0x00, 1); /* slave  pic: all IRQ's unmasked */

#if 0
  /* hack for Minix: getting spurious IRQ0's on start up */
  bx_outp(0x21, 0xfd); /* master pic: all IRQ's masked (except keyboard)*/
  bx_outp(0xa1, 0xff); /* slave  pic: all IRQ's masked */
#endif
}

  static void
bx_bios_default_int_handler(int vector)
{
  bx_printf("BIOS: *** WARNING: Default int handler called for int %02x\n", vector);
  bx_printf("BIOS: ***   ah = %x, al = %x\n", (int) AH, (int) AL);
}

  static void
bx_bios_bootstrap_loader(int vector)
{
  static Boolean been_here = 0;
  extern int switchC;

#if 0 /* bill */
/*  if (been_here) {
    bx_panic("bios_boostrap_loader(): don't know how to run 2nd time\n");
    return;
    } */
#else
    if (been_here && switchC)
      bx_options.bootdrive[0]='c';
#endif

  been_here = 1;

  bx_printf("BIOS bootstrap loader invoked.\n");

  /* The first 512 bytes from 1st logical sector of floppy are loaded
     starting at location 0:7C00 and control is passed to 0:7C00.
     DL holds the drive no where the sector was last read
   */

  /* use Int 13h, funct 2 to read 1st logical sector */

  /* point es:bx to 0:7C00 */
  bx_load_seg_reg(&bx_cpu.es, 0x0000);
  BX = 0x7C00;

  AH = 2; /* function 2 */
  AL = 1; /* read 1 sector */
  CH = 0; /* track 0 */
  CL = 1; /* sector 1 */
  DH = 0; /* head 0 */
  if (bx_options.bootdrive[0] == 'a')
    DL = 0; /* floppy drive A */
  else
#if 1 /* bill */
    if (bx_options.bootdrive[0] == 'b')
      DL = 1; /* B -- you can't do this on a real PC though */
    else  
#endif
      DL = 0x80; /* hard drive 0 */

  bx_invoke_interrupt(0x13);

  if ( (AL != 1) || (bx_get_CF() != 0) ) {
    bx_panic("BIOS boostrap: INT 13h failed to load 1st sector.\n");
    }

  bx_load_seg_reg(&bx_cpu.cs, 0x0000);
  bx_cpu.eip = 0x7C00;
  /*DL = 0; don't do this now */ /* make sure DL points to boot drive */

#if 0
  /* ??? try to get Freedos to boot */
  bx_load_seg_reg(&bx_cpu.ds, 0x0040);
  bx_load_seg_reg(&bx_cpu.ss, 0x0030);
  bx_cpu.esp = 0x0100;
#endif

/*
   At this point, submit a POST-end record to asax.
   Very useful to hook int 13h's e.p. just after it is initialized. Fred
  */

  report_tag = POST_END; /* value 16, check it in watcher.h */
  nadf_record[41].object = NULL;
  nadf_record[42].object = NULL;
  nadf_record[43].object = NULL;
  nadf_record[44].object = NULL;
  submit_record(nadf_record);
}

  static void
bx_bios_int12_handler(int vector)
{
  Bit16u memory_in_k;

  bx_access_physical(0x413, 2, BX_READ, &memory_in_k);

  AX = memory_in_k;

#ifdef BX_DEBUG
  if (bx_dbg.bios)
    bx_printf("BIOS: *** INT 12h called, returning memory size of %dK\n",
      (int) AX);
#endif
}

  static
void bx_bios_int11_handler(int vector)
{
  Bit16u equip_word;

  bx_access_physical(0x410, 2, BX_READ, &equip_word);

  AX = equip_word;

#ifdef BX_DEBUG
  if (bx_dbg.bios)
    bx_printf("BIOS: *** INT 11h called, returning equip_word of %04x\n",
      (int) AX);
#endif
}

  static
void bx_bios_int15_handler(int vector)
{
  Bit16u es, bx;

#ifdef BX_DEBUG
  if (bx_dbg.bios)
    bx_printf("BIOS: *** INT 15h called with AH=%02x, AL=%02x\n",
      (int) AH, (int) AL);
#endif


  switch (AH) {
    case 0x24: /* A20 Control */
		if (AL == 0x00) {
			// disable A20
			bx_set_enable_a20(0);
			bx_set_CF(0);
			AH = 0;
		}
		else if (AL == 0x01) {
			// enable A20
			bx_set_enable_a20(1);
			bx_set_CF(0);
			AH = 0;
		}
		else if (AL == 0x02) {
			// get A20 status
			AL = bx_get_enable_a20()?0x01:0x00;
			bx_set_CF(0);
			AH = 0;
		}
		else if (AL == 0x03) {
			// query A20 gate support
			BX = 0x0003; // support both bit 1 of port 92h and keyboard controller
			bx_set_CF(0);
			AH = 0;
		}
		else {
			bx_printf("BIOS: int15: Func 24h, subfunc %02xh, A20 gate control not supported\n", (unsigned) AL);
			bx_set_CF(1);
			AH = 0x80;
		}
      break;

    case 0x41:
      bx_set_CF(1);
      AH = 0x80;
      break;

    case 0x4f:
      /* keyboard intercept, ignore */
#if BX_CPU < 2
      /* XT keyboard doesn't use */
      bx_set_CF(1);
      AH = 0x80;
#else
      /* AT keyboard.  BIOS just does an IRET */
      /* nothing required */
if (bx_get_CF() == 0) bx_printf("int15h: default handler encounters CF=0\n");
#endif
      break;

    case 0x88: /* extended memory size */
#if BX_CPU < 2
      AH = 0x80;
      bx_set_CF(1);
#else
      /* ??? change this back later... */
      /* number of 1K blocks of extended memory, subtract off 1st 1Meg */
      AX = extended_memory_in_k;
      bx_set_CF(0);
#endif
      break;

    case 0x90:
      /* Device busy interrupt.  Called by Int 16h when no key available */
      return;
      break;

    case 0x91:
      /* Interrupt complete.  Called by Int 16h when key becomes available */
      return;
      break;

    case 0xC0:
#if 0
      bx_set_CF(1);
      AH = 0x80;
      break;
#endif
      bx_set_CF(0);
      AH = 0;
      es = ((BX_BIOS_CONFIG_TABLE & 0xF0000) >> 4);
      bx =  (BX_BIOS_CONFIG_TABLE & 0x0FFFF);
      bx_load_seg_reg(&bx_cpu.es, es);
      BX = bx;
      break;

    case 0xc1:
      bx_set_CF(1);
      AH = 0x80;
      break;

    case 0xC2:
      bx_mouse_bios();
      break;

    case 0xC4:
      bx_printf("BIOS: *** int 15h function AX=%04x, BX=%04x not yet supported!\n",
        (unsigned) AX, (unsigned) BX);
      bx_set_CF(1);
      AH = 0x80;
      break;

    case 0xD8:
      bx_printf("BIOS: *** int 15h function AX=%04x, CL=%02x not yet supported!\n",
        (unsigned) AX, (unsigned) CL);
      bx_set_CF(1);
      break;

    case 0x87:
#if BX_CPU < 2
      bx_printf("BIOS: int15h AH=87\n");
      bx_printf("      CX = %04x\n", (unsigned) CX);
      bx_printf("BIOS: *** int 15h function AH=87 not yet supported!\n");
      bx_set_CF(1);
      AH = 0x80;
      break;
#else
      {
      bx_descriptor_t source_descriptor, dest_descriptor;
      Bit32u base, dword1, dword2;
      Bit16u words;
      Bit32u addr1, addr2;
      Bit32u w;
      Bit16u temp_word;

      base = bx_cpu.es.cache.u.segment.base + SI;
      words = CX;
      /* get source descriptor */
      bx_access_physical(base + 0x10 + 0, 4, BX_READ, &dword1);
      bx_access_physical(base + 0x10 + 4, 4, BX_READ, &dword2);
      bx_parse_descriptor(dword1, dword2, &source_descriptor);
      addr1 = source_descriptor.u.segment.base;

      /* get dest descriptor */
      bx_access_physical(base + 0x18 + 0, 4, BX_READ, &dword1);
      bx_access_physical(base + 0x18 + 4, 4, BX_READ, &dword2);
      bx_parse_descriptor(dword1, dword2, &dest_descriptor);
      addr2 = dest_descriptor.u.segment.base;
      if (source_descriptor.segment==0 || dest_descriptor.segment==0) {
        bx_panic("BIOS: int15 f87: XMS copy: descriptors not data\n");
        }
      if (addr1 == addr2) {
        bx_printf("BIOS: int15 f87: XMS copy of same addresses %06x\n",
          (unsigned) addr1);
        bx_set_CF(0);
        AH = 0; /* source copied into dest */
        return;
        }

      if (bx_dbg.xms) {
        bx_printf("XMS: xfer %u words from %06x to %06x\n",
          (unsigned) words,
          (unsigned) source_descriptor.u.segment.base,
          (unsigned) dest_descriptor.u.segment.base);
        }
      for (w=0; w<words; w++) {
        bx_access_physical(source_descriptor.u.segment.base + w*2,
          2, BX_READ, &temp_word);
        bx_access_physical(dest_descriptor.u.segment.base + w*2,
          2, BX_WRITE, &temp_word);
        }
      bx_set_CF(0);
      AH = 0;
#if BX_CPU >= 3
/* ??? */
bx_cpu.es.cache.u.segment.d_b = 1;
bx_cpu.es.cache.u.segment.limit = 0xfffff;
bx_cpu.es.cache.u.segment.limit_scaled = 0xffffffff;
bx_cpu.ds.cache.u.segment.d_b = 1;
bx_cpu.ds.cache.u.segment.limit = 0xfffff;
bx_cpu.ds.cache.u.segment.limit_scaled = 0xffffffff;
#endif
      /* !!! don't forget to turn on A20 first */
      if (bx_enable_a20 == 0) {
        bx_panic("bios: int15: f87: xms: A20 off!\n");
        }
      return;
      }
#endif
      break;

    case 0xbf:
      bx_printf("BIOS: *** int 15h function AH=bf not yet supported!\n");
      bx_set_CF(1);
      AH = 0x80;
      break;

    case 0xe0:
      bx_printf("BIOS: *** int 15h function AH=e0 not yet supported!\n");
      bx_set_CF(1);
      AH = 0x80;
      break;

    default:
      bx_panic("BIOS: *** int 15h function AH=%02x not yet supported!\n",
        (unsigned) AH);
      bx_set_CF(1);
      AH = 0x80;
      break;
    }
}


  static void
 bx_bios_int17_handler(int vector)
{
#ifdef BX_DEBUG
  if (bx_dbg.bios)
    bx_printf("BIOS: *** INT 17h called with AH=%02x, DX=%04x\n",
      (int) AH, (int) DX);
#endif

  switch (AH) {
    case 0x01: /* intialize printer */
#if 0
      if (DX > 3) {
        bx_panic("BIOS: INT 17h funct 1, called with bogus port #\n");
        }
      AH = 0; /* bogus value */
#endif
      break;

    case 0x02: /* read printer status */
#if 0
      if (DX > 3)
        bx_panic("BIOS: INT 17h funct 2, called with bogus port #\n");
      AH = 0; /* bogus value */
#endif
      break;

    default:
      bx_panic("BIOS: *** int 17h function AH=%02x not yet supported!\n",
        (int) AH);
    }
}
#endif /*BX_EXTERNAL_ENVIRONMENT==0*/

  static Bit32u
bx_port92_io_read_handler(Bit32u address, unsigned io_len)
{
  bx_printf("BIOS: port92h read partially supported!!!\n");
  bx_printf("BIOS:   returning %02x\n", (unsigned) (bx_get_enable_a20() << 1));
  return(bx_get_enable_a20() << 1);
}

  static void
bx_port92_io_write_handler(Bit32u address, Bit32u value, unsigned io_len)
{
  Boolean bx_cpu_reset;

  bx_printf("BIOS: port92h write of %02x partially supported!!!\n",
    (unsigned) value);
bx_printf("BIOS: A20: set_enable_a20() called\n");
  bx_set_enable_a20( (value & 0x02) >> 1 );
  bx_printf("A20: now %u\n", (unsigned) bx_get_enable_a20());
  bx_cpu_reset  = (value & 0x01); /* high speed reset */
  if (bx_cpu_reset) {
    bx_panic("PORT 92h write: CPU reset requested!\n");
    }
}

#if 1 /* bill */

void bx_coldboot(void)
{
  Bit8u  default_int_code[4] = {0x0F, 0x1F, 0x00, 0xCF};
  int memory_in_k = 640-1;

/* reset important interrupt vectors */

  bx_isr_code_cf[2] = 0x13;
  bx_register_int_vector(0x13, bx_isr_code_cf, sizeof(bx_isr_code_cf),
      bx_floppy_int13h_handler
#ifdef WIN32DEBUG
      ,H_FLOPPYINT13
#endif
      );

  default_int_code[2]=0x21;
  bx_register_int_vector(0x21, default_int_code, sizeof(default_int_code),
      bx_bios_default_int_handler
#ifdef WIN32DEBUG
      ,H_BIOS_DEFAULT
#endif      
      );

  default_int_code[2]=0x2F;
  bx_register_int_vector(0x2F, default_int_code, sizeof(default_int_code),
      bx_bios_default_int_handler
#ifdef WIN32DEBUG
      ,H_BIOS_DEFAULT
#endif      
      );

  bx_access_physical(0x413,2,BX_WRITE,&memory_in_k);

/*  bx_register_int_vector((Bit8u) 0x19,   
     bootstrap_int_code, sizeof(bootstrap_int_code),
      bx_bios_bootstrap_loader); */

  bx_interrupt(0x19,1,0); /* if a virus captures int 19 we're screwed */
}

#endif
