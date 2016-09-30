;-----------------------------------------------------------------------------
; CERTLIB Color definitions.
;
;-----------------------------------------------------------------------------
; To make a color definition OR the values together.
;
; 	EX:	mov	ax,B_BLACK OR F_WHITE OR HIGHINTENSITY OR BLINK
;-----------------------------------------------------------------------------

; Foreground colors

F_BLACK	EQU	00h		; Black
F_BLUE		EQU	01h		; Blue
F_GREEN	EQU	02h		; Green
F_CYAN		EQU	03h		; Cyan
F_RED		EQU	04h		; Red
F_VIOLET	EQU	05h		; Violet
F_BROWN	EQU	06h		; Brown
F_WHITE	EQU	07h		; White

; Background colors

B_BLACK	EQU	00h		; Black
B_BLUE		EQU	10h		; Blue
B_GREEN	EQU	20h		; Green
B_CYAN		EQU	30h		; Cyan
B_RED		EQU	40h		; Red
B_VIOLET	EQU	50h		; Violet
B_BROWN	EQU	60h		; Brown
B_WHITE	EQU	70h		; White

; Foreground intensity

HIGHINTENSITY	EQU	08h		; High intensity bit.

; Blink

BLINK		EQU	80h		; Blink bit.

; Underline

UNDERLINE	EQU	80h		; Underline bit.

; Some standard Certus Colors.

HighlightAttrib	EQU	0F000h	;Highlight attrib - Current Window
MonoAttrib		EQU	07h
MainAttrib		EQU	17h	;Color attribute of background window
MainAttrib2		EQU	70h	;Color attribute of background window
WinAttrib      	EQU    	57h    	;Color attribute of forground windows
Highlight		EQU	057h
NOHIGHLIGHT		EQU	017h

Color_Pink		EQU	57h
Color_Blue		EQU	17h
Color_Black		EQU	07h

