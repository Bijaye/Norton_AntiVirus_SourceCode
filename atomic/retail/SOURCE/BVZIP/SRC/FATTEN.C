
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
#define window_size 4096

typedef enum {
  initial_entry, request_input_1, request_input_2, request_input_3,
  request_input_4, request_output_1, request_output_2, request_output_3 }
  entry_points;

typedef struct {
  unsigned char text_buffer[window_size];
  unsigned char follower[256][64];
  char          bits[256];
  char          length_bits;
  unsigned      code;
  char          bits_to_read;
  char          bitcount;
  unsigned      wordbuffer;
  entry_points  entry_point;
  int           index;
  int           length;
  int           displacement;
  unsigned char extra_flag;
  char          state;
 } fatten_vars;

unsigned WINAPI fatten_size(void);
void WINAPI init_fatten(fatten_vars far *var, int length_bits);
int WINAPI fatten(char far *inbuffer,  unsigned short far *input_size,
                      char far *outbuffer, unsigned short far *output_size,
                      fatten_vars far *var, char final);


#ifdef SYM_BUILD_SM
#include "wnfsv.seg"
#else
#include "..\inc\wnfsv.seg"
#endif

unsigned WINAPI fatten_size(void)
 {
  return sizeof(fatten_vars);
 }

void WINAPI init_fatten(fatten_vars far *var, int length_bits)
 {
  var->length_bits = length_bits;
  var->extra_flag = (1 << length_bits) - 1;
  var->entry_point = initial_entry;
 }

