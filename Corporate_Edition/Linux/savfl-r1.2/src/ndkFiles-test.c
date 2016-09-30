//ndkFiles-test.cpp

#include <stdio_ext.h>
#include <string.h>

#include "ndkFiles.h"

#include "dprintf.h"
#include "dprintf_2.h"

#include "ndkDebug.h"
#include "ndkDebugPrint.h"

int main(int argc,char* argv[]) {
	if (dbgInit(argc,argv,"Files")) {
		if (dbgVerbose) {
			ConsolePrintf("\ntesting ConsolePrintf\n");
			printf("calling fflush\n");
		}
		int ret = flushall();
		if (dbgVerbose) printf("flushall returned %d\n",ret);
		if (ret != 0) {
			printf("\n<Error> Expected \"0\" but got \"%i\" !\n",ret); dbgResult = 1;
		}
	}
	return dbgFinal();
}
