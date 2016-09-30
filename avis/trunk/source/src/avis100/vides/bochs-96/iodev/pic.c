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



#include <stdio.h>

#include "iodev.h"
#include "..\..\bochsw\watcher\watcher.h"

static void service_master_pic(void);
static void service_slave_pic(void);


typedef struct {
  Bit8u single_PIC;        /* 0=cascaded PIC, 1=master only */
  Bit8u interrupt_offset;  /* programmable interrupt vector offset */
  union {
    Bit8u   slave_connect_mask; /* for master, a bit for each interrupt line
                                   0=not connect to a slave, 1=connected */
    Bit8u   slave_id;           /* for slave, id number of slave PIC */
    } u;
  Bit8u sfnm;              /* specially fully nested mode: 0=no, 1=yes*/
  Bit8u buffered_mode;     /* 0=no buffered mode, 1=buffered mode */
  Bit8u master_slave;      /* master/slave: 0=slave PIC, 1=master PIC */
  Bit8u auto_eoi;          /* 0=manual EOI, 1=automatic EOI */
  Bit8u imr;               /* interrupt mask register, 1=masked */
  Bit8u isr;               /* in service register */
  Bit8u irr;               /* interrupt request register */
  Bit8u read_reg_select;   /* 0=IRR, 1=ISR */
  Bit8u irq;               /* current IRQ number */
  Boolean INT;             /* INT request pin of PIC */
  struct {
    Boolean    in_init;
    Boolean    requires_4;
    int        byte_expected;
    } init;
  Boolean special_mask;
  } bx_pic_t;

static bx_pic_t master_pic, slave_pic;





  void
bx_init_pic_hardware(void)
{
  bx_iodev_t  io_device;

  /* 8259 PIC (Programmable Interrupt Controller) */
  io_device.read_funct   = bx_pic_io_read_handler;
  io_device.write_funct  = bx_pic_io_write_handler;
  io_device.handler_name = "8259 PIC";
  io_device.start_addr   = 0x0020;
  io_device.end_addr     = 0x0021;
  io_device.irq          = BX_NO_IRQ;
  bx_register_io_handler(io_device);

  io_device.start_addr = 0x00A0;
  io_device.end_addr   = 0x00A1;
  bx_register_io_handler(io_device);


  master_pic.single_PIC = 0;
  master_pic.interrupt_offset = 0x08; /* IRQ0 = INT 0x08 */
  /* slave PIC connected to IRQ2 of master */
  master_pic.u.slave_connect_mask = 0x04;
  master_pic.sfnm = 0; /* normal nested mode */
  master_pic.buffered_mode = 0; /* unbuffered mode */
  master_pic.master_slave  = 0; /* no meaning, buffered_mode=0 */
  master_pic.auto_eoi      = 0; /* manual EOI from CPU */
  master_pic.imr           = 0xFF; /* all IRQ's initially masked */
  master_pic.isr           = 0x00; /* no IRQ's in service */
  master_pic.irr           = 0x00; /* no IRQ's requested */
  master_pic.read_reg_select = 0; /* IRR */
  master_pic.irq = 0;
  master_pic.INT = 0;
  master_pic.init.in_init = 0;
  master_pic.init.requires_4 = 0;
  master_pic.init.byte_expected = 0;
  master_pic.special_mask = 0;

  slave_pic.single_PIC = 0;
  slave_pic.interrupt_offset = 0x70; /* IRQ8 = INT 0x70 */
  slave_pic.u.slave_id = 0x02; /* slave PIC connected to IRQ2 of master */
  slave_pic.sfnm       = 0; /* normal nested mode */
  slave_pic.buffered_mode = 0; /* unbuffered mode */
  slave_pic.master_slave  = 0; /* no meaning, buffered_mode=0 */
  slave_pic.auto_eoi      = 0; /* manual EOI from CPU */
  slave_pic.imr           = 0xFF; /* all IRQ's initially masked */
  slave_pic.isr           = 0x00; /* no IRQ's in service */
  slave_pic.irr           = 0x00; /* no IRQ's requested */
  slave_pic.read_reg_select = 0; /* IRR */
  slave_pic.irq = 0;
  slave_pic.INT = 0;
  slave_pic.init.in_init = 0;
  slave_pic.init.requires_4 = 0;
  slave_pic.init.byte_expected = 0;
  slave_pic.special_mask = 0;
}


