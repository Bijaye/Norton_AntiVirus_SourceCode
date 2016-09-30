#include <stdlib.h>
#include <stdio.h>
#include "autoverv.h"

struct reconstruction_list *make_reconstruction_list(struct phase2_match *recon_summary,
                                         struct reconstruction_vector *goat_bytes,
                                         int num_goat_bytes)
  {
    struct phase2_match *r_summary;
    struct reconstruction_list *rlist=NULL, *newMatch, *curr;
    int j/*, length*/;

    r_summary = recon_summary;
    while(r_summary)
      {
      if(r_summary->g_anchor_type == 0)  /* can only BWRITE at BOF */
        {
          j=0;
          while(j<num_goat_bytes)
            {
            /* find first byte in run */
            for(;(j<num_goat_bytes) && !(r_summary->covered_goat_bytes[j]);j++);
            if(j<num_goat_bytes)
              {
                newMatch = (struct reconstruction_list *)malloc(sizeof(struct reconstruction_list));
                memcpy(&(newMatch->encryption_info),&(r_summary->encryption_info),sizeof(struct key));
                newMatch->g_begin.anchor_type = r_summary->g_anchor_type;
                newMatch->ig_begin.anchor_type = r_summary->vv_anchor_type;
                
                newMatch->g_begin.offset = goat_bytes[j].loc.offset;
                newMatch->ig_begin.offset = goat_bytes[j].loc.offset + r_summary->relative_offset;
                for(;(j+1<num_goat_bytes) && r_summary->covered_goat_bytes[j+1] && 
                  (goat_bytes[j+1].loc.offset == goat_bytes[j].loc.offset+1);j++); /* find last byte */
                newMatch->length = goat_bytes[j].loc.offset-newMatch->g_begin.offset+1;
                
                /* insert in list */
                if(!rlist || (compare_locations(newMatch->g_begin,rlist->g_begin) < 0))
                  {  /* insert at beginning */
                  newMatch->next = rlist;
                  rlist = newMatch;
                  }
                else
                  {  
                  /* find spot for node */
                  curr = rlist;
                  while(curr->next && (compare_locations(newMatch->g_begin,curr->next->g_begin) > 0))
                    curr=curr->next;
                  
                  /* insert it */
                  newMatch->next = curr->next;
                  curr->next = newMatch;
                  }

                j++;  /* continue at next byte */
              }
            }
        }
      r_summary = r_summary->next;
      }

    return(rlist);
  }  



