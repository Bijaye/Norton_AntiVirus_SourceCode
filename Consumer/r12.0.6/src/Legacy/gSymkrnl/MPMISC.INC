MOV32	MACRO	a1,a2,a3,a4
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
                                                        mov     a2,a4
                                                        mov     a1,a3
                                                ELSE
                                                        .ERR <a4 must be a 16-bit register>
                                                ENDIF
                                        ENDIF
                                ELSEIF IS_CONSTANT(a3)
                                        ;;
                                        ;; 3rd argument is a constant
                                        ;;
                                        mov     a2,LOWWORD a3
                                        mov     a1,HIGHWORD a3
                                ELSEIF IS_MEM32(a3)
                                        ;;
                                        ;; 3rd argument is a dword ptr
                                        ;;
                                        mov     a2,WORD PTR a3[ 0 ]
                                        mov     a1,WORD PTR a3[ 2 ]
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
                                        mov     WORD PTR a1[ 0 ],a3
                                        mov     WORD PTR a1[ 2 ],a2
                                ELSE
                                        .ERR <a3 must be a 16-bit register>
                                ENDIF
                        ENDIF
                ELSEIF IS_CONSTANT(a2)
                        mov     WORD PTR a1[ 0 ],LOWWORD a2
                        mov     WORD PTR a1[ 2 ],HIGHWORD a2
                ELSE
                        .ERR <a2 is not a valid source>
                ENDIF
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
        ENDM

NEG32	MACRO	a1,a2
        IF IS_REG16(a1)
                ;;
                ;; first argument is a 16-bit register
                ;;
                IF IS_REG16(a2)
                        ;;
                        ;; second argument is a 16-bit register
                        ;;
                        neg	a1
                        neg	a2
                        sbb	a1,0
                ELSE
                        .ERR <a2 must be a 16-bit register>
                ENDIF
        ELSEIF IS_MEM32(a1)
                ;;
                ;; first argument is a dword ptr
                ;;
                neg	WORD PTR a1[ 2 ]
                neg	WORD PTR a1[ 0 ]
                sbb	WORD PTR a1[ 2 ],0
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
        ENDM

NOT32	MACRO	a1,a2
        IF IS_REG16(a1)
                ;;
                ;; first argument is a 16-bit register
                ;;
                IF IS_REG16(a2)
                        ;;
                        ;; second argument is a 16-bit register
                        ;;
                        not	a1
                        not	a2
                ELSE
                        .ERR <a2 must be a 16-bit register>
                ENDIF
        ELSEIF IS_MEM32(a1)
                ;;
                ;; first argument is a dword ptr
                ;;
                not	WORD PTR a1[ 2 ]
                not	WORD PTR a1[ 0 ]
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
        ENDM

XCHG32	MACRO	a1,a2,a3,a4
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
                                                        xchg    a2,a4
                                                        xchg    a1,a3
                                                ELSE
                                                        .ERR <a4 must be a 16-bit register>
                                                ENDIF
                                        ENDIF
                                ELSEIF IS_CONSTANT(a3)
                                        ;;
                                        ;; 3rd argument is a constant
                                        ;;
                                        .ERR <Cannot XCHG with a constant value>
                                ELSEIF IS_MEM32(a3)
                                        ;;
                                        ;; 3rd argument is a dword ptr
                                        ;;
                                        xchg    a2,WORD PTR a3[ 0 ]
                                        xchg    a1,WORD PTR a3[ 2 ]
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
                                        xchg    WORD PTR a1[ 0 ],a3
                                        xchg    WORD PTR a1[ 2 ],a2
                                ELSE
                                        .ERR <a3 must be a 16-bit register>
                                ENDIF
                        ENDIF
                ELSEIF IS_CONSTANT(a2)
                        .ERR <Cannot XCHG with a constant value>
                ELSE
                        .ERR <a2 is not a valid source>
                ENDIF
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
        ENDM
