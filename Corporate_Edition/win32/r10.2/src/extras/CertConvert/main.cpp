// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2004, 2005, Symantec Corporation, All rights reserved.
// main.cpp


#include "stdafx.h"
#include <iostream>
#include <string>
#include "base64.h"
#include <assert.h>

const char PEM_BEGIN_RSA_KEY[]      = "-----BEGIN RSA PRIVATE KEY-----\n";
const char PEM_END_RSA_KEY[]        = "-----END RSA PRIVATE KEY-----\n";
const char PEM_BEGIN_CERT[]         = "-----BEGIN CERTIFICATE-----\n";
const char PEM_END_CERT[]           = "-----END CERTIFICATE-----\n";
const char PEM_BEGIN_CSR[]          = "-----BEGIN CERTIFICATE REQUEST-----\n";
const char PEM_END_CSR[]            = "-----END CERTIFICATE REQUEST-----\n";

void print_usage()
{
    std::cout << "CertConvert - format conversion utility for Symantec AntiVirus and" << std::endl;
    std::cout << "     Symantec Client Security certificates, private keys and" << std::endl;
    std::cout << "     certificate signing requests.  Converts from DER to PEM and" << std::endl;
    std::cout << "     vice versa." << std::endl;
    std::cout << std::endl;
    std::cout << " Usage:" << std::endl;
    std::cout << "     CertConvert <file type> <output format> <file name>" << std::endl;
    std::cout << "      Where <file type> is one of: CER, PVK or CSR," << std::endl;
    std::cout << "      and <output format> is one of: PEM or DER," << std::endl;
    std::cout << "      and <file name> is the input file to convert."  << std::endl;
    std::cout << "     The output file will have the same name plus an extension" << std::endl;
    std::cout << "      indicating the new output format." << std::endl;
    std::cout << " Example:" << std::endl;
    std::cout << "     CertConvert CSR PEM mycsr.csr" << std::endl;
    std::cout << "      Convert mycsr.csr from DER encoding to PEM encoding and stores" << std::endl;
    std::cout << "      output in mycsr.csr.pem" << std::endl;
    std::cout << " NOTE: CertConvert does not validate the format of the input file!" << std::endl;
}

bool read_file( const char* file_name, unsigned char*& raw_data, size_t& raw_data_len )
{
    bool rtn = true;

    // Open the caller's file.

    FILE* fp = fopen( file_name, "rb" );

    if( fp != NULL )
    {
        // Get the size of the file.

        fseek( fp, 0, SEEK_END );
        size_t raw_data_bytes = ftell( fp );
        fseek( fp, 0, SEEK_SET );

        // Allocate a new buffer to hold the raw data.

        raw_data     = NULL;
        raw_data_len = raw_data_bytes;

        try
        {
            raw_data = new unsigned char[raw_data_len];
        }
        catch( std::bad_alloc& )
        {
            raw_data = NULL;
        }

        if( raw_data == NULL )
        {
            std::cout << "Memory allocation failed" << std::endl;

            rtn = false;
        }
        else
        {
            // Read data from the file into our buffer.

            BYTE* raw_data_iter = raw_data;

            while( raw_data_bytes > 0 )
            {
                size_t bytes_read = fread( raw_data_iter, 1, raw_data_bytes, fp );

                if( ferror( fp ) != 0 )
                {
                    // Some problem reading from the file.

                    std::cout << "Error while reading file " << file_name << ", error: " << errno << std::endl;

                    rtn = false;

                    break;
                }

                raw_data_bytes -= bytes_read;
                raw_data_iter += bytes_read;
            }
        }

        fclose( fp );
    }
    else
    {
        std::cout << "Failed to open file " << file_name << ", error: " << errno << std::endl;

        rtn = false;
    }

    return( rtn );
}

bool save_file( const char* file_name, const unsigned char* raw_data, size_t raw_data_len )
{
    bool rtn = true;

    // Open the caller's file.

    FILE* fp = fopen( file_name, "wb" );

    if( fp != NULL )
    {
        // Write data from our buffer into the file.

        size_t        raw_data_bytes = raw_data_len;
        const BYTE*   raw_data_iter  = raw_data;

        while( raw_data_bytes > 0 )
        {
            size_t bytes_written = fwrite( raw_data_iter, 1, raw_data_bytes, fp );

            if( ferror( fp ) != 0 )
            {
                // Some problem reading from the file.

                std::cout << "Error writing to output file " << file_name << ", error: " << errno << std::endl;

                rtn = false;

                break;
            }

            raw_data_bytes -= bytes_written;
            raw_data_iter += bytes_written;
        }

        // Close the file.

        fclose( fp );
    }
    else
    {
        std::cout << "Error opening output file " << file_name << ", error: " << errno << std::endl;

        rtn = false;
    }

    return( rtn );
}

