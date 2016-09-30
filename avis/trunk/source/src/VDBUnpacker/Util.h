//
//	IBM AntiVirus Immune System
//
//	File Name:	Util.h
//	Author:		Milosz Muszynski
//
//	Utility Classes
//
//	$Log:  $
//


#ifndef __UTIL_DOT_H__
#define __UTIL_DOT_H__

struct DirUtil
{
	static bool			 	rename					( const std::string& dir1, const std::string& dir2 );
	static bool			 	make					( const std::string& dir, void * traceClient = NULL );
	static bool			 	remove					( const std::string& dir, void * traceClient = NULL );
	static bool				makeRandomSubdirectory	( const std::string& dir, std::string& subDir, void * traceClient = NULL );	  
	static bool				moveAll					( const std::string& srcDir, const std::string& dstDir, void * traceClient = NULL );
	static bool				removeAll				( const std::string& dir );
	static bool				prune					( const std::string& dir, void * traceClient = NULL );
	static bool				isDOSCompatible			( const std::string& dir, void * traceClient = NULL );
  static bool				isDir	  	( const std::string& path );
  static bool       renameFileMakingDirs(const std::string &from, const std::string &to, void * traceClient = NULL);
};

struct FileUtil
{
	static bool			 	exists		( const std::string& path, void * traceClient = NULL );
	static bool				isFile		( const std::string& path );
	static bool				remove		( const std::string& path, void * traceClient = NULL );
	static bool			 	allExist	( const std::string& dir, const char ** fileList, void * traceClient = NULL );
	static bool				allRemove	( const std::string& dir, const char ** fileList );
	static bool				move        ( const std::string& oldPath, const std::string& newPath, void * traceClient );
};

struct StrUtil
{
	static void				makeSureEndsWith	( std::string& s1, char c );
	static bool				contains			( const std::string& s, const std::string& s1 );
	static bool				startsWith			( const std::string& str, const std::string& str2 );
};

#endif /* __UTIL_DOT_H__ */
