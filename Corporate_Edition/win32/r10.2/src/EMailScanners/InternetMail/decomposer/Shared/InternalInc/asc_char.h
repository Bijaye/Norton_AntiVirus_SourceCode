// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef __ASC_CHAR_H__
#define __ASC_CHAR_H__

/************************************************************/
/* ASCII/ISO 8859-1 character code equivalents              */
/*                                                          */
/* (C) 2001, Symantec, Inc.  All rights reserved.           */
/*                                                          */
/* History:                                                 */
/*                                                          */
/* 20000620.000 - faa - Initial revision.                   */
/*                                                          */
/************************************************************/

/* Control characters */
#define ASC_CHR_NUL		'\x00'
#define ASC_CHR_SOH		'\x01'
#define ASC_CHR_STX		'\x02'
#define ASC_CHR_ETX		'\x03'
#define ASC_CHR_EOI		'\x04'
#define ASC_CHR_ENQ		'\x05'
#define ASC_CHR_ACK		'\x06'
#define ASC_CHR_BEL		'\x07'
#define ASC_CHR_BS		'\x08'
#define ASC_CHR_HT		'\x09'
#define ASC_CHR_LF		'\x0a'
#define ASC_CHR_VT		'\x0b'
#define ASC_CHR_FF		'\x0c'
#define ASC_CHR_CR		'\x0d'
#define ASC_CHR_SO		'\x0e'
#define ASC_CHR_SI		'\x0f'

#define ASC_CHR_SLE		'\x10'
#define ASC_CHR_CS1		'\x11'
#define ASC_CHR_DC2		'\x12'
#define ASC_CHR_DC3		'\x13'
#define ASC_CHR_DC4		'\x14'
#define ASC_CHR_NAK		'\x15'
#define ASC_CHR_SYN		'\x16'
#define ASC_CHR_ETB		'\x17'
#define ASC_CHR_CAN		'\x18'
#define ASC_CHR_EM		'\x19'
#define ASC_CHR_STB		'\x1a'
#define ASC_CHR_ESC		'\x1b'
#define ASC_CHR_FS		'\x1c'
#define ASC_CHR_GS		'\x1d'
#define ASC_CHR_RS		'\x1e'
#define ASC_CHR_US		'\x1f'

/* Control character aliases */
#define ASC_CHR_TAB		ASC_CHR_HT
#define ASC_CHR_LINEFEED	ASC_CHR_LF
#define ASC_CHR_VTAB		ASC_CHR_VT
#define ASC_CHR_FORMFEED	ASC_CHR_FF

/* Control character literal equivalents */
#define ASC_CHR_CTL_AT		ASC_CHR_NUL
#define ASC_CHR_CTL_A		ASC_CHR_SOH
#define ASC_CHR_CTL_B		ASC_CHR_STX
#define ASC_CHR_CTL_C		ASC_CHR_ETX
#define ASC_CHR_CTL_D		ASC_CHR_EOI
#define ASC_CHR_CTL_E		ASC_CHR_ENQ
#define ASC_CHR_CTL_F		ASC_CHR_ACK
#define ASC_CHR_CTL_G		ASC_CHR_BEL
#define ASC_CHR_CTL_H		ASC_CHR_BS
#define ASC_CHR_CTL_I		ASC_CHR_HT
#define ASC_CHR_CTL_J		ASC_CHR_LF
#define ASC_CHR_CTL_K		ASC_CHR_VT
#define ASC_CHR_CTL_L		ASC_CHR_FF
#define ASC_CHR_CTL_M		ASC_CHR_CR
#define ASC_CHR_CTL_N		ASC_CHR_SO
#define ASC_CHR_CTL_O		ASC_CHR_SI

