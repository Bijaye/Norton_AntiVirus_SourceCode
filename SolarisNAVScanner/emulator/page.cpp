// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/EMULATOR/VCS/PAGE.CPv   1.21   20 Feb 1996 11:27:06   CNACHEN  $
//
// Description:
//
//  This source file contains the support routines for the PAM paging system
//  which is used during program emulation.  (Currently, the paging system
//  requires a swap file for proper operation... Just a side note! :))
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/EMULATOR/VCS/PAGE.CPv  $
// 
//    Rev 1.21   20 Feb 1996 11:27:06   CNACHEN
// Changed all LPSTRs to LPTSTRs.
// 
// 
//    Rev 1.20   16 Feb 1996 13:56:24   CNACHEN
// Once an error occurs, stop all paging.  Also changed access_page so it would
// not return an invalid page frame #.
// 
//    Rev 1.19   13 Feb 1996 12:00:26   CNACHEN
// Chnaged memset to MEMSET.
// 
//    Rev 1.18   13 Feb 1996 11:44:50   CNACHEN
// Changed lower-case mem* to upper case MEM* (memcpy, etc)
// 
//    Rev 1.17   02 Feb 1996 11:44:26   CNACHEN
// Added new dwFlags and revamped all exclusion checking...
// 
// 
//    Rev 1.16   15 Dec 1995 18:59:36   CNACHEN
// low memory can now be read in during global init so we don't need open file
// handles...
// 
//    Rev 1.15   14 Dec 1995 10:49:32   CNACHEN
// Fixed repair stuff...
// 
//    Rev 1.14   13 Dec 1995 13:41:34   CNACHEN
// Changed LONG's to longs for NLM platform!
// 
//    Rev 1.13   13 Dec 1995 11:58:08   CNACHEN
// All File and Memory functions now use #defined versions with PAM prefixes
// 
//    Rev 1.12   30 Oct 1995 13:16:02   CNACHEN
// 
//    Rev 1.11   30 Oct 1995 13:15:22   CNACHEN
// Updated paging system to use old LRU algorithm.
// 
//    Rev 1.10   26 Oct 1995 15:32:52   CNACHEN
// Fixed some paging bugs which arose because of allocation instead of swap
// file usage. In addition, encypt swap file data so we don't false ID on virus
// data...
// 
//    Rev 1.9   26 Oct 1995 14:13:56   CNACHEN
// fixed check for (HFILE)-1 from using = to ==...
// 
//    Rev 1.8   26 Oct 1995 14:09:12   CNACHEN
// Added memory swapping instead of disk swapping if NULL is passed in for
// the swap-file filename.
// 
//    Rev 1.7   19 Oct 1995 18:23:38   CNACHEN
// Initial revision... with comment header :)
// 
//************************************************************************


#include "pamapi.h"

#define             in_memory(page)     (hLocal->page_global.page_index[(WORD)page].flags & PAGE_IN_MEMORY)
#define             on_disk(page)       (hLocal->page_global.page_index[(WORD)page].flags & PAGE_ON_DISK)
#define             make_dirty(page)    hLocal->page_global.page_index[(WORD)page].flags |= PAGE_DIRTY
#define             make_clean(page)    hLocal->page_global.page_index[(WORD)page].flags &= ~PAGE_DIRTY
#define             is_dirty(page)      (hLocal->page_global.page_index[(WORD)page].flags & PAGE_DIRTY)
#define             resident_page(page) hLocal->page_global.page_index[(WORD)page].location



