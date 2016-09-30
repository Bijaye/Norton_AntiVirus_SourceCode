#include <stdio.h>
#include <stdlib.h>
#include "autoverv.h"

int compare_map_rec(struct map_rec *rec1, struct map_rec *rec2, int filetype);

void insert_map_rec(struct map_rec **map_list, char type, void *rec_ptr, int filetype);


struct map_rec *make_map_list(struct match_list *virus_match, 
                        struct universal_goat_match *uni_goat_match,
                        int filetype)

  {
    struct match_list *vmatch;
    struct universal_goat_match *gmatch;
    struct map_rec *map_list=NULL;

    /* point to beginning of the component lists */
    vmatch = virus_match;
    gmatch = uni_goat_match;

    while(vmatch)
      {
      insert_map_rec(&map_list,'V',vmatch,filetype);
      vmatch = vmatch->next;
      }

    while(gmatch)
      {
      insert_map_rec(&map_list,'G',gmatch,filetype);
      gmatch = gmatch->next;
      }
    
    return(map_list);
  }


void insert_map_rec(struct map_rec **map_list, char type, void *rec_ptr, int filetype)
  { 
    struct map_rec *newMatch, *curr;

    newMatch = (struct map_rec *)malloc(sizeof(struct map_rec));
    newMatch->record_type = type;
    newMatch->record_ptr = rec_ptr;

    if(!(*map_list) || (compare_map_rec(newMatch,*map_list,filetype) < 0))
      {  /* no list or insert at beginning */
      newMatch->next = *map_list;
      *map_list = newMatch;
      }
    else
      {  /* insert elsewhere */
      curr = *map_list;
      /* find spot for node */
        while(curr->next && (compare_map_rec(newMatch,curr->next,filetype) > 0))
        curr = curr->next;
      /* insert */
      newMatch->next = curr->next;
      curr->next = newMatch;
      }
      
  }


int compare_map_rec(struct map_rec *rec1, struct map_rec *rec2, int filetype)
  {
    struct single_boundary boundary1, boundary2;

    get_begin_boundary(&boundary1,rec1,filetype);
    get_begin_boundary(&boundary2,rec2,filetype);
    
    return(compare_boundaries(boundary1,boundary2));
  }

