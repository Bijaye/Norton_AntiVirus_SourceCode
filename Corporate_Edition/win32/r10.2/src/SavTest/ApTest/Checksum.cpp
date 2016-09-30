// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\checksum.h"

Checksum::Checksum(void)
{
	Initialize();
}

Checksum::~Checksum(void)
{
}

void Checksum::Initialize()
{
	const DWORD QUOTIENT = 0x04c11db7;
    int i,j;

    unsigned int crc;

    for ( i = 0; i < 256; i++ )
    {
        crc = i << 24;

        for ( j = 0; j < 8; j++ )
        {
            if ( crc & 0x80000000 )
                crc = ( crc << 1 ) ^ QUOTIENT;
            else
                crc = crc << 1;
        }
        //m_crctab[i] = htonl( crc );
		m_crctab[i] = crc;
    }
}

unsigned int Checksum::Calculate( LPCTSTR name )
{
	unsigned int result = 0;
	FILE* file;

	//Open the file for reading
	if ( NULL != ( file = fopen ( name, "r" ) ) )
	{
		std::string buffer;

		debug << "DEBUG_SUCCESS: Checksum::Calculate() opened " << name << std::endl;

		//Read the contents of the file into a buffer.
		//This buffer will be used for the checksum calculation.
		while ( ! feof( file ) )
			buffer += fgetc( file );

		unsigned int *p = ( unsigned int * ) buffer.c_str();
		unsigned int *e = ( unsigned int * ) ( buffer.c_str() + buffer.length() );
	    
		result = ~*p++;

		//Optimized for little endian hardware
		while( p < e )
		{
			result = m_crctab[result & 0xff] ^ result >> 8;
			result = m_crctab[result & 0xff] ^ result >> 8;
			result = m_crctab[result & 0xff] ^ result >> 8;
			result = m_crctab[result & 0xff] ^ result >> 8;
			result ^= *p++;
		}

		fclose( file );
	}
	else
	{
		std::cout << "Error: Failed to open file for checksum: " << name << std::endl;
	}
    
    return ~result;
}