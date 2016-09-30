/*
; ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»
; º Program     :  Explode.Asm                    º
; º Release     :  2.51                           º
; º Description :  Compression routine for PAK    º
; ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶
; º Author      :  Gus Smedstad                   º
; º Last Updated:  10/08/90                       º
; º Converted to 32-bit inline assembly by AndyW  º
; ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶
; º Notice     : This source code is NOT intended º
; º              for general distribution.  Care  º
; º              should be taken to preserve its  º
; º              confidentiality.                 º
; ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶
; º Copyright (C) 1988-1991 by NoGate Consulting  º
; º             All rights reserved.              º
; ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼
*/

// Common includes for Pre-compiled Header
// This should go before any other includes or definitions
#ifdef SYM_BUILD_SM
#include "fsv_pch.h"
#else
#include "..\inc\fsv_pch.h"
#endif
#pragma hdrstop
// Put all other includes after this

#define  window_size          (8192)      // maximum size of an input block
#define  char_alphabet_size   (256)       // # of characters in ascii      
#define  dist_alphabet_size   (64)        // # of length info units         
#define  len_alphabet_size    (64)        // # of distance info units      
#define  char_table_size      (2 * char_alphabet_size)   // table size
#define  dist_table_size      (2 * dist_alphabet_size)   // table size
#define  len_table_size       (2 * dist_alphabet_size)   // table size

// local decode information structure
typedef struct _v_struc
{
  char   achTextBuffer[window_size];   // Text buffer
  WORD   awCharTable[char_table_size]; // Char table
  WORD   awDistTable[dist_table_size]; // Dist table
  WORD   awLenTable[len_table_size];   // Len table
  DWORD  lpOut;                        // ptr to current output
  DWORD  lpIn;                         // ptr to current input
  DWORD  lpStartOutput;                // ptr to start of output
  DWORD  lpEndInput;                   // ptr to end of input
  DWORD  lpEndOutput;                  // ptr to end of output
  DWORD  SaveEAX;                      // temp storage of EAX (usually)
  DWORD  SaveECX;                      // temp storage of ECX (usually)
  DWORD  SaveEDX;                      // temp storage of EDX (usually)
  DWORD  SaveEBP;                      // temp storage of EBP (usually)
  WORD   wCharsEncoded;                // count of characters encoded???
  BYTE   bDistBits;                    // run length???
  BYTE   bFinal;                       // ???
} v_struc, * pv_struct;

static v_struc varsv;                  // declare it static to keep it around
static char    scratch[window_size];   // temp buffer

static   BYTE  bTemp;            // temp byte used to store h or l of registers for comparisons
static   WORD  wAlphabetSize;    // was a local var for make_tree (size of alphabet)
static   DWORD lpTableBase;      // was a local var for make_tree (ptr to base of table)
static   WORD  wTreeCount;       // was a local var for make_tree (count of items in tree???)
static   WORD  wCharCode;        // was a local var for make_tree (char to add???)
static   DWORD lpRLE;            // was a local var for make_tree (ptr to rle table???)

static   char far *  lpInBuffer;    // These variables are globals set equal
static   WORD far *  lpwInputSize;  // to params to explode because explode
static   char far *  lpOutBuffer;   // blows away bp so all the variables on
static   WORD FAR *  lpwOutputSize; // the stack get blown away as well. 
static   VOID far *  lpVarPtr;      // See above.
static   char        bFinal;        // See above.
static   BOOL        fResult;       // See above.



//
// This function returns the sizeof the decode information structure
//
WORD WINAPI explodeWorkSize( void )
{
   return (sizeof(v_struc));
}

//
// This function initializes the decode information structure
//
#pragma optimize( "", off )   // prevent compiler from mucking with the code
void WINAPI initExplode(
   LPVOID lpVarPtr,
   BYTE bCharsEncoded,
   BYTE eight_k_window )
{
   _asm
   {
      mov ebx, [lpVarPtr]              ; ebx = lpVarPtr
      xor eax, eax                     ; eax = 0
      mov [varsv.SaveECX], eax         ; varsv.SaveECX = 0 (eax)
      mov al, BYTE PTR[bCharsEncoded]  ; al = bCharsEncoded
      or       al, al                  ; set flags
      jz  EX_1                         ; if (al == 0) goto EX_1 (make al 0 or 1)
      mov al, 1                        ;     else al = 1
   EX_1:
      mov [varsv.wCharsEncoded], ax    ; varsv.wCharsEncoded = ax
      mov al, [eight_k_window]         ; al = eight_k_window
      or  al, al                       ; set flags
      jz  EX_2                         ; if (al == 0) goto EX_2 (make al 1 or 0)
      mov al, 1                        ;     else al = 1
   EX_2:
      add al, 6                        ; al += 6 (low bits are now 6 or 7)
      mov [varsv.bDistBits], al        ; varsv.bDistBits = al
   }
}
#pragma optimize( "", on )

