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
#include <string.h>
#include <ctype.h>

#include "../config.h"


// If we use memory-mapping to access the disk images, we need the definitions for objects like HANDLE. Fred
#if defined(WIN32) && defined(MEMMAPDISKIMAGES)
#include <windows.h>
#endif

/*#ifdef AUDIT_ENABLED
#include "../audit.h"
#endif
*/

#if BX_USE_NATIVE_FLOPPY
#  ifdef __linux__
#    include <linux/fd.h>
#    include <sys/ioctl.h>
#  else
#    error "native floppy device is not supported for your platform yet"
#  endif /* __linux__ */
#else
#endif /* BX_USE_NATIVE_FLOPPY */


#include "iodev.h"


/* for now, define a delay that all floppy IO operations take (usec) */
#define FLOPPY_DELAY 50000

/* for main status register */
#define FD_MS_MRQ  0x80
#define FD_MS_DIO  0x40
#define FD_MS_NDMA 0x20
#define FD_MS_BUSY 0x10
#define FD_MS_ACTD 0x08
#define FD_MS_ACTC 0x04
#define FD_MS_ACTB 0x02
#define FD_MS_ACTA 0x01

#define FROM_FLOPPY 10
#define TO_FLOPPY   11

static void floppy_command(void);
static void floppy_xfer(Bit8u drive, Bit32u offset, Bit8u *buffer,
            Bit32u bytes, Bit8u direction);
static void bx_floppy_timer_handler(void);



typedef struct {
#if defined(WIN32) && defined(MEMMAPDISKIMAGES)
  HANDLE fd;
  HANDLE map;
  PBYTE  img;
#else	
  int      fd;         /* file descriptor of floppy image file */
#endif
  unsigned sectors_per_track;    /* number of sectors/track */
  unsigned sectors;    /* number of formatted sectors on diskette */
  unsigned tracks;      /* number of tracks */
  unsigned heads;      /* number of heads */
  unsigned type;
  } floppy_t;

typedef struct {
  Bit8u   data_rate;

  Bit8u   command[10]; /* largest command size ??? */
  Bit8u   command_index;
  Bit8u   command_size;
  Boolean command_complete;
  Bit8u   pending_command;

  Bit8u   result[10];
  Bit8u   result_index;
  Bit8u   result_size;

  Bit8u   digital_output_reg;
  Bit8u   cylinder[4]; /* really only using 2 drives */
  Bit8u   last_drive;

  /* MAIN STATUS REGISTER
   * b7: MRQ: main request 1=data register ready     0=data register not ready
   * b6: DIO: data input/output:
   *     1=controller->CPU (ready for data read)
   *     0=CPU->controller (ready for data write)
   * b5: NDMA: non-DMA mode: 1=controller not in DMA modes
   *                         0=controller in DMA mode
   * b4: BUSY: instruction(device busy) 1=active 0=not active
   * b3-0: ACTD, ACTC, ACTB, ACTA:
   *       drive D,C,B,A in positioning mode 1=active 0=not active
   */
  Bit8u   main_status_reg;

  Bit8u   status_reg0;
  Bit8u   status_reg1;
  Bit8u   status_reg2;
  Bit8u   status_reg3;

  } diskette_controller_t;

static floppy_t floppy[2];
static diskette_controller_t controller;
static unsigned num_supported_floppies;


static int floppy_timer_index;

#if BX_EXTERNAL_ENVIRONMENT==0
static Bit8u disk_param_table[11];

/* Floppy buffer for read/writes.  needs to be at least
   2(heads) * 18(sectors/track) * 512(bytes/sector) = 18432
   Also shared now for a hard disk buffer which needs to be at least
   64K.
   */
static Bit8u floppy_buffer[64 * 1024];
#endif


extern bx_options_t bx_options;



  void
