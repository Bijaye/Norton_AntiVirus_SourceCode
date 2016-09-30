// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(__ASC_CTYPE_H)
#define __ASC_CTYPE_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dec_assert.h"

static char ASCII_UCASE[] = 
{ 
'\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', 
'\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F', 
'\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', 
'\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', '\x1E', '\x1F', 
'\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', 
'\x28', '\x29', '\x2A', '\x2B', '\x2C', '\x2D', '\x2E', '\x2F', 
'\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', 
'\x38', '\x39', '\x3A', '\x3B', '\x3C', '\x3D', '\x3E', '\x3F', 
'\x40', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47', 
'\x48', '\x49', '\x4A', '\x4B', '\x4C', '\x4D', '\x4E', '\x4F', 
'\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57', 
'\x58', '\x59', '\x5A', '\x5B', '\x5C', '\x5D', '\x5E', '\x5F', 
'\x60', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47', 
'\x48', '\x49', '\x4A', '\x4B', '\x4C', '\x4D', '\x4E', '\x4F', 
'\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57', 
'\x58', '\x59', '\x5A', '\x7B', '\x7C', '\x7D', '\x7E', '\x7F', 
'\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87', 
'\x88', '\x89', '\x8A', '\x8B', '\x8C', '\x8D', '\x8E', '\x8F', 
'\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97', 
'\x98', '\x99', '\x9A', '\x9B', '\x9C', '\x9D', '\x9E', '\x9F', 
'\xA0', '\xA1', '\xA2', '\xA3', '\xA4', '\xA5', '\xA6', '\xA7', 
'\xA8', '\xA9', '\xAA', '\xAB', '\xAC', '\xAD', '\xAE', '\xAF', 
'\xB0', '\xB1', '\xB2', '\xB3', '\xB4', '\xB5', '\xB6', '\xB7', 
'\xB8', '\xB9', '\xBA', '\xBB', '\xBC', '\xBD', '\xBE', '\xBF', 
'\xC0', '\xC1', '\xC2', '\xC3', '\xC4', '\xC5', '\xC6', '\xC7', 
'\xC8', '\xC9', '\xCA', '\xCB', '\xCC', '\xCD', '\xCE', '\xCF', 
'\xD0', '\xD1', '\xD2', '\xD3', '\xD4', '\xD5', '\xD6', '\xD7', 
'\xD8', '\xD9', '\xDA', '\xDB', '\xDC', '\xDD', '\xDE', '\xDF', 
'\xC0', '\xC1', '\xC2', '\xC3', '\xC4', '\xC5', '\xC6', '\xC7', 
'\xC8', '\xC9', '\xCA', '\xCB', '\xCC', '\xCD', '\xCE', '\xCF', 
'\xD0', '\xD1', '\xD2', '\xD3', '\xD4', '\xD5', '\xD6', '\xF7', 
'\xD8', '\xD9', '\xDA', '\xDB', '\xDC', '\xDD', '\xDE', '\xFF'
};
inline int ascii_toupper(int c) { return ASCII_UCASE[c & 0xFF]; };

