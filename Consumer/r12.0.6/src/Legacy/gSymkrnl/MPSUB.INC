SBB32   MACRO   a1:req,a2:req,a3,a4
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
                                                        sbb     a2,a4
                                                        sbb     a1,a3
                                                ELSE
                                                        .ERR <a4 must be a 16-bit register>
                                                ENDIF
                                        ENDIF
                                ELSEIF IS_CONSTANT(a3)
                                        ;;
                                        ;; 3rd argument is a constant
                                        ;;
                                        sbb     a2,LOWWORD a3
                                        sbb     a1,HIGHWORD a3
                                ELSEIF IS_MEM32(a3)
                                        ;;
                                        ;; 3rd argument is a dword ptr
                                        ;;
                                        sbb     a2,WORD PTR a3[ 0 ]
                                        sbb     a1,WORD PTR a3[ 2 ]
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
                                        sbb     WORD PTR a1[ 0 ],a3
                                        sbb     WORD PTR a1[ 2 ],a2
                                ELSE
                                        .ERR <a3 must be a 16-bit register>
                                ENDIF
                        ENDIF
                ELSEIF IS_CONSTANT(a2)
                        sbb     WORD PTR a1[ 0 ],LOWWORD a2
                        sbb     WORD PTR a1[ 2 ],HIGHWORD a2
                ELSE
                        .ERR <a2 is not a valid source>
                ENDIF
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
        ENDM
        
SUB32   MACRO   a1:req,a2:req,a3,a4
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
                                                        sub     a2,a4
                                                        sbb     a1,a3
                                                ELSE
                                                        .ERR <a4 must be a 16-bit register>
                                                ENDIF
                                        ENDIF
                                ELSEIF IS_CONSTANT(a3)
                                        ;;
                                        ;; 3rd argument is a constant
                                        ;;
                                        sub     a2,LOWWORD a3
                                        sbb     a1,HIGHWORD a3
                                ELSEIF IS_MEM32(a3)
                                        ;;
                                        ;; 3rd argument is a dword ptr
                                        ;;
                                        sub     a2,WORD PTR a3[ 0 ]
                                        sbb     a1,WORD PTR a3[ 2 ]
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
                                        sub     WORD PTR a1[ 0 ],a3
                                        sbb     WORD PTR a1[ 2 ],a2
                                ELSE
                                        .ERR <a3 must be a 16-bit register>
                                ENDIF
                        ENDIF
                ELSEIF IS_CONSTANT(a2)
                        sub     WORD PTR a1[ 0 ],LOWWORD a2
                        sbb     WORD PTR a1[ 2 ],HIGHWORD a2
                ELSE
                        .ERR <a2 is not a valid source>
                ENDIF
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
        ENDM

DEC32   MACRO   a1:req,a2
        IF IS_REG16(a1)
                ;;
                ;; first argument is a 16-bit register
                ;;
                IFB <a2>
                        .ERR <missing 2nd register>
                ELSE
                        IF IS_REG16(a2)
                                sub     a2,1
                                sbb     a1,0
                        ELSE
                                .ERR <a2 is not a valid argument>
                        ENDIF
                ENDIF
        ELSEIF IS_MEM32(a1)
                ;;
                ;; first argument is a dword ptr
                ;;
                sub     WORD PTR a1[ 0 ],1
                sbb     WORD PTR a1[ 1 ],0
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
        ENDM

CMP32   MACRO   a1:req,a2:req,a3,a4
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
                                                        cmp     a1,a3
                                                        jne     tested
                                                        cmp     a2,a4
                                                        tested:
                                                ELSE
                                                        .ERR <a4 must be a 16-bit register>
                                                ENDIF
                                        ENDIF
                                ELSEIF IS_CONSTANT(a3)
                                        ;;
                                        ;; 3rd argument is a constant
                                        ;;
                                        cmp     a1,HIGHWORD a3
                                        jne     tested
                                        cmp     a2,LOWWORD a3
                                        tested:
                                ELSEIF IS_MEM32(a3)
                                        ;;
                                        ;; 3rd argument is a dword ptr
                                        ;;
                                        cmp     a1,WORD PTR a3[ 2 ]
                                        jne     tested
                                        cmp     a2,WORD PTR a3[ 0 ]
                                        tested:
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
                                        cmp     WORD PTR a1[ 2 ],a2
                                        jne     tested
                                        cmp     WORD PTR a1[ 0 ],a3
                                        tested:
                                ELSE
                                        .ERR <a3 must be a 16-bit register>
                                ENDIF
                        ENDIF
                ELSEIF IS_CONSTANT(a2)
                        cmp     WORD PTR a1[ 2 ],HIGHWORD a2
                        jne     tested
                        cmp     WORD PTR a1[ 0 ],LOWWORD a2
                        tested:
                ELSE
                        .ERR <a2 is not a valid source>
                ENDIF
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
        ENDM

