// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.

#include <string>
#include <errno.h>
#include <sys/stat.h>

// To get base types and macros:
#include "NLM.H"

#include "FileUtils.h"

#ifdef stat
#undef stat
#endif

namespace savFL
{


static int addPart( std::string& dest, std::string& component, bool resolveLinks, int& firstMod )
{
    int rc = 0;

    bool needsDelim = dest.length() > 0 && dest[dest.length() -1] != '/';

    if ( (component == ".")
        || (component == ".." && dest.length() < 1) )
    {
        // Only care if it's the first component
        if ( dest.length() < 1 )
        {
#if !defined(LINUX)
#error "A Linux extension to the POSIX.1 standard allows us to pass NULL"
#endif
            errno = 0;
            char* cwd = getcwd( NULL, 0 );
            if ( cwd != NULL )
            {
                dest = cwd;
                free(cwd);
                cwd = 0;
            }
            else
                rc = errno;
        }
    }

    if ( component == "." )
    {
        // Nothing at this point. Ignore it.
    }
    else if ( component == ".." )
    {
        // Back up one
        if ( firstMod >= 0 )
        {
            dest += needsDelim ? "/.." : "..";
        }
        else
        {
            struct stat st = {0};
            int val = stat( dest.c_str(), &st );
            if ( val == 0 )
            {
                if ( !S_ISDIR(st.st_mode) )
                {
                    rc = ENOTDIR;
                }
            }
            else
            {
                rc = errno;
            }

            if ( !rc )
            {
                int pos = dest.length() - ((needsDelim) ? 1 : 2);
                do
                {
                    pos = dest.rfind('/', pos);
                }
                while ( pos > 1 && dest[pos - 2] == '/');

                if ( pos > 0 )
                {
                    dest.erase( pos );
                }
                else if ( pos == 0 )
                {
                    dest = "/";
                }

                if ( firstMod < 0 || pos < firstMod )
                {
                    firstMod = pos + 1;
                }
            }
        }
    }
    else
    {
        // Add the part
        std::string tmp = (needsDelim) ? dest + '/' + component : dest + component;
        if ( resolveLinks )
        {
            struct stat st = {0};

            int val = lstat( tmp.c_str(), &st );

            if ( val == 0 )
            {
                if ( S_ISLNK(st.st_mode) )
                {
                    char* buf = new(std::nothrow) char[st.st_size + 1];
                    if ( buf )
                    {
                        val = readlink( tmp.c_str(), buf, st.st_size );
                        if ( val >= 0 )
                        {
                            int modPos = -1;
                            buf[val] = 0;
                            if ( buf[0] == '/' )
                            {
                                tmp = buf;
                                modPos = 0;
                            }
                            else
                            {
                                tmp = (needsDelim) ? dest + '/' + buf : dest + buf;
                                modPos = dest.length() + ((needsDelim) ? 1 : 0);
                            }

                            if ( lstat( tmp.c_str(), &st ) != 0 )
                            {
                                rc = errno;
                                tmp = (needsDelim) ? dest + '/' + component : dest + component;
                            }
                            else
                            {
                                if ( firstMod < 0 || modPos < firstMod )
                                {
                                    firstMod = modPos;
                                }
                            }
                        }
                        else
                        {
                            rc = errno;
                        }
                        delete[] buf;
                        buf = 0;
                    }
                    else
                    {
                        rc = ENOMEM;
                    }
                }
            }
            else
            {
                rc = errno;
            }

            dest = tmp;
        }
        else
        {
            dest = tmp;
        }
    }

    return rc;
}

int FileUtils::resolvePath( std::string& path, bool resolveLinks )
{
    int rc = 0;

    // Now try to clean up the dot and dot-dot parts
    int firstMod = 0;
    std::string dest;
    std::string src = path;
    while ( rc == 0 && firstMod >= 0 )
    {
        int cur = firstMod;
        int anchor = firstMod;
        bool escaped = false;

        if ( firstMod < 1 )
        {
            dest.clear();
        }
        else
        {
            dest = src.substr( 0, firstMod );
        }
        firstMod = -1;
        while ( cur < (int)src.length() && rc == 0 ) {
            int ch = src[cur];
            if ( escaped )
            {
                cur++;
                escaped = false;
            }
            else if ( ch == '\\' )
            {
                escaped = true;
                cur++;
            }
            else if ( ch == '/' )
            {
                if ( cur > anchor )
                {
                    std::string component = src.substr(anchor, cur - anchor);
                    rc = addPart( dest, component, resolveLinks, firstMod );
                    anchor = cur + 1;
                }
                else if ( cur == 0 )
                {
                    dest = "/";
                    anchor = cur + 1;
                }
                cur++;
            }
            else
            {
                cur++;
            }
        }

        if ( cur > anchor )
        {
            std::string component = src.substr(anchor, cur - anchor );
            if ( dest.length() < 1 && (component == "/") )
            {
                dest = "/";
            }
            else
            {
                rc = addPart( dest, component, resolveLinks, firstMod );
            }
        }
        if ( dest.length() < 1 )
            rc = EINVAL;
        else
            src = dest;
    }

    path = dest;

    return rc;
}

} // namespace savFL