#define ASC_CHR_CTL_P		ASC_CHR_SLE
#define ASC_CHR_CTL_Q		ASC_CHR_CS1
#define ASC_CHR_CTL_R		ASC_CHR_DC2
#define ASC_CHR_CTL_S		ASC_CHR_DC3
#define ASC_CHR_CTL_T		ASC_CHR_DC4
#define ASC_CHR_CTL_U		ASC_CHR_NAK
#define ASC_CHR_CTL_V		ASC_CHR_SYN
#define ASC_CHR_CTL_W		ASC_CHR_ETB
#define ASC_CHR_CTL_X		ASC_CHR_CAN
#define ASC_CHR_CTL_Y		ASC_CHR_EM
#define ASC_CHR_CTL_Z		ASC_CHR_STB
#define ASC_CHR_CTL_LBRACK	ASC_CHR_ESC
#define ASC_CHR_CTL_BKSLSH	ASC_CHR_FS
#define ASC_CHR_CTL_RBRACK	ASC_CHR_GS
#define ASC_CHR_CTL_CARET	ASC_CHR_RS
#define ASC_CHR_CTL_ULINE	ASC_CHR_US

/* Printing characters - punctuation, numbers */
#define ASC_CHR_SPACE	'\x20'
#define ASC_CHR_BANG	'\x21'
#define ASC_CHR_DQUOTE	'\x22'
#define ASC_CHR_POUND	'\x23'
#define ASC_CHR_DOLLAR	'\x24'
#define ASC_CHR_PERCENT '\x25'
#define ASC_CHR_AMPSND	'\x26'
#define ASC_CHR_QUOTE	'\x27'
#define ASC_CHR_LPAREN	'\x28'
#define ASC_CHR_RPAREN	'\x29'
#define ASC_CHR_STAR	'\x2a'
#define ASC_CHR_PLUS	'\x2b'
#define ASC_CHR_COMMA	'\x2c'
#define ASC_CHR_MINUS	'\x2d'
#define ASC_CHR_PERIOD	'\x2e'
#define ASC_CHR_FSLASH	'\x2f'

#define ASC_CHR_0	'\x30'
#define ASC_CHR_1	'\x31'
#define ASC_CHR_2	'\x32'
#define ASC_CHR_3	'\x33'
#define ASC_CHR_4	'\x34'
#define ASC_CHR_5	'\x35'
#define ASC_CHR_6	'\x36'
#define ASC_CHR_7	'\x37'
#define ASC_CHR_8	'\x38'
#define ASC_CHR_9	'\x39'
#define ASC_CHR_COLON	'\x3a'
#define ASC_CHR_SEMI	'\x3b'
#define ASC_CHR_LT	'\x3c'
#define ASC_CHR_EQ	'\x3d'
#define ASC_CHR_GT	'\x3e'
#define ASC_CHR_QUESTION '\x3f'

/* Aliases */
#define ASC_CHR_EXCLM	ASC_CHR_BANG
#define ASC_CHR_OCTHRP	ASC_CHR_POUND
#define ASC_CHR_SQUOTE	ASC_CHR_QUOTE
#define ASC_CHR_AND ASC_CHR_AMPSND
#define ASC_CHR_TIMES	ASC_CHR_STAR
#define ASC_CHR_DASH	ASC_CHR_MINUS
#define ASC_CHR_HYPHEN	ASC_CHR_MINUS
#define ASC_CHR_DOT ASC_CHR_PERIOD
#define ASC_CHR_SLASH	ASC_CHR_FSLASH
#define ASC_CHR_DIV ASC_CHR_FSLASH
#define ASC_CHR_ZERO	ASC_CHR_0
#define ASC_CHR_ONE ASC_CHR_1
#define ASC_CHR_TWO ASC_CHR_2
#define ASC_CHR_THREE	ASC_CHR_3
#define ASC_CHR_FOUR	ASC_CHR_4
#define ASC_CHR_FIVE	ASC_CHR_5
#define ASC_CHR_SIX ASC_CHR_6
#define ASC_CHR_SEVEN	ASC_CHR_7
#define ASC_CHR_EIGHT	ASC_CHR_8
#define ASC_CHR_NINE	ASC_CHR_9

