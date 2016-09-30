/* ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ» */
/* º Program     :  Expand.C                       º */
/* º Release     :  2.51                           º */
/* º Description :  RLE/LZW/Crush expansion        º */
/* º                routines for PAK               º */
/* ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶ */
/* º Author      :  Gus Smedstad                   º */
/* º Last Updated:  10/08/90                       º */
/* ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶ */
/* º Notice     : This source code is NOT intended º */
/* º              for general distribution.  Care  º */
/* º              should be taken to preserve its  º */
/* º              confidentiality.                 º */
/* ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶ */
/* º Copyright (C) 1988-1991 by NoGate Consulting  º */
/* º             All rights reserved.              º */
/* ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼ */


// Common includes for Pre-compiled Header
// This should go before any other includes or definitions
#ifdef SYM_BUILD_SM
#include "fsv_pch.h"
#else
#include "..\inc\fsv_pch.h"
#endif
#pragma hdrstop
// Put all other includes after this

#define DLE          0x90
#define MaxCode      7936
#define MaxLess1     7935
#define HashMax      4096
#define Init_Bits    1

typedef unsigned char byte;

typedef struct {
  __int16 last_was_DLE;
  __int16 lastbyte;
  __int16 count;
 } rle_vars;

#define MAX_BITS  13
#define HSIZE        8192
#define FIRST        257
#define CLEAR        256
#define HSIZEM1      8191

typedef struct {
  __int16     stackp, oldbyte;
  __int16     code, lastcode;
  __int16     lastbyte;
  char    rle_flag;
  __int16     rle_count;
  __int16     last_was_DLE;
  __int16     n_bits, maxcode;
  __int16     maxcodemax;
  __int16     sizex;
  __int16     bitoffset;
  __int16     code_count;
  __int16     delta;
  byte   *buf;
  byte   *nxtptr;
 } LZW_common;

typedef struct {
  LZW_common save;
  __int16        entry_point;
  __int16        prefix[HSIZE];
  byte       suffix[HSIZE];
  byte       stack[HSIZE];
  byte       savebuffer[MAX_BITS];
  __int16        savecount;
  __int16        bits;
 } LZW_vars;


typedef struct {
  byte     bits;
  unsigned limit;
  byte     binary_mode;
  unsigned CodeCount;
  __int16      firstbyte;
  __int16      lastcode;
  __int16      code;
  unsigned robbin;
  __int16      stackp;
  __int16      SeqCount;
  __int16      Hptr;
  __int16      last_was_DLE;
  __int16      rle_count;
  __int16      lastbyte;
  __int16      bitoffset;
  __int16      compressed;
  __int16      inbits;
 } Crush_common;

typedef struct {
  Crush_common save;
  __int16          entry_point;
  byte         stack[MaxCode];
  __int16          prefix[MaxCode];
  byte         suffix[MaxCode];
  byte         LRU[MaxCode];
  byte         History[500];
 } Crush_vars;

void copy_struct2(void *dest, void *src, unsigned size);
void WINAPI initExpandRLE(void);
int WINAPI expandRLE(byte *inbuffer, unsigned short *input_size,
                          byte *outbuffer, unsigned short *output_size,
                          char final);
unsigned WINAPI expandLZWsize(void);
void WINAPI initExpandLZW(LZW_vars *vars, __int16 bits, char rle_flag);
unsigned WINAPI expandCrushSize(void);
void WINAPI initExpandCrush(Crush_vars *vars);
int WINAPI expandCrush(byte *inbuffer, unsigned short *input_size,
                            byte *outbuffer, unsigned short *output_size,
                            Crush_vars *vars, char final);
int WINAPI expandLZW(byte *inbuffer, unsigned short *input_size,
                          byte *outbuffer, unsigned short *output_size,
                          LZW_vars *vars, char final);


#ifdef SYM_BUILD_SM
#include "wnfsv.seg"
#else
#include "..\inc\wnfsv.seg"
#endif

void copy_struct2(void *dest, void *src, unsigned size)
{
#ifdef WIN32
  memcpy( dest, src, size );
#else
  if (size & 1) {
    *((char *)dest)++ = *((char *)src)++;
   }
  size >>= 1;
  while (size--) *((__int16 *)dest)++ = *((__int16 *)src)++;
#endif
}