static char ASCII_LCASE[] = 
{ 
'\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', 
'\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F', 
'\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', 
'\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', '\x1E', '\x1F', 
'\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', 
'\x28', '\x29', '\x2A', '\x2B', '\x2C', '\x2D', '\x2E', '\x2F', 
'\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', 
'\x38', '\x39', '\x3A', '\x3B', '\x3C', '\x3D', '\x3E', '\x3F', 
'\x40', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', 
'\x68', '\x69', '\x6A', '\x6B', '\x6C', '\x6D', '\x6E', '\x6F', 
'\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', 
'\x78', '\x79', '\x7A', '\x5B', '\x5C', '\x5D', '\x5E', '\x5F', 
'\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', 
'\x68', '\x69', '\x6A', '\x6B', '\x6C', '\x6D', '\x6E', '\x6F', 
'\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', 
'\x78', '\x79', '\x7A', '\x7B', '\x7C', '\x7D', '\x7E', '\x7F', 
'\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87', 
'\x88', '\x89', '\x8A', '\x8B', '\x8C', '\x8D', '\x8E', '\x8F', 
'\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97', 
'\x98', '\x99', '\x9A', '\x9B', '\x9C', '\x9D', '\x9E', '\x9F', 
'\xA0', '\xA1', '\xA2', '\xA3', '\xA4', '\xA5', '\xA6', '\xA7', 
'\xA8', '\xA9', '\xAA', '\xAB', '\xAC', '\xAD', '\xAE', '\xAF', 
'\xB0', '\xB1', '\xB2', '\xB3', '\xB4', '\xB5', '\xB6', '\xB7', 
'\xB8', '\xB9', '\xBA', '\xBB', '\xBC', '\xBD', '\xBE', '\xBF', 
'\xE0', '\xE1', '\xE2', '\xE3', '\xE4', '\xE5', '\xE6', '\xE7', 
'\xE8', '\xE9', '\xEA', '\xEB', '\xEC', '\xED', '\xEE', '\xEF', 
'\xF0', '\xF1', '\xF2', '\xF3', '\xF4', '\xF5', '\xF6', '\xD7', 
'\xF8', '\xF9', '\xFA', '\xFB', '\xFC', '\xFD', '\xFE', '\xDF', 
'\xE0', '\xE1', '\xE2', '\xE3', '\xE4', '\xE5', '\xE6', '\xE7', 
'\xE8', '\xE9', '\xEA', '\xEB', '\xEC', '\xED', '\xEE', '\xEF', 
'\xF0', '\xF1', '\xF2', '\xF3', '\xF4', '\xF5', '\xF6', '\xF7', 
'\xF8', '\xF9', '\xFA', '\xFB', '\xFC', '\xFD', '\xFE', '\xFF'
};
inline int ascii_tolower(int c) { return ASCII_LCASE[c & 0xFF]; };

static short int ASCII_ISLOWER[] = 
{ 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1
};
inline int ascii_islower(int c) { return ASCII_ISLOWER[c & 0xFF]; };

