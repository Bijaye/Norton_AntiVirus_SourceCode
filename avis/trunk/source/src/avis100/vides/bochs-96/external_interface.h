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


/* <external_interface.h> */

/*
 * This header defines prototypes which will be called by Bochs,
 * when compiling for use with an external CPU & memory environment.
 * ("./configure --enable-external-cpu-memory")
 * In this configuration, a foreign simulation of a CPU and memory
 * can make use the suite of IO devices provided by Bochs.
 *
 * The interface between the foreign simulation code and Bochs consists
 * of the following mechanisms:
 *
 * - IO read/writes
 *     Supported in Bochs.  Call from foreign code whenever an IO read/write
 *     occurs (INP & OUTP).
 * - A20 control (from keyboard)
 *     Supported by foreign code.  Called from Bochs' keyboard IO code.
 * - Bochs software initialization and control transfer point.
 *     At the appropriate place, foreign code allows Bochs to initialize,
 *     by using this mechanism.  Bochs will return back to the caller,
 *     and the foreign software resumes control again.
 *     Thereafter, Bochs code is invoked only for short
 *     intervals, by means of the mechanisms defined in this header.
 * - Bochs tick mechanism
 *     The foreign code should call this tick mechanism, every instruction
 *     or so.  The tick is the means by which Bochs supplies timers
 *     requested by some of it's IO device emulation components.
 * - INTR/PIC interface
 *     The PIC is supported by Bochs.  To notify foreign CPU code,
 *     when it activates the INTR line, and thus the INT line on the
 *     CPU, Bochs will call the corresponding function.  When the CPU
 *     processes the interrupt, it calls an acknowledge function which
 *     Bochs supplies to notify Bochs that the interrupt has been
 *     processed.  NOTE: DURING acknowledging, Bochs may again activate
 *     the INT logic, since an interrupt may be pending.
 * - CPU reset (from keyboard)
 *     Bochs will call a CPU reset routine, which should exist in the
 *     foreign code, during situations where the keyboard IO device
 *     receives a request to do so.
 *
 * Bochs "typedefs" of interest:
 *   Bit8u/Bit8s:     8 bit unsigned/signed quantities on your platform
 *   Bit16u/Bit16s:  16 bit unsigned/signed quantities on your platform
 *   Bit32u/Bit32s:  32 bit unsigned/signed quantities on your platform
 *   (these are determined by the "configure" script and "config.h")
 *
 * ~/.bochsrc -or ./.bochsrc file:
 *   You'll need a ".bochsrc" file.  First ~/.bochsrc is tried.
 *   If that files doesn't exist, then a local ./.bochsrc is tried.
 *   I should reverse this search order, and probably will soon.
 *   Look at the .bochsrc files which comes with the Bochs distribution,
 *   for options.  Relevant options:
 *     (none yet)
 */

/* 
 need configurable HGA timer (currently 3Hz).
 */

/* The following need to be defined in the external environment code
 *
 * external_set_enable_a20(value):
 *   value > 0: A20 GATE enabled, A20 address line used in address computation
 *   value = 0: A20 GATE disabled, A20 address line always 0
 * external_get_enable_a20():
 *   returns 0 if A20 GATE disabled
 *   returns 1 if A20 GATE enabled
 * external_reset_cpu(void):
 *   called if keyboard logic receives request to RESET CPU
 * external_set_INTR(value):
 *   called by PIC when INT(R) line is changed.  When CPU services
 *   interrupt, call bx_IAC() to receive the interrupt vector and
 *   acknowledge the interrupt.  NOTE: during the bx_IAC call,
 *   the PIC will first call external_set_INTR(0).  If there are
 *   other valid interrupts waiting, then a call to external_set_INTR(1),
 *   will be made, all before the return of bx_IAC().
 */
void    external_set_enable_a20(Bit8u value);
Boolean external_get_enable_a20(void);
void    external_reset_cpu(void);
void    external_set_INTR(Boolean value);

/* The following prototypes represent functions provided by Bochs.
 *
 * bx_bochs_init(argc, argv):
 *   Call this with the conventional main(int argc, char *argv[])
 *   arguments.  These should be the same arguments as Bochs would
 *   normally require to its main() when running standalone.
 *   Ex:
 *
 *     int argc = 2;
 *     char *argv[2] = {"bochs", "-bootC" };
 *
 *     bx_bochs_init(argc, argv)
 * bx_inp(addr, io_len)
 *   takes a 16bit address and io_len==1/2/4, and returns an 8/16/32 bit
 *   quantity depending upon the value of io_len.  Since bx_inp() returns
 *   a 32bit type, upper bits of the return value for 8/16 bit quantities
 *   will be zero.
 * bx_outp(addr, value, io_len)
 *   takes a 16bit address, io_len==1/2/4, and a value of 8/16/32 bits
 *   depending upon io_len.  Since value is a 32bit type, upper bits for
 *   8/16 bit quantities are unused.
 * bx_IAC():
 *   call this when CPU services INT request.  The interrupt vector
 *   will be returned from the PIC.  See note on external_set_INTR() above.
 * bx_TICK():
 *   call this once every instruction.
 * bx_hga_set_video_memory(ptr):
 *   Since memory is handled by the foreign environment, call this
 *   periodically to give Bochs a current dump of HGA video memory.
 *   The buffer passed should be at least 64K in size, and represent
 *   the physical memory locations B0000..BFFFF.
 */
extern         int     bx_bochs_init(int argc, char *argv[]);
extern         Bit32u  bx_inp(Bit16u addr, unsigned io_len);
extern         void    bx_outp(Bit16u addr, Bit32u value, unsigned io_len);
extern         Bit8u   bx_IAC(void);
extern inline  void    bx_TICK(void);
extern         void    bx_hga_set_video_memory(Bit8u *ptr);