int WINAPI fatten(char far *inbuffer,  unsigned short far *input_size,
                      char far *outbuffer, unsigned short far *output_size,
                      fatten_vars far *var, char final)
 {
  unsigned char far *inbuf;
  unsigned char far *outbuf;
  char far *source;
  char far *dest;
  char far *end_of_input;
  char far *end_of_output;
  int       i;
  unsigned  space_left;
  unsigned  total_output;
  unsigned  pass_length;
  unsigned  pre_pass;

  inbuf = inbuffer;
  outbuf = outbuffer;
  end_of_input = inbuffer + *input_size;
  end_of_output = outbuffer + *output_size;
  switch (var->entry_point) {
    case initial_entry    :
      goto initialize;
    case request_input_1  :
      goto new_input_1;
    case request_input_2  :
      goto new_input_2;
    case request_input_3  :
      goto new_input_3;
    case request_input_4  :
      goto new_input_4;
    case request_output_1 :
      goto new_output_1;
    case request_output_2 :
      goto new_output_2;
    case request_output_3 :
      goto new_output_3;
   }
initialize:
  var->bitcount = 0;
  var->wordbuffer = 0;
  i = sizeof(var->text_buffer);
  source = var->text_buffer;
  while (i--) 
    *source++ = 0;
  var->code = 256;
  while (var->code--) {
    if (var->bitcount < 6) {
new_input_3:
      if (inbuf == end_of_input) {
        if (final) 
          goto finish;
        var->entry_point = request_input_3;
        goto exit_fatten;
       }
      var->wordbuffer |= *inbuf++ << var->bitcount;
      var->bitcount += 8;
     }
    var->length = var->wordbuffer & 0x3f;
    var->bitcount -= 6;
    var->wordbuffer >>= 6;
    if (!var->length)
      var->bits[var->code] = 0;
     else {
      i = 1;
      while (1 << i < var->length) i++;
      var->bits[var->code] = i;
      for (var->index = 0; var->index < var->length; var->index++) {
        if (var->bitcount < 8) {
new_input_4:
          if (inbuf == end_of_input) {
            if (final) 
              goto finish;
            var->entry_point = request_input_4;
            goto exit_fatten;
           }
          var->wordbuffer |= *inbuf++ << var->bitcount;
          var->bitcount += 8;
         }
        var->follower[var->code][var->index] = var->wordbuffer & 0xff;
        var->wordbuffer >>= 8;
        var->bitcount -= 8;
       }
     }
   }
  var->code = 0;
  var->state = 0;
  do {
    if (!(var->bits_to_read = var->bits[var->code]))
      var->bits_to_read = 8;
     else {
      if (!var->bitcount) {
new_input_1:
        if (inbuf == end_of_input) {
          if (final) 
            goto finish;
          var->entry_point = request_input_1;
          goto exit_fatten;
         }
        var->wordbuffer = *inbuf++;
        var->bitcount = 8;
       }
      if ((var->wordbuffer & 1))
        var->bits_to_read = 8;
      var->wordbuffer >>= 1;
      var->bitcount--;
     }
    if (var->bitcount < var->bits_to_read) {
new_input_2:
      if (inbuf == end_of_input) {
        if (final) 
          goto finish;
        var->entry_point = request_input_2;
        goto exit_fatten;
       }
      var->wordbuffer |= *inbuf++ << var->bitcount;
      var->bitcount += 8;
     }
    if (var->bits_to_read < 8)
      var->code = var->follower[var->code]
                  [var->wordbuffer & ((1 << var->bits_to_read) - 1)];
     else
      var->code = var->wordbuffer & 0xff;
    var->wordbuffer >>= var->bits_to_read;
    var->bitcount -= var->bits_to_read;
    switch (var->state) {
      case 0 :
        if (var->code == DLE)
          var->state = 1;
         else {
new_output_1:
          if (outbuf == end_of_output) {
            var->entry_point = request_output_1;
            goto exit_fatten;
           }
          *outbuf++ = var->code;
         }
        break;
      case 1 : 
        if (var->code) {
          var->displacement = ((var->code >> var->length_bits) << 8) + 1;
          if ((var->length = (int)(var->code & var->extra_flag)) == (int)var->extra_flag)
            var->state = 2;
           else
            var->state = 3;
         } else {
new_output_2:
          if (outbuf == end_of_output) {
            var->entry_point = request_output_2;
            goto exit_fatten;
           }
          *outbuf++ = DLE;
          var->state = 0;
         }
        break;
      case 2 :
        var->length += var->code;
        var->state = 3;
        break;
      case 3 :
        var->displacement += var->code;
        var->length += 3;
new_output_3:
        if ((space_left = (end_of_output - outbuf)) <
            (pass_length = var->length))
          pass_length = space_left;
        if ((total_output = (outbuf - outbuffer)) < (unsigned)var->displacement) {
          if ((pre_pass = var->displacement - total_output) > pass_length)
            pre_pass = pass_length;
          pass_length -= pre_pass;
          var->length -= pre_pass;
          source = var->text_buffer + sizeof(var->text_buffer) - 
                   var->displacement + total_output;
          while (pre_pass--) 
            *outbuf++ = *source++;
         }
        var->length -= pass_length;
        source = outbuf - var->displacement;
        while (pass_length--)
          *outbuf++ = *source++;
        if (var->length) {
          var->entry_point = request_output_3;
          goto exit_fatten;
         }
        var->state = 0;
     }
   } while (1);
finish:
  *input_size = inbuf - inbuffer;
  *output_size = outbuf - outbuffer;
  return 1;

exit_fatten:
  /* first, we need to copy the output into the text buffer */
  if ((total_output = outbuf - outbuffer) > 0) {
    dest = var->text_buffer;
    if (sizeof(var->text_buffer) > total_output) {
      pre_pass = sizeof(var->text_buffer) - total_output;
      source = var->text_buffer + total_output;
      while (pre_pass--)
        *dest++ = *source++;
     } else {
      total_output = sizeof(var->text_buffer);
     }
    source = outbuf - total_output;
    while (total_output--)
      *dest++ = *source++;
   }
  *input_size = inbuf - inbuffer;
  *output_size = outbuf - outbuffer;
  return 0;
 }
