//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/TIMEDATE.CPv   1.0   12 Mar 1998 12:17:46   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/TIMEDATE.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:46   DCHI
// Initial revision.
// 
//************************************************************************

//
// TIMEDATE.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALTimeDate::operator new()
//  ALTimeDate::ALTimeDate()
//  ALTimeDate::~ALTimeDate()
//  ALTimeDate::ToJulian()
//  ALTimeDate::FromJulian()
//  ALTimeDate::GetUnixTime()
//  ALTimeDate::SetTimeDate(long)
//  ALTimeDate::SetTimeDate(struct tm *)
//  ALTimeDate::GetTimeDate()
//  ALTimeDate::GetDosTime()
//  ALTimeDate::GetDosDate()
//
// DESCRIPTION
//
//  This file contains all of the member functions of class ALTimeDate.
//  This class is used only by ALStorage, but it seemed like a good
//  idea to break it out in a separate class.  A lot of the code in here
//  came straight out of CommLib.
//
// REVISION HISTORY
//
//  May 26, 1994  1.0A   : First release
//
//  August 10, 1994 1.0B : Modified a couple of #ifdefs to take UNIX into
//                         account.
//
//   February 14, 1996  2.0A : New release.
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include <time.h>
#include "timedate.h"

//
// NAME
//
//  ALTimeDate::operator new()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Memory allocator used when ArchiveLib resides in a 16 bit DLL.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  void * ALTimeDate::operator new( size_t size )
//
// C SYNOPSIS
//
//  None.
//
// VB SYNOPSIS
//
//  None.
//
// DELPHI SYNOPSIS
//
//  None.
//
// ARGUMENTS
//
//  size  :  The number of bytes that the compiler has decided will be
//           necessary to construct a new ALTime object.
//
// DESCRIPTION
//
//  When using a DLL, it is easy to get into a dangerous situation when
//  creating objects whose ctor and dtor are both in the DLL.  The problem
//  arises because when you create an object using new, the memory for
//  the object will be allocated from the EXE.  However, when you destroy
//  the object using delete, the memory is freed inside the DLL.  Since
//  the DLL doesn't really own that memory, bad things can happen.
//
//  But, you say, won't the space just go back to the Windows heap regardless
//  of who tries to free it?  Maybe, but maybe not.  If the DLL is using
//  a subsegment allocation scheme, it might do some sort of local free
//  before returning the space to the windows heap.  That is the point where
//  you could conceivably cook your heap.
//
//  By providing our own version of operator new inside this class, we
//  ensure that all memory allocation for the class will be done from
//  inside the DLL, not the EXE calling the DLL.
//
//  Incidentally, I suspect that this function never gets called.
//  ALTimeDate objects exist primarily as members of ALStorage.  When
//  that is the case, their memory is allocated as part of the memory
//  allocation process for the ALStorage derived class.
//
// RETURNS
//
//  A pointer to some memory that should have been pulled out of the
//  heap for the DLL.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//


#if defined( AL_BUILDING_DLL )

void AL_DLL_FAR * AL_PROTO
ALTimeDate::operator new( size_t size )  /* Tag protected function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALTimeDate::ALTimeDate()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The ALTimeDate constructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALTimeDate::ALTimeDate();
//
// C SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  All the constructor does is initialize the data members.  By
//  setting the year to an invalid value of 0, we can always see
//  that the time date stamp for a file hasn't been initialized.
//
// RETURNS
//
//  None.
//
// EXAMPLE
//
//  None, timedate objects are only constructed for use inside
//  the library, as members of ALStorage objects.
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

AL_PROTO
ALTimeDate::ALTimeDate()  /* Tag public function */
{
    miYear = 0;  //This is an illegal year, means it is uninitialized
    miMonth = 0;
    miDate = 0;
    miHour = 0;
    miMinute = 0;
    miSecond = 0;
}

//
// NAME
//
//  ALTimeDate::~ALTimeDate()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The ALTimeDate destructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALTimeDate::~ALTimeDate();
//
// C SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  The destructor has nothing to do.
//
// RETURNS
//
//  None.
//
// EXAMPLE
//
//  None, timedate objects are only constructed for use inside
//  the library, as members of ALStorage objects.  Thus, they are
//  only destroyed inside the library.
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

AL_PROTO
ALTimeDate::~ALTimeDate()  /* Tag public function */
{
}

