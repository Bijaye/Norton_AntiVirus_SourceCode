/*---------------------------------------------------------------------------------------------------------------*/
// AJR:
// 1/8/98:   Replaced windows directory-walking code with POSIX-compliant code
//           using the AVFILEIO library - the windows code incorrectly used the
//           fileinfo struct.

// 1/14/98:  Used ALLOCSAFE for dynamic memory allocation - halts on failure.

// 2/26/98:  Added code to handle multi-byte structures in (EXE) files: no byte in a 
//           multi-byte field matches unless they all do (define USE_STRUCTURE to 
//           build this feature).

// 3/09/98:  Turn off LCS approx. method unless LCS_APPROX defined.

// 5/6/98:   Log samples eliminated by the dot-product test so that the 
//           immune system verification can ignore them.

//           Don't eliminate sample.*

// 8/9/98:   -h option allows input program files to be in the form of hex dumps.

/*---------------------------------------------------------------------------------------------------------------*/
#define _COM ".com"
#define _EXE ".exe"
/*---------------------------------------------------------------------------------------------------------------*/
#include "autoverv.h"
#include "fileIterator.hpp"

#include <time.h>
#include <math.h>

#include <signal.h>

#ifdef aix
#include <sys/io.h>
#endif

/*----------------------------------------------------------------------------------------------------------------*/
extern int ALL_CODE = 0;   //AJR temp??


/* gives syntax & exits */
void  endprg(void);

/* adds a backslash to end of path if needed */
void  fixpath(char *pathstr);

/* determines if we have a goat corresponding to the given infected file */
int   havegoat(const char *filename, DIREC g_dir, char *goatname); 

/* translates nulls into ?s for printout of region types */
char  region_char(char c);

/* compute dot product of sorts between */ /* bigram counts of samples */
double      dot_product(int counts1[65536L], int counts2[65536L], double result11, double result22);

/* add goat match to the linked list */
void  add_goat_match(struct posrec **goat_match, struct match_data *match);

/* add a virus match to the linked list */
struct      match_list *add_virus_match(struct match_list **list, struct match_data *match, struct boundary_location *best_match_location);

/* close files, etc. & exit with rc */
void  doneprg(int rc, FILE *viral_out, FILE *map_out, FILE *vds_out);

/* converts string to lower case */
/* riad: i just removed #ifdef aix... */
char  *strlwr(char *s);

// AJR: hexdump option
static size_t fileRead (unsigned char* buffer, size_t size, size_t count, FILE *stream, bool isHex);


#ifdef VERV_CODO
/* patches the map_list strucutres so it will include the code and data regions */
int patch_map_list (struct map_rec *map_list[file_types], 
                    struct sample_data **samples, 
                    int    num_samples, 
                    char   *infected_dir,
                    polyn  *crc_table);
#endif

//AJR
typedef unsigned short WORD;
static void adjust_match_boundary_to_program_structure (
               struct match_data* pMatch,
               struct sample_data* pGoatSample);
static EXErelocationTableBoundaries getRelocationTableBoundaries (struct sample_data* pSample);
static EXErelocationTableBoundaries getUniGoatRelocationTableBoundaries (
                                       struct sample_data* goat_samples[], 
                                       int num_samples);



void  set_signal_int(int sig)
{
  if(sig == SIGSEGV) 
    printf("\n\n\t\t[Program got a Segmentation Fault -> EXITING]\n\n");
  else if(sig == SIGINT)
    printf("\n\n\t\t[Program got an Interrupt Signal -> EXITING]\n\n");
  else
    printf("\n\n\t\t[Program got a Some Signals -> EXITING]\n\n");
  
  /*signal(SIGINT, set_signal_int);*/
  /*signal(SIGSEGV, set_signal_int);*/
  exit(-1);
}

/*------------------------------------------------------------------------------------------------------------*/

