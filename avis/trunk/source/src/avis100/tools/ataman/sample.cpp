/*
	The source code in this file is hereby released to the public domain.

	The source code is an example, with explanations, of how to make custom
	programming work with the simple mode of the ATRLS.
*/

#define STRICT
#include <windows.h>

#include <conio.h>

#include <stdio.h>
#include <stdlib.h>

static void CleanupConsole(void);
static void InitConsole(void);

static HANDLE hConIn = INVALID_HANDLE_VALUE;
static DWORD dwSaveOldMode;

/*
	When using the code below, you'll want to change all occurrances of:
		// Error
	To something more appropriate.
*/

static int main(int, char **) {
	InitConsole();
	unsigned long cnt = 0;

	printf("\033[2J\033[5;10H<-(5,10) Type Control-Z to exit.\n");

	for (;;) {
		if (!_kbhit()) {
			printf("\033[8;10H\033[K<-(8,10) Idle count: %lu", cnt++);
			Sleep(200);
		} else {
			int c = _getch();
			if (c == '\032') {
				break;
			}
			if (c == 0) {
				c = _getch();
				printf("\033[11;10H\033[K<-(11,10) Hex value of extended character last typed: %x", (unsigned int)c);
			} else {
				printf("\033[11;10H\033[K<-(11,10) Hex value of character last typed: %x", (unsigned int)c);
			}
			cnt = 0;
		}
	}

	printf("\033[2J");

	return (0);
}


static void CleanupConsole(void) {
	if (hConIn != INVALID_HANDLE_VALUE) {
		if (!SetConsoleMode(hConIn, dwSaveOldMode)) {
			// Error
		}
		if (!CloseHandle(hConIn)) {
			// Error
		}
		hConIn = INVALID_HANDLE_VALUE;
	}
}

static void InitConsole(void) {
	/* 
		Setup so that console input mode is automatically cleaned up
		at program exit.
	*/
	if (atexit(CleanupConsole) != 0) {
		// Error
	}


	/*
		Standard Output is attached to a pipe.  Most C runtimes
		buffer output to a pipe.  This is usually not desireable
		when using a pipe to simulate console output.
		Alternatively, you could do a fflush(stdout) at appropriate
		places in your code.
	*/
	if (setvbuf(stdout, NULL, _IONBF, 0) != 0) {
		// Error
	}
	
	
	/*
		These last few lines setup console input in a manner that
		tells the ATRLS that you want to handle character input
		on a character by character basis.
	*/
	hConIn = CreateFile("CONIN$", GENERIC_READ |GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, 0, 0);
	if (hConIn == INVALID_HANDLE_VALUE) {
		// Error
	}
	if (!GetConsoleMode(hConIn, &dwSaveOldMode)) {
		// Error
	}

	/*
		Set console for character at a time I/O.
		The ATRLS will still handle the "special" keys begining
		with ^A or ESC (see tables in the user's manual).
		The operating system will still intercept ^C.
	*/
	if (!SetConsoleMode(hConIn, ENABLE_PROCESSED_INPUT)) {
		// Error
	}
	
	/* ALTERNATIVELY, if you want absolutely RAW processing
		where you have to handle all character processing yourself.
	*/
/*
	if (!SetConsoleMode(hConIn, 0)) {
		// Error
	}
*/
}


/*
	Do NOT use your compiler vendors versions of _kbhit and _getch.
	In simple mode, stdin is connected to an anonymous pipe.  Most
	versions of _kbhit and _getch will not work properly with pipes.
*/
static int chSpecialKeyPending;

int _kbhit(void) {
	DWORD dw;
	INPUT_RECORD ir;
	if (chSpecialKeyPending != 0) {
			return 1;
	}

	for (;;) {
		if (!PeekConsoleInput(hConIn, &ir, 1, &dw)) {
			// Error
			return 0;
		}
		if (dw == 0) {
			return 0;
		}
		// LATER Ctrl, Alt, shift?
		if (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown) {
			return 1;
		}
		/* We don't need this event, so discard it */
		if (!ReadConsoleInput(hConIn, &ir, 1, &dw)) {
			// Error
		}
	}
}

int _getch(void) {
	DWORD dw;
	INPUT_RECORD ir;

	if (chSpecialKeyPending != 0) {
		int ch = chSpecialKeyPending;
		chSpecialKeyPending = 0;
		return ch;
	}

	for (;;) {
		if (!ReadConsoleInput(hConIn, &ir, 1, &dw)) {
			// Error
		}

		if (ir.EventType != KEY_EVENT) {
				continue;
		}
		if (!ir.Event.KeyEvent.bKeyDown) {
			continue;
		}
		/*
			Special Key handling here may not match your compiler.
			You may need to add a switch statement to get the values you expect.
			The values returned here correspond with the VK_ values
			defined (via windows.h) in WINUSER.H.
			(#include <windows.h>, _NOT_ #include <winuser.h>)
		*/
		if (ir.Event.KeyEvent.uChar.AsciiChar == '\0') {
			chSpecialKeyPending = (int) ir.Event.KeyEvent.wVirtualKeyCode;
			return 0;
		}
		return (int) ir.Event.KeyEvent.uChar.AsciiChar;
	}
}