PAMSTATUS global_init_paging(LPTSTR szSwapFile,PAMLHANDLE hLocal)
{
    /* open/create the swap file in read/write mode */

    if (szSwapFile != NULL)
    {
        hLocal->page_global.swap_file = PAMFileOpen(szSwapFile,READ_WRITE_FILE);
        if ((HFILE)-1 == hLocal->page_global.swap_file)
        {
            hLocal->page_global.swap_file =
                PAMFileCreate(szSwapFile,READ_WRITE_FILE);
            if ((HFILE)-1 == hLocal->page_global.swap_file)
                return(PAMSTATUS_FILE_ERROR);

            PAMFileClose(hLocal->page_global.swap_file);

            hLocal->page_global.swap_file =
                PAMFileOpen(szSwapFile,READ_WRITE_FILE);
            if ((HFILE)-1 == hLocal->page_global.swap_file)
                return(PAMSTATUS_FILE_ERROR);
        }
    }
    else
    {
        // use memory allocation instead of swap file!

        hLocal->page_global.swap_file = (HFILE)-1;
    }

	return(PAMSTATUS_OK);
}

void global_close_paging(PAMLHANDLE hLocal)
{
    if (hLocal->page_global.swap_file != (HFILE)-1)
    {
        PAMFileClose(hLocal->page_global.swap_file);
    }
}


void local_close_paging(PAMLHANDLE hLocal)
{
    WORD i;

    if (hLocal->page_global.swap_file == (HFILE)-1)
    {
        // No swap file used.  Free allocated memory instead.

        for (i=0;i<MEMORY_SIZE_IN_PAGES;i++)
        {
            if (hLocal->page_global.page_index[i].flags & PAGE_ON_DISK)
            {
                PAMMemoryFree((LPVOID)
                           hLocal->page_global.page_index[i].disk_addr);
            }
        }
    }
}

void local_init_paging(PAMLHANDLE hLocal)
{
    int i;

    /* zero out the page index */

    MEMSET(hLocal->page_global.page_index,
           0,
           sizeof(hLocal->page_global.page_index));

    for (i=0;i<NUM_RESIDENT_PAGES;i++)
    {
        hLocal->page_global.page_index[i].location = i;

        hLocal->page_global.page_data[i].page_index_num = i;
        hLocal->page_global.page_data[i].last_reference = 0;
	}

	for (i=0;i<MEMORY_SIZE_IN_PAGES;i++)
        hLocal->page_global.page_index[i].disk_addr = DISK_NOT_USED;

    hLocal->page_global.last_reference = 0;
    hLocal->page_global.next_disk_addr = 0;
}


BYTE locate_swap_page(PAMLHANDLE hLocal)
{
    int         i;
    BYTE        page_num;
    DWORD       page_ref;

    page_num = 0;
    page_ref = hLocal->page_global.page_data[0].last_reference;

    for (i=1;i<NUM_RESIDENT_PAGES;i++)
    {
        if (hLocal->page_global.page_data[i].last_reference < page_ref)
        {
            page_ref = hLocal->page_global.page_data[i].last_reference;
            page_num = i;
        }
    }

    return(page_num);
}


void    page_write_byte(PAMLHANDLE hLocal,        // context info
                        BYTE  b,                  // byte to write
                        DWORD address,            // physical address in memory
                        DWORD local_start,        // 0 in COM, after header in EXE
                        DWORD local_address,      // offset from local_start (0 = local_start)
                        HFILE source_file)        // file containing original data
{
    BYTE    res_page_num;
    DWORD   page_num;

    if (hLocal->dwFlags & LOCAL_FLAG_ERROR)   // if we had an error before
        return;                               // then don't do any paging
    
    page_num = address >> SECTOR_SHIFT;

    if (in_memory(page_num))
    {
        res_page_num = resident_page(page_num);
        hLocal->page_global.page_data[res_page_num].last_reference =
            ++hLocal->page_global.last_reference;
    }
    else
    {
        res_page_num = access_page(hLocal,address,local_start,local_address,source_file);
    }

    make_dirty(hLocal->page_global.page_data[res_page_num].page_index_num);

    hLocal->page_global.page_data[res_page_num].
        data[(WORD)(address & SECTOR_MASK)] = b;

    if ((hLocal->dwFlags & (LOCAL_FLAG_REPAIR_REPAIR |
                            LOCAL_FLAG_WRITE_THRU)) ==
                            (LOCAL_FLAG_REPAIR_REPAIR |
                            LOCAL_FLAG_WRITE_THRU) &&
        source_file == hLocal->CPU.stream)
    {
        // check if write is outside the file!

        if (hLocal->CPU.ulImageSize <= local_address)
            return;

        if ((DWORD)PAMFileSeek(hLocal->hRepairFile,
                            (long)(local_start + local_address),
                       SEEK_SET) != (DWORD)(local_start + local_address))
        {
            hLocal->dwFlags |= LOCAL_FLAG_FILE_ERROR;
            return;
        }

        if (PAMFileWrite(hLocal->hRepairFile, &b, sizeof(BYTE)) != sizeof(BYTE))
        {
            hLocal->dwFlags |= LOCAL_FLAG_FILE_ERROR;
            return;
        }
    }
}


