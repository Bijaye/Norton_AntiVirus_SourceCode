// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Test Linux implementation of NDK String Handling functions

// ================== ndkStrings-test ===================================================

#include <ndkStrings.h>

#include "ndkDebug.h"

// ================== language ==========================================================

// Access current language

static void language() {
	BYTE name[25]; int id,rc;

	dbgInt("id = GetCurrentOSLanguageID() -> %i",id = GetCurrentOSLanguageID());
	wfInt("language","GetCurrentOSLanguageID()",15,id);

	dbgInt("rc = ReturnLanguageName(id,name) -> %i",rc = ReturnLanguageName(id,name));
	dbgStr("name == \"%s\"",name);
	wfStr("language","ReturnLanguageName()","English",name);
}

// ================== initLocale ========================================================

// Access the locale data and print its contents.

static void initLocale() {
	struct LCONV lconv,*pLconv;

	dbgStr("NWLsetlocale(LC_CTYPE,\"en_US\") -> \"%s\"",
			 NWLsetlocale(LC_CTYPE,"en_US.UTF-8"));
	dbgStr("nl_langinfo(CODESET) -> \"%s\"",nl_langinfo(CODESET));

	dbgPtr("&lconv == %u",&lconv);
	dbgPtr("NWLlocaleconv(&lconv) -> %u",pLconv = NWLlocaleconv(&lconv));
	wfPtr("locale","NWLlocaleconv()", &lconv, pLconv);

   dbgStr("pLconv->decimal_point        == \"%s\"",pLconv->decimal_point);
   dbgStr("pLconv->thousands_sep        == \"%s\"",pLconv->thousands_sep);
   dbgStr("pLconv->grouping             == \"%s\"",pLconv->grouping);
   dbgStr("pLconv->int_curr_symbol      == \"%s\"",pLconv->int_curr_symbol);
   dbgStr("pLconv->currency_symbol      == \"%s\"",pLconv->currency_symbol);
   dbgStr("pLconv->mon_decimal_point    == \"%s\"",pLconv->mon_decimal_point);
   dbgStr("pLconv->mon_thousands_sep    == \"%s\"",pLconv->mon_thousands_sep);
   dbgStr("pLconv->mon_grouping         == \"%s\"",pLconv->mon_grouping);
   dbgStr("pLconv->positive_sign        == \"%s\"",pLconv->positive_sign);
   dbgStr("pLconv->negative_sign        == \"%s\"",pLconv->negative_sign);
   dbgChr("pLconv->int_frac_digits      == '%c'",  pLconv->int_frac_digits);
   dbgChr("pLconv->frac_digits          == '%c'",  pLconv->frac_digits);
   dbgChr("pLconv->p_cs_precedes        == '%c'",  pLconv->p_cs_precedes);
   dbgChr("pLconv->p_sep_by_space       == '%c'",  pLconv->p_sep_by_space);
   dbgChr("pLconv->n_cs_precedes        == '%c'",  pLconv->n_cs_precedes);
   dbgChr("pLconv->n_sep_by_space       == '%c'",  pLconv->n_sep_by_space);
   dbgChr("pLconv->p_sign_posn          == '%c'",  pLconv->p_sign_posn);
   dbgChr("pLconv->n_sign_posn          == '%c'",  pLconv->n_sign_posn);
#ifdef __USE_ISOC99
   dbgChr("pLconv->int_p_cs_precedes    == '%c'",  pLconv->int_p_cs_precedes);
   dbgChr("pLconv->int_p_sep_by_space   == '%c'",  pLconv->int_p_sep_by_space);
   dbgChr("pLconv->int_n_cs_precedes    == '%c'",  pLconv->int_n_cs_precedes);
   dbgChr("pLconv->int_n_sep_by_space   == '%c'",  pLconv->int_n_sep_by_space);
   dbgChr("pLconv->int_p_sign_posn      == '%c'",  pLconv->int_p_sign_posn);
   dbgChr("pLconv->int_n_sign_posn      == '%c'",  pLconv->int_n_sign_posn);
#else
   dbgChr("pLconv->__int_p_cs_precedes  == '%c'",  pLconv->__int_p_cs_precedes);
   dbgChr("pLconv->__int_p_sep_by_space == '%c'",  pLconv->__int_p_sep_by_space);
   dbgChr("pLconv->__int_n_cs_precedes  == '%c'",  pLconv->__int_n_cs_precedes);
   dbgChr("pLconv->__int_n_sep_by_space == '%c'",  pLconv->__int_n_sep_by_space);
   dbgChr("pLconv->__int_p_sign_posn    == '%c'",  pLconv->__int_p_sign_posn);
   dbgChr("pLconv->__int_n_sign_posn    == '%c'",  pLconv->__int_n_sign_posn);
#endif
}