int main(int argc, char *argv[], char *envp[])
{
  struct        sample_data **samples, **goat_samples;
  int           i,j,startpos,num_samples=0,curr_sample,type,best_len,found_goat=0,len;
  unsigned int  l;
  struct        match_data *match=NULL, *best_match=NULL;
  char          goat_dir[256] = "",infected_dir[256] = "";
  char          scratch[256], ch;
  char          virusname[256];
  FILE          *curr_file, *temp_goat;
  FILE          *viral_out,*map_out, *vds_out;

  FileIterator  fileIterator;
  bool          useListFiles = false;
  unsigned char ***invariant_bytes, ***invariant_map_bytes, *tempbuffer;
  time_t        start_time, end_time;
  int           bigram, orig_num_samples;
  double        *dp, *dp_self, dp_cum, dotprod;
  struct        posrec **goat_match, *curr;
  struct        match_list *virus_match=NULL, *vmatch;
  int           b,t,e,/*bestanchor[file_types+1],range[file_types+1],*/filetype; 
  int           num[file_types];
  polyn         *crc_table;
  int           last_best[encryption_types];
  struct        key_link *key_list[file_types];
  struct        boundary_location *best_match_location;
  struct        universal_goat_match *uni_goat_match[file_types], *ug_match;
  struct        map_rec *map_list[file_types], *m_list;
  struct        reconstruction_vector *goat_bytes[file_types], *variable_virus_bytes[file_types];
  int           num_with_goats[file_types], num_goat_bytes[file_types], num_vv_bytes[file_types];
  int           num_missing[file_types], calc_mz[file_types];
  struct        single_boundary tmp_begin, tmp_end;
  
  int           timings=0, chatty=0, dir=0;
  struct        key crypt_info;
  struct        phase1_match /* *pm, */ **possible_matches;
  struct        phase2_match *r_summary, *reconstruction_summary[file_types];
  struct        reconstruction_list *recon_list[file_types], *rlist;
  
  const char   *currfile;  /* riad */
  int           patchos = 0;
  int           numbytread = 0; //Number of bytes read by fread (to check we load the samples correctly)
  int           speedup = 0; // This is used to tell autoverv not to compute the dot products (-s option) // Fred
  bool          bHexInput = false;
//Fred: those variables are only used with the quick version of lcs
#if 1
  unsigned    * suggested_start = NULL;
  int         othertypei=0;
  struct sample_data * sampleCOM;
  struct sample_data * sampleEXE;
  int         fromCOM;
  int         toCOM;
  int         fromEXE;
  int         toEXE;
  char        COMEPfound=0;
  int         ofsCOMtoEXE=0;
  int         newsuggstart;
  int         * notgoatnumbytes;
  int         minnotgoatnumbytes=MAXINT;
  int         fixedbytesfound=0;
#endif
  int         foundwithnewlcs=0;
  char        approxworked=0;
  char        newlcsmatch=0;
  int         lowerbound=0;
  //
  //signal(SIGINT, set_signal_int);
  //signal(SIGSEGV, set_signal_int);

  //AJR
  EXErelocationTableBoundaries UniGoatRTB;
  FILE* fExcludeLog;   // list of files excluded by the dot-product test

  /* initialize everything!!!!! */
  for(i=0;i<file_types;i++) {
    recon_list[i] = 0; 
    key_list[i] = 0; 
    uni_goat_match[i] = 0;
    map_list[i] = 0;
    goat_bytes[i] = 0;
    variable_virus_bytes[i] = 0;
    num_with_goats[i] = 0;
    num_goat_bytes[i] = 0;
    num_vv_bytes[i] = 0;
    num_missing[i] = 0;
    calc_mz[i] = 0;
    reconstruction_summary[i] = 0;
    recon_list[i] = 0;
  }
  for(i=0;i<encryption_types;i++) last_best[i] = 0;
  
  time(&start_time);
  printf("\nAutoVerv 2.1 (compiled %s %s)\n", __DATE__, __TIME__);
  
  *virusname = 0;
  
  for(i=1;i<argc;i++)
  {
    if(argv[i][0] == '-')  /* if an option */
    {
      /* strlwr(argv[i]); */
      for(l=1;l<strlen(argv[i]);l++) /* go through each char */
        if     (argv[i][l] == 't')  
          timings=1;
        else if(argv[i][l] == 'c')
          chatty=1;
        else if(argv[i][l] == 'p')
          patchos=1;
        else if(argv[i][l] == '!'){
          patchos=1;
          ALL_CODE = 1;}
        else if(argv[i][l] == 's')
          speedup=1;
        else if(argv[i][l] == 'l')
          useListFiles = true;
        else if(argv[i][l] == 'h')   // AJR hex dump input
          bHexInput = true;
        //else if(argv[i][l] == 'n')
        //   { if(++i < argv) strcpy(virusname, argv[i]); }
        else
          endprg();
    }
    else if(dir == 0)
    {
       strcpy(infected_dir, argv[i]);
      dir++;
    }
    else if(dir == 1)
    {
       strcpy(goat_dir, argv[i]);
      dir++;
    }
    else
    {
      strcpy(virusname, argv[i]);
      dir++;
    }
  }
  if(*virusname == 0) if(*infected_dir) strcpy(virusname, infected_dir);
  
  if(dir < 2)  /* if not enough directories were specified */
    endprg();
  
  if (!(useListFiles ? fileIterator.initFromList(goat_dir, infected_dir)
                     : fileIterator.initFromDir (goat_dir, infected_dir))) {
     printf("\nunable to load file names!   Aborting...\n\n");
     exit(1);
  }
  num_samples = fileIterator.count();

  viral_out=fopen("autoverv.viral.out","a");
  map_out=fopen("autoverv.map.out","a");
  vds_out=fopen("autoverv.vds.out","a");
  
  if((!viral_out) || (!map_out) || (!vds_out))
  {
    printf("\nCould not open one or more output files!  Aborting...\n");
    fclose(viral_out);
    fclose(map_out);
    fclose(vds_out);
    exit(1);
  }
  // AJR
  fExcludeLog = fopen (EXCLUDE_LOG, "w");
  if (!fExcludeLog) {
    perror ("exclude log open");
    fclose(viral_out);
    fclose(map_out);
    fclose(vds_out);
    exit(1);
  }

  printf("\nProcessing (%s)...", virusname);  /* claim that we're doing something */

#ifdef DEBUG
  printf("Samples: %d\n",num_samples);
#endif
  
  if(num_samples)
  {
    /* allocate enough space in samples array for number of samples (+1 extra) */
    samples=(struct sample_data **)calloc((num_samples+1),sizeof(struct sample_data *));
    //memset (samples, 0, (num_samples+1)*sizeof(struct sample_data *));
    goat_samples=(struct sample_data **)calloc((num_samples),sizeof(struct sample_data *));
    //memset (goat_samples, 0, num_samples*sizeof(struct sample_data *));
    /* allocate enough space in goat_match array for number of samples */
    goat_match=(struct posrec **)calloc(num_samples,sizeof(struct posrec *));
    //memset (goat_match, 0, num_samples*sizeof(struct posrec *));
    
    if(!samples || !goat_match) mem_err();
    
    
#ifdef DEBUG
    printf("Allocated samples (%p)...",samples);
#endif
    curr_sample=0;
  }
  for (size_t n = 0; n < fileIterator.count(); n++) {
    strcpy(scratch,fileIterator.getInfectedPath(n));
    currfile = fileIterator.getInfectedName(n);
    if((curr_file = fopen(scratch,"rb")) != NULL)
    {
      /*printf("file %s opened\n",scratch);*/
      /*printf("[V: %s ", currfile);*/
      printf("[%s: ", currfile);
      /* allocate a sample record */
      samples[curr_sample]=(struct sample_data *)ALLOC(struct sample_data, 1);
      if(!samples[curr_sample]) mem_err();
      
#ifdef DEBUG
      printf("Allocated samples[%d] (%p)...",curr_sample,&samples[curr_sample]);
#endif
      
      strcpy(samples[curr_sample]->filename,scratch);
      // Fred: need to deal with the case of empty samples
      if ((samples[curr_sample]->num_bytes=filesize(curr_file)/(bHexInput ? 2 : 1)) == 0) { // AJR
        // Sample is empty, skip it
        printf ("No (Empty)]  ");
        free (samples[curr_sample]);
        num_samples--;
        continue;
      }
      samples[curr_sample]->bytes=(unsigned char *)calloc(samples[curr_sample]->num_bytes,
        sizeof(unsigned char));
      samples[curr_sample]->map_bytes=(unsigned char *)calloc(samples[curr_sample]->num_bytes,
        sizeof(unsigned char));
      rewind(curr_file);
      
      /* read in the file */
      if ((numbytread =
        fileRead(samples[curr_sample]->bytes, 1, samples[curr_sample]->num_bytes, curr_file, bHexInput)
        ) < samples[curr_sample]->num_bytes) printf ("-Error reading sample-");
      entry(samples[curr_sample]);   /* determine the file type and entry point */
      
      if(fileIterator.hostFileExists(n))  /* if we have a matching goat */
      {
         strcpy(scratch,fileIterator.getHostPath(n));
        
        if(temp_goat=fopen(scratch,"rb"))   /* and we can successfully open it */
        {
          /*printf("and file %s opened too\n",scratch);*/
          /*printf("G: %s] ", goat_name); fflush(stdout);*/
          printf("OK]  ");
          goat_samples[curr_sample]=(struct sample_data *)ALLOC(struct sample_data, 1);
          if((!goat_samples[curr_sample])) mem_err();
          goat_samples[curr_sample]->num_bytes=filesize(temp_goat)/(bHexInput ? 2 : 1); // AJR
          goat_samples[curr_sample]->bytes=(unsigned char *)calloc(goat_samples[curr_sample]->num_bytes,
            sizeof(unsigned char));
          goat_samples[curr_sample]->map_bytes=
            (unsigned char *)calloc(goat_samples[curr_sample]->num_bytes,sizeof(unsigned char));
          if((!goat_samples[curr_sample]->bytes) || (!goat_samples[curr_sample]->map_bytes))
            mem_err();
          
          /* Read the goat file */
          if ((numbytread =
            fileRead(goat_samples[curr_sample]->bytes, 1, goat_samples[curr_sample]->num_bytes, temp_goat, bHexInput)
            ) < goat_samples[curr_sample]->num_bytes) {
            printf ("Error reading goat: %s\nAsked: %d\nRead: %d\nERROR\n", scratch, goat_samples[curr_sample]->num_bytes, numbytread);
          }
          strcpy(goat_samples[curr_sample]->filename,scratch);
          entry(goat_samples[curr_sample]);
          
          samples[curr_sample+1]=goat_samples[curr_sample];   /* put goat sample into array temporarily */
          
          while((match=lcs(&(samples[curr_sample]),2,NULL,0,NULL)) && (match->length >= min_goat_match))  /* match to find goat parts in inf */
          {
            //AJR
#if defined(USE_STRUCTURE)
            adjust_match_boundary_to_program_structure(match,goat_samples[curr_sample]);
            if (match->length < min_goat_match) {
               free (match);
               match = NULL;
               break;
            }
#endif
            found_goat=-1; /* remember that we found a goat match */
            /* mark goat parts invalid-'G' */
            startpos=match->pos_in_sample[0];
            memset(&(samples[curr_sample]->map_bytes[startpos]),'G',match->length);
            memset(&(goat_samples[curr_sample]->map_bytes[match->pos_in_sample[1]]),'G',match->length);
            add_goat_match(&(goat_match[curr_sample]),match);
            
            /* clean up match data */
            free_match(match);
          }
          
          /* clean up */
          // 6/25/97 Fred: Added the following line
          free_match(match);
          fclose(temp_goat);
        } // endif we can open the matching goat succesfully
        else printf("Can't Open]  ");
        /*curr_sample++;*/       /* riad */
      } // endif we have a matching goat
      else printf("No]  ");
      fflush(stdout);
      /* else
      {*/
      /* let's free the unuseful memory already alocated */
      /* the best thing to do, is to check for matching goat before allocating memory */
      
      /*free(samples[curr_sample]->bytes);
      free(samples[curr_sample]->map_bytes);
    }*/
      fclose(curr_file);  /* done with this file */
      curr_sample++;   /* next sample */
    } // endif we can open the infected file
      }
      
      if(timings)
      {
        time(&end_time);
        printf("\nFinished reading data & matching goats at time %d\n",end_time-start_time);
      }
      
      fprintf(map_out,"Run for directory: %s\n",infected_dir);
      fprintf(viral_out,"Run for directory: %s\n",infected_dir);
      fprintf(vds_out,"* AutoVerv analysis for directory: %s\n",infected_dir);
      fprintf(viral_out,"Goat data matched: ");
      if(found_goat)
        fprintf(viral_out,"Y\n");
      else
        fprintf(viral_out,"N\n");
      
        /* now the samples have been read in, goat parts eliminated from consideration
      as appropriate. */
      /* first get invariant bytes & map_bytes buffers for each of the samples */
      
      invariant_bytes=(unsigned char***)calloc(num_samples,sizeof(unsigned char **));
      invariant_map_bytes=(unsigned char***)calloc(num_samples,sizeof(unsigned char **));
      
      for(i=0;i<num_samples;i++)
      {
        invariant_bytes[i]=(unsigned char **)calloc(encryption_types,sizeof(unsigned char*));
        invariant_map_bytes[i]=(unsigned char **)calloc(encryption_types,sizeof(unsigned char*));
        for(type=0;type<encryption_types;type++)
        {
          create_invariant(samples[i],type,&(invariant_bytes[i][type]),
            &(invariant_map_bytes[i][type]));
          
        }
      }
      
      if(timings)
      {
        time(&end_time);
        printf("Finished creating invariants at time %d\n",end_time-start_time);
      }
      
      if (speedup == 0) {
        /* now go through the (painful) exercise of deciding if certain samples */
        /* should be excluded from consideration */
        if(num_samples > 2)
        {
          dp = (double *)calloc(num_samples,sizeof(double));
          dp_self = (double *)calloc(num_samples,sizeof(double));
          dp_cum = 0.0;
          
          for(type = 2;type<encryption_types;type++)  /* only do for ADD1 and XOR2 */
          {
            /*     printf("Encryption type: %s",encrypt_names[type]); */
            for(i=0;i<num_samples;i++)
            {
              samples[i]->bytes = invariant_bytes[i][type];
              samples[i]->map_bytes = invariant_map_bytes[i][type];
              index_bigrams(samples[i]);
              dp_self[i] = 0.0;
              /* dunga */ 
              /* printf("samples[%d]->bytes=%s - \n samples[%d]->map_bytes=%s\n",i,samples[i]->bytes,i,samples[i]->map_bytes);*/
              for(bigram=0;bigram<65536L;bigram++)
              {
                if(samples[i]->bigram_counts[bigram] > count_limit)
                  samples[i]->bigram_counts[bigram] = 0;
                dp_self[i] += (samples[i]->bigram_counts[bigram] * samples[i]->bigram_counts[bigram]);
              }
            }
            for(i=0;i<num_samples;i++)
            {
              for(j=0;j<i;j++)
              {
                dotprod = dot_product(samples[i]->bigram_counts,samples[j]->bigram_counts,
                  dp_self[i], dp_self[j]);
                dp[j] += dotprod;
                dp[i] += dotprod;
                dp_cum += dotprod;
                
                /* dunga */ /*printf("Sample %d vs Sample %d = %lf\n",i,j,dotprod);*/
              }
              
              /* printf("\n"); */
            }
            /* clean up */
            for(i=0;i<num_samples;i++)
              for(bigram=0;bigram<65536L;bigram++)
                if(samples[i]->position[bigram])
                  free(samples[i]->position[bigram]);
          }
          /* .<- # of encryption types used in dot product calculations */
          dp_cum /= (double) (num_samples*(num_samples-1)/2.0*2);
          
          /* dunga */ /*printf("dp_cum is %lf.\n",dp_cum);*/ 
          
          orig_num_samples = num_samples;
          for(j=0;j<num_samples;j++)
          {
            // AJR: don't eliminate sample.*
            //?? should have left it out of the dot-product calculation above?
            const char* zFileName = max(strrchr(samples[j]->filename,'\\')+1,samples[j]->filename);
          if (!_memicmp(zFileName,"sample", 6)) continue;
            /* .<- this 2 = # of encryption types used in dot product calculations */
            dp[j]/= (double) (2*(orig_num_samples-1));
            /* dunga */ /*printf("Average for sample %d is %lf.\n",j,dp[j]);*/ 
            
            if(dp[j] < (0.3 * dp_cum))
            {
              fprintf(viral_out,"Sample %s is strange -- excluding it.\n",samples[j]->filename);
              fprintf(map_out,"Sample %s is strange -- excluding it.\n",samples[j]->filename);
              fprintf(vds_out,"* Sample %s is strange -- excluding it.\n",samples[j]->filename);
              if(chatty) printf("Sample %s is strange -- excluding it.\n",samples[j]->filename);
              
              // AJR: if left in place, verifyRepair will try to repair it
              fprintf (fExcludeLog, "%s\n", zFileName);
              if (ferror(fExcludeLog)) {
                 perror ("exclude log write");
                 exit(1);
              }

              //free(samples[j]->filename);  /* free strange sample */
              free(samples[j]);
              free_pos(goat_match[j]);
              samples[j]=samples[num_samples-1];   /* move last sample into its position */
              goat_samples[j]=goat_samples[num_samples-1];
              dp[j]=dp[num_samples-1];
              goat_match[j]=goat_match[num_samples-1];
              for(i=0;i<encryption_types;i++)
              {
                free(invariant_bytes[j][i]);
                free(invariant_map_bytes[j][i]);
                invariant_bytes[j][i]=invariant_bytes[num_samples-1][i];
                invariant_map_bytes[j][i]=invariant_map_bytes[num_samples-1][i];
              }
              num_samples--;     /* we now have one less sample */
              j--;  /* check the moved one */
            }
          }
          free(dp);
          free(dp_self);
          
          if(timings)
          {
            time(&end_time);
            printf("Finished dot product computation at time %d\n",end_time-start_time);
          }
        } // end if (num_samples > 2)
      } // end if (speedup == 0)
      // AJR
      fclose(fExcludeLog);
      
      /* deal with zero or one sample cases */
      
      if(num_samples < 2)
      {
        if(num_samples==0)
        {
          fprintf(viral_out,"No samples\n\n");
          fprintf(map_out,"No samples\n\n");
          fprintf(vds_out,"* No samples\n\n\n");
        }
        if(num_samples==1)
        {
          fprintf(viral_out,"Only one sample\n\n");
          fprintf(map_out,"Only one sample\n\n");
          fprintf(vds_out,"* Only one sample\n\n\n");
        }
        printf("Done!\n\n");
        doneprg(0,viral_out,map_out,vds_out);
      }
      
      
      for(i=0;i<file_types;i++)
      {
        /* get universal goat descriptions */
        uni_goat_match[i] = make_universal_goat(i,samples,goat_samples,num_samples,goat_match);
        num[i] = 0;  /* initialize filetype counters */
        num_with_goats[i] = 0;
      }

      //AJR
      UniGoatRTB = getUniGoatRelocationTableBoundaries (goat_samples, num_samples);
      
      for(i=0;i<num_samples;i++)
      {
        filetype = get_file_type(samples[i]);  /* get number based on file type */
        if(filetype >= 0)
        {
          num[filetype]++;  /* count it */
          if(goat_samples[i])  /* if it has a goat */
            num_with_goats[filetype]++;
        }
      }
      
      /* initialize the CRC table with hardwired password */
      crc_table = crc_setup();
      
      if(timings)
      {
        time(&end_time);
        printf("About to start virus matching at time %d\n",end_time-start_time);
      }
      
      /* initialize last best match length values to "big" */
      for(i=0;i<encryption_types;i++)
        last_best[i]=MAXINT;
      
      
      /* now do the matching */
#if 1
      //Fred: Just for speed, I modified lcs
      
      suggested_start = (unsigned *) calloc (num_samples,sizeof(unsigned));
      if (suggested_start == NULL) mem_err();
      for (i=0;i<num_samples;i++)
        suggested_start [i] = samples [i]->entry_pt;
      if (num[COM]>0 && num[EXE]>0) { /* If we have both types */
        for (i=1;i<num_samples;i++)
          if (get_file_type(samples[i]) != get_file_type(samples[0]))
          {othertypei=i;break;}
      }
      if (othertypei>0) {
        if (get_file_type(samples[0]) == COM) {
          sampleCOM = samples[0];
          sampleEXE = samples[othertypei];
        } else {
          sampleEXE = samples[0];
          sampleCOM = samples[othertypei];
        }
        fromCOM = sampleCOM->entry_pt;
        while (fromCOM>0 && fromCOM>sampleCOM->entry_pt-20 && sampleCOM->map_bytes[fromCOM-1] == 0) fromCOM--;
        toCOM = sampleCOM->entry_pt;
        while (toCOM<sampleCOM->num_bytes-1 && toCOM<sampleCOM->entry_pt+20 && sampleCOM->map_bytes[toCOM+1] == 0) toCOM++;
        
        if (toCOM-fromCOM>20) {
          fromEXE = sampleEXE->entry_pt-10000;
          toEXE = sampleEXE->entry_pt+10000;
          if (fromEXE < 0) fromEXE = 0;
          if (toEXE > sampleEXE->num_bytes-1) toEXE = sampleEXE->num_bytes-1;
          
          for (i=fromEXE ; i<toEXE-20 ; i++) {
            if (sampleEXE->map_bytes[i] || sampleCOM->bytes[fromCOM] != sampleEXE->bytes[i]) continue;
            for (j=0 ; fromCOM+j<=toCOM && j<20 && sampleCOM->bytes[fromCOM+j]==sampleEXE->bytes[i+j] && sampleEXE->map_bytes[i+j]==0; j++);
            if (j == 20) {
              COMEPfound = 1;
              ofsCOMtoEXE = (i-sampleEXE->entry_pt) - (fromCOM-sampleCOM->entry_pt);
              break;
            }
            i+=j-1;
          }
        }
      }
      if (COMEPfound) {
        for (i=0;i<num_samples;i++)
          if (get_file_type(samples[i]) == EXE) {
            newsuggstart = suggested_start[i]+ofsCOMtoEXE;
            if (newsuggstart>0 && newsuggstart<samples[i]->num_bytes)
              suggested_start[i] = newsuggstart;
          }
      }
      notgoatnumbytes = (int *) calloc (num_samples,sizeof(int));
      if (notgoatnumbytes == NULL) mem_err();
      for (i=0;i<num_samples;i++) {
        for (j=0;j<samples[i]->num_bytes;j++)
          if (samples[i]->map_bytes[j] == 0)
            notgoatnumbytes [i]++;
      }
      for (i=0;i<num_samples;i++)
        if (notgoatnumbytes[i]<minnotgoatnumbytes)
          minnotgoatnumbytes = notgoatnumbytes[i];
        lowerbound = minnotgoatnumbytes/5;
        if (lowerbound<100) lowerbound=100;
        // End modif
#endif
        
        do {
          /* initial match is 0 */
          best_match = NULL;
          best_len = 0;
          
          /* the begining of the most consuming routine */
          
          newlcsmatch=0;
          for(type=0;type<encryption_types;type++)   /* for each encryption type */
            if(last_best[type] > best_len)  /* only if last best using this encryption */
              /* was longer than current best match */
            {
              for(i=0;i<num_samples;i++)
              {
                samples[i]->bytes = invariant_bytes[i][type];
                samples[i]->map_bytes = invariant_map_bytes[i][type];
              }
              /* this is the time consuming routine here */
#if defined (LCS_APPROX)
              match = lcs(samples, num_samples, suggested_start, lowerbound, &approxworked); //Quick version
#else
              match = lcs(samples, num_samples, NULL,            lowerbound, &approxworked); //original version
#endif
              if(match) {                           /* if there was a match */
                //printf ("Type: %d -> Len: %d\t", type, match->length);
                last_best[type]=match->length;      /* remember this last best length */
                if(match->length>best_len)          /* if this was the best yet */
                  //Fred: Just a test to see if we eliminate a match because of the key length
                  //if(match->length>best_len+2)          /* if this was the best yet */
                {
                  free_match(best_match);         /* forget the old match */
                  best_match=match;               /* make this the new best */
                  best_match->type=type;          /* remember encryption type */
                  best_len=best_match->length;    /* remember the length */
                  newlcsmatch = approxworked;
                }
                else
                  free_match(match);                /* otherwise, forget this match */
              }
            }
            //printf ("\n");
            
            /* the end of the most consuming routine */
            
            if(best_len >= min_match_length)
            {
              //if(best_match->type && 
              // ((best_len % key_length[best_match->type]) != 0))  /* if length is not multiple of key length */
              //  best_len -= best_len % key_length[best_match->type];  /* make it so */
              //best_len += key_length[best_match->type];  /* adjust for key length */
              //best_match->length = best_len;
              
              //Fred: adjust differently:
              int best_len_mod_key_length = (best_match->type)?best_len%key_length [best_match->type]:0;
              best_len += (best_len_mod_key_length)?key_length [best_match->type]-best_len_mod_key_length:0;
              best_match->length = best_len;
              
#if 1
              fixedbytesfound += best_len;
              if (newlcsmatch) foundwithnewlcs += best_len;
              lowerbound = (minnotgoatnumbytes-fixedbytesfound)/5;
              if (lowerbound<100) lowerbound=100;
              if (foundwithnewlcs == fixedbytesfound && fixedbytesfound>100)
                lowerbound = min_match_length;
#endif
              
              best_match_location = find_best_anchor(best_match->pos_in_sample, num_samples, samples);
              
              for(i=0;i<num_samples;i++)
              {
#if 1
                //printf ("%d, MPos=%d, SOfs=%d, MPos-SOfs=%d, End-MPos=%d\n", i, best_match->pos_in_sample[i], suggested_start[i], best_match->pos_in_sample[i]-suggested_start[i], samples[i]->num_bytes-best_match->pos_in_sample[i]);
                suggested_start [i] = best_match->pos_in_sample[i];
#endif
                /* mark bytes as invalid-'V' (virus) */
                startpos = best_match->pos_in_sample[i];
                for(type=0;type<encryption_types;type++)
                  memset(&(invariant_map_bytes[i][type][startpos]),'V',best_len);
                
                /* calculate offsets from various anchors */
                b = startpos;
                t = startpos-samples[i]->entry_pt;
                e = startpos-samples[i]->num_bytes+1;
                
                /* print info */
                fprintf(viral_out,"Position in %s: B%+d   T%+d   E%+d\n",
                  samples[i]->filename,b,t,e);
              }
              
              vmatch=add_virus_match(&virus_match,best_match,best_match_location);  /* add the match to the list */
              
              if(timings)
              {
                time(&end_time);
                printf ("Found match ");
                if (foundwithnewlcs) printf ("with new lcs ");
                printf ("of length%6d, encryption %d location  COM: %c,%d,%d  EXE: %c,%d,%d  ALL: %c,%d,%d at time %d\n",
                        best_len,best_match->type,
                        anchorchar[best_match_location->bestanchor[0]],best_match_location->offset_min[0],best_match_location->offset_range[0],
                        anchorchar[best_match_location->bestanchor[1]],best_match_location->offset_min[1],best_match_location->offset_range[1],
                        anchorchar[best_match_location->bestanchor[2]],best_match_location->offset_min[2],best_match_location->offset_range[2],
                        end_time-start_time);
              }
              
              fprintf(viral_out,"Match encryption type: %s\n",encrypt_names[best_match->type]);
              fprintf(viral_out,"Match length: %d\n",best_len);
              
              /* decrypt (if necessary) with first byte as key */
              tempbuffer = decrypt(best_match,&(invariant_bytes[0][0][best_match->pos_in_sample[0]]));
              /* calculate crc of region */
              vmatch->crc = crc_calc(crc_table, 0L, tempbuffer, best_match->length);
              
              fprintf(viral_out,"Invariant bytes: ");
              for(j=0;j<best_len;j++)
                fprintf(viral_out,"%02X",tempbuffer[j]);  /* print the bytes */
              fprintf(viral_out,"\n");
              free(tempbuffer);    /* release the temporary buffer */
              
              if((num[COM] && num[EXE]) &&
                ((best_match_location->offset_range[ALL] <= best_match_location->offset_range[COM])
                && (best_match_location->offset_range[ALL] <= best_match_location->offset_range[EXE])))
                fprintf(viral_out,"Best fit - ALL: %c%+d (+%d)\n",anchorchar[best_match_location->bestanchor[ALL]],
                best_match_location->offset_min[ALL],best_match_location->offset_range[ALL]);
              else
              {
                if(num[COM])
                  fprintf(viral_out,"Best fit - COM: %c%+d (+%d)\n",anchorchar[best_match_location->bestanchor[COM]],
                  best_match_location->offset_min[COM],best_match_location->offset_range[COM]);
                if(num[EXE])
                  fprintf(viral_out,"Best fit - EXE: %c%+d (+%d)\n",anchorchar[best_match_location->bestanchor[EXE]],
                  best_match_location->offset_min[EXE],best_match_location->offset_range[EXE]);
              }
              
              free(best_match_location);
              fprintf(viral_out,"\n");
            }
            // 6/27/97 Fred: added the 2 following lines
            else
              free_match (best_match);
            
            /* check to see if the time limit (if any) has been exceeded */
            if(time_limit)
            {
              time(&end_time);
              if((end_time - start_time) > time_limit)
              {
                printf("Time limit exceeded. Aborting.\n\n");
                fprintf(viral_out,"Time limit exceeded. Aborting.\n\n");
                doneprg(1,viral_out,map_out,vds_out);
              }
            }
            
      } while (best_len >= min_match_length);
      
      if(timings)
      {
        time(&end_time);
        printf("Finished virus matching at time %d\n",end_time-start_time);
      }
      
      /* return to plain type bytes in samples */
      for(i=0;i<num_samples;i++)
      {
        samples[i]->bytes=invariant_bytes[i][0];
        samples[i]->map_bytes=invariant_map_bytes[i][0];
      }
      
      for(i=0;i<file_types;i++)  
      {
        if(num[i])
        {
          /* create linked list map of infected samples */
          map_list[i] = make_map_list(virus_match, uni_goat_match[i], i);
        }
        else
          map_list[i] = NULL;
        
        if(num_with_goats[i] > 1)  /* if at least two samples with goats */
        {
          /* set up for Jeff's reconstruction stuff */
          key_list[i] = make_key_list(virus_match, samples, goat_samples, num_samples,i);
          make_recon_arrays(samples,goat_samples,num_samples,uni_goat_match[i],
            map_list[i],i,&(goat_bytes[i]),&(variable_virus_bytes[i]),
            num_with_goats[i], &num_goat_bytes[i], &num_vv_bytes[i]);
          
          if(chatty)
          {
            printf("\nfiletype: %d  bytes to reconstruct: %d\n",i,num_goat_bytes[i]);
            {for(int n=0;n<num_goat_bytes[i];n++)printf("%c,%d ",anchorchar[goat_bytes[i][n].loc.anchor_type],goat_bytes[i][n].loc.offset);putchar('\n');}
            printf(" ... %d variable virus bytes to choose from.\n",num_vv_bytes[i]);
            {for(int n=0;n<num_vv_bytes[i];n++)printf("%c,%d ",anchorchar[variable_virus_bytes[i][n].loc.anchor_type],variable_virus_bytes[i][n].loc.offset);putchar('\n');}
          }
          
          if(num_goat_bytes[i]<=num_vv_bytes[i] && num_goat_bytes[i]<100 && num_vv_bytes[i]<1000)
            possible_matches = reconstruct (num_with_goats[i],
            num_goat_bytes[i],
            num_vv_bytes[i],
            goat_bytes[i],
            variable_virus_bytes[i],
            key_list[i]);
          else
            possible_matches = NULL;
          
#if 0
          if(possible_matches)
          {
            for(j=0;j<num_goat_bytes[i];j++)
            {
              if(possible_matches[j])
              {
                printf("Byte %d can be reconstructed.\n",j);
                pm = possible_matches[j];
                while(pm)
                {
                  printf("Match to goat byte %d: ",j);
                  crypt_info = pm->encryption_info;
                  printf("Virus byte %c+%d, crypt type %d, anchor_type %c, key offset %d, key correction %02X\n",
                    (pm->loc).anchor_type,
                    (pm->loc).offset,
                    crypt_info.crypt_type,
                    anchorchar[(crypt_info.key_location).anchor_type],
                    (crypt_info.key_location).offset,f
                    crypt_info.key_correction[0]);
                  pm = pm->next;
                }
              }
            }
          }
#endif
          if(possible_matches)
          {
            reconstruction_summary[i] = greedy_cover(num_goat_bytes[i],goat_bytes[i],possible_matches,i,UniGoatRTB);
            recon_list[i] = make_reconstruction_list(reconstruction_summary[i],
              goat_bytes[i], num_goat_bytes[i]);
            reconstruct_status(reconstruction_summary[i],goat_bytes[i],num_goat_bytes[i],
              i,&(num_missing[i]),&(calc_mz[i]));
            
            if(chatty)
            {
              r_summary = reconstruction_summary[i];
              while(r_summary)
              {
                crypt_info = r_summary->encryption_info;
                printf("\n\nEncryption type: %d (%s).\n", crypt_info.crypt_type, encrypt_names[crypt_info.crypt_type]);
                // 6/18/97 Fred: Added the following line to correct a bug
                // crypt_type = 0 -> No encryption -> Variables not initialized
                if (crypt_info.crypt_type != 0)
                  printf("Key at %c+%d, key correction is %02X.\n",
                  anchorchar[crypt_info.key_location.anchor_type],
                  crypt_info.key_location.offset,
                  crypt_info.key_correction[0]);
                printf("%c + ",anchorchar[r_summary->g_anchor_type]);
                for(j=0;j<num_goat_bytes[i];j++)
                  if(r_summary->covered_goat_bytes[j])
                    printf("%d ",goat_bytes[i][j].loc.offset);
                  printf(" matches ");
                  printf("%c + ",anchorchar[r_summary->vv_anchor_type]);
                  for(j=0;j<num_goat_bytes[i];j++)
                    if(r_summary->covered_goat_bytes[j])
                      printf("%d ",goat_bytes[i][j].loc.offset + r_summary->relative_offset);      
                    printf("\n");
                    r_summary = r_summary->next;
              }
              rlist = recon_list[i];
              while(rlist)
              {
                printf("Reconstruct goat %c%+d from infected goat %c%+d for length %d\n",
                  anchorchar[rlist->g_begin.anchor_type],rlist->g_begin.offset,
                  anchorchar[rlist->ig_begin.anchor_type],rlist->ig_begin.offset,
                  rlist->length);
                rlist=rlist->next;
              }
            }
          }
          else
          {
            reconstruction_summary[i] = NULL;
            recon_list[i] = NULL;
          }
          
          // 6/27/97 Fred: Added the following lines to free the memory correctly
          // Despite all my efforts, I could not get that to work properly, so I removed it
          // Just hope Windows does its job of freeing memory correctly after the end of the program...
          //for (j=0;j<num_goat_bytes[i];j++) {
          //if (possible_matches) {
          //while (possible_matches[j]) {
          //pm = possible_matches[j]->next;
          // 6/30/97 Fred: we cannot even do the following test and free because key_correction is sometimes
          // not null but already deallocated: this code isn't worth peanuts
          //if (possible_matches [j]->encryption_info.key_correction)
          //  free (possible_matches [j]->encryption_info.key_correction);
          //free (possible_matches [j]);
          //possible_matches [j] = pm;
          //}
          //free(possible_matches);
          //possible_matches = NULL;
          //}
          //}
          // modif ends here
          
        }
      }
      
      
      if(timings)
      {
        time(&end_time);
        printf("Finished reconstruction at time %d\n",end_time-start_time);
      }
      
#ifdef VERV_CODO
      if(patchos==1)
      {
        printf("\nPatching Map_List with CodoData\n");
        /* map_list:     list of different sections
        * samples:      list of different used samples
        * infected_dir: used to fetch the .codo filename
        */
        /* update map_list with each filename */
        if(patch_map_list(map_list, 
          samples, 
          num_samples, 
          infected_dir,
          crc_table) == NOT_ENOUGMEM)
          fprintf(stderr, "\nERROR: not enough memory to patch map_list\n");
      }
#endif
      
      if(recon_list[0] || recon_list[1] || num_goat_bytes [0] == 0 || num_goat_bytes [1] == 0)
        write_vds(vds_out,infected_dir,samples,num_samples,map_list,
        uni_goat_match,num,num_with_goats,recon_list,
        num_missing,calc_mz);  /* write VERV info */
      
      write_graphic(virusname /*infected_dir*/, map_list, recon_list); /* Write VP virus-attachment picture file */
      write_bytes(virusname /*infected_dir*/,samples[0],virus_match);
      write_bytes2(virusname /*infected_dir*/,samples[0],map_list,patchos);
      
      printf("\nFinishing writing graphic, bytes, bytes2 and vds files\n");
      fflush(stdout);
      
      /* record region map */
      for(i=0;i<num_samples;i++)
      {
        ch=samples[i]->map_bytes[0];
        len=1;
        fprintf(map_out,"Map for sample: %s\n",samples[i]->filename);
        fprintf(map_out,"File type: %s\n",samples[i]->file_type);
        fprintf(map_out,"Entry point: %d\n\n",samples[i]->entry_pt);
        fprintf(map_out,"Region type: %c   Offset: %6d   ",region_char(ch),0);
        for(j=1;j<samples[i]->num_bytes;j++)
        {
          if(samples[i]->map_bytes[j] == ch)
            len++;  /* counting bytes in current region */
          else
          {
            fprintf(map_out,"Length: %6d\n",len);    /* end of region */
            /* and start the new one */
            ch = samples[i]->map_bytes[j];
            fprintf(map_out,"Region type: %c   Offset: %6d   ",region_char(ch),j);
            len=1;
          }
        }
        /* print end of final region */
        fprintf(map_out,"Length: %6d\n\n",len);    /* end of region */
        fprintf(map_out,"\n");
      }
      
      
      
      time(&end_time);
      printf("Done! (Elapsed time: %d sec)\n\n",end_time-start_time); /* we're done */
      
      
      if(chatty)
      {   /* spew some debugging stuff */
        
        for(i=0;i<num_samples;i++)
        {
          curr=goat_match[i];
          printf("Sample %d (%s):\n",i,samples[i]->filename);
          
          while(curr)
          {
            startpos = curr->infgoatpos;
            printf("Goat at %d from infected goat at b+%d t%+d e-%d to ",curr->goatpos,startpos,
              startpos-samples[i]->entry_pt,samples[i]->num_bytes-startpos-1);
            startpos = startpos+curr->length-1;
            printf("b+%d t%+d e-%d\n",startpos,startpos-samples[i]->entry_pt,samples[i]->num_bytes-startpos-1);
            curr=curr->next;
          }
        }
        
        printf("\n\n");
        
        for(i=0;i<file_types;i++)
        {
          printf("Filetype: %d\n",i);
          printf("------------\n");
          ug_match = uni_goat_match[i];
          while(ug_match)
          {
            printf("Goat section -- from original goat at %c%+d (+%d) to %c%+d (+%d)\n",
              anchorchar[ug_match->g_begin.bestanchor], ug_match->g_begin.offset_min, 
              ug_match->g_begin.offset_range, anchorchar[ug_match->g_end.bestanchor], 
              ug_match->g_end.offset_min, ug_match->g_end.offset_range);
            printf("                     infected goat at %c%+d (+%d) to %c%+d (+%d)\n\n",
              anchorchar[ug_match->ig_begin.bestanchor], ug_match->ig_begin.offset_min, 
              ug_match->ig_begin.offset_range, anchorchar[ug_match->ig_end.bestanchor], 
              ug_match->ig_end.offset_min, ug_match->ig_end.offset_range);
            ug_match = ug_match->next;
          }
        }
        
        printf("\n\n");
        
        for(i=0;i<file_types;i++)
        {
          printf("Filetype: %d\n",i);
          printf("------------\n");
          m_list = map_list[i];
          while(m_list)
          {
            get_begin_boundary(&tmp_begin,m_list,i);
            get_end_boundary(&tmp_end,m_list,i);
            printf("Section type: %c  from %c%+d (+%d) to %c%+d (+%d)\n",m_list->record_type,
              anchorchar[tmp_begin.bestanchor],tmp_begin.offset_min,tmp_begin.offset_range,
              anchorchar[tmp_end.bestanchor],tmp_end.offset_min,tmp_end.offset_range);
            m_list=m_list->next;
          }
        }
      }
      
      // 6/27/97 Fred: Free the samples and everything else
      // Freed pts set to NULL for debug purposes
      for (i=0;i<num_samples;i++) {
        if (samples [i]) {
          free(samples [i]->bytes); samples [i]->bytes = NULL;
          free(samples [i]->map_bytes); samples [i]->map_bytes = NULL;
          samples [i] = NULL;
        }
      }
      free (samples);
      for (i=0;i<num_samples;i++) {
        if (goat_samples [i]) {
          free(goat_samples [i]->bytes); goat_samples [i]->bytes = NULL;
          free(goat_samples [i]->map_bytes); goat_samples [i]->map_bytes = NULL;
          goat_samples [i] = NULL;
        }
      }
      free (goat_samples);
      free (crc_table);
      for (i=0;i<num_samples;i++) {
        if (goat_match [i]) {
          free_pos (goat_match [i]);
          goat_match [i] = NULL; //DEBUG
        }
      }
      free (goat_match);
      for (i=0;i<num_samples;i++) {
        // Don't do it for the plain type since the invariant_bytes array is just another ptr to the sample[]->bytes array
        for (type=1;type<encryption_types;type++) {
          free (invariant_bytes [i][type]);
          free (invariant_map_bytes [i][type]);
        }
        free (invariant_bytes [i]);
        free (invariant_map_bytes [i]);
      }
      free (invariant_bytes);
      free (invariant_map_bytes);
      for(i=0;i<file_types;i++) {
        while(map_list [i]) {
          m_list = map_list[i]->next;
          // Free everything inside the map_list... I don't do it now (friday evening...)
          free (map_list [i]);
          map_list [i] = m_list;
        }
      }
      for (type=0;type<file_types;type++) {
        // Free goats_bytes [type] with all its substructures...
      }
      // Fred: Modified section ends here
      
      /* clean up */
      doneprg(0,viral_out,map_out,vds_out);
      
      return 0;
 }
 
 /*----------------------------------------------------------------------------------------------------------*/
 
 void endprg(void)
 {
   printf("\nSyntax: autoverv <infected directory> <goat directory> [-ptc]\n\n");
   printf("  -- Files in the goat directory with extension .COM and .EXE are considered.\n");
   printf("  -- Files in the infected directory with extensions of .VOR, .VOM, and\n");
   printf("     .VXE are considered\n");
   printf("  -- The -t (timings) switch enables the printing to stdout of timing\n");
   printf("     information\n");
   printf("  -- The -c (chatty) switch enables the printing to stdout of miscellaneous\n");
   printf("     debugging type information\n");
   printf("  -- The -p (patch) is to use the codedata info with autoverv\n");
   printf("\n");
   exit(1);
 }
 
 /*----------------------------------------------------------------------------------------------------------*/
 
 void doneprg(int rc, FILE *viral_out, FILE *map_out, FILE *vds_out)
 {
   fprintf(viral_out,"\n================================================================================\n\n");
   fprintf(map_out,"\n================================================================================\n\n");
   fclose(viral_out);
   fclose(map_out);
   fclose(vds_out);
   exit(rc);
 }
 
 /*----------------------------------------------------------------------------------------------------------*/
 
 void fixpath(char *pathstr)
 {
   /*    #ifdef aix */
#ifdef _WIN32
   /*if(pathstr[strlen(pathstr)-1] == '\\') pathstr[strlen(pathstr)-1] = '\0';*/
   if(pathstr[strlen(pathstr)-1] != '\\') strcat(pathstr,"\\");
   /*strcat(pathstr, "*.*");*/
#else
   if(pathstr[strlen(pathstr)-1] != '/') strcat(pathstr,"/");
#endif
   /* #else 
   if(pathstr[strlen(pathstr)-1] != '\\')
   strcat(pathstr,"\\");
#endif*/
 }
  
 /*----------------------------------------------------------------------------------------------------------*/
 
 char region_char(char c)
 {
   if(c==0)
     return('?');
   else
     return(c);
 }
 
 /*----------------------------------------------------------------------------------------------------------*/
 
 char *strlwr(char *s) /* converts string to lower case */
 {
   int i;
   
   for(i=0;s[i];i++)
     s[i] = tolower(s[i]);
   return(s);
 }
 
 /*----------------------------------------------------------------------------------------------------------*/
 
 _inline double dot_product(int counts1[65536], int counts2[65536], double result11, double result22)
 {
   register int i;
   register int c1;
   register int c2;
   double result12;
   
   result12=counts1[0]*counts2[0];
   for(i=65536;--i;) {
     c1 = counts1[i];
     if (c1 == 0) continue;
     c2 = counts2[i];
     if (c2 == 0) continue;
     result12 += (double)(c1*c2);
   }
   
   return(result12 / sqrt(result11*result22));
 }
 
 /*----------------------------------------------------------------------------------------------------------*/
 
 void add_goat_match(struct posrec **goat_match, struct match_data *match)
 {
   struct posrec *curr, *newMatch;
   
   newMatch = (struct posrec *)ALLOC(struct posrec, 1);  /* allocate a record */
   /* fill in the data appropriately */
   newMatch->goatpos = match->pos_in_sample[1];
   newMatch->infgoatpos = match->pos_in_sample[0];
   newMatch->length = match->length;
   
   curr = *goat_match;
   if(!curr)   /* if no match yet */
   {
     *goat_match=newMatch;  /* start the list */
     newMatch->next=NULL;
   }
   else if(match->pos_in_sample[1] < curr->goatpos)  /* if belongs at beginning of list */
   {
     *goat_match=newMatch;
     newMatch->next=curr;
   }
   else
   {
     /* find spot for record in list (sort by goatpos) */
     while(curr->next && (curr->next->goatpos < match->pos_in_sample[1]))
       curr=curr->next; 
     
     /* link in the new one */
     newMatch->next = curr->next;
     curr->next = newMatch;
   }
 }
 
 /*----------------------------------------------------------------------------------------------------------*/
 
 struct match_list *add_virus_match(struct match_list **list, struct match_data *match,
   struct boundary_location *best_match_location)
   
 {
   struct match_list *curr;
   int filetype;
   
   curr=*list;
   if(!curr)
   {
     *list = (struct match_list *)ALLOC(struct match_list, 1);
     curr = *list;
   }
   else
   {
     while(curr->next)
       curr = curr->next;   /* go to end of list */
     curr->next = (struct match_list *)ALLOC(struct match_list, 1);
     curr=curr->next;
   }
   
   curr->match = match;
   for(filetype=0;filetype<=file_types;filetype++)
   {
     curr->begin.bestanchor[filetype] = best_match_location->bestanchor[filetype];
     curr->begin.offset_min[filetype] = best_match_location->offset_min[filetype];
     curr->begin.offset_range[filetype] = best_match_location->offset_range[filetype];
   }
   curr->next = NULL;
   return(curr);
 } 
 
 /*----------------------------------------------------------------------------------------------------------*/