/* Upper-case letters */
#define ASC_CHR_AT		'\x40'
#define ASC_CHR_A		'\x41'
#define ASC_CHR_B		'\x42'
#define ASC_CHR_C		'\x43'
#define ASC_CHR_D		'\x44'
#define ASC_CHR_E		'\x45'
#define ASC_CHR_F		'\x46'
#define ASC_CHR_G		'\x47'
#define ASC_CHR_H		'\x48'
#define ASC_CHR_I		'\x49'
#define ASC_CHR_J		'\x4a'
#define ASC_CHR_K		'\x4b'
#define ASC_CHR_L		'\x4c'
#define ASC_CHR_M		'\x4d'
#define ASC_CHR_N		'\x4e'
#define ASC_CHR_O		'\x4f'
#define ASC_CHR_P		'\x50'
#define ASC_CHR_Q		'\x51'
#define ASC_CHR_R		'\x52'
#define ASC_CHR_S		'\x53'
#define ASC_CHR_T		'\x54'
#define ASC_CHR_U		'\x55'
#define ASC_CHR_V		'\x56'
#define ASC_CHR_W		'\x57'
#define ASC_CHR_X		'\x58'
#define ASC_CHR_Y		'\x59'
#define ASC_CHR_Z		'\x5a'
#define ASC_CHR_LBRACK	'\x5b'
#define ASC_CHR_BSLASH	'\x5c'
#define ASC_CHR_RBRACK	'\x5d'
#define ASC_CHR_CARET	'\x5e'
#define ASC_CHR_ULINE	'\x5f'
#define ASC_CHR_GRAVE	'\x60'

/* Aliases */
#define ASC_CHR_UC_A	ASC_CHR_A
#define ASC_CHR_UC_B	ASC_CHR_B
#define ASC_CHR_UC_C	ASC_CHR_C
#define ASC_CHR_UC_D	ASC_CHR_D
#define ASC_CHR_UC_E	ASC_CHR_E
#define ASC_CHR_UC_F	ASC_CHR_F
#define ASC_CHR_UC_G	ASC_CHR_G
#define ASC_CHR_UC_H	ASC_CHR_H
#define ASC_CHR_UC_I	ASC_CHR_I
#define ASC_CHR_UC_J	ASC_CHR_J
#define ASC_CHR_UC_K	ASC_CHR_K
#define ASC_CHR_UC_L	ASC_CHR_L
#define ASC_CHR_UC_M	ASC_CHR_M
#define ASC_CHR_UC_N	ASC_CHR_N
#define ASC_CHR_UC_O	ASC_CHR_O
#define ASC_CHR_UC_P	ASC_CHR_P
#define ASC_CHR_UC_Q	ASC_CHR_Q
#define ASC_CHR_UC_R	ASC_CHR_R
#define ASC_CHR_UC_S	ASC_CHR_S
#define ASC_CHR_UC_T	ASC_CHR_T
#define ASC_CHR_UC_U	ASC_CHR_U
#define ASC_CHR_UC_V	ASC_CHR_V
#define ASC_CHR_UC_W	ASC_CHR_W
#define ASC_CHR_UC_X	ASC_CHR_X
#define ASC_CHR_UC_Y	ASC_CHR_Y
#define ASC_CHR_UC_Z	ASC_CHR_Z

/* Lower-case letters */
#define ASC_CHR_BQUOTE	'\x60'
#define ASC_CHR_a		'\x61'
#define ASC_CHR_b		'\x62'
#define ASC_CHR_c		'\x63'
#define ASC_CHR_d		'\x64'
#define ASC_CHR_e		'\x65'
#define ASC_CHR_f		'\x66'
#define ASC_CHR_g		'\x67'
#define ASC_CHR_h		'\x68'
#define ASC_CHR_i		'\x69'
#define ASC_CHR_j		'\x6a'
#define ASC_CHR_k		'\x6b'
#define ASC_CHR_l		'\x6c'
#define ASC_CHR_m		'\x6d'
#define ASC_CHR_n		'\x6e'
#define ASC_CHR_o		'\x6f'

