//AJR: Create a plaintext pseudo-key in case some reconstructions do not need
//     decryption. If there's a real plaintext V-match, the pseodo-key will
//     be elimiated as a duplicate in a later step.

#include <stdio.h>
#include <stdlib.h>
#include "autoverv.h"

struct key_link *make_key_list(struct match_list *virus_match,
			       struct sample_data **samples,
			       struct sample_data **goat_samples,
			       int num_samples,
			       int filetype)


{
  struct match_list *vmatch;
  struct key_link *list=NULL, *oldhead;
  int i, j, k, key_len;
  int num_files;

  /* count number of applicable files */
  num_files = 0;
  for(i=0;i<num_samples;i++)
    if(goat_samples[i] && (get_file_type(samples[i]) == filetype))
      num_files++;

  // AJR create plaintext pseudo-key
  list = (struct key_link *)malloc(sizeof(struct key_link));
  list->encryption_info.crypt_type = 0;   // plaintext
  list->encryption_info.key_location.offset = 0;
  list->encryption_info.key_location.anchor_type = 0;   //?? begin
  list->encryption_info.key_correction = NULL;
  list->keys_in_each_sample = NULL;
  list->next = NULL;

  /* go through matched virus sections */
  vmatch = virus_match;
  while(vmatch)  
    {
      oldhead = list;
      list = (struct key_link *)malloc(sizeof(struct key_link));
      list->encryption_info.crypt_type = vmatch->match->type;
      list->encryption_info.key_location.offset = vmatch->begin.offset_min[filetype];
      list->encryption_info.key_location.anchor_type = vmatch->begin.bestanchor[filetype];
      list->encryption_info.key_correction=NULL;
      
      key_len = key_length[list->encryption_info.crypt_type];
      
	  // 6/26/97 Fred: Modified the section below so that memory does not get allocated when there is no encryption
#if 0	  
	  list->keys_in_each_sample = (unsigned char *)malloc(sizeof(unsigned char) * 
                                            num_files * key_len);
      /* copy key data */
      for(i=0,j=0;i<num_samples;i++)
      if(goat_samples[i] && (get_file_type(samples[i]) == filetype))
        {
          for(k=0;k<key_len;k++)
            list->keys_in_each_sample[key_len*j+k] = samples[i]->bytes[vmatch->match->pos_in_sample[i]+k];
          j++;
        }
#endif

	list->keys_in_each_sample = NULL;

    /* copy key data */
	if (key_len > 0) {
		list->keys_in_each_sample = (unsigned char *)malloc(sizeof(unsigned char) * num_files * key_len);
		for(i=0,j=0;i<num_samples;i++)
			if(goat_samples[i] && (get_file_type(samples[i]) == filetype)) {
				for(k=0;k<key_len;k++)
					list->keys_in_each_sample[key_len*j+k] = samples[i]->bytes[vmatch->match->pos_in_sample[i]+k];
				j++;
				}
	}
	// Fred: end of modified section
	
	  list->next = oldhead;
      vmatch = vmatch->next;  /* next section */
    }
  return(list);
}
