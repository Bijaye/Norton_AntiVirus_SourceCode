
#include "stdafx.h"

#include "ClientReg.h"
#include "SymSaferStrings.h"
#include "SettingsCollector.h"

using std::cin;

#if defined(CXXTEST_ENABLED)
#include "T/MRegistry.h"
#define TXX T
#else
#define TXX
#endif


struct Config {
    bool useStdin;
    std::string server;
    std::string user;
    std::string password;
    std::string directory;

    Config() :
        useStdin(false),
        server(),
        user(),
        password(),
        directory()
    {}
};

void ProcessInput( Config& config, int argc, char* argv[] );
int Extract( HKEY baseKey, char const* targetDirectory );
void DisplayError( LONG errCode );

int main(int argc, char* argv[])
{
    Config config;

    HKEY baseKey = (HKEY)0;
    LONG rc = ERROR_SUCCESS;
    char remoteName[MAX_PATH + 1] = {0};

    ProcessInput( config, argc, argv );

    bool useRemoteRegistry = !config.server.empty();
    bool addRemoteConnection = !config.server.empty() && (!config.user.empty() || !config.password.empty());
    bool remoteConnectionAdded = false;

    if ( useRemoteRegistry ) {
        if ( config.server.find("\\\\") != 0 ) {
            ssStrnCpy( remoteName, "\\\\", sizeof(remoteName) );
        } else {
            remoteName[0] = 0;
        }
        ssStrnAppend( remoteName, config.server.c_str(), sizeof(remoteName) );
    }

    // Setup
    if ( addRemoteConnection ) {
        NETRESOURCEA netThing = {0};
        netThing.dwType = RESOURCETYPE_ANY;
        netThing.lpLocalName = NULL;
        netThing.lpProvider = NULL;
        netThing.lpRemoteName = remoteName;

        DWORD dwR = WNetAddConnection2A( &netThing, config.password.c_str(), config.user.c_str(), 0 );
        if ( dwR != NO_ERROR ) {
            // Attempt fall-back with user prompt.
            std::cout.sync_with_stdio();
            std::cout << "[Err:" << dwR << "]" << std::endl;
            std::cout.sync_with_stdio();

            dwR = WNetAddConnection2A( &netThing, config.password.c_str(), config.user.c_str(), CONNECT_INTERACTIVE | CONNECT_COMMANDLINE );
        }
        if ( dwR == NO_ERROR ) {
            remoteConnectionAdded = true;
        } else {
            DisplayError( dwR );
        }
    }

    if ( useRemoteRegistry ) {
        if ( rc == ERROR_SUCCESS ) {
            rc = RegConnectRegistryA( remoteName, HKEY_LOCAL_MACHINE, &baseKey );
            if ( rc != ERROR_SUCCESS ) {
                DisplayError( rc );
            }
            // a timeout when not logged in gave us ERROR_BAD_NETPATH
        }
    } else {
        baseKey = HKEY_LOCAL_MACHINE;
    }

    // Main processing
    if ( rc == ERROR_SUCCESS ) {
        DWORD index = 0;

#if 0
        LONG localRc = ERROR_SUCCESS;
        do {
            char nameBuf[255 + 1] = {0};
            localRc = RegEnumKeyA( baseKey, index, nameBuf, sizeof(nameBuf) );
            if ( localRc == ERROR_SUCCESS ) {
                std::cout << nameBuf << std::endl;
            }
            index++;
        } while ( localRc == ERROR_SUCCESS );
#endif

        rc = Extract( baseKey, config.directory.c_str() );
    }


    // Shutdown
    if ( useRemoteRegistry ) {
        // First close any HEKY we opened
        if ( baseKey != (HKEY)0 ) {
            RegCloseKey( baseKey );
            baseKey = (HKEY)0;
        }
    }

    if ( remoteConnectionAdded ) {
        WNetCancelConnection2A( remoteName, 0, false );
    }

    return (rc == ERROR_SUCCESS) ? 0 : 1;
}


bool ProcessParam( char const param, char const* val, Config& config, int& found )
{
    bool extra = false;

    switch ( param ) {
        case 'i':
        {
            config.useStdin = true;
        }
        break;
        case 's':
        {
            if ( val ) {
                config.server = val;
                found |= 0x01;
                extra = true;
            }
        }
        break;
        case 'u':
        {
            if ( val ) {
                config.user = val;
                found |= 0x02;
                extra = true;
            }
        }
        break;
        case 'p':
        {
            if ( val ) {
                config.password = val;
                found |= 0x04;
                extra = true;
            }
        }
        break;
        case 'o':
        {
            if ( val ) {
                config.directory = val;
                found |= 0x08;
                extra = true;
            }
        }
        break;
    }

    return extra;
}


void ProcessInput( Config& config, int argc, char* argv[] )
{
    char buf[1024] = {0};
    int found = 0;

    int index = 1;
    while ( index < argc ) {
        if ( argv[index] ) {
            if ( argv[index][0] == '-' ) {
                // a param
                if ( argv[index][1] && (argv[index][2] == 0) ) {
                    if ( ProcessParam( argv[index][1], (index + 1 < argc) ? argv[index + 1] : NULL, config, found ) ) {
                        index++;
                    }
                }
            }
        }
        index++;
    }

    while ( config.useStdin
        && ((found & 0x0f) != 0x0f) ) {
        std::cin.getline( buf, sizeof(buf) );
        size_t count = std::cin.gcount();

        if ( count > 1 ) {
            if ( (count > 3) && (buf[0] == '-') && ((buf[2] == ' ') || (buf[2] == '\t')) ) {
                ProcessParam( buf[1], &buf[3], config, found );
            } else if ( (count > 2) && (buf[0] == '-') && (buf[2] == 0) ) {
                ProcessParam( buf[1], NULL, config, found );
            }
        } else {
            // Stop when blank line encountered
            break;
        }
    }
}

void DisplayError( LONG errCode )
{
    char buf[1024] = {0};
    FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM, NULL, errCode, 0, buf, sizeof(buf), 0);
    std::cout << buf;
}

int Extract( HKEY baseKey, char const* targetDirectory )
{
    LONG rc = ERROR_SUCCESS;
#if defined(_WINDOWS)
    _mkdir( targetDirectory );
#else // defined(_WINDOWS)
    mkdir( targetDirectory, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH );
#endif // defined(_WINDOWS)


    HKEY hKey = 0;
#if !defined(LINUX)
    const char* rootName = szReg_Key_Main;
#else
    const char* rootName = "Software\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion";
#endif // !defined(LINUX)
    rc = TXX::RegOpenKeyExA( baseKey, rootName, 0, KEY_READ, &hKey );

    if ( rc == ERROR_SUCCESS ) {
        SettingsCollector* collector = SettingsCollectorFactory::CreateCollector();
        if ( collector ) {
            collector->Collect( hKey, targetDirectory );
            delete collector;
            collector = 0;
        }

        TXX::RegCloseKey(hKey);
        hKey = 0;
    }

    return rc;
}