//
// NAME
//
//  ALTimeDate::ToJulian()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Return the Julian date for the given ALTimeDate object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  long ALTimeDate::ToJulian();
//
// C SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//   This function is used to make a Julian day number from a normal
//   month/day/year thing.  We need a Julian day in order to make a
//   UNIX style time stamp.  The UNIX time stamp is used to store
//   time stamps in Archive directories.
//
// RETURNS
//
//  The Julian-like date.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

long AL_PROTO
ALTimeDate::ToJulian()  /* Tag public function */
{
    return (long)( miDate - 32076)
          + 1461L * ( miYear + 4800L + ( miMonth - 14) / 12) / 4
          + 367 * ( miMonth - 2 - ( miMonth - 14) / 12 * 12) / 12
          - 3 * (( miYear + 4900L + ( miMonth - 14) / 12) / 100) / 4
          + 1;
}

//
// NAME
//
//  ALTimeDate::FromJulian()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Convert a Julian-style date to an ALTimeDate object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALTimeDate::FromJulian( long jdn );
//
// C SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// ARGUMENTS
//
//  jdn  :  A julian date number, ideally one produced by ToJulian().
//
// DESCRIPTION
//
//   This function is used to convert a julian date to a normal
//   year/month/day.  Time/date stamps are stored in Archives in
//   UNIX format.  This function is needed to convert a UNIX
//   time stamp to a normal mm/dd/yy.
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

void AL_PROTO
ALTimeDate::FromJulian( long jdn )  /* Tag public function */
{
        long x;
        long z;
        long m;
        long d;
        long y;
        const long daysPer400Years = 146097L;
        const long fudgedDaysPer4000Years = 1460970L + 31;

        x = jdn + 68569L;
        z = 4 * x / daysPer400Years;
        x = x - (daysPer400Years * z + 3) / 4;
        y = 4000 * (x + 1) / fudgedDaysPer4000Years;
        x = x - 1461 * y / 4 + 31;
        m = 80 * x / 2447;
        d = x - 2447 * m / 80;
        x = m / 11;
        m = m + 2 - 12 * x;
        y = 100 * (z - 49) + y + x;
//
// I don't know whether or not we could eliminate these temporary longs
//
        miYear = (short int) y;
        miMonth = (short int) m;
        miDate = (short int) d;
}

//
// NAME
//
//  ALTimeDate::GetUnixTime()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Convert an ALTimeDate objects to a Unix-style time number
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  long ALTimeDate::GetUnixTime();
//
// C SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  This function is used to convert the m/d/y h:m:s time stamp for a file
//  into a UNIX time stamp.  The UNIX time stamp is a 32 bit long that
//  is used to store time stamps in an Archive.
//
// RETURNS
//
//  A UNIX time, converted from the internal m/d/y h:m:s data.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

long AL_PROTO
ALTimeDate::GetUnixTime()  /* Tag public function */
{
    const long UnixFirstDay = 2440588L;
    long result;

    result = ToJulian();
    result -= UnixFirstDay;
    if ( result >= 0L ) {
        result *= 3600L * 24;
        result += 3600L * miHour;
        result += 60L * miMinute;
        result += miSecond;
    } else
        result = 0L;
    return result;
}

//
// NAME
//
//  ALTimeDate::SetTimeDate()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Convert a Unix-style time number to an ALTimeDate object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALTimeDate::SetTimeDate( long unix_time );
//
// C SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// ARGUMENTS
//
//  unix_time  :  A long integer in UNIX timestamp format.
//
// DESCRIPTION
//
//  This function is called when we are reading a directory in from
//  an archive.  It is used to set the internal data members of an
//  ALTimeDate object, after converting from unix time.
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

void AL_PROTO
ALTimeDate::SetTimeDate( long unix_time ) /* Tag public function */
{
    const long UnixFirstDay = 2440588L;

    long jd = unix_time / ( 3600L * 24 );
    long hms = unix_time % ( 3600L * 24 );
    FromJulian( jd + UnixFirstDay );
    miHour = (short int) ( hms / 3600 );
    hms -= 3600L * miHour;
    miMinute = (short int) ( hms / 60 );
    miSecond = (short int) ( hms - ( miMinute * 60 ) );
}

