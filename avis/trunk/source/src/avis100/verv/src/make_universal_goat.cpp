#include <stdio.h>
#include <stdlib.h>
/*#include <values.h>*/
#include "autoverv.h"

void add_universal_goat_match(struct universal_goat_match **uni_match, int filetype,
                        struct boundary_location *g_begin, struct boundary_location *g_end,
                        struct boundary_location *ig_begin, struct boundary_location *ig_end);


struct universal_goat_match *make_universal_goat(int filetype, struct sample_data **samples,
                                     struct sample_data **goat_samples, int num_samples,
                                     struct posrec **goat_match)

  {
    int i, more_regions, any_regions,j,
        *gb_boundary=NULL,
        *ge_boundary=NULL,
        *igb_boundary=NULL,
        *ige_boundary=NULL;
    struct posrec **curr_region=NULL;
    struct universal_goat_match *uni_match = NULL;
    struct boundary_location *universal_gb_boundary=NULL, 
                             *universal_ge_boundary=NULL,
                             *universal_igb_boundary=NULL,
                             *universal_ige_boundary=NULL;
    struct sample_data **samples_with_goats=NULL, **goats_filetype=NULL;
    int num_samples_with_goats=0;


    /* allocate stuff */
    curr_region = (struct posrec **)calloc(num_samples,sizeof(struct posrec *));
    samples_with_goats = (struct sample_data **)calloc(num_samples,sizeof(struct sample_data *));
    goats_filetype = (struct sample_data **)calloc(num_samples,sizeof(struct sample_data *));

    for(i=0;i<num_samples;i++)
      {
        if(goat_samples[i] && (get_file_type(samples[i]) == filetype))
          {
          if(!goat_match[i])  /* if any sample had no goat sections */
            return(NULL);  /* can't do anything universally */
          else
            {
            curr_region[num_samples_with_goats] = goat_match[i];

            /* make arrays with only the samples were considering */
            samples_with_goats[num_samples_with_goats] = samples[i];
            goats_filetype[num_samples_with_goats] = goat_samples[i];
            num_samples_with_goats++;
            }
          }
      }

    gb_boundary = (int *) malloc(sizeof(int) * num_samples);
    ge_boundary = (int *) malloc(sizeof(int) * num_samples);
    igb_boundary = (int *) malloc(sizeof(int) * num_samples);
    ige_boundary = (int *) malloc(sizeof(int) * num_samples);

    more_regions = 1;
    do
      {
      any_regions = 0;
      j=0;
        for(i=0;(i<num_samples_with_goats) && more_regions;i++)
        if(goats_filetype[i] && (get_file_type(samples_with_goats[i]) == filetype))
          {
            if(curr_region[i])
            {
              gb_boundary[i] = curr_region[i]->goatpos;
              ge_boundary[i] = curr_region[i]->goatpos + curr_region[i]->length-1;
              igb_boundary[j] = curr_region[i]->infgoatpos;
              ige_boundary[j++] = curr_region[i]->infgoatpos + curr_region[i]->length-1;
              curr_region[i] = curr_region[i]->next;
              any_regions = 1;
            }
            else
            more_regions = 0;
          }

        if(any_regions && more_regions)
          {
            universal_gb_boundary = find_best_anchor(gb_boundary, num_samples_with_goats, goats_filetype);
            universal_ge_boundary = find_best_anchor(ge_boundary, num_samples_with_goats, goats_filetype);
            universal_igb_boundary = find_best_anchor(igb_boundary, num_samples_with_goats, samples_with_goats);
            universal_ige_boundary = find_best_anchor(ige_boundary, num_samples_with_goats, samples_with_goats);
            /* add to universal goat match list */
            add_universal_goat_match(&uni_match,filetype,
                             universal_gb_boundary,universal_ge_boundary,
                                     universal_igb_boundary,universal_ige_boundary);
          }
      } while (more_regions && any_regions);

    /* clean up */
	// 6/26/97 Fred: Added the =NULL to test the effect on BoundsChecker
    free(gb_boundary);gb_boundary=NULL;
    free(ge_boundary);ge_boundary=NULL;
    free(igb_boundary);igb_boundary=NULL;
    free(ige_boundary);ige_boundary=NULL;
	// 6/26/97 Fred: Added the 4 following free() (mem leak reported by BoundsChecker)
    free(universal_gb_boundary);universal_gb_boundary=NULL;
    free(universal_ge_boundary);universal_ge_boundary=NULL;
    free(universal_igb_boundary);universal_igb_boundary=NULL;
    free(universal_ige_boundary);universal_ige_boundary=NULL;
    free(samples_with_goats);samples_with_goats=NULL;
    free(curr_region);curr_region=NULL;
	// 6/25/97 Fred: Added the following free (BoundsChecker complained about a memory leak)
	free(goats_filetype);goats_filetype=NULL;

    return(uni_match);
  }


void add_universal_goat_match(struct universal_goat_match **uni_match, int filetype,
                        struct boundary_location *g_begin, struct boundary_location *g_end,
                        struct boundary_location *ig_begin, struct boundary_location *ig_end)
     
  {
    struct universal_goat_match *curr, *newMatch;
    
    /* if the range for this section isn't too great */
    if((g_begin->offset_range[ALL] <= max_goat_range) &&
       (g_end->offset_range[ALL] <= max_goat_range) &&
       (ig_begin->offset_range[filetype] <= max_goat_range) &&
       (ig_end->offset_range[filetype] <= max_goat_range))
      {  /* add it */
      newMatch = (struct universal_goat_match *)malloc(sizeof(struct universal_goat_match));
      
      
      assign_boundary(&(newMatch->g_begin),g_begin,ALL);
      assign_boundary(&(newMatch->g_end),g_end,ALL);
      assign_boundary(&(newMatch->ig_begin),ig_begin,filetype);
      assign_boundary(&(newMatch->ig_end),ig_end,filetype);
      
      if(!(*uni_match) || (compare_boundaries(newMatch->g_begin,(*uni_match)->g_begin) < 0))
        {  /* no list or insert at beginning */
          newMatch->next = *uni_match;
          *uni_match = newMatch;
        }
      else
        {  /* insert elsewhere */
          curr = *uni_match;
          /* find spot for node */
          while(curr->next && (compare_boundaries(newMatch->g_begin,curr->next->g_begin) > 0))
            curr=curr->next;
          newMatch->next = curr->next;
          curr->next = newMatch;
        }
      }
  }
