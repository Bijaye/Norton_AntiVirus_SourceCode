#include <stdio.h>
#include <stdlib.h>

#include "autoverv.h"

int equivalent_keys(const key_link* key1, const key_link* key2, int num_samples)
{
   int i;
   unsigned char b;
   
   if (key1->encryption_info.crypt_type != key2->encryption_info.crypt_type) {
      return 0;
   }
   
   const unsigned char
      *keys_in_each_sample1 = key1->keys_in_each_sample,
      *keys_in_each_sample2 = key2->keys_in_each_sample;
   
   switch(key1->encryption_info.crypt_type)
   {
   case 0: /* Plain */
      return 1;
      
   case 1: /* XOR1 */
      b = keys_in_each_sample1[0] ^ keys_in_each_sample2[0];
      for(i=1;
         (i<num_samples) && 
         ((keys_in_each_sample1[i] ^ keys_in_each_sample2[i]) == b);
         i++);      
      if(i == num_samples)
         return 1;
      else 
         return 0;
      
   case 2: /* Add1 */
      b = keys_in_each_sample1[0] - keys_in_each_sample2[0];
      for(i=1;
         (i<num_samples) && 
         ((keys_in_each_sample1[i] - keys_in_each_sample2[i]) == b);
         i++);      
      if(i == num_samples)
         return 1;
      else 
         return 0;
      
   case 3: /* XOR2 */
      b = keys_in_each_sample1[0] ^ keys_in_each_sample2[0];
      for(i=1;
         (i<num_samples) && 
         ((keys_in_each_sample1[2*i] ^ keys_in_each_sample2[2*i]) == b);
         i++);      
      if(i != num_samples) {
         return 0;
      }else  {
         b = keys_in_each_sample1[1] ^ keys_in_each_sample2[1];
         for(i=1;
            (i<num_samples) && 
            ((keys_in_each_sample1[2*i+1] ^ keys_in_each_sample2[2*i+1]) == b);
            i++);      
         if(i != num_samples)
            return 0;
         else
            return 1;
      }
      
   default:
      return 0;
   }
}