bx_init_floppy_hardware(void)
{
  Bit32u i;
  bx_iodev_t  io_device;
#if !defined(WIN32) || !defined(MEMMAPDISKIMAGES)
  struct stat stat_buf;
#endif
  int ret;
  unsigned long floppya_size, floppyb_size;

  /* FLOPPY DRIVE */
  io_device.read_funct   = bx_floppy_io_read_handler;
  io_device.write_funct  = bx_floppy_io_write_handler;
  io_device.handler_name = "Floppy Drive";
  io_device.start_addr   = 0x03F0;
  io_device.end_addr     = 0x03F7;
  io_device.irq          = 6;
  bx_register_io_handler(io_device);

  bx_cmos_reg[0x10] = 0x00; /* start out with: no drive 0, no drive 1 */

  /* Floppy A setup */
  floppy[0].sectors_per_track = 0;
  floppy[0].tracks            = 0;
  floppy[0].heads             = 0;
  floppy[0].sectors           = 0;
  floppy[0].type              = BX_FLOPPY_NONE;

  num_supported_floppies = 0;

  if (strlen(bx_options.floppya.path) == 0) {
    bx_cmos_reg[0x10] = (bx_cmos_reg[0x10] & 0x0f) | 0x00;
    }
  else {
    /* open floppy A image file */


#if defined(WIN32) && defined(MEMMAPDISKIMAGES)

  // If we use memory-mapped images, open the file, create the mapping and create a view of the whole file
  floppy[0].fd = CreateFile(bx_options.floppya.path, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL, NULL);
  if (floppy[0].fd == INVALID_HANDLE_VALUE)
    bx_panic("floppy: could not open image file '%s'!\n", bx_options.floppya.path);
  floppy[0].map = CreateFileMapping(floppy[0].fd, NULL, PAGE_READWRITE, 0, 0 ,NULL);
  if (floppy[0].map == NULL) {
	CloseHandle(floppy[0].fd);
    bx_panic("floppy: could not open image file '%s'!\n", bx_options.floppya.path);
  }
  floppy[0].img = MapViewOfFile(floppy[0].map, FILE_MAP_WRITE, 0, 0, 0);
  if (floppy[0].img == NULL) {
	CloseHandle(floppy[0].map);
	CloseHandle(floppy[0].fd);
    bx_panic("floppy: could not open image file '%s'!\n", bx_options.floppya.path);
  }

#else
	  
 #ifdef WIN32	  
	floppy[0].fd = open(bx_options.floppya.path, O_RDWR | O_BINARY);
 #else
    floppy[0].fd = open(bx_options.floppya.path, O_RDWR);
 #endif
    if (floppy[0].fd < 0) {
      perror("floppy open of a:\n");
      bx_panic("floppy: could not open image file '%s'!\n",
        bx_options.floppya.path);
      }

#endif

    floppya_size = 0;
    if ( bx_options.floppya.type==BX_FLOPPY_AUTO ) {
      /* look at size of image file to calculate floppy geometry */
#if defined(WIN32) && defined(MEMMAPDISKIMAGES)
		floppya_size = GetFileSize(floppy[0].fd, NULL);
#else

		if (fstat(floppy[0].fd, &stat_buf)) {
			perror("fstat'ing floppy 0 drive image file");
			bx_panic("fstat() returns error!\n");
        }
		floppya_size = stat_buf.st_size;
#endif	
	}
    else {
      switch (bx_options.floppya.type) {
        case BX_FLOPPY_1_44:  floppya_size = 2 * 80 * 18 * 512; break;
        case BX_FLOPPY_1_2:   floppya_size = 2 * 80 * 15 * 512; break;
        case BX_FLOPPY_0_720: floppya_size = 2 * 80 *  9 * 512; break;
        default:
          bx_panic("init_floppy: bad floppya type\n");
        }
      }

    switch (floppya_size) {
      case (2 * 80 * 18 * 512) : /* 1.44M 3.5" floppy */
        floppy[0].sectors_per_track = 18;
        floppy[0].tracks            = 80;
        floppy[0].heads             = 2;
        floppy[0].sectors           = 2 * 80 * 18;
        floppy[0].type              = BX_FLOPPY_1_44;
        bx_cmos_reg[0x10] = (bx_cmos_reg[0x10] & 0x0f) | 0x40;
        num_supported_floppies++;
        break;
#if 1 /* bill */
      case (2 * 81 * 18 * 512) : /* 1.44M 3.5" with 'extra track' */
        floppy[0].sectors_per_track = 18;
        floppy[0].tracks            = 81;
        floppy[0].heads             = 2;
        floppy[0].sectors           = 2 * 81 * 18;
        floppy[0].type              = BX_FLOPPY_1_44;
/* riad: dunno what's that, fix the 0x40 !!! */
        bx_cmos_reg[0x10] = (bx_cmos_reg[0x10] & 0x0f) | 0x40;
        num_supported_floppies++;
        break;
#endif
      case (2 * 80 * 15 * 512) : /* 1.2M 5.25" floppy */
        floppy[0].sectors_per_track = 15;
        floppy[0].tracks            = 80;
        floppy[0].heads             = 2;
        floppy[0].sectors           = 2 * 80 * 15;
        floppy[0].type              = BX_FLOPPY_1_2;
        bx_cmos_reg[0x10] = (bx_cmos_reg[0x10] & 0x0f) | 0x20;
        num_supported_floppies++;
        break;
      case (2 * 80 * 9 * 512) : /* 720K 3.5" floppy */
        floppy[0].sectors_per_track = 9;
        floppy[0].tracks            = 80;
        floppy[0].heads             = 2;
        floppy[0].sectors           = 2 * 80 * 9;
        floppy[0].type              = BX_FLOPPY_0_720;
        bx_cmos_reg[0x10] = (bx_cmos_reg[0x10] & 0x0f) | 0x30;
        num_supported_floppies++;
        break;
      case (2 * 81 * 9 * 512) : /* 720K 3.5" floppy with 'extra track' */
        floppy[0].sectors_per_track = 9;
        floppy[0].tracks            = 81;
        floppy[0].heads             = 2;
        floppy[0].sectors           = 2 * 81 * 9;
        floppy[0].type              = BX_FLOPPY_0_720;
        bx_cmos_reg[0x10] = (bx_cmos_reg[0x10] & 0x0f) | 0x30;
        num_supported_floppies++;
        break;  

// Added this to be able to boot DOS 3.0 Fred
      case (2 * 40 * 9 * 512) : /* 360K 5.25" floppy */
        floppy[0].sectors_per_track = 9;
        floppy[0].tracks            = 40;
        floppy[0].heads             = 2;
        floppy[0].sectors           = 2 * 40 * 9;
        floppy[0].type              = BX_FLOPPY_0_360;
        bx_cmos_reg[0x10] = (bx_cmos_reg[0x10] & 0x0f) | 0x10;
        num_supported_floppies++;
        break;

      default: /* unsupported floppy size, others easily supported */
        bx_panic("unrecognized size of floppy disk 0 image file\n");
      }
    }


  /* Floppy B setup */
  floppy[1].sectors_per_track = 0;
  floppy[1].tracks            = 0;
  floppy[1].heads             = 0;
  floppy[1].sectors           = 0;
  floppy[1].type              = BX_FLOPPY_NONE;

  if (strlen(bx_options.floppyb.path) == 0) {
    bx_cmos_reg[0x10] = (bx_cmos_reg[0x10] & 0xf0) | 0x00;
    }
  else {
    /* open floppy B image file */

#if defined(WIN32) && defined(MEMMAPDISKIMAGES)

  // If we use memory-mapped images, open the file, create the mapping and create a view of the whole file
  floppy[1].fd = CreateFile(bx_options.floppyb.path, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL, NULL);
  if (floppy[1].fd == INVALID_HANDLE_VALUE)
    bx_panic("floppy: could not open image file '%s'!\n", bx_options.floppyb.path);
  floppy[1].map = CreateFileMapping(floppy[1].fd, NULL, PAGE_READWRITE, 0, 0 ,NULL);
  if (floppy[1].map == NULL) {
	CloseHandle(floppy[1].fd);
    bx_panic("floppy: could not open image file '%s'!\n", bx_options.floppyb.path);
  }
  floppy[1].img = MapViewOfFile(floppy[1].map, FILE_MAP_WRITE, 0, 0, 0);
  if (floppy[1].img == NULL) {
	CloseHandle(floppy[1].map);
	CloseHandle(floppy[1].fd);
    bx_panic("floppy: could not open image file '%s'!\n", bx_options.floppyb.path);
  }

#else
	  
 #ifdef WIN32	  
	floppy[1].fd = open(bx_options.floppyb.path, O_RDWR | O_BINARY);
 #else
    floppy[1].fd = open(bx_options.floppyb.path, O_RDWR);
 #endif
    if (floppy[1].fd < 0) {
      perror("floppy open of a:\n");
      bx_panic("floppy: could not open image file '%s'!\n",
        bx_options.floppyb.path);
      }

#endif

    floppyb_size = 0;
    if ( bx_options.floppyb.type==BX_FLOPPY_AUTO ) {
      /* look at size of image file to calculate floppy geometry */
#if defined(WIN32) && defined(MEMMAPDISKIMAGES)
		floppyb_size = GetFileSize(floppy[1].fd, NULL);
#else
		ret = fstat(floppy[1].fd, &stat_buf);
		if (ret) {
			perror("fstat'ing floppy 1 drive image file");
			bx_panic("fstat() returns error!\n");
        }
		floppyb_size = stat_buf.st_size;
#endif
      }
    else {
      switch (bx_options.floppyb.type) {
        case BX_FLOPPY_1_44:  floppyb_size = 2 * 80 * 18 * 512; break;
        case BX_FLOPPY_1_2:   floppyb_size = 2 * 80 * 15 * 512; break;
        case BX_FLOPPY_0_720: floppyb_size = 2 * 80 *  9 * 512; break;
        default:
          bx_panic("init_floppy: bad floppyb type\n");
        }
      }

    switch (floppyb_size) {
      case (2 * 80 * 18 * 512) : /* 1.44M 3.5" floppy */
        floppy[1].sectors_per_track = 18;
        floppy[1].tracks            = 80;
        floppy[1].heads             = 2;
        floppy[1].sectors           = 2 * 80 * 18;
        floppy[1].type              = BX_FLOPPY_1_44;
        bx_cmos_reg[0x10] = (bx_cmos_reg[0x10] & 0xf0) | 0x04;
        num_supported_floppies++;
        break;
      case (2 * 80 * 15 * 512) : /* 1.2M 5.25" floppy */
        floppy[1].sectors_per_track = 15;
        floppy[1].tracks            = 80;
        floppy[1].heads             = 2;
        floppy[1].sectors           = 2 * 80 * 15;
        floppy[1].type              = BX_FLOPPY_1_2;
        bx_cmos_reg[0x10] = (bx_cmos_reg[0x10] & 0xf0) | 0x02;
        num_supported_floppies++;
        break;
      case (2 * 80 * 9 * 512) : /* 720K 3.5" floppy */
        floppy[1].sectors_per_track = 9;
        floppy[1].tracks            = 80;
        floppy[1].heads             = 2;
        floppy[1].sectors           = 2 * 80 * 9;
        floppy[1].type              = BX_FLOPPY_0_720;
        bx_cmos_reg[0x10] = (bx_cmos_reg[0x10] & 0xf0) | 0x03;
        num_supported_floppies++;
        break;
// Added this to be able to boot DOS 3.0 (for MacGyver) Fred
      case (2 * 40 * 9 * 512) : /* 360K 5.25" floppy */
        floppy[1].sectors_per_track = 9;
        floppy[1].tracks            = 40;
        floppy[1].heads             = 2;
        floppy[1].sectors           = 2 * 40 * 9;
        floppy[1].type              = BX_FLOPPY_0_360;
        bx_cmos_reg[0x10] = (bx_cmos_reg[0x10] & 0xf0) | 0x01;
        num_supported_floppies++;
        break;

      default: /* unsupported floppy size, others easily supported */
        bx_panic("unrecognized size of floppy disk 1 image file\n");
      }
    }


  /* CMOS Equipment Byte register */
  if (num_supported_floppies > 0)
    bx_cmos_reg[0x14] = (bx_cmos_reg[0x14] & 0x3e) |
                        ((num_supported_floppies-1) << 6) |
                        1;
  else
    bx_cmos_reg[0x14] = (bx_cmos_reg[0x14] & 0x3e);


  controller.data_rate = 0; /* 500 Kbps */

  controller.command_complete = 1; /* waiting for new command */
  controller.command_index = 0;
  controller.command_size = 0;
  controller.pending_command = 0;

  controller.result_index = 0;
  controller.result_size = 0;

  /* data register ready, not in DMA mode */
  controller.main_status_reg = FD_MS_MRQ;
  controller.status_reg0 = 0;
  controller.status_reg1 = 0;
  controller.status_reg2 = 0;
  controller.status_reg3 = 0;

  controller.digital_output_reg = 0x04;
  for (i=0; i<4; i++) {
    controller.cylinder[i] = 0;
    }
  controller.last_drive = 0;

  floppy_timer_index =
    bx_register_timer( bx_floppy_timer_handler, FLOPPY_DELAY, 0,0);
}


