//
//	IBM AntiVirus Immune System
//
//	File Name:	Util.cpp
//	Author:		Milosz Muszynski
//
//	Utility Classes
//
//	$Log:  $
//


//------ pragmas ------------------------------------------------------
#pragma warning( disable : 4786 ) // identifier truncated warning 

//------ windows includes ---------------------------------------------
#include "windows.h"

//------ run time library includes ------------------------------------
#include <direct.h>
#include <io.h>
#include <time.h>
#include <stdlib.h>
#include <strstrea.h>
#include <sys/types.h>
#include <sys/stat.h>

//------ standard C++ library includes --------------------------------
#include <string>
#include <vector>


//------ custom includes ----------------------------------------------
#include "ZDirectory.h"
#include "StrVector.h"
#include "Util.h"
#include "MyTraceClient.h"


//------ methods of the utility classes -------------------------------

//
// DirUtil::makeNew
//

bool DirUtil::makeRandomSubdirectory( const std::string& dir, std::string& subDir, void * traceClient )
{
	srand( (unsigned int)time( NULL ) );

	std::string( s );

	std::string dirPath = dir;

	StrUtil::makeSureEndsWith( dirPath, '\\' );

	do
	{
		int x = rand();
		ostrstream os;
		os << x << ends;
		s = dirPath + os.str();
	}
	while ( FileUtil::exists( s ) );

	bool success = make( s, traceClient );

	if ( success )
		subDir = s;

	return success;
}


//
// DirUtil::make
//

bool DirUtil::make( const std::string& dir, void * traceClient )
{
	MyTraceClient * _tc = static_cast<MyTraceClient*>( traceClient );

	int result = 0;

  /*
   * Quick sanity exit, added inw 2000-02-11.
   */
  if (dir.length() == 0)
    return false;

	StrVector v( dir, "\\" );

	std::string s;

	if ( !StrUtil::startsWith( dir, "\\\\" ) )
	{
		for ( int i = 0; result == 0 && i < v.size(); i++ )
		{
			if ( i != 0 )
			{
				s  += "\\";
			}
			else
			{
				if ( dir[ 0 ] == '\\' )
					s = "\\";
			}

			s += v[ i ];
			if ( !FileUtil::exists( s ) )
			{
				result = _mkdir( s.c_str() );
				if (_tc) _tc->debug( "VDBUnpacker: %s %s", result == 0 ? "created" : "could not create", s.c_str() );
			}
		}
	}
	else
	{
		s = "\\\\";
		for ( int i = 0; result == 0 && i < v.size(); i++ )
		{
			s += v[ i ] + "\\";

			if ( i > 1 )
			{
				if ( !FileUtil::exists( s ) )
				{
					result = _mkdir( s.c_str() );
					if (_tc) _tc->debug( "VDBUnpacker: %s %s", result == 0 ? "created" : "could not create", s.c_str() );
				}
			}
		}
 	}

	return result == 0;
}


//
// DirUtil::remove
//

bool DirUtil::remove( const std::string& dir, void * traceClient )
{
	MyTraceClient * _tc = static_cast<MyTraceClient*>( traceClient );

	int result = _rmdir( dir.c_str() );

	if (_tc) _tc->debug( "VDBUnpacker: %s %s", result == 0 ? "removed" : "could not remove", dir.c_str() );

	return result == 0;
}


//
// DirUtil::rename
//

bool DirUtil::rename( const std::string& dir1, const std::string& dir2 )
{
    int result = ::rename( dir1.c_str(), dir2.c_str() );

    return result == 0;
}


//
// DirUtil::moveAll
//

bool DirUtil::moveAll( const std::string& srcDir, const std::string& dstDir, void * traceClient )
{
	MyTraceClient * _tc = static_cast<MyTraceClient*>( traceClient );

	bool success = true;

	std::string src = srcDir;
	std::string dst = dstDir;

	StrUtil::makeSureEndsWith( src, '\\' );
	StrUtil::makeSureEndsWith( dst, '\\' );

	ZDirectory dir( src );

	std::string fileName;

	if ( dir.getFirst( "*.*", fileName ) )
	{
		do
		{
			if ( dir.isNormalFile() )
			{
				if ( !FileUtil::move( src + fileName, dst + fileName, _tc ) )
					success = false;
			}
		}
		while ( dir.getNext( fileName ) );
	}

	dir.findClose();

	if (_tc) _tc->debug( "VDBUnpacker: %s from %s to %s", success ? "files moved" : "could not move files", srcDir.c_str(), dstDir.c_str() );

    return success;
}


