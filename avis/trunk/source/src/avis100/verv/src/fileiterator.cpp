// see the following header file for details
#include "FileIterator.hpp"

#include "avfileio.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

const char PATH_DELIMITER = '\\';



static const char* getNameFromPath (const char* zPath) {
   const char* pName = strrchr(zPath, PATH_DELIMITER);
   return pName? pName+1 : zPath;
}



static bool strlenNoMoreThan (const char* str, size_t len) {
   if (str)
      for (int i = 0; i <= len; i++) 
         if (!str[i]) 
            return true;
   return false;
}



static bool validExtension (const char* zFileName) {
   const char* ext = strrchr(zFileName, '.');
   if (!ext)   ext = strrchr(zFileName, '_');
   if (!ext) return false;
   return !stricmp(ext,".COM") || !stricmp(ext,".EXE") 
       || !stricmp(ext,".VOM") || !stricmp(ext,".VXE")
       || !stricmp(ext,"_COM") || !stricmp(ext,"_EXE");
}



static bool getName (char* zName, int len, FILE* pFile) {
   int readLength;

   fgets (zName, len, pFile);
   if (feof(pFile) || ferror(pFile)) {
      if (ferror(pFile)) perror ("read error");
      zName[0] = '\0';
      return false;
   }

   readLength = strlen(zName);
   while (readLength && isspace(zName[readLength-1])) {
      zName[--readLength] = '\0';
   }
   if (readLength > _MAX_PATH) {
      fprintf (stderr, "name too long: %s\n", zName);
      zName[0] = '\0';
      return false;
   }
   return true;
}



static bool makeHostName(char* zName, const char* zHostList) {
   char  zFileName[_MAX_PATH+2];
   char* pFname = _MAX (strrchr(zName,'\\'), zName);
   strncpy(zFileName, pFname, sizeof(zFileName));
   strcpy (zName,zHostList);
   strcat (zName, "\\");
   strcat (zName, zFileName);
   return true;
}



bool FileIterator::initFromList (const char* zHostList, const char* zInfectedList) {
   FILE* fHost;
   FILE* fInfected;
   const char mode[] = "r";
   char zName[_MAX_PATH+2];

   assert (strlenNoMoreThan(zHostList,_MAX_PATH) && strlenNoMoreThan(zInfectedList,_MAX_PATH));

   fHost = fopen (zHostList, mode);
   if (!fHost && errno != EACCES) {   // if EACCES assume it's a directory
      perror ("host list open");
      return false;
   }

   fInfected = fopen(zInfectedList, mode);
   if (!fInfected) {
      perror ("infected list open");
      if (fHost) fclose(fHost);
      return false;
   }

   while (getName(zName, sizeof(zName), fInfected)) {
      InfectedNames.push_back(zName);
      fHost ? getName(zName, sizeof(zName), fHost) : makeHostName(zName,zHostList);
      HostNames.push_back(zName);
   }

   if (ferror(fInfected) || (fHost && ferror(fHost))) {
      if (ferror(fInfected)) fprintf(stderr, "%s: read error\n", zInfectedList);
      if (fHost && ferror(fHost))     fprintf(stderr, "%s: read error\n", zHostList);
      fclose(fInfected);
      if (fHost) fclose(fHost);
      return false;
   }
   fclose(fInfected);
   if (fHost) fclose(fHost);
   return true;
}



bool FileIterator::initFromDir  (const char* zHostDir, const char* zInfectedDir) {
   FILE* pHost;
   DIR* dir;
   struct dirent* direntry;
   string hostDir, infectedDir;
   string hostPath, infectedPath;

   assert (strlenNoMoreThan(zHostDir,_MAX_PATH) && strlenNoMoreThan(zInfectedDir,_MAX_PATH));

   hostDir = zHostDir;
   if (hostDir[hostDir.size()-1] != PATH_DELIMITER) hostDir += PATH_DELIMITER;
   infectedDir = zInfectedDir;
   if (infectedDir[infectedDir.size()-1] != PATH_DELIMITER) infectedDir += PATH_DELIMITER;
 
   if (!(dir = opendir(zInfectedDir))) {
      fprintf(stderr, "unable to open infected directory %s\n", zInfectedDir);
      return false;
   }
   while (direntry = readdir(dir)) {
      if (validExtension(direntry->d_name)) {

         infectedPath = infectedDir + direntry->d_name;
         hostPath = hostDir + direntry->d_name;

         if (infectedPath.size() > _MAX_PATH || hostPath.size() > _MAX_PATH) {
            if (infectedPath.size() > _MAX_PATH)fprintf(stderr, "name too long: %s", infectedPath.c_str());
            if (hostPath.size() > _MAX_PATH)    fprintf(stderr, "name too long: %s", hostPath.c_str());
            closedir(dir);
            return false;
         }

         if (!(pHost = fopen (hostPath.c_str(), "rb"))) {
            hostPath = "";
         }else fclose(pHost);
            
         InfectedNames.push_back(infectedPath);
         HostNames.push_back(hostPath);
      }
   }
   closedir(dir);
   return true;
}



size_t FileIterator::count (void) const {
   assert(InfectedNames.size() == HostNames.size());
   return InfectedNames.size();
}



bool FileIterator::infectedFileExists (size_t n) const {
   return n < InfectedNames.size() && InfectedNames[n].size() > 0;
}



bool FileIterator::hostFileExists (size_t n) const {
   return n < HostNames.size() && HostNames[n].size() > 0;
}



const char* FileIterator::getInfectedPath (size_t n) const {
   assert (n < InfectedNames.size());
   return InfectedNames[n].c_str();
}



const char* FileIterator::getHostPath (size_t n) const {
   assert (n < HostNames.size());
   return HostNames[n].c_str();
}



const char* FileIterator::getInfectedName (size_t n) const {
   assert (n < InfectedNames.size());
   return getNameFromPath(InfectedNames[n].c_str());
}



const char* FileIterator::getHostName (size_t n) const {
   assert (n < InfectedNames.size());
   return getNameFromPath(HostNames[n].c_str());
}