#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_init_floppy_bios(void)
{
  Bit8u status[7];
  Bit8u zero8, recalibrate_status, controller_info;
  Bit8u disk_media_state, disk_op_start_state, motor_timeout;
  Bit16u equip_word;
  Bit8u  equip_byte;
  Bit32u i;

  bx_isr_code_cf[2] = 0x13;
  bx_register_int_vector(0x13, bx_isr_code_cf, sizeof(bx_isr_code_cf),
      bx_floppy_int13h_handler
#ifdef WIN32DEBUG
      ,H_FLOPPYINT13
#endif      
      );
#if 1 /* bill */
/* some viruses expect int 40h to point to int 13h's floppy handler. */
  bx_isr_code_cf[2] = 0x40;
  bx_register_int_vector(0x40, bx_isr_code_cf, sizeof(bx_isr_code_cf),
                         bx_floppy_int13h_handler
#ifdef WIN32DEBUG
			 ,H_FLOPPYINT13
#endif 			 
			 );
#endif

/*#ifdef AUDIT_ENABLED
  bx_access_physical(0x4C, 2, BX_READ, &OrigIntVec[1].ofs);
  bx_access_physical(0x4E, 2, BX_READ, &OrigIntVec[1].seg);
  bx_access_physical(0x100, 2, BX_READ, &OrigIntVec[2].ofs);
  bx_access_physical(0x102, 2, BX_READ, &OrigIntVec[2].seg);
#endif
*/

  zero8 = 0;

  recalibrate_status = 0x03; /* drive 0 & 1 calibrated */
  /* diskette recalibrate status */
  bx_access_physical(0x43E, 1, BX_WRITE, &recalibrate_status);

  /* diskette motor status */
  /* maybe I should tell the system this is always on ??? */
  bx_access_physical(0x43F, 1, BX_WRITE, &zero8);

  motor_timeout = 0x00;
  /* diskette motor timeout counter */
  bx_access_physical(0x440, 1, BX_WRITE, &motor_timeout);

  /* diskette controller status return code */
  bx_access_physical(0x441, 1, BX_WRITE, &zero8);

  status[0] = 0;
  status[1] = 0;
  status[2] = 0;
  status[3] = 0;
  status[4] = 0;
  status[5] = 0;
  status[6] = 0;

  /* diskette & disk controller status bytes (7bytes) */
  for (i=0; i<7; i++) {
    bx_access_physical(0x442 + i, 1, BX_WRITE, &status[i]);
    }

  /* diskette configuration data */
  bx_access_physical(0x48B, 1, BX_WRITE, &zero8);

  /* diskette controller information */
  /* drive 0 & 1 type has been determined,
     drive 0 & 1 have no diskette changed detection line
   */
  controller_info = 0x44;
  bx_access_physical(0x48F, 1, BX_WRITE, &controller_info);

  /* diskette 0 media state */
  disk_media_state = 0x10;
  switch (floppy[0].type) {
	case BX_FLOPPY_0_360: disk_media_state |= 0x04; break;
    case BX_FLOPPY_0_720: disk_media_state |= 0x07; break;
    case BX_FLOPPY_1_2:   disk_media_state |= 0x05; break;
    case BX_FLOPPY_1_44:  disk_media_state |= 0x07; break;
    case BX_FLOPPY_NONE:  break;
    default:
      bx_panic("init_floppy: bad floppya type\n");
    }
  bx_access_physical(0x490, 1, BX_WRITE, &disk_media_state);

  /* diskette 1 media state */
  disk_media_state = 0x10;
  switch (floppy[1].type) {
	case BX_FLOPPY_0_360: disk_media_state |= 0x04; break;
    case BX_FLOPPY_0_720: disk_media_state |= 0x07; break;
    case BX_FLOPPY_1_2:   disk_media_state |= 0x05; break;
    case BX_FLOPPY_1_44:  disk_media_state |= 0x07; break;
    case BX_FLOPPY_NONE:  break;
    default:
      bx_panic("init_floppy: bad floppyb type\n");
    }
  bx_access_physical(0x491, 1, BX_WRITE, &disk_media_state);

  /* diskette 0 operational starting state */
  /* drive type has been determined, has no changed detection line */
  disk_op_start_state = 0x04;
  bx_access_physical(0x492, 1, BX_WRITE, &disk_op_start_state);

  /* diskette 1 operational starting state */
  /* drive type has been determined, has no changed detection line */
  disk_op_start_state = 0x04;
  bx_access_physical(0x493, 1, BX_WRITE, &disk_op_start_state);

  /* diskette 0 current cylinder */
  bx_access_physical(0x494, 1, BX_WRITE, &zero8);

  /* diskette 1 current cylinder */
  bx_access_physical(0x495, 1, BX_WRITE, &zero8);

  /* since no provisions are made for multiple drive types, most
     values in this table are ignored.  I set parameters for 1.44M
     floppy here */
  disk_param_table[0]  = 0xAF;
  disk_param_table[1]  = 0x03; /* no DMA used */
  disk_param_table[2]  = 0x25;
  disk_param_table[3]  = 0x02;
  switch (floppy[0].type) {
// Random values here, I just want to try it. Fred
	case BX_FLOPPY_0_360:
      disk_param_table[4] = 9;
      disk_param_table[5]  = 0x1B;
      disk_param_table[7]  = 0x54;
      break;

    case BX_FLOPPY_0_720:
      disk_param_table[4] = 9;     /* sectors per track */
      disk_param_table[5]  = 0x2a;
      disk_param_table[7]  = 0x50; /* format gap length */
      break;
    case BX_FLOPPY_1_2:
      disk_param_table[4] = 15;
      disk_param_table[5]  = 0x1B;
      disk_param_table[7]  = 0x54;
      break;
    case BX_FLOPPY_1_44:
      disk_param_table[4] = 18;
      disk_param_table[5]  = 0x1B;
      disk_param_table[7]  = 0x6C;
      break;
    case BX_FLOPPY_NONE:
      disk_param_table[4] = 18;
      disk_param_table[5]  = 0x1B;
      disk_param_table[7]  = 0x6C;
      break;
    default:
      bx_panic("init_floppy: bad floppya type\n");
    }
  disk_param_table[6]  = 0xFF;
  disk_param_table[8]  = 0xF6;
  disk_param_table[9]  = 0x0F;
  disk_param_table[10] = 0x01; /* most systems default to 8 */

  /* diskette parameter table in BIOS ROM area*/
  for (i=0; i < 11; i++) {
    bx_access_physical(BX_DISKETTE_PARAM_TBL + i,
      1, BX_WRITE, &disk_param_table[i]);
    }

  /* int 1E points to disk parameter table */
  bx_set_interrupt_vector(0x1E, BX_DISKETTE_PARAM_TBL);

  bx_outp(0x70, 0x14, 1);
  equip_byte = bx_inp(0x71, 1);
bx_printf("equip_byte = %02x\n", (unsigned) equip_byte);

  bx_access_physical(0x410, 2, BX_READ, &equip_word);
  equip_word = (equip_word & 0xff00) | equip_byte;
  bx_access_physical(0x410, 2, BX_WRITE, &equip_word);
}
#endif


  /* reads from the floppy io ports */
  Bit32u
bx_floppy_io_read_handler(Bit32u address, unsigned io_len)
{
  Bit8u status, value;

#if BX_DMA_FLOPPY_IO < 1
  /* if we're not supporting DMA/floppy IO just return a bogus value */
  return(0xffffffff);
#endif

  if (bx_dbg.floppy)
    bx_printf("floppy: read access to port %04x\n", (unsigned) address);

  switch (address) {
    case 0x3F2: /* diskette controller digital output register */
      break;
    case 0x3F4: /* diskette controller main status register */
      status = controller.main_status_reg;
      /*controller.main_status_reg*/
      return(status);
      break;
    case 0x3F5: /* diskette controller data */
      if (controller.result_size == 0) {
        bx_panic("floppy: diskette controller:port3f5: no results to read\n");
        }

      value = controller.result[controller.result_index++];
      if (controller.result_index >= controller.result_size) {
        controller.result_size = 0;
        controller.result_index = 0;
        controller.main_status_reg = 0x80;
        }
      return(value);
      break;
    case 0x3F7: /* diskette controller digital input register */
      break;
    }

  bx_panic("floppy: io_read: bailing\n");
  return(0);
}

  /* writes to the floppy io ports */
  void
