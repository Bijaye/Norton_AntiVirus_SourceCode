#include <stdio.h>
#include <stdlib.h>

#include "autoverv.h"

void eliminate_duplicate_keys(int num_samples,struct key_link *crypt_key_list){
   key_link
      *crypt_key_item1,  // the current item.
      *crypt_key_item2,  // the item that will be removed if it matches item1.
      *crypt_key_item_previous;  // the item before item2, that will be linked
                                 // to item2->next if item2 is to be removed.
   
   // walk the key list
   crypt_key_item1 = crypt_key_list;
   while(crypt_key_item1) {

      // match forwards from item1
      crypt_key_item_previous = crypt_key_item1;
      while((crypt_key_item2 = crypt_key_item_previous->next) != NULL) {

         if (equivalent_keys(crypt_key_item1,crypt_key_item2,num_samples)) {
            // remove item2
            crypt_key_item_previous->next = crypt_key_item2->next;
            // 6/27/97 Fred: added the 2 following lines to free the key structure properly
            free(crypt_key_item2->encryption_info.key_correction);
            free(crypt_key_item2->keys_in_each_sample);
            free(crypt_key_item2);
         }else {
            // item_previous advances only if item2 is kept, 
            // in which case item2 is the next item.
            crypt_key_item_previous = crypt_key_item2;
         }
      }
      crypt_key_item1 = crypt_key_item1->next;
   }
   return;
}

