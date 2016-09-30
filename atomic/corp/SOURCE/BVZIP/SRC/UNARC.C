
// Common includes for Pre-compiled Header
// This should go before any other includes or definitions
#ifdef SYM_BUILD_SM
#include "fsv_pch.h"
#else
#include "..\inc\fsv_pch.h"
#endif
#pragma hdrstop
// Put all other includes after this


#define DLE 0x90

typedef unsigned char byte;

typedef __int16 node[2];

typedef struct {
  __int16    bitoffset;
  __int16    numnodes;
  __int16    outbyte;
  __int16    count;
  __int16    last_was_DLE;
  __int16    rle_count;
  __int16    lastbyte;
  __int16    nextbyte;
 } huffman_common;

typedef struct {
  huffman_common save;
  __int16            entry_point;
  node           table[256];
 } huffman_vars;

        /*  --- type definitions for uncrunch ---  */

typedef struct {
          char          used;
          __int16           next;
          __int16           prefix;
          unsigned char suffix;
         }  entry;

        /*  --- constants for uncrunch ---  */

#define TABSIZE      4096

typedef struct {
  __int16    stackp;
  __int16    code_count;
  __int16    newcode, code, oldcode;
  __int16    oldbyte;
  __int16    last_was_DLE;
  __int16    rle_flag;
  __int16    rle_count;
  __int16    lastbyte;
  __int16    tempbyte;
  __int16    newhash;
 } crunch_common;

typedef struct {
  crunch_common save;
  __int16           entry_point;
  unsigned char stack[TABSIZE];
  entry         string_tab[TABSIZE];
 } crunch_vars;


void copy_struct3(void far *dest, void far *src, unsigned size);
unsigned WINAPI expand_huffman_size(void);
void WINAPI init_expand_huffman(huffman_vars far *vars);
int WINAPI expand_huffman(byte far *inbuffer, unsigned short far *input_size,
                              byte far *outbuffer, unsigned short far *output_size,
                              huffman_vars far *vars, char final);
unsigned WINAPI uncrunch_size(void);
void WINAPI add_code(__int16 pred, __int16 foll, __int16 newhash, entry far *table);
void WINAPI init_uncrunch(crunch_vars far *vars, char newhash, char rle_flag);
int WINAPI uncrunch(byte far *inbuffer, unsigned short far *input_size,
                        byte far *outbuffer, unsigned short far *output_size,
                        crunch_vars far* vars, char final);
                              

#ifdef SYM_BUILD_SM
#include "wnfsv.seg"
#else
#include "..\inc\wnfsv.seg"
#endif

void copy_struct3(void far *dest, void far *src, unsigned size)
{
#ifdef WIN32
  memcpy( dest, src, size );
#else
  if (size & 1) {
    *((char far *)dest)++ = *((char far *)src)++;
   }
  size >>= 1;
  while (size--) *((__int16 far *)dest)++ = *((__int16 far *)src)++;
#endif
}

/* ------ Huffman Squeezing ------ */

unsigned WINAPI expand_huffman_size(void)
 {
  return sizeof(huffman_vars);
 }

void WINAPI init_expand_huffman(huffman_vars far *vars)
 {
  vars->save.bitoffset = 8;
  vars->save.last_was_DLE = 0;
  vars->table[0][0] = - 257;
  vars->table[0][1] = - 257;
  vars->save.count = 0;
  vars->entry_point = 0;
 }

int WINAPI expand_huffman(byte far *inbuffer, unsigned short far *input_size,
                              byte far *outbuffer, unsigned short far *output_size,
                              huffman_vars far *vars, char final)
 {
  __int16            i;
  unsigned       in_size;
  unsigned       out_size;
  huffman_common local;
  char far       *pt;


  in_size = *input_size;
  out_size = *output_size;
  copy_struct3(&local, &vars->save, sizeof(local));
  switch (vars->entry_point) {
    case 1: goto entry1;
    case 2: goto entry2;
    case 3: goto entry3;
    case 4: goto entry4;
   }
  if (in_size < sizeof(__int16)) {
    *output_size = 0;
    return 1;
   }
  local.numnodes = *((__int16 *)inbuffer)++;
  in_size -= sizeof(__int16);
  if ((local.numnodes >= 0) && (local.numnodes <= 256)) {
entry1:
    i = local.numnodes * sizeof(node) - local.count;
    if ((unsigned)i > in_size) i = in_size;
    in_size -= i;
    pt = (char far *) vars->table;
    while (i--)
      pt[local.count++] = *inbuffer++;
    if ((unsigned)local.count < local.numnodes * sizeof(node)) {
      if (final) {
        *output_size = 0;
        return 1;
       }
      vars->entry_point = 1;
      goto exit;
     }
entry2:
    do {
      local.outbyte = 0;
      while (local.outbyte >= 0 && local.outbyte <= local.numnodes) {
        if (++local.bitoffset > 7) {
entry3:
          if (in_size--) {
            local.nextbyte = *inbuffer++;
            local.bitoffset = 0;
            local.outbyte = vars->table[local.outbyte][1 & local.nextbyte];
           } else if (final) {
            local.outbyte = -257;
           } else {
            in_size = 0;
            vars->entry_point = 3;
            goto exit;
           }
         } else {
          local.outbyte = vars->table[local.outbyte][1 & (local.nextbyte >>= 1)];
         }
       }
      if (local.outbyte > 0)
   local.outbyte = 257;
       else
        local.outbyte = -(local.outbyte + 1);
      if (local.outbyte < 256) {
        if (local.last_was_DLE) {
          if ((local.rle_count = local.outbyte) == 0) {
            *outbuffer++ = DLE;
            out_size--;
           } else {
            local.rle_count--;
entry4:
            if ((unsigned)local.rle_count > out_size) {
              local.rle_count -= out_size;
              do {
                *outbuffer++ = (char) local.lastbyte;
               } while (--out_size);
              vars->entry_point = 4;
              goto exit;
             } else {
              out_size -= local.rle_count;
              do {
                *outbuffer++ = (char) local.lastbyte;
               } while (--local.rle_count);
             }
           }
          local.last_was_DLE = 0;
         } else {
          if (local.outbyte == DLE) {
            local.last_was_DLE = 1;
           } else {
            *outbuffer++ = (char) local.lastbyte = (char)local.outbyte;
            out_size--;
           }
         }
       }
     } while (local.outbyte < 256 && out_size);
    if (local.outbyte < 256 && (in_size || !final)) {
      vars->entry_point = 2;
exit:
      *input_size -= in_size;
      *output_size -= out_size;
      copy_struct3(&vars->save, &local, sizeof(local));
      return 0;
     }
   }
  *output_size -= out_size;
  return 1;
 }

