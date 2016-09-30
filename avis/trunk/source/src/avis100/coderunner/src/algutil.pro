
/** Return the length of the instruction at buf[ip], or -1 if error **/
extern int alg_inslen(unsigned char *buf, int ip);

/** Return the length of the r/m-format instruction in the buffer. **/
extern int alg_rmlen(unsigned char *ptr);
