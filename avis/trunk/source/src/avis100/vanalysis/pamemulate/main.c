#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <process.h>
#include <ctype.h>
#include <time.h>
//#include <alloc.h>

#include "pamapi.h"
#include "callfake.h"
#include "analysis.h"

extern unsigned _stklen = 16384U;
DWORD       dwNumViruses=0, dwTotalFiles=0;

char start_path[128];
PAMLHANDLE hgLocal;
PAMGHANDLE hgGlobal;
DWORD dwGSIter;

DWORD gdwPagesRead;

char    root[MAXDIR], initial_dir[MAXDIR], master_dat[MAXDIR];

/* BUILD_SET DEFINE! */

void do_file(char *filename, char *pathname, PAMLHANDLE hLocal)
{
    WORD        wVirusID, sys_flag;
    PAMSTATUS   pamStatus;
    HFILE       hFile;
    
    strupr(filename);
#ifdef BUILD_SET
    if (!strstr(filename,".EXE") && !strstr(filename,".COM") &&
        !strstr(filename,".VXE") && !strstr(filename,".VOM") &&
        !strstr(filename,".SYS") && !strstr(filename,".VIR") &&
        !strstr(filename,".OVL") && !strstr(filename,".BIN") )
        return;
#endif
    if (strstr(filename,".SYS"))
        sys_flag = FLAG_SYS_FILE;
    else
        sys_flag = FALSE;
    
    printf("Scan: %-60s -> \n",pathname);
    
    dwTotalFiles++;
    
    hFile = DFileOpen(filename,READ_ONLY_FILE);
    if (hFile != (HFILE)-1)
    {
        gdwPagesRead = 0;
        pamStatus = PAMScanFile(hLocal, hFile,  sys_flag,&wVirusID);
        printf("Pages read = %lu\n",gdwPagesRead);
        DFileClose(hFile);
    }
    else
        pamStatus = PAMSTATUS_FILE_ERROR;
    
    if (pamStatus == PAMSTATUS_VIRUS_FOUND)
    {
        char temp[256];
        
        dwNumViruses ++;
        
        printf("\nVIRUS ID: %04X\n\n\n",wVirusID);
        
        if (gbRepairFile)
        {
            if (gbForceAskForID)
            {
                printf("Enter repair ID index value: ");
                scanf("%04X",&wVirusID);
                printf("Using virus ID 0x%04X\n",wVirusID);
            }
            
            sprintf(temp,"copy %s temp.xxx",filename);
            system(temp);
            
            pamStatus = PAMRepairFile(hLocal,
                master_dat,
                "temp.xxx",
                filename,
                wVirusID,
                0);
            
            switch (pamStatus)
            {
            case PAMSTATUS_OK:
                printf("Repaired OK!\n");
                break;
            case PAMSTATUS_NO_REPAIR:
                printf("No repair possible.\n");
                break;
            default:
                printf("Error occured: %d\n",pamStatus);
                break;
            }
        }
        else
            pamStatus = PAMSTATUS_NO_REPAIR;
        
        if (pamStatus != PAMSTATUS_OK && gbDeleteFile)
        {
            unlink(filename);
            printf("Deleted!\n");
        }
    }
    else if (pamStatus == PAMSTATUS_OK)
    {
#ifndef BUILD_SET
        printf("\nNo virus!\n\n");
#endif
    }
    else
        printf("Error dealing with file %s: %d\n",filename,pamStatus);
}

void do_all_files(char *path, PAMLHANDLE hLocal)
{
#if defined(_MSC_VER)
    printf ("do_all_files not supported in VC++\n");
#else
    struct ffblk       data;
    char             next[256];
    int             len = strlen(path);
    
    strcpy(next, path);
    if (next[len-1] != '\\')
        next[len++] = '\\';
    
    if(!findfirst("*.*", &data, 0xFF))
    {
        // Found a file.
        do
        {
            strcpy(next + len, data.ff_name);
            
            if(data.ff_attrib & FA_DIREC &&
                strcmp(data.ff_name, ".") && strcmp(data.ff_name, ".."))
            {
                // Directory
                chdir(data.ff_name);
                do_all_files(next,hLocal);
                chdir("..");
            }
            else if (!(data.ff_attrib & (FA_LABEL | FA_DIREC)))
            {
                // File
                do_file(data.ff_name, next,hLocal);
            }
        } while(!findnext(&data));
    }
#endif
}

int handler(void)
{
    PAMLocalClose(hgLocal);
    PAMGlobalClose(hgGlobal);
    setdisk(toupper(start_path[0])-'A');
    start_path[strlen(start_path)-1] = 0;
    chdir(start_path);
    return(0);
}


