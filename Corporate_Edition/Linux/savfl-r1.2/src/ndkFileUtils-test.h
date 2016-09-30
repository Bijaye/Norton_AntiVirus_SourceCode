
#include <cxxtest/TestSuite.h>

#include <stdio.h>


#include <string>
#include <stack>
//#include "ndkDefinitions.h"
//#include "NLM.H"
#include "FileUtils.h"

#include "vpstrutils.h"

class testFileUtils : public CxxTest::TestSuite
{
public:
    bool _fixturesValid;
    std::string _testDir;
    std::stack<std::string> _cleanupList;

    testFileUtils() :
        _fixturesValid(false)
    {
        char scratch[PATH_MAX] = {0};

        char* cwd = getcwd( NULL, 0 );
        if ( cwd != NULL )
        {
            strcpy( scratch, cwd );
            free(cwd);
            cwd = 0;
            strcat( scratch, "/TestBox_XXXXXX" );
            char * tmp = mkdtemp( scratch );
            if ( tmp )
            {
                _testDir = scratch;
                _cleanupList.push(_testDir);

                _fixturesValid = true;

                {
                    char* dirNames[] = {
                        "TestFarm",
                        "TestFarm/LinkFarmNot",
                        "TestFarm/This Is A Spaced Dir",
                        "TestFarm/EdgeNames",
/*
                        "TestFarm/EdgeNames/D:\\colon_backslash",
                        "TestFarm/EdgeNames/D:colon",
*/
                        "TestFarm/LinkFarm",
                        "TestFarm/LinkFarm/ThisIsAnActualDirectory",
                        "TestFarm/LongNames",
/*
                        "TestFarm/LongNames/DirectoryWithALongNameOf_255_Characters_e123456789f123456789g123456789h123456789i123456789j123456789a123456789b123456789c123456789d123456789e123456789f123456789g123456789h123456789i123456789j123456789a123456789b123456789c123456789d123456789e123456789f1234",
                        "TestFarm/LongNames/DirectoryWithALongNameOf_255_Characters_e123456789f123456789g123456789h123456789i123456789j123456789a123456789b123456789c123456789d123456789e123456789f123456789g123456789h123456789i123456789j123456789a123456789b123456789c123456789d123456789e123456789f1234/SecondDirWithALargeNameOf_255_Characters_123456789f123456789g123456789h123456789i123456789j123456789a123456789b123456789c123456789d123456789e123456789f123456789g123456789h123456789i123456789j123456789a123456789b123456789c123456789d123456789e123456789f1234",
                        "TestFarm/LongNames/DirectoryWithALongNameOf_255_Characters_e123456789f123456789g123456789h123456789i123456789j123456789a123456789b123456789c123456789d123456789e123456789f123456789g123456789h123456789i123456789j123456789a123456789b123456789c123456789d123456789e123456789f1234/SecondDirWithALargeNameOf_255_Characters_123456789f123456789g123456789h123456789i123456789j123456789a123456789b123456789c123456789d123456789e123456789f123456789g123456789h123456789i123456789j123456789a123456789b123456789c123456789d123456789e123456789f1234/ThirdDirectoryWithALongNameOf_255_Characters_56789f123456789g123456789h123456789i123456789j123456789a123456789b123456789c123456789d123456789e123456789f123456789g123456789h123456789i123456789j123456789a123456789b123456789c123456789d123456789e123456789f1234",
*/
                        "TestTooMany",
                        "TestTooMany/HereAreTooMany",
                        "TestTooMany/HereAreTooMany/sub",
                        0
                    };

                    for ( int i = 0; dirNames[i] && _fixturesValid; i++ )
                    {
                        std::string dirName = _testDir + "/" + dirNames[i];
                        int rc = mkdir( dirName.c_str(), S_IRWXU | S_IRWXG );
                        TSM_ASSERT_EQUALS( "mkdir()", rc, 0 );
                        if ( rc == 0 )
                        {
                            _cleanupList.push( dirName );
                        }
                        else
                        {
                            _fixturesValid = false;
                        }
                    }
                }

                if ( _fixturesValid )
                {
                    char* fileNames[] = {
                        "TestFarm/LinkFarmNot/ALinkedFileThatShouldBeCovered.txt",
                        "TestFarm/This Is A Spaced Dir/spaced file.txt",
                        "TestFarm/This Is A Spaced Dir/one.txt",
/*
                        "TestFarm/EdgeNames/D:colon/base.txt",
                        "TestFarm/EdgeNames/C:\\colon_backslash.txt",
                        "TestFarm/EdgeNames/C:colon.txt",
*/
                        "TestFarm/EdgeNames/one.txt",
                        "TestFarm/LinkFarm/ASingleFileAtTheTop.txt",
                        "TestFarm/LinkFarm/ThisIsAnActualDirectory/FileThatIsNormalAndNotALink.txt",

                        "TestTooMany/RealFileUpHere.txt",
                        "TestTooMany/HereAreTooMany/RealFile.txt",
                        0
                    };

                    for ( int i = 0; fileNames[i] && _fixturesValid; i++ )
                    {
                        std::string fileName = _testDir + "/" + fileNames[i];
                        int fh = open( fileName.c_str(), O_RDWR | O_CREAT, S_IREAD | S_IWRITE | S_IRGRP | S_IWGRP );
                        TSM_ASSERT_DIFFERS( "open()", fh, -1 );
                        if ( fh != -1 )
                        {
                            vpstrncpy( scratch, fileName.c_str(), sizeof(scratch) );
                            write( fh, "\n\n", 2 );
                            write( fh, scratch, strlen(scratch) );
                            write( fh, "\n\n", 2 );
                            close( fh );
                            _cleanupList.push( fileName );
                        }
                        else
                        {
                            _fixturesValid = false;
                        }
                    }
                }

                if ( _fixturesValid )
                {
                    char* linkSets[] = {
                        "TestFarm/LinkFarm/DanglingLink", "ThereIsNoSuchFileAroundHereAtAll.zzz",
                        "TestFarm/LinkFarm/LinkToOutsideFile.txt", "../LinkFarmNot/ALinkedFileThatShouldBeCovered.txt",
                        "TestFarm/LinkFarm/ThisIsAnActualDirectory/AbsoluteLinkBackUp", "~/TestFarm/LinkFarm",
                        "TestFarm/LinkFarm/ThisIsAnActualDirectory/FileLinkToTheNormalFileRightHere.txt", "FileThatIsNormalAndNotALink.txt",
                        "TestFarm/LinkFarm/ThisIsAnActualDirectory/RelativeLinkBackUp", "../../LinkFarm",
                        "TestFarm/LinkFarm/ThisIsAnActualDirectory/TrickySideLinkCircular", "../TrickyLinkToTheActualDirectory",
                        "TestFarm/LinkFarm/TrickyLinkToTheActualDirectory", "ThisIsAnActualDirectory",
                        "TestTooMany/HereAreTooMany/bounce", "../HereAreTooMany",
                        "TestTooMany/HereAreTooMany/link00", "RealFile.txt",
                        "TestTooMany/HereAreTooMany/link01", "link00",
                        "TestTooMany/HereAreTooMany/link02", "link01",
                        "TestTooMany/HereAreTooMany/link03", "link02",
                        "TestTooMany/HereAreTooMany/link04", "link03",
                        "TestTooMany/HereAreTooMany/link05", "link04",
                        "TestTooMany/HereAreTooMany/link06", "link05",
                        "TestTooMany/HereAreTooMany/link07", "link06",
                        "TestTooMany/HereAreTooMany/link08", "link07",
                        "TestTooMany/HereAreTooMany/link09", "link08",
                        "TestTooMany/HereAreTooMany/link10", "link09",
                        "TestTooMany/HereAreTooMany/link11", "link10",
                        "TestTooMany/HereAreTooMany/link12", "link11",
                        "TestTooMany/HereAreTooMany/link13", "link12",
                        "TestTooMany/HereAreTooMany/link14", "link13",
                        "TestTooMany/HereAreTooMany/link15", "link14",
                        "TestTooMany/HereAreTooMany/link16", "link15",
                        "TestTooMany/HereAreTooMany/link17", "link16",
                        "TestTooMany/HereAreTooMany/link18", "link17",
                        "TestTooMany/HereAreTooMany/link19", "link18",
                        "TestTooMany/HereAreTooMany/linklink", "linkup",
                        "TestTooMany/HereAreTooMany/linkup2", "../RealFileUpHere.txt",
                        "TestTooMany/HereAreTooMany/linkupReal", "../../TestTooMany/HereAreTooMany/RealFile.txt",
                        "TestTooMany/HereAreTooMany/linkup", "../../TestTooMany/HereAreTooMany/link01",
                        "TestTooMany/HereAreTooMany/linkupup", "../HereAreTooMany/linkup",
                        "TestTooMany/HereAreTooMany/sub/bounce", "../..",
                        0,0
                    };

                    for ( int i = 0; linkSets[i] && _fixturesValid; i += 2 )
                    {
                        std::string linkName = _testDir + "/" + linkSets[i];
                        std::string linkVal;
                        if ( linkSets[i+1][0] == '~' )
                        {
                            linkVal = _testDir + "/" + (linkSets[i+1]+1);
                        }
                        else
                        {
                            linkVal = linkSets[i+1];
                        }
                        int rc = symlink( linkVal.c_str(), linkName.c_str() );
                        TSM_ASSERT_EQUALS( "symlink()", rc, 0 );
                        if ( rc == 0 )
                        {
                            _cleanupList.push( linkName );
                        }
                        else
                        {
                            _fixturesValid = false;
                        }
                    }
                }
            }
            else
            {
                TS_WARN("No temp dir created");
            }
        }
        TS_ASSERT(_fixturesValid);
    }

