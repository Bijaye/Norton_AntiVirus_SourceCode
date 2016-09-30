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


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef WIN32
   #include <unistd.h>
#endif
#include <ctype.h>

#include "iodev.h"


// If we use memory-mapped files, we need windows.h for things like HANDLE...
#if defined(WIN32) && defined(MEMMAPDISKIMAGES)
#include <windows.h>
#endif

#define BX_DEBUG_HD 1


void record_disk(Bit16u head, Bit16u sector, Bit16u cylinder); /* ??? */
Bit16u minhead=1000, maxhead=0,
       minsector=1000, maxsector=0,
       mincylinder=1000, maxcylinder=0;




extern bx_options_t bx_options;

typedef struct {
// With memory-mapped files, keep the HANDLE to the file, the HANDLE to the mapping and the mem pointer to the mapping
#if defined(WIN32) && defined(MEMMAPDISKIMAGES)
  HANDLE fd;
  HANDLE map;
  PBYTE  img;
#else	
  int      fd;
#endif
  unsigned cylinders;
  unsigned heads;
  unsigned sectors;
  } hard_drive_t;

#if 1 /* bill */
hard_drive_t hard_drive;
#endif

typedef struct {
  struct {
    Boolean busy;
    Boolean drive_ready;
    Boolean write_fault;
    Boolean seek_complete;
    Boolean drq;
    Boolean corrected_data;
    Boolean index_pulse;
    Boolean prev_command_error;
    } status;
  Bit8u    error_register;
  Boolean  diag_mode;
  Bit8u    diag_byte;
  Bit8u    drive_select;
  Bit8u    head_no;
  Bit8u    sector_count;
  Bit8u    sector_no;
  Bit16u   cylinder_no;
  Bit8u    buffer[512];
  unsigned buffer_index;
  Bit8u    current_command;
  } controller_t;

/*static hard_drive_t hard_drive;*/
static controller_t controller;


#if 0
static Bit8u param_table[16];
#endif

#if BX_EXTERNAL_ENVIRONMENT==0
static void set_disk_ret_status(Bit8u val);

static Bit8u disk_buffer[64 * 1024];
#endif




  void
