//************************************************************************
//
// $Header:   S:\navex\vcs\navexlnk.c_v   1.0   19 Oct 1995 14:01:52   DCHI  $
//
// Description:
//  NAVEX DOS linker.
//
//************************************************************************
// $Log:   S:\navex\vcs\navexlnk.c_v  $
// 
//    Rev 1.0   19 Oct 1995 14:01:52   DCHI
// Initial revision.
// 
//************************************************************************

#include <malloc.h>
#include <stdio.h>
#include <string.h>

typedef unsigned char BYTE;
typedef unsigned int WORD;

typedef struct
{
	BYTE	bySymantecSig[256];
} header_type;

#define NLINK_FILE      "NAVEXLNK.TMP"
#define MAX_COM_SIZE    0xF000U
#define ERROR           -1

int process(char *link_file)
{
	FILE    *stream, *out, *module;
	BYTE    *pBuffer;
	WORD	wBytes;
	char    temp[256], *ptr;
	header_type h = {"NAVEX - Norton Antivirus EXternal Module - Copyright (C) 1995 Symantec Corp."};

    pBuffer = (BYTE *)_fmalloc(MAX_COM_SIZE);
	if (pBuffer == NULL)
	{
		printf("nlink: unable to allocate memory.\n");
		return(ERROR);
	}

	stream = fopen(link_file,"rb");
	if (!stream)
	{
		printf("nlink: unable to open lnk file.\n");
		return(ERROR);
	}

	out = fopen(NLINK_FILE,"wb");
	if (!out)
	{
		fclose(stream);
		printf("nlink: unable to open swap file %s.\n",NLINK_FILE);
		return(ERROR);
	}

	/* write that header out! */

	if (fwrite(&h,sizeof(h),1,out) != 1)
	{
		printf("nlink: unable to write to %s.\n",NLINK_FILE);
		fclose(stream);
		fclose(out);
		unlink(NLINK_FILE);
		return(ERROR);
	}

	while (!feof(stream))
	{
		if (fgets(temp,256,stream))
		{
			if ((ptr = strchr(temp,'+')) != NULL)
			{
				/* .COM file... */

				*ptr = 0;       /* remove the + from the EOL */

				module = fopen(temp,"rb");
				if (!module)
				{
					printf("nlink: unable to open MODULE %s.\n",temp);
					fclose(stream);
					fclose(out);
					unlink(NLINK_FILE);
					return(ERROR);
				}
				else
					printf("nlink: linking with %s.\n",temp);

				wBytes = fread(pBuffer,sizeof(BYTE),MAX_COM_SIZE,module);

				if (fwrite(&wBytes,sizeof(WORD),1,out) != 1 ||
					fwrite(pBuffer,sizeof(BYTE),wBytes,out) != wBytes)
				{
					printf("nlink: unable to write to %s.\n",NLINK_FILE);
					fclose(module);
					fclose(stream);
					fclose(out);
					unlink(NLINK_FILE);
					return(ERROR);
				}

				fclose(module);
			}
			else
			{
				/* last item in the list (target file)... */

				/* remove newline character from EOS */

				ptr = strchr(temp,'\r');
				if (ptr)
					*ptr = 0;

				break;
				
			}
		}
	}

	fclose(out);
	fclose(stream);

    unlink(temp);
	if (rename(NLINK_FILE,temp))
	{
		printf("nlink: unable to rename %s to %s.\n",NLINK_FILE,temp);
		return(ERROR);
	}

	printf("nlink: producing output file %s.\n",temp);

	return(0);
}




int main(int argc, char *argv[])
{
    printf("navexlnk v1.0 by Carey Nachenberg.  ");
    printf("Copyright (C) Symantec 1995\n\n");

    if (argc != 2)
    {
        printf("usage: nlink file.lnk\n");
        return(ERROR);
    }

	if (*argv[1] == '@')
		return(process(argv[1]+1));
	else
		return(process(argv[1]));

}