BYTE    page_read_byte(PAMLHANDLE hLocal,        // context info
                       DWORD address,            // physical address in memory
                       DWORD local_start,        // 0 in COM, after header in EXE
                       DWORD local_address,      // offset from local_start (0 = local_start)
                       HFILE source_file)        // file containing original data
{
    BYTE            res_page_num;
    DWORD           page_num;

    if (hLocal->dwFlags & LOCAL_FLAG_ERROR)   // if we had an error before
        return(0);                            // then don't do any paging

	page_num = address >> SECTOR_SHIFT;

	// make sure to set the exec bit in the page table if we're executed
	// code in this page..

    if (hLocal->page_global.executable_fetch)
        hLocal->page_global.page_index[(WORD)page_num].flags |= PAGE_EXEC;

    if (in_memory(page_num))
    {
        res_page_num = resident_page(page_num);
        hLocal->page_global.page_data[res_page_num].last_reference =
            ++hLocal->page_global.last_reference;
    }
    else
    {
        res_page_num = access_page(hLocal,
                                   address,
                                   local_start,
                                   local_address,
                                   source_file);
    }

    return(hLocal->page_global.page_data[res_page_num].
            data[(WORD)(address & SECTOR_MASK)]);
}


PAMSTATUS page_read_sector(PAMLHANDLE hLocal,    // context information
                           DWORD address,        // physical address in memory
                           DWORD local_start,    // 0 in COM, after header in EXE
                           DWORD local_address,  // offset from local_start (0 = local_start)
                           HFILE source_file,    // file containing original data
                           BYTE *buffer)         // buffer where to put page
{
    BYTE            res_page_num;
    DWORD           page_num;

    if (hLocal->dwFlags & LOCAL_FLAG_ERROR)   // if we had an error before
        return(0);                            // then don't do any paging

    page_num = address >> SECTOR_SHIFT;

    if (in_memory(page_num))
        res_page_num = resident_page(page_num);
    else
        res_page_num = access_page(hLocal,
                                   address,
                                   local_start,
                                   local_address,
                                   source_file);

    MEMCPY(buffer,
           hLocal->page_global.page_data[res_page_num].data,
           SECTOR_SIZE);

    /* error will be set by access_page */

    if (hLocal->dwFlags & LOCAL_FLAG_FILE_ERROR)
        return(PAMSTATUS_FILE_ERROR);

    if (hLocal->dwFlags & LOCAL_FLAG_MEM_ERROR)
        return(PAMSTATUS_MEM_ERROR);

    return(PAMSTATUS_OK);
}


// remember to optimize this later!

void reset_page(BYTE *buffer, WORD fill_word)
{
    WORD    *ptr;
    int     i;

    ptr = (WORD *)buffer;
    for (i=0;i<SECTOR_SIZE/2;i++,ptr++)
        *ptr = fill_word;

}

// encrypt all pages written to disk

void encrypt_page(BYTE *buffer)
{
    int i;

    for (i=0;i<SECTOR_SIZE;i++)
        buffer[i] ^= 0xFF;
}


