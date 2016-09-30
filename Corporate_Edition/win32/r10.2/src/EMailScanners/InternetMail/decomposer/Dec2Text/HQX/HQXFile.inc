// HQXFile.inc
// Copyright 2001 Symantec, Peter Norton Product Group
//************************************************************************

#if !defined(CPU_BIG_ENDIAN)
#define SWAPWORD(w)    ( (WORD)((WORD)(w) >> 8) | (WORD)((WORD)(w) << 8) )
#define SWAPDWORD(dw)  ( (DWORD)((DWORD)(dw) >> 24)              | \
                         (DWORD)((DWORD)(dw) << 24)              | \
                         (DWORD)(((DWORD)(dw) >> 8) & 0xFF00U)   | \
                         (DWORD)(((DWORD)(dw) << 8) & 0xFF0000UL) )
#endif

// (This file must be converted with BinHex 4.0)<CR><LF><:>
static const char gszHQXIdentifier[] = 
{
    ASC_CHR_LPAREN,
    ASC_CHR_T,
    ASC_CHR_h,
    ASC_CHR_i,
    ASC_CHR_s,
    ASC_CHR_SPACE,
    ASC_CHR_f,
    ASC_CHR_i,
    ASC_CHR_l,
    ASC_CHR_e,
    ASC_CHR_SPACE,
    ASC_CHR_m,
    ASC_CHR_u,
    ASC_CHR_s,
    ASC_CHR_t,
    ASC_CHR_SPACE,
    ASC_CHR_b,
    ASC_CHR_e,
    ASC_CHR_SPACE,
    ASC_CHR_c,
    ASC_CHR_o,
    ASC_CHR_n,
    ASC_CHR_v,
    ASC_CHR_e,
    ASC_CHR_r,
    ASC_CHR_t,
    ASC_CHR_e,
    ASC_CHR_d,
    ASC_CHR_SPACE,
    ASC_CHR_w,
    ASC_CHR_i,
    ASC_CHR_t,
    ASC_CHR_h,
    ASC_CHR_SPACE,
    ASC_CHR_B,
    ASC_CHR_i,
    ASC_CHR_n,
    ASC_CHR_H,
    ASC_CHR_e,
    ASC_CHR_x,
    ASC_CHR_SPACE,
    ASC_CHR_4,
    ASC_CHR_PERIOD,
    ASC_CHR_0,
    ASC_CHR_RPAREN,
    ASC_CHR_CR,
    ASC_CHR_LF,
    ASC_CHR_COLON,
    ASC_CHR_NUL
};

// Array of valid BinHex characters.  Valid characters start at ASCII 33, so if you subtract
// ASC_SPACE (32) from the input, you will get an index into this table.  Invalid BinHex
// characters are represented by 0xFF.
static unsigned char gszDecodeTable[] = 
{
    0xFF,       // Invalid
    0x00,       // !
    0x01,       // "
    0x02,       // #
    0x03,       // $
    0x04,       // %
    0x05,       // &
    0x06,       // '
    0x07,       // (
    0x08,       // )
    0x09,       // *
    0x0A,       // +
    0x0B,       // ,
    0x0C,       // -
    0xFF,       // Invalid  .
    0xFF,       // Invalid  <backslash>
    0x0D,       // 0
    0x0E,       // 1
    0x0F,       // 2
    0x10,       // 3
    0x11,       // 4
    0x12,       // 5
    0x13,       // 6
    0xFF,       // Invalid  7
    0x14,       // 8
    0x15,       // 9
    0xFF,       // Invalid  :
    0xFF,       // Invalid  ;
    0xFF,       // Invalid  < 
    0xFF,       // Invalid  = 
    0xFF,       // Invalid  > 
    0xFF,       // Invalid  ?
    0x16,       // @
    0x17,       // A
    0x18,       // B
    0x19,       // C
    0x1A,       // D
    0x1B,       // E
    0x1C,       // F
    0x1D,       // G
    0x1E,       // H
    0x1F,       // I
    0x20,       // J
    0x21,       // K
    0x22,       // L
    0x23,       // M
    0x24,       // N
    0xFF,       // Invalid  0
    0x25,       // P
    0x26,       // Q
    0x27,       // R
    0x28,       // S
    0x29,       // T
    0x2A,       // U
    0x2B,       // V
    0xFF,       // Invalid  W
    0x2C,       // X
    0x2D,       // Y
    0x2E,       // Z
    0x2F,       // [
    0xFF,       // Invalid  <forward slash> 
    0xFF,       // Invalid  ] 
    0xFF,       // Invalid  ^ 
    0xFF,       // Invalid  _
    0x30,       // `
    0x31,       // a
    0x32,       // b
    0x33,       // c
    0x34,       // d
    0x35,       // e
    0x36,       // f
    0xFF,       // Invalid  g
    0x37,       // h
    0x38,       // i
    0x39,       // j
    0x3A,       // k
    0x3B,       // l
    0x3C,       // m
    0xFF,       // Invalid  n 
    0xFF,       // Invalid  o
    0x3D,       // p
    0x3E,       // q
    0x3F        // r
};

