//???
//AJR temp?
//   Modify bytes2 to only write out sections longer than MIN_SECTION_LENGTH long.
//   bytes3 is bytes2 as it used to be, called now when bytes2 is finished.


#include <stdlib.h>
#include <stdio.h>
/*#include <values.h>*/
#include "autoverv.h"

//AJR temp?
#define MIN_SECTION_LENGTH 4
static void write_bytes3(char *infected_dir, struct sample_data *sample, 
                         struct map_rec *map_list[file_types]);



void write_bytes2(char *infected_dir, struct sample_data *sample, 
                  struct map_rec *map_list[file_types], int useCode_DataSegregation)
{
  int i,j, ft, startpos;
  FILE *out;
  char out_name[256];
  char virusname[256];
  char ftype[5];

  struct map_rec *m_list;
  struct match_list *vmatch;
  struct single_boundary tmp_begin/*, tmp_end*/;

  int flag;

  for(j=strlen(infected_dir)-2;(j>=0) && (infected_dir[j]!='/');j--);
  strcpy(virusname,&(infected_dir[j+1]));
  //virusname[strlen(virusname)-1] = '\0';


  for(ft=0, flag=0;(ft<file_types) && (!flag) ;ft++)
    {
      strcpy(out_name,virusname);

      if(ft==0) strcpy(ftype,".com");
      else if (ft==1) strcpy(ftype,".exe");

/*      strcat(out_name,ftype); */

      if(map_list[ft] && map_list[ft]->record_ptr)
      {
        strcat(out_name,".bytes2");
        out = fopen(out_name,"w");
        
        fprintf(out, "<VIRUS> %s\n", virusname);
        fprintf(out, "<DIRECTORY> %s\n", infected_dir);
        flag = 1;
      }
      else
      {
        flag = 0;
        continue;
      }

/*    fprintf(out, "<FTYPE> %s\n\n", &(ftype[1])); */

      m_list = map_list[ft];
      while(m_list)
      {
        if(m_list->record_type == 'V')
          {
            if((vmatch = (struct match_list *)(m_list->record_ptr)))
            {
              if(vmatch->match
              && (!useCode_DataSegregation || vmatch->match->length >= MIN_SECTION_LENGTH)
              )   // AJR temp?
                {
                  get_begin_boundary(&tmp_begin,m_list,ft);
                  fprintf(out,"<MARKER> %c\n",anchorchar[tmp_begin.bestanchor]);
                  fprintf(out,"<OFFSET> %d\n",tmp_begin.offset_min);
                  fprintf(out,"<LENGTH> %d\n",vmatch->match->length);
                  fprintf(out,"<CRYPT> %s\n", encrypt_names[vmatch->match->type]);
#ifdef VERV_CODO
                  fprintf(out,"<CODE>  %s\n", (vmatch->code)?"YES":"NO");
#endif
                  startpos = vmatch->match->pos_in_sample[0];
                  fprintf(out,"<BYTES> ");
                  for(i=0;i<vmatch->match->length;i++)
                        fprintf(out,"%02X",sample->bytes[i+startpos]);
                  fprintf(out, "\n\n");
                }
            }
          }
        m_list = m_list->next;
      }

      fclose(out);
    }
  if (useCode_DataSegregation)
     write_bytes3(infected_dir,sample,map_list);   // AJR temp?

  return;
}

//AJR temp?

static void write_bytes3(char *infected_dir, struct sample_data *sample, struct map_rec *map_list[file_types])
{
  int i,j, ft, startpos;
  FILE *out;
  char out_name[256];
  char virusname[256];
  char ftype[5];

  struct map_rec *m_list;
  struct match_list *vmatch;
  struct single_boundary tmp_begin/*, tmp_end*/;

  int flag;

  for(j=strlen(infected_dir)-2;(j>=0) && (infected_dir[j]!='/');j--);
  strcpy(virusname,&(infected_dir[j+1]));
  //virusname[strlen(virusname)-1] = '\0';


  for(ft=0, flag=0;(ft<file_types) && (!flag) ;ft++)
    {
      strcpy(out_name,virusname);

      if(ft==0) strcpy(ftype,".com");
      else if (ft==1) strcpy(ftype,".exe");

/*      strcat(out_name,ftype); */

      if(map_list[ft] && map_list[ft]->record_ptr)
      {
        strcat(out_name,".bytes3");
        out = fopen(out_name,"w");
        
        fprintf(out, "<VIRUS> %s\n", virusname);
        fprintf(out, "<DIRECTORY> %s\n", infected_dir);
        flag = 1;
      }
      else
      {
        flag = 0;
        continue;
      }

/*    fprintf(out, "<FTYPE> %s\n\n", &(ftype[1])); */

      m_list = map_list[ft];
      while(m_list)
      {
        if(m_list->record_type == 'V')
          {
            if((vmatch = (struct match_list *)(m_list->record_ptr)))
            {
              if(vmatch->match)
                {
                  get_begin_boundary(&tmp_begin,m_list,ft);
                  fprintf(out,"<MARKER> %c\n",anchorchar[tmp_begin.bestanchor]);
                  fprintf(out,"<OFFSET> %d\n",tmp_begin.offset_min);
                  fprintf(out,"<LENGTH> %d\n",vmatch->match->length);
                  fprintf(out,"<CRYPT> %s\n", encrypt_names[vmatch->match->type]);
#ifdef VERV_CODO
                  fprintf(out,"<CODE>  %s\n", (vmatch->code)?"YES":"NO");
#endif
                  startpos = vmatch->match->pos_in_sample[0];
                  fprintf(out,"<BYTES> ");
                  for(i=0;i<vmatch->match->length;i++)
                        fprintf(out,"%02X",sample->bytes[i+startpos]);
                  fprintf(out, "\n\n");
                }
            }
          }
        m_list = m_list->next;
      }

      fclose(out);
    }
  return;
}
