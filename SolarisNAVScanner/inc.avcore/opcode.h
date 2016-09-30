// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/OPCODE.H_v   1.0   01 Feb 1996 10:17:44   CNACHEN  $
//
// Description:
//
//  This source file contains constants for each of the different opcodes
//  supported by the CPU emulator.  The binary notation may have been a silly
//  choice, but it does make instruction decoding easier :)
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/OPCODE.H_v  $
// 
//    Rev 1.0   01 Feb 1996 10:17:44   CNACHEN
// Initial revision.
// 
//    Rev 1.5   12 Jan 1996 12:05:28   CNACHEN
// Added floating point support (to skip over float instructions)
// 
//    Rev 1.4   20 Dec 1995 12:19:36   CNACHEN
// Added 0F,AF, 69, 6B IMUL instructions...
// 
//    Rev 1.3   19 Oct 1995 18:39:16   CNACHEN
// Initial revision... With comment headers... :)
//************************************************************************


/* ADC */

#define oc00010000b		0x10
#define	oc00010001b		0x11
#define oc00010010b		0x12
#define oc00010011b		0x13

#define oc10000000b		0x80
#define oc10000001b		0x81
#define oc10000010b		0x82
#define oc10000011b		0x83

#define oc00010100b		0x14
#define oc00010101b		0x15

/* ADD */

#define oc00000000b		0x00
#define oc00000001b		0x01
#define oc00000010b		0x02
#define oc00000011b		0x03

#define oc10000000b		0x80
#define oc10000001b		0x81
#define oc10000010b		0x82
#define	oc10000011b		0x83

#define oc00000100b		0x04
#define oc00000101b		0x05

/* CALL */

#define oc11101000b		0xe8
#define oc11111111b		0xff
#define oc10011010b		0x9a

/* CBD */

#define oc10011000b		0x98

/* CLC */

#define oc11111000b		0xf8

/* CLD */

#define oc11111100b		0xfc

/* CLI */

#define oc11111010b		0xfa

/* CMC */

#define oc11110101b		0xf5

/* CMP */

#define oc00111000b		0x38
#define oc00111001b		0x39
#define oc00111010b		0x3a
#define oc00111011b		0x3b

#define oc00111100b		0x3c
#define oc00111101b		0x3d

/* CWD */

#define oc10011001b		0x99

/* DEC */

#define oc11111110b		0xfe

/* DIV/IDIV */

#define oc11110110b		0xf6
#define oc11110111b		0xf7


#define oc01001000b		0x48
#define oc01001001b		0x49
#define oc01001010b		0x4a
#define oc01001011b		0x4b
#define oc01001100b		0x4c
#define oc01001101b		0x4d
#define oc01001110b		0x4e
#define oc01001111b		0x4f

/* INC */

#define oc01000000b		0x40
#define oc01000001b		0x41
#define oc01000010b		0x42
#define oc01000011b		0x43
#define oc01000100b		0x44
#define oc01000101b		0x45
#define oc01000110b		0x46
#define oc01000111b		0x47

/* IRET */

#define oc11001111b		0xCF

/* JCOND */

#define oc01110000b		0x70
#define oc01110001b		0x71
#define oc01110010b		0x72
#define oc01110011b		0x73
#define oc01110100b		0x74
#define oc01110101b		0x75
#define oc01110110b		0x76
#define oc01110111b		0x77
#define oc01111000b		0x78
#define oc01111001b		0x79
#define oc01111010b		0x7a
#define oc01111011b		0x7b
#define oc01111100b		0x7c
#define oc01111101b		0x7d
#define oc01111110b		0x7e
#define oc01111111b		0x7f

/* jcxz */

#define oc11100011b		0xe3

/* jmp */

#define oc11101011b		0xeb
#define oc11111111b		0xff
#define oc11101010b		0xea
#define oc11101001b		0xe9

/* lahf */

#define oc10011111b		0x9f

/* sahf */

#define oc10011110b     0x9E

