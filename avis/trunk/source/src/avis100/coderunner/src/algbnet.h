typedef struct s_netterm_descriptor
{
   unsigned char netterm[4];
   int netterm_len;
   int netterm_weight;
} netterm_descriptor;

netterm_descriptor term_list[] =
{
   { { 0x13^0xFF, 0x04^0xFF, 0xB1^0xFF, 0x06^0xFF }, 4, 13 },
   { { 0x8C^0xFF, 0x0E^0xFF, 0x4E^0xFF, 0x00^0xFF }, 4, 6 },
   { { 0xD3^0xFF, 0xE0^0xFF, 0x8E^0xFF, 0xC0^0xFF }, 4, 18 },
   { { 0xB4^0xFF, 0x0E^0xFF, 0xB7^0xFF, 0x00^0xFF }, 4, 15 },
   { { 0xA3^0xFF, 0x13^0xFF, 0x04^0xFF }, 3, 12 },
   { { 0xA1^0xFF, 0x13^0xFF, 0x04^0xFF }, 3, 26 },
   { { 0xBE^0xFF, 0x01^0xFF, 0xB9^0xFF }, 3, -3},
   { { 0xBF^0xFF, 0xBE^0xFF, 0x01^0xFF }, 3, 15 },
   { { 0x00^0xFF, 0x02^0xFF, 0xB9^0xFF }, 3, 10 },
   { { 0x02^0xFF, 0xB9^0xFF, 0x01^0xFF }, 3,  0},
   { { 0x8E^0xFF, 0xC0^0xFF, 0xB8^0xFF }, 3, -7},
   { { 0x00^0xFF, 0x01^0xFF, 0xCD^0xFF }, 3, -2},
   { { 0x00^0xFF, 0x02^0xFF, 0xFC^0xFF }, 3, 23 },
   { { 0x01^0xFF, 0xBA^0xFF, 0x80^0xFF }, 3,  9 },
   { { 0x01^0xFF, 0xCD^0xFF, 0x13^0xFF }, 3,  9 },
   { { 0x03^0xFF, 0x33^0xFF, 0xDB^0xFF }, 3, 3 },
   { { 0x06^0xFF, 0x4C^0xFF, 0x00^0xFF }, 3, 6 },
   { { 0x07^0xFF, 0xBB^0xFF, 0x00^0xFF }, 3, -3 },
   { { 0x80^0xFF, 0x00^0xFF, 0xCD^0xFF }, 3, 6 },
   { { 0x80^0xFF, 0xFC^0xFF, 0x02^0xFF }, 3,  8 },
   { { 0xA1^0xFF, 0x4C^0xFF, 0x00^0xFF }, 3, 3 },
   { { 0xB8^0xFF, 0x01^0xFF, 0x03^0xFF }, 3, 17 },
   { { 0xBA^0xFF, 0x00^0xFF, 0x01^0xFF }, 3,  7 },
   { { 0xCD^0xFF, 0x13^0xFF, 0x0E^0xFF }, 3, 6 },
   { { 0xD8^0xFF, 0x8E^0xFF, 0xD0^0xFF }, 3, 8 },
   { { 0xF6^0xFF, 0x8B^0xFF, 0xFE^0xFF }, 3, 8 }
};

#define NUM_TERMS (sizeof(term_list)/sizeof(netterm_descriptor))

/*
 * Squash down the times-found count for an ngram, to one of a few values.
 */
int
alg_boot_term_threshold(int count)
{
   int rv;

   switch(count)
   {
      case(0):
         rv = 0;
         break;
      case(1):
         rv = 1;
         break;
      case(2):
      default:
         rv = 2;
         break;
   }
   return rv;
}

/*
 * Like memcmp(), but the second parameter points to a region masked
 * with 0XFF.
 */
int
masked_memcmp(unsigned char *test_buf,
              unsigned char *key,
              register unsigned int len_key)
{
   register unsigned int i;

   for (i=0; i<len_key; ++i)
   {
      if ((key[i]^(unsigned char)0xFF) != test_buf[i])
         return 1;
   }
   return 0;
}

/*
 * Return the number of times that an ngram appears in the buffer.
 */
