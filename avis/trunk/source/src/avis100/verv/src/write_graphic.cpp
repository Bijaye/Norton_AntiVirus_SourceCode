#include <stdlib.h>
#include <stdio.h>
/*#include <values.h>*/
#include "autoverv.h"

#define UNKNOWN   0
#define GOAT            1
#define INFECTED  2
#define ENCRYPTED 3
#define IMBEDDED_GOAT   6

void write_graphic(char *infected_dir, 
                   struct map_rec *map_list[file_types], 
                   struct reconstruction_list *recon_list[file_types])
{
  int i,j;
  FILE *viz_out;
  char viz_name[256];
  char virusname[256];
  char ftype[5];
  int curr_pos, anchorpos[3], section_color;
  int b_offset, e_offset;
  int curr_anchor, b_anchor, e_anchor;
  int crypt_type;

  int
    abs_begin,
    abs_end;
      
/*  int r_anchor, r_begin, r_end; */

  struct map_rec *m_list;
  struct reconstruction_list *r_list;
  struct match_list *vmatch;
  struct single_boundary tmp_begin, tmp_end;

  struct visual_section
    {
#ifdef VERV_CODO
      int code;
#endif
      int color;
      int abs_begin;
      int abs_end;
      struct visual_section *next;
    };

  struct visual_section
    *vis_section_list = NULL,
    *curr_vis_section = NULL,
    *new_vis_section = NULL;
  
