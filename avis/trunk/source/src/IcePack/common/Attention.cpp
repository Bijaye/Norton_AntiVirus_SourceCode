// Attention.cpp: implementation of the Attention class.
//////////////////////////////////////////////////////////////////////

/* ----- */

/*
 * This file has been commented in an attempt to work out what it
 * does and why it wasn't working.  None of the comments in the
 * file were written design or code-authoring time, and reflect
 * the understanding of the code that I have, rather than the
 * understanding of the code that the author had.
 *
 * You may decide which of the above is greater.
 *
 * [inw 2000-04-14]
 */

/* ----- */

#include "stdafx.h"
#include "Attention.h"

#include <string>

#include <AVIS.h>
#include <WinRegistry.h>
#include <Logger.h>

#include "GlobalData.h"

using namespace std;

/* ----- */

const char *Attention::key          = "attention";

/*
 * The presence of these two globals make this concret object
 * very un-thread-safe.  The original author did not seem to mind
 * that this was the case, and bashed on regardless, ever
 * courageous in the face of silliness.
 */
uint        Attention::attFlags     = 0;
bool        Attention::initialized  = false;

/*
 * Note that when changing this list, you must also change the
 * flags enum and the 'typeCount' value in Attention.h.  See
 * comment there for more information.
 */
Attention::AttMap Attention::attMap[typeCount] =
{
  { AttType::Defcast,      "defcast"      },
  { AttType::Disk,         "disk"         },
  { AttType::Download,     "download"     },
  { AttType::Quarantine,   "quarantine"   },
  { AttType::Query,        "query"        },
  { AttType::ScanExplicit, "scanexplicit" },
  { AttType::Submit,       "submit"       },
  { AttType::Target,       "target"       },
  { AttType::Shutdown,     "shutdown"     },
  { AttType::Unavailable,  "unavailable"  },
};

/* ----- */

/*
 * [ The following comments are not comments written at design or ]
 * [ code-authoring time.  They are comments added later in an    ]
 * [ attempt to fix the broken code herein.                       ]
 *
 * MakeString works on the first parameter, treating it as a
 * bitfield.  It constructs a string containing the string
 * translations of that bitfield, separated by ", ", and places
 * that string into the second parameter.
 */
bool Attention::MakeString(uint flags, std::string& target)
{
  bool  didFirst;
  int   i;

  /*
   * Clear the target string to start with.
   */
  target = "";

  /*
   * Loop across all the possible attention types.
   */
  didFirst = false;
  for (i = 0; i < typeCount; i++)
  {
    /*
     * If the bit for this attention type is set,
     * then add the string identifier for the attention type
     * to the end of the target string, preceding it with ", " if
     * necessary.
     */
    if (flags & attMap[i].type)
    {
      if (didFirst)
        target  += ", ";
      target    += attMap[i].description;
      didFirst  = true;
    }
  }

  /*
   * This method always returns true.  It cannot fail.  Or so the
   * author thought.
   */
  return true;
}

/* ----- */

/*
 * [ The following comments are not comments written at design or ]
 * [ code-authoring time.  They are comments added later in an    ]
 * [ attempt to fix the broken code herein.                       ]
 *
 * ParseString works on the second parameter, treating it as a
 * string of comma-separated string values.  For each value in
 * the string, the bit flag value is looked up in the attMap
 * table, and ORed into flags.
 *
 * No complaint is made if an unrecognised value is found in the
 * string.
 */
bool Attention::ParseString(uint& flags, const char *s)
{
  int     beginning;
  int     end;
  int     i;
  string  item;
  string  source(s);

  /*
   * Clear flags to start with.
   */
  flags = 0;

  /*
   * Duck out early if the string is clearly silly.
   * [BoundsChecker-induced.  inw 2000-06-17]
   */
  if ((!s) || (!s[0]))
    return true;

  /*
   * Initialise the loop controllers to the start of the string,
   * and the first comma, respectively.
   */
  beginning = 0;
  end       = source.find(",", 0);

  /*
   * Loop.
   */
  while (beginning != string::npos)
  {
    /*
     * If there as no comma to be found, take the remainder of
     * the string as the value.  If there was a comma, take the
     * chunk of the string between 'beginning' and the comma.
     */
    if (string::npos == end)
      item = source.substr(beginning, source.length() - beginning);
    else
      item = source.substr(beginning, end - beginning);

    /*
     * Look up the current value in the string -> bit value table.
     */
    for (i = 0; ((i < typeCount) && (item != attMap[i].description)); i++);

    /*
     * If we found a match, then set the appropriate bit in the
     * flags.
     */
    if (i < typeCount)
      flags |= attMap[i].type;

    /*
     * If we're not at the end of the string yet, then advance
     * the start position to just after the ", " (note the
     * space), and look from there to find the next comma.
     *
     * If we are at the end of the string, move beginning to the
     * invalid string position to cause the loop to terminate.
     */
    if (string::npos != end)
    {
      beginning = end + 2;
      end      = source.find(",", beginning);
    }
    else
      beginning = string::npos;
  }

  /*
   * This method always returns true.  It cannot fail.  Or so the
   * author thought.
   */
  return true;
}

