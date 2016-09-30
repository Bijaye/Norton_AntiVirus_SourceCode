#include <stdlib.h>  
#include <stdio.h>
/*#include <values.h>*/
#include "autoverv.h"

void write_vp(char *infected_dir, struct map_rec *map_list[file_types], struct reconstruction_list *recon_list[file_types])
{
  int i,j;
  FILE *vp_out;
  char vp_name[256];
  char virusname[256];
  char ftype[5];
  int curr_pos, anchorpos[3], section_color;
  int b_offset, e_offset;
  int curr_anchor, b_anchor, e_anchor;
  int xmin, xmax;
  int crypt_type;

  int r_anchor, r_begin, r_end;

  struct map_rec *m_list;
  struct reconstruction_list *r_list;
  struct match_list *vmatch;
  struct single_boundary tmp_begin, tmp_end;

  for(i=0;i<file_types;i++)
    {
      for(j=strlen(infected_dir)-2;(j>=0) && (infected_dir[j]!='/');j--);
      strcpy(virusname,&(infected_dir[j+1]));
      virusname[strlen(virusname)-1] = '\0';

      strcpy(vp_name,virusname);

      if(i==0)
      strcpy(ftype,".com");
      else
      strcpy(ftype,".exe");

      strcat(vp_name,ftype);

      strcat(vp_name,".exg");
      vp_out = fopen(vp_name,"w");

      fprintf(vp_out,"# Virus %s\n", virusname);
      fprintf(vp_out,"# Victim type: (%s.)\n\n",ftype);
      
      m_list = map_list[i];

      curr_pos = 0;
      curr_anchor = 0;
      for(j=0;j<3;j++) anchorpos[j] = 0;

      while(m_list)
      {
        get_begin_boundary(&tmp_begin,m_list,i);
        get_end_boundary(&tmp_end,m_list,i);

        b_anchor = tmp_begin.bestanchor;
        b_offset = tmp_begin.offset_min;
        e_anchor = tmp_end.bestanchor;
        e_offset = tmp_end.offset_min;

        if(b_anchor != curr_anchor)
          {
            curr_anchor = b_anchor;
            while(curr_pos > anchorpos[curr_anchor])
            anchorpos[curr_anchor]+=1000;
          }

        curr_pos = anchorpos[curr_anchor]+b_offset;

        if(e_anchor != curr_anchor)
          {
            curr_anchor = e_anchor;
            while(curr_pos > anchorpos[curr_anchor])
            anchorpos[curr_anchor]+=1000;
          }

        curr_pos = anchorpos[curr_anchor]+e_offset;

        m_list=m_list->next;
      }

      xmin = -100;
      xmax = anchorpos[curr_anchor]+e_offset + 100;
      
      fprintf(vp_out,"\naxis invisible xmin %d xmax %d ymin 0.5 ymax 2.5\n",xmin,xmax);

      m_list = map_list[i];

      curr_pos = 0;
      curr_anchor = 0;

      while(m_list)
      {
        get_begin_boundary(&tmp_begin,m_list,i);
        get_end_boundary(&tmp_end,m_list,i);
        
        if(m_list->record_type == 'V')
          {
            section_color = 4;

            if((vmatch = (struct match_list *)(m_list->record_ptr)))
            {

              if(vmatch->match)
                crypt_type = vmatch->match->type;
              else
                crypt_type = 0;
              
              if(crypt_type)
                section_color = 7;

            }
            else
              crypt_type = 0;

          }
        else
          section_color = 2;

        b_anchor = tmp_begin.bestanchor;
        b_offset = tmp_begin.offset_min;
        e_anchor = tmp_end.bestanchor;
        e_offset = tmp_end.offset_min;

        while(b_anchor != curr_anchor)
          curr_anchor++;

        if(curr_pos < anchorpos[curr_anchor]+b_offset)
          fprintf(vp_out,"color 5\nline box %d 1.25 %d 1.75 fill\n",curr_pos+1,anchorpos[curr_anchor]+b_offset-1);

        curr_pos = anchorpos[curr_anchor]+b_offset;
        
        fprintf(vp_out,"color %d\n",section_color);

        fprintf(vp_out,"line box %d 1.25 ",curr_pos);

        while(e_anchor != curr_anchor)
          curr_anchor++;
          
        fprintf(vp_out ,"%d 1.75 fill\n",anchorpos[curr_anchor]+e_offset);
        
        if(((anchorpos[curr_anchor]+e_offset - curr_pos) > 250) && (m_list->record_type == 'G'))
          {
            fprintf(vp_out,"\nc2font 21150\n");
            fprintf(vp_out,"c2color 0\n");
            fprintf(vp_out,"comment %d 1.5 center\n", (anchorpos[curr_anchor]+e_offset + curr_pos)/2);
            fprintf(vp_out,"\\Host\\ \n");
          }
        else if(m_list->record_type == 'V')
          {
            if((anchorpos[curr_anchor]+e_offset - curr_pos > 600))
            {
              fprintf(vp_out,"\nc2font 21150\n");
              fprintf(vp_out,"c2color 0\n");
              fprintf(vp_out,"comment %d 1.5 center\n", (anchorpos[curr_anchor]+e_offset + curr_pos)/2);
              fprintf(vp_out,"\\%s\\ \n", virusname);
            }
            if(((anchorpos[curr_anchor]+e_offset - curr_pos) > 150) && crypt_type)
            {
              fprintf(vp_out,"\nc2font 12100\n");
              fprintf(vp_out,"c2color 0\n");
              fprintf(vp_out,"comment %d 1.35 center\n", (anchorpos[curr_anchor]+e_offset + curr_pos)/2);
              fprintf(vp_out,"\\(%s)\\ \n", encrypt_names[crypt_type]);
            }           
          }

        curr_pos = anchorpos[curr_anchor]+e_offset;

        m_list=m_list->next;
      }

      r_list = recon_list[i];

      while(r_list)
      {
        r_anchor = r_list->ig_begin.anchor_type;
        r_begin = r_list->ig_begin.offset;
        r_end = r_begin+r_list->length-1;
        fprintf(vp_out,"color 2\nline box %d 1.25 %d 1.75 fill\n",
              anchorpos[r_anchor]+r_begin,
              anchorpos[r_anchor]+r_end);
        r_list = r_list->next;
      }
      
      fprintf(vp_out,"xydata \n");
      fprintf(vp_out,"%d 1.25\n",anchorpos[1]);
      fprintf(vp_out,"%d 1.75\n",anchorpos[1]);
      fprintf(vp_out,"curve isym -1 iline 1 color 0\n\n"); 
      
      fprintf(vp_out,"c2font 12180\n");
      fprintf(vp_out,"c2color 7\n");
      fprintf(vp_out,"comment %d 0.75 center\n",(int) ((xmin+xmax)/2));
      fprintf(vp_out,"Infection of %s programs by %s\n\n",ftype,virusname);

      fprintf(vp_out,"\n");
      fprintf(vp_out,"view erase fill\n");

      fclose(vp_out);
    }
  return;
}