//
// NAME
//
//  ALTimeDate::SetTimeDate()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Convert a C-style tm structure to an ALTimeDate object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALTimeDate::SetTimeDate( struct tm *tblock );
//
// C SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// ARGUMENTS
//
//  tblock :  A time date stamp as used by the C run time library.
//
// DESCRIPTION
//
//  When working with DOS files, time stamps are read in to a structure
//  in the struct tm format.  This function provides an easy way to convert
//  the structure into our internal format.  When a DOS file is opened
//  using Open(), this function is called.
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

void AL_PROTO
ALTimeDate::SetTimeDate( struct tm AL_DLL_FAR *tblock )  /* Tag public function */
{
    AL_ASSERT( tblock != 0, "SetTimeDate: passing illegal null parameter" );
    miYear = (short int) ( tblock->tm_year + 1900 );
    miMonth = (short int) ( tblock->tm_mon + 1 );
    miDate = (short int) tblock->tm_mday;
    miHour = (short int) tblock->tm_hour;
    miMinute = (short int) tblock->tm_min;
    miSecond = (short int) tblock->tm_sec;
}

//
// NAME
//
//  ALTimeDate::GetTimeDate()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Convert an ALTimeDate object to a C-style tm structure.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALTimeDate::GetTimeDate( struct tm *tblock );
//
// C SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// ARGUMENTS
//
//  tblock  : A structure in the format used by the C runtime library for
//            storing time and date stamps.
//
// DESCRIPTION
//
//  This function provides the reverse of SetTimeDate().  You would think
//  that we could just set the appropriate members of struct tm, but
//  there is a problem with that.  struct tm has one element that is
//  supposed to be the day of the week, and another that is supposed
//  to be the number of the day within the year.  We could try to
//  figure those out using the julian day function, but since gmtime()
//  will figure them out for us, we'll use that instead.
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

void AL_PROTO
ALTimeDate::GetTimeDate( struct tm AL_DLL_FAR *tblock )  /* Tag public function */
{
    AL_ASSERT( tblock != 0, "GetTimeDate: passing illegal null parameter" );
    long unix_time = GetUnixTime();
    struct tm AL_DLL_FAR *result = gmtime( (const time_t *) &unix_time );
    if ( result ) {
        *tblock = *result;
        tblock->tm_isdst = 0;
    } else {    //This should never happen!
        tblock->tm_year = miYear - 1900;
        tblock->tm_mon = miMonth - 1;
        tblock->tm_mday = miDate;
        tblock->tm_hour = miHour;
        tblock->tm_min = miMinute;
        tblock->tm_sec = miSecond;
        tblock->tm_wday = 0;
        tblock->tm_yday = 0;
        tblock->tm_isdst = 0;
    }
}

//
// NAME
//
//  ALTimeDate::GetDosTime()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Convert the time portion of an ALTimeDate object to 16 bit DOS format.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  unsigned short int ALTimeDate::GetDosTime();
//
// C SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  When we close a file that needs to have its time and date stamp
//  set, we normally use the _dos_setftime() function to do the
//  work.  It expects to see the time packed into a particular
//  sequence of bits in an unsigned short.  That is what this
//  function does.  It packs the bits just the way you want them.
//
// RETURNS
//
//  This function returns the time stored in this.  The bits of the
//  time are packed into the form that is needed by the _dos_setftime()
//  format.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

unsigned short int AL_PROTO
ALTimeDate::GetDosTime()  /* Tag public function */
{
    int result;
    result = miSecond / 2;
    result |= miMinute << 5;
    result |= miHour << 11;
    return (unsigned short int) result;
}

//
// NAME
//
//  ALTimeDate::GetDosDate()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Convert the date portion of an ALTimeDate object to 16 bit DOS format.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  unsigned short int ALTimeDate::GetDosDate();
//
// C SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, class ALTimedate is not exported to C/VB/Delphi.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  When we close a file that needs to have its time and date stamp
//  set, we normally use the _dos_setftime() function to do the
//  work.  It expects to see the date packed into a particular
//  sequence of bits in an unsigned short.  That is what this
//  function does.  It packs the bits just the way you want them.
//
// RETURNS
//
//  This function returns the date stored in this.  The bits of the
//  date are packed into the form that is needed by the _dos_setftime()
//  format.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

unsigned short int AL_PROTO
ALTimeDate::GetDosDate()  /* Tag public function */
{
    int result;
    result = miDate;
    result |= miMonth << 5;
    result |= (miYear-1980) << 9;
    return (unsigned short int ) result;
}