// ================== characters ========================================================

static void characters() {
	wchar_t* wcString = L"Sch\x00f6""ne Gr\x00fc\x00df""e aus Skr\x00e4""be\x00e5""n in Brom\x00f6""lla";
	char*		sbString =	"Schone Gruse aus Skrabean in Bromolla";
	char*		mbString;

	wchar_t* wcl = L"\x00f6""ne Gr\x00fc\x00df""e aus Skr\x00e4""be\x00e5""n in Brom\x00f6""lla";
	char*		sbl =	 "one Gruse aus Skrabean in Bromolla";
	char*		mbl;

	wchar_t* wcr = L"ne Gr\x00fc\x00df""e aus Skr\x00e4""be\x00e5""n in Brom\x00f6""lla";
	char*		sbr =	 "ne Gruse aus Skrabean in Bromolla";
	char*		mbr;

	wchar_t* wcs; char *mbs,*sbs; size_t l;

	mbString = wcs2mbs(wcString);
	mbl		= wcs2mbs(wcl);
	mbr		= wcs2mbs(wcr);

	dbgWcs("wcString = L\"%ls\"",wcString);
	dbgStr("mbString =  \"%s\"", mbString);
	dbgStr("sbString =  \"%s\"", sbString);

	// Get the string length as number of bytes and characters.

	dbgInt("strlen(mbString) -> %u",l = strlen(mbString));
	wfLng("characters", "strlen(mbString)", 43, l);
	dbgInt("_mbslen(mbString) -> %u",l = _mbslen(mbString));
	wfLng("characters", "_mbslen(mbString)", 37, l);

	// Step over the first non-ascii character in the string and then back.

	wcs = wcString + 3;
	dbgWcs("wcs == L\"%ls\"",wcs);
	dbgWcs("_wcsinc(wcs) -> L\"%ls\"",wcs = _wcsinc(wcs));
	wfWcs("characters","_wcsinc()",wcr,wcs);
	dbgWcs("_wcsdec(wcString,wcs) -> L\"%ls\"",wcs = _wcsdec(wcString,wcs));
	wfWcs("characters","_wcsdec()",wcl,wcs);

	mbs = mbString + 3;
	dbgStr("mbs == \"%s\"",mbs);
	dbgStr("_mbsinc(mbs) -> \"%s\"",mbs = _mbsinc(mbs));
	wfStr("characters","_mbsinc()",mbr,mbs);
	dbgStr("_mbsdec(mbString,mbs) -> \"%s\"",mbs = _mbsdec(mbString,mbs));
	wfStr("characters","_mbsdec()",mbl,mbs);

	sbs = sbString + 3;
	dbgStr("sbs == \"%s\"",sbs);
	dbgStr("_strinc(sbs) -> \"%s\"",sbs = _strinc(sbs));
	wfStr("characters","_strinc()",sbr,sbs);
	dbgStr("_strdec(sbString,sbs) -> \"%s\"",sbs = _strdec(sbString,sbs));
	wfStr("characters","_strdec()",sbl,sbs);

	// Start at the second to last character and step forward to the last, to the
	// terminating '\0', and then past it. The last step should not work and
	// still point to the terminating '\0'.

	wcs = wcString + wcslen(wcString) - 2;
	dbgWcs("wcs == L\"%ls\"",wcs);
	dbgWcs("_wcsinc(wcs) -> L\"%ls\"",wcs = _wcsinc(wcs));
	wfWcs("characters","_wcsinc() last",L"a",wcs);
	dbgWcs("_wcsinc(wcs) -> L\"%ls\"",wcs = _wcsinc(wcs));
	wfWcs("characters","_wcsinc() end",L"",wcs);
	dbgWcs("_wcsinc(wcs) -> L\"%ls\"",wcs = _wcsinc(wcs));
	wfWcs("characters","_wcsinc() after",L"",wcs);

	mbs = mbString + strlen(mbString) - 2;
	dbgStr("mbs == \"%s\"",mbs);
	dbgStr("_mbsinc(mbs) -> \"%s\"",mbs = _mbsinc(mbs));
	wfStr("characters","_mbsinc() last","a",mbs);
	dbgStr("_mbsinc(mbs) -> \"%s\"",mbs = _mbsinc(mbs));
	wfStr("characters","_mbsinc() end","",mbs);
	dbgStr("_mbsinc(mbs) -> \"%s\"",mbs = _mbsinc(mbs));
	wfStr("characters","_mbsinc() after","",mbs);

	sbs = sbString + strlen(sbString) - 2;
	dbgStr("sbs == \"%s\"",sbs);
	dbgStr("_strinc(sbs) -> \"%s\"",sbs = _strinc(sbs));
	wfStr("characters","_strinc() last","a",sbs);
	dbgStr("_strinc(sbs) -> \"%s\"",sbs = _strinc(sbs));
	wfStr("characters","_strinc() end","",sbs);
	dbgStr("_strinc(sbs) -> \"%s\"",sbs = _strinc(sbs));
	wfStr("characters","_strinc() after","",sbs);

	// Backward step from a NULL string is an error.

	wcs = NULL;
	dbgWcs("wcs == L\"%ls\"",wcs);
	dbgWcs("_wcsdec(wcString,wcs) -> L\"%ls\"",wcs = _wcsdec(wcString,wcs));
	wfWcs("characters","_wcsdec() NULL",NULL,wcs);

	mbs = NULL;
	dbgStr("mbs == \"%s\"",mbs);
	dbgStr("_mbsdec(mbString,mbs) -> \"%s\"",mbs = _mbsdec(mbString,mbs));
	wfStr("characters","_mbsdec() NULL",NULL,mbs);

	sbs = NULL;
	dbgStr("sbs == \"%s\"",sbs);
	dbgStr("_strdec(sbString,sbs) -> \"%s\"",sbs = _strdec(sbString,sbs));
	wfStr("characters","_strdec() NULL",NULL,sbs);

	// Backward step from before the start of the string is an error.

	wcs = wcString - 1;
	dbgNil("wcs == before");
	dbgWcs("_wcsdec(wcString,wcs) -> L\"%ls\"",wcs = _wcsdec(wcString,wcs));
	wfWcs("characters","_wcsdec() before",NULL,wcs);

	mbs = mbString - 1;
	dbgNil("mbs == before");
	dbgStr("_mbsdec(mbString,mbs) -> \"%s\"",mbs = _mbsdec(mbString,mbs));
	wfStr("characters","_mbsdec() before",NULL,mbs);

	sbs = sbString - 1;
	dbgNil("sbs == before");
	dbgStr("_strdec(sbString,sbs) -> \"%s\"",sbs = _strdec(sbString,sbs));
	wfStr("characters","_strdec() before",NULL,sbs);

	// Backward step from the start of the string is an error.

	wcs = wcString;
	dbgWcs("wcs == L\"%ls\"",wcs);
	dbgWcs("_wcsdec(wcString,wcs) -> L\"%ls\"",wcs = _wcsdec(wcString,wcs));
	wfWcs("characters","_wcsdec() beginning",NULL,wcs);

	mbs = mbString;
	dbgStr("mbs == \"%s\"",mbs);
	dbgStr("_mbsdec(mbString,mbs) -> \"%s\"",mbs = _mbsdec(mbString,mbs));
	wfStr("characters","_mbsdec() beginning",NULL,mbs);

	sbs = sbString;
	dbgStr("sbs == \"%s\"",sbs);
	dbgStr("_strdec(sbString,sbs) -> \"%s\"",sbs = _strdec(sbString,sbs));
	wfStr("characters","_strdec() beginning",NULL,sbs);

	// It's OK to step backward from the terminating '\0' character.

	wcs = wcString + wcslen(wcString);
	dbgWcs("wcs == L\"%ls\"",wcs);
	dbgWcs("_wcsdec(wcString,wcs) -> L\"%ls\"",wcs = _wcsdec(wcString,wcs));
	wfWcs("characters","_wcsdec() end",L"a",wcs);

	mbs = mbString + strlen(mbString);
	dbgStr("mbs == \"%s\"",mbs);
	dbgStr("_mbsdec(mbString,mbs) -> \"%s\"",mbs = _mbsdec(mbString,mbs));
	wfStr("characters","_mbsdec() end","a",mbs);

	sbs = sbString + strlen(sbString);
	dbgStr("sbs == \"%s\"",sbs);
	dbgStr("_strdec(sbString,sbs) -> \"%s\"",sbs = _strdec(sbString,sbs));
	wfStr("characters","_strdec() end","a",sbs);

	// It's an error to step backward from a point after the terminating '\0'.

	wcs = wcString + wcslen(wcString) + 1;
	dbgNil("wcs == after");
	dbgWcs("_wcsdec(wcString,wcs) -> L\"%ls\"",wcs = _wcsdec(wcString,wcs));
	wfWcs("characters","_wcsdec() after",NULL,wcs);

	mbs = mbString + strlen(mbString) + 1;
	dbgNil("mbs == after");
	dbgStr("_mbsdec(mbString,mbs) -> \"%s\"",mbs = _mbsdec(mbString,mbs));
	wfStr("characters","_mbsdec() after",NULL,mbs);

	sbs = sbString + strlen(sbString) + 1;
	dbgNil("sbs == after");
	dbgStr("_strdec(sbString,sbs) -> \"%s\"",sbs = _strdec(sbString,sbs));
	wfStr("characters","_strdec() after",NULL,sbs);

	free(mbString); free(mbl); free(mbr);
}

