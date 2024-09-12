.ORIG x3000
;Setting 0x4000 to 1
; AND R1, R1, #0
; ADD R1, R1, #1

; LEA R0, LOC
; LDW R0, R0, #0
; STW R1, R0, #0

;Doing the summation - R0 loop var
; R0 - Loop variable
; R1 - Address of data
; R2 - Addition Store
; R3 - Content var
; R4 - Sum var

LEA R1, DATA
LDW R1, R1, #0

LEA R0, AMM
LDW R0, R0, #0


AND R2, R2, #0

LOOP  LDB R3, R1, #0
ADD R2, R2, R3
ADD R1, R1, #1

ADD R0, R0, #-1
BRP LOOP

LEA R0, STORE
LDW R0, R0, #0
STW R2, R0, #0

LEA R0, PROTEXC
LDW R0, R0, #0
STW R2, R0, #0

HALT

AMM .FILL x0014
STORE .FILL xC014
DATA .FILL xC000
LOC .FILL x4000
PROTEXC .FILL x0000
.END