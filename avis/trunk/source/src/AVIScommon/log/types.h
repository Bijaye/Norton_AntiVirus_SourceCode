// types.h - I've dodged this for just too long.
// This contains simple typedefs and functors required all over the place

// NOTE!  This keeps #defines for each type, not at the file level.
// That way, we can use it or variations of it, all over the place without
// worrying about stepping on each others toes

// Functors



#if !defined (__TYPE_MSTRINGSTRING__)
#define __TYPE_MSTRINGSTRING__

#pragma warning ( disable : 4786 )
#include <map>
#include <string>
struct F_stricmp
{
  bool operator()(const std::string& a,const std::string& b) const
    {
      return (stricmp(a.c_str(),b.c_str()) < 0);
    }
};
typedef std::map<std::string,std::string,F_stricmp> MStringString;

#endif // __TYPE_MSTRINGSTRING__

