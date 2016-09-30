#include <stdlib.h>
#include <stdio.h>
/*#include <values.h>*/
#include "autoverv.h"


char *signstr(int number);
char *type_name(int type);
void vds_insert(struct vds_region **vds_list, struct vds_region *newRegion);
void vds_compress(struct vds_region **vds_list, int usetype);
int get_buffer_offset(struct vds_region *vds_list, int anchor, int offset, int filetype, struct vds_region **vds_result);
int get_buffer_size(struct vds_region *vds_list, int filetype);

/* This element of linked list is used to keep track of the bytes of the Verv repair buffer that have been decrypted */
/* The purpose of this is to avoid deXORing several times the same bytes of the buffer when they serve to repair several */
/* types of files */
typedef struct buffer_segment_item {
	int ofs;
	int len;
	struct buffer_segment_item * next;
} bsi;

void write_vds(FILE *vds_out, char *infected_dir, struct sample_data **samples, int num_samples,
             struct map_rec *map_list[file_types], struct universal_goat_match *uni_match[file_types],
             int num[file_types], int num_with_goats[file_types],
             struct reconstruction_list *recon_list[file_types],
             int num_missing[file_types], int calc_mz[file_types])

  {
    int i,j;
    char name[256];
    struct match_list *vmatch;
    struct universal_goat_match *gmatch;
    struct reconstruction_list *rlist;
    struct map_rec *m_list;
    int begin, end, anchor;
    int bytes=0;
    int usetype,filetype, curroffset, curranchor;
    int ok;
    struct vds_region *vds_list=NULL, *vds_reg;
	struct vds_region *begin_region = NULL;
	struct vds_region *end_region = NULL;
	struct vds_region *key_region = NULL;
	int begin_found;
	int end_found;
	bsi * decrypted_buffer_bytes = NULL;
	bsi * dbbitem = NULL;
	//bsi * prevdbbitem = NULL;
	int rlen;
    /*polyn *crc_table;*/


    /* pick last directory as virus name */
    for(i=strlen(infected_dir)-2;(i>0) && (infected_dir[i] != pathchar);i--);
    if(infected_dir[i] == pathchar) i++;
    strcpy(name,&(infected_dir[i]));
    name[strlen(name)-1]=0;
    for(j=0;(j<strlen(name)) && (name[j]!='.');j++);
    name[j]=0;

    /* indicate the virus name */
    fprintf(vds_out,"VIRUS %s\n",name);
    fprintf(vds_out,"NAME %s\n",name);
   
    /* pick a type to base things off of */
    if(num[COM])
      usetype = COM;
    else
      usetype = EXE;

    /* write FLOAD lines */
    m_list = map_list[usetype];

    while(m_list)  /* go through each region */
      {
      if(m_list->record_type == 'V')  /* if a virus section */
        {
          /* cast as virus section */
          vmatch = (struct match_list *)(m_list->record_ptr);
          
          /* decide if the range is zero for all types */
          ok = 1;
          for(filetype=0;filetype<file_types;filetype++)
            if(num[filetype] && (vmatch->begin.offset_range[filetype] != 0))
            ok = 0;

          if(ok)  /* if the range on this region is zero for all filetypes */
            {
            /* decide if ALL can work for this region */
            ok = 1;
            for(filetype=0;filetype<file_types;filetype++)
              if(!num[filetype] || 
                 (num[filetype] && (vmatch->begin.offset_range[ALL] > vmatch->begin.offset_range[filetype])))
                ok=0;

            if(ok)  /* if ALL can work */
              {
                /* allocate a new record */
                vds_reg = (struct vds_region *)malloc(sizeof(struct vds_region));

                vds_reg->filetype = ALL;
                vds_reg->begin.anchor_type = vmatch->begin.bestanchor[ALL];
                vds_reg->begin.offset = vmatch->begin.offset_min[ALL];
                vds_reg->length = vmatch->match->length;
                vds_reg->useless_bytes = 0;

                /* insert into list */
                vds_insert(&vds_list,vds_reg);
              }
            else
              for(filetype=0;filetype<file_types;filetype++)  /* for each filetype */
                if(num[filetype])
                  {
                  /* allocate a new record */
                  vds_reg = (struct vds_region *)malloc(sizeof(struct vds_region));
                  
                  vds_reg->filetype = filetype;
                  vds_reg->begin.anchor_type = vmatch->begin.bestanchor[filetype];
                  vds_reg->begin.offset = vmatch->begin.offset_min[filetype];
                  vds_reg->length = vmatch->match->length;
                  vds_reg->useless_bytes = 0;
                  
                  /* insert into list */
                  vds_insert(&vds_list,vds_reg);
                  }
            }
        }
      m_list = m_list->next;
      }

    /* optimize the buffer usage (to an extent) */
    vds_compress(&vds_list,usetype); //DEBUG uncomment this later


    /* load the reconstruction bytes */
    for(filetype=0;filetype<file_types;filetype++)
      {
      rlist = recon_list[filetype];
      while(rlist)
        {
             /* if there is a real need to reconstruct this byte */
          if(rlist->encryption_info.crypt_type || (rlist->g_begin.offset != rlist->ig_begin.offset)
             || (rlist->g_begin.anchor_type != rlist->ig_begin.anchor_type))
            {
            if(rlist->encryption_info.crypt_type)  /* get key if necessary */
              {
                anchor = rlist->encryption_info.key_location.anchor_type;
                begin = rlist->encryption_info.key_location.offset;
                end = begin+key_length[rlist->encryption_info.crypt_type]-1;

                /* if key byte(s) are not in the buffer already */
                if((get_buffer_offset(vds_list,anchor,begin,filetype,&begin_region) < 0) 
                   || (get_buffer_offset(vds_list,anchor,end,filetype,&end_region) < 0))
                  {  
                  /* just add the bytes to end of buffer -- could be improved at */
                  /* some point to adjust existing regions... */
                  vds_reg = (struct vds_region *)malloc(sizeof(struct vds_region));
                  vds_reg->filetype = filetype;
                  vds_reg->begin.anchor_type = anchor;
                  vds_reg->begin.offset = begin;
                  vds_reg->length = end-begin+1;
                  vds_reg->useless_bytes = 0;

                  /* insert into list */
                  vds_insert(&vds_list,vds_reg);
                  }
              }
                
            anchor = rlist->ig_begin.anchor_type;
            begin = rlist->ig_begin.offset;
            end = begin + rlist->length - 1;

			begin_found = get_buffer_offset(vds_list,anchor,begin,filetype,&begin_region);
			/* begin_found is -1 if we could not find the begin byte, positive otherwise */
			end_found = get_buffer_offset(vds_list,anchor,end,filetype,&end_region);
			/* end_found is -1 if we could not find the end byte, positive otherwise */

            /* if these bytes are not in the buffer already */
            if(begin_found < 0 ||  end_found < 0)
              {
                /* just add the bytes to end of buffer -- could be improved at */
                /* some point to adjust existing regions... */
                vds_reg = (struct vds_region *)malloc(sizeof(struct vds_region));
                vds_reg->filetype = filetype;
                vds_reg->begin.anchor_type = anchor;
                vds_reg->begin.offset = begin;
                vds_reg->length = end-begin+1;
                vds_reg->useless_bytes = 0;

                /* insert in list */
                vds_insert(&vds_list,vds_reg);
              }
            }
          rlist = rlist->next;
        }
      }


    /* output the FLOAD lines */
    vds_reg = vds_list;
    while(vds_reg)
      {
      begin = vds_reg->begin.offset;
      end = begin + vds_reg->length - 1;
      anchor = vds_reg->begin.anchor_type;
      fprintf(vds_out,"FLOAD %s %c %s%x %c %s%x\n",type_name(vds_reg->filetype),
            anchorchar[anchor],signstr(begin),abs(begin),
            anchorchar[anchor],signstr(end),abs(end));

      vds_reg = vds_reg->next;
      }



    /* print appropriate warning if buffer is too big */
    for(filetype=0;filetype<file_types;filetype++)
      if(get_buffer_size(vds_list,filetype) > verv_buffer_max)
      fprintf(vds_out,"* Caution: buffer size for %s exceeds current VERV-imposed maximum!\n",
            type_name(filetype));


    /* decrypt stuff needed for repair (before verification, while keys are still valid) */
	// Fred: Problem (30/6/97), we decrypt some bytes twice !
	// This comes from the vds_list where some bytes appear several times for each type of file

	 for(filetype=0;filetype<file_types;filetype++)
      {
      rlist = recon_list[filetype];
      while(rlist)
        {
          if(rlist->encryption_info.crypt_type)  /* if bytes are encrypted */
            { /* output a line to decrypt them */
            
            /* this is to try to deal with XOR2 until it is properly fixed */
            if((rlist->encryption_info.crypt_type == 3) && (rlist->length == 1))
              {
                rlist->encryption_info.crypt_type = 1;  /* make an XOR1 instead */
              }
            /*********/

//DEBUG HERE MAKE SURE WE ELIMINATE THE RIGHT BYTES
            
			
			anchor = rlist->ig_begin.anchor_type;
            begin = get_buffer_offset(vds_list,anchor,rlist->ig_begin.offset,filetype,&begin_region);
			rlen = rlist->length;
			/* Go to the end of the linked list of decrypted buffer sections */
			/* Then allocate new section */
			/* Deal with empty linked list */
			if (decrypted_buffer_bytes == NULL) {
				decrypted_buffer_bytes = (bsi*)malloc(sizeof(bsi));
				decrypted_buffer_bytes->ofs = begin;
				decrypted_buffer_bytes->len = rlen;
				decrypted_buffer_bytes->next = NULL;
			} else {
				dbbitem = decrypted_buffer_bytes;
				while (dbbitem->next) {
					if (dbbitem->ofs <= begin && dbbitem->ofs+dbbitem->len > begin) {
						/* This can lead to negative rlen */
						begin = dbbitem->ofs+dbbitem->len;
						rlen -= dbbitem->ofs+dbbitem->len-begin;
					}
					dbbitem = dbbitem->next;
				}
				if (dbbitem->ofs <= begin && dbbitem->ofs+dbbitem->len > begin) {
					/* This can lead to negative rlen */
					begin = dbbitem->ofs+dbbitem->len;
					rlen -= dbbitem->ofs+dbbitem->len-begin;
				}
				if (rlen>0) {
					dbbitem->next = (bsi*)malloc(sizeof(bsi));
					dbbitem->next->ofs = begin;
					dbbitem->next->len = rlen;
					dbbitem->next->next = NULL;
				}
			}
			if (rlen>0)
			fprintf(vds_out,"%s %x %x %x 0 %x\n",encrypt_names[rlist->encryption_info.crypt_type],
                  begin, begin+rlen-1,
                  get_buffer_offset(vds_list, rlist->encryption_info.key_location.anchor_type,
                                rlist->encryption_info.key_location.offset,filetype,&key_region),
                  rlist->encryption_info.key_correction[0]);
            }
          rlist = rlist->next;
        }
      }


    /* write decrypt/CONST lines for verfication */
    m_list = map_list[usetype];
    while(m_list)
      {
      if(m_list->record_type == 'V')  /* if a virus section */
        {
          vmatch = (struct match_list *)(m_list->record_ptr);

          /* exclude useless sections*/
          if(vmatch->code && (vmatch->match->length>=5))
          {
          /* decide if the range is zero for all types */
          ok = 1;
          for(filetype=0;filetype<file_types;filetype++)
            if(num[filetype] && (vmatch->begin.offset_range[filetype] != 0))
            ok = 0;

          if(ok)
            /* if we can specify region precisely */
            {
            /* decide if ALL can work for this region */
            ok = 1;
            for(filetype=0;filetype<file_types;filetype++)
              if(!num[filetype] || 
                 (num[filetype] && (vmatch->begin.offset_range[ALL] > vmatch->begin.offset_range[filetype])) )
                ok=0;
            if(ok)
              filetype=ALL;
            else
              filetype=usetype;
               
            begin = get_buffer_offset(vds_list,vmatch->begin.bestanchor[filetype],
                                vmatch->begin.offset_min[filetype],filetype,&begin_region);
            end = begin + vmatch->match->length - 1;

            if(vmatch->match->type)  /* if decryption is needed */
              fprintf(vds_out,"%s %x %x %x 0 0\n",encrypt_names[vmatch->match->type], begin, end, begin);

            fprintf(vds_out,"CONST %x %x %08x\n",begin,end,vmatch->crc);
            bytes += vmatch->match->length;
            /* riad fflush(vds_out); */
              }
            }
        }
      m_list = m_list->next;
      }
    fprintf(vds_out,"* Bytes that will be verified: %d (0x%x)\n",bytes,bytes);



    /* do the repair stuff */
    for(filetype=0;filetype<file_types;filetype++)
      if(num_with_goats[filetype]>1)
      {
        /* point to list of goat sections for repair */
        gmatch = uni_match[filetype];
  
        if(gmatch)  /* if something to do */
          {
            fprintf(vds_out,"REPAIR %s\n",type_name(filetype));
            
            /* do the FCOPYs to get the bulk of the infected program back */
            curroffset = 0;
            curranchor = 0;
            while(gmatch) 
            {
              if(gmatch->g_begin.bestanchor == curranchor)
                {
                  begin = gmatch->ig_begin.offset_min - 
                  (gmatch->g_begin.offset_min-curroffset);   /* include variable bytes (hopefully we'll reconstruct) */
                }
              else
                begin = gmatch->ig_begin.offset_min;

              end = gmatch->ig_end.offset_min + gmatch->ig_end.offset_range;  /* include padding */

              fprintf(vds_out,"  FCOPY %c %s%x %c %s%x\n",anchorchar[gmatch->ig_begin.bestanchor],
                    signstr(begin),abs(begin),anchorchar[gmatch->ig_end.bestanchor],
                    signstr(end),abs(end));

              curroffset = gmatch->g_end.offset_min+1;
              curranchor = gmatch->g_end.bestanchor;
              gmatch = gmatch->next;
            }

            /* output the BWRITEs */
            rlist = recon_list[filetype];
            while(rlist)
            {
              /* if bytes really need to be reconstructed */
              if(rlist->encryption_info.crypt_type || (rlist->g_begin.offset != rlist->ig_begin.offset)
                 || (rlist->g_begin.anchor_type != rlist->ig_begin.anchor_type) )
                {
                  fprintf(vds_out,"  BWRITE %x %x %x\n",
                        get_buffer_offset(vds_list,rlist->ig_begin.anchor_type,rlist->ig_begin.offset,filetype,&begin_region),
                        rlist->g_begin.offset,rlist->length);
                }
              rlist = rlist->next;
            }

            /* if we couldn't reconstruct all important bytes, print warning */
            if(num_missing[filetype])
            fprintf(vds_out,"  * WARNING: %d bytes could not be reconstructed!\n",num_missing[filetype]);
            
            /* if needed, add calc_mz_length */
            if(calc_mz[filetype])
            {
              fprintf(vds_out,"  CALC_MZ_LENGTH\n");
              fprintf(vds_out,"  BWRITE 0 0 6\n");
            }

          }
        else
          fprintf(vds_out,"* Unable to repair %s (probable overwriting virus)\n",type_name(filetype));
      }
      else 
      fprintf(vds_out,"* Unable to repair %s (not enough samples)\n",type_name(filetype));

	// 6/26/97 Fred: Release memory
	while (vds_list) {
		vds_reg = vds_list->next;
		free (vds_list);
		vds_list = vds_reg;
	}

    fprintf(vds_out,"\n\n");
  }


