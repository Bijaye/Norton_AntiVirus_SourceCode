extern int algdet_verbose;
extern int alg_scan(char *testfilename, char *rootfilename, unsigned char *buf,
                    char *oldbuf, unsigned long oblen, int ob_wholefile,
                    int inside_compressed, unsigned char saved_head[],
                    unsigned int saved_head_len, unsigned long filelen,
                    unsigned long saved_epoint);
extern int
ReadFileRegion(unsigned long abs_offset,
               unsigned char *buffer,
               unsigned short int  bytes_to_read);
