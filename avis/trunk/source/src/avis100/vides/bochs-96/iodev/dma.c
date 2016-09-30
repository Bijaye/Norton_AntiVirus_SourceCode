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



#include "iodev.h"



dma_controller_t dma1;



  void
bx_init_dma_hardware(void)
{
  bx_iodev_t  io_device;
  unsigned c;

  /* 8237 DMA controller */
  io_device.read_funct   = bx_dma_io_read_handler;
  io_device.write_funct  = bx_dma_io_write_handler;
  io_device.handler_name = "DMA controller";
  io_device.start_addr   = 0x0000;
  io_device.end_addr     = 0x000F;
  io_device.irq          = BX_NO_IRQ;
  bx_register_io_handler(io_device);

  io_device.start_addr   = 0x0081;
  io_device.end_addr     = 0x008D;
  bx_register_io_handler(io_device);

  io_device.start_addr   = 0x008F;
  io_device.end_addr     = 0x008F;
  bx_register_io_handler(io_device);

  io_device.start_addr   = 0x00C0;
  io_device.end_addr     = 0x00DE;
  bx_register_io_handler(io_device);

  dma1.mask = 0x0f; /* all 4 channels masked */
  dma1.flip_flop = 0; /* cleared */
  for (c=0; c<4; c++) {
    dma1.chan[c].mode = 0;
    dma1.chan[c].base_address = 0;
    dma1.chan[c].current_address = 0;
    dma1.chan[c].base_count = 0;
    dma1.chan[c].current_count = 0;
    dma1.chan[c].page_reg = 0;
    }
}


#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_init_dma_bios(void)
{
}
#endif


  /* 8237 DMA controller */
  Bit32u
bx_dma_io_read_handler(Bit32u address, unsigned io_len)
{
  if (bx_dbg.temp)
    bx_printf("dma_io_read addr=%04x\n", (unsigned) address);

#if BX_DMA_FLOPPY_IO < 1
  /* if we're not supporting DMA/floppy IO just return a bogus value */
  return(0xff);
#endif

  switch (address) {
    case 0x00: /* DMA-1 current address, channel 0 */
      if (dma1.flip_flop==0) {
        dma1.flip_flop = !dma1.flip_flop;
        return(dma1.chan[0].current_address & 0xff);
        }
      else {
        dma1.flip_flop = !dma1.flip_flop;
        return(dma1.chan[0].current_address >> 8);
        }
    case 0x01: /* DMA-1 current count, channel 0 */
      if (dma1.flip_flop==0) {
        dma1.flip_flop = !dma1.flip_flop;
        return(dma1.chan[0].current_count & 0xff);
        }
      else {
        dma1.flip_flop = !dma1.flip_flop;
        return(dma1.chan[0].current_count >> 8);
        }

    case 0x02: /* DMA-1 current address, channel 1 */
      if (dma1.flip_flop==0) {
        dma1.flip_flop = !dma1.flip_flop;
        return(dma1.chan[1].current_address & 0xff);
        }
      else {
        dma1.flip_flop = !dma1.flip_flop;
        return(dma1.chan[1].current_address >> 8);
        }
    case 0x03: /* DMA-1 current count, channel 1 */
      if (dma1.flip_flop==0) {
        dma1.flip_flop = !dma1.flip_flop;
        return(dma1.chan[1].current_count & 0xff);
        }
      else {
        dma1.flip_flop = !dma1.flip_flop;
        return(dma1.chan[1].current_count >> 8);
        }

    case 0x04: /* DMA-1 current address, channel 2 */
      if (dma1.flip_flop==0) {
        dma1.flip_flop = !dma1.flip_flop;
        return(dma1.chan[2].current_address & 0xff);
        }
      else {
        dma1.flip_flop = !dma1.flip_flop;
        return(dma1.chan[2].current_address >> 8);
        }
    case 0x05: /* DMA-1 current count, channel 2 */
      if (dma1.flip_flop==0) {
        dma1.flip_flop = !dma1.flip_flop;
        return(dma1.chan[2].current_count & 0xff);
        }
      else {
        dma1.flip_flop = !dma1.flip_flop;
        return(dma1.chan[2].current_count >> 8);
        }

    case 0x06: /* DMA-1 current address, channel 3 */
      if (dma1.flip_flop==0) {
        dma1.flip_flop = !dma1.flip_flop;
        return(dma1.chan[3].current_address & 0xff);
        }
      else {
        dma1.flip_flop = !dma1.flip_flop;
        return(dma1.chan[3].current_address >> 8);
        }
    case 0x07: /* DMA-1 current count, channel 3 */
      if (dma1.flip_flop==0) {
        dma1.flip_flop = !dma1.flip_flop;
        return(dma1.chan[3].current_count & 0xff);
        }
      else {
        dma1.flip_flop = !dma1.flip_flop;
        return(dma1.chan[3].current_count >> 8);
        }

    default:
#if 0 /* bill */
      bx_panic("dma_io_read: unsupported address=%04x\n", (unsigned) address);
#else
      bx_printf("dma_io_read: unsupported address=%04x\n", (unsigned) address);
#endif
      return(0);
    }

}

  /* 8237 DMA controller */
  void