BYTE    access_page(PAMLHANDLE hLocal,        // context info
                    DWORD address,            // physical address in memory
                    DWORD local_start,        // 0 in COM, after header in EXE
                    DWORD local_address,      // offset from local_start (0 =
                                              // local_start)
                    HFILE source_file)        // file containing original data
{
    DWORD   page_num, res_swap_page;
	BYTE    swap_page;

    if (hLocal->dwFlags & LOCAL_FLAG_ERROR)   // if we had an error before
        return(0);                            // then don't do any paging

	page_num = address >> SECTOR_SHIFT;

	// need to swap a page out and load a new one
    // swap_page = 0 thru NUM_RESIDENT_PAGES - 1 (resident page to swap out)
    // res_swap_page = 0 thru MEMORY_SIZE_IN_PAGES - 1 (index to page table
    // for the page to be swapped out)

    swap_page = locate_swap_page(hLocal);

    res_swap_page = hLocal->page_global.page_data[swap_page].page_index_num;

    if (is_dirty(res_swap_page))
	{
        DWORD ulOffset;

        // need to save page to swap file before loading in new page



        if (hLocal->page_global.swap_file != (HFILE)-1)
        {
            if (hLocal->page_global.page_index[(WORD)res_swap_page].disk_addr ==
                DISK_NOT_USED)
                hLocal->page_global.page_index[(WORD)res_swap_page].disk_addr =
                    hLocal->page_global.next_disk_addr++;

            ulOffset = (DWORD)hLocal->page_global.
                        page_index[(WORD)res_swap_page].disk_addr << SECTOR_SHIFT;

            if ((DWORD)PAMFileSeek(hLocal->page_global.swap_file,
                                ulOffset,
                                SEEK_SET) != (DWORD)ulOffset)
            {
                hLocal->dwFlags |= LOCAL_FLAG_FILE_ERROR;
                return(swap_page);
            }

            // encrypt the page before we write to disk...

            encrypt_page(hLocal->page_global.page_data[swap_page].data);

            if (PAMFileWrite(hLocal->page_global.swap_file,
                            hLocal->page_global.page_data[swap_page].data,
                            SECTOR_SIZE) != SECTOR_SIZE)
            {
                hLocal->dwFlags |= LOCAL_FLAG_FILE_ERROR;
                return(swap_page);
            }
        }
        else
        {
            if (hLocal->page_global.page_index[(WORD)res_swap_page].disk_addr ==
                DISK_NOT_USED)
            {
                LPVOID lpvPtr;

                lpvPtr = PAMMemoryAlloc(SECTOR_SIZE);

                if (NULL == lpvPtr)
                {
                    hLocal->dwFlags |= LOCAL_FLAG_MEM_ERROR;
                    return(swap_page);
                }

                hLocal->page_global.page_index[(WORD)res_swap_page].disk_addr =
                        (DWORD)lpvPtr;
            }

            // copy our page into alloc'd buffer

            MEMCPY((LPVOID)hLocal->page_global.
                            page_index[(WORD)res_swap_page].disk_addr,
                   hLocal->page_global.page_data[swap_page].data,
                   SECTOR_SIZE);
        }

        // the page from memory is now on disk and is no longer in memory
        // PAGE_ON_DISK is always on once the page has been written to disk

        hLocal->page_global.page_index[(WORD)res_swap_page].flags |=
			PAGE_ON_DISK;
	}

	// make sure that the page index table knows that the sector is no
	// longer in memory

    hLocal->page_global.page_index[(WORD)res_swap_page].flags &=
            ~PAGE_IN_MEMORY;

    // place CD 26 in all bytes of the page (in case they are outside the file)
    // this will terminate most decryptors since INT is usually invalid and
    // INT 26 is definitely not allowed

    reset_page(hLocal->page_global.page_data[swap_page].data,
               hLocal->hGPAM->config_info.wFillWord);

	if (on_disk(page_num))              // in the swap file
	{
        DWORD ulOffset;

        if (hLocal->page_global.swap_file != (HFILE)-1)
        {
            ulOffset = (DWORD)hLocal->page_global.
                                page_index[(WORD)page_num].disk_addr <<
                                        SECTOR_SHIFT;

            if ((DWORD)PAMFileSeek(hLocal->page_global.swap_file,
                                ulOffset,
                                SEEK_SET) != (DWORD)ulOffset)
            {
                hLocal->dwFlags |= LOCAL_FLAG_FILE_ERROR;
                return(swap_page);
            }

			if (PAMFileRead(hLocal->page_global.swap_file,
                           hLocal->page_global.page_data[swap_page].data,
                           SECTOR_SIZE) != SECTOR_SIZE)
            {
                hLocal->dwFlags |= LOCAL_FLAG_FILE_ERROR;
                return(swap_page);
            }

            // decrypt the page after we read from disk...

            encrypt_page(hLocal->page_global.page_data[swap_page].data);

        }
        else
		{
			MEMCPY(hLocal->page_global.page_data[swap_page].data,
				   (LPVOID)hLocal->page_global.
							page_index[(WORD)page_num].disk_addr,
							SECTOR_SIZE);
		}
	}
    else if ((HFILE)-1 != source_file)
	{
		long lBytesToRead;

		// in this case, we have not yet loaded the sector from the source
		// file (or low memory)



		if ((DWORD)PAMFileSeek(source_file,
							local_start + (local_address & ADDRESS_MASK),
							SEEK_SET) !=
							(DWORD)(local_start + (local_address & ADDRESS_MASK)))
		{
            hLocal->dwFlags |= LOCAL_FLAG_FILE_ERROR;
            return(swap_page);
		}

		if (hLocal->CPU.stream == source_file)
			lBytesToRead =
				(long)hLocal->CPU.ulImageSize - (local_address & ADDRESS_MASK);
		else
			lBytesToRead = SECTOR_SIZE;		// dealing with low memory

		if (lBytesToRead > 0)
		{
			lBytesToRead =
				(lBytesToRead > SECTOR_SIZE) ? SECTOR_SIZE : lBytesToRead;

			if (PAMFileRead(source_file,
						 hLocal->page_global.page_data[swap_page].data,
						 (WORD)lBytesToRead) != lBytesToRead)
			{
                hLocal->dwFlags |= LOCAL_FLAG_FILE_ERROR;
                return(swap_page);
			}
		}

		// if we read past the end of the file, no problem, our buffer just
		// has 0's in it!
	}
#ifdef LOW_IN_RAM
    else
    {
        MEMCPY(hLocal->page_global.page_data[swap_page].data,
               &hLocal->hGPAM->low_mem_area
                [(WORD)(local_address & ADDRESS_MASK)],
               SECTOR_SIZE);
    }
#endif

	// the following 8 lines of code are duplicated above!

	hLocal->page_global.page_data[swap_page].page_index_num = page_num;
	hLocal->page_global.page_index[(WORD)page_num].location = swap_page;
	hLocal->page_global.page_index[(WORD)page_num].flags |= PAGE_IN_MEMORY;
	hLocal->page_global.page_data[swap_page].last_reference =
		++hLocal->page_global.last_reference;

	return(swap_page);
}

// this can only be used right after local paging init!
// it's used to insert an already read buffer into the paging system

void insert_resident_page(PAMLHANDLE hLocal, DWORD address, BYTE *sector)
{
    DWORD index;

    index = address >> SECTOR_SHIFT;

    hLocal->page_global.page_index[(WORD)index].flags = PAGE_IN_MEMORY;

	// place in the first page

    hLocal->page_global.page_index[(WORD)index].location  = 0;


    MEMCPY(hLocal->page_global.page_data[0].data,sector,SECTOR_SIZE);

    hLocal->page_global.page_data[0].page_index_num = index;
    hLocal->page_global.page_data[0].last_reference =
        ++hLocal->page_global.last_reference;
}