bx_init_hard_drive_hardware(void)
{
#if defined(WIN32) && defined(MEMMAPDISKIMAGES)
  DWORD diskimgsz;
#else
  struct stat stat_buf;
#endif
  int ret;

  bx_iodev_t  io_device;

  /* HARD DRIVE 0 */
  io_device.read_funct   = bx_hard_drive_io_read_handler;
  io_device.write_funct  = bx_hard_drive_io_write_handler;
  io_device.handler_name = "Hard Drive 0";
  io_device.start_addr   = 0x01F0;
  io_device.end_addr     = 0x01F7;
  io_device.irq          = 14;
  bx_register_io_handler(io_device);


  // (Initially this line was just after the file opening, I moved it before for clarity, Fred)
  bx_cmos_reg[0x12] = 0x00; /* start out with: no drive 0, no drive 1 */

  
  /* open hard drive image file */
#if defined(WIN32) && defined(MEMMAPDISKIMAGES)

  // If we use memory-mapped images, open the file, create the mapping and create a view of the whole file
  // This assumes that the disk images do not grow too big, but this is very unlikely to be a problem
  hard_drive.fd = CreateFile(bx_options.diskc.path, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL, NULL);
  if (hard_drive.fd == INVALID_HANDLE_VALUE)
    bx_panic("could not open hard drive image file '%s'!\n", bx_options.diskc.path);
  hard_drive.map = CreateFileMapping(hard_drive.fd, NULL, PAGE_READWRITE, 0, 0 ,NULL);
  if (hard_drive.map == NULL) {
	CloseHandle(hard_drive.fd);
    bx_panic("could not open hard drive image file '%s'!\n", bx_options.diskc.path);
  }
  hard_drive.img = MapViewOfFile(hard_drive.map, FILE_MAP_WRITE, 0, 0, 0);
  if (hard_drive.img == NULL) {
	CloseHandle(hard_drive.map);
	CloseHandle(hard_drive.fd);
    bx_panic("could not open hard drive image file '%s'!\n", bx_options.diskc.path);
  }

  // Then use GetFileSize rather than fstat to get the size of the disk image
  diskimgsz = GetFileSize(hard_drive.fd, NULL);

  switch (diskimgsz) { //...see below for the rest of the switch

#else

 #ifdef WIN32
  // Using plain file access with win32
  hard_drive.fd = open(bx_options.diskc.path, O_RDWR | O_BINARY);
 #else
  hard_drive.fd = open(bx_options.diskc.path, O_RDWR);
 #endif
  if (hard_drive.fd < 0) {
    bx_panic("could not open hard drive image file '%s'!\n",
      bx_options.diskc.path);
    }

  /* look at size of image file to calculate disk geometry */
  ret = fstat(hard_drive.fd, &stat_buf);
  if (ret) {
    perror("fstat'ing hard drive image file");
    bx_panic("fstat() returns error!\n");
    }

  switch (stat_buf.st_size) {

#endif

    case (306 * 4 * 17 * 512) : /* type 1 = 10MB */
      hard_drive.cylinders = 306;
      hard_drive.heads     = 4;
      hard_drive.sectors   = 17;
      bx_cmos_reg[0x12] = (bx_cmos_reg[0x12] & 0x0f) | 0x10;
      break;

    case (615 * 4 * 17 * 512) : /* type 2 = 20MB */
      hard_drive.cylinders = 615;
      hard_drive.heads     = 4;
      hard_drive.sectors   = 17;
      bx_cmos_reg[0x12] = (bx_cmos_reg[0x12] & 0x0f) | 0x20;
      break;

    case (615 * 6 * 17 * 512) : /* type 3 = 30MB */
      hard_drive.cylinders = 615;
      hard_drive.heads     = 6;
      hard_drive.sectors   = 17;
      bx_cmos_reg[0x12] = (bx_cmos_reg[0x12] & 0x0f) | 0x30;
      break;

    default:
      bx_panic("unrecognized size of hard disk image file\n");
    }

  controller.status.busy                 = 0;
  controller.status.drive_ready          = 1;
  controller.status.write_fault          = 0;
  controller.status.seek_complete        = 1;
  controller.status.drq                  = 0;
  controller.status.corrected_data       = 0;
  controller.status.index_pulse          = 0;
  controller.status.prev_command_error   = 0;
  controller.error_register = 0;
  controller.diag_mode      = 0;
  controller.diag_byte      = 0;
  controller.drive_select   = 0;
  controller.head_no        = 0;
  controller.sector_count   = 0;
  controller.sector_no      = 0;
  controller.cylinder_no    = 0;
  controller.current_command = 0x00;
  controller.buffer_index = 0;
}


#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_init_hard_drive_bios(void)
{
  Bit8u zero8, num_drives, control_byte;

#if 0
  for XT systems, IRQ5 = INT 0x0D is called.
  for AT+ systems, IRQ14 = INT 76h is called.
    INT 76h calls INT 15h function ax=9100

  typical_int_code[2] = 0x76;
  bx_register_int_vector(0x76, typical_int_code, sizeof(typical_int_code),
      bx_hard_drv_int_handler);

  typical_int_code[2] = 0x0D;
  bx_register_int_vector(0x0D, typical_int_code, sizeof(typical_int_code),
      bx_hard_drv_int_handler);
#endif

  zero8 = 0;

  /* hard disk status of last operation */
  bx_access_physical(0x474, 1, BX_WRITE, &zero8);

  /* hard disk number attached */
  num_drives = 1;
  bx_access_physical(0x475, 1, BX_WRITE, &num_drives);

  /* hard disk control byte */
  control_byte = 0xc0;  /* disable retries on disk error */
  bx_access_physical(0x476, 1, BX_WRITE, &control_byte);

  /* hard disk port offset (XT only ???) */
  bx_access_physical(0x477, 1, BX_WRITE, &zero8);

  /* hard disk status register */
  bx_access_physical(0x48C, 1, BX_WRITE, &zero8);

  /* hard disk error register */
  bx_access_physical(0x48D, 1, BX_WRITE, &zero8);

  /* hard disk task complete flag */
  bx_access_physical(0x48E, 1, BX_WRITE, &zero8);


#if 0
  param_table[0] = hard_drive.cylinders  & 0xff;
  param_table[1] = hard_drive.cylinders  >> 8;
  param_table[2] = hard_drive.heads;
  param_table[3] = 0;
  param_table[4] = 0;
  param_table[5] = 0xff;
  param_table[6] = 0xff;
  param_table[7] = 0;
  param_table[8] = 0xc0;
  param_table[9] = 0;
  param_table[10] = 0;
  param_table[11] = 0;
  param_table[12] = hard_drive.cylinders & 0xff;
  param_table[13] = hard_drive.cylinders >> 8;
  param_table[14] = hard_drive.sectors;
  param_table[15] = 0;

  /* disk parameter table in BIOS ROM area*/
  for (i=0; i < 16; i++) {
    bx_access_physical(BX_DISK_PARAM_TBL + i,
      1, BX_WRITE, &param_table[i]);
    }

  /* int 1E points to disk parameter table */
  bx_set_interrupt_vector(0x41, BX_DISK_PARAM_TBL);
#endif

#if 0
  /* unmask IRQ 5 */
  val = bx_inp(0x21);
  bx_outp(0x21, val & ~0x20);
#endif
}
#endif



  Bit32u
bx_hard_drive_io_read_handler(Bit32u address, unsigned io_len)
{
  Bit8u value8;
  Bit16u value16;

  if (io_len==2 && address!=0x1f0) {
    bx_panic("disk: non-byte IO read to %04x\n", (unsigned) address);
    }

  if (bx_dbg.disk)
    bx_printf("disk: IO read from %04x\n", (unsigned) address);

  switch (address) {
    case 0x1f0: /* hard disk data (16bit) */
      if (io_len != 2) {
        bx_panic("disk: non-word IO read from %04x\n", (unsigned) address);
        }
      switch (controller.current_command) {
        case 0x20: /* read sectors */
          if (controller.buffer_index >= 512)
            bx_panic("disk: IO read(1f0): buffer_index >= 512\n");
          value16  = controller.buffer[controller.buffer_index];
          value16 |= (controller.buffer[controller.buffer_index+1] << 8);
          controller.buffer_index += 2;
          
          /* if buffer completely read */
          if (controller.buffer_index >= 512) {
            /* update sector count, sector number, cylinder,
             * drive, head, status
             * if there are more sectors, read next one in...
             */
            controller.buffer_index = 0;
            /* sector count of 0 means 256, it'll wrap */
            controller.sector_count--;
            controller.sector_no++;
            if (controller.sector_no > hard_drive.sectors) {
              controller.sector_no = 1;
              controller.head_no++;
              if (controller.head_no >= hard_drive.heads) {
                controller.head_no = 0;
                controller.cylinder_no++;
                if (controller.cylinder_no >= hard_drive.cylinders)
                  controller.cylinder_no = hard_drive.cylinders - 1;
                }
              }

            controller.status.busy = 0;
            controller.status.drive_ready = 1;
            controller.status.corrected_data = 0;
            controller.status.prev_command_error = 0;

            if (controller.sector_count==0) {
              controller.status.drq = 0;
              }
            else { /* read next one into controller buffer */
              unsigned long logical_sector;
              int ret;

              controller.status.drq = 1;

              logical_sector = (controller.cylinder_no * hard_drive.heads *
                                hard_drive.sectors) +
                               (controller.head_no * hard_drive.sectors) +
                               (controller.sector_no - 1);

              if (logical_sector >= (hard_drive.cylinders *
                  hard_drive.heads * hard_drive.sectors))
                bx_panic("disk: read sectors: out of bounds\n");

#if defined(WIN32) && defined(MEMMAPDISKIMAGES)
			  // Using memory-mapped images, just copy the memory

			  memcpy(controller.buffer, hard_drive.img+logical_sector*512, 512);

#else
			  // Using classic file access with lseek and read

			  ret = lseek(hard_drive.fd, logical_sector * 512, SEEK_SET);

              if (ret < 0)
                bx_panic("disk: could lseek() hard drive image file\n");
			  
			  ret = read(hard_drive.fd, controller.buffer, 512);
              if (ret < 512)
                bx_panic("disk: could not read() hard drive image file\n");
#endif
              controller.buffer_index = 0;
              bx_trigger_irq(14);
              }
            }
          return(value16);
          break;
        default:
          bx_panic("disk: IO read(1f0h): current command is %02xh\n",
            (unsigned) controller.current_command);
        }
      break;

    case 0x1f1: /* hard disk error register */
      if (controller.diag_mode) {
        value8 = controller.diag_byte;
        return(value8);
        controller.diag_mode = 0;
        }
      value8 = controller.error_register;
      return(value8);
      break;

    case 0x1f2: /* hard disk sector count */
      if (controller.current_command==0x20 ||
          controller.current_command==0x30) {
        value8 = controller.sector_count;
        return(value8);
        }
      bx_panic("disk: IO read(0x1f2): current command not read/write\n");
      break;

    case 0x1f7: /* hard disk status */
      value8 = (
        (controller.status.busy << 7) |
        (controller.status.drive_ready << 6) |
        (controller.status.write_fault << 5) |
        (controller.status.seek_complete << 4) |
        (controller.status.drq << 3) |
        (controller.status.corrected_data << 2) |
        (controller.status.index_pulse << 1) |
        (controller.status.prev_command_error) );
      return(value8);
      break;
    default:
      bx_panic("hard drive: io read to address %x unsupported\n",
        (unsigned) address);
    }

  bx_panic("hard drive: shouldnt get here!\n");
  return(0);
}

  void
bx_hard_drive_io_write_handler(Bit32u address, Bit32u value, unsigned io_len)
{
  unsigned long logical_sector;
  int ret;

  if (io_len==2 && address!=0x1f0) {
    bx_panic("disk: non-byte IO write to %04x\n", (unsigned) address);
    }

  if (bx_dbg.disk)
    bx_printf("disk: IO write to %04x = %02x\n",
      (unsigned) address, (unsigned) value);

  switch (address) {
    case 0x1f0: 
      if (io_len != 2) {
        bx_panic("disk: non-word IO read from %04x\n", (unsigned) address);
        }
      switch (controller.current_command) {
        case 0x30:
          if (controller.buffer_index >= 512)
            bx_panic("disk: IO write(1f0): buffer_index >= 512\n");
          controller.buffer[controller.buffer_index] = value;
          controller.buffer[controller.buffer_index+1] = (value >> 8);
          controller.buffer_index += 2;

          /* if buffer completely written */
          if (controller.buffer_index >= 512) {
            unsigned long logical_sector;
            int ret;

            logical_sector = (controller.cylinder_no * hard_drive.heads *
                              hard_drive.sectors) +
                             (controller.head_no * hard_drive.sectors) +
                             (controller.sector_no - 1);

            if (logical_sector >= (hard_drive.cylinders *
                hard_drive.heads * hard_drive.sectors))
              bx_panic("disk: write sectors: out of bounds\n");


#if defined(WIN32) && defined(MEMMAPDISKIMAGES)

            memcpy(hard_drive.img+logical_sector*512, controller.buffer, 512);
			
#else			
			ret = lseek(hard_drive.fd, logical_sector * 512, SEEK_SET);
            if (ret < 0)
              bx_panic("disk: could lseek() hard drive image file\n");

            ret = write(hard_drive.fd, controller.buffer, 512);
            if (ret < 512)
              bx_panic("disk: could not write() hard drive image file\n");
#endif
            controller.buffer_index = 0;

            /* update sector count, sector number, cylinder,
             * drive, head, status
             * if there are more sectors, read next one in...
             */
            /* sector count of 0 means 256, it'll wrap */
            controller.sector_count--;
            controller.sector_no++;
            if (controller.sector_no > hard_drive.sectors) {
              controller.sector_no = 1;
              controller.head_no++;
              if (controller.head_no >= hard_drive.heads) {
                controller.head_no = 0;
                controller.cylinder_no++;
                if (controller.cylinder_no >= hard_drive.cylinders)
                  controller.cylinder_no = hard_drive.cylinders - 1;
                }
              }

            /* When the write is complete, controller clears the DRQ bit and
             * sets the BSY bit.
             * If at least one more sector is to be written, controller sets DRQ bit,
             * clears BSY bit, and issues IRQ 14
             */

            controller.status.busy = 0;
            if (controller.sector_count!=0) {
              controller.status.drq = 1;
              controller.status.busy = 0;
              }
            else { /* no more sectors to write */
              controller.status.drq = 0;
              controller.status.drive_ready = 1;
              controller.status.corrected_data = 0;
              controller.status.prev_command_error = 0;
              }
            bx_trigger_irq(14);
            }
          break;

        default:
          bx_panic("disk: IO write(1f0h): current command is %02xh\n",
            (unsigned) controller.current_command);
        }
      break;

    case 0x1f1: /* hard disk write precompensation */
      if (bx_dbg.disk) {
        if (value == 0xff)
          bx_printf("disk: no precompensation\n");
        else
          bx_printf("disk: precompensation value %02x\n", (unsigned) value);
        }
      break;

    case 0x1f2: /* hard disk sector count */
      controller.sector_count = value;
      if (bx_dbg.disk)
        bx_printf("disk: sector count = %u\n", (unsigned) value);
      break;

    case 0x1f3: /* hard disk sector number */
      controller.sector_no = value;
      if (bx_dbg.disk)
        bx_printf("disk: sector number = %u\n", (unsigned) value);
      break;

    case 0x1f4: /* hard disk cylinder low */
      controller.cylinder_no = (controller.cylinder_no & 0x0300) | value;
      if (bx_dbg.disk)
        bx_printf("disk: cylinder low = %02xh\n", (unsigned) value);
      break;

    case 0x1f5: /* hard disk cylinder high */
      value &= 0x03; /* only use 2 bits for a total of 10bits */
      controller.cylinder_no = (controller.cylinder_no & 0x00ff) | value;
      if (bx_dbg.disk)
        bx_printf("disk: cylinder high = %02xh\n", (unsigned) value);
      break;

    case 0x1f6: /* hard disk drive and head register */
      if ( (value & 0x80) == 0 )
        bx_panic("disk: IO write(1f6h): ECC field 0\n");
      if ( ((value >> 5) & 0x03) != 1 )
        bx_panic("disk: IO write(1f6h: sector size NOT 512\n");
      controller.drive_select = (value >> 4) & 0x01;
      controller.head_no = (value & 0x0f);
      break;

    case 0x1f7: /* hard disk command */
      if ( (value & 0xf0) == 0x10 )
        value = 0x10;
      switch (value) {
        case 0x10: /* calibrate drive */
          if (controller.drive_select != 0)
            bx_panic("disk: calibrate drive != 0\n");
          /* move head to cylinder 0, issue IRQ 14 */
          controller.cylinder_no = 0;
          bx_trigger_irq(14);
          break;
        case 0x20: /* read multiple sectors, with retries */
          /* update sector_no, always points to current sector
           * after each sector is read to buffer, DRQ bit set and issue IRQ 14
           * if interrupt handler transfers all data words into main memory,
           * and more sectors to read, then set BSY bit again, clear DRQ and
           * read next sector into buffer
           * sector count of 0 means 256 sectors
           */

          controller.current_command = value;

          logical_sector = (controller.cylinder_no * hard_drive.heads *
                            hard_drive.sectors) +
                           (controller.head_no * hard_drive.sectors) +
                           (controller.sector_no - 1);

          if (logical_sector >=
              (hard_drive.cylinders * hard_drive.heads * hard_drive.sectors)) {
            bx_panic("disk: read sectors: out of bounds\n");
            }


#if defined(WIN32) && defined(MEMMAPDISKIMAGES)
          
		  memcpy(controller.buffer, hard_drive.img+logical_sector*512, 512);
	
#else
		  ret = lseek(hard_drive.fd, logical_sector * 512, SEEK_SET);

          if (ret < 0) {
            bx_panic("disk: could not lseek() hard drive image file\n");
            }
		  
		  ret = read(hard_drive.fd, controller.buffer, 512);
          if (ret < 512) {
            bx_panic("disk: could not read() hard drive image file\n");
            }
#endif
          controller.status.busy = 0;
          controller.status.drq = 1;
          controller.buffer_index = 0;
          bx_trigger_irq(14);
          break;

        case 0x30: /* write multiple sectors, with retries */
          /* update sector_no, always points to current sector
           * after each sector is read to buffer, DRQ bit set and issue IRQ 14
           * if interrupt handler transfers all data words into main memory,
           * and more sectors to read, then set BSY bit again, clear DRQ and
           * read next sector into buffer
           * sector count of 0 means 256 sectors
           */

          controller.current_command = value;

          /* implicit seek done :^) */
          controller.status.busy = 0;
          controller.status.drq = 1;
          controller.buffer_index = 0;
          break;

        case 0x90: /* Drive Diagnostic */
          controller.diag_byte = 0x01; /* no error, drive 0 and 1 */
          controller.diag_mode = 1;
          break;
        case 0x91: /* initialize drive parameters */
          bx_printf("initialize drive params\n");
          bx_printf("  sector count = %u\n",
            (unsigned) controller.sector_count);
          bx_printf("  drive select = %u\n",
            (unsigned) controller.drive_select);
          bx_printf("  head number = %u\n",
            (unsigned) controller.head_no);
          if (controller.drive_select != 0)
            bx_panic("disk: init drive params: drive != 0\n");
          if (controller.sector_count != hard_drive.sectors)
            bx_panic("disk: init drive params: sector count doesnt match\n");
          if ( controller.head_no != (hard_drive.heads-1) )
            bx_printf("disk: init drive params: head number doesn't match\n");
          controller.status.busy = 0;
          controller.status.drive_ready = 1;
          controller.status.drq = 1;
          break;
        default:
          bx_panic("IO write(1f7h): command %02x\n", (unsigned) value);
        }
      break;

    default:
      bx_panic("hard drive: io write to address %x = %02x\n",
        (unsigned) address, (unsigned) value);
    }
}

#if 0
  void
bx_hard_drv_int_handler(int vector)
{
#if 0
  bx_panic("bx_hard_drv_int_handler(): unsupported.\n");
#endif
  bx_outp(0x20, 0x20);  /* send EOI to master PIC */
}
#endif

#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_hard_drive_int13h(void)
{
  Bit8u drive, num_sectors, sector, head, status;
  Bit16u max_cylinder, cylinder, total_sectors;
  int logical_sector, ret;

#ifdef BX_DEBUG
if (bx_dbg.disk) {
  bx_printf("\n\n*** hard drive int13h handler called\n");
  bx_printf("    ah:%x al:%x ch:%x cl:%x dh:%x dl:%x\n", 
    AH, AL, CH, CL, DH, DL);
  }
#endif

  /* at this point, DL is >= 0x80 to be passed from the floppy int13h
     handler code */
  if (DL > 0x80) { /* only handle one disk for now */
if (bx_dbg.disk)
  bx_printf("drive > 0x80\n");
    set_disk_ret_status(AH = 0x01);
    bx_set_CF(1); /* error occurred */
    return;
    }

  switch (AH) {

    case 0x00: /* disk controller reset */
      drive = DL;

      set_disk_ret_status(AH = 0);
      set_diskette_ret_status(0);
      set_diskette_current_cyl(0, 0); /* current cylinder, diskette 1 */
      set_diskette_current_cyl(1, 0); /* current cylinder, diskette 2 */
      bx_set_CF(0); /* successful */
#if BX_DEBUG
if (bx_dbg.disk)
      bx_printf("disk controller reset\n");
#endif
#if 0
      bx_trigger_irq(5);
#endif
      return;
      break;

    case 0x01: /* read disk status */
      bx_access_physical(0x474, 1, BX_READ, &status);
      AH = status;
      set_disk_ret_status(0);
      /* set CF if error status read */
      if (AH) bx_set_CF(1);
      else    bx_set_CF(0);
#if BX_DEBUG
if (bx_dbg.disk)
      bx_printf("read disk status\n");
#endif
      return;
      break;

    case 0x02: /* read disk sectors */
#if BX_DEBUG
if (bx_dbg.disk)
      bx_printf("read disk sectors\n");
#endif
      num_sectors = AL;
      cylinder    = CH;
      cylinder    |= ( ((Bit16u) CL) << 2) & 0x300;
      cylinder    |= ( ((Bit16u) DH) << 4) & 0xc00;
      sector      = (CL & 0x3f);
      head        = (DH & 0x3f);

      /* record_disk(head, sector, cylinder); */

#if 0
???
      if ( (num_sectors > hard_drive.sectors)  ||
#endif
      if ( (cylinder >= hard_drive.cylinders) ||
           (sector > hard_drive.sectors) ||
           (head >= hard_drive.heads) ) {

		bx_printf("^^^ INT 13h received invalid params (read)\n");

        set_disk_ret_status(AH = 4);
        bx_set_CF(1); /* error occurred */
        return;
        }

      /* check for read past end of track. */
      if ( (sector + num_sectors - 1) > hard_drive.sectors) {

		bx_printf("^^^ INT 13h read past end of track\n");
		#ifdef BX_DEBUG_HD
		#if 0
		bx_printf("    num_sectors  = %u\n", (unsigned) num_sectors);
		bx_printf("    cylinder     = %u\n", (unsigned) cylinder);
		bx_printf("    sector       = %u\n", (unsigned) sector);
		bx_printf("    head         = %u\n", (unsigned) head);
		bx_printf("    hd.sectors   = %u\n", (unsigned) hard_drive.sectors);
		bx_printf("    hd.heads     = %u\n", (unsigned) hard_drive.heads);
		bx_printf("    hd.cylinders = %u\n", (unsigned) hard_drive.cylinders);
		#endif
		#endif
        }

      /* check for boundary overrun */
      if ( (BX + (512 * num_sectors) - 1) >
           0xFFFF) {
#if 0 /* bill */
bx_panic("boundary overrun in disk op\n");
#else
bx_printf("boundary overrun in disk op\n");
#endif
        set_disk_ret_status(AH = 9);
        bx_set_CF(1); /* error occurred */
		bx_printf("^^^ INT 13h boundary overrun failed\n");
        return;
        }

      if (num_sectors > 128)
        bx_panic("bx_hard_drive_int13h(): num_sectors out of range!\n");

      logical_sector = (cylinder * hard_drive.heads * hard_drive.sectors) +
                       (head * hard_drive.sectors) +
                       (sector - 1);
if ((logical_sector + (num_sectors-1)) >=
    (hard_drive.cylinders * hard_drive.heads * hard_drive.sectors)) {
#if 0 /* bill */
  bx_panic("(read) logical sector out of bounds\n");
#else
  bx_printf("(read) logical sector out of bounds\n");
  set_disk_ret_status(AH = 1);
  bx_cpu.eflags.cf = 1;
#endif
  }

#if defined(WIN32) && defined(MEMMAPDISKIMAGES)

	  memcpy(disk_buffer, hard_drive.img+logical_sector*512, num_sectors*512);

#else
      ret = lseek(hard_drive.fd, logical_sector * 512, SEEK_SET);
      if (ret < 0) {
        bx_panic("could not perform lseek() on hard drive image file\n");
        }
 
	  ret = read(hard_drive.fd, disk_buffer, num_sectors * 512);
      if (ret < (num_sectors * 512)) {
        bx_panic("could not perform read() on hard drive image file\n");
        }
#endif

      bx_virtual_block_write(&bx_cpu.es, BX, num_sectors * 512, &disk_buffer);

      set_disk_ret_status(AH = 0);
      AL = num_sectors;
      bx_set_CF(0); /* successful */
#if 0
      bx_trigger_irq(5);
#endif
      return;
      break;

    case 0x3: /* write disk sectors */
#if BX_DEBUG
if (bx_dbg.disk)
      bx_printf("write disk sectors\n");
#endif
      num_sectors = AL;
      cylinder    = CH;
      cylinder    |= ( ((Bit16u) CL) << 2) & 0x300;
      cylinder    |= ( ((Bit16u) DH) << 4) & 0xc00;
      sector      = (CL & 0x3f);
      head        = (DH & 0x3f);

record_disk(head, sector, cylinder);

#if 0
???
      if ( (num_sectors > hard_drive.sectors)  ||
#endif

      if ( (cylinder >= hard_drive.cylinders) ||
           (sector > hard_drive.sectors) ||
           (head >= hard_drive.heads) ) {

		  bx_printf("^^^ INT 13h received invalid params (write)\n");
#ifdef BX_DEBUG_HD
bx_printf("    num_sectors = 0x%x\n", (unsigned) num_sectors);
bx_printf("    cylinder    = 0x%x\n", (unsigned) cylinder);
bx_printf("    sector      = 0x%x\n", (unsigned) sector);
bx_printf("    head        = 0x%x\n", (unsigned) head);
#endif
        set_disk_ret_status(AH = 4);
        bx_set_CF(1); /* error occurred */
        return;
        }

      /* check for write past end of track. */
      if ( (sector + num_sectors - 1) > hard_drive.sectors) {
        }

      /* check for boundary overrun */
      if ( (BX + (512 * num_sectors) - 1) >
           0xFFFF) {
bx_panic("boundary overrun in disk op\n");
        set_disk_ret_status(AH = 9);
        bx_set_CF(1); /* error occurred */
#ifdef BX_DEBUG_HD
bx_printf("^^^ INT 13h boundary overrun\n");
#endif
        return;
        }

      if (num_sectors > 128)
        bx_panic("bx_hard_drive_int13h(): num_sectors out of range!\n");

      logical_sector = (cylinder * hard_drive.heads * hard_drive.sectors) +
                       (head * hard_drive.sectors) +
                       (sector - 1);

if ((logical_sector + (num_sectors-1)) >=
    (hard_drive.cylinders * hard_drive.heads * hard_drive.sectors)) {
#if 0 /* bill */
  bx_panic("(write) logical sector out of bounds\n");
#else
  bx_printf("(write) logical sector out of bounds\n");
  set_disk_ret_status(AH = 1);
  bx_cpu.eflags.cf = 1;
#endif
  }

#ifdef BX_DEBUG
		if (bx_dbg.disk) {
		bx_printf("logical sector is %d\n", (int) logical_sector);
		bx_printf("seeking to %d\n", (int) logical_sector * 512);
		}
#endif


      bx_virtual_block_read(&bx_cpu.es, BX, num_sectors * 512, &disk_buffer);

		if (bx_dbg.disk && num_sectors==1) {
			int i;
			for (i=0; i<512; i++) {
				if (isgraph(disk_buffer[i]))
					bx_printf("%c", (unsigned char) disk_buffer[i]);
				else
					bx_printf("?");
			}
			bx_printf("\n");
		}

#if defined(WIN32) && defined(MEMMAPDISKIMAGES)

	  memcpy(hard_drive.img+logical_sector*512, disk_buffer, num_sectors*512);

#else

      ret = lseek(hard_drive.fd, logical_sector * 512, SEEK_SET);
      if (ret < 0) {
        bx_panic("could not perform lseek() on hard drive image file\n");
        }

      ret = write(hard_drive.fd, disk_buffer, num_sectors * 512);
      if (ret < (num_sectors * 512)) {
        bx_panic("could not perform write() on hard drive image file\n");
        }
#endif


if (bx_dbg.disk)
bx_printf("all checks passed\n");
      set_disk_ret_status(AH = 0);
      AL = num_sectors;
      bx_set_CF(0); /* successful */
#if 0
      bx_trigger_irq(5);
#endif
      return;
      break;

    case 0x4: /* verify disk sectors */
#if BX_DEBUG
if (bx_dbg.disk)
      bx_printf("verify disk sectors\n");
#endif
      num_sectors = AL;
      cylinder    = CH;
      cylinder    |= ( ((Bit16u) CL) << 2) & 0x300;
      cylinder    |= ( ((Bit16u) DH) << 4) & 0xc00;
      sector      = (CL & 0x3f);
      head        = (DH & 0x3f);

#if 0
???
      if ( (num_sectors > hard_drive.sectors)  ||
#endif

      if ( (cylinder >= hard_drive.cylinders) ||
           (sector > hard_drive.sectors) ||
           (head >= hard_drive.heads) ) {
bx_printf("^^^ INT 13h received invalid params (verify)\n");
        set_disk_ret_status(AH = 4);
        bx_set_CF(1); /* error occurred */
        return;
        }

      /* check for verify past end of track.  */
      if ( (sector + num_sectors - 1) > hard_drive.sectors) {
        }

      /* check for boundary overrun */
      if ( (BX + (512 * num_sectors) - 1) >
           0xFFFF) {
        set_disk_ret_status(AH = 9);
        bx_set_CF(1); /* error occurred */
bx_printf("^^^ INT 13h boundary overrun\n");
        return;
        }

      if (num_sectors > 128)
        bx_panic("bx_hard_drive_int13h(): num_sectors out of range!\n");

      logical_sector = (cylinder * hard_drive.heads * hard_drive.sectors) +
                       (head * hard_drive.sectors) +
                       (sector - 1);

if ((logical_sector + (num_sectors-1)) >=
    (hard_drive.cylinders * hard_drive.heads * hard_drive.sectors)) {
#if 0 /* bill */
  bx_panic("(verify) logical sector out of bounds\n");
#else
  bx_printf("(verify) logical sector out of bounds\n");
  set_disk_ret_status(AH = 1);
  bx_cpu.eflags.cf = 1;
#endif
  }

#ifdef BX_DEBUG
if (bx_dbg.disk)
bx_printf("logical sector is %d\n", (int) logical_sector);
if (bx_dbg.disk)
bx_printf("seeking to %d\n", (int) logical_sector * 512);
#endif


#if defined(WIN32) && defined(MEMMAPDISKIMAGES)

	  memcpy(disk_buffer, hard_drive.img+logical_sector*512, num_sectors*512);

#else
      ret = lseek(hard_drive.fd, logical_sector * 512, SEEK_SET);
      if (ret < 0) {
        bx_panic("could not perform lseek() on hard drive image file\n");
        }

	  ret = read(hard_drive.fd, disk_buffer, num_sectors * 512);
      if (ret < (num_sectors * 512)) {
        bx_panic("could not perform read() on hard drive image file\n");
        }
#endif

      set_disk_ret_status(AH = 0);
      AL = num_sectors;
      bx_set_CF(0); /* successful */
#if 0
      bx_trigger_irq(5);
#endif
      return;
      break;

    case 0x05: /* format disk track */
bx_panic("format disk track called\n");
#if BX_DEBUG
if (bx_dbg.disk)
      bx_printf("format disk track\n");
#endif
      /* nop */
      set_disk_ret_status(AH = 0);
      bx_set_CF(0); /* successful */
#if 0
      bx_trigger_irq(5);
#endif
      return;
      break;

    case 0x08: /* read disk drive parameters */
#if BX_DEBUG
if (bx_dbg.disk)
      bx_printf("read disk drive parameters\n");
#endif
      max_cylinder = hard_drive.cylinders - 2; /* 0 based */
      AL = 0;
      CH = max_cylinder & 0xff;
      CL = (max_cylinder >> 2) & 0xc0;
      CL |= (hard_drive.sectors & 0x3f);
      DH = (max_cylinder >> 4) & 0xc0;
      DH |= ((hard_drive.heads - 1) & 0x3f);
      DL = 1;  /* one drive for now */
      set_disk_ret_status(AH = 0);
      bx_set_CF(0); /* successful */
#if 0
bx_printf("read disk drive params...\n");
bx_printf("CH: 0x%02x\n", (unsigned) CH);
bx_printf("CL: 0x%02x\n", (unsigned) CL);
bx_printf("DH: 0x%02x\n", (unsigned) DH);
bx_printf("DL: 0x%02x\n", (unsigned) DL);
#endif
      return;
      break;

    case 0x09: /* initialize drive parameters */
#if BX_DEBUG
if (bx_dbg.disk)
      bx_printf("initialize drive parameters\n");
#endif
      set_disk_ret_status(AH = 0);
      bx_set_CF(0); /* successful */
      return;
      break;
 
    case 0x0a: /* read disk sectors with ECC */
    case 0x0b: /* write disk sectors with ECC */
#if BX_DEBUG
if (bx_dbg.disk)
      bx_printf("read/write disk sectors with ECC\n");
#endif
#if 0 /* bill */
      bx_panic("int13h Functions 0Ah & 0Bh not implemented!\n");
#else
      bx_printf("int13h Functions 0Ah & 0Bh not implemented!\n");
      set_disk_ret_status(AH = 1);
      bx_cpu.eflags.cf = 1;
#endif
      return;
      break;

    case 0x0c: /* seek to specified cylinder */
#if BX_DEBUG
if (bx_dbg.disk)
      bx_printf("seek to specified cylinder\n");
#endif
      bx_printf("int13h function 0ch (seek) not implemented!\n");
      set_disk_ret_status(AH = 0);
      bx_set_CF(0); /* successful */
#if 0
      bx_trigger_irq(5);
#endif
      return;
      break;

    case 0x0d: /* alternate disk reset */
#if BX_DEBUG
if (bx_dbg.disk)
      bx_printf("alternate disk reset\n");
#endif
      set_disk_ret_status(AH = 0);
      bx_set_CF(0); /* successful */
#if 0
      bx_trigger_irq(5);
#endif
      return;
      break;

    case 0x10: /* check drive ready */
#if BX_DEBUG
if (bx_dbg.disk)
      bx_printf("check drive ready\n");
#endif
      set_disk_ret_status(AH = 0);
      bx_set_CF(0); /* successful */
      return;
      break;

    case 0x11: /* recalibrate */
#if BX_DEBUG
if (bx_dbg.disk)
      bx_printf("recalibrate\n");
#endif
      set_disk_ret_status(AH = 0);
      bx_set_CF(0); /* successful */
#if 0
      bx_trigger_irq(5);
#endif
      return;
      break;

    case 0x14: /* controller internal diagnostic */
#if BX_DEBUG
if (bx_dbg.disk)
      bx_printf("controller internal diagnostic\n");
#endif
      set_disk_ret_status(AH = 0);
      bx_set_CF(0); /* successful */
      AL = 0;
#if 0
      bx_trigger_irq(5);
#endif
      return;
      break;

    case 0x15: /* read disk drive size */
#if BX_DEBUG
if (bx_dbg.disk)
      bx_printf("read disk drive size\n");
#endif
      total_sectors = hard_drive.heads * hard_drive.sectors *
        (hard_drive.cylinders - 1);
      AH = 0x03;
      CX = total_sectors >> 16;
      DX = total_sectors & 0xffff;
      set_disk_ret_status(0);
      bx_set_CF(0); /* successful */
      return;
      break;

    case 0x18: /* */
      bx_printf("case 0x18 found in bx_hard_drive_int13h() unhandled.\n");
      bx_set_CF(1); /* unsuccessful */
      return;
      break;

    default:
#if 0 /* bill */
      bx_panic("case 0x%x found in bx_hard_drive_int13h()\n", (int)AH);
#else
      bx_printf("case 0x%x found in bx_hard_drive_int13h()\n", (int)AH);
      set_disk_ret_status(AH = 1);
      bx_cpu.eflags.cf = 1;
#endif
      break;
    }
}
#endif


#if BX_EXTERNAL_ENVIRONMENT==0
  static void
set_disk_ret_status(Bit8u val)
{
#if BX_DEBUG
if (bx_dbg.disk)
  bx_printf("set disk status to 0x%x\n", (int) val);
#endif
  bx_access_physical(0x474, 1, BX_WRITE, &val);
}
#endif

  void
bx_close_harddrive(void)
{
#if BX_DEBUG
if (bx_dbg.disk) {
  fprintf(stderr, "head: %u .. %u\n", (unsigned) minhead, maxhead);
  fprintf(stderr, "sector: %u .. %u\n", (unsigned) minsector, maxsector);
  fprintf(stderr, "cylinder: %u .. %u\n", (unsigned) mincylinder, maxcylinder);
  }
#endif


#if defined(WIN32) && defined(MEMMAPDISKIMAGES)
  UnmapViewOfFile(hard_drive.img);
  CloseHandle(hard_drive.map);
  CloseHandle(hard_drive.fd);
#else
  if (hard_drive.fd > -1) {	  
	  close(hard_drive.fd);
    }
#endif
}

  void
record_disk(Bit16u head, Bit16u sector, Bit16u cylinder)
{
  if (head < minhead) minhead = head;
  if (head > maxhead) maxhead = head;
  
  if (sector < minsector) minsector = sector;
  if (sector > maxsector) maxsector = sector;

  if (cylinder < mincylinder) mincylinder = cylinder;
  if (cylinder > maxcylinder) maxcylinder = cylinder;
}


char *gethdimg(void)
{
  return hard_drive.img;
}
