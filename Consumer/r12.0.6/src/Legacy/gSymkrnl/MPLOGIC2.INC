FLAGS_REGISTER	RECORD	OVERFLOW_FLAG	: 1,
			DIRECTION_FLAG	: 1,
			INTERRUPT_FLAG	: 1,
			TRACE_FLAG	: 1,
			SIGN_FLAG	: 1,
			ZERO_FLAG	: 1,
			UNUSED_FLAG	: 1,
			AUXILIARY_FLAG	: 1,
			UNUSED_FLAG2	: 1,
			PARITY_FLAG	: 1,
			UNUSED_FLAG3	: 1,
			CARRY_FLAG	: 1

FIXFLAGS	MACRO			
                push	AX
                pushf
                pop	AX
                and	AX,(MASK DIRECTION_FLAG) OR (MASK INTERRUPT_FLAG) OR (MASK TRACE_FLAG) OR (MASK SIGN_FLAG) OR (MASK ZERO_FLAG) OR (MASK AUXILIARY_FLAG) OR (MASK PARITY_FLAG)
                push	AX
                popf
                pop	AX
                ENDM

AND32	MACRO	a1:req,a2:req,a3,a4
        IF IS_REG16(a1)
                ;;
                ;; first argument is a 16-bit register
                ;;
                IF IS_REG16(a2)
                        ;;
                        ;; second argument is a 16-bit register
                        ;;
                        IFB <a3>
                                .ERR <missing source argument>
                        ELSE
                                IF IS_REG16(a3)
                                        ;;
                                        ;; 3rd argument is a 16-bit register
                                        ;;
                                        IFB <a4>
                                                .ERR <missing source argument>
                                        ELSE
                                                IF IS_REG16(a4)
                                                        ;;
                                                        ;; 4th argument is a 16-bit register
                                                        ;;
                                                        and     a2,a4
                                                        and     a1,a3
                                                        cmp32	a1,a2,0
                                                        fixflags
                                                ELSE
                                                        .ERR <a4 must be a 16-bit register>
                                                ENDIF
                                        ENDIF
                                ELSEIF IS_CONSTANT(a3)
                                        ;;
                                        ;; 3rd argument is a constant
                                        ;;
                                        and     a2,(a3 AND 00000FFFFH)
                                        and     a1,((a3 AND 0FFFF0000H) / 10000H)
                                        cmp32	a1,a2,0
                                        fixflags
                                ELSEIF IS_MEM32(a3)
                                        ;;
                                        ;; 3rd argument is a dword ptr
                                        ;;
                                        and     a2,WORD PTR a3[ 0 ]
                                        and     a1,WORD PTR a3[ 2 ]
                                        cmp32	a1,a2,0
                                        fixflags
                                ELSE
                                        .ERR <a3 is not a valid source>
                                ENDIF
                        ENDIF
                ELSE
                        .ERR <a2 must be a 16-bit register>
                ENDIF
        ELSEIF IS_MEM32(a1)
                ;;
                ;; first argument is a dword ptr
                ;;
                IF IS_REG16(a2)
                        ;;
                        ;; 2nd argument is a register
                        ;;
                        IFB <a3>
                                .ERR <2nd source register is missing>
                        ELSE
                                IF IS_REG16(a3)
                                        and     WORD PTR a1[ 0 ],a3
                                        and     WORD PTR a1[ 2 ],a2
                                        cmp32	a1,0
                                        fixflags
                                ELSE
                                        .ERR <a3 must be a 16-bit register>
                                ENDIF
                        ENDIF
                ELSEIF IS_CONSTANT(a2)
                        and     WORD PTR a1[ 0 ],(a2 AND 00000FFFFH)
                        and     WORD PTR a1[ 2 ],((a2 AND 0FFFF0000H) / 10000H)
                        cmp32	a1,0
                        fixflags
                ELSE
                        .ERR <a2 is not a valid source>
                ENDIF
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
	ENDM

