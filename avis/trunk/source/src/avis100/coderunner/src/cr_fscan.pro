int   filerun_and_scan (
         unsigned int       scan_length,           /* Buffer size.         */
         unsigned int       ept_offset_into_buf,   /* Index of entry point into buffer. */
         char               *filename,             /* Not used by CR.      */
         unsigned short     is_exe_type,           /* Not used by CR.      */
         unsigned short     ExeFile,               /* TRUE => EXE type file*/
         CR_WORD            StartSeg,              /* Starting CS. 0x3000? */
         CR_WORD            FileHdrSize,           /* 0 for COM files.     */
         CR_ULONG           FileSize,              /* Full file size.      */
         CR_WORD           *pReloItems,            /* Exe reloc. pairs     */
         int              (*pfnReadFileRegion)(    /* Ptr to read-file fn. */
                              CR_ULONG abs_offset,
                              CR_BYTE *buffer,
                              CR_WORD bytes_to_read
                              ),
         long               base,                  /* Not used by CR.      */
         int                object_type,           /* Not used by CR.      */
         unsigned char      *buffer_to_be_scanned, /* Buffer itself.       */
         char               *vp_name,              /* Not used by CR.      */
         unsigned short     inside_compressed,     /* Not used by CR.      */
         unsigned short int initdses,              /* Init. val. of DS, ES */
         unsigned short int initcs,                /* Init. val. of CS reg */
         unsigned short int initip,                /* Init. val. of IP reg */
         unsigned short int initss,                /* Init. val. of SS reg */
         unsigned short int initsp,                /* Init. val. of SP reg */
         unsigned short     scalpel_mode,          /* Scan more buffers if */
         CR_ULONG           saved_epoint           /* ept offset into file */
         );