// AJR
//These two functions assume a valid header (e.g. an uninfected goat)

int next_ProgramSection_Beginning (const int offset, const int fileType, const EXErelocationTableBoundaries RTB) {
   if (fileType == EXE) {   //if we ever have more types, this could be a switch
      if (offset < 0x1C) {
         return offset + (offset % 2);
      }else if (offset > RTB.start && offset <= RTB.end) {
         const int offset_in_table = offset - RTB.start;
         if (offset_in_table % 4) {
            return offset + 4 - (offset_in_table % 4);
         }
      }
   }
   return offset;
}



int prior_ProgramSection_End (const int offset, const int fileType, const EXErelocationTableBoundaries RTB){
   if (fileType == EXE) {   //if we ever have more types, this could be a switch
      if (offset < 0x1C) {
         if (offset % 2 != 1) {
            return offset /*- (offset % 2)*/ - 1;
         }
      }else if (offset > RTB.start && offset <= RTB.end) {
         const int offset_in_table = offset - RTB.start;
         if (offset_in_table % 4 != 3) {
            return offset - (offset_in_table % 4) - 1;
         }
      }
   }
   return offset;   // may be -1
}

//-----------------------------------------------------------------------------------------------
//AJR
static void adjust_match_boundary_to_program_structure (
               struct match_data* pMatch, 
               struct sample_data* pGoatSample)
{
   //assumes pMatch->pos_in_sample is a 2-integer array, with the first entry giving the 
   //offset of the match in the infected file, and the second entry giving the offset
   //in the goat.