//
// This function decodes a block
//
#pragma optimize( "", off )   // prevent compiler from mucking with the code
int WINAPI explode(
   char far *  lpInBuffer1,
   WORD FAR *  lpwInputSize1,
   char far *  lpOutBuffer1,
   WORD FAR *  lpwOutputSize1,
   VOID far *  lpVarPtr1,
   char        bFinal1
)
{
   // save parameters to global since bp is going to get whacked in the
   // assembly block below
   lpInBuffer     = lpInBuffer1;
   lpwInputSize   = lpwInputSize1;
   lpOutBuffer    = lpOutBuffer1;
   lpwOutputSize  = lpwOutputSize1;
   lpVarPtr       = lpVarPtr1;
   bFinal         = bFinal1;

   _asm
   {
      mov  ebx, [lpVarPtr]              ; ebx = lpVarPtr
      mov  al,[bFinal]                  ; al = bFinal
      mov  [varsv.bFinal], al           ; varsv.bFinal = al
      mov  eax, [lpInBuffer]            ; eax = lpInBuffer
      mov  [varsv.lpIn], eax            ; varsv.lpIn = eax
      mov  edi, [lpwInputSize]          ; edi = lpwInputSize (calculate end of input)
      xor  esi, esi
      mov  si, WORD PTR[edi]
      add  eax, esi                     ; eax += edi         (by adding input size and lpInBuffer)
      mov  [varsv.lpEndInput], eax      ; varsv.lpEndInput = eax
      mov  eax, [lpOutBuffer]           ; eax = lpOutBuffer
      mov  [varsv.lpOut], eax           ; varsv.lpOut = eax
      mov  [varsv.lpStartOutput], eax   ; varsv.lpStartOutput = eax
      mov  edi, [lpwOutputSize]         ; edi = lpwOutputSize (calculate end of output)
      xor  ecx, ecx
      mov  cx, WORD PTR[edi]
      add  eax, ecx                     ; eax += edi          (by adding output size and lpOutBuffer)
      sub  eax, 3                       ; eax -= 3
      mov  [varsv.lpEndOutput], eax     ; varsv.lpEndOutput = eax

      mov  esi, [lpInBuffer]            ; esi = lpInBuffer
      push ebp                          ; PUSH EBP

      cld                               ; CLD
      mov  eax, [varsv.SaveEAX]         ; eax = varsv.SaveEAX
      mov  ecx, [varsv.SaveECX]         ; ecx = varsv.SaveECX
      mov  edx, [varsv.SaveEDX]         ; edx = varsv.SaveEDX
      mov  ebp, [varsv.SaveEBP]         ; ebp = varsv.SaveEBP

      push ecx                          ; PUSH ECX
      and  ecx, 255                     ; ecx &= 255 (mask out all but bottom byte)
      mov  edi, ecx                     ; edi = ecx
      shl  edi, 2                       ; edi *= 4
      add  edi, ecx                     ; edi += ecx (this makes edi *= 5)
      pop  ecx                          ; POP ECX
      add edi, 8                        ; edi += 8  (add length of next two instructs so)
                                        ;           (edi will actually point to the first)
                                        ;           (jmp in the table)
      add edi, $                        ; (add the current ip to edi)
      jmp edi                           ; (make the jmp into the jmp table)
      jmp EX_initialization             ; jmp table
      jmp EX_new_input_1                ; jmp table
      jmp EX_decode_loop                ; jmp table
      jmp EX_new_input_3                ; jmp table
      jmp EX_new_input_4                ; jmp table
      jmp EX_decode_pointer             ; jmp table
      jmp EX_new_input_6                ; jmp table
      jmp EX_new_input_7                ; jmp table
      jmp EX_new_input_8                ; jmp table
      jmp EX_new_output_2               ; jmp table

      ; initialization - read trees, and initialize window.

   EX_initialization:

      ; read trees
      xor edx, edx                      ; zero out edx
      mov dx, WORD PTR[varsv.wCharsEncoded] ; save varsv.wCharsEncoded into dx
      and dx, 0ffh                      ; mask out all but bottom byte (3+wCharsEncoded trees)
      add dl, 3                         ; add 3 to dl

                                        ; first one 0 length
      xor  eax, eax                     ; zero out eax
      mov  ebp, eax                     ; save eax to ebp
   EX_new_input_1:
      mov  edi, ebp                     ; move previous bytes to edi
      mov  ebp, [varsv.lpEndInput]      ; ebp is end of input
      add  edi, ebx                     ; ??????????make edi point to scratch base
      mov  esi, [varsv.lpIn]            ; esi points to input
      jmp  EX_RLE_load_loop

   EX_load_RLE_size:                    ; called if varsv.wCharsEncoded is not zero
      cmp  esi, ebp                     ; if varsv.lpIn > varsv.lpEndInput, request input
      jae  EX_request_input_1
      mov  al, BYTE PTR[esi]            ; load length of run from varsv.lpIn?????
      mov  ah, 0                        ; zero out upper byte
      add  ax, 2                        ; add two??????????????????????
   EX_RLE_load_loop:
      mov  ecx, ebp                     ; ecx gets varsv.lpEndInput
      sub  ecx, esi                     ; subtract varsv.lpIn to give length of input remaining in ecx
      cmp  cx, ax                       ; compare to eax
      jb   EX_load_RLE
      mov  cx, ax                       ; if ecx > eax, ecx = eax
   EX_load_RLE:
      sub  ax, cx                       ; eax = eax - ecx = 
      shr  cx, 1                        ; divide ecx by 2
      rep  movsw                        ; move words from esi to edi until ecx = 0
      rcl  cx, 1                        ; divide by two with carry in bit 0 to retrieve remainder of previous shr
      rep  movsb                        ; move extra byte if any
      or   ax, ax                       ; set flags
      jnz  EX_request_input_1
      dec  dl                           ; dl is varsv.wCharsEncoded
      jnz  EX_load_RLE_size

      mov  [varsv.lpIn], esi  ; save input pointer

      ; make trees from RLE data

      mov  esi, ebx                      ; si points to RLE buffer
      push eax                           ; save eax
      mov ax, WORD PTR[varsv.wCharsEncoded] ; move varsv.chars_encode to ax
      mov [bTemp], al                    ; move al to bTemp
      pop eax                            ; restore eax
      cmp  BYTE PTR[bTemp], 1            ; compare 1 and low byte of wCharsEncoded
      jnz  EX_make_length_tree

      ; make character tree

      mov  cx, char_alphabet_size        ; save alphabet size to ecx
      lea  edi, [varsv.awCharTable]      ; put address of varsv.awCharTable in edi
      call make_tree                     ; make a tree

   EX_make_length_tree:
      mov  cx, len_alphabet_size         ; put len_wAlphabetSize in ecx
      lea  edi, [varsv.awLenTable]       ; put varsv.awLenTable in edi
      call make_tree                     ; make another tree
      mov  cx, dist_alphabet_size
      lea  edi, [varsv.awDistTable]
      call make_tree

      ; initialize window

      mov  cx, window_size /2            ; OK 
      xor  eax, eax
      lea  edi, [varsv.achTextBuffer]
      rep  stosw

      mov  ch, 0                         ; # of bits in input buffer is 0
      mov  esi, [varsv.lpIn]

   EX_decode_loop:
      dec  ch
      jns  EX_read_bit
      mov  ch, 15
      lods WORD PTR[esi]
      cmp  esi, [varsv.lpEndInput]
      jbe  EX_read_bit
      dec  esi
      sub  ch, 8
      cmp  esi, [varsv.lpEndInput]
      jbe  EX_read_bit
      dec  esi
      sub  ch, 7
      cmp  BYTE PTR[varsv.bFinal], 0
      je   EX_request_input_2
      jmp  EX_finished
   EX_read_bit:
      shr  ax, 1
      jnc  EX_decode_pointer
      push eax
      mov ax, WORD PTR[varsv.wCharsEncoded]
      mov BYTE PTR[bTemp], al
      pop eax
      cmp  BYTE PTR[bTemp], 1
      je   EX_decode_char
   EX_new_input_3:
      call read_byte
      jc   EX_request_input_3
      mov  dl, al
      mov  al, ah
      mov  ah, 0
      sub  ch, 8
      jmp  EX_store_char

   EX_decode_char:
         xor  edx, edx
   EX_new_input_4:
      lea  edi, [varsv.awCharTable]
      call read_code
      jc   EX_request_input_4

   EX_store_char:
      mov  edi, [varsv.lpOut]
      mov  [edi], dl
      inc  edi
      mov  [varsv.lpOut], edi
      cmp  edi, [varsv.lpEndOutput]
      jb   EX_decode_loop
      jmp  EX_request_output_1

   EX_decode_pointer:
      call read_byte
      jc   EX_request_input_5
      mov  dl, -1                      ; fill dl with all 1's
      mov  dh, BYTE PTR[varsv.bDistBits]
      mov  cl, 8                       ; create mask for lower 6 or 7 bits.
      sub  cl, dh
      shr  dl, cl
      and  dl, al                      ; put low bits into dl
      mov  cl, dh                      ; shift input buffer
      shr  ax, cl
      sub  ch, cl                      ; subtract bits used from count
      and  edx, 0ffffh                 ; mask out upper word
      mov  ebp, edx                    ; save dx

      xor  edx, edx                    ; load upper bits
   EX_new_input_6:
      lea  edi, [varsv.awDistTable]
      call read_code
      jc   EX_request_input_6
      xchg edx, ebp                    ; restore dx
      mov  cl, dh
      mov  dh, 0
      and  ebp, 0ffffh                 ; mask out upper word
      shl  bp, cl
      or   bp, dx   
      inc  bp                          ; bp now has displacement

      xor  edx, edx                    ; load match length
   EX_new_input_7:
      lea  edi, [varsv.awLenTable]
      call read_code
      jc   EX_request_input_7
      cmp  dl, 63                      ; if length code is 63, add next byte
      jne  EX_check_output
   EX_new_input_8:
      call read_byte
      jc   EX_request_input_8
      mov  cl, ah                      ; save ah
      mov  ah, 0                       ; add byte to length
      add  dx, ax
      mov  al, cl                      ; complete shift
      sub  ch, 8

   EX_check_output:
      add  dx, 2                       ; calculate correct match length
      add  dx, WORD PTR[varsv.wCharsEncoded] ; add 1 if characters are encoded
   EX_new_output_2:
      push eax     ; save registers - were going to be using a lot of them
      push ecx
      push esi

      mov  edi, [varsv.lpOut]          ; load output pointer
      mov  ecx, [varsv.lpEndOutput]    ; calculate space left
      sub  ecx, edi
      cmp  cx, dx                      ; compare to size of shift
      jbe  EX_check_origin
      mov  cx, dx
   EX_check_origin:
      sub  dx, cx
      mov  esi, edi
      sub  esi, ebp                    ; check if we have to use the prefix buffer
      jb   EX_copy_from_prefix
      cmp  esi, [varsv.lpStartOutput]
      jae  EX_copy_from_buffer
   EX_copy_from_prefix:
      sub  esi, [varsv.lpStartOutput]  ; subtract buffer start from offset
      mov  ax, cx                      ; calculate length in normal buffer
      mov  cx, si
      neg  cx
      sub  ax, cx
      jae  EX_load_prefix_address
      add  cx, ax
      xor  eax, eax                    ; zero length if negative
   EX_load_prefix_address:
      lea  esi, [(varsv.achTextBuffer)+esi+window_size]  ; load address in prefix buffer
      cmp  bp, 1
      jbe  EX_byte_move_1
      shr  cx, 1
      rep  movsw
      rcl  cx, 1
   EX_byte_move_1:
      rep  movsb
      mov  cx, ax
      mov  esi, [varsv.lpStartOutput]
   EX_copy_from_buffer:
      cmp  bp, 1
      jbe  EX_byte_move_2
      shr  cx, 1
      rep  movsw
      rcl  cx, 1
   EX_byte_move_2:
      rep  movsb

      pop  esi       ; restore other registers
      pop  ecx
      pop  eax

      mov  [varsv.lpOut], edi    ; save output pointer
      or   dx, dx                ; check if we have more to move
      jnz  EX_request_output_2
      jmp  EX_decode_loop

   EX_request_input_1:           ; restore normal state when RLE loop abends.
      mov  bp, di                ; save # of bytes already read
      sub  bp, bx
      mov  cl, 1
      jmp  EX_request_input

   EX_request_input_2:
      mov  cl, 2
      jmp  EX_request_input

   EX_request_input_3:
      mov  cl, 3
      jmp  EX_request_input

   EX_request_input_4:
      mov  cl, 4
      jmp  EX_request_input

   EX_request_input_5:
      mov  cl, 5
      jmp  EX_request_input

   EX_request_input_6:
      mov  cl, 6
      jmp  EX_request_input

   EX_request_input_7:
      mov  cl, 7
      jmp  EX_request_input

   EX_request_input_8:
      mov  cl, 8
      jmp  EX_request_input

   EX_request_output_1:
      mov  cl, 2
      jmp  EX_exit

   EX_request_output_2:
      mov  cl, 9
      jmp  EX_exit

   EX_request_input:
      cmp  BYTE PTR[varsv.bFinal], 0
      jne  EX_finished

   EX_exit:
      mov  [varsv.SaveEAX], eax
      mov  [varsv.SaveECX], ecx
      mov  [varsv.SaveEDX], edx
      mov  [varsv.SaveEBP], ebp

      push esi                      ; save si

      ; copy last 8192 bytes of output buffer into prefix.

      mov  ax, window_size          ; default copy length

      mov  edi, [varsv.lpOut] ; calculate size of output
      sub  edi, [varsv.lpStartOutput]

      mov  cx, ax                   ; check if smaller than window size
      sub  cx, di                   ; calculate amount of prefix to copy
      jbe  EX_copy_second_part
      mov  ax, di                   ; save output size
      lea  esi, [(varsv.achTextBuffer)+edi]    ; calculate source and destination
      lea  edi, [varsv.achTextBuffer]
      shr  cx, 1                    ; copy prefix
      rep  movsw
      rcl  cx, 1
      rep  movsb
   EX_copy_second_part:
      lea  edi, [(varsv.achTextBuffer)+window_size] ; set destination
      and  eax, 0ffffh                     ;mask out upper word
      sub  edi, eax
      mov  esi, [varsv.lpOut]              ; set source
      sub  esi, eax
      mov  cx, ax                          ; copy data
      shr  cx, 1
      rep  movsw
      rcl  cx, 1
      rep  movsb
      pop  esi                             ; restore si
      xor  eax, eax
      jmp  EX_end
   EX_finished:
      mov  eax, 1
   EX_end:
      mov  WORD PTR [fResult], ax
      pop  ebp                             ; restore BP
      mov  edi, [varsv.lpOut]
      mov  ebx, [lpwInputSize]             ; calculate input and output used
      sub  esi, [lpInBuffer]
      mov  WORD PTR[ebx], si
      mov  ebx, [lpwOutputSize]
      sub  edi, [lpOutBuffer]
      mov  WORD PTR[ebx], di
      jmp  EX_EndExplode



;proc make_tree near
;local wAlphabetSize:word, lpTableBase:word, wTreeCount:word, wCharCode:word, 
;      lpRLE:word = worksize
 ; create tree from length data
 ;  assumes ds:bx  vars
 ;             si  RLE data ptr
 ;             cx  alphabet size
 ;             di  start of table
 ;  returns:   si  RLE data ptr
 ;           es = ds
 ;           ax, cx, dx burned

 ; first, make length table from RLE data
   make_tree:
      mov  [lpTableBase], edi       ; lpTableBase = edi (save base address for output table)
      mov  [wAlphabetSize], cx      ; wAlphabetSize = cx (save alphabet size)
      lea  edi, [scratch]           ; edi = scratch (length table in scratch area ???????)
      xor  eax, eax                 ; ax = 0 (low byte = value)
      stosb                         ; high byte = length
      cld                           ; init first byte
      lodsb                         ; read # of RLE bytes
      mov  dl, al  ; si points to RLE data - load # of bytes
      inc  dl
   EX_length_loop:
      lodsb         ; load next byte
      mov  ch, al   ; move # of bytes to ch
      and  al, 15   ; calculate length in al
      mov  cl, 4    ; shift # bytes >> 4
      shr  ch, cl
      mov  cl, ch   ; set counter
      inc  cl
      mov  ch, 0
   EX_store_loop:
      inc  ah       ; increment value counter
      stosw
      loop EX_store_loop
      dec  dl       ; loop until out of RLE bytes
      jne  EX_length_loop
      mov  [lpRLE], esi   ; save RLE pointer
      lea  esi, [scratch]   ; sort from bottom of scratch to end of scratch  ?????
      sub  edi, 3
      call quick_sort
      cld
      mov  edi, [lpTableBase]     ; set tree to all -1's
      mov  cx, [wAlphabetSize]
      shl  cx, 1
      and  ecx, 0ffffh            ; mask out upper word
      xor  eax, eax
      dec  ax
      rep  stosw
      xor  esi, esi
      mov  si, [wAlphabetSize]
      shl  si, 1
      and  esi, 0ffffh              ; mask out upper word
      lea  esi, [scratch+esi-2]        ; make si point to end of scratch area ???????
      push ebx                    ; save bx
      mov  ebx, [lpTableBase]
      xor  eax, eax
      mov  [wTreeCount], ax

      ; int   wCharCode
      ; int   pointer
      ; int   tree count
      ; int   counter

      ; int   base pointer  bx
      ; int   working code  ax
      ; char  length        cl
      ; int   next          dx

   EX_code_loop:
      mov  WORD PTR[wCharCode], ax   ; save code
      xor  edi, edi        ; zero pointer
      mov  cl, [esi+1]     ; load length - 1
      or   cl, cl
      jz   EX_set_leaf     ; jump if length is only 1
   EX_connect_loop:
      shl  ax, 1           ; tree count *= 2 (calculate address of next branch)
      adc  di, 0
      shl  di, 1           ; di *= 2
      and  edi, 0ffffh     ; mask out upper word
      xor  edx, edx        ; clear upper word of edx so edx can be used as dx
      mov  dx, [ebx+edi]   ; load branch address into dx   ???????????????????
      or   dx, dx
      jns  EX_next_branch  ; check if branch initialized
      inc  WORD PTR[wTreeCount]    ; if not, use next free branch
      inc  WORD PTR[wTreeCount]
      mov  dx, WORD PTR[wTreeCount]
      mov  WORD PTR[ebx+edi], dx              ; ??????????????????????????????
   EX_next_branch:
      mov  di, dx         ; make di be next branch
      dec  cl             ; decrement length count
      jnz  EX_connect_loop
   EX_set_leaf:
      shl  ax, 1     ; calculate branch address
      adc  di, 0
      shl  di, 1
      and  edi, 0ffffh
      mov  al, BYTE PTR[esi]  ; load value
      mov  ah, 80h   ; set high bit
      mov  WORD PTR[ebx+edi], ax   ; save leaf value   ??????????????????????????????
      mov  al, 0         ; calculate next code
      mov  cl, [esi+1]
      shr  ax, cl        ; shift 8000h left (length - 1) bits
      add  ax, WORD PTR[wCharCode]    ; add to code
      sub  esi, 2
      dec  WORD PTR[wAlphabetSize]  ; decrement counter
      jnz  EX_code_loop
      mov  esi, [lpRLE]
      pop  ebx
      ret
;endp make_tree



;proc quick_sort near
; sort word area from ESI to EDI
; burns ax, dx

   quick_sort:
      cmp esi, edi            ; if (esi < edi)
      jb EX_start             ;     goto EX_start
      ret                     ; else return
   EX_start:      
      push edi                ; PUSH EDI
      push esi                ; PUSH ESI
      mov edx, esi            ; EDX = ESI (find size of area to sort)
      mov eax, edi            ; EAX = EDI
      sub eax, esi            ; EAX -= ESI
      shr ax, 1               ; AX /= 2  (get the middle)
      and eax, 0FFFEh         ; WORD align and mask out upper word
      add esi, eax            ; ESI += EAX (add start ptr to result)
      mov ax, WORD PTR [esi]  ; AX = WORD PTR [esi] (gives us the value from the center in ax!)
                              ; (this is used as the quicksort pivot value)
      mov esi, edx            ; ESI = EDX (restore esi to original value)
   EX_outer_loop:
      cmp ax, WORD PTR [esi]  ; if (ax <= WORD PTR [esi] (check value at lower index against pivot)
      jbe EX_check_high       ;     goto EX_check_high (we have one on this end that needs swapped)
   EX_low_loop:
      add esi, 2              ; esi += 2 (move to next WORD)
      cmp ax, WORD PTR [esi]  ; if (ax > WORD PTR [esi]) (check value at lower index against pivot)
      ja EX_low_loop          ;     goto EX_low_loop (repeat until we find a value that needs swapped)
   EX_check_high:
      cmp ax, WORD PTR [edi]  ; if (ax >= WORD PTR [edi]) (check value at upper index against pivot)
      jae EX_swap             ;     goto EX_swap
   EX_high_loop:
      sub edi, 2              ; move to previous WORD
      cmp ax, WORD PTR [edi]  ; check value at upper index against pivot
      jb EX_high_loop         ; repeat until we find a value that needs swapped
   EX_swap:
      cmp esi, edi            ; make sure the indices have not passed each other
      ja EX_done
      mov dx, WORD PTR [esi]  ; get value pointed to by esi
      cmp dx, WORD PTR [edi]  ; compare against value pointed to by edi
      je EX_advance           ; if the same, save cycles by not swapping
      xchg dx, WORD PTR [edi] ; do the swap
      mov WORD PTR [esi], dx  ; finish the swap
   EX_advance:
      add esi, 2              ; move to next WORD
      sub edi, 2              ; move to previous WORD
      cmp esi, edi            ; make sure the indices have not passed each other
      jb EX_outer_loop
   EX_done:                   ; finished with current pass
      pop ecx                 ; retrieve original addresses
      pop edx
      mov eax, edi            ; determine whether it's beneficial to do sort
      sub eax, ecx            ; bottom or top half first
      and eax, 0ffffh         ; mask out upper word
      add eax, esi
      cmp eax, edx
      jb EX_low_first
      xchg esi, ecx           ; else swap indices so top is sorted first
      xchg edi, edx
   EX_low_first:
      push edx                ; save the range for the second pass
      push esi
      mov esi, ecx            ; the current range is from ecx to edi
      call quick_sort
      pop esi                 ; restore original starting indices
      pop edi
      jmp quick_sort          ; jump to the top without pushing ret addr on stack
;endp quick_sort



;proc read_code near
; assumes
;      ebx is vars
;      esi points to input
;         eax is input buffer
;         ch is # of bits in input buffer
;         dx is current code (0 to start)
;         edi is table address
; returns
;         dx    partial or complete code
;         carry set if out of input

   read_code:
      xchg ebx, edx  ; make code into base
   EX_decode_huff:
      dec  ch
      jns  EX_shift
      xchg ebx, edx  ; restore bx
      mov  ch, 15

      lods WORD PTR[esi]            ; THE FINAL BUG WAS HERE (left off WORD PTR)

      cmp  esi, [varsv.lpEndInput]
      jbe  EX_loaded
      dec  esi
      sub  ch, 8
      cmp  esi, [varsv.lpEndInput]
      jbe  EX_loaded
      sub  ch, 7
      dec  esi
      stc
      ret
   EX_loaded:
      xchg ebx, edx
   EX_shift:
      shr  eax, 1
      adc  bx, 0      ; bx is currently input code
      shl  bx, 1
      and  ebx, 0ffffh ; mask out upper word
      mov  bx, WORD PTR[edi+ebx]   ; ???????????
      or   bx, bx
      jns  EX_decode_huff
      xchg ebx, edx   ; restore ebx
      and  dh, 7fh
      jz   EX_doneRC
      stc
   EX_doneRC:
      ret
;endp read_code



;proc read_byte near
; assumes EBX is vars
;          AX is something   
;         ESI is input stream
   read_byte:
      cmp ch, 8               ; if (ch < 8)
      jb  EX_load_byte        ;     goto EX_load_byte 
      clc                     ; CLC
      ret                     ; return
   EX_load_byte:
      mov  cl, 8              ; cl = 8
      sub  cl, ch             ; cl -= ch
      shl  al, cl             ; al *= 2^cl
      mov  ah, BYTE PTR [esi] ; ah = BYTE PTR [esi]
      shr  ax, cl             ; ax /= 2^cl
      inc  esi                ; esi++
      cmp  esi, [varsv.lpEndInput] ; if (esi <= varsv.lpEndInput)
      jbe  EX_input_valid     ;           goto EX_input_valid
      dec  esi                ; esi--
      stc                     ; STC
      ret                     ; return
   EX_input_valid:
      add ch, 8               ; ch += 8
      ret                     ; return
;endp read_byte

   EX_EndExplode:
   
   }
   
   return (fResult);
}
#pragma optimize( "", on )
