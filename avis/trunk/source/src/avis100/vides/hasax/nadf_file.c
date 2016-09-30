/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ Management of the NADF files /)
 :*
 :* NAME          NADF_file
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     creat_NADF
 :* REMARKS       (/ creation of a NADF file                            /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     open_NADF
 :* REMARKS       (/ opening of an existing NADF file                   /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     read_NADF
 :* REMARKS       (/ reading of a NADF record                           /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     write_NADF
 :* REMARKS       (/ writing of a NADF record                           /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     close_NADF
 :* REMARKS       (/ closing of a NADF file                             /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     copy_bytes
 :* REMARKS       (/ copy bytes /)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1992-03-09
 :*   AUTHOR      (/ Van Meerbeck Eric        , SWN46 /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION NADF_file .
 :******************************************************************
*/
#include <stdlib.h> 
#include "NADF_flag.h"        /* file containing the NADF flags    */
#include "SATX_error.h"       /* file containing the error symbols */
#include "SATX_sys.h"         /* definition of system calls */

#ifdef WIN32
#include <sys/stat.h>
#include <io.h>
#endif

#define ALIGN_REC   4         /* alignment number */
#define LENGTH_SIZ  4         /* size of the NADF length field */
 
static char *buff_add[MAX_FILES];     /* beginning of the buffers */
static char *buff_end[MAX_FILES];     /* end of the buffers */
static char *buff_ptr[MAX_FILES];     /* current position in the buffers */
static short open_mode[MAX_FILES];    /* flag indicating the open mode  */
                                      /* 'WRITE' or 'READ'              */
static int buff_len[MAX_FILES];       /* real size of the buffers */
static int flush_len[MAX_FILES];      /* flush size to synchonize the */
                                      /* writing of the buffers       */
struct
    {
    int len;
    char val[12];
    } TypeNADF= {15,"__NADF__1|\0"};


/* update by Abdelaziz Mounji Fundp */
enum errortype {fatal, lexical, syntax, semantics};  
/* end update */
 
/*
 :******************************************************************
 :* UNIT-BODY NADF_file.creat_NADF
 :*
 :* TITLE         (/ creation of a NADF file /)
 :*
 :* PURPOSE       creation of a NADF file called 'filename'.
 :*               This file is open in write mode.
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-03-09
 :*   AUTHOR      (/ Van Meerbeck Eric        , SWN46 /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/ - try to open the NADF file
 :*                 - if the open succeeded
 :*                     - closing of the file
 :*                     - return ESFILEEX
 :*
 :*                 - else
 :*                     - try to create the file
 :*                     - if the creation succeeded and the maximum of open
 :*                       files is not reached
 :*                         - a memory area is requested for the output
 :*                           buffer
 :*                         - if the memory allocation failed
 :*                             - return ESMALLOC
 :*
 :*                         - initialization of the buffer information
 :*                         - open_mode = 'WRITE'
 :*                         - writing the type_NADF record
 :*                         - if the return code < 0
 :*                             - return the error code
 :*
 :*                         - return the file descriptor
 :*
 :*                     - else
 :*                         - if the maximum of open files is reached
 :*                             - closing and erasing of the file
 :*                             - return ESOVERFLOW
 :*
 :*                         - return ESBADCREAT
 :*
 :*                                                               /)
 :*   DATA        NONE                                      END-DATA
 :*
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/ SAT_X design: common routines /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   IN    filename      (/ name of the NADF file to be created /)
 :*                                                               INOUT
 :*
 :*   OUT   NONE
 :*
 :*   RETURNS  "int"      (/ if the value is >=0, it is the file descriptor
 :*                          else it is the error code /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY NADF_file.creat_NADF .
 :******************************************************************
*/
creat_NADF(filename)
char *filename;
    {
    int fd;                  /* file descriptor */
    int rc;                  /* return code of 'write_NADF' */
    int buffLen = BUF_SIZE;  /* size of the buffer */
    char *buffAdd;           /* pointer to the memory area */
    char cmd[MAXPATHLEN+4];  /* command line to erase the file */
    
#ifdef WIN32
	fd = open(filename, O_RDONLY | O_BINARY);
#else
    fd = open(filename, O_RDONLY);  /* Checking of the existence of */
                                     /* the file                     */
#endif 
    if (fd >= 0)                    /* If the file already exists   */
        {
        close(fd);
        return(ESFILEEX);              /* error : the file exists */
        }
    else
        {
#ifdef WIN32
        /* for zarkon's sakes, what's this ??? */
        fd = open(filename, 
                _O_CREAT | _O_TRUNC | _O_RDWR | _O_BINARY, 
                _S_IREAD | _S_IWRITE);
#else
        fd = creat(filename, PROT_NADF); /* 'PROT_NADF' is the protection */
                                         /* of the NADF file (by default) */
#endif
 
        if ((fd >= 0) && (fd < MAX_FILES)) /* If no problem during 'create'*/
            {
            /* Try to allocate a dynamic area as great as possible */
 
            buffAdd = NULL;   /* no memory area */
 
            while ((buffAdd == NULL) && (buffLen >= BUFSIZ))
              {
              buffAdd = malloc(buffLen);   /* Ask for a dynamic area */
 
              if (buffAdd == NULL)          /* Allocation problem */
                  {
                  /* decrease the size of the buffer required */
                  buffLen = buffLen - BUFSIZ;
                  }
              }
            if (buffAdd == NULL)
                {
                return(ESMALLOC);         /* Allocation error */
                }
            open_mode[fd] = WRITE;      /* to store the open mode */
 
            /* storage of the buffer information */
            buff_add[fd] = buffAdd;
            buff_end[fd] = buffAdd+buffLen;
            buff_ptr[fd] = buffAdd;
            buff_len[fd] = buffLen;
 
            rc = write_NADF(fd, &TypeNADF, FLUSH);
                                       /* 'TypeNADF' is a NADF record */
                                       /* indicating the NADF format  */
            if (rc < 0)
                {
                return(rc);     /* error during the 'write_NADF' */
                }
 
            return(fd);                /* returning file descriptor */
            }
        else
            {
            /* if the maximum of open files is reached */
            if (fd >= MAX_FILES)
                {
                /* closing of the file */
                close(fd);
 
                /* erase the file */
#if 0 /* riad: man.... this sucks down here... */
                sprintf(cmd, "rm %s", filename);
                system(cmd);*/
#else
                remove(filename);
#endif
                return(ESOVERFLOW);  /* error: limit of open files */
                }
 
            return(ESBADCREAT);    /* error: no create executed */
            }
        }
    }
 
/*
 :******************************************************************
 :* UNIT-BODY NADF_file.open_NADF
 :*
 :* TITLE         (/ opening of an existing NADF file /)
 :*
 :* PURPOSE       opening of the NADF file called 'filename'.
 :*               This file is open in read mode.
 :*               If the file is not in a NADF form, an error is returned.
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-03-09
 :*   AUTHOR      (/ Van Meerbeck Eric        , SWN46 /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/ - try to open the NADF file in 'READ_ONLY' access
 :*                 - if the open succeeded and the maximum of open file
 :*                   is not reached ( < MAX_FILES )
 :*                     - a memory area is requested for the input
 :*                       buffer
 :*                     - if the memory allocation failed
 :*                         - return ESMALLOC
 :*
 :*                     - filling the input buffer by reading the NADF file
 :*                     - if the return code < 0
 :*                         - return ESBADREAD
 :*
 :*                     - if the type_NADF record is the first of the file
 :*                         - initialization of the buffer information
 :*                         - open_mode = 'READ'
 :*                         - return the file descriptor
 :*
 :*                     - else
 :*                         - release the buffer area
 :*                         - closing the file
 :*                         - return ESBADFORM
 :*
 :*                 - else
 :*                     - if the maximum of open files is reached
 :*                         - closing and erasing of the file
 :*                         - return ESOVERFLOW
 :*
 :*                     - return ESBADOPEN
 :*
 :*                                                               /)
 :*   DATA        NONE                                      END-DATA
 :*
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/ SAT_X design: common routines /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   IN    filename      (/ <description> /)                 INOUT
 :*
 :*   RETURNS  "int"      (/ if the value is >=0, it is the file descriptor
 :*                          else it is the error code /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY NADF_file.open_NADF .
 :******************************************************************
*/
open_NADF(filename)
char *filename;
    {
    int  fd;                   /* file descriptor */
    int  i;                    /* loop counter */
    int  buffLen = BUF_SIZE;   /* Length of the buffer */
    int  nb_read = 0;          /* number of read bytes */
    int  len_rec;              /* length of the current record  */
    int  *rec_len;             /* pointer to the field 'length' of the */
                               /* records                              */
    char *buffAdd;             /* pointer to the memory area */
    char *buffPtr;             /* pointer to the current position */
    
#ifdef WIN32
	fd = open(filename,O_RDONLY | O_BINARY);
#else
    fd = open(filename,O_RDONLY);
#endif
    if ((fd >= 0) && (fd < MAX_FILES))  /* if the open succeeded */
        {
        /* Try to allocate a dynamic area as great as possible */
 
        buffAdd = NULL;   /* no memory area pointed by this variable */
 
        while ((buffAdd == NULL) && (buffLen > BUFSIZ))
          {
          buffAdd = malloc(buffLen);    /* Ask for a dynamic area */
 
          if (buffAdd == NULL)          /* Allocation problem */
              {
              /* decrease the size of the buffer required */
              buffLen = buffLen - BUFSIZ;
              }
          else
              {
              if ((nb_read = read(fd, buffAdd, buffLen)) < 0)
                  {
                  free(buffAdd);        /* release the memory area */
                  return(ESBADREAD);    /* error of reading */
                  }
              }
          }
        if (buffAdd == NULL)
            {
            return(ESMALLOC);         /* Allocation error */
            }
 
        /* assignation of the length */
        rec_len = (int *) buffAdd;
        len_rec = *rec_len;
 
        /* check if the first record is 'TypeNADF' */
 
        /* if the size is the same */
        if ((len_rec == TypeNADF.len) && (nb_read > sizeof(int)))
            {
            rec_len++;          /* skip the field 'length' */
            buffPtr = (char *) rec_len;
 
            /* the file is a NADF */
            if (strcmp(buffPtr, TypeNADF.val) == 0)
                {
                /* Initialization of the buffer environment */
                buff_add[fd] = buffAdd;
                buff_ptr[fd] = buffAdd+len_rec;
                buff_len[fd] = buffLen;
                buff_end[fd] = buffAdd+nb_read;
 
                /* alignment */
                for(i=0; i<((len_rec*(ALIGN_REC-1))%ALIGN_REC); i++)
                    {
                    buff_ptr[fd]++;
                    }
 
                /* the open mode is 'READ' */
                open_mode[fd] = READ;
 
                return(fd);    /* the file descriptor is returned */
                }
            }
        free(buffAdd);       /* release the memory area */
        close(fd);           /* close the file       */
        return(ESBADFORM);   /* error: not NADF format */
        }
    else
        {
        if (fd >= MAX_FILES)
            {
            close(fd);
            return(ESOVERFLOW); /* error the limit of open files reached */
            }
 
        return(ESBADOPEN);   /* error: no open executed */
        }
    }
 
/*
 :******************************************************************
 :* UNIT-BODY NADF_file.read_NADF
 :*
 :* TITLE         (/ reading of a NADF record         /)
 :*
 :* PURPOSE       reading of a NADF record in the buffer corresponding
 :*               to the file.
 :*               If the record is not completely contained in the buffer,
 :*               the following part of the file is loaded.
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-03-09
 :*   AUTHOR      (/ Van Meerbeck Eric        , SWN46 /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/ - if the file descriptor is valid
 :*                     - if the open_mode is 'READ'
 :*                         - if the buffer is empty
 :*                             - filling of the buffer by reading
 :*                             - if return code < 0
 :*                                     - return ESBADREAD
 :*                             - if return code == 0
 :*                                     - return ESEOF (Normal EOF)
 :*                             - the buffer information is updated
 :*
 :*                         - if the field length is not completely
 :*                           in the buffer
 :*                             - move the remainder of the buffer
 :*                               to the beginning
 :*                             - reading of the following of the file
 :*                             - if an error occurred or the read bytes
 :*                               is insufficient to complete the length
 :*                                 - return ESBADREAD
 :*
 :*                         - if the size of the record is > size of the
 :*                           buffer that can be filled
 :*                             - a new sufficient memory area is requested
 :*                             - if the memory allocation failed
 :*                                 - return ESMALLOC
 :*
 :*                             - else
 :*                                 - moving the remainder of the old buffer
 :*                                   to the beginning of the new
 :*                                 - release the old buffer
 :*                                 - reset the buffer information
 :*                                 - reading of the following of the file
 :*                                   and filling the new buffer
 :*                                 - if return code < 1
 :*                                     - return ESBADREAD
 :*
 :*                         - if the size of the record is > size of the
 :*                           filled buffer
 :*                                 - moving the remainder of the buffer
 :*                                   to the beginning
 :*                                 - reset the buffer information
 :*                                 - reading of the following of the file
 :*                                   and filling the new buffer
 :*                                 - if return code < 1
 :*                                     - return ESBADREAD
 :*
 :*                         - assign the output pointer
 :*                         - return success
 :*
 :*                     - else
 :*                         - if the open_mode is 'WRITE'
 :*                             - return ESBADMODE
 :*
 :*                 - return ESBADFID
 :*                                                               /)
 :*   DATA        NONE                                      END-DATA
 :*
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/ SAT_X design: common routines /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   IN    fd            (/ file descriptor of a NADF file open with
 :*                        "open_NADF" <description> /)        INOUT,
 :*
 :*   OUT    rec_add       (/ address of the NADF record /)     INOUT
 :*
 :*   RETURNS  "int"      (/ if the value is >=0, it signals the success
 :*                          of the operation else it is the error code /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY NADF_file.read_NADF .
 :******************************************************************
*/
read_NADF(fd, rec_add)
int fd;
char **rec_add;
    {
    char *malloc_add;  /* address of the dynamic memory allocation */
    int  i;            /* loop counter */
    int  malloc_len;   /* size of the dynamic memory allocation    */
    int  nb;           /* number of bytes remaining in the buffer  */
    int  nb_read;      /* number of read bytes */
    int  *rec_len;     /* length of the current record */
    int  rest;         /* number of bytes that can be read  */
    

 
    if ((fd > -1) && (fd < MAX_FILES))   /* fd is in the valid range */
        {
 
        if (open_mode[fd] == READ)    /* fd is a file descriptor of   */
                                      /* a file open with 'open_NADF' */
            {
            rec_len = (int *) buff_ptr[fd];
 
            /* CASE buffer is empty */
 
            if (buff_ptr[fd] >= buff_end[fd])
                {
                nb_read = read(fd, buff_add[fd], buff_len[fd]);
                if ( nb_read < 0)
                    {
                    return(ESBADREAD);      /* error during the reading */
                    }
                else
                    {
                    if (nb_read == 0)
                        {
                        return(ESEOF);      /* normal end of file */
                        }
                    rec_len      = (int *) buff_add[fd];
                    buff_ptr[fd] = buff_add[fd];
                    buff_end[fd] = buff_ptr[fd]+nb_read;
                    }
                }
            /* number of bytes in the rest of the filled part of the   */
            /* buffer                                                  */
            nb = buff_end[fd] - buff_ptr[fd];
 
 
            /* CASE the field length is not completely read */
 
            if (nb < sizeof(*rec_len))
               {
               /* copy from, to, number_of_bytes */
               copy_bytes(buff_ptr[fd], buff_add[fd], nb);
 
               rec_len      = (int *) buff_add[fd];
               buff_ptr[fd] = buff_add[fd];
               nb_read      = read(fd, buff_add[fd]+nb, buff_len[fd]-BUFSIZ);
               if (nb_read+nb < sizeof(*rec_len))
                   {
                   return(ESBADREAD);      /* EOF or error reading */
                   }
               buff_end[fd] = buff_ptr[fd] + nb_read + nb;
               nb = nb + nb_read - *rec_len;
               }
 
            /* CASE the length of the record is higher than the size */
            /*      of the buffer that can be filled                 */
 
            if (*rec_len > (buff_len[fd]-BUFSIZ+ (nb%BUFSIZ)))
                         /* the '%' is the modulo operator */
                {
                fprintf(stderr, "error number  43");
                malloc_len = BUFSIZ*((*rec_len/BUFSIZ)+1);
                malloc_add = malloc(malloc_len);
 
                if (malloc_add == NULL)
                    {
                    return(ESMALLOC);       /* Memory alloc error */
                    }
                else
                    {
                    /* copy from, to, number_of_bytes */
                    copy_bytes(buff_ptr[fd], malloc_add, nb);
                    free(buff_add[fd]);
                    buff_add[fd] = malloc_add;
                    buff_len[fd] = malloc_len;
                    buff_ptr[fd] = malloc_add;
                    rec_len      = (int *) malloc_add;
                    rest = ((buff_len[fd] - nb)/BUFSIZ) * BUFSIZ;
                    nb_read = read(fd, buff_add[fd]+nb, rest);
                    if (nb_read < 1)
                        {
                        return(ESBADREAD);      /* EOF or error reading */
                        }
                    buff_end[fd]  = buff_ptr[fd] + nb_read + nb;
                    }
                }
 
            /* CASE the length of the record is higher than the size */
            /*      of the filled rest of the buffer                 */
 
            if (*rec_len > nb)
                {
                /* copy from, to, number_of_bytes */
                copy_bytes(buff_ptr[fd], buff_add[fd], nb);
		rec_len = (int *) buff_add[fd];              /* isa */
                buff_ptr[fd] = buff_add[fd];
                rest = ((buff_len[fd] - nb)/BUFSIZ) * BUFSIZ;
                nb_read = read(fd, buff_add[fd]+nb, rest);
                if (nb_read+nb < *rec_len)
                    {
                    return(ESBADREAD);      /* EOF or error reading */
                    }
                buff_end[fd] = buff_ptr[fd] + nb_read + nb;
                }
            *rec_add  = buff_ptr[fd];
            rec_len   = (int *) *rec_add;
 
            /* skip the current record */
            buff_ptr[fd] = buff_ptr[fd] + *rec_len;
 
            /* alignment */
            for(i=0; i<(((*rec_len)*(ALIGN_REC-1))%ALIGN_REC); i++)
                {
                buff_ptr[fd]++;
                }
 
            return(0);                      /* Success  */
            }
 
        if (open_mode[fd] == WRITE)    /* fd is a file descriptor of    */
                                       /* a file open with 'creat_NADF' */
            {
            return(ESBADMODE);          /* error : the file is not open in */
                                        /*         the 'read' mode         */
            }
        }
    return(ESBADFID);           /* the file descriptor is invalid */
    }
 
/*
 :******************************************************************
 :* UNIT-BODY NADF_file.write_NADF
 :*
 :* TITLE         (/ writing of a NADF record /)
 :*
 :* PURPOSE       writing of a NADF record in the buffer corresponding
 :*               to the file.
 :*               When the buffer is full, it is really written.
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-03-10
 :*   AUTHOR      (/ Van Meerbeck Eric        , SWN46 /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL     (/ <descript. of control flow (none -> no CONTROL>
 :*                                                               /)
 :*   DATA        NONE                                      END-DATA
 :*
 :*   CONTROL    (/ - if the file descriptor is valid
 :*                     - if the open_mode is 'WRITE'
 :*                         - if the pointer to the record is 'NULL'
 :*                             - return ESNORECORD
 :*
 :*                         - While the length of the remainder of the record
 :*                           to be written > free space of the buffer
 :*                             - filling of the buffer
 :*                             - writing the buffer
 :*                             - if return code != size of the buffer
 :*                                 - return ESBADWRITE
 :*
 :*                             - reset the buffer information
 :*
 :*                         - the remainder of the record is copied into
 :*                           the buffer
 :*                         - shifting of the current position to respect
 :*                           the alignment
 :*                         - if flag == FLUSH
 :*                             - writing of the buffer
 :*                             - if return code != size to be written
 :*                                 - return ESBADWRITE
 :*                         - reset the buffer information
 :*
 :*                     - else
 :*                         - if the open_mode is 'READ'
 :*                             - return ESBADMODE
 :*
 :*                 - return ESBADFID
 :*                                                               /)
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/ SAT_X design: common routines /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   IN    fd            (/ file descriptor of a NADF file open with
 :*                        "creat_NADF"  /)                    INOUT,
 :*
 :*         rec_add       (/ address of the NADF record to be written /)
 :*                                                            INOUT,
 :*
 :*         flush         (/ flag indicating if the buffer must be flushed /)
 :*                                                            INOUT
 :*
 :*   RETURNS  "int"      (/ if the value is >=0, it signals the success
 :*                          of the operation else it is the error code /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY NADF_file.write_NADF .
 :******************************************************************
*/
write_NADF(fd, rec_add, flush)
int fd;
char *rec_add;
short flush;
    {
    int  i;         /* loop counter */
    int  nb;        /* number of bytes remaining in the buffer */
    int  nb_to_wrt; /* number of bytes to be written */
    int  nb_trans;  /* number of bytes written effectively */
    int  *rec_len;  /* length of the record specified in the first field */
    int  len_rec;   /* length of the record specified in the first field */
    char *rec_ptr;  /* address of the current position in the record */
 
    if ((fd > -1) && (fd < MAX_FILES))   /* fd is in the valid range */
        {
 
        if (open_mode[fd] == WRITE)
            {
            if (rec_add == NULL)
                {
                return(ESNORECORD);  /* no record to be written */
                }
            rec_ptr = rec_add;    /* current position = beginning */
            rec_len = (int *) rec_add;
            len_rec = *rec_len;
            /* To treat the records with a length > free space in buffer */
            while (len_rec > (buff_end[fd] - buff_ptr[fd] - flush_len[fd]))
              {
              nb = buff_end[fd] - buff_ptr[fd];
              copy_bytes(rec_ptr, buff_ptr[fd], nb);
              nb_trans = write(fd, buff_add[fd], buff_len[fd]);
              if (nb_trans != buff_len[fd])
                  {
                  return(ESBADWRITE);   /* Writing error */
                  }
              buff_ptr[fd] = buff_add[fd];
              rec_ptr = rec_ptr + nb;
              len_rec = len_rec - nb;
              flush_len[fd] = 0;
              }
 
            /* CASE length of the record <= free space in the buffer */
            copy_bytes(rec_ptr, buff_ptr[fd], len_rec);
            buff_ptr[fd] = buff_ptr[fd] + len_rec;
 
            /* alignment */
            for(i=0; i<((len_rec*(ALIGN_REC-1))%ALIGN_REC); i++)
                {
                *buff_ptr[fd]='\0';
                buff_ptr[fd]++;
                }
 
 
            /* CASE flush required */
            if (flush == FLUSH)
                {
                /* flush */
                nb_to_wrt = buff_ptr[fd]-buff_add[fd];
                nb_trans  = write(fd, buff_add[fd], nb_to_wrt);
 
                if (nb_trans != nb_to_wrt)
                    {
                    return(ESBADWRITE);   /* Writing error */
                    }
                flush_len[fd] = (buff_ptr[fd]-buff_add[fd]) % BUFSIZ;
                             /* the '%' is the modulo operator */
                buff_ptr[fd] = buff_add[fd];
 
                }
            return(0);       /* success */
            }
        if (open_mode[fd] == READ)     /* fd is a file descriptor of    */
                                       /* a file open with 'open_NADF'  */
            {
            return(ESBADMODE);          /* error : the file is not open in */
                                        /*         the 'write' mode        */
            }
        }
    return(ESBADFID);           /* the file descriptor is invalid */
    }
 
/*
 :******************************************************************
 :* UNIT-BODY NADF_file.close_NADF
 :*
 :* TITLE         (/ closing of a NADF file /)
 :*
 :* PURPOSE       this function closes the NADF file open by 'open_NADF'
 :*               or 'creat_NADF'.
 :*               If the file is in 'WRITE' access the buffer is flushed.
 :*               The buffer is release.
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-03-10
 :*   AUTHOR      (/ Van Meerbeck Eric        , SWN46 /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/ - if the file descriptor is valid
 :*                     - if the open_mode is 'WRITE'
 :*                         - writing the buffer
 :*                         - if return code != size of the buffer
 :*                             - return ESBADWRITE
 :*
 :*                         - release the buffer area
 :*                         - closing of the file
 :*                         - reset the buffer information
 :*                         - reset the open_mode
 :*                         - return success
 :*
 :*                     - else
 :*                         - if the open_mode is 'READ'
 :*                             - release the buffer area
 :*                             - closing of the file
 :*                             - reset the buffer information
 :*                             - reset the open_mode
 :*                             - return success
 :*
 :*                 - reset the open_mode
 :*                 - return ESBADFID
 :*                                                               /)
 :*   DATA        NONE                                      END-DATA
 :*
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/ SAT_X design: common routines /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   IN    fd            (/ file descriptor of a NADF file open by
 :*                        "creat_NADF" or "open_NADF" /)      INOUT
 :*
 :*   RETURNS  "int"      (/ if the value is >=0, it signals the success
 :*                          of the operation else it is the error code /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY NADF_file.close_NADF .
 :******************************************************************
*/
close_NADF(fd)
int fd;
    {
    int  nb;        /* number of bytes to be written */
    int  nb_trans;  /* number of bytes written effectively */
 
    if ((fd >-1) && (fd < MAX_FILES))  /* if the file descriptor is valid */
        {
        if (open_mode[fd] == WRITE)   /* if the open mode is 'WRITE' */
            {
            /* flush */
            nb = buff_ptr[fd]-buff_add[fd];
            nb_trans = write(fd, buff_add[fd], nb);
 
            if (nb_trans != nb)
                {
                return(ESBADWRITE);   /* Writing error */
                }
            free(buff_add[fd]);     /* release the memory area */
            close(fd);
            open_mode[fd] = NONE;
            return(0);              /* success */
            }
        else
            {
            if (open_mode[fd] == READ)  /* if the open mode is 'READ' */
                {
                free(buff_add[fd]);   /* release the memory area */
                close(fd);
                open_mode[fd] = NONE;
                return(0);            /* success */
                }
            }
        }
    open_mode[fd] = NONE;
    return(ESBADFID); /* error: invalid 'fd' value */
    }
 
/*
 :******************************************************************
 :* UNIT-BODY NADF_file.copy_bytes
 :*
 :* TITLE         (/ copy bytes /)
 :*
 :* PURPOSE       (/ copy a sequence of bytes from one location to another /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-12-02
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA       (/
 :*                                                                /)
 :*
 :*                                                          END-DATA
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/                                          /)
 :*
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre    (/ - 'from' is a pointer to a string
 :*             - 'to' is a pointer to an allocated area  of at least 'n'
 :*               bytes in length
 :*                                                          /)
 :*                                                             END-Pre
 :*   Post   (/ - the 'n' bytes starting from 'from' are copied to the area
 :*               pointed by 'to'
 :*                                                                 /)
 :*                                                             END-Post
 :*   RETURNS   NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY NADF_file.copy_bytes
 :******************************************************************
*/
__inline static copy_bytes(from, to, n)
{
   memcpy(to, from, n);
}				

