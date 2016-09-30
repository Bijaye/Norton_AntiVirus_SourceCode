#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#define BSIZE 65536

int main (int argc, char * argv [])
{
int fd1;
int fd2;
int i;
char buf1 [BSIZE];
char buf2 [BSIZE];
int n1;
int n2;
int n3;
int offset=0;

if (argc<3) {
	fprintf (stderr, "Usage: compare <file1> <file2>\n");
	return 1;
}

if ((fd1 = open (argv [1], O_RDONLY|O_BINARY))==-1) {
	fprintf (stderr, "Unable to open %s\n", argv [1]);
	return 1;
}
if ((fd2 = open (argv [2], O_RDONLY|O_BINARY))==-1) {
	fprintf (stderr, "Unable to open %s\n", argv [2]);
	return 1;
}

do {
printf (".");
if ((n1 = read (fd1, buf1, BSIZE))<0) goto DIFFERENT;
//printf ("n1=%d ", n1);
n2 = 0;
do
	if ((n2 += (n3 = read (fd2, &buf2[n2], n1-n2)))<0) goto DIFFERENT;
while (n2<n1 && n3>0);
//printf ("n2=%d\n", n2);
if (n1 != n2) goto DIFFERENT;
for (i=0;i<n1;i++,offset++)
	if (buf1[i] != buf2 [i]) goto DIFFERENT;
} while (n1>0);

close (fd1);
close (fd2);
printf ("\nFiles are equal\n");
return 0;

DIFFERENT:
close (fd1);
close (fd2);
printf ("\nFiles are different (at offset %d)\n", offset);
return 1;
}
