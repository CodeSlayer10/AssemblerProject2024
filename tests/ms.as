mcr m_mcr 
    inc r2
    mov r3, r1 
endmcr

.entry LIST
.extern W
.define sz=3
MAIN: mov r3, LIST[sz]
LOOP: jmp W
prn #-5
mov STR[5], STR[2]
sub r1, r4
cmp K, #sz
bne W
;m_mcr
L1: inc L3 
.entry LOOP
bne LOOP
END: hlt
.define len = 4
STR: .string "abcdef"
LIST: .data 6, -9, len
K: .data 22
.extern L3