int
ngram_term_count(unsigned char *buf,
                 unsigned int buf_len,
                 unsigned char *key,
                 register unsigned int len_key)
{
   register int i;
   int cnt = 0;

   for (i=0; i<(int)(buf_len-len_key); ++i)
   {
      if (!masked_memcmp(&buf[i], key, len_key))
         ++cnt;
   }
   return cnt;
}

//===========================================================================
//
// Return the number of times that an ngram appears in the buffers.

extern bOOlean in_exclusion_list(unsigned char *buf);
int   ngram_term_count_ex (
         BUFFER                  *pBuffers,     // <-  Buffers with data
         int                     iNumBuffers,   // <-  Number of elements in pBuffers.
         unsigned char           *key,
         register unsigned int   len_key
         )
{
//---------------------------------------------------------------------------
   register int   i, j;
   int            cnt = 0;
//---------------------------------------------------------------------------

   // BOOTRUN.H:
   // typedef  struct _BUFFER
   // {
   //    unsigned char  *pBuffer;
   //    unsigned long  ulBufferSize;
   // } BUFFER;
   //

   for (i=0; i<iNumBuffers; i++)
      {
      if (pBuffers[i].ulBufferSize == 512 && in_exclusion_list (pBuffers[i].pBuffer))
         continue;
      for (j=0; j<(pBuffers[i].ulBufferSize - len_key); j++)
         {
         if (!masked_memcmp(&(pBuffers[i].pBuffer[j]), key, len_key))
            ++cnt;
         }
      }


   return cnt;
}
//===========================================================================

/*
 * For an ngram, count the number of times the term is found in the
 * buffer and run through the term squashing function.
 */
int
alg_boot_term_output(unsigned char *buf,
                     unsigned int buf_len,
                     unsigned char *key,
                     unsigned int len_key)
{
   return alg_boot_term_threshold(ngram_term_count(buf,
                                                   buf_len,
                                                   key,
                                                   len_key));
}

//===========================================================================
//
// For an ngram, count the number of times the term is found in the
// buffer and run through the term squashing function.

int   alg_boot_term_output_ex (
         BUFFER         *pBuffers,     // <-  Buffers with data
         int            iNumBuffers,   // <-  Number of elements in pBuffers.
         unsigned char  *key,
         unsigned int   len_key
         )
{
   return alg_boot_term_threshold (
            ngram_term_count_ex (
               pBuffers,
               iNumBuffers,
               key,
               len_key
               )
            );
}
//===========================================================================


/*
 * Return sum of term counts run through the term squashing function.
 */
int
alg_boot_net(unsigned char *buf, unsigned int buf_len)
{
   int i;
   int sum = 0;

   for (i=0; i<NUM_TERMS; ++i)
   {
      sum += alg_boot_term_output(buf,
                                  buf_len,
                                  term_list[i].netterm,
                                  term_list[i].netterm_len) *
             term_list[i].netterm_weight;
   }
   return sum;
}

//===========================================================================
//
// Return sum of term counts run through the term squashing function.

int   alg_boot_net_ex (
         BUFFER   *pBuffers,  // <-  Buffers with data
         int      iNumBuffers // <-  Number of elements in pBuffers.
         )
{
//---------------------------------------------------------------------------
   int            i;
   int            sum = 0;
//---------------------------------------------------------------------------

   for (i=0; i<NUM_TERMS; ++i)
   {
      sum +=
         alg_boot_term_output_ex (
            pBuffers,
            iNumBuffers,
            term_list[i].netterm,
            term_list[i].netterm_len
            ) * term_list[i].netterm_weight;
   }
   return sum;
}
//===========================================================================


#define THRESHOLD 34

/*
 * Return 1 if sum is greater than output threshold, 0 otherwise.
 */
int
alg_boot_net_output(unsigned char *buf, unsigned int buf_len)
{
   return alg_boot_net(buf, buf_len) >= THRESHOLD;
}

//===========================================================================
//
// Return 1 if sum is greater than output threshold, 0 otherwise.

int   alg_boot_net_output_ex (
         BUFFER   *pBuffers,  // <-  Buffers with data
         int      iNumBuffers // <-  Number of elements in pBuffers.
         )
{
   return (alg_boot_net_ex (pBuffers, iNumBuffers) >= THRESHOLD);
}
//===========================================================================
