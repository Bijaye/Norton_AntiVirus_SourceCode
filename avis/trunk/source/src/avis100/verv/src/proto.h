#include "polymath.h"  /* the crc function prototypes, etc. */

poly *crc_setup(void);  /* sets up crc table with password */

struct column_match_info
*add_column_match(struct column_match_info **,
                  struct column_match_info *,
                  int,
                  int,
                  int,
                  int,
                  int,
                  int,
                  unsigned char);
void
calculate_class_ranges(struct class_info *);

struct infection_pattern
*calculate_infection_pattern(struct file_data *,
                             struct file_data *,
			     FILE *);

int
calculate_marker_offset(struct file_data *,
                        int);

struct section_markers
*calculate_marker_offsets(struct file_data *,
                         struct file_data *,
                         struct diff_region *,
                         char);

void
calculate_possible_matches(struct class_info *,
                           struct file_data *[],
                           struct file_data *[],
                           struct link *[],
                           struct crypt_key_position_info *[],
                           struct column_match_info ***);

int
check_range(int,
            int,
            int,
            int);

void
classify(struct infection_pattern *[],
         int,
         struct class_info *[],
         int *);

int
compare_class(struct class_info *,
              struct infection_pattern *);

void
count_bigrams(struct file_data *,
              struct diff_data *,
              int[]);

unsigned char
crypt_op(unsigned char,
         unsigned char,
         int);

int
crypt_key_match(struct class_info *,
                struct file_data *[],
                struct crypt_key_position_info *,
                int,
                int,
                int);

void
entry(struct file_data *);

void
free_column_match_info(struct column_match_info *);

void
free_link(struct link *);

void greedy_cover(struct class_info *,
                  struct column_match_info ***,
                  struct cover_tally_info **);

void identify_unmatched_regions(struct file_data *,
                                struct file_data *,
                                struct diff_data *,
                                int);

void
index_bigrams(struct file_data *,
              struct link *[],
              int []);

struct class_info
*initialize_class(int,
                  struct infection_pattern *);

struct diff_data
*diff(struct file_data *,
      struct file_data *);

int
match_length(struct file_data *,
             int,
             struct file_data *,
             int);

void
print_class_pattern(FILE *,
                    struct class_info *,
                    struct file_data *[]);

void
print_entry_info(FILE *,
                 struct file_data *,
                 struct file_data *);

void print_possible_matches(FILE *,
                            struct class_info *,
                            struct column_match_info ***);

void
print_range(FILE *,
            struct class_range *,
            char);

int
q_valid_region1(int,
                struct diff_data *);

int
q_valid_region2(int,
                struct diff_data *);

struct file_data
*read_file (char *,int);

void
update_class(int,
             struct class_info *,
             struct infection_pattern *);

void
crypt_match(int,
            unsigned char *[],
            int,
            int *,
            int,
            int);

void
crypt_match0(int,
             unsigned char *[],
             int,
             int *);

void
crypt_match1(int,
             unsigned char *[],
             int,
             int *,
             int);

void
crypt_match2(int,
             unsigned char *[],
             int,
             int *);

void
crypt_match3(int,
             unsigned char *[],
             int,
             int *);

void
print_vds_info(FILE *,
                struct class_info *[],
                int,
                struct file_data *[],
                struct cover_tally_info **[],
                struct crypt_match_info *,
                int);

void
print_const_info(FILE *,
                 struct class_info *,
                 struct file_data *,
                 struct cover_tally_info **);

struct link
*virus_match(struct class_info *,
             struct file_data *[],
             int,
             struct crypt_key_position_info *[]);



int
which_indicator(struct span *);

struct crypt_key_info
*create_cover_category_key_info(int,
                                int,
                                unsigned char,
                                int);

struct cover_tally_info
*create_cover_category(int,
                       int,
                       int,
                       int,
                       int,
                       int,
                       int,
                       unsigned char,
                       int);


unsigned char* dexor1(unsigned char *bytes,int numbytes,unsigned char key);

unsigned char* dexor2(unsigned char *bytes,int numbytes,unsigned char key_high,
                      unsigned char key_low);


unsigned char* deadd1(unsigned char *bytes,int numbytes,unsigned char key);

void correct_key (struct class_info *class[],int num_classes,
                  struct file_data *infected_goat[],
                  struct cover_tally_info **reconstructed_bytes_list[]);


void adjust_sections(struct class_info *class,
                     struct cover_tally_info ***reconstructed_bytes_list);

void print_raw_info(FILE *output, struct class_info *class[],
                    int num_classes,
                    struct file_data *infected_goat[],
                    struct link **variable_column_list[],
                    struct crypt_match_info *matched_sections,
                    int match_class);

unsigned char *get_plain_buffer(struct file_data *infected_goat,
                    struct class_info *class,
                    struct crypt_match_info *crypt_match_section);

struct crypt_match_info *align_sections(struct class_info *class[],int num_classes,
          struct file_data *infected_goat[],struct cover_tally_info **reconstructed_bytes_list[],
          int *match_class);

void print_virus_bytes(FILE *,
		       struct infection_pattern *,
		       struct file_data *);