//
// DirUtil::prune
//

bool DirUtil::prune( const std::string& dir, void * traceClient )
{
	MyTraceClient * _tc = static_cast<MyTraceClient*>( traceClient );

	bool success = true;

	std::string src = dir;

	StrUtil::makeSureEndsWith( src, '\\' );

	ZDirectory _dir( src );

	std::string fileName;

	if ( _dir.getFirst( "*.*", fileName ) )
	{
		do
		{
			if ( _dir.isNormalFile() )
			{
				if ( !FileUtil::remove( src + fileName ) )
				{
					success = false;
					if (_tc) _tc->debug( "VDBUnpacker: errno = %d, %s %s", errno, (src+fileName).c_str(), errno == EACCES ? "is read only" : "" );
				}
			}
			else if ( _dir.isDirectory() )
			{
				if ( !DirUtil::prune( src + fileName ) )
					success = false;
			}
		}
		while ( _dir.getNext( fileName ) );
	}

	_dir.findClose();

	if ( !DirUtil::remove( dir ) )
	{
		success = false;
		if (_tc) _tc->debug( "VDBUnpacker: errno = %d, %s %s", errno, dir.c_str(), errno == EACCES ? "is read only" : "" );
	}

	if (_tc) _tc->debug( "VDBUnpacker: %s %s", dir.c_str(), success ? "pruned" : "could not be pruned" );

    return success;
}


//
// DirUtil::removeAll
//

bool DirUtil::removeAll( const std::string& dir )
{
	bool success = true;

	std::string src = dir;

	StrUtil::makeSureEndsWith( src, '\\' );

	ZDirectory _dir( src );

	std::string fileName;

	if ( _dir.getFirst( "*.*", fileName ) )
	{
		do
		{
			if ( _dir.isNormalFile() )
			{
				if ( !FileUtil::remove( src + fileName ) )
					success = false;
			}
		}
		while ( _dir.getNext( fileName ) );
	}

	_dir.findClose();


    return success;
}


//
// FileUtil::isDOSComptible
//

bool DirUtil::isDOSCompatible( const std::string& dir, void * traceClient )
{
	MyTraceClient * _tc = static_cast<MyTraceClient*>( traceClient );

	char buffer[ MAX_PATH ];  

	DWORD rc = GetShortPathName( dir.c_str(), buffer, sizeof( buffer ) );

	bool compatible = false;

	if ( rc != 0 )
	{
		std::string shortName( buffer );
		compatible = ( shortName == dir );
	}

	if ( StrUtil::contains( dir, "\\\\" ) )
		compatible = false;

	if ( _tc ) _tc->debug( "VDBUnpacker: %s %s DOS compatible", dir.c_str(), compatible ? "is" : "is not" );

	return compatible;
}

//
// DirUtil::isDir
//

bool DirUtil::isDir(const std::string& dir)
{
  struct stat statBuf;

  if (! FileUtil::exists(dir))
    return false;

  if (-1 == stat(dir.c_str(), &statBuf))
    return false;

  return ((statBuf.st_mode & _S_IFDIR) ? true : false);
}

/*
 * DirUtil::renameFileMakingDirs
 *
 * Allows the caller to move a file without having to faff
 * about worrying that the destination directory tree already
 * exists.  The destination file need not, of course,
 * have any directory components at all, in which case the
 * file will be moved into the current working directory.
 * This is probably not what you wanted to happen.
 *
 * Returns false if it fails, true if it doesn't.
 *
 * inw 2000-02-11
 */
bool DirUtil::renameFileMakingDirs(const std::string &from, const std::string &to, void *traceClient)
{
  int         q     = 0;
  std::string toTwo = to;

  /*
   * Sanity check that we didn't get some empty strings...
   */
  if ((from.size() == 0) || (to.size() == 0))
    return false;

  /*
   * Turn all the forward slashes into backslashes.
   * This happens all the time with archives, and it's
   * only a matter of time before it happens to
   * a .VDB package, and it would be a shame if it
   * caught us out.  So, flip 'em.
   */
  while ((q = toTwo.find('/')) != std::string::npos)
    toTwo.replace(q, 1, "\\");

  /*
   * Make sure that the source exists, and is a file.
   */
  if (! FileUtil::isFile(from))
    return false;

  /*
   * Look for the _last_ slash in the destination name.
   * If one is found, then take the section of the string
   * before it, and pass it to make(), which will ensure
   * that the directory tree exists.
   *
   * If there isn't a \ in the name, then skip the directory
   * making business.  If it's at the beginning, skip it
   * as well.
   */
  q = toTwo.rfind('\\');
  if ((q != std::string::npos) && (q != 0))
    if (! DirUtil::make(toTwo.substr(0, q)))
      return false;

  /*
   * Now execute the rename, and return whatever it says.
   */
  return DirUtil::rename(from, to);
}