OR32	MACRO	a1:req,a2:req,a3,a4
        IF IS_REG16(a1)
                ;;
                ;; first argument is a 16-bit register
                ;;
                IF IS_REG16(a2)
                        ;;
                        ;; second argument is a 16-bit register
                        ;;
                        IFB <a3>
                                .ERR <missing source argument>
                        ELSE
                                IF IS_REG16(a3)
                                        ;;
                                        ;; 3rd argument is a 16-bit register
                                        ;;
                                        IFB <a4>
                                                .ERR <missing source argument>
                                        ELSE
                                                IF IS_REG16(a4)
                                                        ;;
                                                        ;; 4th argument is a 16-bit register
                                                        ;;
                                                        or      a2,a4
                                                        or      a1,a3
                                                        cmp32	a1,a2,0
                                                        fixflags
                                                ELSE
                                                        .ERR <a4 must be a 16-bit register>
                                                ENDIF
                                        ENDIF
                                ELSEIF IS_CONSTANT(a3)
                                        ;;
                                        ;; 3rd argument is a constant
                                        ;;
                                        or      a2,(a3 AND 00000FFFFH)
                                        or      a1,((a3 AND 0FFFF0000H) / 10000H)
                                        cmp32	a1,a2,0
                                        fixflags
                                ELSEIF IS_MEM32(a3)
                                        ;;
                                        ;; 3rd argument is a dword ptr
                                        ;;
                                        or      a2,WORD PTR a3[ 0 ]
                                        or      a1,WORD PTR a3[ 2 ]
                                        cmp32	a1,a2,0
                                        fixflags
                                ELSE
                                        .ERR <a3 is not a valid source>
                                ENDIF
                        ENDIF
                ELSE
                        .ERR <a2 must be a 16-bit register>
                ENDIF
        ELSEIF IS_MEM32(a1)
                ;;
                ;; first argument is a dword ptr
                ;;
                IF IS_REG16(a2)
                        ;;
                        ;; 2nd argument is a register
                        ;;
                        IFB <a3>
                                .ERR <2nd source register is missing>
                        ELSE
                                IF IS_REG16(a3)
                                        or      WORD PTR a1[ 0 ],a3
                                        or      WORD PTR a1[ 2 ],a2
                                        cmp32	a1,0
                                        fixflags
                                ELSE
                                        .ERR <a3 must be a 16-bit register>
                                ENDIF
                        ENDIF
                ELSEIF IS_CONSTANT(a2)
                        or      WORD PTR a1[ 0 ],(a2 AND 00000FFFFH)
                        or      WORD PTR a1[ 2 ],((a2 AND 0FFFF0000H) / 10000H)
                        cmp32	a1,0
                        fixflags
                ELSE
                        .ERR <a2 is not a valid source>
                ENDIF
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
	ENDM

TEST32  MACRO   a1:req,a2:req,a3,a4
        LOCAL   tested
        IF IS_REG16(a1)
                ;;
                ;; first argument is a 16-bit register
                ;;
                IF IS_REG16(a2)
                        ;;
                        ;; second argument is a 16-bit register
                        ;;
                        IFB <a3>
                                .ERR <missing source argument>
                        ELSE
                                IF IS_REG16(a3)
                                        ;;
                                        ;; 3rd argument is a 16-bit register
                                        ;;
                                        IFB <a4>
                                                .ERR <missing source argument>
                                        ELSE
                                                IF IS_REG16(a4)
                                                        ;;
                                                        ;; 4th argument is a 16-bit register
                                                        ;;
                                                        test    a1,a3
                                                        jne     tested
                                                        test    a2,a4
                                                        tested:
                                                        fixflags
                                                ELSE
                                                        .ERR <a4 must be a 16-bit register>
                                                ENDIF
                                        ENDIF
                                ELSEIF IS_CONSTANT(a3)
                                        ;;
                                        ;; 3rd argument is a constant
                                        ;;
                                        test    a1,((a3 AND 0FFFF0000H) / 10000H)
                                        jne     tested
                                        test    a2,(a3 AND 00000FFFFH)
                                        tested:
                                        fixflags
                                ELSEIF IS_MEM32(a3)
                                        ;;
                                        ;; 3rd argument is a dword ptr
                                        ;;
                                        test    a1,WORD PTR a3[ 2 ]
                                        jne     tested
                                        test    a2,WORD PTR a3[ 0 ]
                                        tested:
                                        fixflags
                                ELSE
                                        .ERR <a3 is not a valid source>
                                ENDIF
                        ENDIF
                ELSE
                        .ERR <a2 must be a 16-bit register>
                ENDIF
        ELSEIF IS_MEM32(a1)
                ;;
                ;; first argument is a dword ptr
                ;;
                IF IS_REG16(a2)
                        ;;
                        ;; 2nd argument is a register
                        ;;
                        IFB <a3>
                                .ERR <2nd source register is missing>
                        ELSE
                                IF IS_REG16(a3)
                                        test    WORD PTR a1[ 2 ],a2
                                        jne     tested
                                        test    WORD PTR a1[ 0 ],a3
                                        tested:
                                        fixflags
                                ELSE
                                        .ERR <a3 must be a 16-bit register>
                                ENDIF
                        ENDIF
                ELSEIF IS_CONSTANT(a2)
                        test    WORD PTR a1[ 2 ],((a2 AND 0FFFF0000H) / 10000H)
                        jne     tested
                        test    WORD PTR a1[ 0 ],(a2 AND 00000FFFFH)
                        tested:
                        fixflags
                ELSE
                        .ERR <a2 is not a valid source>
                ENDIF
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
        ENDM

