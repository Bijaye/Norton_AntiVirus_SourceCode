#include <stdio.h>
#include <stdlib.h>
/*#include <values.h>*/

#include "autoverv.h"

int find_match_len(struct sample_data *master, int master_pos,
                   struct sample_data *slave,  int slave_pos,
               int current_best_match_length)
  {
     int master_max,
     slave_max,
     m_curr_pos,
     s_curr_pos;

     master_max = master->num_bytes;
     slave_max = slave->num_bytes;

#if 1 /* Quick test to see if we can't possibly do better than previous best match. If so, kick out now. */

     m_curr_pos=master_pos+current_best_match_length;
     s_curr_pos=slave_pos+current_best_match_length;

     if((m_curr_pos >= master_max) || (s_curr_pos >= slave_max) ||   /* Past end */
      master->map_bytes[m_curr_pos] ||  slave->map_bytes[s_curr_pos] ||  /* Invalid bytes? */
      (master->bytes[m_curr_pos] != slave->bytes[s_curr_pos]))  /* a mismatch */
       return 0;
#endif

     m_curr_pos=master_pos;
     s_curr_pos=slave_pos;


     while((m_curr_pos < master_max) && (s_curr_pos < slave_max) &&   /* Not past end */
         !master->map_bytes[m_curr_pos] && !slave->map_bytes[s_curr_pos] &&  /* while still valid */
           (master->bytes[m_curr_pos] == slave->bytes[s_curr_pos]))  /* and not a mismatch */
       {
         m_curr_pos++;    /* go to next byte in each sample */
         s_curr_pos++;
       }

    return(m_curr_pos-master_pos);    /* compute length and return */
  }