bx_floppy_io_write_handler(Bit32u address, Bit32u value, unsigned io_len)
{
  Bit8u dma_and_interrupt_enable;
  Bit8u normal_operation;
  Bit8u drive_select;
  Bit8u motor_on_drive0, motor_on_drive1;

#if BX_DMA_FLOPPY_IO < 1
  /* if we're not supporting DMA/floppy IO just return */
  return;
#endif

  if (bx_dbg.floppy)
    bx_printf("floppy: write access to port %04x, value=%02x\n",
      (unsigned) address, (unsigned) value);

  switch (address) {
    case 0x3F2: /* diskette controller digital output register */
      controller.digital_output_reg = value;
      motor_on_drive1 = value & 0x20;
      motor_on_drive0 = value & 0x10;
      dma_and_interrupt_enable = value & 0x08;
      normal_operation = value & 0x04;
      drive_select = value & 0x03;
      if (bx_dbg.floppy) {
        bx_printf("floppy: io_write: digital output register\n");
        bx_printf("  motor on, drive1 = %d\n", motor_on_drive1 > 0);
        bx_printf("  motor on, drive0 = %d\n", motor_on_drive0 > 0);
        bx_printf("  dma_and_interrupt_enable=%02x\n",
          (unsigned) dma_and_interrupt_enable);
        bx_printf("  normal_operation=%02x\n",
          (unsigned) normal_operation);
        bx_printf("  drive_select=%02x\n",
          (unsigned) drive_select);
        }
      if (drive_select>1) {
        bx_panic("floppy: io_write: drive_select>1\n");
        }
      break;

    case 0x3f4: /* diskette controller data rate select register */
      bx_panic("floppy: io_write: data rate select register\n");
      break;

    case 0x3F5: /* diskette controller data */
      if (bx_dbg.floppy)
        bx_printf("floppy: command = %02x\n", (unsigned) value);
      if (controller.command_complete) {
        if (controller.pending_command!=0)
          bx_panic("floppy: io: 3f5: receiving new comm, old one (%02x) pending\n",
            (unsigned) controller.pending_command);
        controller.command[0] = value;
        controller.command_complete = 0;
        controller.command_index = 1;
        /* read/write command in progress */
        controller.main_status_reg |= 0x10;
        switch (value) {
          case 0x03: /* specify */
            controller.command_size = 3;
            break;
          case 0x07: /* recalibrate */
            controller.command_size = 2;
            break;
          case 0x08: /* sense interrupt status */
            controller.command_size = 1;
            floppy_command();
            controller.command_complete = 1;
            break;
          case 0x0f: /* seek */
            controller.command_size = 3;
            break;
          case 0x4a: /* read ID */
            controller.command_size = 2;
            break;
          case 0xc5: /* write normal data */
            controller.command_size = 9;
            break;
          case 0xe6: /* read normal data */
            controller.command_size = 9;
            break;
          default:
            bx_panic("floppy: io write:3f5: unsupported case %02x\n",
              (unsigned) value);
          }
        }
      else {
        controller.command[controller.command_index++] =
          value;
        if (controller.command_index ==
            controller.command_size) {
          /* read/write command not in progress any more */
          floppy_command();
          controller.command_complete = 1;
          }
        }
      if (bx_dbg.floppy)
        bx_printf("floppy: io_write: diskette controller data\n");
      return;
      break;

    case 0x3F6: /* diskette controller (reserved) */
      if (bx_dbg.floppy)
        bx_printf("floppy: io_write: reserved register unsupported\n");
      break;
    case 0x3F7: /* diskette controller configuration control register */
      if (bx_dbg.floppy)
        bx_printf("floppy: io_write: config control register\n");
      controller.data_rate = value & 0x03;
      if (bx_dbg.floppy)
        switch (controller.data_rate) {
          case 0: bx_printf("  500 Kbps\n"); break;
          case 1: bx_printf("  300 Kbps\n"); break;
          case 2: bx_printf("  250 Kbps\n"); break;
          case 3: bx_printf("  1 Mbps\n"); break;
          }
      return;
      break;
   default:
      bx_panic("floppy: io_write: unknown port %04h\n", (unsigned) address);
      break;
    }
}