static short int ASCII_ISUPPER[] = 
{ 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
inline int ascii_isupper(int c) { return ASCII_ISUPPER[c & 0xFF]; };

static short int ASCII_ISDIGIT[] = 
{ 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
inline int ascii_isdigit(int c) { return ASCII_ISDIGIT[c & 0xFF]; };

static short int ASCII_ISALPHA[] = 
{ 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 
0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1
};
inline int ascii_isalpha(int c) { return ASCII_ISALPHA[c & 0xFF]; };

static short int ASCII_ISALNUM[] =
{
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1
};
inline int ascii_isalnum(int c) { return ASCII_ISALNUM[c & 0xFF]; };

static short int ASCII_ISSPACE[] =
{
0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
inline int ascii_isspace(int c) { return ASCII_ISSPACE[c & 0xFF]; };

inline int ascii_atoi(char* lpDigit)
{
	int ret_val = 0;
	int len		= 0;
	int sign	= 1;

	dec_assert(lpDigit != NULL);

	while (*lpDigit != ASC_CHR_NUL && 
		  ascii_isspace(*lpDigit) != 0)
	{
	  lpDigit++;
	}

	if (*lpDigit == ASC_CHR_MINUS)
	{
	  sign = -1;
	  lpDigit++;
	}
	else if (*lpDigit == ASC_CHR_PLUS)
	{
	  // sign = 1;
	  lpDigit++;
	}

	len = strlen(lpDigit);
	if (len < 1)
	  return 0;

	for (int i=0; i<len; i++)
	{
	  int digit;
	  int is_digit = 1;  
	  switch (lpDigit[i])
		{
			case ASC_CHR_0:
				digit = 0;
				break;
			case ASC_CHR_1:
				digit = 1;
				break;
			case ASC_CHR_2:
				digit = 2;
				break;
			case ASC_CHR_3:
				digit = 3;
				break;
			case ASC_CHR_4:
				digit = 4;
				break;
			case ASC_CHR_5:
				digit = 5;
				break;
			case ASC_CHR_6:
				digit = 6;
				break;
			case ASC_CHR_7:
				digit = 7;
				break;
			case ASC_CHR_8:
				digit = 8;
				break;
			case ASC_CHR_9:
				digit = 9;
				break;
			default:
				is_digit = 0;
				digit = 0;	// quiets down compiler warnings
				break;
		}
		if (is_digit == 1)
		  ret_val = (ret_val * 10) + digit;
		else
		  break;
	} 

	return (sign * ret_val);
}

inline int ascii_atol(char* lpDigit)
{
	long ret_val = 0;
	int len = 0;
	long sign = 1;

	dec_assert(lpDigit != NULL);

	while (*lpDigit != ASC_CHR_NUL && 
		  ascii_isspace(*lpDigit) != 0)
	{
	  lpDigit++;
	}

	if (*lpDigit == ASC_CHR_MINUS)
	{
	  sign = -1;
	  lpDigit++;
	}
	else if (*lpDigit == ASC_CHR_PLUS)
	{
	  // sign = 1;
	  lpDigit++;
	}

	len = strlen(lpDigit);
	if (len < 1)
	  return 0;

	for (int i=0; i<len; i++)
	{
	  long digit;
	  int is_digit = 1;  
	  switch (lpDigit[i])
		{
			case ASC_CHR_0:
				digit = 0;
				break;
			case ASC_CHR_1:
				digit = 1;
				break;
			case ASC_CHR_2:
				digit = 2;
				break;
			case ASC_CHR_3:
				digit = 3;
				break;
			case ASC_CHR_4:
				digit = 4;
				break;
			case ASC_CHR_5:
				digit = 5;
				break;
			case ASC_CHR_6:
				digit = 6;
				break;
			case ASC_CHR_7:
				digit = 7;
				break;
			case ASC_CHR_8:
				digit = 8;
				break;
			case ASC_CHR_9:
				digit = 9;
				break;
			default:
				is_digit = 0;
		}
		if (is_digit == 1)
		  ret_val = (ret_val * 10) + digit;
		else
		  break;
	} 

	return (sign * ret_val);
}

/////////////////////////////////////////////////////////////////////
// Removed Until Needed (RUN)
#if 0

#include <math.h>	// for pow() prototype

static void interpret_digits(const char **p, float *f, int after_decimal)
{
   float multiplier = 1;
 
   while(ascii_isdigit(**p)) {
		if(!after_decimal) {
			*f *= 10;
		}
		else {
			multiplier /= 10;
		}

		switch(**p) {
		case ASC_CHR_0:
			break;

		case ASC_CHR_1:
			*f += multiplier;
			break;

		case ASC_CHR_2:
			*f += (2 * multiplier);
			break;

		case ASC_CHR_3:
			*f += (3 * multiplier);
			break;

		case ASC_CHR_4:
			*f += (4 * multiplier);
			break;

		case ASC_CHR_5:
			*f += (5 * multiplier);
			break;

		case ASC_CHR_6:
			*f += (6 * multiplier);
			break;

		case ASC_CHR_7:
			*f += (7 * multiplier);
			break;

		case ASC_CHR_8:
			*f += (8 * multiplier);
			break;

		case ASC_CHR_9:
			*f += (9 * multiplier);
			break;

		default:
			dec_assert(0);
			break;
		}

		++(*p);
	}
}

static float ascii_atof(const char *num)
{
	const char *p = num;
	int sign = 1;
	float rc = 0;
	float exponent = 0;
	int exponent_sign = 1;

	dec_assert(num != NULL);

	if(*p == ASC_CHR_MINUS) {
		sign = -1;
		++p;
	}
	else if(*p == ASC_CHR_SPACE) {
		++p;
	}

	interpret_digits(&p, &rc, 0);

	if(*p == ASC_CHR_PERIOD) {
		++p;
		interpret_digits(&p, &rc, 1);
	}

	if((*p == ASC_CHR_e) || (*p == ASC_CHR_E)) {
		++p;
		if(*p == ASC_CHR_MINUS) {
			++p;
			exponent_sign = -1;
		}

		interpret_digits(&p, &exponent, 0);
	}

	exponent = (float)pow(10, (exponent * (float)exponent_sign));
	return rc * exponent * (float)sign;
}
#endif

// END Removed Until Needed (End RUN)
/////////////////////////////////////////////////////////////////////

inline char* ascii_btoa(int value, char* dest)
{
   // numeric-to-ascii-char map
   static char map[16] =
   { 
	 ASC_CHR_0, ASC_CHR_1, ASC_CHR_2, ASC_CHR_3,
	 ASC_CHR_4, ASC_CHR_5, ASC_CHR_6, ASC_CHR_7,  
	 ASC_CHR_8, ASC_CHR_9, ASC_CHR_A, ASC_CHR_B,
	 ASC_CHR_C, ASC_CHR_D, ASC_CHR_E, ASC_CHR_F
   };

   // Note: MUST always write three bytes.
   dest[0] = map[(value >> 4) & 0xF];
   dest[1] = map[value	& 0xF];
   dest[2] = 0;

   return dest;
}

inline char* ascii_itoa(int value, char* dest, int radix)
{
   // only support radix values between 2-16.
   dec_assert(radix >= 2 && radix <=16);
   if (radix < 2 || radix > 16)
	 return NULL;

   // numeric-to-ascii-char map
   static char map[16] =
   { 
	 ASC_CHR_0, ASC_CHR_1, ASC_CHR_2, ASC_CHR_3,
	 ASC_CHR_4, ASC_CHR_5, ASC_CHR_6, ASC_CHR_7,  
	 ASC_CHR_8, ASC_CHR_9, ASC_CHR_A, ASC_CHR_B,
	 ASC_CHR_C, ASC_CHR_D, ASC_CHR_E, ASC_CHR_F
   };

   // actual itoa uses up to 33 bytes for wide chars, we use 36 chars.
   char buff[36];
   int original = value;
   int i = 0;

   buff[0] = buff[1] = 0;
   // special case for a null value
   if (value == 0)
   {
	 buff[0] = ASC_CHR_0;
   }
   else
   {  // build the string in reverse
	 value = abs(value);
	 while (value != 0)
	 {
		buff[i++] = map[ value % radix ];
		value /= radix;
	 }

	 // for whatever reason, the itoa definition says that only in base-10 if the number
	 //  is negative do we prepend the "minus" sign. We do it all the time, though just
	 //  because it's the right thing to do. Note: buff[i] should already reference the
	 //  next character in the array that is available for writing.
	 if (original < 0)
		buff[i++] = ASC_CHR_MINUS;

	 // null-terminate
	 buff[i] = 0;
   }

   // now reverse the string to the destination.
   int j = 0;
   i = strlen(buff);
   while (i > 0)
	 dest[j++] = buff[--i];
   dest[j] = 0;
	
   return dest;
}

inline char* ascii_ltoa(long value, char* dest, int radix)
{
   // only support radix values between 2-16.
   dec_assert(radix >= 2 && radix <=16);
   if (radix < 2 || radix > 16)
	 return NULL;

   // numeric-to-ascii-char map
   static char map[16] =
   { 
	 ASC_CHR_0, ASC_CHR_1, ASC_CHR_2, ASC_CHR_3,
	 ASC_CHR_4, ASC_CHR_5, ASC_CHR_6, ASC_CHR_7,  
	 ASC_CHR_8, ASC_CHR_9, ASC_CHR_A, ASC_CHR_B,
	 ASC_CHR_C, ASC_CHR_D, ASC_CHR_E, ASC_CHR_F
   };

   // actual itoa uses up to 33 bytes for wide chars, we use 36 chars.
   char buff[36];
   long original = value;
   int i = 0;

   buff[0] = buff[1] = 0;
   // special case for a null value
   if (value == 0)
   {
	 buff[0] = ASC_CHR_0;
   }
   else
   {  // build the string in reverse
	 value = labs(value);
	 while (value != 0)
	 {
		buff[i++] = map[ value % (long)radix ];
		value /= (long)radix;
	 }

	 // for whatever reason, the itoa definition says that only in base-10 if the number
	 //  is negative do we prepend the "minus" sign. We do it all the time, though just
	 //  because it's the right thing to do. Note: buff[i] should already reference the
	 //  next character in the array that is available for writing.
	 if (original < 0)
		buff[i++] = ASC_CHR_MINUS;

	 // null-terminate
	 buff[i] = 0;
   }

   // now reverse the string to the destination.
   int j = 0;
   i = strlen(buff);
   while (i > 0)
	 dest[j++] = buff[--i];
   dest[j] = 0;
	
   return dest;
}

/* Version for unsigned longs */
static char* ascii_ultoa(unsigned long value, char* dest, int radix)
{
   // only support radix values between 2-16.
   dec_assert(radix >= 2 && radix <=16);
   if (radix < 2 || radix > 16)
	 return NULL;

   // numeric-to-ascii-char map
   static char map[16] =
   { 
	 ASC_CHR_0, ASC_CHR_1, ASC_CHR_2, ASC_CHR_3,
	 ASC_CHR_4, ASC_CHR_5, ASC_CHR_6, ASC_CHR_7,  
	 ASC_CHR_8, ASC_CHR_9, ASC_CHR_A, ASC_CHR_B,
	 ASC_CHR_C, ASC_CHR_D, ASC_CHR_E, ASC_CHR_F
   };

   // actual itoa uses up to 33 bytes for wide chars, we use 36 chars.
   char buff[36];
   unsigned long original = value;
   int i = 0;

   buff[0] = buff[1] = 0;
   // special case for a null value
   if (value == 0)
   {
	 buff[0] = ASC_CHR_0;
   }
   else
   {  // build the string in reverse
	 while (value != 0)
	 {
		buff[i++] = map[ value % (unsigned long)radix ];
		value /= (unsigned long)radix;
	 }

	 buff[i] = 0;
   }

   // now reverse the string to the destination.
   int j = 0;
   i = strlen(buff);
   while (i > 0)
	 dest[j++] = buff[--i];
   dest[j] = 0;
	
   return dest;
}

/* Convert a float to a string. We use sprintf to convert it to
a native string, and then convert the characters in that string
to ASCII. */
static void ascii_ftoa(float value, char *string)
{
	int i;
	int len;

	dec_assert(string != NULL);
	sprintf(string, "%f", value);

	len = strlen(string);

	for(i = 0; i < len; ++i) {
		switch(string[i]) {
		case '0':
			string[i] = ASC_CHR_0;
			break;

		case '1':
			string[i] = ASC_CHR_1;
			break;

		case '2':
			string[i] = ASC_CHR_2;
			break;

		case '3':
			string[i] = ASC_CHR_3;
			break;

		case '4':
			string[i] = ASC_CHR_4;
			break;

		case '5':
			string[i] = ASC_CHR_5;
			break;

		case '6':
			string[i] = ASC_CHR_6;
			break;

		case '7':
			string[i] = ASC_CHR_7;
			break;

		case '8':
			string[i] = ASC_CHR_8;
			break;

		case '9':
			string[i] = ASC_CHR_9;
			break;

		case '.':
			string[i] = ASC_CHR_PERIOD;
			break;

		case '-':
			string[i] = ASC_CHR_MINUS;
			break;

		case 'e':
			string[i] = ASC_CHR_e;
			break;

		case 'E':
			string[i] = ASC_CHR_E;
			break;

		default:
			dec_assert(0);
			string[i] = ASC_CHR_0;
			break;
		}
	}
}

#endif // __ASC_CTYPE_H