char *signstr(int number)
  {
    static char neg[2] = "-";
    static char none[2] = "";

    if(number < 0)
/* modified by riad */
      return(/*&*/ neg);
    else
      return(/*&*/ none);
  }


char *type_name(int type)
 {
    static char s_exe[6]  = "S-EXE";
    static char s_com[6]  = "S-COM";
    static char s_file[7] = "S-FILE";
    static char err[4]    = "ERR";
/* modified by riad */
    switch(type) {
    case COM:
      return(/*&*/ s_com);
    case EXE:
      return(/*&*/ s_exe);
    case ALL:
      return(/*&*/ s_file);
    default:
      return(/*&*/ err);
    }
  }


void vds_insert(struct vds_region **vds_list, struct vds_region *newRegion)
  {
    struct vds_region *curr;
    int buff_offset = 0;

    /* if an empty list or this should be first rec */
    if(!(*vds_list) || (((*vds_list)->filetype != ALL) && (newRegion->filetype == ALL)))
      {
      newRegion->next = *vds_list;
      *vds_list = newRegion;
      }
    else if(newRegion->filetype == ALL)  /* if an ALL, it goes first in the list */
      {
      curr=*vds_list;
      buff_offset = curr->length;
      /* find last ALL */
      while(curr->next && (curr->next->filetype == ALL))
        {
          curr=curr->next;
          buff_offset += curr->length;
        }
      /* insert after it */
      newRegion->next = curr->next;
      curr->next = newRegion;
      }
    else  /* if not, insert at end of list */
      {
      curr=*vds_list;
      if((curr->filetype == newRegion->filetype) || (curr->filetype == ALL))
        buff_offset = curr->length;
      /* find last node in list */
      while(curr->next)
        {
          curr=curr->next;
          
          /* keep track of buffer offset as we go */
          if((curr->filetype == newRegion->filetype) || (curr->filetype == ALL))
            buff_offset += curr->length;
	  }
      /* insert after it */
      newRegion->next = NULL;
      curr->next = newRegion;
      }
    newRegion->buffer_offset = buff_offset;
  }


