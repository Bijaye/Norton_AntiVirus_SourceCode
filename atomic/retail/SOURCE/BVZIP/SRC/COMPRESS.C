/* ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ» */
/* º Program     :  Compress.C                     º */
/* º Release     :  2.51                           º */
/* º Description :  RLE/LZW/Crush compression      º */
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
#define HashMax      4096
#define Init_Bits    1
#define MAX_BITS     13
#define HSIZE        8192
#define MaxCode      7936
#define MaxLess1     7935
#define FIRST        257
#define CLEAR        256
#define History_Size 500

typedef unsigned char byte;

#define Hash(c, n) (((c << 3) + n) & (HashMax - 1))

typedef struct {
  __int16          SeqCount;
  __int16          binary_mode;
  __int16          Hptr;
  __int16          code;
  __int16          nextbyte;
  __int16          oldcode;
  __int16          oldbyte;
  __int16          lastbyte;
  __int16          count;
  __int16          bits;
  __int16          CodeCount;
  __int16          robbin;
  __int16          limit;
  __int16          sp;
  __int16          bitoffset;
  __int16          rle_stack[3];
 } Crush_common;

typedef struct {
  Crush_common save;
  __int16          entry_point;
  __int16          outbyte;
  byte         History[History_Size];
  byte         LRU[MaxCode];
  __int16          HashTable[HashMax];
  __int16          Hnext[MaxCode];
  __int16          prefix[MaxCode];
  byte         suffix[MaxCode];
 } Crush_vars;

typedef struct {
  __int16     code;
  __int16     nextbyte;
  __int16     lastbyte;
  __int16     rle_flag;
  __int16     n_bits;
  __int16     maxcode;
  __int16     free_ent;
  __int16     maxcodemax;
  __int16     sizex;
  __int16     bitoffset;
  __int16     sp;
  __int16     count;
  __int16     rle_stack[3];
  byte   *buf;
 } LZW_common;

typedef struct {
  LZW_common save;
  __int16        entry_point;
  __int16        prefix[HSIZE];
  byte       suffix[HSIZE];
  __int16        Hnext[HSIZE];
  __int16        HashTable[HashMax];
  byte       buffer_save[MAX_BITS];
 } LZW_vars;



unsigned WINAPI LZWworkSize(void);
void WINAPI initCrush(Crush_vars *vars);
int WINAPI Crush(byte *inbuffer, unsigned short *input_size,
                     byte *outbuffer, unsigned short *output_size,
                     Crush_vars *vars, char final);
void copy_struct1(void *dest, void *src, unsigned size);
void WINAPI initCrush(Crush_vars *vars);
int WINAPI LZW(byte *inbuffer, unsigned short *input_size,
                   byte *outbuffer, unsigned short *output_size,
                   LZW_vars *vars, char final);
void WINAPI initLZW(LZW_vars *vars, __int16 bits, char rle_flag);
void WINAPI initRLE(void);
int WINAPI RLE(byte *inbuffer, unsigned short *input_size,
                   byte *outbuffer, unsigned short *output_size,
                   char final);
unsigned WINAPI CrushWorkSize(void);


#ifdef SYM_BUILD_SM
#include "wnfsv.seg"
#else
#include "..\inc\wnfsv.seg"
#endif

void copy_struct1(void *dest, void *src, unsigned size)
{
#ifdef WIN32
  memcpy( dest, src, size );
#else
  if (size & 1)
    *((char *)dest)++ = *((char *)src)++;
  size >>= 1;
  while (size--)
    *((__int16 *)dest)++ = *((__int16 *)src)++;
#endif
}

static struct {
  __int16     lastbyte;
  __int16     count;
 } rle_vars;


void WINAPI initRLE(void)
 {
  rle_vars.lastbyte = -1;
  rle_vars.count = 0;
 }