/* ----- */

/*
 * [ The following comments are not comments written at design or ]
 * [ code-authoring time.  They are comments added later in an    ]
 * [ attempt to fix the broken code herein.                       ]
 *
 * Set_() either sets (if the second parameter is 'true') or unsets
 * (if [you guessed it] the second parameter is 'false') the
 * attention bit specified by the first parameter (the first
 * parameter is an entry in the AttType enum -- see Attention.h
 * for the details).
 *
 * Set_() returns 'true' on success, 'false' on failure.
 */
bool Attention::Set_(AttType aType, bool set)
{
  bool    rc      = false;
  uint    flags   = 0;

  /*
   * Attempt to get the current bitfield from the registry.  If
   * this works, then turn on or off the bit we're interested in,
   * as appropriate.
   */
  if (Get(flags))
  {
    if (set)
      flags |= aType;
    else
      flags &= ~aType;
  }

  /*
   * If we changed the flags in the above operation, then we need
   * to write the new attention flags back to the registry.
   */
  if (attFlags != flags)
  {
    WinRegistry uRoot(HKEY_LOCAL_MACHINE, GlobalData::regUpdateRoot);

    /*
     * Is he registry key valid?
     */
    if (uRoot.ValidKey())
    {
      string newValue;

      /*
       * Convert the bitfield back into a string suitable for the
       * registry (why we don't just put the bitfield in the
       * registry I will never fully understand), and attempt to
       * set that value back into the registry.  If it works,
       * then remember the new flags, and set the return code to
       * 'true'.
       *
       * If it doesn't work, complain in the log.
       */
      MakeString(flags, newValue);
      if (uRoot.SetValue(key, (const uchar *) newValue.c_str()))
      {
        attFlags    = flags;
        initialized = true;
        rc          = true;
      }
      else
        Logger::Log(Logger::LogError, Logger::LogIcePackOther,
              "Unable to set attention registry value in Attention::Set_");
    }
    else /* Invalid registry key */
      Logger::Log(Logger::LogError, Logger::LogIcePackOther,
            "Unable to get valid registry key to update directory from Attention::Set_");
  }
  else /* We didn't change the flags, so we can claim success. */
    rc = true;

  /*
   * JD.
   */
  return rc;
}

/* ----- */

/*
 * [ The following comments are not comments written at design or ]
 * [ code-authoring time.  They are comments added later in an    ]
 * [ attempt to fix the broken code herein.                       ]
 *
 * Get() retrives the current attention bitfield from the
 * registry, and places it into the single parameter (flags).
 *
 * Set_() returns 'true' on success, 'false' on failure.  If the
 * method returns false, the value of flags is untouched.
 */
bool Attention::Get(uint& flags)
{
  bool rc = false;

  /*
   * If we already have the current flags value (we've already
   * done the hard work) then use that value instead of going to
   * the registry.
   */
  if (initialized)
  {
    flags = attFlags;
    rc    = true;
  }
  else
  {
    /*
     * Okay, we don't have the current value handy.  Crack open
     * the appropriate registry key.
     */
    uchar       charValue[maxStrLen];
    DWORD       length = maxStrLen;
    WinRegistry uRoot(HKEY_LOCAL_MACHINE, GlobalData::regUpdateRoot);

    /*
     * If we opened the key and everything is okay, then attempt
     * to query the 'attention' value.  If that query fails,
     * clear the flags setting.  If it succeeds, call
     * ParseString() to convert the string read from the registry
     * into the bitfield.  Either way, we have a value for the
     * current bitfield (either it's zero because the registry
     * value is absent, or we got it from parsing the string), so
     * set the return code to true.
     *
     * If the reigstry key access failed (presumably the key does
     * not exist), complain about it in the log, and drop through
     * with the return code still set to false.
     */
    if (uRoot.ValidKey())
    {
      if (! uRoot.QueryValue(key, charValue, length))
        flags = 0;
      else
        rc = ParseString(flags, (const char *)charValue);

      attFlags    = flags;
      initialized = true;
      rc          = true;
    }
    else
      Logger::Log(Logger::LogError, Logger::LogIcePackOther,
            "Unable to get valid registry key to update directory from Attention::Get");
  }

  /*
   * JD.
   */
  return rc;
}
/* ----- End Transmission ----- */