#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_init_pic_bios(void)
{
}
#endif



  Bit32u
bx_pic_io_read_handler(Bit32u address, unsigned io_len)
{

  if (bx_dbg.pic)
    bx_printf("pic: IO read from %04x\n", (unsigned) address);

  /*
   8259A PIC
   */

  switch (address) {
    case 0x20:
      if (master_pic.read_reg_select) { /* ISR */
        if (bx_dbg.pic) bx_printf("pic: read master ISR = %02x\n",
	                  (unsigned) master_pic.isr);
	return(master_pic.isr);
	}
      else { /* IRR */
        if (bx_dbg.pic) bx_printf("pic: read master IRR = %02x\n",
	                  (unsigned) master_pic.irr);
bx_printf("pic: read master IRR = %02x\n",
	                  (unsigned) master_pic.irr);
	return(master_pic.irr);
	}
      break;
    case 0x21:
      if (bx_dbg.pic) bx_printf("pic: read master IMR = %02x\n",
                        (unsigned) master_pic.imr);
      return(master_pic.imr);
      break;
    case 0xA0:
      if (slave_pic.read_reg_select) { /* ISR */
        if (bx_dbg.pic) bx_printf("pic: read slave ISR = %02x\n",
                          (unsigned) slave_pic.isr);
	return(slave_pic.isr);
	}
      else { /* IRR */
        if (bx_dbg.pic) bx_printf("pic: read slave IRR = %02x\n",
                          (unsigned) slave_pic.irr);
	return(slave_pic.irr);
	}
      break;
    case 0xA1:
      if (bx_dbg.pic) bx_printf("pic: read slave IMR = %02x\n",
                        (unsigned) slave_pic.imr);
      return(slave_pic.imr);
      break;
    }
#if 0 /* bill */
  bx_panic("pic: io read to address %04x\n", (unsigned) address);
#else
  bx_printf("pic: io read to address %04x\n", (unsigned) address);
#endif
  return(0); /* default if not found above */
}

  void