#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_floppy_int13h_handler(int vector)
{
  Bit8u drive, num_sectors, track, sector, head, status;
  unsigned long logical_sector;
  Bit16u es, di;
  Boolean boundary_overrun;

#if 0 /* I don't want code/data segregation now, just replication */
#ifdef FOR_BOO /* riad */
  // every int13 call goes here (floppy and harddrive)
  // we won't catch OUT/IN to/from disk controller here though

  extern int g_do_boo;
  extern MIN_MAX_INDEX stuff;

  if(g_do_boo && (vector == 0x13) && (stuff.junk != 2) && (AH==2 || AH==3))
    call_sector_trace(DL, // disk
                      AL, // number of sectors
                      CX2Cyl(CX),               // cylinder/track
                      DH,                       // head
                      CX2Sec(CX),               // sector number
                      bx_cpu.es.selector.value, // segment
                      BX,                       // offset
                      AH);                      /* operation */
#endif
#endif

  if (bx_dbg.floppy) {
    bx_printf("floppy: interrupt handler called\n");
    bx_printf("    ah:%x al:%x ch:%x cl:%x dh:%x dl:%x\n", 
      AH, AL, CH, CL, DH, DL);
    }

  if (DL >= 0x80) {
    bx_hard_drive_int13h();
    return;
    }


  switch (AH) {

    case 0x0: /* diskette controller reset */
      drive = DL;
      if (drive > 1) {
        set_diskette_ret_status(AH = 1); /* invalid param ... */
        bx_set_CF(1); /* error occurred */
        return;
        }
      if (floppy[drive].type == BX_FLOPPY_NONE) {
        set_diskette_ret_status(AH = 0x80); /* drive not responding */
        bx_set_CF(1); /* error occurred */
        return;
        }
      set_diskette_ret_status(AH = 0);
      bx_set_CF(0); /* successful */
      set_diskette_current_cyl(drive, 0); /* current cylinder */
      return;
      break;

    case 0x1: /* read diskette status */
      bx_access_physical(0x441, 1, BX_READ, &status);
      AH = status;
      bx_set_CF(0); /* successful */
      /* I don't know if this service sets the return status ??? */
      /* set_diskette_ret_status(0); */
      return;
      break;

    case 0x2: /* read diskette sectors */
      num_sectors = AL;
      track       = CH;
      sector      = CL;
      head        = DH;
      drive       = DL;

      if (drive > 1) {
        set_diskette_ret_status(AH = 1);
        AL = 0; /* no sectors read */
        bx_set_CF(1); /* error occurred */
        return;
	}

      if (floppy[drive].type == BX_FLOPPY_NONE) {
        set_diskette_ret_status(AH = 0x80); /* drive not responding */
        bx_set_CF(1); /* error occurred */
        return;
        }

      if ( (track >= floppy[drive].tracks) ||
           (sector > floppy[drive].sectors_per_track) ||
           (head >= floppy[drive].heads) ) {
#if 0
        if (bx_dbg.floppy)
#endif
        bx_printf("floppy: INT 13h func 02h received invalid params\n");
        bx_printf("track=%u sector=%u head=%u\n",
              (unsigned) track, (unsigned) sector, (unsigned) head);
        set_diskette_ret_status(AH = 1);
        AL = 0; /* no sectors read */
        bx_set_CF(1); /* error occurred */
        return;
        }

      if (num_sectors > 128) {
#if 0
        if (bx_dbg.floppy)
#endif
          bx_printf("floppy: int13h(): num_sectors > 128!\n");
        set_diskette_ret_status(AH = 1);
        AL = 0; /* no sectors read */
        bx_set_CF(1); /* error occurred */
        return;
        }

      /* check for read past end of track. */
      if ( (num_sectors + sector - 1) > floppy[drive].sectors_per_track) {
#if 0
        if (bx_dbg.floppy)
#endif
          bx_printf("floppy: int13h(): read past end of track!\n");
        }

      /* check for boundary overrun */
      if ( (BX + (512 * num_sectors) - 1) >
           0xFFFF) {
        bx_panic("floppy: boundary overrun in disk op\n");
        set_diskette_ret_status(AH = 9);
        bx_set_CF(1); /* error occurred */
        return;
        }

      logical_sector = (track * 2 * floppy[drive].sectors_per_track) +
                       (head * floppy[drive].sectors_per_track) +
                       (sector - 1);

      if (logical_sector >= floppy[drive].sectors) {
#if 0
        if (bx_dbg.floppy)
#endif
          bx_printf("floppy: INT 13h logical sector past last\n");
        set_diskette_ret_status(AH = 4);
        AL = 0; /* no sectors read */
        bx_set_CF(1); /* error occurred */
        return;
        }


      boundary_overrun = 0;

#if 0
      /* check for boundary overrun */
      if ( (BX + (512 * num_sectors) - 1) >
           0xFFFF) {
        num_sectors = ((0x10000 - BX) / 512);
        boundary_overrun = 1;
        }
#endif

      floppy_xfer(drive, logical_sector*512, floppy_buffer,
                     num_sectors*512, FROM_FLOPPY);

      bx_virtual_block_write(&bx_cpu.es, BX,
        num_sectors * 512, &floppy_buffer);

      set_diskette_current_cyl(drive, track);
      AL = num_sectors;
      if (boundary_overrun) {
#if 0
        if (bx_dbg.floppy)
#endif
          bx_printf("floppy: INT 13h incurred boundary overrun\n");
        set_diskette_ret_status(AH = 0x09); /* data boundary error */
        bx_set_CF(1); /* error occurred */
        }
      else {
        set_diskette_ret_status(AH = 0);
        bx_set_CF(0); /* successful */
        }
      return;
      break;

    case 0x3: /* write diskette sectors */
      num_sectors = AL;
      track       = CH;
      sector      = CL;
      head        = DH;
      drive       = DL;
      if (drive > 1) {
        set_diskette_ret_status(AH = 1);
        AL = 0; /* no sectors written */
        bx_set_CF(1); /* error occurred */
        return;
	}

      if (floppy[drive].type == BX_FLOPPY_NONE) {
        set_diskette_ret_status(AH = 0x80); /* drive not responding */
        bx_set_CF(1); /* error occurred */
        return;
        }

      if ( (track > floppy[drive].tracks) ||
           (sector > floppy[drive].sectors_per_track) ||
           (head > floppy[drive].heads) ) {
#ifdef BX_DEBUG
        if (bx_dbg.floppy)
#endif
        bx_printf("floppy: INT 13h requested write past EOT\n");
        set_diskette_ret_status(AH = 1);
        AL = 0; /* no sectors written */
        bx_set_CF(1); /* error occurred */
        return;
        }

      if (num_sectors > 128) {
#ifdef BX_DEBUG
        if (bx_dbg.floppy)
#endif
        bx_printf("floppy: int13h(): num_sectors out of range!\n");
        set_diskette_ret_status(AH = 1);
        AL = 0; /* no sectors read */
        bx_set_CF(1); /* error occurred */
        return;
        }

      /* check for write past end of track. */
      if ( (num_sectors + sector - 1) > floppy[drive].sectors_per_track) {
        }

      logical_sector = (track * 2 * floppy[drive].sectors_per_track) +
                       (head * floppy[drive].sectors_per_track) +
                       (sector - 1);

#if 0
      if ( (BX + (512 * num_sectors) - 1) >
           0xFFFF) {
bx_panic("floppy: boundary overrun in disk op\n");
        set_diskette_ret_status(AH = 9);
        bx_set_CF(1); /* error occurred */
#ifdef BX_DEBUG2
bx_printf("floppy: INT 13h boundary overrun\n");
#endif
        return;
        }
#endif


      if (logical_sector >= floppy[drive].sectors) {
#ifdef BX_DEBUG
        if (bx_dbg.floppy)
#endif
        bx_printf("floppy: INT 13h requested write past EOT\n");
        set_diskette_ret_status(AH = 4);
        AL = 0; /* no sectors written */
        bx_set_CF(1); /* error occurred */
        return;
        }

      bx_virtual_block_read(&bx_cpu.es, BX,
        num_sectors * 512, &floppy_buffer);

      floppy_xfer(drive, logical_sector*512, floppy_buffer,
                     num_sectors*512, TO_FLOPPY);

      set_diskette_ret_status(AH = 0);
      set_diskette_current_cyl(drive, track);
      AL = num_sectors;
      bx_set_CF(0); /* successful */
      return;
      break;

    case 0x4: /* verify diskette sectors */
      num_sectors = AL;
      track       = CH;
      sector      = CL;
      head        = DH;
      drive       = DL;
      if (drive > 1) {
        set_diskette_ret_status(AH = 1);
        AL = 0; /* no sectors verified */
        bx_set_CF(1); /* error occurred */
	}

      if (floppy[drive].type == BX_FLOPPY_NONE) {
        set_diskette_ret_status(AH = 0x80); /* drive not responding */
        bx_set_CF(1); /* error occurred */
        return;
        }

      if ( (track > floppy[drive].tracks) ||
           (sector > floppy[drive].sectors_per_track) ||
           (head > floppy[drive].heads) ) {
#ifdef BX_DEBUG
          if (bx_dbg.floppy)
#endif
        bx_printf("floppy: INT 13h requested verify past EOT\n");
        set_diskette_ret_status(AH = 1);
        AL = 0; /* no sectors verified */
        bx_set_CF(1); /* error occurred */
        }

      if (num_sectors > 128) {
#ifdef BX_DEBUG
        if (bx_dbg.floppy)
#endif
        bx_printf("floppy: int13h(): num_sectors out of range!\n");
        set_diskette_ret_status(AH = 1);
        AL = 0; /* no sectors read */
        bx_set_CF(1); /* error occurred */
        return;
        }

      /* check for verify past end of track. */
      if ( (num_sectors + sector - 1) > floppy[drive].sectors_per_track) {
        }

#if 0
      /* check for boundary overrun */
      if ( (BX + (512 * num_sectors) - 1) >
           0xFFFF) {
        set_diskette_ret_status(AH = 4);
        bx_set_CF(1); /* error occurred */
		bx_printf("floppy: INT 13h boundary overrun\n");
        return;
        }
#endif


      logical_sector = (track * 2 * floppy[drive].sectors_per_track) +
                       (head * floppy[drive].sectors_per_track) +
                       (sector - 1);
      if (logical_sector >= floppy[drive].sectors) {
#ifdef BX_DEBUG
        if (bx_dbg.floppy)
#endif
        bx_printf("floppy: INT 13h requested verify past EOT\n");
        set_diskette_ret_status(AH = 4);
        AL = 0; /* no sectors verified */
        bx_set_CF(1); /* error occurred */
        }


      floppy_xfer(drive, logical_sector*512, floppy_buffer,
                     num_sectors*512, FROM_FLOPPY);

      set_diskette_ret_status(AH = 0);
      set_diskette_current_cyl(drive, track);
      AL = num_sectors;
      bx_set_CF(0); /* successful */
      return;
      break;

    case 0x5: /* format diskette track */
      num_sectors = AL;
      track       = CH;
      head        = DH;
      drive       = DL;

      // if track not present, create it and make this floppy image file bigger
      bx_printf("format diskette track: (num=%d) (tr=%02d hd=%02d drive=%02d)\n",
          AL, CH, DH, DL);
      if(track==80)
      {
          long this_size = 81 * floppy[drive].heads * floppy[drive].sectors_per_track * 512;
          if(chsize(floppy[drive].fd, this_size) != -1)
          {
              floppy[drive].tracks = 81;
              floppy[drive].sectors= this_size/512;
          }
          else
              bx_printf("couldn't resize the floppy image file to %d\n", this_size);
      }

      if (drive > 1) {
        set_diskette_ret_status(AH = 1);
        bx_set_CF(1); /* error occurred */
      }
      if (floppy[drive].type == BX_FLOPPY_NONE) {
        set_diskette_ret_status(AH = 0x80); /* drive not responding */
        bx_set_CF(1); /* error occurred */
        return;
        }

      /* nop */
      AH = 0;
      set_diskette_ret_status(AH = 0);
      set_diskette_current_cyl(drive, track);
      bx_set_CF(0); /* successful */
      break;

    case 0x8: /* read diskette drive parameters */
      drive = DL;
      if (drive>1 || floppy[drive].type==BX_FLOPPY_NONE) {
        AX = 0;
        BX = 0;
        CX = 0;
        DX = 0;
        bx_load_seg_reg(&bx_cpu.es, 0);
        DL = num_supported_floppies;
        set_diskette_ret_status(AH=1);
        bx_set_CF(1); /* error occurred */
        return;
        }

      switch ( floppy[drive].type ) {
		case BX_FLOPPY_0_360: BL = 1; break;
        case BX_FLOPPY_1_2:   BL = 2; break;
        case BX_FLOPPY_0_720: BL = 3; break;
        case BX_FLOPPY_1_44:  BL = 4; break;
        default:
          bx_panic("floppy: int13: bad floppy type\n");
        }
      AH = 0; /* success */
      AL = 0;
      BH = 0;
      CH = floppy[drive].tracks - 1;
      CL = floppy[drive].sectors_per_track;
      DH = 1; /* max head number */
      DL = num_supported_floppies;

      /* set es & di to point to 11 byte diskette param table */
      bx_access_physical(0x78, 2, BX_READ, &di);
      bx_access_physical(0x78 + 2, 2, BX_READ, &es);
      bx_load_seg_reg(&bx_cpu.es, es);
      DI = di;
      bx_set_CF(0); /* successful */
      /* disk status not changed upon success */
      return;
      break;

    case 0x15: /* read diskette drive type */
      drive = DL;
      if (drive > 1) {
        AH = 0; /* drive not present */
        bx_set_CF(1); /* invalid drive number */
        return;
        }
      if (floppy[drive].type==BX_FLOPPY_NONE) {
        AH = 0;
        bx_set_CF(0); /* successful, not present */
        }
      AH = 1; /* doesn't support change line */
      bx_set_CF(0); /* successful */
      return;
      break;

    case 0x16: /* get diskette change line status */
      drive = DL;
      if (drive > 1) {
        set_diskette_ret_status(AH = 0x01); /* invalid drive */
        bx_set_CF(1);
        return;
        }

      set_diskette_ret_status(AH = 0x06); /* change line not supported */
      bx_set_CF(1);
      return;
      break;

    case 0x17: /* set diskette type for format(old) */
      /* not used for 1.44M floppies */
bx_printf("floppy: unsupported int13h function 0x17\n");
      set_diskette_ret_status(AH = 1); /* not supported */
      bx_set_CF(1);
      return;
      break;

    case 0x18: /* set diskette type for format(new) */
bx_printf("floppy: AH = %x, CX = %x, DL = %x\n", (unsigned) AH,
  (unsigned) CX, (unsigned) DL);
bx_printf("floppy: unsupported int13h function 0x18\n");
      set_diskette_ret_status(AH = 1); /* do later */
      bx_set_CF(1);
      return;
      break;

    case 0x20: /* ??? */
      set_diskette_ret_status(AH = 1);
      bx_set_CF(1);
      bx_printf("floppy: unsupported int13h function 0x20\n");
      return;
      break;

    default:
#if 0 /* bill */
      bx_panic("floppy: found case 0x%x in int13h_handler()!\n", AH);
#else
      bx_printf("found case 0x%x in bx_floppy_int13h_handler()!\n", AH);
      set_diskette_ret_status(AH = 1);
      bx_cpu.eflags.cf = 1;       
#endif
      break;
    }
}
#endif


