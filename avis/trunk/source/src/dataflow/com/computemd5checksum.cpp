#include <stdafx.h>
#include "computeMD5CheckSum.h"
#include "AVISFile.h"
#include "MD5Calculator.h"

std::string computeMD5CheckSum (std::string fileName)
{
    std::string checkSumString;

    FILE *fp;

    checkSumString = "";
    fp = fopen (fileName.c_str(), "rb");

    if (fp)
    {
        MD5Calculator MD5Calc;
        uchar *buffer;
        uint bufferSize;
#define MD5_BUFFER_SIZE (10000 * 1024)
        buffer = (uchar *) malloc (MD5_BUFFER_SIZE);
        while (1)
        {
            bufferSize = fread (buffer, sizeof(uchar), MD5_BUFFER_SIZE, fp);
            if (!bufferSize)
                break;

            MD5Calc.CRCCalculate(buffer, bufferSize);
        }
        MD5Calc.Done(checkSumString);
        fclose (fp);
        free (buffer);
    }

    return (checkSumString);
}
