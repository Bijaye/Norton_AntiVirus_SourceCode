// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Test Linux implementation of NDK Integer Handling functions

// ================== ndkIntegers-test ==================================================

#include <cxxtest/TestSuite.h>

#include <ndkIntegers.h>

// --------------------- rotation -------------------------------------------------------

// Rotation by 4 is shown in hexadecimal representation. Rotation of 0 and 32 leaves the
// rotated value unchanged.

class ndkIntegersTest : public CxxTest::TestSuite
{
public:

void testRotation()
{
    unsigned long lFrom = 0xA9876543;
    char string[9];
    std::string sFrom("a9876543");
    std::string sToLeft("9876543a");
    std::string sToRight("3a987654");

    ultoa(lFrom, string, 16);
    TSM_ASSERT_EQUALS( "lFrom", sFrom, std::string(string) );

    ultoa(_lrotl(lFrom, 4), string, 16);
    TSM_ASSERT_EQUALS( "_lrotl()", sToLeft, std::string(string) );

    ultoa(_lrotr(lFrom, 4), string, 16);
    TSM_ASSERT_EQUALS( "_lrotr()", sToRight, std::string(string) );

    ultoa(_lrotl(lFrom, 0), string, 16);
    TSM_ASSERT_EQUALS( "_lrotl()", sFrom, std::string(string) );

    ultoa(_lrotr(lFrom, 0), string, 16);
    TSM_ASSERT_EQUALS( "_lrotr()", sFrom, std::string(string) );

    ultoa(_lrotl(lFrom, 32), string, 16);
    TSM_ASSERT_EQUALS( "_lrotl()", sFrom, std::string(string) );

    ultoa(_lrotr(lFrom, 32), string, 16);
    TSM_ASSERT_EQUALS( "_lrotr()", sFrom, std::string(string) );
}

// --------------------- radixConversion ------------------------------------------------

void testRadixConversion()
{
    int nr = 49;
    char string[7];
    char *s;

    //dbgInt("nr == %i", nr);

    s = itoa(nr, string, 2);
    TSM_ASSERT_EQUALS( "itoa", &string[0], s );

    itoa(nr, string, 10);
    TSM_ASSERT_EQUALS( "itoa 10", std::string("49"), std::string(string) );

    itoa(- nr, string, 10);
    TSM_ASSERT_EQUALS( "- itoa 10", std::string("-49"), std::string(string) );

    ltoa(nr, string, 10);
    TSM_ASSERT_EQUALS( "ltoa 10", std::string("49"), std::string(string) );

    ltoa(- nr, string, 10);
    TSM_ASSERT_EQUALS( "- ltoa 10", std::string("-49"), std::string(string) );

    itoa(nr, string, 2);
    TSM_ASSERT_EQUALS( "itoa 2", std::string("110001"), std::string(string) );

    itoa(nr, string, 7);
    TSM_ASSERT_EQUALS( "itoa 7", std::string("100"), std::string(string) );

    itoa(nr, string, 11);
    TSM_ASSERT_EQUALS( "itoa 11", std::string("45"), std::string(string) );

    itoa(nr, string, 18);
    TSM_ASSERT_EQUALS( "itoa 18", std::string("2d"), std::string(string) );

    itoa(nr, string, 24);
    TSM_ASSERT_EQUALS( "itoa 24", std::string("21"), std::string(string) );

    ltoa(nr, string, 29);
    TSM_ASSERT_EQUALS( "ltoa 29", std::string("1k"), std::string(string) );

    ultoa(nr, string, 36);
    TSM_ASSERT_EQUALS( "ultoa 36", std::string("1d"), std::string(string) );
}

};