   int i;

   const int fileType = get_file_type(pGoatSample);
   const EXErelocationTableBoundaries RTB = getRelocationTableBoundaries(pGoatSample);

   const int match_start_in_goat = pMatch->pos_in_sample[1];
   const int adjusted_match_start = next_ProgramSection_Beginning(match_start_in_goat,fileType,RTB);
   const int match_start_delta = adjusted_match_start - match_start_in_goat;

   const int match_end_in_goat = match_start_in_goat + pMatch->length - 1;
   const int adjusted_match_end = prior_ProgramSection_End(match_end_in_goat,fileType,RTB);
   const int match_end_delta = adjusted_match_end - match_end_in_goat;
 
   if (match_start_delta) {
      pMatch->length -= match_start_delta;
      for (i = 0; i < 2; i++) {
         pMatch->pos_in_sample[i] += match_start_delta;
      }
   }
 
   if (match_end_delta) {
      pMatch->length += match_end_delta;   //+ve delta increases length
   }

   if (pMatch->length < 0) {
      pMatch->length = 0;
   }
}



static EXErelocationTableBoundaries getRelocationTableBoundaries (struct sample_data* pSample) {
   EXErelocationTableBoundaries boundaries = {0,0};
   if (get_file_type(pSample) == EXE) {
      const WORD relocation_table_offset = *(WORD*)(pSample->bytes + 0x18);
      const WORD relocation_table_length = *(WORD*)(pSample->bytes + 0x06);
      if (relocation_table_length) {
         boundaries.start = (int)relocation_table_offset;
         boundaries.end = (int)relocation_table_offset + (int)relocation_table_length * 4 - 1;
      }
   }
   return boundaries;
}



