// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPI/VCS/search.cpv   1.3   11 Jun 1998 14:38:44   MKEATIN  $
//
// Description:
//
//  This source file contains the PAM string searching module.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVAPI/VCS/search.cpv  $
// 
//    Rev 1.3   11 Jun 1998 14:38:44   MKEATIN
// We now use PAMFilexxx routines for ALL file operations.
// 
//    Rev 1.2   21 May 1998 20:31:08   MKEATIN
// Changed pamapi.h to pamapi_l.h
//
//    Rev 1.1   21 May 1998 19:48:04   MKEATIN
// Now using file callbacks
//
//    Rev 1.0   21 May 1998 19:23:48   MKEATIN
// Initial revision.
//
//    Rev 1.9   09 Apr 1996 12:11:40   CNACHEN
// No change.
//
//    Rev 1.8   13 Feb 1996 12:00:34   CNACHEN
// Chnaged memset to MEMSET.
//
//    Rev 1.7   13 Feb 1996 11:45:00   CNACHEN
// Changed lower-case mem* to upper case MEM* (memcpy, etc)
//
//    Rev 1.6   13 Dec 1995 11:58:10   CNACHEN
// All File and Memory functions now use #defined versions with PAM prefixes
//
//    Rev 1.5   19 Oct 1995 18:23:36   CNACHEN
// Initial revision... with comment header :)
//
//************************************************************************

#include "avendian.h"
#include "pamapi_l.h"


/* create tables function. slow but works. */

void create_tables(PAMGHANDLE pPAMG)
{
    WORD    i, j, imin, imax, *ptr, val, byte_num, bit_num;

    for (i=0;i<NUM_TOP_SETS;i++)
    {
		imin = imax = 0xFFFFU;

        MEMSET(pPAMG->table_array[i].bit_table,0,BIT_TABLE_SIZE);

		for (j=0;j<pPAMG->num_sigs;j++)
		{
			/* check to see if we've hit the start of this set's signatures */

			if (imin == 0xFFFFU && pPAMG->sig_ptr_array[j]->sig[0] ==
				pPAMG->top_set[i])
				imin = j;

			/* if we have a valid signature, place it in the bit table */

			if (pPAMG->sig_ptr_array[j]->sig[0] == pPAMG->top_set[i])
			{
				ptr = (WORD *)(pPAMG->sig_ptr_array[j]->sig+1);
				val = (*((LPBYTE)ptr) << 8) + *(((LPBYTE)ptr)+1);
				val >>= BIT_TABLE_SHIFT;

				byte_num = val >> 3;
				bit_num  = val & 7;

				pPAMG->table_array[i].bit_table[byte_num] |= (1 << bit_num);
			}

			/* record the ending of this set's signatures */

			if (imin != 0xFFFFU && imax == 0xFFFFU &&
					pPAMG->sig_ptr_array[j]->sig[0] != pPAMG->top_set[i])
			{
				imax = j-1;
				break;
			}
		}

		if (imax == 0xFFFFU)
			imax = pPAMG->num_sigs-1;

		pPAMG->table_array[i].low_index = imin;
        pPAMG->table_array[i].high_index = imax;
    }
}




WORD binary_search(PAMGHANDLE hGlobal, BYTE set, BYTE *buffer)
{
    BYTE    data[SIG_LEN];
	long	imin, imax, i;
    int     result;

    /* copy data from the buffer into our local comparison area */

    MEMCPY(data,buffer,SIG_LEN);

	/* encrypt the data so we can search for it */

	for (i=ENCRYPT_START;i<SIG_LEN;i++)
		data[(WORD)i]++;

    imin = hGlobal->table_array[set].low_index;
    imax = hGlobal->table_array[set].high_index;

    i = (imin + imax) >> 1;             /* divide by 2 */

    while (imin <= imax)
    {
        result = MEMCMP(data,
                        hGlobal->sig_ptr_array[(WORD)i]->sig,
                        hGlobal->sig_ptr_array[(WORD)i]->length);

        if (!result)
            return((WORD)hGlobal->sig_ptr_array[(WORD)i]->virus_index);

        if (result < 0)
            imax = i - 1;
        else
            imin = i + 1;

        i = (imin + imax) >> 1;
    }

    return(NOT_FOUND);
}


