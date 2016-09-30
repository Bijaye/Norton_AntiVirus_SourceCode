// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.

#ifndef FILE_UTILS_H_INCLUDED
#define FILE_UTILS_H_INCLUDED

namespace savFL
{

class FileUtils
{
public:

    static int resolvePath( std::string& path, bool resolveLinks );

private:
    FileUtils();
    virtual ~FileUtils();

    // Forbid default copying and assignment
    FileUtils( const FileUtils& );
    FileUtils& operator=( const FileUtils& );
};


} // namespace savFL

#endif // FILE_UTILS_H_INCLUDED