int WINAPI RLE(byte *inbuffer, unsigned short *input_size,
                   byte *outbuffer, unsigned short *output_size,
                   char final)
 {
  unsigned in_size;
  unsigned out_size;
  __int16      nextbyte;

  in_size = *input_size;
  out_size = *output_size;
  while (out_size > 3 && in_size) {
    in_size--;
    if ((nextbyte = *inbuffer++) == rle_vars.lastbyte) {
      rle_vars.count++;
     } else {
      if (rle_vars.count) {
        if (rle_vars.count > 2) {
          *outbuffer++ = DLE;
          *outbuffer++ = rle_vars.count + 1;
          out_size -= 2;
         } else {
          out_size -= rle_vars.count;
          *outbuffer++ = (byte)rle_vars.lastbyte;
          if (rle_vars.count > 1)
            *outbuffer++ = (byte)rle_vars.lastbyte;
         }
        rle_vars.count = 0;
       }
      if (nextbyte == DLE) { /* DLE becomes DLE 0 */
        *outbuffer++ = DLE;
        *outbuffer++ = 0;
        out_size -= 2;
        rle_vars.lastbyte = -1;
       } else {
        *outbuffer++ = (byte)rle_vars.lastbyte = (byte)nextbyte;
        out_size--;
       }
     }
   }
  *input_size -= in_size;
  if (in_size || !final) {
    *output_size -= out_size;
    return 0;
   }
  if (rle_vars.count) {
    if (rle_vars.count > 2) {
      *outbuffer++ = DLE;
      *outbuffer++ = rle_vars.count + 1;
      out_size -= 2;
     } else {
      out_size -= rle_vars.count;
      *outbuffer++ = (byte)rle_vars.lastbyte;
      if (rle_vars.count > 1)
        *outbuffer++ = (byte)rle_vars.lastbyte;
     }
    rle_vars.count = 0;
   }
  *output_size -= out_size;
  return 1;
 }


unsigned WINAPI LZWworkSize(void)
 {
  return sizeof(LZW_vars);
 }

void WINAPI initLZW(LZW_vars *vars, __int16 bits, char rle_flag)
 {
  __int16 i;

  vars->save.sp = vars->save.bitoffset = 0;
  vars->save.maxcodemax = 1 << bits;
  vars->save.n_bits = 9;
  vars->save.maxcode = (1 << 9) - 1;
  vars->save.sizex = 9 << 3;
  vars->save.rle_flag = rle_flag;
  i = 256;
  while (i--) {
    vars->prefix[i] = 0;
    vars->suffix[i] = (byte)i;
   }
  i = HashMax;
  while (i--) vars->HashTable[i] = 0;
  vars->save.free_ent = FIRST;
  vars->entry_point = 0;
  vars->buffer_save[0] = 0;
 }

