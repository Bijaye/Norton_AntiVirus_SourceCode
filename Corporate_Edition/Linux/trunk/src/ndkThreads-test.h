
#include <cxxtest/TestSuite.h>

#include <stdio.h>


#include <string>
#include <stack>

#include "winThreads.h"

/*
 * Using C-Style casts on numbers to make their type quite clear.
 *
 * (Note that testing the operated upon memory makes these checks non-threadsafe)
 */

class testThreads : public CxxTest::TestSuite
{
public:

    template <class T>
        void testInterlockedExchange( T* target, T value )
        {
//             std::ostringstream tmp;
//             tmp << "In check Exchange " << *target << " <--> " << value;
//             TS_TRACE( tmp.rdbuf()->str() );
            T oldTarget = *target;
            T oldValue = value;
            T result = InterlockedExchange( target, value );
            TS_ASSERT_EQUALS( oldValue, *target );
            TS_ASSERT_EQUALS( result, oldTarget );
        };

    void testInterlockedExchange(void)
    {
        long target = 0;
        unsigned long utarget = 0;
        testInterlockedExchange( &target, (long)1 );
        testInterlockedExchange( &utarget, (unsigned long)1 );

        target = 5;
        utarget = 5;
        testInterlockedExchange( &target, (long)0 );
        testInterlockedExchange( &utarget, (unsigned long)0 );
    }

    template <class T>
        void testInterlockedExchangeAdd( T addend, long increment, T expected )
        {
//             std::ostringstream tmp;
//             tmp << "In check ExchangeAdd " << addend << " + " << increment << "  -> " << expected;
//             TS_TRACE( tmp.rdbuf()->str() );
            T oldAddend = addend;
            T oldIncrement = increment;
            T result = InterlockedExchangeAdd( &addend, increment );
            TS_ASSERT_EQUALS( oldIncrement, increment );
            TS_ASSERT_EQUALS( addend, oldAddend + increment );
            TS_ASSERT_EQUALS( result, oldAddend );
            TS_ASSERT_EQUALS( addend, expected );
            TS_ASSERT_EQUALS( oldAddend + increment, expected );
        };

    void testInterlockedExchangeAdd(void)
    {
        testInterlockedExchangeAdd( (long)-1, (long)1, (long)0 );

        testInterlockedExchangeAdd( (long)LONG_MAX, (long)1, (long)LONG_MIN );

        testInterlockedExchangeAdd( (unsigned long)ULONG_MAX, (long)1, (unsigned long)0 );

        testInterlockedExchangeAdd( (long)0, (long)-1, (long)-1 );

        testInterlockedExchangeAdd( (long)LONG_MIN, (long)-1, (long)LONG_MAX );

        testInterlockedExchangeAdd( (unsigned long)0, (long)-1, (unsigned long)ULONG_MAX );

        // TODO - expand test to include more range of 'increment'


        testInterlockedExchangeAdd( (long)0, (long)1, (long)1 );
        testInterlockedExchangeAdd( (unsigned long)0, (unsigned long)1, (unsigned long)1 );

        testInterlockedExchangeAdd( (long)1, (long)3, (long)4 );
        testInterlockedExchangeAdd( (unsigned long)1, (unsigned long)3, (unsigned long)4 );
    }

    template <class T>
        void testInterlockedIncrement( T target, T expected )
        {
//             std::ostringstream tmp;
//             tmp << "In check Increment " << target << "  -> " << expected;
//             TS_TRACE( tmp.rdbuf()->str() );
            T oldTarget = target;
            T result = InterlockedIncrement( &target );
            TS_ASSERT_EQUALS( target, result );
            TS_ASSERT_EQUALS( target, oldTarget + 1 );
            TS_ASSERT_EQUALS( result, oldTarget + 1 );
            TS_ASSERT_EQUALS( target, expected );
            TS_ASSERT_EQUALS( oldTarget + 1, expected );
        };

    void testInterlockedIncrement(void)
    {
        testInterlockedIncrement( (long)-1, (long)0 );

        testInterlockedIncrement( (long)LONG_MAX, (long)LONG_MIN );

        testInterlockedIncrement( (unsigned long)ULONG_MAX, (unsigned long)0 );


        testInterlockedIncrement( (long)0, (long)1 );
        testInterlockedIncrement( (unsigned long)0, (unsigned long)1 );

        testInterlockedIncrement( (long)1, (long)2 );
        testInterlockedIncrement( (unsigned long)1, (unsigned long)2 );

        testInterlockedIncrement( (long)2, (long)3 );
        testInterlockedIncrement( (unsigned long)2, (unsigned long)3 );

        // byte boundaray cases
        testInterlockedIncrement( (long)0x0ff, (long)0x100 );
        testInterlockedIncrement( (unsigned long)0x0ff, (unsigned long)0x100 );

        testInterlockedIncrement( (long)0x0ffff, (long)0x10000 );
        testInterlockedIncrement( (unsigned long)0x0ffff, (unsigned long)0x10000 );

        testInterlockedIncrement( (long)0x0ffffff, (long)0x1000000 );
        testInterlockedIncrement( (unsigned long)0x0ffffff, (unsigned long)0x1000000 );
    }

    template <class T>
        void testInterlockedDecrement( T target, T expected )
        {
//             std::ostringstream tmp;
//             tmp << "In check Decrement " << target << "  -> " << expected;
//             TS_TRACE( tmp.rdbuf()->str() );
            T oldTarget = target;
            T result = InterlockedDecrement( &target );
            TS_ASSERT_EQUALS( target, result );
            TS_ASSERT_EQUALS( target, oldTarget - 1 );
            TS_ASSERT_EQUALS( result, oldTarget - 1 );
            TS_ASSERT_EQUALS( target, expected );
            TS_ASSERT_EQUALS( oldTarget - 1, expected );
        };

    void testInterlockedDecrement(void)
    {
        testInterlockedDecrement( (long)0, (long)-1 );

        testInterlockedDecrement( (long)LONG_MIN, (long)LONG_MAX );

        testInterlockedDecrement( (unsigned long)0, (unsigned long)ULONG_MAX );



        testInterlockedDecrement( (long)1, (long)0 );
        testInterlockedDecrement( (unsigned long)1, (unsigned long)0 );

        testInterlockedDecrement( (long)2, (long)1 );
        testInterlockedDecrement( (unsigned long)2, (unsigned long)1 );

        testInterlockedDecrement( (long)3, (long)2 );
        testInterlockedDecrement( (unsigned long)3, (unsigned long)2 );

        // byte boundaray cases
        testInterlockedDecrement( (long)0x100, (long)0x0ff );
        testInterlockedDecrement( (unsigned long)0x100, (unsigned long)0x0ff );

        testInterlockedDecrement( (long)0x10000, (long)0x0ffff );
        testInterlockedDecrement( (unsigned long)0x10000, (unsigned long)0x0ffff );

        testInterlockedDecrement( (long)0x1000000, (long)0x0ffffff );
        testInterlockedDecrement( (unsigned long)0x1000000, (unsigned long)0x0ffffff );
    }
};