/* ------- Run-Length coding ------ */

static rle_vars rle;

void WINAPI initExpandRLE(void)
 {
  rle.last_was_DLE = 0;
  rle.count = 0;
 }


int WINAPI expandRLE(byte *inbuffer, unsigned short *input_size,
                          byte *outbuffer, unsigned short *output_size,
                          char final)
 {
  __int16 b;
  unsigned in_size, out_size;

  in_size = *input_size;
  out_size = *output_size;

  if (rle.count) {
    if ( (unsigned)rle.count > out_size) {
      rle.count -= out_size;
      do {
        *outbuffer++ = (byte)rle.lastbyte;
       } while (--out_size);
     } else {
      out_size -= rle.count;
      do {
        *outbuffer++ = (byte)rle.lastbyte;
       } while (--rle.count);
     }
   }

  while (out_size && in_size) {
    in_size--;
    if (rle.last_was_DLE) {
      if ((rle.count = *inbuffer++) == 0) {
        *outbuffer++ = DLE;
        out_size--;
       } else {
        if ((unsigned)--rle.count > out_size) {
          rle.count -= out_size;
          do {
            *outbuffer++ = (byte)rle.lastbyte;
           } while (--out_size);
         } else {
          out_size -= rle.count;
          do {
            *outbuffer++ = (byte)rle.lastbyte;
           } while (--rle.count);
         }
       }
      rle.last_was_DLE = 0;
     } else {
      if ((b = *inbuffer++) == DLE) {
        rle.last_was_DLE = 1;
       } else {
        *outbuffer++ = (byte)rle.lastbyte = (byte)b;
        out_size--;
       }
     }
   }
  *input_size -= in_size;
  *output_size -= out_size;
  return (!in_size && final && !rle.count);
 }

unsigned WINAPI expandLZWsize(void)
 {
  return (sizeof(LZW_vars));
 }

void WINAPI initExpandLZW(LZW_vars *vars, __int16 bits, char rle_flag)
 {
  __int16 i;

  vars->save.bitoffset = 0;
  vars->save.last_was_DLE = 0;
  vars->save.maxcodemax = 1 << bits;
  vars->save.n_bits = 9;
  vars->bits = bits;
  vars->save.rle_flag = rle_flag;
  vars->save.last_was_DLE = 0;
  vars->save.maxcode = (1 << 9) - 1;
  i = 256;
  while (i--) {
    vars->prefix[i] = 0;
    vars->suffix[i] = (byte)i;
   }
  vars->save.stackp = 0;
  vars->entry_point = 0;
  vars->save.code = CLEAR;
  vars->save.delta = 0;
 }


