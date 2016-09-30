//
//  IBM AntiVirus Immune System
//
//  File Name:  Util.h
//  Author:   Milosz Muszynski
//
//  Utility Classes
//
//  $Log:  $
//


#ifndef __UTIL__H__
#define __UTIL__H__

struct DirUtil
{
  static bool rename                 (const std::string& dir1, const std::string& dir2);
  static bool make                   (const std::string& dir);
  static bool remove                 (const std::string& dir);
  static bool makeRandomSubdirectory (const std::string& dir, std::string& subDir);
  static bool moveAll                (const std::string& srcDir, const std::string& dstDir);
  static bool removeAll              (const std::string& dir);
  static bool prune                  (const std::string& dir);
  static bool isDOSCompatible        (const std::string& dir);
  static bool isDir                  (const std::string& dir);
};

struct FileUtil
{
  static bool       exists    (const std::string& path);
  static bool       isFile    (const std::string& path);
  static bool       remove    (const std::string& path);
  static bool       allExist  (const std::string& dir, const char **fileList);
  static bool       allRemove (const std::string& dir, const char **fileList);
  static bool       move      (const std::string& oldPath, const std::string& newPath);
};

#endif __UTIL__H__
