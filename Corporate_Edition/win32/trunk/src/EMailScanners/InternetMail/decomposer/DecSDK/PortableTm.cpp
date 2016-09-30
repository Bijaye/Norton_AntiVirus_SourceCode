// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////
// Time conversion support routines
//

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#include "PortableTm.h"
#include "dec_assert.h"

//
// This is straight out of MSDN
//
void MyUnixTimeToFileTime(time_t t, LPFILETIME pft)
{
#if !defined(WINDOWS)
	dec_assert(0);
	return;
#else
	dec_assert(pft);

	// Note that LONGLONG is a 64-bit value
	LONGLONG ll;

	ll = Int32x32To64(t, 10000000) + 116444736000000000;
	pft->dwLowDateTime = (DWORD)ll;
	pft->dwHighDateTime = ll >> 32;
#endif
} 


// 
// This was ripped from Support.cpp (Dec2Lib)
//
// MyFileTimeToUnixTime
//
//    Description:
//
//      Convert a Windows FILETIME to a UNIX time_t value. To simplify the
//      math, the conversion is only accurate to within 15 seconds.
//
//    Arguments:
//
//      file_time:               The FILETIME to convert. This represents a
//                               64-bit value, which is the number of 100
//                               nanosecond intervals since midnight on
//                               January 1, 1601.
//
//      unix_time:               A pointer to the time_t where the result will
//                               be placed. This value will be a UNIX time,
//                               which is the number of seconds since midnight
//                               on January 1, 1970.
//
//    Return value:
//
//      None. 
//
void MyFileTimeToUnixTime(const FILETIME *file_time, time_t *unix_time)
{
	time_t ut;

	const time_t s_minute = 4;				// In units of 15 seconds 
	const time_t s_hour = s_minute * 60;
	const time_t s_day = s_hour * 24;
	const time_t s_year = s_day * 365;
	const time_t s_leapyear = s_year + s_day;
	const time_t s_hundred_years = (s_year * 76) + (s_leapyear * 24);
	const time_t s_sixty_nine_years = (s_year * 52) + (s_leapyear * 17);

	if(file_time == NULL) 
	{
		dec_assert(0);
		return;
	}

	if(unix_time == NULL) 
	{
		dec_assert(0);
		return;
	}

	//
	// Scale down by 150,000,000. This puts the value in terms of 15 second
	// intervals instead of 100 nanosecond intervals. 
	//
	ut = (time_t)big_divide(file_time->dwLowDateTime, file_time->dwHighDateTime, 150000000);

	//
	// Move it in reference from the year 1601 to 1970. 
	//
	ut -= ((3 * s_hundred_years) + s_sixty_nine_years);

	//
	// Scale it back up to seconds 
	//
	ut *= 15;

	//
	// Store the result
	//
	*unix_time = ut;
}

// big_divide
//
//    Description:
//
//      Divide a 64-bit quantity by a 32-bit quantity and return the 32-bit
//      result. The assumption is that the result will fit into 32 bits. If that
//      is not true, this function raises an assertion failure in debug builds.
//
//      This function uses the distributive property of division that says:
//
//        (a + b + c ...) / y = (a / y) + (b / y) + (c / y) ...
//
//      Based on this property, we conclude that if we can find some large
//      quantity k, which is less than the dividend, and which can be divided
//      evenly by the divisor resulting in some known value q, we can reduce
//      the problem by subtracting k from the dividend and accumulating q
//      such that it will eventually be added to the result. Thus, we convert
//      the problem:
//
//       x / y
//
//      into the problem:
//
//      ((x - k) / y) + (k / y) or...
//
//      ((x - k) / y) + q
//
//      Now let x become the quantity x - k. We can repeat the process by
//      choosing a new k, constrained by the new x, subtracting it from the new
//      x, while adding the quantity k / y to our accumulator q. This again
//      results in the form:
//
//      ((x - k) / y) + q becoming
//      (x / y) + q
//
//      where x is now smaller and q is now larger. If we repeat this process
//      enough times, we eventually have a value of x that can be represented
//      in 32 bits. At that time we can get the result by dividing this by
//      the divisor and adding the result to our accumulated q.
//
//      Because we don't have a 64-bit division primitive, the problem now
//      becomes how to choose a value of k with a known quotient q which is
//      sufficiently large that we don't have to repeat the process too many
//      times. This is easily done by shifting the divisor left by some
//      number of bits n, and calculating q as 2 ^ n. If we always choose n
//      such that our resulting highest 1 bit in k is in the position just lower
//      than the highest one bit in the dividend, we can then subtract this k
//      either 1 or 2 times in order to eliminate that 1 bit in the dividend.
//      Next we repeat the process, shifting up to just under the next highest
//      one bit in the dividend. Eventually we eliminate all of the 1 bits in
//      the high DWORD. When the high DWORD is zero we can complete the process
//      by simple division and addition.
//
//    Arguments:
//
//      low:                     The low DWORD of the dividend
//
//      high:                    The high DWORD of the dividend
//
//      divisor:                 The divisor.
//
//    Return value:
//
//      The function returns the result of the divison, which the caller must
//      be able to guarantee will fit into a 32-bit DWORD. 