#if BX_EXTERNAL_ENVIRONMENT==0
  void
set_diskette_ret_status(Bit8u value)
{

#ifdef BX_DEBUG
  if (bx_dbg.floppy)
    bx_printf("SET DISKETTE STATUS %d\n", (int) value);
#endif
  bx_access_physical(0x441, 1, BX_WRITE, &value);
}

  void
set_diskette_current_cyl(Bit8u drive, Bit8u cyl)
{
  if (drive > 1)
    bx_printf("set_diskette_current_cyl(): drive > 1\n");
  bx_access_physical(0x494+drive, 1, BX_WRITE, &cyl);
}
#endif

  static void
floppy_command(void)
{
#if BX_EXTERNAL_ENVIRONMENT==1
  bx_panic("floppy_command(): uses DMA: not supported for"
           " external environment\n");
#else
  unsigned i;
  Bit8u step_rate_time;
  Bit8u head_unload_time;
  Bit8u head_load_time;
  Bit8u motor_on;
  Bit8u head, drive, cylinder, sector, eot;
  Bit32u dma_base, dma_roof, logical_sector;
  Bit16u num_sectors;


  if (bx_dbg.floppy) {
    bx_printf("FLOPPY COMMAND: ");
    for (i=0; i<controller.command_size; i++)
      bx_printf("[%02x] ", (unsigned) controller.command[i]);
    bx_printf("\n");
    }

#if 0
  /* execute phase of command is in progress */
  controller.main_status_reg |= 20;
#endif

  switch (controller.command[0]) {
    case 0x03: /* specify */
      /* execution: specified parameters are loaded */
      /* result: no result bytes, no interrupt */
      step_rate_time = controller.command[1] >> 4;
      head_unload_time = controller.command[1] & 0x0f;
      head_load_time = controller.command[2] >> 1;
      /*controller.main_status_reg = FD_MS_MRQ;*/
      return;
      break;

    case 0x07: /* recalibrate */
      controller.last_drive = controller.command[1] & 0x03;
      if (bx_dbg.floppy)
        bx_printf("floppy_command(): recalibrate drive %u\n",
          (unsigned) controller.last_drive);
      if (controller.last_drive > 1)
        bx_panic("floppy_command(): drive > 1\n");
      motor_on = ( (controller.digital_output_reg>>(controller.last_drive+4))
                   & 0x01 );
      if (motor_on == 0)
        bx_panic("floppy_command(): recal drive with motor off\n");
      controller.cylinder[controller.last_drive] = 0;
      bx_activate_timer( floppy_timer_index,
        1 /* active */,
        FLOPPY_DELAY /* useconds*/ );
      /* command head to track 0
       * controller set to non-busy
       * error condition noted in Status reg 0's equipment check bit
       * seek end bit set to 1 in Status reg 0 regardless of outcome
       */
      /* data reg not ready, controller busy */
      controller.main_status_reg = 0x50;
      controller.pending_command = 0x07; /* recalibrate pending */
      return;
      break;

    case 0x08: /* sense interrupt status */
      /* execution:
       *   get status
       * result:
       *   no interupt
       *   byte0 = status reg0
       *   byte1 = current cylinder number (0 to 79)
       */
      /*controller.status_reg0 = ;*/
      controller.result[0] = 0x20 | controller.last_drive;
      controller.result[1] = controller.cylinder[controller.last_drive];
      controller.result_size = 2;
      controller.result_index = 0;

      /* read ready */
      controller.main_status_reg = 0xd0;
      if (bx_dbg.floppy)
        bx_printf("sense interrupt status\n");
      return;
      break;

    case 0x0f: /* seek */
      /* command:
       *   byte0 = 0F
       *   byte1 = drive & head select
       *   byte2 = cylinder number
       * execution:
       *   postion head over specified cylinder
       * result:
       *   no result bytes, issues an interrupt
       */
      head = (controller.command[1] >> 2) & 0x01;
      controller.last_drive = controller.command[1] & 0x03;
      controller.cylinder[controller.last_drive] =
        controller.command[2];
      if (controller.last_drive > 1)
        bx_panic("floppy_command(): seek: drive>1\n");
      /* ??? should also check cylinder validity */
      bx_activate_timer( floppy_timer_index,
        1 /* active */,
        FLOPPY_DELAY /* useconds*/ );
      /* data reg not ready, controller busy */
      controller.main_status_reg = 0xd0;
      controller.pending_command = 0x0f; /* seek pending */
      return;
      break;

    case 0x4a: /* read ID */
      drive = controller.command[1] & 0x03;
      motor_on = (controller.digital_output_reg>>(drive+4)) & 0x01;
      if (motor_on == 0)
        bx_panic("floppy_command(): 4a: motor not on\n");
      if (drive > 0)
        bx_panic("floppy io: 4a: bad drive #\n");
      controller.result_size = 7;
      controller.result_index = 0;
      controller.result[0] = 0; /* ??? */
      controller.result[1] = 0;
      controller.result[2] = 0;
      controller.result[3] = controller.cylinder[drive];
      controller.result[4] = 0; /* head */
      controller.result[5] = 0; /* sector at completion */
      controller.result[6] = 2;
      bx_activate_timer( floppy_timer_index,
        1 /* active */,
        FLOPPY_DELAY /* useconds*/ );
      /* data reg not ready, controller busy */
      controller.main_status_reg = 0xd0;
      controller.pending_command = 0x4a; /* read ID pending */
      return;
      break;

    case 0xe6: /* read normal data */
    case 0xc5: /* write normal data */
      drive = controller.command[1] & 0x03;
      motor_on = (controller.digital_output_reg>>(drive+4)) & 0x01;
      if (motor_on == 0)
        bx_panic("floppy_command(): read/write: motor not on\n");
      head = controller.command[3] & 0x01;
      cylinder = controller.command[2]; /* 0..79 depending */
      sector = controller.command[4];   /* 1..36 depending */
      eot = controller.command[6];      /* 1..36 depending */
      if (bx_dbg.floppy) {
        bx_printf("\n\nread/write normal data\n");
        bx_printf("BEFORE\n");
        bx_printf("  drive    = %u\n", (unsigned) drive);
        bx_printf("  head     = %u\n", (unsigned) head);
        bx_printf("  cylinder = %u\n", (unsigned) cylinder);
        bx_printf("  sector   = %u\n", (unsigned) sector);
        bx_printf("  eot      = %u\n", (unsigned) eot);
        }
      if (drive > 0)
        bx_panic("floppy io: bad drive #\n");
      if (head > 1)
        bx_panic("floppy io: bad head #\n");
      if ( cylinder >= floppy[drive].tracks ) {
        bx_printf("\nfloppy io: normal read/write: params out of range\n");
        bx_printf("*** sector # %02xh\n", (unsigned) sector);
        bx_printf("*** cylinder #%02xh\n", (unsigned) cylinder);
        bx_printf("*** eot #%02xh\n", (unsigned) eot);
        bx_printf("*** head #%02xh\n", (unsigned) head);
        bx_panic("bailing\n");
        return;
        }

      if (sector > floppy[drive].sectors_per_track) {
        /* return error */
        bx_printf("floppy_command: attempt to read/write sector %u,"
                     " sectors/track=%u\n", (unsigned) sector,
                     (unsigned) floppy[drive].sectors_per_track);
        controller.result_size = 7;
        controller.result_index = 0;
        controller.result[0] = 0x40 | drive; /* abnormal termination */
        controller.result[1] = 0x84; /* sector past EOT, no data error */
        controller.result[2] = 0x00;
        controller.result[3] = cylinder;
        controller.result[4] = head;
        controller.result[5] = sector;
        controller.result[6] = 2;

        controller.cylinder[drive] = cylinder;
        bx_activate_timer( floppy_timer_index,
          1 /* active */,
          FLOPPY_DELAY /* useconds*/ );
        /* data reg not ready, controller busy */
        controller.main_status_reg = 0xd0;
        controller.pending_command = controller.command[0];
        return;
        }

      if (eot != floppy[drive].sectors_per_track)
        bx_panic("floppy io: bad eot #%02xh\n", (unsigned) eot);

      if (cylinder != controller.cylinder[drive])
        bx_printf("floppy io: cylinder request != current cylinder\n");

      logical_sector = (cylinder * 2 * floppy[drive].sectors_per_track) +
                       (head * floppy[drive].sectors_per_track) +
                       (sector - 1);

      if (logical_sector >= floppy[drive].sectors) {
        bx_panic("floppy io: logical sector out of bounds\n");
        }

      if ( ((dma1.chan[2].base_count + 1) % 512) != 0 )
        bx_panic("floppy io: read/write sector(s): DMA count not"
                     " multiple of 512\n");
      num_sectors = (dma1.chan[2].base_count + 1) / 512;
      if ( (sector + (num_sectors-1)) > eot )
        bx_panic("floppy io: read/write sector(s): requested sectors"
                     "not all within same track\n");

      dma_base = (dma1.chan[2].page_reg << 16) | dma1.chan[2].base_address;
      dma_roof = dma_base + dma1.chan[2].base_count;
      if ( (dma_base & 0xffff0000) != (dma_roof & 0xffff0000) ) {
        bx_panic("floppy io: DMA request outside 64k boundary\n");
        }

      /* values after completion of data xfer */
      /* ??? calculation depends on base_count be multiple of 512 */
      sector += num_sectors;
      if (sector > floppy[drive].sectors_per_track) {
        sector -= floppy[drive].sectors_per_track;
        head++;
        if (head>1) {
          head=0;
          cylinder++;
          if (cylinder >= floppy[drive].tracks) {
            cylinder = floppy[drive].tracks;
            bx_printf("floppy_command: clamping cylinder to max\n");
            }
          }
        }

      if (bx_dbg.floppy) {
        bx_printf("AFTER\n");
        bx_printf("  drive    = %u\n", (unsigned) drive);
        bx_printf("  head     = %u\n", (unsigned) head);
        bx_printf("  cylinder = %u\n", (unsigned) cylinder);
        bx_printf("  sector   = %u\n", (unsigned) sector);
        bx_printf("  eot      = %u\n", (unsigned) eot);
        }

      controller.result_size = 7;
      controller.result_index = 0;
      controller.result[0] = 0;
      controller.result[1] = 0;
      controller.result[2] = 0;
      controller.result[3] = cylinder;
      controller.result[4] = head;
      controller.result[5] = sector;
      controller.result[6] = 2;

      controller.cylinder[drive] = cylinder;

      if (controller.command[0] == 0xe6) { /* read */
        Bit32u addr;
        unsigned index;
        floppy_xfer(drive, logical_sector*512, floppy_buffer,
          dma1.chan[2].base_count + 1, FROM_FLOPPY);

        for (addr=dma_base,index=0; addr<=dma_roof; addr++, index++) {
          bx_access_physical(addr, 1, BX_WRITE, &floppy_buffer[index]);
          }
        dma1.chan[2].current_address = dma1.chan[2].base_address +
                                       dma1.chan[2].base_count + 1;
        dma1.chan[2].current_count = 0xffff;
        }
      else if (controller.command[0] == 0xc5) { /* write */
        Bit32u addr;
        unsigned index;
        for (addr=dma_base,index=0; addr<=dma_roof; addr++, index++) {
          bx_access_physical(addr, 1, BX_READ, &floppy_buffer[index]);
          }
        floppy_xfer(drive, logical_sector*512, floppy_buffer,
          dma1.chan[2].base_count + 1, TO_FLOPPY);
        dma1.chan[2].current_address = dma1.chan[2].base_address +
                                       dma1.chan[2].base_count + 1;
        dma1.chan[2].current_count = 0xffff;
        }
      else bx_panic("floppy_command(): unknown read/write command\n");

      bx_activate_timer( floppy_timer_index,
        1 /* active */,
        FLOPPY_DELAY /* useconds*/ );
      /* data reg not ready, controller busy */
      controller.main_status_reg = 0xd0;
      controller.pending_command = controller.command[0];
      return;
      break;

    default:
      bx_panic("floppy_command(): unknown function\n");
    }
  bx_panic("\nfloppy_command()\n");
#endif
}

  static void