bx_dma_io_write_handler(Bit32u address, Bit32u value, unsigned len)
{
  Bit8u set_mask_bit;
  Bit8u channel;

  if (bx_dbg.temp)
    bx_printf("\ndma_io_write: address=%04x value=%02x\n",
      (unsigned) address, (unsigned) value);

#if BX_DMA_FLOPPY_IO < 1
  /* if we're not supporting DMA/floppy IO just return */
  return;
#endif

  switch (address) {
    case 0x00:
      if (bx_dbg.temp)
        bx_printf("  DMA-1 base and current address, channel 0\n");
      return;
      break;
    case 0x01:
      if (bx_dbg.temp)
        bx_printf("  DMA-1 base and current count, channel 0\n");
      return;
      break;

    case 0x02:
      if (bx_dbg.temp)
        bx_printf("  DMA-1 base and current address, channel 1\n");
      return;
      break;
    case 0x03:
      if (bx_dbg.temp)
        bx_printf("  DMA-1 base and current count, channel 1\n");
      return;
      break;

    case 0x04:
      if (bx_dbg.temp)
        bx_printf("  DMA-1 base and current address, channel 2\n");
      if (dma1.flip_flop==0) { /* 1st byte */
        dma1.chan[2].base_address = value;
        dma1.chan[2].current_address = value;
        }
      else { /* 2nd byte */
        dma1.chan[2].base_address |= (value << 8);
        dma1.chan[2].current_address |= (value << 8);
        if (bx_dbg.temp) {
          bx_printf("    base = %04x\n",
            (unsigned) dma1.chan[2].base_address);
          bx_printf("    curr = %04x\n",
            (unsigned) dma1.chan[2].current_address);
          }
        }
      dma1.flip_flop = !dma1.flip_flop;
      return;
      break;

    case 0x05:
      if (bx_dbg.temp)
        bx_printf("  DMA-1 base and current count, channel 2\n");
      if (dma1.flip_flop==0) { /* 1st byte */
        dma1.chan[2].base_count = value;
        dma1.chan[2].current_count = value;
        }
      else { /* 2nd byte */
        dma1.chan[2].base_count |= (value << 8);
        dma1.chan[2].current_count |= (value << 8);
        if (bx_dbg.temp) {
          bx_printf("    base = %04x\n",
            (unsigned) dma1.chan[2].base_count);
          bx_printf("    curr = %04x\n",
            (unsigned) dma1.chan[2].current_count);
          }
        }
      dma1.flip_flop = !dma1.flip_flop;
      return;
      break;

    case 0x06:
      if (bx_dbg.temp)
        bx_printf("  DMA-1 base and current address, channel 3\n");
      return;
      break;
    case 0x07:
      if (bx_dbg.temp)
        bx_printf("  DMA-1 base and current count, channel 3\n");
      return;
      break;

    case 0x08: /* DMA-1: command register */
      /* ??? */
      if (value != 0x04)
        bx_printf("DMA: write to 0008: value(%02xh) not 04h\n",
          (unsigned) value);
      return;
      break;

    case 0x0a:
      set_mask_bit = value & 0x40;
      channel = value & 0x03;
      if (set_mask_bit)
        dma1.mask |= (1 << channel);
      else
        dma1.mask &= ~(1 << channel);
      if (bx_dbg.temp)
        bx_printf("DMA1: set_mask_bit=%u, channel=%u, mask now=%02xh\n",
          (unsigned) set_mask_bit, (unsigned) channel, (unsigned) dma1.mask);
      return;
      break;

    case 0x0b: /* dma-1 mode register */
      channel = value & 0x03;
      dma1.chan[channel].mode = value >> 2;
      if (bx_dbg.temp)
        bx_printf("DMA1: mode register[%u] = %02x\n",
          (unsigned) channel, (unsigned) dma1.chan[channel].mode);
      return;
      break;

    case 0x0c: /* dma-1 clear byte flip/flop */
      if (bx_dbg.temp)
        bx_printf("DMA1: clear flip/flop\n");
      dma1.flip_flop = 0;
      return;
      break;

    case 0x0d: /* dma-1: master disable */
      /* ??? */
      return;
      break;

    case 0x81: /* dma page register, channel 2 */
      /* address bits A16-A23 for DMA channel 2 */
      dma1.chan[2].page_reg = value;
      if (bx_dbg.temp)
        bx_printf("DMA1: page register 2 = %02x\n", (unsigned) value);
      return;
      break;

    case 0xd0: /* DMA-2 command register */
      if (value != 0x04)
        bx_printf("DMA2: write command register: value(%02xh)!=04h\n",
          (unsigned) value);
      return;
      break;

    case 0xda: /* DMA-2: master disable */
      /* ??? */
      return;
      break;

    default:
#if 0 /* bill */
      bx_panic("DMA: write: %04xh = %02xh\n",
        (unsigned) address, (unsigned) value);
#else
      bx_printf("DMA: write: %04xh = %02xh\n", 
        (unsigned) address, (unsigned) value);
      return;
#endif
    }
}