int main( const int argc, const char* argv[] )
{
    // Validate args.

    if( argc != 4 )
    {
        print_usage();
        return( -1 );
    }

    // What type of file are we looking at?

    std::string file_type = argv[1];
    if( stricmp( file_type.c_str(), "CSR" ) != 0 &&
        stricmp( file_type.c_str(), "CER" ) != 0 &&
        stricmp( file_type.c_str(), "PVK" ) != 0 )
    {
        print_usage();
        return( -1 );
    }

    // What form are we converting to? PEM or DER?

    std::string file_format = argv[2];
    if( stricmp( file_format.c_str(), "PEM" ) != 0 &&
        stricmp( file_format.c_str(), "DER" ) != 0 )
    {
        print_usage();
        return( -1 );
    }

    // For now, no PEM to DER support.

    if( stricmp( file_format.c_str(), "DER" ) == 0 )
    {
        std::cout << "PEM to DER conversion is not currently supported." << std::endl;
        return( -1 );
    }

    // Output file with same new but different extension.

    std::string file_name_in = argv[3];
    std::string file_name_out = file_name_in + (stricmp( file_format.c_str(), "PEM" ) == 0 ? ".pem" : ".der");

    // Open in file.

    unsigned char* raw_data = NULL;
    size_t raw_data_len = 0;
    unsigned char* conv_data = NULL;
    size_t conv_data_len = 0;

    int ret = 0;

    if( read_file( file_name_in.c_str(), raw_data, raw_data_len ) == false )
    {
        ret = -1;
        goto Cleanup;
    }

    // Base64 encode or decode.

    if( stricmp( file_format.c_str(), "PEM" ) == 0 )
    {
        // DER to PEM conversion.

        // This is just Base64 encoding plus the right header and trailer strings.

        if( Base64( true, raw_data, raw_data_len, conv_data, conv_data_len ) == false )
        {
            std::cout << "Memory allocation failed" << std::endl;
            ret = -1;
            goto Cleanup;
        }

        std::string out_data;

        // Write the header.

        if( stricmp( file_type.c_str(), "CSR" ) == 0 )
        {
            out_data = PEM_BEGIN_CSR;
        }
        else if( stricmp( file_type.c_str(), "CER" ) == 0 )
        {
            out_data = PEM_BEGIN_CERT;
        }
        else
        {
            SAVASSERT( stricmp( file_type.c_str(), "PVK" ) == 0 );
            out_data = PEM_BEGIN_RSA_KEY;
        }

        // Write the data in 64 byte chunks.

        unsigned char* conv_data_iter = conv_data;

        while( conv_data_len > 0 )
        {
            if( conv_data_len > 64 )
            {
                out_data.append( reinterpret_cast<const char*>(conv_data_iter), 64 );
                out_data += '\n';
                conv_data_iter += 64;
                conv_data_len -= 64;
            }
            else
            {
                out_data.append( reinterpret_cast<const char*>(conv_data_iter), conv_data_len );
                out_data += '\n';
                conv_data_iter += conv_data_len;
                conv_data_len = 0;
            }
        }

        // Write the trailer.

        if( stricmp( file_type.c_str(), "CSR" ) == 0 )
        {
            out_data += PEM_END_CSR;
        }
        else if( stricmp( file_type.c_str(), "CER" ) == 0 )
        {
            out_data += PEM_END_CERT;
        }
        else
        {
            SAVASSERT( stricmp( file_type.c_str(), "PVK" ) == 0 );
            out_data += PEM_END_RSA_KEY;
        }

        if( save_file( file_name_out.c_str(), reinterpret_cast<const unsigned char*>(out_data.c_str()), out_data.length() ) == false )
        {
            ret = -1;
        }

        std::cout << "Converted " << file_type.c_str() << " " << file_name_in.c_str() << " to "
                  << file_format.c_str() << " and saved to " << file_name_out.c_str() << std::endl;
    }
    else
    {
        // PEM to DER conversion.
    }

Cleanup:

    // Wipe memory.

    if( raw_data != NULL )
    {
	    SecureZeroMemory( raw_data, raw_data_len );
        delete [] raw_data;
    }

    if( conv_data != NULL )
    {
	    SecureZeroMemory( conv_data, conv_data_len );
        delete [] conv_data;
    }

    return( ret );
}