// ================== search ============================================================

static void search() {
	wchar_t wcc; char *mbString,*mbr,*mbs;

	mbString = wcs2mbs(L"Sch\x00f6""ne Gr\x00fc\x00df""e aus Skr\x00e4""be\x00e5""n in Brom\x00f6""lla");
	dbgStr("mbString == \"%s\"",mbString);

	// Locate the only '\u00df' multi-byte character in the string both from the beginning and
	// from the end.

	wcc = L'\x00df'; mbr = wcs2mbs(L"\x00df""e aus Skr\x00e4""be\x00e5""n in Brom\x00f6""lla");
	dbgWch("wcc == L'%lc'",wcc);
	dbgStr("mbschr(mbString,wcc) -> \"%s\"",mbs = mbschr(mbString,wcc));
	wfStr("search","mbschr()",mbr,mbs);
	dbgStr("mbsrchr(mbString,wcc) -> \"%s\"",mbs = mbsrchr(mbString,wcc));
	wfStr("search","mbsrchr()",mbr,mbs); free(mbr);

	// Locate the only 'u' single-byte character in the string both from the beginning and
	// from the end.

	wcc = L'u'; mbr = wcs2mbs(L"us Skr\x00e4""be\x00e5""n in Brom\x00f6""lla");
	dbgWch("wcc == L'%lc'",wcc);
	dbgStr("mbschr(mbString,wcc) -> \"%s\"",mbs = mbschr(mbString,wcc));
	wfStr("search","mbschr()",mbr,mbs);
	dbgStr("mbsrchr(mbString,wcc) -> \"%s\"",mbs = mbsrchr(mbString,wcc));
	wfStr("search","mbsrchr()",mbr,mbs); free(mbr);

	// Locate the first and the last '\u00f6' multi-byte character in the string.

	wcc = L'\x00f6'; mbr = wcs2mbs(L"\x00f6""ne Gr\x00fc\x00df""e aus Skr\x00e4""be\x00e5""n in Brom\x00f6""lla");
	dbgWch("wcc == L'%lc'",wcc);
	dbgStr("mbschr(mbString,wcc) -> \"%s\"",mbs = mbschr(mbString,wcc));
	wfStr("search","mbschr()",mbr,mbs); free(mbr);
	mbr = wcs2mbs(L"\x00f6""lla");
	dbgStr("mbsrchr(mbString,wcc) -> \"%s\"",mbs = mbsrchr(mbString,wcc));
	wfStr("search","mbsrchr()",mbr,mbs); free(mbr);

	// Locate the first and the last 'r' single-byte character in the string.

	wcc = L'r'; mbr = wcs2mbs(L"r\x00fc\x00df""e aus Skr\x00e4""be\x00e5""n in Brom\x00f6""lla");
	dbgWch("wcc == L'%lc'",wcc);
	dbgStr("mbschr(mbString,wcc) -> \"%s\"",mbs = mbschr(mbString,wcc));
	wfStr("search","mbschr()",mbr,mbs); free(mbr);
	mbr = wcs2mbs(L"rom\x00f6""lla");
	dbgStr("mbsrchr(mbString,wcc) -> \"%s\"",mbs = mbsrchr(mbString,wcc));
	wfStr("search","mbsrchr()",mbr,mbs); free(mbr);

	// Locate the first and the last character in the string.

	wcc = L'S'; mbr = mbString;
	dbgWch("wcc == L'%lc'",wcc);
	dbgStr("mbschr(mbString,wcc) -> \"%s\"",mbs = mbschr(mbString,wcc));
	wfStr("search","mbschr()",mbr,mbs);
	wcc = L'a'; mbr = wcs2mbs(L"a");
	dbgWch("wcc == L'%lc'",wcc);
	dbgStr("mbsrchr(mbString,wcc) -> \"%s\"",mbs = mbsrchr(mbString,wcc));
	wfStr("search","mbsrchr()",mbr,mbs); free(mbr);

	// Locate the non-existing '\u00c5' multi-byte character in the string both from the
	// beginning and from the end.

	wcc = L'\x00c5';
	dbgWch("wcc == L'%lc'",wcc);
	dbgStr("mbschr(mbString,wcc) -> \"%s\"",mbs = mbschr(mbString,wcc));
	wfStr("search","mbschr()",NULL,mbs);
	dbgStr("mbsrchr(mbString,wcc) -> \"%s\"",mbs = mbsrchr(mbString,wcc));
	wfStr("search","mbsrchr()",NULL,mbs);

	// Locate the non-existing 'X' single-byte character in the string both from the
	// beginning and from the end.

	wcc = L'X';
	dbgWch("wcc == L'%lc'",wcc);
	dbgStr("mbschr(mbString,wcc) -> \"%s\"",mbs = mbschr(mbString,wcc));
	wfStr("search","mbschr()",NULL,mbs);
	dbgStr("mbsrchr(mbString,wcc) -> \"%s\"",mbs = mbsrchr(mbString,wcc));
	wfStr("search","mbsrchr()",NULL,mbs);

	// The terminating '\0' is not part of the string;	 therefore, it is an error to look
	// for a '\0'.

	wcc = L'\0';
	dbgNil("wcc == L'\\0'");
	dbgStr("mbschr(mbString,wcc) -> \"%s\"",mbs = mbschr(mbString,wcc));
	wfStr("search","mbschr()",NULL,mbs);
	dbgStr("mbsrchr(mbString,wcc) -> \"%s\"",mbs = mbsrchr(mbString,wcc));
	wfStr("search","mbsrchr()",NULL,mbs);

	// Locate the '\u00c5' multi-byte character in an empty string both from the beginning and
	// from the end.

	strcpy(mbString,""); dbgStr("mbString == \"%s\"",mbString);

	wcc = L'\x00c5';
	dbgWch("wcc == L'%lc'",wcc);
	dbgStr("mbschr(mbString,wcc) -> \"%s\"",mbs = mbschr(mbString,wcc));
	wfStr("search","mbschr()",NULL,mbs);
	dbgStr("mbsrchr(mbString,wcc) -> \"%s\"",mbs = mbsrchr(mbString,wcc));
	wfStr("search","mbsrchr()",NULL,mbs);

	// Locate the 'X' single-byte character in an empty string both from the beginning and
	// from the end.

	wcc = L'X';
	dbgWch("wcc == L'%lc'",wcc);
	dbgStr("mbschr(mbString,wcc) -> \"%s\"",mbs = mbschr(mbString,wcc));
	wfStr("search","mbschr()",NULL,mbs);
	dbgStr("mbsrchr(mbString,wcc) -> \"%s\"",mbs = mbsrchr(mbString,wcc));
	wfStr("search","mbsrchr()",NULL,mbs);

	free(mbString);
}