int WINAPI LZW(byte *inbuffer, unsigned short *input_size,
                   byte *outbuffer, unsigned short *output_size,
                   LZW_vars *vars, char final)
 {
  LZW_common    local;
  unsigned      in_size;
  unsigned      out_size;
  __int16           i;
  __int16           h;
  __int16           outcode;
  __int16           bp;
  __int16           r_off;

  in_size = *input_size;
  out_size = *output_size;
  if (out_size <= MAX_BITS) goto exit_LZW_2;
  copy_struct1(&local, &vars->save, sizeof(local));
  i = MAX_BITS;
  local.buf = outbuffer;
  while (i--) local.buf[i] = vars->buffer_save[i];

  switch (vars->entry_point) {
    case 0 : goto entry0;
    case 1 : goto entry1;
    case 2 : goto entry2;
    case 3 : goto entry3;
    case 4 : goto entry4;
    case 5 : goto entry5;
    case 6 : goto entry6;
    case 7 : goto entry7;
    case 8 : goto entry8;
    case 9 : goto entry9;
   }

entry0:
  local.nextbyte = local.lastbyte = ((in_size--) ? *inbuffer++ : -1);

  while (local.nextbyte >= 0) {
    if (local.free_ent == local.maxcodemax) {
      local.code = CLEAR;
     } else {
      local.code = local.nextbyte;
      if (local.rle_flag) {  /* get a byte, run length-encoded */
        if (local.sp > 0) {  /* if we have a byte waiting, use that */
          local.nextbyte = local.rle_stack[--local.sp];
         } else {
entry1:
          if (local.lastbyte == DLE) { /* DLE becomes DLE 0 */
            local.nextbyte = 0;
            local.lastbyte = -1;
           } else if (in_size--) {
            if ((local.nextbyte = *inbuffer++) == local.lastbyte) {
              local.count = 1;
              do {   /* count identical bytes */
                local.count++;
entry2:
                if (in_size--) {
                  local.nextbyte = *inbuffer++;
                 } else if (final) {
                  local.nextbyte = -1;
                  in_size = 0;
                  break;
                 } else {
                  vars->entry_point = 2;
                  in_size = 0;
                  goto exit_LZW;
                 }
               } while (local.lastbyte == local.nextbyte && local.count < 255);
              if (local.count > 3) {
                local.rle_stack[1] = local.count;
                local.rle_stack[0] = local.lastbyte = local.nextbyte;
                local.nextbyte = DLE;
                local.sp = 2;
               } else {
                local.rle_stack[0] = local.nextbyte;
                if (local.count < 3) {
                  local.sp = 1;
                 } else {
                  local.rle_stack[1] = local.lastbyte;
                  local.sp = 2;
                 }
                local.nextbyte = local.lastbyte;
                local.lastbyte = local.rle_stack[0];
               }
              /* end of run logic */
             } else {
              local.lastbyte = local.nextbyte;
             }
           } else if (final) { /* true end of data */
            local.nextbyte = -1;
            in_size = 0;
           } else {
            vars->entry_point = 1;
            in_size = 0;
            goto exit_LZW;
           }
         }
       } else {
entry3:
        if (in_size--) {
          local.nextbyte = *inbuffer++;
         } else if (final) {
          local.nextbyte = -1;
          in_size = 0;
         } else {
          vars->entry_point = 3;
          in_size = 0;
          goto exit_LZW;
         }
       }
      h = vars->HashTable[Hash(local.code, local.nextbyte)];
      if (h) do {
        if (vars->prefix[h] != local.code ||
            (__int16)vars->suffix[h] != local.nextbyte) {
          h = vars->Hnext[h];
         } else {
          local.code = h;
          if (local.rle_flag) {  /* get a byte, run length-encoded */
            if (local.sp > 0) {  /* if we have a byte waiting, use that */
              local.nextbyte = local.rle_stack[--local.sp];
             } else {
              if (local.sp > 0) {  /* if we have a byte waiting, use that */
                local.nextbyte = local.rle_stack[--local.sp];
               } else {
entry4:
                if (local.lastbyte == DLE) { /* DLE becomes DLE 0 */
                  local.nextbyte = 0;
                  local.lastbyte = -1;
                 } else if (in_size--) {
                  if ((local.nextbyte = *inbuffer++) == local.lastbyte) {
                    local.count = 1;
                    do {   /* count identical bytes */
                      local.count++;
entry5:
                      if (in_size--) {
                        local.nextbyte = *inbuffer++;
                       } else if (final) {
                        local.nextbyte = -1;
                        in_size = 0;
                        break;
                       } else {
                        vars->entry_point = 5;
                        in_size = 0;
                        goto exit_LZW;
                       }
                     } while (local.lastbyte == local.nextbyte && local.count < 255);
                    if (local.count > 3) {
                      local.rle_stack[1] = local.count;
                      local.rle_stack[0] = local.lastbyte = local.nextbyte;
                      local.nextbyte = DLE;
                      local.sp = 2;
                     } else {
                      local.rle_stack[0] = local.nextbyte;
                      if (local.count < 3) {
                        local.sp = 1;
                       } else {
                        local.rle_stack[1] = local.lastbyte;
                        local.sp = 2;
                       }
                      local.nextbyte = local.lastbyte;
                      local.lastbyte = local.rle_stack[0];
                     }
                    /* end of run logic */
                   } else {
                    local.lastbyte = local.nextbyte;
                   }
                 } else if (final) { /* true end of data */
                  local.nextbyte = -1;
                  in_size = 0;
                 } else {
                  vars->entry_point = 4;
                  in_size = 0;
                  goto exit_LZW;
                 }
               }
             } /* end of RLE */
           } else {
entry6:
            if (in_size--) {
              local.nextbyte = *inbuffer++;
             } else if (final) {
              local.nextbyte = -1;
              in_size = 0;
              break;
             } else {
              vars->entry_point = 6;
              in_size = 0;
              goto exit_LZW;
             }
           }
          h = vars->HashTable[Hash(local.code, local.nextbyte)];
         }
       } while (h);
     }
    if (local.bitoffset >= local.sizex) {
      local.buf += local.n_bits;         /* next chunk of buffer */
      out_size -= local.n_bits;
      local.bitoffset = 0;
entry7:
      if (out_size <= MAX_BITS) { /* check if we have enough buffer */
        vars->entry_point = 7;
        goto exit_LZW;
       }
      local.buf[0] = 0;
     }
    bp = local.bitoffset >> 3;
    local.buf[bp++] |= (local.code << (r_off = local.bitoffset & 7));
    outcode = local.code >> (8 - r_off);
    if (local.n_bits + r_off >= 16) {
      local.buf[bp++] = (byte)outcode;
      outcode >>= 8;
     }
    local.buf[bp] = (byte)outcode;
    local.bitoffset += local.n_bits;
    if (local.code == CLEAR) {
      local.buf += local.n_bits;
      out_size -= local.n_bits;
      local.bitoffset = 0;
entry8:
      if (out_size <= MAX_BITS) {
        vars->entry_point = 8;
        goto exit_LZW;
       }
      local.buf[0] = 0;
      local.n_bits = 9;
      i = HashMax;
      while (i--) vars->HashTable[i] = 0;
      local.free_ent = FIRST;
      local.sizex = local.n_bits << 3;
      local.maxcode = (1 << local.n_bits) - 1;
     } else if (local.free_ent < local.maxcodemax) {
      vars->prefix[local.free_ent] = local.code;
      vars->suffix[local.free_ent] = (byte)local.nextbyte;
      h = Hash(local.code, local.nextbyte);
      vars->Hnext[local.free_ent] = vars->HashTable[h];
      vars->HashTable[h] = local.free_ent;
      if (local.free_ent++ > local.maxcode) {
        local.buf += local.n_bits;
        out_size -= local.n_bits;
   local.bitoffset = 0;
entry9:
        if (out_size <= MAX_BITS) {
          vars->entry_point = 9;
          goto exit_LZW;
         }
   local.buf[0] = 0;
        local.maxcode = (1 << ++local.n_bits) - 1;
   local.sizex = local.n_bits << 3;
       }
     }
   }
  out_size -= (local.bitoffset + 7) >> 3;
  *output_size -= out_size;
  return 1;
exit_LZW:
  i = (local.bitoffset >> 3) + 1;
  if (i > MAX_BITS)
    i = MAX_BITS;
  while (i--) vars->buffer_save[i] = local.buf[i];
  copy_struct1(&vars->save, &local, sizeof(local));
exit_LZW_2:
  *input_size -= in_size;
  *output_size -= out_size;
  return 0;
 }