floppy_xfer(Bit8u drive, Bit32u offset, Bit8u *buffer,
            Bit32u bytes, Bit8u direction)
{
  int ret;

  if (drive > 1)
    bx_panic("floppy_xfer: drive > 1\n");

  if (bx_dbg.floppy) {
    bx_printf("drive=%u\n", (unsigned) drive);
    bx_printf("offset=%u\n", (unsigned) offset);
    bx_printf("bytes=%u\n", (unsigned) bytes);
    bx_printf("direction=%s\n", (direction==FROM_FLOPPY)? "from" : "to");
    }


#if defined(WIN32) && defined(MEMMAPDISKIMAGES)
  // We do not need the lseek if we use memory-mapped files
#else  
  ret = lseek(floppy[drive].fd, offset, SEEK_SET);
  if (ret < 0) {
    bx_panic("could not perform lseek() on floppy image file\n");
  }
#endif


  if (direction == FROM_FLOPPY) {

#if defined(WIN32) && defined(MEMMAPDISKIMAGES)
	memcpy(buffer, floppy[drive].img+offset, bytes);
#else
	ret = read(floppy[drive].fd, buffer, bytes);
    if (ret < bytes) {
      /* ??? */
      if (ret > 0) {
 #ifdef WIN32
	     bx_panic("floppy: partial read() on floppy image returns %u/%u\n",
          (unsigned) ret, (unsigned) bytes);
 #else
        bx_printf("floppy: partial read() on floppy image returns %u/%u\n",
          (unsigned) ret, (unsigned) bytes);
        memset(buffer + ret, 0, bytes - ret);
 #endif
        }
      else {
        bx_printf("floppy: read() on floppy image returns 0\n");
        memset(buffer, 0, bytes);
        }
      }
#endif
    }
  else { /* TO_FLOPPY */


#if defined(WIN32) && defined(MEMMAPDISKIMAGES)
	memcpy(floppy[drive].img+offset, buffer, bytes);
#else	  
	ret = write(floppy[drive].fd, buffer, bytes);
    if (ret < bytes) {
      bx_panic("could not perform write() on floppy image file\n");
      }
#endif
  }

#ifdef WIN32DEBUG2
  bx_printf("WIN32DEBUG:::floppy ret=%d bytes=%d\n", ret, bytes);
#endif
}




  static void
bx_floppy_timer_handler(void)
{
  switch ( controller.pending_command ) {
    case 0x07: /* recal */
      controller.pending_command = 0;
      /* write ready, not busy */
      controller.main_status_reg = 0x80;
      bx_trigger_irq(6);
      break;

    case 0x0f: /* seek */
      controller.pending_command = 0;
      /* write ready, not busy */
      controller.main_status_reg = 0x80;
      bx_trigger_irq(6);
      break;

    case 0x4a: /* read ID */
      controller.pending_command = 0;
      /* read ready, busy */
      controller.main_status_reg = 0xd0;
      if (bx_dbg.floppy)
        bx_printf("<<READ ID DONE>>\n");
      bx_trigger_irq(6);
      break;

    case 0xc5: /* write normal data */
      controller.pending_command = 0;
      /* read ready, busy */
      controller.main_status_reg = 0xd0;
      if (bx_dbg.floppy)
        bx_printf("<<WRITE DONE>>\n");
      bx_trigger_irq(6);
      break;

    case 0xe6: /* read normal data */
      controller.pending_command = 0;
      /* read ready, busy */
      controller.main_status_reg = 0xd0;
      if (bx_dbg.floppy)
        bx_printf("<<READ DONE>>\n");
      bx_trigger_irq(6);
      break;

    default: 
      bx_panic("floppy_timer_handler(): unknown case %02x\n",
        (unsigned) controller.pending_command);
    }
}






/* this is not used... */