int WINAPI expandLZW(byte *inbuffer, unsigned short *input_size,
                          byte *outbuffer, unsigned short *output_size,
                          LZW_vars *vars, char final)
 {
  unsigned char bitmask[9];
  LZW_common local;
  unsigned   in_size;
  unsigned   out_size;
  __int16        savecode;
  __int16        i;
  __int16        bitsx;
  __int16        r_off;

  in_size = *input_size;
  out_size = *output_size;
  copy_struct2(&local, &vars->save, sizeof(local));
  bitmask[8] = 0xff;
  i = 8;
  while (i--) bitmask[i] = bitmask[i+1] >> 1;

  if (vars->entry_point == 1) goto entry1;
  if (vars->entry_point == 2) goto entry2;
  if (vars->entry_point == 3) {
    local.buf = vars->savebuffer + vars->savecount;    /* copy into savebuffer */
    if (in_size < (unsigned)(local.n_bits - vars->savecount)) {
      vars->savecount += in_size;
      local.delta = i = in_size;
      while (i--) *local.buf++ = *inbuffer++;
      if (!final) {
        in_size = 0;
        goto exit;
       }
      local.sizex = vars->savecount;
     } else {
      local.nxtptr = inbuffer + (i = local.n_bits - vars->savecount);
      local.delta = i; /* fill up savebuffer */
      while (i--) *local.buf++ = *inbuffer++;
      local.sizex = local.n_bits;
     }
    local.buf = vars->savebuffer;
    goto entry3;
   }
  if (vars->entry_point == 0) {
    local.nxtptr = inbuffer;
   }
  while (out_size) {  /* actually, until out of in or out buffer */
    if (local.code == CLEAR) {
      i = 256;
      while (i--) vars->prefix[i] = 0;
      local.code_count = FIRST - 1;
      local.sizex = 0;
      local.n_bits = 9;
      local.maxcode = (1 << 9) - 1;
     } else {
      if ((savecode = local.code) >= local.code_count) {
        vars->stack[local.stackp++] = (byte)local.oldbyte;
        if ((local.code = local.lastcode) >= local.code_count) {
          in_size = 0;
          final = 1;
          break;   /* this is an error - compressed data is corrupt */
         }
       }
      if (local.code >= 256) do {
        vars->stack[local.stackp++] = vars->suffix[local.code];
       } while ((local.code = vars->prefix[local.code]) >= 256);
      vars->stack[local.stackp++] = (byte)local.oldbyte = (byte)vars->suffix[local.code];
      if (local.code_count < local.maxcodemax) {
        vars->prefix[local.code_count] = local.lastcode;
        vars->suffix[local.code_count] = (byte)local.oldbyte;
        if (++local.code_count > local.maxcode) {
          local.sizex = 0;
          if (++local.n_bits == vars->bits)
            local.maxcode = local.maxcodemax;
           else
            local.maxcode = (1 << local.n_bits) - 1;
         }
       }
      local.lastcode = savecode;
entry1:
      if (local.rle_flag) {
        do {
          if (local.last_was_DLE) {
            if ((local.rle_count = vars->stack[--local.stackp]) == 0) {
              *outbuffer++ = DLE;
              out_size--;
             } else {
              local.rle_count--;
entry2:
              if ((unsigned)local.rle_count > out_size) {
                local.rle_count -= out_size;
                do {
                  *outbuffer++ = (byte)local.lastbyte;
                 } while (--out_size);
                vars->entry_point = 2;
                goto exit;
               } else {
                out_size -= local.rle_count;
                do {
                  *outbuffer++ = (byte)local.lastbyte;
                 } while (--local.rle_count);
               }
             }
            local.last_was_DLE = 0;
           } else {
            if ((i = vars->stack[--local.stackp]) == DLE) {
              local.last_was_DLE = 1;
             } else {
              *outbuffer++ = (byte)local.lastbyte = (byte)i;
              out_size--;
             }
           }
         } while (local.stackp && out_size);
       } else {
        do {
          *outbuffer++ = vars->stack[--local.stackp];
         } while (--out_size && local.stackp);
       }
      if (local.stackp) {            /* ask for more output space */
        vars->entry_point = 1;
        goto exit;
       }
     }
    if (local.bitoffset >= local.sizex) {
      local.buf = local.nxtptr;  /* move buffer pointer up */
      if ((in_size -= local.delta) < (unsigned)local.n_bits) {
            /* less than a full block */
        if (final) {
          if (in_size < 2) {
            in_size = 0;
            break;  /* not enough to form another local.code */
           }
          local.sizex = in_size;
         } else {
          vars->entry_point = 3;
          vars->savecount = i = in_size;
          while (i--) vars->savebuffer[i] = local.buf[i];
          in_size = 0;
          goto exit; /* ask for more bytes */
         }
       } else {
        local.sizex = local.n_bits;
       }
      local.nxtptr = local.buf + local.sizex;
      local.delta = local.sizex;
entry3:
      local.bitoffset = 0;
      local.sizex = (local.sizex << 3) - local.n_bits + 1;
     }
    bitsx = local.n_bits;
    r_off = local.bitoffset & 7;
    local.code = *local.buf++ >> r_off;
    r_off = 8 - r_off;
    bitsx -= r_off;
    if (bitsx >= 8) {
      local.code |= (*local.buf++ << r_off);
      r_off += 8;
      bitsx -= 8;
     }
    local.code |= ((*local.buf & bitmask[bitsx]) << r_off);
    if (bitsx == 8) local.buf++;
    local.bitoffset += local.n_bits;
   }
  if (in_size || !final) {
    vars->entry_point = 4;
    goto exit;
   }
  *output_size -= out_size;
  return 1;
exit:
  *input_size -= in_size;
  *output_size -= out_size;
  copy_struct2(&vars->save, &local, sizeof(local));
  return 0;
 }