#define ASC_CHR_p		'\x70'
#define ASC_CHR_q		'\x71'
#define ASC_CHR_r		'\x72'
#define ASC_CHR_s		'\x73'
#define ASC_CHR_t		'\x74'
#define ASC_CHR_u		'\x75'
#define ASC_CHR_v		'\x76'
#define ASC_CHR_w		'\x77'
#define ASC_CHR_x		'\x78'
#define ASC_CHR_y		'\x79'
#define ASC_CHR_z		'\x7a'
#define ASC_CHR_LBRACE	'\x7b'
#define ASC_CHR_VBAR	'\x7c'
#define ASC_CHR_RBRACE	'\x7d'
#define ASC_CHR_TILDE	'\x7e'
#define ASC_CHR_DEL		'\x7f'

/* Aliases */
#define ASC_CHR_LC_A	ASC_CHR_a
#define ASC_CHR_LC_B	ASC_CHR_b
#define ASC_CHR_LC_C	ASC_CHR_c
#define ASC_CHR_LC_D	ASC_CHR_d
#define ASC_CHR_LC_E	ASC_CHR_e
#define ASC_CHR_LC_F	ASC_CHR_f
#define ASC_CHR_LC_G	ASC_CHR_g
#define ASC_CHR_LC_H	ASC_CHR_h
#define ASC_CHR_LC_I	ASC_CHR_i
#define ASC_CHR_LC_J	ASC_CHR_j
#define ASC_CHR_LC_K	ASC_CHR_k
#define ASC_CHR_LC_L	ASC_CHR_l
#define ASC_CHR_LC_M	ASC_CHR_m
#define ASC_CHR_LC_N	ASC_CHR_n
#define ASC_CHR_LC_O	ASC_CHR_o
#define ASC_CHR_LC_P	ASC_CHR_p
#define ASC_CHR_LC_Q	ASC_CHR_q
#define ASC_CHR_LC_R	ASC_CHR_r
#define ASC_CHR_LC_S	ASC_CHR_s
#define ASC_CHR_LC_T	ASC_CHR_t
#define ASC_CHR_LC_U	ASC_CHR_u
#define ASC_CHR_LC_V	ASC_CHR_v
#define ASC_CHR_LC_W	ASC_CHR_w
#define ASC_CHR_LC_X	ASC_CHR_x
#define ASC_CHR_LC_Y	ASC_CHR_y
#define ASC_CHR_LC_Z	ASC_CHR_z

/* ISO-8859-1 extension */
#define ISO_CHR_NBSPACE '\xa0'
#define ISO_CHR_INVBANG '\xa1'
#define ISO_CHR_CENT	'\xa2'
#define ISO_CHR_POUNDS	'\xa3'
#define ISO_CHR_GENCUR	'\xa4'
#define ISO_CHR_YEN		'\xa5'
#define ISO_CHR_BRKVBAR '\xa6'
#define ISO_CHR_SECTION '\xa7'
#define ISO_CHR_UMLAUT	'\xa8'
#define ISO_CHR_COPYRT	'\xa9'
#define ISO_CHR_FEMORD	'\xaa'
#define ISO_CHR_LTANGQT '\xab'
#define ISO_CHR_NOT		'\xac'
#define ISO_CHR_SFTHYPH '\xad'
#define ISO_CHR_TRDMK	'\xae'
#define ISO_CHR_MACRON	'\xaf'

#define ISO_CHR_DEGREE	'\xb0'
#define ISO_CHR_PLUSMIN '\xb1'
#define ISO_CHR_SQUARED '\xb2'
#define ISO_CHR_CUBED	'\xb3'
#define ISO_CHR_ACUTE	'\xb4'
#define ISO_CHR_MICRO	'\xb5'
#define ISO_CHR_PGRAPH	'\xb6'
#define ISO_CHR_MIDDOT	'\xb7'
#define ISO_CHR_CEDILLA '\xb8'
#define ISO_CHR_FIRST	'\xb9'
#define ISO_CHR_MASCORD '\xba'
#define ISO_CHR_RTANGQT '\xbb'
#define ISO_CHR_QUARTER '\xbc'
#define ISO_CHR_HALF	'\xbd'
#define ISO_CHR_THRFRTH '\xbe'
#define ISO_CHR_INVQUES '\xbf'