static EXErelocationTableBoundaries getUniGoatRelocationTableBoundaries (
                                       struct sample_data* goat_samples[], 
                                       int num_samples)
{
   EXErelocationTableBoundaries uniBoundaries = {0,0};
   int i;
   for (i = 0; i < num_samples; i++) {
      if (goat_samples[i] && get_file_type(goat_samples[i]) == EXE) {
         const EXErelocationTableBoundaries currBoundaries = getRelocationTableBoundaries(goat_samples[i]);
         if (currBoundaries.end > currBoundaries.start) {
            if (currBoundaries.start < uniBoundaries.start || uniBoundaries.end == 0) {
               uniBoundaries.start = currBoundaries.start;
            }
            if (currBoundaries.end > uniBoundaries.end || uniBoundaries.end == 0) {
               uniBoundaries.end = currBoundaries.end;
            }
         }
      }
   }
   return uniBoundaries;
}



// AJR: hexdump option

static int x2i (char c) {
	if      (c >= '0' && c <= '9') return c - '0';
	else if (c >= 'a' && c <= 'f') return c - 'a' + 0x0a;
	else if (c >= 'A' && c <= 'F') return c - 'A' + 0x0a;
	return 0;
}

static size_t hex2bfr (void* pDest, const char* zHex, size_t lDest) {
   unsigned char* pDestByte = (unsigned char*)pDest;
   for (size_t i = 0; i < lDest && zHex[2*i] && zHex[2*i+1]; i++) {
      pDestByte[i] = x2i(zHex[2*i]) * 0x10  +  x2i(zHex[2*i + 1]);
   }
   return i;
}


	
static size_t fileRead (unsigned char* buffer, size_t size, size_t count, FILE *stream, bool isHex) {
   char*  pHexBfr;   // to contain the hex digits
   size_t nHex;      // number of objects read

   if (!isHex) return fread(buffer, size, count, stream);

   pHexBfr = (char*)malloc(2*size*count + 1);
   if (!pHexBfr) {puts ("hexfile alloc fail\n"); return 0;}

   nHex = fread(pHexBfr, 2*size, count, stream);
   nHex = hex2bfr (buffer, pHexBfr, nHex);

   free(pHexBfr);
   return nHex;
}
