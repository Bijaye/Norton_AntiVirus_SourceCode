#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "error.h"
#include "getline.h"
#include "sort.h"
#include "loadlex.h"

char lexfilename[256];
char fmt[256];

/* Format :
 *
 * NN : file number, starting from 0
 * PN : path name
 * VN : virus name
 * SN : sample name
 * NF : number of fragments
 * NL : size of likely array
 * NV : number of viruses
 * IM : immediate
 * RE : rescan
 * NS : number of signatures
 * HE : heuristic
 * H1 : heuristic & 1
 * TI : time
 * PS : sigs array
 * PF : frags array
 * PV : virs array
 * WS : weights sum
 *
 */

int glob_n = 0;

void printlist (long n, long *p)
{
	int i;
	printf ("    ");
	for (i=0; i<n; i++) {
		printf ("%ld ", *p++);
	}
	printf ("    ");
}

void dumplex (struct lex *plex)
{
	char *token;
	char aux[256];

	strcpy (aux, fmt);
	token = strtok (aux, ",");
	while (token) {
		if (strcmp(token, "PN")==0) printf ("%s ", plex->pathname);
		else if (strcmp(token, "NN")==0) printf ("%6ld ", glob_n);
		else if (strcmp(token, "VN")==0) printf ("%12s ", plex->virname);
		else if (strcmp(token, "SN")==0) printf ("%12s ", plex->smpname);
		else if (strcmp(token, "NF")==0) printf ("%6ld ", plex->z_nfrags);
		else if (strcmp(token, "NL")==0) printf ("%6ld ", plex->z_nlikely);
		else if (strcmp(token, "NV")==0) printf ("%6ld ", plex->z_nvirs);
		else if (strcmp(token, "IM")==0) printf ("%1d ", plex->immediate);
		else if (strcmp(token, "RE")==0) printf ("%1d ", plex->rescan);
		else if (strcmp(token, "NS")==0) printf ("%6ld ", plex->nsigs);
		else if (strcmp(token, "HE")==0) printf ("%3ld ", plex->heu);
		else if (strcmp(token, "H1")==0) printf ("%3ld ", plex->heu & 1);
		else if (strcmp(token, "TI")==0) printf ("%6ld ", plex->time);
		else if (strcmp(token, "PS")==0) printlist (plex->nsigs, plex->psigs);
		else if (strcmp(token, "PF")==0) printlist (plex->z_nfrags, plex->pfrags);
		else if (strcmp(token, "PV")==0) printlist (plex->z_nvirs, plex->pvirs);
		else if (strcmp(token, "WS")==0) printf ("%5ld ", plex->weights_sum);
		else error ("analex", "dumplex", "Bad format !", 0, 1, 0,0,0, fmt,token,NULL);
		token = strtok (NULL, ",");
	}
	printf ("\n");
}

void help (void)
{
	fprintf (stderr, "Create a summary of a trace.lex file.\n");
	fprintf (stderr, "Usage : analex (trace.lex file) (format) > (output)\n");
	fprintf (stderr, "format is a string describing the summary format. It is a list of\n");
	fprintf (stderr, "groups of 2 letters separated by commas. The possible groups are :\n");
	fprintf (stderr, "  NN : file number, starting from 0\n");
 	fprintf (stderr, "  PN : path name\n");
 	fprintf (stderr, "  VN : virus name\n");
 	fprintf (stderr, "  SN : sample name\n");
 	fprintf (stderr, "  NF : number of fragments\n");
 	fprintf (stderr, "  NL : size of likely array\n");
 	fprintf (stderr, "  NV : number of viruses\n");
 	fprintf (stderr, "  IM : immediate\n");
 	fprintf (stderr, "  RE : rescan\n");
 	fprintf (stderr, "  NS : number of signatures\n");
 	fprintf (stderr, "  HE : heuristic\n");
 	fprintf (stderr, "  H1 : heuristic & 1\n");
 	fprintf (stderr, "  TI : time\n");
 	fprintf (stderr, "  PS : sigs array\n");
 	fprintf (stderr, "  PF : frags array\n");
 	fprintf (stderr, "  PV : virs array\n");
 	fprintf (stderr, "  WS : weights sum\n");
	exit (1);
}



int main (int argc, char **argv)
{
	FILE *file;
	struct lex *plex;
	long l;

	glob_n = 0;
	argc--;
	if (argc<2) help ();

	strcpy (lexfilename, argv[1]);
	strcpy (fmt, argv[2]);
	argc -= 2;
	argv += 2;

	file = fopen (lexfilename, "r");
	if (file==NULL) fopenerr ("analex", "main", lexfilename);
	l = 0;
	while ((plex=nextlex (file))!=NULL) {
		dumplex (plex);
		if ((l%21)==0) {
			fprintf (stderr, "%6ld %12s %12s\r", l, plex->virname, plex->smpname);
		}
		l++;
		glob_n ++;
	}
	fprintf (stderr, "\n");
	fclose (file);
	return 0;
}