/* optimizes the buffer usage to create minimal LOAD type lines */
/* this isn't pretty ... probably could be made prettier */
void vds_compress(struct vds_region **vds_list, int usetype)
  {
    struct vds_region *start,*curr, *old, *prev;
    int curranchor, buff_offset, useless, filetype;

    if(!(*vds_list))  /* if no list for some reason, just quit */
      return;

    /* compress ALLs */
    start = *vds_list;
    while(start && (start->filetype == ALL))
      {
      curr = start->next;
      curranchor = start->begin.anchor_type;

      /* find last ALL record with this anchor type */
      while(curr && curr->next && (curr->next->filetype == ALL) && 
            (curr->next->begin.anchor_type == curranchor))
        {
          start->next = curr->next;  /* delete from list */
          old = curr;
          curr = curr->next;  /* go to next */
          free(old);
        }
      
      /* adjust start & delete last record */
      if(curr && (curr->filetype == ALL))
        {
          start->length = curr->begin.offset - start->begin.offset + curr->length;
          start->next = curr->next;  /* delete from list */
          old = curr;
          curr = curr->next;  /* go to next */
          free(old);
        }
      start = start->next;  /* go on */
      }
    

    /* optimize the usetype records */
    start = *vds_list;
    while(start && (start->filetype != usetype))
      start = start->next;  
    while(start && (start->filetype == usetype) && start->next)
      {
      prev = start;
      curr = start->next;
      while(curr->next && (curr->filetype != usetype))
        {
          prev = curr;
          curr = curr->next;   /* get next rec of this type */
        }
      curranchor = start->begin.anchor_type;

      useless = 0;
      /* find last usetype record with this anchor type */
      while(curr)
        {
          if((curr->filetype == usetype) && (curr->begin.anchor_type == curranchor))
            {
            useless = 1 + curr->begin.offset - (start->begin.offset + start->length - 1);
            /* compute new length -- depends on fact that recs are in order */
            start->length = curr->begin.offset + curr->length - start->begin.offset;  
            prev->next = curr->next;
            old = curr;
            curr = curr->next;
            free(old);
            }
          else
            {
            curr->useless_bytes = useless;
            prev = curr;
            curr = curr->next;
            }
        }
      
      start = start->next;
      while(start && (start->filetype != usetype))
        start = start->next;  /* go on */
      }


    /* optimize the rest of the records */
    for(filetype=usetype+1;filetype < file_types;filetype++)  /* for each remaining filetype */
      {
      start = *vds_list;
      /* get first rec of this filetype */
      while(start && (start->filetype != filetype))
        start=start->next;
      while(start)
        {
          prev = start;
          curr = start->next;
          curranchor = start->begin.anchor_type;

          while(curr && (curr->filetype != filetype))  /* find next record */
            {
            prev = curr;
            curr = curr->next;
            }
          
          while(curr)
            {
            useless = 1 + curr->begin.offset - (start->begin.offset + start->length - 1);

            if((curr->begin.anchor_type == curranchor) && (useless == curr->useless_bytes))
              {  /* combine */
                /* check if beginning needs to move back */
                if(curr->begin.offset < start->begin.offset)  
                  start->begin.offset = curr->begin.offset;

                /* check if ending needs to move forward */
                if( (curr->begin.offset + curr->length - 1) > (start->begin.offset + start->length - 1))
                  start->length = curr->begin.offset + curr->length - start->begin.offset;

                /* delete curr */
                prev->next = curr->next;
                old = curr;
                curr = curr->next;
                while(curr && (curr->filetype != filetype))  /* find next record */
                  {
                  prev = curr;
                  curr = curr->next;
                  }
                free(old);
              }
            else
              {  /* keep separate */
                curr->begin.offset -= curr->useless_bytes;  /* adjust to make things look like usetype */
                curr = NULL;  /* signal that we're done */
              }
            }

          start = start->next;
          while(start && (start->filetype != filetype))
            start=start->next;
        }
      }
    

    /* finally, fix-up buffer offsets */
    for(filetype=0;filetype<file_types;filetype++)
      {
      curr = *vds_list;
      buff_offset = 0;
      /* find end of ALLs */
      while(curr && ((curr->filetype == ALL) || (curr->filetype == filetype)))
        {
          curr->buffer_offset = buff_offset;
          buff_offset += curr->length;
          curr = curr->next;
        }
      }
  }    


int get_buffer_offset(struct vds_region *vds_list,int anchor, int offset, int filetype, struct vds_region **vds_result)
  {
    struct vds_region *curr;

	*vds_result = NULL;
    curr = vds_list;
    while(curr)
      {
      /* if the desired anchor offset is covered by current region */
      if((curr->filetype == ALL || curr->filetype == filetype) &&
		 curr->begin.anchor_type == anchor &&
		 curr->begin.offset <= offset &&
		 curr->begin.offset+curr->length-1 >= offset
		 ) {
			*vds_result = curr;	/* Return the region in which we found the given byte */
			return(curr->buffer_offset + (offset - curr->begin.offset));  /* return offset */
	  }
      curr = curr->next;  /* try next one */
      }
    return(-1);  /* if nothing found, return -1 */
  }


int get_buffer_size(struct vds_region *vds_list, int filetype)
  {
    struct vds_region *curr;
    int lastoffset=0;

    curr = vds_list;
    while(curr)
      {
      if((curr->filetype == ALL) || (curr->filetype == filetype))
        lastoffset = curr->buffer_offset + curr->length - 1;
      curr = curr->next;
      }
    return(lastoffset);
  }
