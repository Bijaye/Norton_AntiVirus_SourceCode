#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "autoverv.h"


void write_bytes(char *infected_dir,struct sample_data *sample, 
             struct match_list *virus_match)
  {
    struct match_list *vmatch;
    FILE *bytes_file;
    char name[256];
    int i, startpos;
    
    /* pick last directory as virus name */
    for(i=strlen(infected_dir)-2;(i>0) && (infected_dir[i] != pathchar);i--);
    if(infected_dir[i] == pathchar) i++;
    strcpy(name,&(infected_dir[i]));
    //name[strlen(name)-1]=0;
    strcat(name,".bytes");
    bytes_file = fopen(name,"w");
    if(!bytes_file)
      {
      printf("Error opening %s!\n",name);
      return;
      }

    vmatch = virus_match;
    while(vmatch)
      {
      if(vmatch->match->type == 0)   /* if plain */
        {
          /* print bytes */
          startpos = vmatch->match->pos_in_sample[0];
          for(i=0;i<vmatch->match->length;i++)
            fprintf(bytes_file,"%02X",sample->bytes[i+startpos]);
          /* print separator of 48 dots */
          fprintf(bytes_file,"................................................");
        }
      vmatch=vmatch->next;
      }
    fprintf(bytes_file,"\n");
    fclose(bytes_file);
}
