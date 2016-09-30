#include <stdlib.h>
#include <stdio.h>
#include "autoverv.h"


void assign_boundary(struct single_boundary *single, struct boundary_location *boundary,
                 int filetype)

{
  single->bestanchor = boundary->bestanchor[filetype];
  single->offset_min = boundary->offset_min[filetype];
  single->offset_range = boundary->offset_range[filetype];
}


int compare_boundaries(struct single_boundary boundary1, struct single_boundary boundary2)
  {
    if(boundary1.bestanchor != boundary2.bestanchor)
      return(boundary1.bestanchor - boundary2.bestanchor);
    else
      return(boundary1.offset_min - boundary2.offset_min);
  }



void get_begin_boundary(struct single_boundary *single, struct map_rec *rec, int filetype)
  {
    struct single_boundary *temp;
    switch(rec->record_type) {
      case 'V':
        assign_boundary(single,&(((struct match_list *)(rec->record_ptr))->begin),filetype);
      break;
      case 'G':
      temp = &(((struct universal_goat_match *)(rec->record_ptr))->ig_begin);
      single->bestanchor = temp->bestanchor;
      single->offset_min = temp->offset_min;
      single->offset_range = temp->offset_range;
      break;
      }
  }


void get_end_boundary(struct single_boundary *single, struct map_rec *rec, int filetype)
  {
    struct single_boundary *temp;
    switch(rec->record_type) {
      case 'V':
        assign_boundary(single,&(((struct match_list *)(rec->record_ptr))->begin),filetype);
      single->offset_min += ((struct match_list *)(rec->record_ptr))->match->length-1;
      break;
      case 'G':
      temp = &(((struct universal_goat_match *)(rec->record_ptr))->ig_end);
      single->bestanchor = temp->bestanchor;
      single->offset_min = temp->offset_min;
      single->offset_range = temp->offset_range;
      break;
      }
  }


int compare_locations(struct location location1, struct location location2)
  {
    if(location1.anchor_type != location2.anchor_type)
      return(location1.anchor_type - location2.anchor_type);
    else
      return(location1.offset - location2.offset);
  }