void bx_switch_disk(void)
{
#if 0

/* DL = drive (0=A, 1=B, 0x80=C, etc.)
   DS:SI -> null terminated string to filename */

  typedef struct
    {
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

  extern hard_drive_t hard_drive;
  char filename[65];
  int ret;
#if defined(WIN32) && defined(MEMMAPDISKIMAGES)
  DWORD diskimgsz;
#else
  struct stat stat_buf;
#endif
  extern int lock_floppy;

  if (lock_floppy) return;
  bx_printf("setting drive %d image to %s\n",SI,filename);

  switch(DL)
    {
    case 0x80:  

#if defined(WIN32) && defined(MEMMAPDISKIMAGES)
	CloseImage(&hard_drive.fd, &hard_drive.map, &hard_drive.img);

    strcpy(bx_options.diskc.path, filename);

	  // Open and mem-map image
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

	  diskimgsz = GetFileSize(hard_drive.fd, NULL);
	  switch (diskimgsz) { //...see below for the rest of the switch

#else
	  close(hard_drive.fd);
      strcpy(bx_options.diskc.path,filename);
 #ifdef WIN32
      hard_drive.fd = open(bx_options.diskc.path, O_RDWR | O_BINARY);
 #else
      hard_drive.fd = open(bx_options.diskc.path, O_RDWR);
 #endif
      if (hard_drive.fd < 0)
        bx_panic("could not open hard drive image file '%s'!\n",
                  bx_options.diskc.path);
	  
	  /* look at size of image file to calculate disk geometry */
      ret = fstat(hard_drive.fd, &stat_buf);
      if (ret) {
        perror("fstat'ing hard drive image file");
        bx_panic("fstat() returns error!\n");
      }

      switch (stat_buf.st_size) {
#endif

      case (306 * 4 * 17 * 512) :
       hard_drive.cylinders = 306;
        hard_drive.heads     = 4;
        hard_drive.sectors   = 17;
        break;
      case (615 * 4 * 17 * 512) :
        hard_drive.cylinders = 615;
        hard_drive.heads     = 4;
        hard_drive.sectors   = 17;
        break;
      default:
        bx_panic("unrecognized size of hard disk image file\n");
      }
      break;
      
    case 0:


#if defined(WIN32) && defined(MEMMAPDISKIMAGES)
	  // Close image
	  UnmapViewOfFile(floppy[0].img);
	  CloseHandle(floppy[0].map);
	  CloseHandle(floppy[0].fd);

      strcpy(bx_options.floppya.path,filename);

	  // Open and mem-map image
	  floppy[0].fd = CreateFile(bx_options.floppya.path, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL, NULL);
	  if (floppy[0].fd == INVALID_HANDLE_VALUE)
		bx_panic("could not open floppy image file '%s'!\n", bx_options.floppya.path);
	  floppy[0].map = CreateFileMapping(floppy[0].fd, NULL, PAGE_READWRITE, 0, 0 ,NULL);
	  if (floppy[0].map == NULL) {
		CloseHandle(floppy[0].fd);
	    bx_panic("could not open floppy image file '%s'!\n", bx_options.floppya.path);
	  }
	  floppy[0].img = MapViewOfFile(floppy[0].map, FILE_MAP_WRITE, 0, 0, 0);
	  if (floppy[0].img == NULL) {
		CloseHandle(floppy[0].map);
		CloseHandle(floppy[0].fd);
	    bx_panic("could not open floppy image file '%s'!\n", bx_options.floppya.path);
	  }

#else
	  close(floppy[0].fd);
      strcpy(bx_options.floppya.path,filename);  
      /* open floppy A image file */
 #ifdef WIN32
      floppy[0].fd = open(bx_options.floppya.path, O_RDWR | O_BINARY);
 #else
      floppy[0].fd = open(bx_options.floppya.path, O_RDWR);
 #endif
      if (floppy[0].fd < 0) {
        bx_panic("could not open floppy image file '%s'!\n",
                  bx_options.floppya.path);
      }
#endif
      break;
        
    case 1:

		
#if defined(WIN32) && defined(MEMMAPDISKIMAGES)
	  // Close image
	  UnmapViewOfFile(floppy[1].img);
	  CloseHandle(floppy[1].map);
	  CloseHandle(floppy[1].fd);

      strcpy(bx_options.floppyb.path,filename);

	  // Open and mem-map image
	  floppy[1].fd = CreateFile(bx_options.floppyb.path, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL, NULL);
	  if (floppy[1].fd == INVALID_HANDLE_VALUE)
		bx_panic("could not open floppy image file '%s'!\n", bx_options.floppyb.path);
	  floppy[1].map = CreateFileMapping(floppy[1].fd, NULL, PAGE_READWRITE, 0, 0 ,NULL);
	  if (floppy[1].map == NULL) {
		CloseHandle(floppy[1].fd);
	    bx_panic("could not open floppy image file '%s'!\n", bx_options.floppyb.path);
	  }
	  floppy[1].img = MapViewOfFile(floppy[1].map, FILE_MAP_WRITE, 0, 0, 0);
	  if (floppy[1].img == NULL) {
		CloseHandle(floppy[1].map);
		CloseHandle(floppy[1].fd);
	    bx_panic("could not open floppy image file '%s'!\n", bx_options.floppyb.path);
	  }

#else	
	  close(floppy[1].fd);
      strcpy(bx_options.floppyb.path,filename);
      /* open floppy B image file */
 #ifdef WIN32
      floppy[1].fd = open(bx_options.floppyb.path, O_RDWR | O_BINARY);
 #else
      floppy[1].fd = open(bx_options.floppyb.path, O_RDWR);
 #endif
      if (floppy[1].fd < 0) {
        bx_panic("could not open floppy image file '%s'!\n",
                  bx_options.floppyb.path);
      }
#endif
    }
#endif
}






#if defined(WIN32) && defined(MEMMAPDISKIMAGES)
static int OpenImage(char *path, HANDLE *fd, HANDLE *map, PBYTE *img) {
	*fd = CreateFile(path, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (*fd == INVALID_HANDLE_VALUE)
		return 1;
	*map = CreateFileMapping(*fd, NULL, PAGE_READWRITE, 0, 0 ,NULL);
	if (*map == NULL) {
		CloseHandle(*fd);
	    return 1;
	}
	*img = MapViewOfFile(*map, FILE_MAP_WRITE, 0, 0, 0);
	if (*img == NULL) {
		CloseHandle(*map);
		CloseHandle(*fd);
	    return 1;
	}
	return 0;
}

static void CloseImage(HANDLE *fd, HANDLE *map, PBYTE *img) {
	UnmapViewOfFile(*img);
	CloseHandle(*map);
	CloseHandle(*fd);
}
#endif





static int overwrite_disk_img(char *diskpath, char *newdisk) {
  FILE *f=NULL, *g=NULL;
  long r;
  int rc = 0;
#define MYCOPYBUFSZ (256*1024)
  char copybuf[MYCOPYBUFSZ];
  if (unlink(diskpath))
	  return 1;
  f = fopen(newdisk, "rb");
  g = fopen(diskpath, "wb");
  if (f == NULL || g == NULL) { rc = 1; goto disk_ow_close_and_ret; }
  while (r = fread(copybuf, 1, MYCOPYBUFSZ, f)) {
	  if (fwrite(copybuf, 1, r, g) != r) { rc = 1 ; goto disk_ow_close_and_ret; }
  }
  if (! feof(f)) rc = 1;
disk_ow_close_and_ret:
  if (f) fclose(f);
  if (g) fclose(g);
  return rc;
}

/* this is used instead of bx_switch_disk */
/* drives supported: 0 -> A: */

void bx_replace_disk(int drive, char *newdisk)
{
  typedef struct
    {
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

  extern hard_drive_t hard_drive;

  switch(drive)
    {
    case 0:

#if defined(WIN32) && defined(MEMMAPDISKIMAGES)
	CloseImage(&floppy[0].fd, &floppy[0].map, &floppy[0].img);
#else
	close(floppy[0].fd);
#endif

    if (overwrite_disk_img(bx_options.floppya.path, newdisk)) {
		bx_panic("can't overwrite disk image (%s)!\n", bx_options.floppya.path);
	}

#if defined(WIN32) && defined(MEMMAPDISKIMAGES)
	if (OpenImage(bx_options.floppya.path, &floppy[0].fd, &floppy[0].map, &floppy[0].img)) {
		bx_panic("could not open floppy image file '%s'!\n", bx_options.floppya.path);
	}
#else
 #ifdef WIN32
      floppy[0].fd = open(bx_options.floppya.path, O_RDWR | O_BINARY);
 #else
      floppy[0].fd = open(bx_options.floppya.path, O_RDWR);
 #endif
      if (floppy[0].fd < 0) {
        bx_panic("could not open floppy image file '%s'!\n", bx_options.floppya.path);
      }
#endif
      break;
    }
}


char *getfloppyimg(int n)
{
  return floppy[n].img;
}