/*  ----- Lempel - Zev coding - improvements by Gus Smedstad ------ */
/*  ----- 'crushed' encoding ------ */


unsigned WINAPI expandCrushSize(void)
 {
  return (sizeof(Crush_vars));
 }

void WINAPI initExpandCrush(Crush_vars *vars)
 {
  __int16 i;

  i = 500;
  while (i--) vars->History[i] = 0;

  vars->save.SeqCount = vars->save.Hptr = 0;
  vars->save.stackp = 0;

  vars->save.bits = 1;
  vars->save.limit = 2;
  vars->save.binary_mode = 1;
  vars->save.CodeCount = 0;
  vars->save.lastcode = 0;
  vars->save.robbin = 1;
  vars->save.last_was_DLE = 0;
  vars->save.bitoffset = 0;
  vars->entry_point = 0;
 }

int WINAPI expandCrush(byte *inbuffer, unsigned short *input_size,
                            byte *outbuffer, unsigned short *output_size,
                            Crush_vars *vars, char final)
 {
  unsigned char bitmask[9];
  Crush_common local;
  unsigned     in_size;
  unsigned     out_size;
  __int16          NewCode;
  __int16          i;
  __int16          last;

  i = 8;
  bitmask[8] = 0xff;
  while (i--) bitmask[i] = bitmask[i+1] >> 1;
  in_size = *input_size;
  out_size = *output_size;
  copy_struct2(&local, &vars->save, sizeof(local));
  switch (vars->entry_point) {
    case 0: goto entry0;
    case 1: goto entry1;
    case 2: goto entry2;
    case 3: goto entry3;
    case 4: goto entry4;
    case 5: goto entry5;
    case 6: goto entry6;
   }
//  __emit__('(','C',')','1','9','8','8',' ','b','y',' ');
//  __emit__('N','o','G','a','t','e',' ');
//  __emit__('C','o','n','s','u','l','t','i','n','g');
//  __emit__('(' ^ 0xAA,'C' ^ 0xAA,')' ^ 0xAA,'1' ^ 0xAA,'9' ^ 0xAA,'8' ^ 0xAA);
//  __emit__('8' ^ 0xAA,' ' ^ 0xAA,'b' ^ 0xAA,'y' ^ 0xAA,' ' ^ 0xAA);
//  __emit__('N' ^ 0xAA,'o' ^ 0xAA,'G' ^ 0xAA,'a' ^ 0xAA,'t' ^ 0xAA,'e' ^ 0xAA);
//  __emit__(' ' ^ 0xAA,'C' ^ 0xAA,'o' ^ 0xAA,'n' ^ 0xAA,'s' ^ 0xAA,'u' ^ 0xAA);
//  __emit__('l' ^ 0xAA,'t' ^ 0xAA,'i' ^ 0xAA,'n' ^ 0xAA,'g' ^ 0xAA);
entry0:
  while (out_size) {
    local.inbits = local.bits;
    if (local.binary_mode) {
entry1:
      if (!in_size) {
        if (final) break;
        vars->entry_point = 1;
        goto exit;
       }
      if ((local.compressed = *inbuffer & (1 << local.bitoffset)) == 0)
        local.inbits = 8;
      if (++local.bitoffset == 8) {
        local.bitoffset = 0;
        inbuffer++;
        in_size--;
       }
     }
entry2:
    if (!in_size) {
      if (final) break;
      vars->entry_point = 2;
      goto exit;
     }
    if ((i = 8 - local.bitoffset) > local.inbits) {
      local.code = (*inbuffer >> local.bitoffset) & bitmask[local.inbits];
      local.bitoffset += local.inbits;
     } else {
      local.code = *inbuffer++ >> local.bitoffset;
      in_size--;
      local.bitoffset = local.inbits - i;
      local.inbits = i;
      if (local.bitoffset >= 8) {
entry5:
        if (!in_size) {
          if (final) break;
          vars->entry_point = 5;
          goto exit;
         }
        local.code |= (*inbuffer++ << local.inbits);
        in_size--;
        local.inbits += 8;
        local.bitoffset -= 8;
       }
      if (local.bitoffset) {
entry6:
        if (!in_size) {
          if (final) break;
          vars->entry_point = 6;
          goto exit;
         }
        local.code |= ((*inbuffer & bitmask[local.bitoffset]) << local.inbits);
       }
     }
    if (!local.binary_mode) {
      local.code -= 256;
     } else if (!local.compressed) {
      local.code = - local.code - 1;
     }
    if (!local.code) {
      in_size = 0;
      final = 1;
      break;
     }
    i = local.code;
    local.stackp = 0;
    if ((unsigned)i > local.CodeCount && i > 0)
      i = 0;
     else
      while (i > 0) {
        vars->LRU[i] = 4;
        vars->stack[local.stackp++] = vars->suffix[i];
        i = vars->prefix[i];
       }
    vars->stack[local.stackp++] = (byte)local.firstbyte = (byte)(- i - 1);
entry3:
    do {
      if (local.last_was_DLE) {
        if ((local.rle_count = vars->stack[--local.stackp]) == 0) {
          *outbuffer++ = DLE;
          out_size--;
         } else {
          local.rle_count--;
entry4:
          if ((unsigned)local.rle_count > out_size) {
            local.rle_count -= out_size;
            do {
              *outbuffer++ = (byte)local.lastbyte;
             } while (--out_size);
            vars->entry_point = 4;
            goto exit;
           } else {
            out_size -= local.rle_count;
            do {
              *outbuffer++ = (byte)local.lastbyte;
             } while (--local.rle_count);
           }
         }
        local.last_was_DLE = 0;
       } else {
        if ((i = vars->stack[--local.stackp]) == DLE) {
          local.last_was_DLE = 1;
         } else {
          *outbuffer++ = (byte)local.lastbyte = (byte)i;
          out_size--;
         }
       }
     } while (local.stackp && out_size);
    if (local.stackp) {            /* ask for more output space */
      vars->entry_point = 3;
      goto exit;
     }

    if (local.lastcode) {
      if (vars->History[local.Hptr]) local.SeqCount--;
      vars->History[local.Hptr] = local.code > 0;
      if (local.code > 0) local.SeqCount++;
      local.Hptr++;
      if (local.Hptr > 499) local.Hptr = 0;

      if ((local.SeqCount < 375) ^ (local.binary_mode)) {
        local.binary_mode = !(local.binary_mode);
        local.limit = 1 << local.bits;
        if (!(local.binary_mode)) local.limit -= 256;
       }

      if (local.CodeCount == MaxLess1) {
        i = local.robbin;
        last = 257;
        do {
          if (++i > MaxLess1) i = 1;
          if (vars->LRU[i] < (unsigned)last) {
            NewCode = i;
            last = vars->LRU[i];
           }
          vars->LRU[i]--;
         }  while ((vars->LRU[i]) && ((unsigned)i != local.robbin)) ;
        local.robbin = i;
       } else {
        NewCode = ++local.CodeCount;
        if (local.CodeCount >= local.limit) {
          local.bits++;
          local.limit = 1 << local.bits;
          if (!(local.binary_mode)) local.limit -= 256;
         }
       }
      vars->prefix[NewCode] = local.lastcode;
      vars->suffix[NewCode] = (byte)local.firstbyte;
      vars->LRU[NewCode] = 2;
     }
    local.lastcode = local.code;
   }
  if (in_size || !final) {
    vars->entry_point = 0;
exit:
    *input_size -= in_size;
    *output_size -= out_size;
    copy_struct2(&vars->save, &local, sizeof(local));
    return 0;
   }
  *input_size -= in_size;
  *output_size -= out_size;
  return 1;
 }
