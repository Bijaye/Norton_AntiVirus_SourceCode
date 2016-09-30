#ifndef AUTOVERV_H
#define AUTOVERV_H

#if defined(_cplusplus) || defined(__cplusplus)
   extern "C" {
#endif

#if defined(_WIN32)
#   pragma warning(4:4786)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
   #include <values.h>
   #include <dirent.h>
#else
   #include "avfileio.h"
   #define  MAXINT (int)(0x7FFFFFFFL)
   #define  strcasecmp stricmp
#endif
typedef DIR * DIREC;

#include "allocsafe.h"

#include "common.h"
#include "polymath.h"

// AJR
#define max(a, b)  (((a) > (b)) ? (a) : (b))
#define min(a, b)  (((a) < (b)) ? (a) : (b))

/* riad */
#define ALLOC(type, size)       calloc((size), sizeof(type))

//Fred
// WARNING First type of bigram is little endian on Intel machines, while second type is always big endian
#ifdef CASTSHORTBIGRAM
#define BIGRAM(array,i) ((int) *((unsigned short *) &array[i]))
#else
#define BIGRAM(array,i) ((int) (array[i]*256+array[i+1]))
// The following does not work! I don't know why...
//#define BIGRAM(array,i) ((int) (((int)array[i])<<8+((int)array[i+1])))
#endif
/* defines */

#define max_bigram_counts 25      /* Heuristic parameter used to decide when
                                     enough bigrams are enough */

#define min_skip_length 1000      /* sets minimum match size for which we think we won't find a */
                                  /* larger region containing a subset of the given region. */
                                  /* This is used to speed the matching process by skipping match */
                                  /* attempts from all the offsets in regions skipped */

#define min_goat_match  25 //!!!!!!        /* set minimum length of goat region to match (don't match stuff in header until later in reconstruction step */

#define min_match_length 4        /* sets the length of the smallest viral match to report */

#define encryption_types 4        /* sets the number of available encryption types */

#define verv_buffer_max 9000      /* defines the maximum buffer size allowed by VERV */

#define time_limit /*600*/0            /* maximum number of seconds to allow the program to run */
                                  /* 0 means no time limit.  The program only checks the time */
                                  /* after each match cycle, so overrun will occur */

#define max_extra_vv_bytes 200    /* sets the maximum number of extra variable virus bytes allowed */
                                  /* when filling from the last section out to end of file */
                                  /* 0 means no limit */

#define count_limit 25            /* sets max bigram count limit for dot product computation */

#define file_types 2              /* number of file types to really consider (i.e. com/exe) */

#define max_goat_range 32         /* maximum range to allow on a goat section */

#define COM 0                     /* type for com */
#define EXE 1                     /* type for exe */
#define ALL 2                     /* "type" for all files collectively */

// AJR: log of files excluded by dot-product calculation
#define EXCLUDE_LOG "autoverv.xcl"

/*#ifdef aix*/
#define pathchar '/'
/*#else
#define pathchar '\\'
#endif*/


/* globals */

/* establish key lengths for each encryption type */
static const int key_length[encryption_types]={0,1,1,2};

/* establish encryption names */
static const char *encrypt_names[encryption_types]={"PLAIN","XOR1","ADD1","XOR2"};

/* establish anchor type chars */
static const char anchorchar[3] = {'B','T','E'};




/* data types */

struct sample_data
{
  char filename[256];           /* filename of sample on disk */
  int num_bytes;                /* Length of sample in bytes */
  unsigned char *bytes;         /* sample bytes */
  unsigned char *map_bytes;     /* maps the validity/match status of bytes */
  char file_type[7];            /* string holding the VERV style file-type */
                                /* E8-COM, E9-COM, P-COM, S-EXE -- and our added DEV */
  unsigned int entry_pt;        /* File entry point */

  /* internal use fields */
  int bigram_counts[65536];     /* counts for each bigram */
  int *position[65536];         /* array of positions for each bigram */
  int max_match_len;            /* maximum match length for the current bigram */
  int max_match_pos;            /* position of that match */
};

struct boundary_location
{
  int bestanchor[file_types+1];
  int offset_min[file_types+1];
  int offset_range[file_types+1];
};

struct single_boundary
{
  int bestanchor;
  int offset_min;
  int offset_range;
};


struct posrec
{
  int goatpos;
  int infgoatpos;
  int length;
  struct posrec *next;
};


struct match_data
{
  int length;
  int type;
  int *pos_in_sample;
};

#ifdef VERV_CODO

#define NOT_ENOUGMEM 12
#define SUCCESS      0   

struct temp_boundaries
{
  int offset;
  int length;
  int present;
  struct temp_boundaries *next;
};

#endif

struct match_list
{
#ifdef VERV_CODO

  #define VERV_CODE 1 /* not sure 100% */
  #define VERV_DATA 0 /* may be */
  int code;           /* flag indicating that this section is code or no */
  /* this is a temporary struct used for temporary storing of boundaries
   * when codedata segregation has to be done, normally it's NULL
   */
  struct temp_boundaries *tempboundaries;

#endif

  /* normal members */
  struct match_data *match;
  struct boundary_location begin;
  polyn  crc;
  struct match_list *next;
};


struct sample_list
{
  int sample_num;
  struct sample_list *next;
};


struct universal_goat_match
{
  struct single_boundary g_begin, g_end, ig_begin, ig_end;
  struct universal_goat_match *next;
};

struct map_rec
{
  char record_type;
#ifdef VERV_CODO
  union
  {
     struct match_list *matchlist;
     void   *record_ptr;
  };
#else
  void *record_ptr;
#endif
  struct map_rec *next;
};

/* structures for interface with Jeff's reconstruction stuff */

struct location
{
  char anchor_type;
  int offset;
};

struct key
{
  int crypt_type;
  struct location key_location;
  unsigned char *key_correction;
};

struct key_link
{
  struct key encryption_info;
  unsigned char *keys_in_each_sample;
  struct key_link *next;
};

struct phase1_match
{
  struct location loc;
  struct key encryption_info;
  struct phase1_match *next;
};

struct phase2_match
{
  char g_anchor_type;
  char vv_anchor_type;
  int relative_offset;
  struct key encryption_info;
  int coverage;
  int *covered_goat_bytes;
  struct phase2_match *next;
};  

struct reconstruction_vector
{
  struct location loc;
  unsigned char *sample;
};

struct reconstruction_list {
  struct key encryption_info;
  struct location g_begin;
  struct location ig_begin;
  int length;
  struct reconstruction_list *next;
};

struct vds_region {
  int filetype;
  struct location begin;
  int length;
  int buffer_offset;
  int useless_bytes;
  struct vds_region *next;
};

  

/* function prototypes */

/* creates an index of bigrams in sample -- used for matching */
void index_bigrams(struct sample_data *sample);

/* called to give error and abort program in case of memory error */
void mem_err(void);

/* finds longest common substring of valid bytes between given samples */
struct match_data *lcs(struct sample_data *samples[],
                       int num_samples, unsigned suggested_start[], int lowerbound, char * approxworked);

/* computes the length of a match at a given pt in each of the samples */
/*  ... used by lcs */
int find_match_len(struct sample_data *master, int master_pos,
                   struct sample_data *slave,  int slave_pos,
		   int current_best_match_length);

/* computes the size of a file */
long int filesize(FILE *stream);

/* frees a match_data structure including the contained array */
void free_match(struct match_data *match);

/* creates "invariant bytes" using the given encryption type */
void create_invariant(struct sample_data *sample, int encrypt_type,
                      unsigned char **inv_bytes, unsigned char **inv_map);

/* determines the file type and entry point of the given sample */
void entry(struct sample_data *sample);

/* frees a linked list of posrecs pointed to by head*/
void free_pos(struct posrec *head);

/* writes .vds format file for VERV */
void write_vds(FILE *vds_out, char *infected_dir, struct sample_data **samples, int num_samples,
	       struct map_rec *map_list[file_types], struct universal_goat_match *uni_match[file_types],
	       int num[file_types], int num_with_goats[file_types],
	       struct reconstruction_list *recon_list[file_types],
	       int num_missing[file_types], int calc_mz[file_types]);

/* write .exg format file for VP */
void write_graphic(char *infected_dir, struct map_rec *map_list[file_types],struct reconstruction_list *recon_list[file_types]);

/* sets up a CRC table with the hardwired password */
polyn *crc_setup(void);

/* creates a temporary buffer of the match text decrypted appropriately */
/* with the first byte(s) of the region */
unsigned char *decrypt(struct match_data *match, unsigned char *bytes);

/* writes the .bytes file for the signature extractor's use */
void write_bytes(char *infected_dir, struct sample_data *sample,
		 struct match_list *virus_match);

void write_bytes2(char *infected_dir, struct sample_data *sample,
		 struct map_rec *map_list[file_types], int useCode_DataSegregation);

/* creates the crypt_key_info list needed for reconstruction */
struct key_link *make_key_list(struct match_list *virus_match,
			       struct sample_data **samples,
			       struct sample_data **goat_samples,
			       int num_samples, int filetype);

/* gets the integer file type for the given sample */
int get_file_type(struct sample_data *sample);

/* makes a list of univeral goat match information for the given filetype */
struct universal_goat_match *make_universal_goat(int filetype, struct sample_data **samples,
						 struct sample_data **goat_samples, int num_samples,
						 struct posrec **goat_match);

/* decides on the best anchor to use for given boundary */
struct boundary_location *find_best_anchor(int *offsets,
                                           int num_samples,
                                           struct sample_data **samples);

/* assigns to a single_boundary structure the corresponding fields of the given boundary */
/* structure for the given filetype */
void assign_boundary(struct single_boundary *single, struct boundary_location *boundary,
		     int filetype);

/* return is < 0 if boundary1 comes before boundary2 */
/*           = 0 if boundaries are the same          */
/*           > 0 if boundary1 comes after boundary2  */
int compare_boundaries(struct single_boundary boundary1, struct single_boundary boundary2);

/* makes a combined linked list of goat and viral sections for the given filetype */
struct map_rec *make_map_list(struct match_list *virus_match, 
			      struct universal_goat_match *uni_goat_match,
			      int filetype);

/* creates the arrays needed for Jeff's reconstruction stuff */
void make_recon_arrays(struct sample_data **samples, struct sample_data **goat_samples,
		       int num_samples, struct universal_goat_match *uni_goat_match,
		       struct map_rec *map_list, int filetype,
		       struct reconstruction_vector **goat_bytes, 
		       struct reconstruction_vector **variable_virus_bytes,
		       int num_files, int *num_goat_bytes, int *num_vv_bytes);


/* computes absolute offset into sample (i.e. from beginning) given an anchor and offset */
int compute_offset(int anchor, int offset, struct sample_data *sample);

// compute the minimum absolute offset of a boundary location 
int compute_boundary_minoffset (const struct single_boundary* pBoundary,
                                struct sample_data *pSample);

/* gets the beginning boundary of a region in the infected goat from a map_rec (i.e 
   could be a goat or virus section) */
void get_begin_boundary(struct single_boundary *single, struct map_rec *rec, int filetype);

/* gets the ending boundary of a region in the infected goat from a map_rec (i.e 
   could be a goat or virus section) */
void get_end_boundary(struct single_boundary *single, struct map_rec *rec, int filetype);

/* return is < 0 if location1 comes before location2 */
/*           = 0 if locations are the same          */
/*           > 0 if location1 comes after location2  */
int compare_locations(struct location location1, struct location location2);


/* Functions needed for reconstruction. Get Jeff to add comments describing these. */

struct phase1_match **reconstruct(int,
				  int,
				  int,
				  struct reconstruction_vector *,
				  struct reconstruction_vector *,
				  struct key_link *);

void eliminate_duplicate_keys(int,
			      struct key_link *);

int equivalent_keys(const key_link* key1, const key_link* key2, int num_samples);

int derive_correction(int,
		      unsigned char *,
		      unsigned char *,
		      struct key_link *);

/* takes the result of greedy_cover and converts into a list of runs */
struct reconstruction_list *make_reconstruction_list(struct phase2_match *recon_summary,
						     struct reconstruction_vector *goat_bytes,
						     int num_goat_bytes);
  

/* returns in num_missing the number of important/semi-important bytes not reconstructed */
/* returns in calc_mz a 1 if CALC_MZ_LENGTH is required to complete the reconstruction */
void reconstruct_status(struct phase2_match *recon_summary,
			struct reconstruction_vector *goat_bytes,
			int num_goat_bytes, int filetype,
			int *num_missing, int *calc_mz);

#ifdef VERV_CODO
/* patches the map_list strucutres so it will include the code and data regions */
int patch_map_list (struct map_rec *map_list[file_types], 
                    struct sample_data **samples, 
                    int    num_samples, 
                    char   *infected_dir,
                    polyn  *crc_table);
#endif


//AJR
typedef struct EXErelocationTableBoundaries {
   int start;
   int end;
} EXErelocationTableBoundaries;

int next_ProgramSection_Beginning (const int offset, 
                                   const int fileType, 
                                   const EXErelocationTableBoundaries RTB);
int prior_ProgramSection_End (const int offset, 
                              const int fileType, 
                              const EXErelocationTableBoundaries RTB);

//extended def.
struct phase2_match *greedy_cover(int num_goat_bytes,
                          struct reconstruction_vector *goat_bytes,
                          struct phase1_match **possible_matches,
                          const int fileType,
                          EXErelocationTableBoundaries UniGoatRTB);



#if defined(_cplusplus) || defined(__cplusplus)
   }   // extern "C"
#endif

#endif   // AUTOVERV_H
