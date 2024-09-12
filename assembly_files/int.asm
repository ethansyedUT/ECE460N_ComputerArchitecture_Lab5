.ORIG x1200
;Used regs 
;r0, r1
STW R0, R6, #-1
STW R1, R6, #-2
STW R2, R6, #-3
STW R3, R6, #-4
STW R4, R6, #-5

;R0 - Page Table Loc
;R1 - PTE
;R2 - PT Offset
;R3 - CLR mask
;R4 - Check end PT


LEA R0, PTLOC
LDW R0, R0, #0

AND R2, R2, #0

LEA R3, MASK
LDW R3, R3, #0

LEA R4, LOOP
LDW R4, R4, #0

START LDW R1, R0, #0
AND R1, R1, R3
STW R1, R0, #0

ADD R0, R0, #1
ADD R2, R2, #1
ADD R1, R2, R4
BRN START


;restore regs
LDW R0, R6, #-1
LDW R1, R6, #-2
LDW R2, R6, #-3
LDW R3, R6, #-4
LDW R4, R6, #-5

RTI
LOOP .FILL #-128
MASK .FILL xFFFE
PTLOC .FILL x1000
.END