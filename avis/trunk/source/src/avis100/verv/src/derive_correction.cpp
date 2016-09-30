#include <stdio.h>
#include <stdlib.h>

#include "autoverv.h"
  
int derive_correction(int num_samples,
                  unsigned char *goat_vector,
                  unsigned char *variable_virus_vector,
                  struct key_link *crypt_key_item)
{
  int
    i,
    match;

  unsigned char
    b1new,
    b1;

  unsigned char
    *kc;
  unsigned char * oldkc; // Fred: This is a debug variable, to test memory leaks reported by BoundsChecker

  // 6/30/97 Fred: by default, we start with no key correction
  if (crypt_key_item)
	crypt_key_item->encryption_info.key_correction = NULL;

  switch((crypt_key_item->encryption_info).crypt_type)
    {
    case 0: /* Plain */
      for(i=0,match=1;(i<num_samples) && match;i++)
		 // 6/26/97 Fred: This & seems to be a bug, changed it to &&
		 //match = match & (goat_vector[i] == variable_virus_vector[i]);
         match = match && (goat_vector[i] == variable_virus_vector[i]);
      if(match)
      {
        //kc = NULL;
        (crypt_key_item->encryption_info).key_correction = NULL;
        return 1;
      }
      else
        return 0;
    case 1: /* XOR1 */
      b1 = goat_vector[0] ^ variable_virus_vector[0] ^ crypt_key_item->keys_in_each_sample[0];
      for(i=1;
        (i<num_samples) &&
        ((b1new = goat_vector[i] ^ variable_virus_vector[i] ^ crypt_key_item->keys_in_each_sample[i]) == b1);
        i++);
      if((i == num_samples) && (b1new == b1))
      {
        kc = (unsigned char *) ALLOC(unsigned char, 1);
        kc[0] = b1;
        oldkc = (crypt_key_item->encryption_info).key_correction; free(oldkc);
        (crypt_key_item->encryption_info).key_correction = kc;
        return 1;
      }
      else 
        return 0;
    case 2: /* Add1 */
      b1 = goat_vector[0] - variable_virus_vector[0] + crypt_key_item->keys_in_each_sample[0];
      for(
        i=1; 
        (i<num_samples) &&
        ((b1new = goat_vector[i] - variable_virus_vector[i] + crypt_key_item->keys_in_each_sample[i]) == b1); 
        i++);
      if((i == num_samples) && (b1new == b1))
      {
        kc = (unsigned char *) ALLOC(unsigned char, 1);
        kc[0] = b1;
		// Fred: The following line is a horrible way to correct an ugly memory leak, come back to that later...
        oldkc = (crypt_key_item->encryption_info).key_correction; free(oldkc);
        (crypt_key_item->encryption_info).key_correction = kc;
        return 1;
      }
      else 
        return 0;
    case 3: /* XOR2 */
      b1 = goat_vector[0] ^ variable_virus_vector[0] ^ crypt_key_item->keys_in_each_sample[0];
      for(i=1;
        (i<num_samples) && 
        ((b1new = goat_vector[i] ^ variable_virus_vector[i] ^ crypt_key_item->keys_in_each_sample[2*i]) == b1);
        i++);      
      if((i == num_samples) && (b1new == b1))
      {
        kc = (unsigned char *) ALLOC(unsigned char, 2);
        kc[0] = b1;
        kc[1] = 0x00;
        oldkc = (crypt_key_item->encryption_info).key_correction; free(oldkc);
        (crypt_key_item->encryption_info).key_correction = kc;
        return 1;
      }
      else
      {
        b1 = goat_vector[0] ^ variable_virus_vector[0] ^ crypt_key_item->keys_in_each_sample[1];
        for(i=1;
            (i<num_samples) && 
            ((b1new = goat_vector[i] ^ variable_virus_vector[i] ^ crypt_key_item->keys_in_each_sample[2*i+1]) == b1);
            i++);      
        if((i == num_samples) && (b1new == b1))
          {
            kc = (unsigned char *) ALLOC(unsigned char, 2);
            kc[0] = 0x00;
            kc[1] = b1;
            oldkc = (crypt_key_item->encryption_info).key_correction; free(oldkc);
            (crypt_key_item->encryption_info).key_correction = kc;
            return 1;
          }
        else
          return 0;
      }
    default:
      return 0;
    }
}
