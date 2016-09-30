#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "autoverv.h"

/** Calculate a CRC polynomial from the hardwired keyphrase, and call crc_init **/
polyn *crc_setup(void)
{
   polyn our_poly;
   polyn *our_table;
   PWB keyphrase;

   memset(keyphrase.db, '\0', PWB_SIZE);
   strncpy((char*)keyphrase.db, "Verifier", sizeof(keyphrase.db));

   our_poly = new_poly(&keyphrase);

   our_table = crc_init(our_poly);          /* build the crc lookup table */
   if (!our_table) {
      printf("Not enough memory for CRC table.\n");
   }

   return our_table;
}  /* end crc_setup */