//
// FileUtil::exists
//

bool FileUtil::exists( const std::string& path, void * traceClient )
{
	MyTraceClient * _tc = static_cast<MyTraceClient*>( traceClient );

    int result = _access( path.c_str(), 0 );

	bool success = ( result != -1 );

	if (_tc) _tc->debug( "VDBUnpacker: %s %s", path.c_str(), success ? "exists" : "does not exist" );

    return success;
}

//
// FileUtil::isFile
//

bool FileUtil::isFile( const std::string& path )
{
  struct stat statBuf;

  if (! FileUtil::exists(path))
    return false;

  if (-1 == stat(path.c_str(), &statBuf))
    return false;

  return ((statBuf.st_mode & _S_IFREG) ? true : false);
}

//
// FileUtil::remove
//

bool FileUtil::remove( const std::string& path, void * traceClient )
{
	MyTraceClient * _tc = static_cast<MyTraceClient*>( traceClient );

	int result = ::remove( path.c_str() );

	if (_tc) _tc->debug( "VDBUnpacker: %s %s", result == 0 ? "removed" : "could not remove", path.c_str() );

	if (result == -1)
	{
		if (_tc) _tc->debug( "VDBUnpacker: errno = %d%s", errno, errno == EACCES ? ", file is read only" : "" );
	}

	return result == 0;
}

//
// FileUtil::allExist
//

bool FileUtil::allExist ( const std::string& dir, const char ** fileList, void * traceClient )
{
	MyTraceClient * _tc = static_cast<MyTraceClient*>( traceClient );

	bool _allExist = true;
	std::string dirPath = dir;
	StrUtil::makeSureEndsWith( dirPath, '\\' );
	for ( int i = 0; _allExist && fileList[ i ] != NULL; i++ )
	{
		std::string s = dirPath + fileList[ i ];
		bool exists = (_access( s.c_str(), 0 ) != -1);
		if ( !exists )
			_allExist = false;
		if (_tc) _tc->debug( "VDBUnpacker: %s %s", s.c_str(), exists ? "exists" : "does not exist" );
	}
	return _allExist;
}

//
// FileUtil::allRemove
//

bool FileUtil::allRemove ( const std::string& dir, const char ** fileList )
{
	bool allRemoved = true;
	std::string dirPath = dir;
	StrUtil::makeSureEndsWith( dirPath, '\\' );
	for ( int i = 0; fileList[ i ] != NULL; i++ )
	{
		std::string s = dirPath + fileList[ i ];
		if ( ::remove( s.c_str() ) == 0 )
			allRemoved = false;
	}
	return allRemoved;
}

//
// FileUtil::move
//

bool FileUtil::move( const std::string& oldPath, const std::string& newPath, void * traceClient )
{
	MyTraceClient * _tc = static_cast<MyTraceClient*>( traceClient );

	BOOL b = MoveFileEx( oldPath.c_str(), newPath.c_str(),
						 MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );

//	if ( b )
//	{
//		if (_tc) _tc->debug( "VDBUnpacker: moved %s to %s", oldPath.c_str(), newPath.c_str() );
//	}
//	else 
	if ( !b )
	{
		DWORD e = GetLastError();
		if (_tc) _tc->debug( "VDBUnpacker: could not move %s to %s, error: %8.8x ", oldPath.c_str(), newPath.c_str(), e );
	}

	return b ? true : false;
}

//
// StrUtil::makeSureEndsWith
//

void StrUtil::makeSureEndsWith( std::string& s1, char c )
{
	char b[ 2 ];
	b[ 0 ] = c;
	b[ 1 ] = '\0';
	if ( s1.length() == 0 )
	{
		s1 = b;
	}
	else
	{
		if ( s1[ s1.length() - 1 ] != c )
			s1 += b;
	}
}

//
// contains
//
bool StrUtil::contains( const std::string& s, const std::string& s1 )
{
	int pos = s.find( s1 );
	return pos != std::string::npos;
}

//
// startsWith
//
bool StrUtil::startsWith		( const std::string& str, const std::string& str2 )
{
	int pos = str.find( str2 );
	return pos == 0;
}
