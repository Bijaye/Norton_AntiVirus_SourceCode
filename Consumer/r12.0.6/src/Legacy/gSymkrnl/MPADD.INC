ADC32   MACRO   a1:req,a2:req,a3,a4
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
                                                        adc     a2,a4
                                                        adc     a1,a3
                                                ELSE
                                                        .ERR <a4 must be a 16-bit register>
                                                ENDIF
                                        ENDIF
                                ELSEIF IS_CONSTANT(a3)
                                        ;;
                                        ;; 3rd argument is a constant
                                        ;;
                                        adc     a2,LOWWORD a3
                                        adc     a1,HIGHWORD a3
                                ELSEIF IS_MEM32(a3)
                                        ;;
                                        ;; 3rd argument is a dword ptr
                                        ;;
                                        adc     a2,WORD PTR a3[ 0 ]
                                        adc     a1,WORD PTR a3[ 2 ]
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
                                        adc     WORD PTR a1[ 0 ],a3
                                        adc     WORD PTR a1[ 2 ],a2
                                ELSE
                                        .ERR <a3 must be a 16-bit register>
                                ENDIF
                        ENDIF
                ELSEIF IS_CONSTANT(a2)
                        adc     WORD PTR a1[ 0 ],LOWWORD a2
                        adc     WORD PTR a1[ 2 ],HIGHWORD a2
                ELSE
                        .ERR <a2 is not a valid source>
                ENDIF
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
        ENDM
        
ADD32   MACRO   a1:req,a2:req,a3,a4
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
                                                        add     a2,a4
                                                        adc     a1,a3
                                                ELSE
                                                        .ERR <a4 must be a 16-bit register>
                                                ENDIF
                                        ENDIF
                                ELSEIF IS_CONSTANT(a3)
                                        ;;
                                        ;; 3rd argument is a constant
                                        ;;
                                        add     a2,LOWWORD a3
                                        adc     a1,HIGHWORD a3
                                ELSEIF IS_MEM32(a3)
                                        ;;
                                        ;; 3rd argument is a dword ptr
                                        ;;
                                        add     a2,WORD PTR a3[ 0 ]
                                        adc     a1,WORD PTR a3[ 2 ]
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
                                        add     WORD PTR a1[ 0 ],a3
                                        adc     WORD PTR a1[ 2 ],a2
                                ELSE
                                        .ERR <a3 must be a 16-bit register>
                                ENDIF
                        ENDIF
                ELSEIF IS_CONSTANT(a2)
                        add     WORD PTR a1[ 0 ],LOWWORD a2
                        adc     WORD PTR a1[ 2 ],HIGHWORD a2
                ELSE
                        .ERR <a2 is not a valid source>
                ENDIF
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
        ENDM

INC32   MACRO   a1:req,a2
        IF IS_REG16(a1)
                ;;
                ;; first argument is a 16-bit register
                ;;
                IFB <a2>
                        .ERR <missing 2nd register>
                ELSE
                        IF IS_REG16(a2)
                                add     a2,1
                                adc     a1,0
                        ELSE
                                .ERR <a2 is not a valid argument>
                        ENDIF
                ENDIF
        ELSEIF IS_MEM32(a1)
                ;;
                ;; first argument is a dword ptr
                ;;
                add     WORD PTR a1[ 0 ],1
                adc     WORD PTR a1[ 1 ],0
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
        ENDM