WORD find_sig(PAMGHANDLE hGlobal, BYTE *buffer, WORD buff_len)
{
	WORD i, val, byte_num, bit_num, result;
	BYTE set;

	/* signature must be within 512-SIG_LEN bytes in buffer */

	buff_len -= SIG_LEN;

	for (i=0;i<buff_len;i++, buffer++)
	{
		/* check to see if we have a byte in consideration for scanning */

		set = hGlobal->index_array[*buffer];
		if (set != 0xFF)
		{
			/* yes.  now check the next two bytes to see if they are OK */
			val = (((WORD)(*(buffer+1)) << 8) + *(buffer+2)) >> BIT_TABLE_SHIFT;

			byte_num = val >> 3;
			bit_num  = val & 7;

			if (hGlobal->table_array[set].bit_table[byte_num] &
					(1 << bit_num))
			{
				if (hGlobal->table_array[set].low_index != 0xFFFF)
				{
					result = binary_search(hGlobal,set,buffer);

					if (result != NOT_FOUND)
						return(result);
				}
			}
		}
	}

	return(NOT_FOUND);
}


void global_close_search(PAMGHANDLE hGlobal)
{
	WORD i;

	if (hGlobal->sig_ptr_array != NULL)
    {
        for (i=0;i<hGlobal->num_sigs;i+=CLUSTER_SIZE)
        {
            PAMMemoryFree(hGlobal->sig_ptr_array[i]);
            hGlobal->sig_ptr_array[i] = NULL;
        }

        PAMMemoryFree(hGlobal->sig_ptr_array);
    }
}


PAMSTATUS global_init_search(HFILE hFile, DWORD dwOffset, PAMGHANDLE pPAMG)
{
    WORD        i, j, wClusterSize;
    sig_type    sig;

    /* first seek to the top of the string searching data */

    if ((DWORD)PAMFileSeek(hFile,dwOffset,SEEK_SET) != dwOffset)
        return(PAMSTATUS_FILE_ERROR);

    /* next load the 16 special search bytes from the file.  they will be
       stored in the first 16 bytes of the record */

    if (PAMFileRead(hFile,pPAMG->top_set,sizeof(pPAMG->top_set)) !=
        sizeof(pPAMG->top_set))
        return(PAMSTATUS_FILE_ERROR);

    /* the index_array contains 0xFF for each byte which is not a search
       byte, or the index into the top_set array of the search_byte */

    for (i=0;i<256;i++)
        pPAMG->index_array[i] = 0xFF;

    for (i=0;i<NUM_TOP_SETS;i++)
        pPAMG->index_array[pPAMG->top_set[i]] = (BYTE)i;

    /* next we want to read all of the signatures in.  First read in the # of
       signatures */

    if (PAMFileRead(hFile,&(pPAMG->num_sigs),sizeof(pPAMG->num_sigs)) !=
        sizeof(pPAMG->num_sigs))
        return(PAMSTATUS_FILE_ERROR);

    pPAMG->num_sigs = WENDIAN(pPAMG->num_sigs);

    wClusterSize = 0;

    // allocate memory for our pointer table now...
    // this table is limited to 16K signatures. sizeof(DWORD) * 16K > 64K

    pPAMG->sig_ptr_array =
        (sig_type **)PAMMemoryAlloc(sizeof(sig_type *) * pPAMG->num_sigs);

    if (NULL == pPAMG->sig_ptr_array)
        return(PAMSTATUS_MEM_ERROR);

    for (i=0;i<pPAMG->num_sigs;i++)
    {
        if (sizeof(sig) != PAMFileRead(hFile,&sig,sizeof(sig)))
        {
            /* free anything allocated so far */

            for (j=0 ; j<i ; j += CLUSTER_SIZE )
                PAMMemoryFree(pPAMG->sig_ptr_array[j]);

            PAMMemoryFree(pPAMG->sig_ptr_array);
			pPAMG->sig_ptr_array = NULL;

			return(PAMSTATUS_FILE_ERROR);
		}

        sig.length = WENDIAN(sig.length);
        sig.virus_index = DWENDIAN(sig.virus_index);

		// check to see if we need to allocate a cluster of memory for
		// more signatures

		if (wClusterSize == 0)
		{
			wClusterSize = pPAMG->num_sigs - i;
			if (wClusterSize > CLUSTER_SIZE)
				wClusterSize = CLUSTER_SIZE;

			pPAMG->sig_ptr_array[i] =
                (sig_type *)PAMMemoryAlloc(sizeof(sig_type)*wClusterSize);

			if (NULL == pPAMG->sig_ptr_array[i])
			{
				for (j=0;j<i;j+=CLUSTER_SIZE)
                    PAMMemoryFree(pPAMG->sig_ptr_array[j]);

                PAMMemoryFree(pPAMG->sig_ptr_array);
				pPAMG->sig_ptr_array = NULL;

				return(PAMSTATUS_MEM_ERROR);
			}
		}
		else
			pPAMG->sig_ptr_array[i] = pPAMG->sig_ptr_array[i-1]+1;

		wClusterSize--;

		*(pPAMG->sig_ptr_array[i]) = sig;
    }

    /* once we have loaded all the signatures (encrypted), now create the */
    /* tables... (this can be pre-done and read in for final version) */

    create_tables(pPAMG);

    return(PAMSTATUS_OK);
}