/* lds */

#define oc11000101b		0xc5

/* lea */

#define oc10001101b		0x8d

/* les */

#define oc11000100b		0xc4

/* lodsb */

#define oc10101100b		0xac
#define oc10101101b		0xad

/* loop */

#define oc11100010b		0xe2

/* loopz */

#define oc11100001b		0xe1

/* loopnz */

#define oc11100000b		0xe0

/* mov */

#define oc10001000b		0x88
#define oc10001001b		0x89
#define oc10001010b		0x8a
#define oc10001011b		0x8b

#define oc10110000b		0xb0
#define oc10110001b		0xb1
#define oc10110010b		0xb2
#define oc10110011b		0xb3
#define oc10110100b		0xb4
#define oc10110101b		0xb5
#define oc10110110b		0xb6
#define oc10110111b		0xb7
#define oc10111000b		0xb8
#define oc10111001b		0xb9
#define oc10111010b		0xba
#define oc10111011b		0xbb
#define oc10111100b		0xbc
#define oc10111101b		0xbd
#define oc10111110b		0xbe
#define oc10111111b		0xbf

#define oc11000110b		0xc6
#define oc11000111b		0xc7

#define oc10100000b		0xa0
#define oc10100001b		0xa1

#define oc10100010b		0xa2
#define oc10100011b		0xa3

#define oc10001110b		0x8e
#define oc10001100b		0x8c

/* movs */

#define oc10100100b		0xa4
#define oc10100101b		0xa5

/* or */

#define oc00001000b		0x08
#define oc00001001b		0x09
#define oc00001010b		0x0a
#define oc00001011b		0x0b

#define oc00001100b		0x0c
#define oc00001101b		0x0d

/* pop */

#define oc01011000b		0x58
#define oc01011001b		0x59
#define oc01011010b		0x5a
#define oc01011011b		0x5b
#define oc01011100b		0x5c
#define oc01011101b		0x5d
#define oc01011110b		0x5e
#define oc01011111b		0x5f

#define oc10001111b		0x8f

/* pop sregs */

#define oc00000111b		0x07
#define oc00010111b		0x17
#define oc00011111b		0x1f

/* prefix for JCond's! */

#define oc00001111b     0x0f
        
/* popf */

#define oc10011101b		0x9d

/* push */

#define oc01010000b		0x50
#define oc01010001b		0x51
#define oc01010010b		0x52
#define oc01010011b		0x53
#define oc01010100b		0x54
#define oc01010101b		0x55
#define oc01010110b		0x56
#define oc01010111b		0x57

/* push sreg */

#define oc00000110b		0x06
#define oc00001110b		0x0e
#define oc00010110b		0x16
#define oc00011110b		0x1e

/* pushf */

#define oc10011100b		0x9c

/* rcl */

#define oc11010000b		0xd0
#define oc11010001b		0xd1

#define oc11010010b		0xd2
#define oc11010011b		0xd3

/* rots (i8) */

#define oc11000000b     0xC0
#define oc11000001b     0xC1


/* repnz */

#define oc11110010b		0xf2

/* repz */

#define oc11110011b		0xf3

/* ret stuff */

#define oc11000011b		0xc3
#define oc11000010b		0xc2
#define oc11001011b		0xcb
#define oc11001010b		0xca

/* sbb */

#define oc00011000b		0x18
#define oc00011001b		0x19
#define oc00011010b		0x1a
#define oc00011011b		0x1b

#define oc00011100b		0x1c
#define oc00011101b		0x1d

/* SEG */

#define oc00100110b		0x26
#define oc00101110b		0x2e
#define oc00110110b		0x36
#define oc00111110b		0x3e

#define oc01100100b     0x64
#define oc01100101b     0x65

/* STC */

#define oc11111001b		0xf9

/* STD */

#define oc11111101b		0xfd

/* STI */

#define oc11111011b		0xfb

/* STOS */

#define oc10101010b		0xaa
#define oc10101011b		0xab

