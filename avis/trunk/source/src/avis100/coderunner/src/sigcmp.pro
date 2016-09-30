bOOlean
match_vcomplex_sig(byte *masked_signature,
                   byte *test_addr,
                   register int len);
int mycmp(byte *masked_signature,
          byte *test_addr,
          register int len,
          int *mismatched_bytes_count,
          struct sigdats *sig);