unsigned WINAPI CrushWorkSize(void)
 {
  return sizeof(Crush_vars);
 }

void WINAPI initCrush(Crush_vars *vars)
 {
  __int16 i;

  vars->save.Hptr = vars->save.SeqCount = vars->save.sp =
     vars->save.CodeCount = 0;
  vars->save.binary_mode = 1;

  vars->save.robbin = 1;
  vars->save.bits = Init_Bits;
  vars->save.limit = 1 << Init_Bits;

  i = History_Size;
  while (i--)
    vars->History[i] = 0;
  i = HashMax;
  while (i--)
    vars->HashTable[i] = 0;

  vars->save.bitoffset = 0;
  vars->outbyte = 0;
  vars->entry_point = 0;
  }


int WINAPI Crush(byte *inbuffer, unsigned short *input_size,
                     byte *outbuffer, unsigned short *output_size,
                     Crush_vars *vars, char final)
 {

  __int16          h, last;
  __int16          NewCode;
  __int16          i;
  __int16          outcode;
  __int16          outbits;

  Crush_common local;
  unsigned     out_size;
  unsigned     in_size;


  in_size = *input_size;
  out_size = *output_size;
  *outbuffer = (byte)vars->outbyte;
  copy_struct1(&local, &vars->save, sizeof(local));

  switch (vars->entry_point) {
    case 0 : goto entry0;
    case 1 : goto entry1;
    case 2 : goto entry2;
    case 3 : goto entry3;
    case 4 : goto entry4;
    case 5 : goto entry5;
    case 6 : goto entry6;
    case 7 : goto entry7;
    case 8 : goto entry8;
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
  if ((local.nextbyte = local.lastbyte =
       ((in_size--) ? *inbuffer++ : -1)) >= 0) {
    local.code = - local.nextbyte - 1;
    *outbuffer++ |= local.nextbyte << 1;
    out_size--;
    local.bitoffset = 1;
    *outbuffer = local.nextbyte >> 7;
entry1:
    if (local.lastbyte == DLE) { /* DLE becomes DLE 0 */
      local.nextbyte = 0;
      local.lastbyte = -1;
     } else if (in_size--) {
      if ((local.nextbyte = *inbuffer++) == local.lastbyte) {
                                    /* possible run */
        local.count = 1;
        do {   /* count identical bytes */
          local.count++;
entry2:
          if (in_size--) {
            local.nextbyte = *inbuffer++;
           } else {
            in_size = 0;
            if (final) {
              local.nextbyte = -1;
              break;
             } else {
              vars->entry_point = 2;
              goto exit_Crush;
             }
           }
         } while (local.lastbyte == local.nextbyte && local.count < 255);
        if (local.count > 3) {
          local.rle_stack[1] = local.count;
          local.rle_stack[0] = local.lastbyte = local.nextbyte;
          local.nextbyte = DLE;
          local.sp = 2;
         } else {
          local.rle_stack[0] = local.nextbyte;
          if (local.count < 3) {
            local.sp = 1;
           } else {
            local.rle_stack[1] = local.lastbyte;
            local.sp = 2;
           }
          local.nextbyte = local.lastbyte;
          local.lastbyte = local.rle_stack[0];
         }
        /* end of run logic */
       } else {
        local.lastbyte = local.nextbyte;
       }
     } else if (final) { /* true end of data */
      local.nextbyte = -1;
      in_size = 0;
     } else {
      vars->entry_point = 1;
      in_size = 0;
      goto exit_Crush;
     }
   }
  while (local.nextbyte >= 0) {
    local.oldcode = local.code;
    local.code = -local.nextbyte - 1;
    local.oldbyte = local.nextbyte;
    if (local.sp > 0) {  /* if we have a byte waiting, use that */
      local.nextbyte = local.rle_stack[--local.sp];
     } else {
entry3:
      if (local.lastbyte == DLE) { /* DLE becomes DLE 0 */
        local.nextbyte = 0;
        local.lastbyte = -1;
       } else if (in_size--) {
        if ((local.nextbyte = *inbuffer++) == local.lastbyte) {
          local.count = 1;
          do {   /* count identical bytes */
            local.count++;
entry4:
            if (in_size--) {
              local.nextbyte = *inbuffer++;
             } else if (final) {
              local.nextbyte = -1;
              in_size = 0;
             } else {
              vars->entry_point = 4;
              in_size = 0;
              goto exit_Crush;
             }
           } while (local.lastbyte == local.nextbyte && local.count < 255);
          if (local.count > 3) {
            local.rle_stack[1] = local.count;
            local.rle_stack[0] = local.lastbyte = local.nextbyte;
            local.nextbyte = DLE;
            local.sp = 2;
           } else {
            local.rle_stack[0] = local.nextbyte;
            if (local.count < 3) {
              local.sp = 1;
             } else {
              local.rle_stack[1] = local.lastbyte;
              local.sp = 2;
             }
            local.nextbyte = local.lastbyte;
            local.lastbyte = local.rle_stack[0];
           }
          /* end of run logic */
         } else {
          local.lastbyte = local.nextbyte;
         }
       } else if (final) { /* true end of data */
        local.nextbyte = -1;
        in_size = 0;
       } else {
        vars->entry_point = 3;
        in_size = 0;
        goto exit_Crush;
       }
     }

    h = vars->HashTable[Hash(local.code, local.nextbyte)];
    if (h) do {
      if (vars->prefix[h] != local.code ||
          (__int16)vars->suffix[h] != local.nextbyte) {
        h = vars->Hnext[h];
       } else {
   local.code = h;
        vars->LRU[local.code] = 4;
        if (local.sp > 0) {  /* if we have a byte waiting, use that */
          local.nextbyte = local.rle_stack[--local.sp];
         } else {
entry5:
          if (local.lastbyte == DLE) { /* DLE becomes DLE 0 */
            local.nextbyte = 0;
            local.lastbyte = -1;
           } else if (in_size--) {
            if ((local.nextbyte = *inbuffer++) == local.lastbyte) {
              local.count = 1;
              do {   /* count identical bytes */
                local.count++;
entry6:
                if (in_size--) {
                  local.nextbyte = *inbuffer++;
                 } else if (final) {
                  local.nextbyte = -1;
                  in_size = 0;
                  break;
                 } else {
                  vars->entry_point = 6;
                  in_size = 0;
                  goto exit_Crush;
                 }
               } while (local.lastbyte == local.nextbyte && local.count < 255);
              if (local.count > 3) {
                local.rle_stack[1] = local.count;
                local.rle_stack[0] = local.lastbyte = local.nextbyte;
                local.nextbyte = DLE;
                local.sp = 2;
               } else {
                local.rle_stack[0] = local.nextbyte;
                if (local.count < 3) {
                  local.sp = 1;
                 } else {
                  local.rle_stack[1] = local.lastbyte;
                  local.sp = 2;
                 }
                local.nextbyte = local.lastbyte;
                local.lastbyte = local.rle_stack[0];
               }
              /* end of run logic */
             } else {
              local.lastbyte = local.nextbyte;
             }
           } else if (final) { /* true end of data */
            local.nextbyte = -1;
            in_size = 0;
            break;
           } else {
            vars->entry_point = 5;
            in_size = 0;
            goto exit_Crush;
           }
         }
   h = vars->HashTable[Hash(local.code, local.nextbyte)];
       }
     } while (h);

    if (vars->History[local.Hptr]) local.SeqCount--;
    vars->History[local.Hptr] = local.code > 0;
    if (local.code > 0) local.SeqCount++;
    local.Hptr++;
    if (local.Hptr >= History_Size) local.Hptr = 0;

entry7:
    if (out_size < 3) {
      vars->entry_point = 7;
      goto exit_Crush;
     }
    if (local.binary_mode) {
      if (local.code > 0) {
        outcode = (local.code << 1) + 1;
        outbits = local.bits + 1;
       } else {
        outcode = (-local.code - 1) << 1;
        outbits = 9;
       }
     } else {
      outcode = local.code + 256;
      outbits = local.bits;
     }

    *outbuffer |= outcode << local.bitoffset;
    local.bitoffset += outbits;
    while (local.bitoffset > 7) {
      out_size--;
      local.bitoffset -= 8;
      *++outbuffer = outcode >> (outbits - local.bitoffset);
     }

    if ((local.SeqCount < 375) ^ local.binary_mode) {
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
       }  while ((vars->LRU[i]) && (i != local.robbin)) ;
      local.robbin = i;
      last = Hash(vars->prefix[NewCode], vars->suffix[NewCode]);  /* delete hash */
      if ((h = vars->HashTable[last]) == NewCode) {
        vars->HashTable[last] = vars->Hnext[h];
       } else {
        do {
          last = h;
          h = vars->Hnext[h];
         } while (h != NewCode);
        vars->Hnext[last] = vars->Hnext[h];
       }
     } else {
      local.CodeCount++;
      NewCode = local.CodeCount;
      if (local.CodeCount >= local.limit) {
        local.bits++;
        local.limit = 1 << local.bits;
        if (!(local.binary_mode)) local.limit -= 256;
       }
     }

    vars->prefix[NewCode] = local.oldcode;
    vars->suffix[NewCode] = (byte)local.oldbyte;
    vars->LRU[NewCode] = 2;
    h = Hash(vars->prefix[NewCode], vars->suffix[NewCode]);
    vars->Hnext[NewCode] = vars->HashTable[h];
    vars->HashTable[h] = NewCode;
   }

entry8:
  if (out_size < 4) {
    vars->entry_point = 8;
    goto exit_Crush;
   }

  if (local.binary_mode) {
    outcode = 1;
    outbits = local.bits + 1;
   } else {
    outcode = 256;
    outbits = local.bits;
   }
  *outbuffer |= outcode << local.bitoffset;
  local.bitoffset += outbits;
  while (local.bitoffset > 7) {
    out_size--;
    local.bitoffset -= 8;
    *++outbuffer = outcode >> (outbits - local.bitoffset);
   }

  out_size -= (local.bitoffset + 7) >> 3;
  *output_size -= out_size;
  return 1;
exit_Crush:
  *input_size -= in_size;
  *output_size -= out_size;
  copy_struct1(&vars->save, &local, sizeof(local));
  vars->outbyte = *outbuffer;
  return 0;
 }