// ================== caseConversion ====================================================

static void caseConversion() {
	wchar_t* wcMixed = L"Sch\x00f6""ne Gr\x00fc\x00df""e aus Skr\x00e4""be\x00e5""n in Brom\x00f6""lla";
	wchar_t* wcUpper = L"SCH\x00d6""NE GR\x00dc\x00df""E AUS SKR\x00c4""BE\x00c5""N IN BROM\x00d6""LLA";
	wchar_t* wcLower = L"sch\x00f6""ne gr\x00fc\x00df""e aus skr\x00e4""be\x00e5""n in brom\x00f6""lla";
	char*		sbMixed =  "Schone Gruse aus Skrabean in Bromolla";
	char*		sbUpper =  "SCHONE GRUSE AUS SKRABEAN IN BROMOLLA";
	char*		sbLower =  "schone gruse aus skrabean in bromolla";

	char* mbUpper = wcs2mbs(wcUpper);
	char* mbLower = wcs2mbs(wcLower);

	size_t n = 100;
	wchar_t* wcString = calloc(n,sizeof(wchar_t));
	char*		mbString;
	char*		sbString = calloc(n,sizeof(char));

	wchar_t* wcs; char *mbs,*sbs;

// --------------------- _wcsupr _wcslwr ------------------------------------------------

	wcscpy(wcString,wcMixed);
	dbgWcs("wcString == L\"%ls\"",wcString);

	dbgWcs("_wcsupr(wcString) -> L\"%ls\"",wcs = _wcsupr(wcString));
	wfPtr("caseConversion","_wcsupr()",wcString,wcs);
	wfWcs("caseConversion","_wcsupr()",wcUpper,wcs);

	dbgWcs("_wcslwr(wcString) -> L\"%ls\"",wcs = _wcslwr(wcString));
	wfPtr("caseConversion","_wcsupr()",wcString,wcs);
	wfWcs("caseConversion","_wcslwr()",wcLower,wcs);

	free(wcString);

// --------------------- _mbsupr _mbslwr ------------------------------------------------

	mbString = wcs2mbs(wcMixed);
	dbgStr("mbString == \"%s\"",mbString);

	dbgStr("_mbsupr(mbString) -> \"%s\"",mbs = _mbsupr(mbString));
	wfPtr("caseConversion","_mbsupr()",mbString,mbs);
	wfStr("caseConversion","_mbsupr()",mbUpper,mbs);

	dbgStr("_mbslwr(mbString) -> \"%s\"",mbs = _mbslwr(mbString));
	wfPtr("caseConversion","_mbsupr()",mbString,mbs);
	wfStr("caseConversion","_mbslwr()",mbLower,mbs);

	free(mbString); free(mbUpper); free(mbLower);

// --------------------- _strupr _strlwr ------------------------------------------------

	strcpy(sbString,sbMixed);
	dbgStr("sbString == \"%s\"",sbString);

	dbgStr("_strupr(sbString) -> \"%s\"",sbs = _strupr(sbString));
	wfPtr("caseConversion","_sbsupr()",sbString,sbs);
	wfStr("caseConversion","_strupr()",sbUpper,sbs);

	dbgStr("_strlwr(sbString) -> \"%s\"",sbs = _strlwr(sbString));
	wfPtr("caseConversion","_sbsupr()",sbString,sbs);
	wfStr("caseConversion","_strlwr()",sbLower,sbs);

	free(sbString);
}

// ================== main ==============================================================

int main(int argc,char* argv[]) {
	if (dbgInit(argc,argv,"Strings")) {
		dbgNL("language()");		   language();
		dbgNL("initLocale()");		initLocale();
		dbgNL("characters()");		characters();
		dbgNL("search()");			search();
		dbgNL("caseConversion()"); caseConversion();
	}
	return dbgFinal();
}
