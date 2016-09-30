
/* useful declarations for UNIX/DOS/WINDOWS compatibility */

#ifdef __UNIX__
#define SLASH  '/'
#define ASLASH '\\'
#else
#define SLASH  '\\'
#define ASLASH '/'
#endif

/* multiple defines used with com_options */

#define        OPT_ALLFILES      0x01
#define        OPT_ONEFILE       0x02
#define        OPT_FROMINDEX     0x04
#define        OPT_DIRECTORY     0x08
#define        OPT_SUBDIRS       0x10
#define        OPT_WOW           0x20
#define        OPT_SAVEHEX       0x40
#define        OPT_DECRYPT       0x80
#define        OPT_NODEBUG       0x100 /* we prefer to force debug */

/* Some useful extensions for output filenames */
#define HEXSUFFIX1   "codo"
#define HEXSUFFIX2   "sections"
#define PATCHSUFFIX  "patch"

/* Various Definitions for CODE DATA segregation code and stuff            */

/* this is a list structure to store the different unconditionnal jumps    */
/* we encountered during emulation                                         */

typedef struct _jmp_stuff
   {
   ULONG    address;    // this is the address we found a JUMP in the code
   short    jumpcond;   // the condition the JUMP is actually into are:
                        // = -1 if new unconditionnal branch
                        // = 0  if the jump was done
                        // = 1  if the jump hasn't been done (like NOP)
                        // = 2  the two previous cases have been encountered
  
   short    banned;     // Well, when a jump start messing with out patience
                        // we just ban him, and the next machine states that
                        // would meet it will be happy to know that it is banned
                        // So this jump will be tolerated during a short time
   /*
   short    bannedown;  // ...commented, not useful actually
   */
   short    number;     // the number of the jump in chronological order

   // Those two variables are reset after each execution of a machine state
   int      counterup;  // how many times we got '1' for this JUMP
   int      counterdown;// how many times we got '0' for this JUMP

   // Well, used like _IdleLoops thingy
   int      j_IdleLoops;

   struct _jmp_stuff *next;
   } jmp_stuff;


/* what about diffining the sturcture that is going to hold the different  */
/* "state machines". It's gonna eat a lot of memory though                */

//                   |
//                   |        initial code
//                   |
//                  [*]       here we got an unconditionnal jump
//                 . |
//               .   |        we continue after saving the state machine
//            [*]    |    
//           . |    [*] 
//          .  |   . |  
//                .  |   
//                   |

#define PREFETCH_QUEUE_SIZE   8
 
typedef struct _machine_stuff
   {
   MACHINE_STATE  ms;   // a machine state saved for later use
   RETURN_DATA    rd;   // retdata saved for later uselessness

   int            PQBuff[PREFETCH_QUEUE_SIZE]; /* Prefetch queue buffer.  */

   WORD           m_ActiveInstructions;
   WORD           m_IdleInstructions;
   WORD           m_IdleLoops;
   ULONG          m_IdleLoopLinearAddr;
   WORD           NumOfDiffInst;
   WORD           NumOfRealActiveInst;
   WORD           NumOfRealIdleInst;
   WORD           NumOfDiffLoops;

   short          number;           // rage against the machine identificator

   jmp_stuff             *curjmp;   // used for free_machine_state
   struct _machine_stuff *previous; // previous metaphysical world
   struct _machine_stuff *next;     // the future, unfortunatly metaphysical as well 

   } machine_stuff;

// definition of some usefull/useless functions

#define        getclock(begin,end)   (double)(end-begin)/CLOCKS_PER_SEC
#ifdef __UNIX__
#define        stricmp  strcasecmp
#endif
#define        IsPrefix(n,p)         !stricmp(n+strlen(n)-strlen(p),p)
#define        allocate(structure)   (structure *)malloc(sizeof(structure))

// real functions used for real code data emulation
int            free_machine_state(machine_stuff *);
int            free_almost_evrything(void);
machine_stuff  *update_machine_state(MACHINE_STATE *, RETURN_DATA *, signed char, WORD);
jmp_stuff      *get_jmp_in_list(ULONG);
int            save_hex_regions(char *, char *, int);
int            patch_filename(char *, char *);

// somewhat usefull functions to handle executed/modified sections
MINMAX         *get_new_active_section(ULONG);
MINMAX         *is_in_active_section(ULONG);
MINMAX         *get_new_exe_section(void);
MINMAX         *is_in_exe_section(ULONG);

// definitly useless functions, for debuging only
int            printdebug(char *, int);
int            mystuff_getoptions(struct cr_ini *, char *);

// abosulutly no functions down here
;
;


