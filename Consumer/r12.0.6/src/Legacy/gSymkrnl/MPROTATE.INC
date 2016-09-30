ROL32   MACRO   a1:req,a2:req,a3
        LOCAL   shifted
        LOCAL   shiftAgain
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
                                IF IS_ONE(a3)
                                        ;;
                                        ;; 3rd argument is 1
                                        ;;
                                        pushf           ; save flags
                                        shl     a2,1
                                        rcl     a1,1
                                        popf            ; restore flags
                                        adc     a2,0    ; OR in carry if necessary
                                ELSEIF IS_CL(a3)
                                        ;;
                                        ;; 3rd argument is CL register
                                        ;;
                                        push    CX
                                        xor     CH,CH
                                        jcxz    shifted
                                        shiftAgain:
                                        shl     a2,1
                                        rcl     a1,1
                                        adc     a2,0
                                        loop    shiftAgain
                                        shifted:
                                        pop     CX
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
                IF IS_ONE(a2)
                        ;;
                        ;; 2nd argument is 1
                        ;;
                        shl     WORD PTR a1[ 2 ],1
                        rcl     WORD PTR a1[ 0 ],1
                        adc     WORD PTR a1[ 2 ],0
                ELSEIF IS_CL(a2)
                        ;;
                        ;; 2rd argument is CL register
                        ;;
                        push    CX
                        xor     CH,CH
                        jcxz    shifted
                        shiftAgain:
                        shl     WORD PTR a1[ 2 ],1
                        rcl     WORD PTR a1[ 0 ],1
                        adc     WORD PTR a1[ 2 ],0
                        loop    shiftAgain
                        shifted:
                        pop     CX
                ELSE
                        .ERR <a2 is not a valid source>
                ENDIF
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
        ENDM

ROR32   MACRO   a1:req,a2:req,a3
        LOCAL   shifted
        LOCAL   shiftAgain
        LOCAL   no_carry
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
                                IF IS_ONE(a3)
                                        ;;
                                        ;; 3rd argument is 1
                                        ;;
                                        sar     a1,1
                                        rcr     a2,1
                                        jnc     no_carry
                                        or      a1,08000H       ; set high bit 
                                        no_carry:
                                ELSEIF IS_CL(a3)
                                        ;;
                                        ;; 3rd argument is CL register
                                        ;;
                                        push    CX
                                        xor     CH,CH
                                        jcxz    shifted
                                        shiftAgain:
                                        sar     a1,1
                                        rcr     a2,1
                                        jnc     no_carry
                                        or      a1,08000H
                                        no_carry:
                                        loop    shiftAgain
                                        shifted:
                                        pop     CX
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
                IF IS_ONE(a2)
                        ;;
                        ;; 2nd argument is 1
                        ;;
                        sar     WORD PTR a1[ 0 ],1
                        rcr     WORD PTR a1[ 2 ],1
                        jnc     no_carry
                        or      WORD PTR a1[ 0 ],08000H
                        no_carry:
                ELSEIF IS_CL(a2)
                        ;;
                        ;; 2rd argument is CL register
                        ;;
                        push    CX
                        xor     CH,CH
                        jcxz    shifted
                        shiftAgain:
                        sar     WORD PTR a1[ 0 ],1
                        rcr     WORD PTR a1[ 2 ],1
                        jnc     no_carry
                        or      WORD PTR a1[ 0 ],08000H
                        no_carry:
                        loop    shiftAgain
                        shifted:
                        pop     CX
                ELSE
                        .ERR <a2 is not a valid source>
                ENDIF
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
        ENDM

