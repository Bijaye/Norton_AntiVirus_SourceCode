#ifndef FILEITERATOR_HPP
#define FILEITERATOR_HPP

#include <vector>
#include <string>
using namespace std;

// Contains host-infected filename pairs from either a pair of directories
// or a pair of files containing lists of file names, depending on how
// it is initialized.

// count returns the number of pairs, and the get... methods each return 
// an element from the nth pair. The get...Path methods return a filename
// with as much of its path as was specified either in the filename list
// or in the initFromDir arguments, depending on how the object was 
// initialized. The get...Name methods return just the filename without
// any path information.

// If a requested item is missing (not all infected files have
// corresponding host files), a zero-length string, not NULL, is returned.
// The ...Exists methods check whether the nth host or infected filename 
// is non-zero in length (all trailing blanks are removed from filenames,
// so blank lines are always truncated to zero-length strings.)

// When loading from directories, every filename from the infected 
// directory ending in .COM, .EXE, .VOM, .VXE, _COM or _EXE (regardless
// of case) is loaded, and it is paired with the same-named file from
// the host directory, if that exists.

// A list file should have one file name per line. The nth line in the 
// infected list is paired with the nth line in the host list, so if an
// infected file has no corresponding host, use a blank line at that 
// position in the host list file.



class FileIterator {
   public:
      bool initFromList (const char* zHostList, const char* zInfectedList);
      bool initFromDir  (const char* zHostDir,  const char* zInfectedDir);
      size_t count (void) const;
      bool infectedFileExists (size_t n) const;
      bool hostFileExists     (size_t n) const;
      const char* getInfectedPath (size_t n) const;
      const char* getHostPath     (size_t n) const;
      const char* getInfectedName (size_t n) const;
      const char* getHostName     (size_t n) const;

   private:
      vector<string> HostNames;
      vector<string> InfectedNames;
};

#endif
