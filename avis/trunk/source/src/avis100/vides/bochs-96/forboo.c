#include <stdio.h>
#include "forboo.h"

void (*recordFetch)(Bit16u seg, Bit32u ofs, int sz) = recordFetchOp;

static Bit8u boot_sec_code_map[512];

static void printBootCodeMap(void) {
	int i=0,imin;
	FILE *fsec;
	extern char boo_sections[]; // declared in main.c

	// starts by merging code sections distant by less than 2 bytes
	for (i=0; i<512-4; ++i)
		if (boot_sec_code_map[i] && boot_sec_code_map[i+3])
			boot_sec_code_map[i+1] = boot_sec_code_map[i+2] = 1;

	// then print the sections to a file
	if ((fsec = fopen(boo_sections,"wb")) == NULL) {
		fprintf(stderr, "Can't open boot code/data sections file!\n");
		return;
	}
	for (i=0; i<512;) {
		for (; i<512 && !boot_sec_code_map[i]; ++i);
		imin = i;
		for (; i<512 && boot_sec_code_map[i]; ++i);
		if (i-1>imin) fprintf(fsec," [%6d %6d]\n",imin,i-1);
	}
	fclose(fsec);
}

void recordFetchOp(Bit16u seg, Bit32u ofs, int sz) {
	Bit32u la = (seg*16+ofs)&0xFFFFF; // linear address in real-mode
	static int boot_sec_spy;
	if (la >= 0x7C00 && la < 0x7E00) {
		boot_sec_spy=1;
		while (sz--) boot_sec_code_map[la++&0x1FF] = 1;
	}
	else if (la < 0x7C00 || la >= 0x7E00 && boot_sec_spy) {
		boot_sec_spy=0;
		recordFetch = NULL;
		printBootCodeMap();
		return;
	}
}
