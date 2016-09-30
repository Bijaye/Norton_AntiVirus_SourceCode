;----------------------------------------------------------------------------
; SetAnsiFlag
; GetAnsiFlag
;
;       Set or Get the current setting for file function in Certlib4.
;       Need to set Ansi Flag to TRUE to access functions using
;       ANSI filenames, set flag to FALSE to access functions using
;       OEM filenames.
;----------------------------------------------------------------------------
include CTS4.INC

ifdef WIN16
.data
else
.code
endif

PUBLIC wAnsi
wAnsi   word    0

ifdef WIN16
.code
endif

;----------------------------------------------------------------------------
SetAnsiFlag PROC FAR,
        wFlag:WORD

        mov     ax,wFlag
        mov     wAnsi,ax

        ret

SetAnsiFlag ENDP
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
GetAnsiFlag PROC FAR USES DS

ifdef WIN16
        mov     ax, @data
else
        mov     ax, cs
endif
        mov     ds, ax
        mov     ax,wAnsi
        ret

GetAnsiFlag ENDP
;----------------------------------------------------------------------------
END