bx_pic_io_write_handler(Bit32u address, Bit32u value, unsigned io_len)
{
  int irq;

  if (bx_dbg.pic)
    bx_printf("pic: IO write to %04x = %02x\n",
      (unsigned) address, (unsigned) value);

  /*
   8259A PIC
   */

  switch (address) {
    case 0x20:
      if (value & 0x10) { /* initialization command 1 */
bx_printf("pic:master: init command 1 found %02x\n", (unsigned) value);
        if (bx_dbg.pic) {
          bx_printf("pic:master: init command 1 found\n");
          bx_printf("            requires 4 = %u\n",
            (unsigned) (value & 0x01) );
          bx_printf("            cascade mode: [0=cascade,1=single] %u\n",
            (unsigned) ((value & 0x02) >> 1));
          }
        master_pic.init.in_init = 1;
        master_pic.init.requires_4 = (value & 0x01);
        master_pic.init.byte_expected = 2; /* operation command 2 */
        master_pic.imr           = 0xFF; /* all IRQ's initially masked */
        master_pic.isr           = 0x00; /* no IRQ's in service */
        master_pic.irr           = 0x00; /* no IRQ's requested */
        master_pic.INT = 0; /* reprogramming clears previous INTR request */
        if ( (value & 0x02) == 1 )
          bx_panic("pic:master: init command: single mode\n");
        bx_set_INTR(0);
        return;
        }

      if ( (value & 0x18) == 0x08 ) { /* OCW3 */
        Bit8u special_mask, poll, read_op;

        special_mask = (value & 0x60) >> 5;
        poll         = (value & 0x04) >> 2;
        read_op      = (value & 0x03);
        if (poll)
          bx_panic("pic:master:OCW3: poll bit set\n");
        if (read_op == 0x02) /* read IRR */
	  master_pic.read_reg_select = 0;
        else if (read_op == 0x03) /* read ISR */
	  master_pic.read_reg_select = 1;
        if (special_mask == 0x02) { /* cancel special mask */
          master_pic.special_mask = 0;
          }
        else if (special_mask == 0x03) { /* set specific mask */
          master_pic.special_mask = 1;
          service_master_pic();
          }
        return;
        }

      /* OCW2 */
      switch (value) {
	case 0x0A: /* select read interrupt request register */
	  master_pic.read_reg_select = 0;
	  break;
	case 0x0B: /* select read interrupt in-service register */
	  master_pic.read_reg_select = 1;
	  break;

	case 0x20: /* end of interrupt command */
          /* clear highest current in service bit */
          for (irq=0; irq<=7; irq++) {
            if (master_pic.isr & (1 << irq)) {
              master_pic.isr &= ~(1 << irq);
              break; /* out of for loop */
              }
            }
          service_master_pic();
	  break;

        case 0x60: /* specific EOI 0 */
        case 0x61: /* specific EOI 1 */
        case 0x62: /* specific EOI 2 */
        case 0x63: /* specific EOI 3 */
        case 0x64: /* specific EOI 4 */
        case 0x65: /* specific EOI 5 */
        case 0x66: /* specific EOI 6 */
        case 0x67: /* specific EOI 7 */
          master_pic.isr &= ~(1 << (value-0x60));
          service_master_pic();
	  break;

        default:
#if 0 /* bill */
          bx_panic("PIC: write to port 20h = %02x\n", value);
#else
          bx_printf("PIC: write to port 20h = %02x\n", value);
#endif
	} /* switch (value) */          
      break;

    case 0x21:
      /* initialization mode operation */
      if (master_pic.init.in_init) {
        switch (master_pic.init.byte_expected) {
          case 2:
            master_pic.interrupt_offset = value & 0xf8;
            master_pic.init.byte_expected = 3;
            bx_printf("pic:master: init command 2 = %02x\n", (unsigned) value);
            bx_printf("            offset = INT %02x\n",
              master_pic.interrupt_offset);
            return;
            break;
          case 3:
            bx_printf("pic:master: init command 3 = %02x\n", (unsigned) value);
            if (master_pic.init.requires_4) {
              master_pic.init.byte_expected = 4;
	      }
            else {
              master_pic.init.in_init = 0;
	      }
            return;
            break;
          case 4:
            bx_printf("pic:master: init command 4 = %02x\n", (unsigned) value);
            if (value & 0x02) bx_printf("pic:        auto EOI\n");
            else bx_printf("pic: normal EOI interrupt\n");
            if (value & 0x01) bx_printf("pic:        80x86 mode\n");
            else bx_panic("pic:        not 80x86 mode\n");
            master_pic.init.in_init = 0;
            return;
            break;
          default:
#if 0 /* bill */
            bx_panic("pic:master expecting bad init command\n");
#else
            bx_printf("pic:master expecting bad init command\n");
            break;
#endif
          }
        }

      /* normal operation */
      if (bx_dbg.pic)
        bx_printf("pic: setting master pic IMR to %02x\n", value);
      master_pic.imr = value;
      service_master_pic();
      return;
      break;

    case 0xA0:
      if (value & 0x10) { /* initialization command 1 */
        if (bx_dbg.pic) {
          bx_printf("pic:slave: init command 1 found\n");
          bx_printf("           requires 4 = %u\n",
            (unsigned) (value & 0x01) );
          bx_printf("           cascade mode: [0=cascade,1=single] %u\n",
            (unsigned) ((value & 0x02) >> 1));
          }
        slave_pic.init.in_init = 1;
        slave_pic.init.requires_4 = (value & 0x01);
        slave_pic.init.byte_expected = 2; /* operation command 2 */
        slave_pic.imr           = 0xFF; /* all IRQ's initially masked */
        slave_pic.isr           = 0x00; /* no IRQ's in service */
        slave_pic.irr           = 0x00; /* no IRQ's requested */
        slave_pic.INT = 0; /* reprogramming clears previous INTR request */
        if ( (value & 0x02) == 1 )
          bx_panic("pic:slave: init command: single mode\n");
        return;
        }

      if ( (value & 0x18) == 0x08 ) { /* OCW3 */
        Bit8u special_mask, poll, read_op;

        special_mask = (value & 0x60) >> 5;
        poll         = (value & 0x04) >> 2;
        read_op      = (value & 0x03);
        if (poll)
          bx_panic("pic:slave:OCW3: poll bit set\n");
        if (read_op == 0x02) /* read IRR */
	  slave_pic.read_reg_select = 0;
        else if (read_op == 0x03) /* read ISR */
	  slave_pic.read_reg_select = 1;
        if (special_mask == 0x02) { /* cancel special mask */
          slave_pic.special_mask = 0;
          }
        else if (special_mask == 0x03) { /* set specific mask */
          slave_pic.special_mask = 1;
          service_slave_pic();
          bx_printf("pic:slave: OCW3 not implemented (%02x)\n",
            (unsigned) value);
          }
        return;
        }

      switch (value) {
	case 0x0A: /* select read interrupt request register */
	  slave_pic.read_reg_select = 0;
	  break;
	case 0x0B: /* select read interrupt in-service register */
	  slave_pic.read_reg_select = 1;
	  break;
	case 0x20: /* end of interrupt command */
          /* clear highest current in service bit */
          for (irq=0; irq<=7; irq++) {
            if (slave_pic.isr & (1 << irq)) {
              slave_pic.isr &= ~(1 << irq);
              break; /* out of for loop */
              }
            }
          service_slave_pic();
	  break;

        case 0x60: /* specific EOI 0 */
        case 0x61: /* specific EOI 1 */
        case 0x62: /* specific EOI 2 */
        case 0x63: /* specific EOI 3 */
        case 0x64: /* specific EOI 4 */
        case 0x65: /* specific EOI 5 */
        case 0x66: /* specific EOI 6 */
        case 0x67: /* specific EOI 7 */
          slave_pic.isr &= ~(1 << (value-0x60));
          service_slave_pic();
	  break;

        default:
#if 0 /* billou */
          bx_panic("PIC: write to port A0h = %02x\n", value);
#else
          bx_printf("PIC: write to port A0h = %02x\n", value);
#endif
	} /* switch (value) */          
      break;

    case 0xA1:
      /* initialization mode operation */
      if (slave_pic.init.in_init) {
        switch (slave_pic.init.byte_expected) {
          case 2:
            slave_pic.interrupt_offset = value & 0xf8;
            slave_pic.init.byte_expected = 3;
            bx_printf("pic:slave: init command 2 = %02x\n", (unsigned) value);
            bx_printf("           offset = INT %02x\n",
              slave_pic.interrupt_offset);
            return;
            break;
          case 3:
            bx_printf("pic:slave: init command 3 = %02x\n", (unsigned) value);
            if (slave_pic.init.requires_4) {
              slave_pic.init.byte_expected = 4;
	      }
            else {
              slave_pic.init.in_init = 0;
	      }
            return;
            break;
          case 4:
            bx_printf("pic:slave: init command 4 = %02x\n", (unsigned) value);
            if (value & 0x02) bx_printf("pic:       auto EOI\n");
            else bx_printf("pic: normal EOI interrupt\n");
            if (value & 0x01) bx_printf("pic:       80x86 mode\n");
            else bx_panic("pic: not 80x86 mode\n");
            slave_pic.init.in_init = 0;
            return;
            break;
          default:
#if 0 /* bill */
            bx_panic("pic:slave: expecting bad init command\n");
#else
            bx_printf("pic:slave: expecting bad init command\n");
#endif
          }
        }

      /* normal operation */
      if (bx_dbg.pic)
        bx_printf("pic: setting slave pic IMR to %02x\n", value);
      slave_pic.imr = value;
      service_slave_pic();
      return;
      break;
    } /* switch (address) */

  return;
}

  void
