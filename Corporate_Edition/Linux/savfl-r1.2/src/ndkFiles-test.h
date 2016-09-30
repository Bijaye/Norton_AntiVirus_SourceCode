//ndkFiles-test.h

#include <cxxtest/TestSuite.h>

#include <stdio_ext.h>
#include <string.h>

#include "ndkFiles.h"

class ndkFilesTest : public CxxTest::TestSuite
{
public:

    void testFflush( void )
    {
        int ret = flushall();
        TSM_ASSERT_EQUALS( "ret = flushall()", ret, 0 );
    }
};