DWORD big_divide(DWORD low, DWORD high, DWORD divisor)
{
	DWORD q;
	DWORD this_q;
	int divisor_highbit;
	int bits;

	if(divisor == 0) 
	{
		//
		// Division by zero!
		//
		dec_assert(0);
		return 0xFFFFFFFF;
	}

	if(high >= divisor) 
	{
		//
		// Overflow--the result won't fit into 32 bits 
		//
		dec_assert(0);
		return 0xFFFFFFFF;
	}

	q = 0;
	divisor_highbit = highest_one_bit(divisor);

	while(high != 0) 
	{
		bits = highest_one_bit(high) + 32;

		bits -= divisor_highbit;
		--bits;

		if(bits > 31) 
		{
			//
			// Overflow--the result won't fit into 32 bits 
			//
			dec_assert(0);
			return 0xFFFFFFFF;
		}

		shift_subtract(&low, &high, bits, divisor);

		this_q = 1 << bits;

		if(q > (0xFFFFFFFF - this_q))
		{
			//
			// Overflow--the result won't fit into 32 bits 
			//
			dec_assert(0);
			return 0xFFFFFFFF;
		}

		q += this_q;
	}

	this_q = low / divisor;

	if(q > (0xFFFFFFFF - this_q)) 
	{
		//
		// Overflow--the result won't fit into 32 bits 
		//
		dec_assert(0);
		return 0xFFFFFFFF;
	}

	return (q + this_q);
}

// highest_one_bit
//
//    Description:
//
//      Find the position of the highest bit in the given value that is set.
//      For example, for 1, this value is 0. For 7 this value is 2. This can
//      also be viewed as the greatest value of n for which it is true that
//      (2 ^ n) <= val.
//
//    Arguments:
//
//      val:                     The value for which to find the highest one bit.
//
//    Return value:
//
//      The bit position of the highest one bit, or the greatest n for which it
//      holds that (2 ^ n) <= val. 

int highest_one_bit(DWORD val)
{
	int rc;
	DWORD mask;

	dec_assert(val != 0);

	rc = 31;
	mask = 1 << rc;

	while(1) 
	{
		if((val & mask) != 0)
		{
			return rc;
		}

		mask = (mask >> 1);
		dec_assert(mask != 0);

		--rc;
		dec_assert(rc >= 0);
	}
}


// shift_subtract
//
//    Description:
//
//      Shift a number left by some quantity of bits, and then subtract the
//      result from a 64-bit quantity. The shift is done with 64-bit resolution.
//      this is part of our implementation of a portable 64-bit divide operation
//      for platforms that don't support 64-bit math.
//
//    Arguments:
//
//      low:                     A pointer to the low-order DWORD of the
//                               accumulator. The result of the shift is
//                               subtracted from this accumulator, and the result
//                               of this operation is stored back into it.
//
//      high:                    A pointer to the high-order DWORD of the
//                               accumulator.
//
//      bits:                    The number of bits to shift our number left
//                               before subtracting. This shift is done with 64
//                               bit resolution.
//
//      num:                     The number that will be shifted and then
//                               subtracted from the accumulator.
//
//    Return value:
//
//      None. 

void shift_subtract(DWORD *low, DWORD *high, int bits, DWORD num)
{
	DWORD val_low;
	DWORD val_high;
	DWORD mask;
	DWORD original_low;

	//
	// 64-bit shift left by bits 
	//
	mask = ((1 << bits) - 1) << (32 - bits);
	val_high = (num & mask) >> (32 - bits);
	val_low = num << bits;

	//
	// 64-bit subtract. The bit masking is necessary to remain portable in
	// weird cases where a DWORD might be larger than 32 bits in size. 
	//
	original_low = *low & 0xFFFFFFFF;
	*low = ((*low - val_low) & 0xFFFFFFFF);

	if(*low > original_low) 
	{
		//
		// Overflow detected. Borrow one. 
		//
		++val_high;
	}

	*high -= val_high;
}