    virtual ~testFileUtils()
    {
        if ( _testDir.length() > 0 )
        {
            struct stat st = {0};
            int val = stat( _testDir.c_str(), &st );
            if ( val == 0 )
            {
                while ( !_cleanupList.empty() )
                {
                    std::string name = _cleanupList.top();
                    _cleanupList.pop();
                    remove( name.c_str() );
                }
            }
        }
    }

    static testFileUtils *createSuite()
    {
        return new testFileUtils();
    }

    static void destroySuite( testFileUtils *suite )
    {
        delete suite;
    }


// --------------------------------------------------



    void testFixtureStub(void)
    {
        // A simple stub to fix counts
        TS_ASSERT(true);
    }


    bool isPathResolvedSame( std::string& orig )
    {
        bool same = true;

        char scratch[PATH_MAX] = {0};
        std::string path = orig;
        int rc = savFL::FileUtils::resolvePath( path, true );
        TSM_ASSERT_EQUALS( "resolvePath()", rc, 0 );
        same &= (rc == 0);

        char* other = realpath( path.c_str(), scratch );
        //int ourErr = errno;
        TS_ASSERT( other != NULL );
        if ( other )
        {
            TS_ASSERT_EQUALS( std::string(other), path );
            same &= (std::string(other) == path);
        }
        else
        {
            same = false;
        }


        char* canon = canonicalize_file_name( orig.c_str() );
        TS_ASSERT( canon != NULL );
        if ( canon )
        {
            TS_ASSERT_EQUALS( std::string(canon), path );
            same &= ( std::string(canon) == path );
        }
        else
        {
            same = false;
        }

//         TS_TRACE(orig);
//         TS_TRACE(path);
//         if ( other ) {
//             TS_TRACE(std::string(other));
//         } else {
//             TS_TRACE(std::string("XXX - no realpath()"));
//         }
//         if ( canon ) {
//             TS_TRACE(std::string(canon));
//         } else {
//             TS_TRACE(std::string("XXX - no canonicalized()"));
//         }
//         TS_TRACE("");

        return same;
    }


