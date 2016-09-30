// Attention.h: interface for the Attention class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATTENTION_H__BCDDA8E7_B1C9_11D3_AEA3_00A0C9C71BBC__INCLUDED_)
#define AFX_ATTENTION_H__BCDDA8E7_B1C9_11D3_AEA3_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AVIS.h>
#include <string>

class Attention  
{
  public:

    /*
     * These are the bit field values of the various attention
     * types.
     */
    enum AttType
    {
      Defcast      = 0x0001,
      Disk         = 0x0002,
      Download     = 0x0004,
      Quarantine   = 0x0008,
      Query        = 0x0010,
      ScanExplicit = 0x0020,
      Submit       = 0x0040,
      Target       = 0x0080,
      Shutdown     = 0x0100,
      Unavailable  = 0x0200,
    };

    /*
     * Note that typeCount must always be kept up to date with
     * the number of values in the above enum, which in turn must
     * be kept up to date with the array 'attMap' in
     * Attention.cpp.
     * No, I would definitely not have done it this way either,
     * but there it is.
     */
    enum { typeCount =  10 };
    enum { maxStrLen = 256 };

    static bool Set(AttType aType)
    {
      return Set_(aType, true);
    }

    static bool UnSet(AttType aType)
    {
      return Set_(aType, false);
    }

    static bool InDiskAttention()
    {
      uint flags;
      Get(flags);
      return (0 != (Disk & flags));
    }

    static bool InQuarantineAttention()
    {
      uint flags;
      Get(flags);
      return (0 != (Quarantine & flags));
    }

    static bool InScanExplicitAttention()
    {
      uint flags;
      Get(flags);
      return (0 != (ScanExplicit & flags));
    }

    static bool Get(uint& flags);

  private:
    Attention               ();
    virtual ~Attention      ();

    static bool Set_        (AttType aType, bool set);
    static bool MakeString  (uint flags, std::string& target);
    static bool ParseString (uint& flags, const char *source);

    static const char *key;
    static uint        attFlags;
    static bool        initialized;

    typedef struct typ_AttMap
    {
      AttType     type;
      const char *description;
    } AttMap;

    static AttMap attMap[typeCount];
};

#endif // !defined(AFX_ATTENTION_H__BCDDA8E7_B1C9_11D3_AEA3_00A0C9C71BBC__INCLUDED_)