/* ----- Lemel - Zev coding - first (silly!) version ----- */
/* ----- 'crunched' ----- */

unsigned WINAPI uncrunch_size(void)
 {
  return sizeof(crunch_vars);
 }

void WINAPI add_code(__int16 pred, __int16 foll, __int16 newhash, entry far *table)
 {
  long local;
  __int16  n;
  __int16  next;

  if (newhash) {
    local = pred + foll;
    n = (__int16)((local * 15073) & 0xFFF);
   } else {
    local = ((pred + foll) | 0x800) & 0xffff;
    n = (__int16)(((local * local) >> 6) & 0xfff);
   }
  if (table[n].used) {
    next = table[n].next;
    while (next > 0) {
      n = next;
      next = table[n].next;
     }
    next = (n + 101) & 0xFFF;
    while (table[next].used) {
      next++;
      if (next == 4096)
        next = 0;
     }
    table[n].next = next;
    n = next;
   }
  table[n].used = 1;
  table[n].next = 0;
  table[n].prefix = pred;
  table[n].suffix = (byte)foll;
 }

void WINAPI init_uncrunch(crunch_vars far *vars, char newhash, char rle_flag)
 {
  __int16 i;

  vars->save.last_was_DLE = 0;
  i = TABSIZE;
  while (i--) {
    vars->string_tab[i].suffix = 0;
    vars->string_tab[i].next = 0;
    vars->string_tab[i].used = 0;
   }
  for (i = 0; i < 256; i++) {
    add_code(-1, i, newhash, vars->string_tab);
   }
  vars->save.tempbyte = -1;
  vars->save.code_count = TABSIZE - 256;
  vars->save.stackp = 0;
  vars->save.rle_flag = rle_flag;
  vars->save.newhash = newhash;
  vars->save.oldcode = -1;
  vars->entry_point = 0;
 }

int WINAPI uncrunch(byte far *inbuffer, unsigned short far *input_size,
                        byte far *outbuffer, unsigned short far *output_size,
                        crunch_vars far* vars, char final)
 {
  __int16           i;
  crunch_common local;
  unsigned      in_size;
  unsigned      out_size;

  in_size = *input_size;
  out_size = *output_size;
  copy_struct3(&local, &vars->save, sizeof(local));
  switch (vars->entry_point) {
    case 1: goto entry1;
    case 2: goto entry2;
    case 3: goto entry3;
    case 4: goto entry4;
   }
  do {
entry1:
    if (!in_size--) {
      in_size = 0;
      if (final) break;
      vars->entry_point = 1;
      goto exit;
     }
    if (local.tempbyte == -1) {
      local.tempbyte = *inbuffer++;
entry2:
      if (!in_size--) {
        in_size = 0;
        if (final) break;
        vars->entry_point = 2;
        goto exit;
       }
      local.newcode = (local.tempbyte << 4) | (*inbuffer >> 4);
      local.tempbyte = *inbuffer++ & 0xF;
     } else {
      local.newcode = *inbuffer++ | (local.tempbyte << 8);
      local.tempbyte = -1;
     }
    i = local.newcode;
    local.stackp = 0;
    if (!vars->string_tab[i].used) {
      i = local.oldcode;
      vars->stack[local.stackp++] = (byte)local.oldbyte;
     }
    if (!vars->string_tab[i].used) {
      in_size = 0;
      final = 1;
      break;
     } else {
      while (vars->string_tab[i].prefix != -1) {
        vars->stack[local.stackp++] = vars->string_tab[i].suffix;
        i = vars->string_tab[i].prefix;
       }
     }
    vars->stack[local.stackp++] = (byte)local.oldbyte = vars->string_tab[i].suffix;
entry3:
    if (local.rle_flag) {
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
     } else {
      do {
        *outbuffer++ = vars->stack[--local.stackp];
       } while (--out_size && local.stackp);
     }
    if (local.stackp) {            /* ask for more output space */
      vars->entry_point = 3;
      goto exit;
     }
    if (local.code_count && local.oldcode != -1) {
      add_code(local.oldcode, local.oldbyte, local.newhash, vars->string_tab);
      if (!vars->string_tab[local.newcode].used) {
        i = 0;
        break;
       }
      local.code_count--;
     }
    local.oldcode = local.newcode;
   } while (out_size);
  if (in_size || !final) {
    vars->entry_point = 0;
exit:
    *input_size -= in_size;
    *output_size -= out_size;
    copy_struct3(&vars->save, &local, sizeof(local));
    return 0;
   }
  *output_size -= out_size;
  return 1;
 }