static unsigned char gszEncodeTable[] = 
{
    //
    // Chars 0 - 32 are invalid.
    //
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,

    ASC_CHR_BANG,     // 0x00
    ASC_CHR_DQUOTE,   // 0x01
    ASC_CHR_POUND,    // 0x02 
    ASC_CHR_DOLLAR,   // 0x03
    ASC_CHR_PERCENT,  // 0x04
    ASC_CHR_AMPSND,   // 0x05
    ASC_CHR_QUOTE,    // 0x06
    ASC_CHR_LPAREN,   // 0x07
    ASC_CHR_RPAREN,   // 0x08
    ASC_CHR_STAR,     // 0x09
    ASC_CHR_PLUS,     // 0x0A
    ASC_CHR_COMMA,    // 0x0B
    ASC_CHR_MINUS,    // 0x0C
    ASC_CHR_0,        // 0x0D
    ASC_CHR_1,        // 0x0E
    ASC_CHR_2,        // 0x0F
    ASC_CHR_3,        // 0x10
    ASC_CHR_4,        // 0x11
    ASC_CHR_5,        // 0x12
    ASC_CHR_6,        // 0x13
    ASC_CHR_8,        // 0x14
    ASC_CHR_9,        // 0x15
    ASC_CHR_AT,       // 0x16
    ASC_CHR_A,        // 0x17
    ASC_CHR_B,        // 0x18
    ASC_CHR_C,        // 0x19
    ASC_CHR_D,        // 0x1A
    ASC_CHR_E,        // 0x1B
    ASC_CHR_F,        // 0x1C
    ASC_CHR_G,        // 0x1D
    ASC_CHR_H,        // 0x1E
    ASC_CHR_I,        // 0x1F
    ASC_CHR_J,        // 0x20
    ASC_CHR_K,        // 0x21
    ASC_CHR_L,        // 0x22
    ASC_CHR_M,        // 0x23
    ASC_CHR_N,        // 0x24
    ASC_CHR_P,        // 0x25
    ASC_CHR_Q,        // 0x26
    ASC_CHR_R,        // 0x27
    ASC_CHR_S,        // 0x28
    ASC_CHR_T,        // 0x29
    ASC_CHR_U,        // 0x2A
    ASC_CHR_V,        // 0x2B
    ASC_CHR_X,        // 0x2C
    ASC_CHR_Y,        // 0x2D
    ASC_CHR_Z,        // 0x2E
    ASC_CHR_LBRACK,   // 0x2F
    ASC_CHR_GRAVE,    // 0x30
    ASC_CHR_a,        // 0x31
    ASC_CHR_b,        // 0x32
    ASC_CHR_c,        // 0x33
    ASC_CHR_d,        // 0x34
    ASC_CHR_e,        // 0x35
    ASC_CHR_f,        // 0x36
    ASC_CHR_h,        // 0x37
    ASC_CHR_i,        // 0x38
    ASC_CHR_j,        // 0x39
    ASC_CHR_k,        // 0x3A
    ASC_CHR_l,        // 0x3B
    ASC_CHR_m ,       // 0x3C
    ASC_CHR_p,        // 0x3D
    ASC_CHR_q,        // 0x3E
    ASC_CHR_r         // 0x3F
};