bx_trigger_irq(unsigned irq_no)
{
  int irq_no_bitmask;

#if BX_DEBUG
  if ( irq_no > 15 )
    bx_panic("trigger_irq: irq out of range\n");
#endif

  if (bx_dbg.pic)
    bx_printf("trigger_irq(%d decimal)\n", (unsigned) irq_no);

  if (irq_no <= 7) {
    irq_no_bitmask = 1 << irq_no;
    master_pic.irr |= irq_no_bitmask;
    service_master_pic();
    }
  else { /* irq = 8..15 */
    irq_no_bitmask = 1 << (irq_no - 8);
    slave_pic.irr |= irq_no_bitmask;
    service_slave_pic();
    }
}

  /* */
  static void
service_master_pic(void)
{
  Bit8u unmasked_requests;
  int irq;
  Bit8u isr, max_irq;

  if (master_pic.INT) { /* last interrupt still not acknowleged */
    return;
    }

  if (master_pic.special_mask) {
    /* all priorities may be enabled.  check all IRR bits except ones
     * which have corresponding ISR bits set
     */
    max_irq = 7;
    }
  else { /* normal mode */
    /* Find the highest priority IRQ that is enabled due to current ISR */
    isr = master_pic.isr;
    if (isr) {
      max_irq = 0;
      while ( (isr & 0x01) == 0 ) {
        isr >>= 1;
        max_irq++;
        }
      if (max_irq == 0 ) return; /* IRQ0 in-service, no other priorities allowed */
      if (max_irq > 7) bx_panic("error in service_master_pic()\n");
      }
    else
      max_irq = 7; /* 0..7 bits in ISR are cleared */
    }


  /* now, see if there are any higher priority requests */
  if ((unmasked_requests = (master_pic.irr & ~master_pic.imr)) ) {
    for (irq=0; irq<=max_irq; irq++) {
      /* for special mode, since we're looking at all IRQ's, skip if
       * current IRQ is already in-service
       */
      if ( master_pic.special_mask && ((master_pic.isr >> irq) & 0x01) )
        continue;
      if (unmasked_requests & (1 << irq)) {
        if (bx_dbg.pic)
          bx_printf("pic: signalling IRQ(%u)\n",
            (unsigned) irq);
        master_pic.irr &= ~(1 << irq);
        /*??? do for slave too: master_pic.isr |=  (1 << irq);*/
        master_pic.INT = 1;
        bx_set_INTR(1);
        master_pic.irq = irq;
        return;
        } /* if (unmasked_requests & ... */
      } /* for (irq=7 ... */
    } /* if (unmasked_requests = ... */
}


  static void