/* SUB */

#define oc00101000b		0x28
#define oc00101001b		0x29
#define oc00101010b		0x2a
#define oc00101011b		0x2b

#define oc00101100b		0x2c
#define oc00101101b		0x2d

/* XCHG */

#define oc10000110b		0x86
#define oc10000111b		0x87

#define oc10010000b		0x90
#define oc10010001b		0x91
#define oc10010010b		0x92
#define oc10010011b		0x93
#define oc10010100b		0x94
#define oc10010101b		0x95
#define oc10010110b		0x96
#define oc10010111b		0x97

/* XOR */

#define oc00110000b		0x30
#define oc00110001b		0x31
#define oc00110010b		0x32
#define oc00110011b		0x33

#define oc00110100b		0x34
#define oc00110101b		0x35

/* AND */

#define oc00100000b		0x20
#define oc00100001b		0x21
#define oc00100010b		0x22
#define oc00100011b		0x23

#define oc00100100b		0x24
#define oc00100101b		0x25

/* TEST */

#define oc10000100b		0x84
#define oc10000101b		0x85

#define oc10101000b		0xa8
#define oc10101001b		0xa9


/* int */

#define oc11001100b		0xCC
#define oc11001101b		0xCD


/* CMPS */

#define oc10100110b		0xA6
#define oc10100111b		0xA7


/* SCAS */

#define oc10101110b     0xAE
#define oc10101111b     0xAF


/* OPERAND SIZE OVERRIDE */

#define oc01100110b     0x66

/* ADDRESS SIZE OVERRIDE */

#define oc01100111b     0x67

/* AAA */

#define oc00110111b     0x37

/* AAD */

#define oc11010101b     0xD5

/* AAM */

#define oc11010100b     0xD4

/* AAS */

#define oc00111111b     0x3F


/* DAA */

#define oc00100111b     0x27

/* DAS */

#define oc00101111b     0x2F

/* ENTER */

#define oc11001000b     0xC8

/* IN */

#define oc11100100b     0xE4
#define oc11100101b     0xE5
#define oc11101100b     0xEC
#define oc11101101b     0xED

/* INSB/INSW/INSD */

#define oc01101100b     0x6C
#define oc01101101b     0x6D

/* INTO */

#define oc11001110b     0xCE

/* LEAVE */

#define oc11001001b     0xC9


/* LOCK */

#define oc11110000b     0xF0


/* OUT */

#define OUT_8_AL        0xE6
#define OUT_8_AX        0xE7
#define OUT_DX_AL       0xEE
#define OUT_DX_AX       0xEF

/* OUTS */

#define OUTS_1          0x6E
#define OUTS_2          0x6F


/* POPA/POPAD */

#define POPA_OPCODE     0x61


/* PUSH IMMED */

#define PUSH_I8_OPCODE  0x6A
#define PUSH_I16_OPCODE 0x68

/* PUSHA/PUSHAD */

#define PUSHA_OPCODE    0x60

/* WAIT */

#define WAIT_OPCODE     0x9B


/* XLAT */

#define XLAT_OPCODE     0xD7


/* ARPL */

#define ARPL_OPCODE     0x63

/* SETALC */

#define SETALC_OPCODE   0xD6

/* IMUL */

#define IMUL_69_OPCODE  0x69
#define IMUL_6B_OPCODE  0x6B
#define IMUL_AF_OPCODE  0xAF

#define BOUND_OPCODE    0x62

#define FLOAT_OPCODE1   0xD8
#define FLOAT_OPCODE2   0xD9
#define FLOAT_OPCODE3   0xDA
#define FLOAT_OPCODE4   0xDB
#define FLOAT_OPCODE5   0xDC
#define FLOAT_OPCODE6   0xDD
#define FLOAT_OPCODE7   0xDE
#define FLOAT_OPCODE8   0xDF

#define RESERVED_OPCODE1 0xF1
#define RESERVED_OPCODE2 0xF4       // halt actually