void parse_options(int argc, char *argv[])
{
    const char* const options[][2] = {
        {"/TREE","examine directory subtree recursively\n"},
        {"/DEL", "delete (infected?) file\n"},
    };
    int i;
    
    gbRepairFile = gbDeleteFile = gbRecurse = FALSE;
    dwGSIter = 9999999LU;   //0xFFFFFFFF;
    
    for (i=1;i<argc;i++)
    {
        if (*argv[i] == '-') *argv[i] = '/';
        if (*argv[i] != '/') continue;

        if (!stricmp(argv[i],"/V"))   
        {
            gbVerbose = TRUE;
        }
        
        if (!stricmp(argv[i],"/TREE"))   //T
        {
            gbRecurse = TRUE;
        }
        
        if (!stricmp(argv[i],"/DEL"))   //D
        {
            gbDeleteFile = TRUE;
        }
        
        if (!stricmp(argv[i],"/REP"))   //R
        {
            gbRepairFile = TRUE;
        }
        
        if (!stricmp(argv[i],"/DREP"))   //G
        {
            gbDebugRepair = TRUE;
        }
        
        if (!stricmp(argv[i],"/ID"))   //F
        {
            gbForceAskForID = TRUE;
        }
        
        if (!stricmp(argv[i],"/MEM"))   //M
        {
            gbDumpMem = TRUE;
        }
        
        if (!stricmp(argv[i],"/NMOD"))   //no. of contiguous modified instructions to emulate
        {
            gbModified_Emulation_Limit = atoi(argv[++i]);
            assert (i < argc);
        }

        if (!stricmp(argv[i],"/MAX")) {  // max. instruction-emulation count
            dwGSIter = atol(argv[++i]);
            assert (i < argc);
        }

        // Emulation stops if and when the IP points to the beginning of the given string
        // Probably for use in GETSIG (see 'Writing PAM definitions')
        if (!stricmp(argv[i],"/XSIG")) {   // next arg is hex sig (maybe comma-delimited)
            gzSig = argv[++i];
        }
        
        if (!stricmp(argv[i],"/TEST")) {  // look for the given signature
           gbTestSig = TRUE;
        }
        
        if (!stricmp(argv[i],"/SIGCOUNT")) {
           gnSigs = atol (argv[++i]);
        }
        
        if (!stricmp(argv[i],"/SIGLIST")) {
           strcpy(gzSigList,argv[++i]);
        }
        
        if (!stricmp(argv[i],"/SIGSORTED")) {  // siglist is in ascending lex order
            gbSigSorted = TRUE;
        }
    }
}


void main(int argc, char *argv[])
{
    
    char            *proot, *ptr;
    int             initial_disk;
    PAMGHANDLE      hGlobal;
    PAMLHANDLE      hLocal;
    PAMSTATUS       pamStatus;
    
    // provide a callback functionality here!
    
#  if defined(BORLAND)
    ctrlbrk(handler);
#  endif
    strcpy(start_path, argv[0]);
    ptr = strrchr(start_path,'\\');
    if (ptr) 
        *(ptr+1) = 0;
    
    getcwd(root,MAXDIR);
    gzHome = strdup(root);
    sprintf(initial_dir,"%s\\master.dat",root);
    strcpy(master_dat,initial_dir);
    
    pamStatus = PAMGlobalInit(initial_dir,&hGlobal);
    if (pamStatus != PAMSTATUS_OK)
    {
        printf("Something didn't go right during global init: %d\n",pamStatus);
        exit(8);
    }
    
    pamStatus = PAMLocalInit(initial_dir,
        NULL,
        hGlobal,
        &hLocal);
    
    printf("Done\n");
    
    if (pamStatus != PAMSTATUS_OK)
    {
        printf("Something didn't go right during local init: %d\n",pamStatus);
        exit(8);
    }
    
    hgGlobal = hGlobal;
    hgLocal = hLocal;
    
    getcwd(initial_dir, MAXDIR);
    initial_disk = getdisk();
    
    if(argc >= 2)
        strcpy(root, argv[1]);
    else
        strcpy(root, "\\");
    
    parse_options(argc,argv);

    loadSig (hLocal,gzSig);   //AJR
    if (gnSigs) loadSigs();   //AJR
    
    // Is a drive letter specified?
    if(root[1] == ':')
    {
        proot = root + 2;
        setdisk(toupper(root[0]) - 'A');
    }
    else
        proot = root;
    
    
    if (!gbRecurse) {
        char* zPath = proot;
        char* zFile = strrchr(zPath,'\\');
        
        initAnalysis (gzHome);
        
        if (!zFile) {
            zFile = zPath;
            zPath = initial_dir;
        }else {
            *(zFile++) = '\0';
            if (strlen(zPath) == 0) zPath = "\\";
            chdir(zPath);
        }
        
        do_file (zFile, zPath, hLocal);
        
    }else {
        
        if (!strlen(proot))
        {
            chdir("\\");
            do_all_files(root,hLocal);
        }
        else if (chdir(proot) != -1)
            do_all_files(root,hLocal);
        else
            printf("Invalid directory.\n");
    }
    
    setdisk(initial_disk);
    chdir(initial_dir);
    
    PAMLocalClose(hLocal);
    PAMGlobalClose(hGlobal);
    endAnalysis();
    
    printf("\n\nPAM Summary\n");
    printf("Files Scanned: %ld\nViruses Found: %ld\n",
        dwTotalFiles, dwNumViruses);

    if (gbTestSig && (dwTotalFiles == 0 || dwNumViruses != dwTotalFiles)) {
       printf("*** signature was not found\n");
       exit (1);
    }
    
    exit(0);
}
