#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int main(int argc, char* argv[]) {
int sleeptime;

	if (argc !=2) {
	  fprintf(stderr, "Usage: %s msnum\n", argv[0]);
	  fprintf(stderr, "Sleeps for msnum milliseconds and returns.\n");
	  return 100;
	}

	sleeptime = atoi(argv[1]);
	if (sleeptime <=0) {
	  fprintf(stderr, "%s: bad sleeptime value: %s\n", argv[0], argv[1]);
	  return 100;
	}

	Sleep(sleeptime);

return 0;
}
