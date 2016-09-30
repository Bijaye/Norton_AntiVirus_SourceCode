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

//------ pragmas ------------------------------------------------------
#pragma warning ( disable : 4786 ) // identifier truncated

//------ standard C++ library includes --------------------------------
#include <string>
#include <vector>

//------ custom includes ----------------------------------------------
#include "ZDirectory.h"
#include "StrVector.h"
#include "Util.h"
#include "StrUtil.h"


//------ methods of the utility classes -------------------------------

//
// DirUtil::makeNew
//

bool DirUtil::makeRandomSubdirectory( const std::string& dir, std::string& subDir  )
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

	bool success = make( s );

	if ( success )
		subDir = s;

	return success;
}


//
// DirUtil::make
//

bool DirUtil::make( const std::string& dir )
{
	int result = 0;

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
				result = _mkdir( s.c_str() );
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
				}
			}
		}
 	}

	return result == 0;
}


//
// DirUtil::remove
//

bool DirUtil::remove( const std::string& dir )
{
	int result = _rmdir( dir.c_str() );
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

bool DirUtil::moveAll( const std::string& srcDir, const std::string& dstDir )
{
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
				if ( !FileUtil::move( src + fileName, dst + fileName ) )
					success = false;
			}
		}
		while ( dir.getNext( fileName ) );
	}

	dir.findClose();


    return success;
}


//
// DirUtil::prune
//

bool DirUtil::prune( const std::string& dir )
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
		success = false;

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
// DirUtil::isDOSComptible
//

bool DirUtil::isDOSCompatible( const std::string& dir )
{
	char buffer[ MAX_PATH ];  

	DWORD rc = GetShortPathName( dir.c_str(), buffer, sizeof( buffer ) );

	bool compatible = false;

	if ( rc != 0 )
	{
		std::string shortName( buffer );
		compatible = ( shortName == dir );
	}

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


//
// FileUtil::exists
//

bool FileUtil::exists( const std::string& path )
{
    int result = _access( path.c_str(), 0 );

    return result != -1;
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

bool FileUtil::remove( const std::string& path )
{
	int result = ::remove( path.c_str() );

	return result == 0;
}

//
// FileUtil::allExist
//

bool FileUtil::allExist ( const std::string& dir, const char ** fileList )
{
	bool _allExist = true;
	std::string dirPath = dir;
	StrUtil::makeSureEndsWith( dirPath, '\\' );
	for ( int i = 0; _allExist && fileList[ i ] != NULL; i++ )
	{
		std::string s = dirPath + fileList[ i ];
		if ( _access( s.c_str(), 0 ) == -1 )
			_allExist = false;
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

bool FileUtil::move( const std::string& oldPath, const std::string& newPath )
{
	BOOL b = MoveFileEx( oldPath.c_str(), newPath.c_str(),
						 MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
	return b ? true : false;
}