service_slave_pic(void)
{
  Bit8u unmasked_requests;
  int irq;
  Bit8u isr, lowest_priority_irq;

  if (slave_pic.INT) { /* last interrupt still not acknowleged */
    return;
    }

  /* Find the highest priority IRQ that is enabled due to current ISR */
  isr = slave_pic.isr;
  if (isr) {
    lowest_priority_irq = 0;
    while ( !(isr & 0x01) ) {
      isr >>= 1;
      lowest_priority_irq++;
      }
    if (lowest_priority_irq > 7) bx_panic("error in service_slave_pic()\n");
    }
  else
    lowest_priority_irq = 8;


  /* now, see if there are any higher priority requests */
  if ((unmasked_requests = (slave_pic.irr & ~slave_pic.imr)) ) {
    for (irq=0; irq<lowest_priority_irq; irq++) {
      if (unmasked_requests & (1 << irq)) {
        if (bx_dbg.pic)
          bx_printf("pic(slave): signalling IRQ(%u)\n",
            (unsigned) 8 + irq);
        slave_pic.irr &= ~(1 << irq);
        slave_pic.INT = 1;
        master_pic.irr |= 0x04; /* request IRQ 2 on master pic */
        slave_pic.irq = irq;
        service_master_pic();
        return;
        } /* if (unmasked_requests & ... */
      } /* for (irq=7 ... */
    } /* if (unmasked_requests = ... */
}


  /* CPU handshakes with PIC after acknowledging interrupt */
  Bit8u
bx_IAC(void)
{
  Bit8u vector;

  bx_set_INTR(0);
  master_pic.INT = 0;
  master_pic.isr |= (1 << master_pic.irq);
  master_pic.irr &= ~(1 << master_pic.irq);

  if (master_pic.irq != 2) {

/* Hooman: watching hardware interrupts */
      if(irq_watch[master_pic.irq])
         report_irq(master_pic.irq);
/* end Hooman */

      vector = master_pic.irq + master_pic.interrupt_offset;
    }
  else { /* IRQ2 = slave pic IRQ8..15 */

/* Hooman: watching hardware interrupts */
      if(irq_watch[slave_pic.irq])
         report_irq(slave_pic.irq);
/* end Hooman */

    slave_pic.INT = 0;
    vector = slave_pic.irq + slave_pic.interrupt_offset;
    slave_pic.isr |= (1 << slave_pic.irq);
    slave_pic.irr &= ~(1 << slave_pic.irq);
    service_slave_pic();
    }

  service_master_pic();

  return(vector);
}

  void
show_pic_state(void)
{
bx_printf("master_pic.imr = %02x\n", master_pic.imr);
bx_printf("master_pic.isr = %02x\n", master_pic.isr);
bx_printf("master_pic.irr = %02x\n", master_pic.irr);
bx_printf("master_pic.irq = %02x\n", master_pic.irq);
}
