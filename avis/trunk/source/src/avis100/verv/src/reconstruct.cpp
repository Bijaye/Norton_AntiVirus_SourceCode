#include <stdio.h>
#include <stdlib.h>

#include "autoverv.h"
  
struct phase1_match **reconstruct(int num_samples,
                          int num_goat_bytes,
                          int num_variable_virus_bytes,
                          struct reconstruction_vector *goat_bytes,
                          struct reconstruction_vector *variable_virus_bytes,
                          struct key_link *crypt_key_list)

{
  
  int
    i,
    success,
    g_index,
    v_index;

  struct phase1_match **possible_match;

  struct key_link *crypt_key_item;
  struct phase1_match *tentative_match;

  /*unsigned char *key_correction;*/

  /*struct key
    crypt_info;*/

  eliminate_duplicate_keys(num_samples, crypt_key_list);


  possible_match = (struct phase1_match **) malloc(sizeof(struct phase1_match *) * num_goat_bytes);
  for(i=0;i<num_goat_bytes;i++)
    possible_match[i] = (struct phase1_match *) NULL;


  crypt_key_item = crypt_key_list;

#if 0
  while(crypt_key_item)
    {
      crypt_info = crypt_key_item->encryption_info;
      printf("%d %c %d \n",
           crypt_info.crypt_type,
           (crypt_info.key_location).anchor_type,
           (crypt_info.key_location).offset); 
      crypt_key_item = crypt_key_item->next;
    }
#endif

  for(g_index = 0; g_index < num_goat_bytes; g_index++)
    {
      for(v_index = 0; v_index < num_variable_virus_bytes; v_index++)
      {
        crypt_key_item = crypt_key_list;
        while(crypt_key_item)
          {
            success = derive_correction(num_samples,
                                goat_bytes[g_index].sample,
                                variable_virus_bytes[v_index].sample,
                                crypt_key_item);
            if(success)
            {
printf("fix? g:%c,%d v:%c,%d   \tk:%d,%c,%d\n",anchorchar[goat_bytes[g_index].loc.anchor_type],goat_bytes[g_index].loc.offset,anchorchar[variable_virus_bytes[v_index].loc.anchor_type],variable_virus_bytes[v_index].loc.offset,crypt_key_item->encryption_info.crypt_type,anchorchar[crypt_key_item->encryption_info.key_location.anchor_type],crypt_key_item->encryption_info.key_location.offset); 
              tentative_match = (struct phase1_match *) malloc(sizeof(struct phase1_match));
              tentative_match->loc = (variable_virus_bytes[v_index]).loc;
              tentative_match->encryption_info = crypt_key_item->encryption_info;
  
              if(possible_match[g_index] == NULL)
                {
                  possible_match[g_index] = tentative_match;
                  possible_match[g_index]->next = NULL;
                }
              else
                {
                  tentative_match->next = possible_match[g_index];
                  possible_match[g_index] = tentative_match;
                }
            }
            crypt_key_item = crypt_key_item->next;
          }
      }
    }

  return(possible_match);

}

        
         