    void testSpecials(void)
    {
        TS_ASSERT(_fixturesValid);
        if ( _fixturesValid )
        {
            char* cases[] = {
                "/",
                ".",
                "./",
                "..",
                "../",
                0
            };

            for ( int i = 0; cases[i]; i++ )
            {
                std::string path( cases[i] );
                bool matchesRealpath = isPathResolvedSame( path );
                TSM_ASSERT( path, matchesRealpath );
            }
        }
    }

    void testBase(void)
    {
        TS_ASSERT(_fixturesValid);
        if ( _fixturesValid )
        {
            char* cases[] = {
                "TestTooMany/HereAreTooMany/link04",
                "TestFarm/LinkFarm/LinkToOutsideFile.txt",
                "TestFarm/LinkFarm/../LinkFarmNot/ALinkedFileThatShouldBeCovered.txt",
                "TestFarm/LinkFarm/TrickyLinkToTheActualDirectory",
                "TestTooMany/HereAreTooMany/../HereAreTooMany/link04",
                "TestFarm/LinkFarm/../LinkFarm/LinkToOutsideFile.txt",
                "TestFarm/LinkFarm/../LinkFarm/TrickyLinkToTheActualDirectory",
                "TestFarm/LinkFarm/../LinkFarm/TrickyLinkToTheActualDirectory/../../../../../../../../../../../../../../../../../../../../../../..",
                "TestTooMany/HereAreTooMany/linkup",
                "TestTooMany/HereAreTooMany/linklink",
                "TestTooMany/HereAreTooMany/linkupup",
                "TestTooMany/HereAreTooMany/linkupReal",
                "TestTooMany/HereAreTooMany/linkup2",
                "TestTooMany/HereAreTooMany/bounce/../RealFileUpHere.txt",
                "TestTooMany/HereAreTooMany/sub/bounce/../TestTooMany/RealFileUpHere.txt",
                "TestTooMany/HereAreTooMany/sub/bounce/../..",
                0
            };

            for ( int i = 0; cases[i]; i++ )
            {
                std::string path = _testDir + "/" + cases[i];
                bool matchesRealpath = isPathResolvedSame( path );
                TSM_ASSERT( path, matchesRealpath );
            }
        }
    }

    void testBad(void)
    {
        TS_ASSERT(_fixturesValid);
        if ( _fixturesValid )
        {
            char* cases[] = {
                "TestFarm/LinkFarm/DanglingLink",
                "TestFarm/LinkFarm/../LinkFarm/DanglingLink/../../../../../../../../../../../../../../../../../../../../../../../../../../../../..",
                0
            };

            for ( int i = 0; cases[i]; i++ )
            {
//                 char scratch[PATH_MAX] = {0};
                std::string path = _testDir + "/" +  cases[i];
                int rc = savFL::FileUtils::resolvePath( path, true );
                TSM_ASSERT_DIFFERS( "resolvePath()", rc, 0 );

//                 char* other = realpath( cases[i], scratch );
                //int ourErr = errno;

//                 TS_TRACE(std::string(cases[i]));
//                 TS_TRACE(path);
//                 if ( other ) {
//                     TS_TRACE(std::string(other));
//                 } else {
//                     TS_TRACE(std::string("XXX - no realpath()"));
//                 }
//                 TS_TRACE("");
            }
        }
    }
};
