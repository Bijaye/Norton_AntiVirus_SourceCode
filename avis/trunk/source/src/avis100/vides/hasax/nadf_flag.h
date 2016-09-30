 
#define BUF_SIZE   2*BUFSIZ /* default size of the buffers */
#define READ       1        /* open mode flag */
#define WRITE      2        /* open mode flag */
 
#define PROT_NADF  384      /* protection mode of the NADF files */
#ifndef MAX_FILES
#define MAX_FILES  20       /* maximal number of simultaneously open files */
#endif
 
#define FLUSH      1        /* flag indicating that the 'write' must flush */
                            /* the buffer                                  */
#define NO_FLUSH   2        /* flag indicating that the 'write' must not   */
                            /* flush the buffer                            */
 
#define NONE       0        /* default */