#define ISO_CHR_A_GRAVE		'\xc0'
#define ISO_CHR_A_ACUTE		'\xc1'
#define ISO_CHR_A_CIRCUM	'\xc2'
#define ISO_CHR_A_TILDE		'\xc3'
#define ISO_CHR_A_UMLAUT	'\xc4'
#define ISO_CHR_A_RING		'\xc5'
#define ISO_CHR_AE_LIG		'\xc6'
#define ISO_CHR_C_CEDILLA	'\xc7'
#define ISO_CHR_E_GRAVE		'\xc8'
#define ISO_CHR_E_ACUTE		'\xc9'
#define ISO_CHR_E_CIRCUM	'\xca'
#define ISO_CHR_E_UMLAUT	'\xcb'
#define ISO_CHR_I_GRAVE		'\xcc'
#define ISO_CHR_I_ACUTE		'\xcd'
#define ISO_CHR_I_CIRCUM	'\xce'
#define ISO_CHR_I_UMLAUT	'\xcf'

#define ISO_CHR_ETH			'\xd0'
#define ISO_CHR_N_TILDE		'\xd1'
#define ISO_CHR_O_GRAVE		'\xd2'
#define ISO_CHR_O_ACUTE		'\xd3'
#define ISO_CHR_O_CIRCUM	'\xd4'
#define ISO_CHR_O_TILDE		'\xd5'
#define ISO_CHR_O_UMLAUT	'\xd6'
#define ISO_CHR_MULTIPLY	'\xd7'
#define ISO_CHR_O_SLASH		'\xd8'
#define ISO_CHR_U_GRAVE		'\xd9'
#define ISO_CHR_U_ACUTE		'\xda'
#define ISO_CHR_U_CIRCUM	'\xdb'
#define ISO_CHR_U_UMLAUT	'\xdc'
#define ISO_CHR_Y_TILDE		'\xdd'
#define ISO_CHR_THORN		'\xde'
#define ISO_CHR_SS_LIG		'\xdf'

#define ISO_CHR_a_GRAVE		'\xe0'
#define ISO_CHR_a_ACUTE		'\xe1'
#define ISO_CHR_a_CIRCUM	'\xe2'
#define ISO_CHR_a_TILDE		'\xe3'
#define ISO_CHR_a_UMLAUT	'\xe4'
#define ISO_CHR_a_RING		'\xe5'
#define ISO_CHR_ae_LIG		'\xe6'
#define ISO_CHR_c_CEDILLA	'\xe7'
#define ISO_CHR_e_GRAVE		'\xe8'
#define ISO_CHR_e_ACUTE		'\xe9'
#define ISO_CHR_e_CIRCUM	'\xea'
#define ISO_CHR_e_UMLAUT	'\xeb'
#define ISO_CHR_i_GRAVE		'\xec'
#define ISO_CHR_i_ACUTE		'\xed'
#define ISO_CHR_i_CIRCUM	'\xee'
#define ISO_CHR_i_UMLAUT	'\xef'

#define ISO_CHR_eth		'\xf0'
#define ISO_CHR_n_TILDE		'\xf1'
#define ISO_CHR_o_GRAVE		'\xf2'
#define ISO_CHR_o_ACUTE		'\xf3'
#define ISO_CHR_o_CIRCUM	'\xf4'
#define ISO_CHR_o_TILDE		'\xf5'
#define ISO_CHR_o_UMLAUT	'\xf6'
#define ISO_CHR_DIVIDE		'\xf7'
#define ISO_CHR_o_SLASH		'\xf8'
#define ISO_CHR_u_GRAVE		'\xf9'
#define ISO_CHR_u_ACUTE		'\xfa'
#define ISO_CHR_u_CIRCUM	'\xfb'
#define ISO_CHR_u_UMLAUT	'\xfc'
#define ISO_CHR_y_TILDE		'\xfd'
#define ISO_CHR_thorn		'\xfe'
#define ISO_CHR_y_UMLAUT	'\xff'

#endif
