IS_REG16        MACRO   reg
                IF (OPATTR(reg)) AND 00010000y
                        IF (TYPE(reg) EQ WORD)
                                EXITM %1
                        ELSE
                                EXITM %0
                        ENDIF
                ELSE
                        EXITM %0
                ENDIF
                ENDM

IS_MEM32        MACRO   mem
                IF (OPATTR(reg)) AND 00010100y
                        EXITM %0
                ELSE
                        IF (TYPE(mem) EQ 4)
                                EXITM %1
                        ELSE
                                EXITM %0
                        ENDIF
                ENDIF
                ENDM

IS_CONSTANT     MACRO   constant
                IF (OPATTR(constant)) AND 00000100y
                        EXITM %1
                ELSE
                        EXITM %0
                ENDIF
                ENDM

INCLUDE MPADD.INC
INCLUDE	MPSUB.INC

IS_ONE  MACRO   arg
        IF IS_CONSTANT(arg)
                IF arg EQ 1
                        EXITM %1
                ELSE
                        EXITM %0
                ENDIF
        ELSE
                EXITM %0        ;; not a constant
        ENDIF
        ENDM

IS_CL   MACRO   arg
        IF (OPATTR(arg)) AND 00010000y
                IFIDNI <arg>, <CL>
                        EXITM %1
                ELSE
                        EXITM %0
                ENDIF
        ELSE
                EXITM %0        ;; not a register
        ENDIF
        ENDM

INCLUDE	MPARI_SH.INC
INCLUDE	MPROTATE.INC
INCLUDE MPMUL.INC
INCLUDE MPMISC.INC
INCLUDE	MPLOG_SH.INC
INCLUDE MPROTC.INC
INCLUDE	MPLOGIC.INC
INCLUDE MPDIV.INC