  for(i=0;i<file_types;i++)
    {
      for(j=strlen(infected_dir)-2;(j>=0) && (infected_dir[j]!='/');j--);
      strcpy(virusname,&(infected_dir[j+1]));
      //virusname[strlen(virusname)-1] = '\0';

      strcpy(viz_name,virusname);

      if(i==0)
         strcpy(ftype,".com");
      else
         strcpy(ftype,".exe");

      strcat(viz_name,ftype);

      strcat(viz_name,".viz");
      viz_out = fopen(viz_name,"w");

      fprintf(viz_out,"# Virus (%s)\n", virusname);
      fprintf(viz_out,"# Victim type: (%s.)\n\n",ftype);
      fprintf(viz_out,"# type 0 -> UNKNOWN\n");
      fprintf(viz_out,"# type 1 -> GOAT\n");
      fprintf(viz_out,"# type 2 -> INFECTED\n");
      fprintf(viz_out,"# type 3 -> XOR1-ENCRYPTED\n");
      fprintf(viz_out,"# type 4 -> ADD1-ENCRYPTED\n");
      fprintf(viz_out,"# type 5 -> XOR2-ENCRYPTED\n");
      fprintf(viz_out,"# type 6 -> IMBEDDED GOAT\n\n");

      fprintf(viz_out, "name %s\n", virusname);
      fprintf(viz_out, "ext %s\n\n", ftype);

      curr_pos = 0;
      curr_anchor = 0;
      for(j=0;j<3;j++) anchorpos[j] = 0;

      m_list = map_list[i];

      while(m_list)
      {
#ifdef VERV_CODO
        int code = -1;
#endif
        get_begin_boundary(&tmp_begin,m_list,i);
        get_end_boundary(&tmp_end,m_list,i);
        
        if(m_list->record_type == 'V')
          {
            section_color = INFECTED;

            if((vmatch = (struct match_list *)(m_list->record_ptr)))
            {
              if(vmatch->match)
                crypt_type = vmatch->match->type;
              else
                crypt_type = 0;
              
              if(crypt_type)  section_color = crypt_type + 2;  /* XOR1, ADD or XOR2 */
#ifdef VERV_CODO
              code = vmatch->code;
#endif
            }
            else crypt_type = 0;
          }
        else section_color = GOAT;

        b_anchor = tmp_begin.bestanchor;
        b_offset = tmp_begin.offset_min;
        e_anchor = tmp_end.bestanchor;
        e_offset = tmp_end.offset_min;

        if(curr_anchor != b_anchor)
          {
            /* we moved to another relative anchor position */
            /* what we can do, is giving an arbitrary absolute position for this new */
            /* anchor, but being sure that the offset doesn't go over the left limit */

            anchorpos[b_anchor] = anchorpos[curr_anchor /* the precedent anchor */] + abs(e_offset)+1000;
            curr_anchor = b_anchor;
          }

        abs_begin = anchorpos[curr_anchor]+b_offset;

        if(curr_anchor != e_anchor)
          {
            anchorpos[e_anchor] = anchorpos[curr_anchor /* the precedent anchor */] + abs(e_offset)+1000;
            curr_anchor = e_anchor;
          }

        abs_end = anchorpos[curr_anchor]+e_offset;

        new_vis_section = (struct visual_section *) malloc(sizeof(struct visual_section));
        new_vis_section->color = section_color;
        new_vis_section->abs_begin = abs_begin;
        new_vis_section->abs_end = abs_end;
#ifdef VERV_CODO
        new_vis_section->code = code;
#endif
        new_vis_section->next = NULL;

        if(curr_vis_section)
          {
            curr_vis_section->next = new_vis_section;
            curr_vis_section = new_vis_section;
          }
        else
          {
            vis_section_list = new_vis_section;
            curr_vis_section = new_vis_section;
          }

#if 0       
        fprintf(viz_out,"type %d box %d %d \n",section_color , abs_begin, abs_end);
#endif

        m_list=m_list->next;
      }

      r_list = recon_list[i];

      while(r_list)
      {
        curr_anchor = r_list->ig_begin.anchor_type;
        abs_begin = anchorpos[curr_anchor] + r_list->ig_begin.offset;
        abs_end = abs_begin + r_list->length - 1;

        section_color = IMBEDDED_GOAT;

        new_vis_section = (struct visual_section *) malloc(sizeof(struct visual_section));
        new_vis_section->color = section_color;
        new_vis_section->abs_begin = abs_begin;
        new_vis_section->abs_end = abs_end;
#ifdef VERV_CODO
        new_vis_section->code = -1;
#endif
        new_vis_section->next = NULL;

        curr_vis_section = vis_section_list;
        if(new_vis_section->abs_begin < curr_vis_section->abs_begin)
          {
            new_vis_section->next = vis_section_list;
            vis_section_list = new_vis_section;
          }
        else
          {
            while(curr_vis_section && (new_vis_section->abs_begin > curr_vis_section->abs_begin))
            {
              if(curr_vis_section->next)
                {
                  if(curr_vis_section->next->abs_begin > new_vis_section->abs_begin)
                  {
                    new_vis_section->next = curr_vis_section->next;
                    curr_vis_section->next = new_vis_section;
                    break;
                  }
                }
              else
                {
                  curr_vis_section->next = new_vis_section;
                  break;
                }
              curr_vis_section = curr_vis_section->next;
            }
          }
#if 0
        fprintf(viz_out,"type %d box %d %d \n",section_color , abs_begin, abs_end);
#endif

        r_list = r_list->next;
      }

      curr_vis_section = vis_section_list;
      while(curr_vis_section)
      {
#ifdef VERV_CODO
        if(curr_vis_section->code == VERV_CODE)
          fprintf(viz_out,"ctype %d box %d %d \n",
              curr_vis_section->color,
              curr_vis_section->abs_begin,
              curr_vis_section->abs_end);
        else
#endif
          fprintf(viz_out,"type  %d box %d %d \n",
              curr_vis_section->color,
              curr_vis_section->abs_begin,
              curr_vis_section->abs_end);
        curr_vis_section = curr_vis_section->next;
      }

      fprintf(viz_out, "\nentry %d\n\n", anchorpos[1]); 

      fclose(viz_out);
    }

	// 6/26/97 Fred: Free memory
	while (vis_section_list) {
		curr_vis_section = vis_section_list->next;
		free (vis_section_list);
		vis_section_list = curr_vis_section;
	}

  return;
}






