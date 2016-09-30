// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Test Linux implementation of NDK Integer Handling functions

// ================== ndkIntegers-test ==================================================

#include <ndkIntegers.h>

#include "ndkDebug.h"

// --------------------- rotation -------------------------------------------------------

// Rotation by 4 is shown in hexadecimal representation. Rotation of 0 and 32 leaves the
// rotated value unchanged.

static void rotation() {
	unsigned long lFrom = 0xA9876543;
	char string[9],
		*sFrom    =  "a9876543",
		*sToLeft  =   "9876543a",
		*sToRight = "3a987654";

	dbgPtr("ultoa(lFrom,string,16) -> 0x%s",ultoa(lFrom,string,16));
	wfStr("rotation","lFrom",sFrom,string);

	dbgPtr("ultoa(_lrotl(lFrom,4),string,16) -> 0x%s",
			 ultoa(_lrotl(lFrom,4),string,16));
	wfStr("rotation","_lrotl()",sToLeft,string);

	dbgPtr("ultoa(_lrotr(lFrom,4),string,16) -> 0x%s",
			 ultoa(_lrotr(lFrom,4),string,16));
	wfStr("rotation","_lrotr()",sToRight,string);

	dbgPtr("ultoa(_lrotl(lFrom,0),string,16) -> 0x%s",
			 ultoa(_lrotl(lFrom,0),string,16));
	wfStr("rotation","_lrotl()",sFrom,string);

	dbgPtr("ultoa(_lrotr(lFrom,0),string,16) -> 0x%s",
			 ultoa(_lrotr(lFrom,0),string,16));
	wfStr("rotation","_lrotr()",sFrom,string);

	dbgPtr("ultoa(_lrotl(lFrom,32),string,16) -> 0x%s",
			 ultoa(_lrotl(lFrom,32),string,16));
	wfStr("rotation","_lrotl()",sFrom,string);

	dbgPtr("ultoa(_lrotr(lFrom,32),string,16) -> 0x%s",
			 ultoa(_lrotr(lFrom,32),string,16));
	wfStr("rotation","_lrotr()",sFrom,string);
}

// --------------------- radixConversion ------------------------------------------------

static void radixConversion() {
	int nr = 49; char string[7],*s;

	dbgInt("nr == %i",nr);
	
	dbgPtr("itoa(nr,string,2) -> %p",s = itoa(nr,string,2));
	wfPtr("radixConversion", "itoa", &string, s);

	dbgStr("itoa(nr,string,10) -> \"%s\"",itoa(nr,string,10));
	wfStr("radixConversion","itoa 10","49",string);

	dbgStr("itoa(- nr,string,10) -> \"%s\"",itoa(- nr,string,10));
	wfStr("radixConversion","- itoa 10","-49",string);

	dbgStr("ltoa(nr,string,10) -> \"%s\"",ltoa(nr,string,10));
	wfStr("radixConversion","ltoa 10","49",string);

	dbgStr("ltoa(- nr,string,10) -> \"%s\"",ltoa(- nr,string,10));
	wfStr("radixConversion","- ltoa 10","-49",string);

	dbgStr("itoa(nr,string,2) -> \"%s\"",itoa(nr,string,2));
	wfStr("radixConversion","itoa 2","110001",string);

	dbgStr("itoa(nr,string,7) -> \"%s\"",itoa(nr,string,7));
	wfStr("radixConversion","itoa 7","100",string);

	dbgStr("itoa(nr,string,11) -> \"%s\"",itoa(nr,string,11));
	wfStr("radixConversion","itoa 11","45",string);

	dbgStr("itoa(nr,string,18) -> \"%s\"",itoa(nr,string,18));
	wfStr("radixConversion","itoa 18","2d",string);

	dbgStr("itoa(nr,string,24) -> \"%s\"",itoa(nr,string,24));
	wfStr("radixConversion","itoa 24","21",string);

	dbgStr("ltoa(nr,string,29) -> \"%s\"",ltoa(nr,string,29));
	wfStr("radixConversion","ltoa 29","1k",string);

	dbgStr("ultoa(nr,string,36) -> \"%s\"",ultoa(nr,string,36));
	wfStr("radixConversion","ultoa 36","1d",string);
}

// --------------------- main -----------------------------------------------------------

int main(int argc,char* argv[]) {
	if (dbgInit(argc,argv,"Integers")) {
		dbgNL("rotation()");        rotation();
		dbgNL("radixConversion()"); radixConversion();
	}
	return dbgFinal();
}