XOR32	MACRO	a1:req,a2:req,a3,a4
        IF IS_REG16(a1)
                ;;
                ;; first argument is a 16-bit register
                ;;
                IF IS_REG16(a2)
                        ;;
                        ;; second argument is a 16-bit register
                        ;;
                        IFB <a3>
                                .ERR <missing source argument>
                        ELSE
                                IF IS_REG16(a3)
                                        ;;
                                        ;; 3rd argument is a 16-bit register
                                        ;;
                                        IFB <a4>
                                                .ERR <missing source argument>
                                        ELSE
                                                IF IS_REG16(a4)
                                                        ;;
                                                        ;; 4th argument is a 16-bit register
                                                        ;;
                                                        xor     a2,a4
                                                        xor     a1,a3
                                                        cmp32	a1,a2,0
                                                        fixflags
                                                ELSE
                                                        .ERR <a4 must be a 16-bit register>
                                                ENDIF
                                        ENDIF
                                ELSEIF IS_CONSTANT(a3)
                                        ;;
                                        ;; 3rd argument is a constant
                                        ;;
                                        xor     a2,(a3 AND 00000FFFFH)
                                        xor     a1,((a3 AND 0FFFF0000H) / 10000H)
                                        cmp32	a1,a2,0
                                        fixflags
                                ELSEIF IS_MEM32(a3)
                                        ;;
                                        ;; 3rd argument is a dword ptr
                                        ;;
                                        xor     a2,WORD PTR a3[ 0 ]
                                        xor     a1,WORD PTR a3[ 2 ]
                                        cmp32	a1,a2,0
                                        fixflags
                                ELSE
                                        .ERR <a3 is not a valid source>
                                ENDIF
                        ENDIF
                ELSE
                        .ERR <a2 must be a 16-bit register>
                ENDIF
        ELSEIF IS_MEM32(a1)
                ;;
                ;; first argument is a dword ptr
                ;;
                IF IS_REG16(a2)
                        ;;
                        ;; 2nd argument is a register
                        ;;
                        IFB <a3>
                                .ERR <2nd source register is missing>
                        ELSE
                                IF IS_REG16(a3)
                                        xor     WORD PTR a1[ 0 ],a3
                                        xor     WORD PTR a1[ 2 ],a2
                                        cmp32	a1,0
                                        fixflags
                                ELSE
                                        .ERR <a3 must be a 16-bit register>
                                ENDIF
                        ENDIF
                ELSEIF IS_CONSTANT(a2)
                        xor     WORD PTR a1[ 0 ],(a2 AND 00000FFFFH)
                        xor     WORD PTR a1[ 2 ],((a2 AND 0FFFF0000H) / 10000H)
                        cmp32	a1,0
                        fixflags
                ELSE
                        .ERR <a2 is not a valid source>
                ENDIF
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
	ENDM


