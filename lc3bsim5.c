/*
    Name 1: Your Name
    UTEID 1: Your UTEID
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N - Lab 5                                           */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         pagetable    page table in LC-3b machine language   */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE 1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x)&0xFFFF)

/***************************************************************/
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS
{
    IRD1,
    IRD0,
    COND2,
    COND1,
    COND0,
    J5,
    J4,
    J3,
    J2,
    J1,
    J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    PCMUX1,
    PCMUX0,
    DRMUX1, // new l4
    DRMUX0,
    SR1MUX1, // new l4
    SR1MUX0,
    ADDR1MUX,
    ADDR2MUX1,
    ADDR2MUX0,
    MARMUX1,
    MARMUX0,
    ALUK1,
    ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
    /* MODIFY: you have to add all your new control signals */
    // NEW L4 CNTRL Signals
    LD_PSR,
    LD_INTV,
    LD_EXCV,
    LD_VCB,
    LD_SSP,
    LD_USP,
    LD_VEC,
    INT_EN,
    EXC_EN,
    UA_EN,
    UOP_EN,
    GATE_SP,
    GATE_PSR,
    GATE_VEC,
    REGMUX,
    SPMUX1,
    SPMUX0,
    PSRMUX1,
    PSRMUX0,
    // NEW L5 CNTRL Signals
    LD_VA,
    LD_RETR,
    PF_EN,
    PA_EN,
    GATE_PTE,
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x) { return ((x[IRD1] << 1) + x[IRD0]); }
int GetCOND(int *x) { return ((x[COND2] << 2) + (x[COND1] << 1) + x[COND0]); } // modified l4
int GetJ(int *x) { return ((x[J5] << 5) + (x[J4] << 4) +
                           (x[J3] << 3) + (x[J2] << 2) +
                           (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x) { return (x[LD_MAR]); }
int GetLD_MDR(int *x) { return (x[LD_MDR]); }
int GetLD_IR(int *x) { return (x[LD_IR]); }
int GetLD_BEN(int *x) { return (x[LD_BEN]); }
int GetLD_REG(int *x) { return (x[LD_REG]); }
int GetLD_CC(int *x) { return (x[LD_CC]); }
int GetLD_PC(int *x) { return (x[LD_PC]); }
int GetGATE_PC(int *x) { return (x[GATE_PC]); }
int GetGATE_MDR(int *x) { return (x[GATE_MDR]); }
int GetGATE_ALU(int *x) { return (x[GATE_ALU]); }
int GetGATE_MARMUX(int *x) { return (x[GATE_MARMUX]); }
int GetGATE_SHF(int *x) { return (x[GATE_SHF]); }
int GetPCMUX(int *x) { return ((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x) { return ((x[DRMUX1] << 1) + x[DRMUX0]); }    // modified l4
int GetSR1MUX(int *x) { return ((x[SR1MUX1]) << 1) + x[SR1MUX0]; } // modified l4
int GetADDR1MUX(int *x) { return (x[ADDR1MUX]); }
int GetADDR2MUX(int *x) { return ((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x) { return ((x[MARMUX1] << 1) + x[MARMUX0]); }
int GetALUK(int *x) { return ((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x) { return (x[MIO_EN]); }
int GetR_W(int *x) { return (x[R_W]); }
int GetDATA_SIZE(int *x) { return (x[DATA_SIZE]); }
int GetLSHF1(int *x) { return (x[LSHF1]); }
/* MODIFY: you can add more Get functions for your new control signals */
int GetLD_PSR(int *x) { return (x[LD_PSR]); }
int GetLD_INTV(int *x) { return (x[LD_INTV]); }
int GetLD_EXCV(int *x) { return (x[LD_EXCV]); }
int GetLD_VCB(int *x) { return (x[LD_VCB]); }
int GetLD_SSP(int *x) { return (x[LD_SSP]); }
int GetLD_USP(int *x) { return (x[LD_USP]); }
int GetLD_VEC(int *x) { return (x[LD_VEC]); }
int GetINT_EN(int *x) { return (x[INT_EN]); }
int GetEXC_EN(int *x) { return (x[EXC_EN]); }
int GetUA_EN(int *x) { return (x[UA_EN]); }
int GetUOP_EN(int *x) { return (x[UOP_EN]); }
int GetGATE_SP(int *x) { return (x[GATE_SP]); }
int GetGATE_PSR(int *x) { return (x[GATE_PSR]); }
int GetGATE_VEC(int *x) { return (x[GATE_VEC]); }
int GetREGMUX(int *x) { return (x[REGMUX]); }
int GetSPMUX(int *x) { return ((x[SPMUX1] << 1) + x[SPMUX0]); }
int GetPSRMUX(int *x) { return ((x[PSRMUX1] << 1) + x[PSRMUX0]); }
// Lab 5 cntrl signals
int GetLD_VA(int *x) { return x[LD_VA]; }
int GetLD_RETR(int *x) { return x[LD_RETR]; }
int GetPF_EN(int *x) { return x[PF_EN]; }
int GetPA_EN(int *x) { return x[PA_EN]; }
int GetGate_PTE(int *x) { return x[GATE_PTE]; }

/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A
   There are two write enable signals, one for each byte. WE0 is used for
   the least significant byte of a word. WE1 is used for the most significant
   byte of a word. */

#define WORDS_IN_MEM 0x2000 /* 32 frames */
#define MEM_CYCLES 5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT; /* run bit */
int BUS;     /* value of the bus */

typedef struct System_Latches_Struct
{

    int PC,  /* program counter */
        MDR, /* memory data register */
        MAR, /* memory address register */
        IR,  /* instruction register */
        N,   /* n condition bit */
        Z,   /* z condition bit */
        P,   /* p condition bit */
        BEN; /* ben register */

    int READY; /* ready bit */
               /* The ready bit is also latched as you dont want the memory system to assert it
                  at a bad point in the cycle*/

    int REGS[LC_3b_REGS]; /* register file. */

    int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

    int STATE_NUMBER; /* Current State Number - Provided for debugging */
    // Lab5 STATE_NUMBER used to determine RETR

    /* For lab 4 */
    int INTV; /* Interrupt vector register */
    int EXCV; /* Exception vector register */
    int SSP;  /* Initial value of system stack pointer */
    /* MODIFY: you should add here any other registers you need to implement interrupts and exceptions */
    int USP; // User Stack ptr reg
    int PSR; // Program Status Register
    int VCB; // Vector Condition Bits Register
    int VEC; // Vector Register (VectorTable begin + INTV or EXCV)

    /* For lab 5 */
    int PTBR; /* This is initialized when we load the page table */
    int VA;   /* Temporary VA register */
    /* MODIFY: you should add here any other registers you need to implement virtual memory */
    int RETR; // Register to hold return state after successful VM translation

} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/* For lab 5 */
#define PAGE_NUM_BITS 9
#define PTE_PFN_MASK 0x3E00
#define PTE_VALID_MASK 0x0004
#define PAGE_OFFSET_MASK 0x1FF

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help()
{
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle()
{

    eval_micro_sequencer();
    cycle_memory();
    eval_bus_drivers();
    drive_bus();
    latch_datapath_values();

    CURRENT_LATCHES = NEXT_LATCHES;

    CYCLE_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles)
{
    int i;

    if (RUN_BIT == FALSE)
    {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++)
    {
        if (CURRENT_LATCHES.PC == 0x0000)
        {
            RUN_BIT = FALSE;
            printf("Simulator halted\n\n");
            break;
        }
        cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed.                 */
/*                                                             */
/***************************************************************/
void go()
{
    if (RUN_BIT == FALSE)
    {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
        cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE *dumpsim_file, int start, int stop)
{
    int address; /* this is a byte address */

    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE *dumpsim_file)
{
    int k;

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    /* New Lab 4*/
    printf("\nPSR          : 0x%0.4x\n", CURRENT_LATCHES.PSR);
    printf("VCB          : 0x%0.4x\n", CURRENT_LATCHES.VCB);
    printf("INTV         : 0x%0.4x\n", CURRENT_LATCHES.INTV);
    printf("EXCV         : 0x%0.4x\n", CURRENT_LATCHES.EXCV);
    printf("VEC          : 0x%0.4x\n", CURRENT_LATCHES.VEC);
    printf("SSP          : 0x%0.4x\n", CURRENT_LATCHES.SSP);
    printf("USP          : 0x%0.4x\n", CURRENT_LATCHES.USP);
    /* New Lab 6 */
    printf("\nPTBR         : 0x%0.4x\n", CURRENT_LATCHES.PTBR);
    printf("VA           : 0x%0.4x\n", CURRENT_LATCHES.VA);
    printf("RETR         : 0x%0.4x\n\n", CURRENT_LATCHES.RETR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/***************************************************************/
void get_command(FILE *dumpsim_file)
{
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch (buffer[0])
    {
    case 'G':
    case 'g':
        go();
        break;

    case 'M':
    case 'm':
        scanf("%i %i", &start, &stop);
        mdump(dumpsim_file, start, stop);
        break;

    case '?':
        help();
        break;
    case 'Q':
    case 'q':
        printf("Bye.\n");
        exit(0);

    case 'R':
    case 'r':
        if (buffer[1] == 'd' || buffer[1] == 'D')
            rdump(dumpsim_file);
        else
        {
            scanf("%d", &cycles);
            run(cycles);
        }
        break;

    default:
        printf("Invalid Command\n");
        break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename)
{
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL)
    {
        printf("Error: Can't open micro-code file %s\n", ucode_filename);
        exit(-1);
    }

    /* Read a line for each row in the control store. */
    for (i = 0; i < CONTROL_STORE_ROWS; i++)
    {
        if (fscanf(ucode, "%[^\n]\n", line) == EOF)
        {
            printf("Error: Too few lines (%d) in micro-code file: %s\n",
                   i, ucode_filename);
            exit(-1);
        }

        /* Put in bits one at a time. */
        index = 0;

        for (j = 0; j < CONTROL_STORE_BITS; j++)
        {
            /* Needs to find enough bits in line. */
            if (line[index] == '\0')
            {
                printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
                       ucode_filename, i);
                exit(-1);
            }
            if (line[index] != '0' && line[index] != '1')
            {
                printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
                       ucode_filename, i, j);
                exit(-1);
            }

            /* Set the bit in the Control Store. */
            CONTROL_STORE[i][j] = (line[index] == '0') ? 0 : 1;
            index++;
        }

        /* Warn about extra bits in line. */
        if (line[index] != '\0')
            printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
                   ucode_filename, i);
    }
    printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory()
{
    int i;

    for (i = 0; i < WORDS_IN_MEM; i++)
    {
        MEMORY[i][0] = 0;
        MEMORY[i][1] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename, int is_virtual_base)
{
    FILE *prog;
    int ii, word, program_base, pte, virtual_pc;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL)
    {
        printf("Error: Can't open program file %s\n", program_filename);
        exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
        program_base = word >> 1;
    else
    {
        printf("Error: Program file is empty\n");
        exit(-1);
    }

    if (is_virtual_base)
    {
        if (CURRENT_LATCHES.PTBR == 0)
        {
            printf("Error: Page table base not loaded %s\n", program_filename);
            exit(-1);
        }

        /* convert virtual_base to physical_base */
        virtual_pc = program_base << 1;
        pte = (MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][1] << 8) |
              MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][0];

        printf("virtual base of program: %04x\npte: %04x\n", program_base << 1, pte);
        if ((pte & PTE_VALID_MASK) == PTE_VALID_MASK)
        {
            program_base = (pte & PTE_PFN_MASK) | ((program_base << 1) & PAGE_OFFSET_MASK);
            printf("physical base of program: %x\n\n", program_base);
            program_base = program_base >> 1;
        }
        else
        {
            printf("attempting to load a program into an invalid (non-resident) page\n\n");
            exit(-1);
        }
    }
    else
    {
        /* is page table */
        CURRENT_LATCHES.PTBR = program_base << 1;
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF)
    {
        /* Make sure it fits. */
        if (program_base + ii >= WORDS_IN_MEM)
        {
            printf("Error: Program file %s is too long to fit in memory. %x\n",
                   program_filename, ii);
            exit(-1);
        }

        /* Write the word to memory array. */
        MEMORY[program_base + ii][0] = word & 0x00FF;
        MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
        ;
        ii++;
    }

    if (CURRENT_LATCHES.PC == 0 && is_virtual_base)
        CURRENT_LATCHES.PC = virtual_pc;

    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */
/*             and set up initial state of the machine         */
/*                                                             */
/***************************************************************/
void initialize(char *argv[], int num_prog_files)
{
    int i;
    init_control_store(argv[1]);

    init_memory();
    load_program(argv[2], 0);
    for (i = 0; i < num_prog_files; i++)
    {
        load_program(argv[i + 3], 1);
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int) * CONTROL_STORE_BITS);
    CURRENT_LATCHES.SSP = 0x3000; /* Initial value of system stack pointer */

    /* MODIFY: you can add more initialization code HERE */
    // Lab 4
    CURRENT_LATCHES.PSR = 0x8002; /* Initial value of program status register */
    CURRENT_LATCHES.REGS[6] = 0xFE00;
    // Lab 5

    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[])
{
    FILE *dumpsim_file;

    /* Error Checking */
    if (argc < 4)
    {
        printf("Error: usage: %s <micro_code_file> <page table file> <program_file_1> <program_file_2> ...\n",
               argv[0]);
        exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv, argc - 3);

    if ((dumpsim_file = fopen("dumpsim", "w")) == NULL)
    {
        printf("Error: Can't open dumpsim file\n");
        exit(-1);
    }

    while (1)
        get_command(dumpsim_file);
}

/***************************************************************/
/* Do not modify the above code, except for the places indicated
   with a "MODIFY:" comment.
   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */
/***************************************************************/

void printIns(int toPrint[]);
int SEXT(int val, int bits);
void PSRCCSet(int source);

int memLast = -1;

// Interrupt Signal
int INT = 0;

// Gate evalution storage
int gateMarmux = -1;
int gatePC = -1;
int gateALU = -1;
int gateSHF = -1;
int gateMDR = -1;
int gatePTE = -1;

// new
int gateVEC = -1;
int gateSP = -1;
int gatePSR = -1;

// custom defines
#define src1BaseReg(regVal) ((regVal >> 6) & 0x7)
#define src119(regVal) ((regVal >> 9) & 0x7)
#define VecTableAddr 0x0200

void eval_micro_sequencer()
{
    // can be optimized
    /*
     * Evaluate the address of the next state according to the
     * micro sequencer logic. Latch the next microinstruction.
     */

    // TODO: CHECK THIS OUT
    if ((CURRENT_LATCHES.VCB > 0 || NEXT_LATCHES.VCB > 0))
    {
        // check vcb before mem access and replace current state
        int nextState = 38;
        CURRENT_LATCHES.STATE_NUMBER = nextState;
        NEXT_LATCHES.STATE_NUMBER = nextState;
        memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[nextState], sizeof(int) * CONTROL_STORE_BITS);
        memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[nextState], sizeof(int) * CONTROL_STORE_BITS);
    }
    else
    {
        // can "optimize" but I refuse; makes code less readable
        int j;
        int nextState;
        switch (GetIRD(CURRENT_LATCHES.MICROINSTRUCTION))
        {
        case 0:
            // normal listen to j bits + cond bits
            j = GetJ(CURRENT_LATCHES.MICROINSTRUCTION);
            switch (GetCOND(CURRENT_LATCHES.MICROINSTRUCTION))
            {
            case 0:
                // cond 00 just move to nxt state
                break;
            case 1:
                // listen for ready bit (01)
                // possible slow-down; come back later
                j = j | (CURRENT_LATCHES.READY << 1);
                break;
            case 2:
                // listen for branch (10)
                j = j | (CURRENT_LATCHES.BEN << 2);
                break;
            case 3:
                // listen for addr mode (11)
                // gets IR[11] and ands with j
                j = j | (((CURRENT_LATCHES.IR >> 11) & 1));
                break;
            case 4:
                // Interrupt
                j = j | ((GetINT_EN(CURRENT_LATCHES.MICROINSTRUCTION) & INT) << 3);
                break;
            case 5:
                // SP Restore Save
                j = j | (((CURRENT_LATCHES.PSR >> 15) & 1) << 4);
                j = j | (((NEXT_LATCHES.PSR >> 15) & 1) << 4);
                break;
            default:
                break;
            }
            NEXT_LATCHES.STATE_NUMBER = j;
            memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[j], sizeof(int) * CONTROL_STORE_BITS);
            break;

        case 1:
            // pull from the instruction register bits [15:12]
            // state 32
            nextState = ((CURRENT_LATCHES.IR >> 12) & 0xF);
            NEXT_LATCHES.STATE_NUMBER = nextState;
            memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[nextState], sizeof(int) * CONTROL_STORE_BITS);
            break;
        case 2:
            // Return to stored RETR after VM translation
            nextState = CURRENT_LATCHES.RETR;
            NEXT_LATCHES.STATE_NUMBER = nextState;
            memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[nextState], sizeof(int) * CONTROL_STORE_BITS);
            break;
        case 3:
            // unused
            break;
        }
    }
}

void cycle_memory()
{
    /*
     * This function emulates memory and the WE logic.
     * Keep track of which cycle of MEMEN we are dealing with.
     * If fourth, we need to latch Ready bit at the end of
     * cycle to prepare microsequencer for the fifth cycle.
     */

    NEXT_LATCHES.READY = 0;
    if (memLast == -1)
    {
        memLast = CYCLE_COUNT;
    }

    if (GetCOND(CURRENT_LATCHES.MICROINSTRUCTION) == 1 && CYCLE_COUNT - memLast == 4)
    {
        NEXT_LATCHES.READY = 1;
        memLast = CYCLE_COUNT;
    }
    else if (GetCOND(CURRENT_LATCHES.MICROINSTRUCTION) != 1)
    {
        memLast = CYCLE_COUNT;
    }
    if (CYCLE_COUNT == 300)
    {
        INT = 1;
    }

    //  Ready bit ready on end of fourth cycle
}

void eval_bus_drivers()
{

    /*
     * Datapath routine emulating operations before driving the bus.
     * Evaluate the input of tristate drivers
     *             Gate_MARMUX,
     *		 Gate_PC,
     *		 Gate_ALU,
     *		 Gate_SHF,
     *		 Gate_MDR.
     */

    // can optimize by checking all gates together first and returning if 0
    // might cause issues ; not constantly evaluating gates, currently only when gate is enabled
    if (GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION))
    {

        int IR_holder;
        switch (GetMARMUX(CURRENT_LATCHES.MICROINSTRUCTION))
        {
        case 0:
            // LSFH ZEXT
            gateMarmux = Low16bits((CURRENT_LATCHES.IR & 0xFF) << 1);
            break;
        case 1:
            // Addr adder mux
            // lshf 1 (addr2mux value) + ADDR1MUX value

            // get addr2mux val
            IR_holder = Low16bits(CURRENT_LATCHES.IR);
            int addr2MuxVal = -1;
            switch (GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION))
            {
            case 0:
                // load 0
                addr2MuxVal = 0;
                break;
            case 1:
                // load offset 6 from IR
                addr2MuxVal = SEXT(IR_holder & 0x3F, 6);
                break;
            case 2:
                // load offset 9 from IR
                addr2MuxVal = SEXT(IR_holder & 0x1FF, 9);
                break;
            case 3:
                // load offset 11 from IR
                addr2MuxVal = SEXT(IR_holder & 0x7FF, 11);
                break;
            }
            if (GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION))
            {
                addr2MuxVal = addr2MuxVal << 1;
            }

            // get addr1mux val
            int addr1MuxVal = -1;
            if (GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION))
            {
                // baseReg
                // IR[8:6] ?
                switch (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION))
                {
                case 0:
                    addr1MuxVal = CURRENT_LATCHES.REGS[src119(CURRENT_LATCHES.IR)];
                    break;
                case 1:
                    addr1MuxVal = CURRENT_LATCHES.REGS[src1BaseReg(CURRENT_LATCHES.IR)]; // reg number
                    break;
                case 2:
                    addr1MuxVal = CURRENT_LATCHES.REGS[6];
                    break;
                case 3:
                    // unused
                    break;
                }
            }
            else
            {
                // PC
                addr1MuxVal = CURRENT_LATCHES.PC;
            }
            gateMarmux = Low16bits(addr1MuxVal + addr2MuxVal);

            break;
        case 2:
            // Document later
            gateMarmux = (CURRENT_LATCHES.PTBR & 0xFF00) + (((CURRENT_LATCHES.MAR >> 9) & 0x7F) << 1);
            break;
        case 3:
            // Phys mem
            gateMarmux = (CURRENT_LATCHES.MDR & PTE_PFN_MASK) + (CURRENT_LATCHES.VA & 0xFF);
            break;
        }
    }
    else if (GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        gatePC = CURRENT_LATCHES.PC;
    }
    else if (GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION))
    {

        // SR1 + SR2MUX value

        // get sr1 val from reg file

        int sr1Val = -1;
        switch (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION))
        {
        case 0:
            sr1Val = CURRENT_LATCHES.REGS[src119(CURRENT_LATCHES.IR)];
            break;
        case 1:
            sr1Val = CURRENT_LATCHES.REGS[src1BaseReg(CURRENT_LATCHES.IR)]; // reg number
            break;
        case 2:
            sr1Val = CURRENT_LATCHES.REGS[6];
            break;
        case 3:
            // unused
            break;
        }

        // get SR2Mux value
        // bit 5 of IR is used to distinguish mux val
        int sr2MuxVal = -1;
        sr2MuxVal = ((CURRENT_LATCHES.IR >> 5) & 1);
        if (sr2MuxVal)
        {
            // use imm5 val
            sr2MuxVal = SEXT(CURRENT_LATCHES.IR & 0x1F, 5);
        }
        else
        {
            // use sr2 val
            sr2MuxVal = CURRENT_LATCHES.REGS[CURRENT_LATCHES.IR & 0x7];
        }

        switch (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION))
        {
        case 0:
            // add
            gateALU = Low16bits(sr1Val + sr2MuxVal);
            break;
        case 1:
            // and
            gateALU = Low16bits(sr1Val & sr2MuxVal);
            break;
        case 2:
            // xor
            gateALU = Low16bits(sr1Val ^ sr2MuxVal);
            break;
        case 3:
            // pass a
            gateALU = Low16bits(sr1Val);
            break;
        }
    }
    else if (GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        // use IR[5:4] to determine shift type and Log or Arith
        int type = ((CURRENT_LATCHES.IR >> 4) & 0x3);

        // CANT DO THIS MUST CHECK SR1MUX!!!!!!!!!!!!!!!!!
        int sr1Val = -1;
        switch (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION))
        {
        case 0:
            sr1Val = CURRENT_LATCHES.REGS[src119(CURRENT_LATCHES.IR)];
            break;
        case 1:
            sr1Val = CURRENT_LATCHES.REGS[src1BaseReg(CURRENT_LATCHES.IR)]; // reg number
            break;
        case 2:
            sr1Val = CURRENT_LATCHES.REGS[6];
            break;
        case 3:
            // unused
            break;
        }

        switch (type)
        {
        case 0:
            // lshf by imm4
            sr1Val = sr1Val << (CURRENT_LATCHES.IR & 0xF);
            break;

        case 1:
            // rshfl by imm4
            {
                int ammVal = CURRENT_LATCHES.IR & 0xF;
                int mask = 1;

                for (int i = 0; i < ammVal; i++)
                {
                    mask = (mask * 2) + 1;
                }
                mask = Low16bits(~(mask << (16 - ammVal)));

                sr1Val = (sr1Val >> ammVal) & mask;

                break;
            }

        case 3:
            // rshfa by imm4
            {
                int ammVal = CURRENT_LATCHES.IR & 0xF;
                int mask = 1;

                for (int i = 0; i < ammVal; i++)
                {
                    mask = (mask * 2) + 1;
                }
                mask = Low16bits((mask << (16 - ammVal)));

                int signPreserve = (sr1Val >> 15) & 1;

                if (signPreserve)
                {
                    sr1Val = (sr1Val >> ammVal) | mask;
                }
                else
                {
                    mask = Low16bits(~mask);
                    sr1Val = (sr1Val >> ammVal) & mask;
                }
                break;
            }
        }
        gateSHF = Low16bits(sr1Val);
    }
    else if (GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION))
    {

        if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION))
        {
            // if size is 1 load word
            gateMDR = CURRENT_LATCHES.MDR;
        }
        else
        {
            // if size is 0 load byte
            if (CURRENT_LATCHES.MAR & 1)
            {
                // if 1 get high byte
                gateMDR = SEXT(((CURRENT_LATCHES.MDR >> 8) & 0xFF), 8);
            }
            else
            {
                // get low byte
                gateMDR = SEXT((CURRENT_LATCHES.MDR & 0xFF), 8);
            }
        }
    }
    else if (GetGATE_VEC(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        gateVEC = CURRENT_LATCHES.VEC;
    }
    else if (GetGATE_SP(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        // defining SR1 to be 10 to always get r6
        switch (GetSPMUX(CURRENT_LATCHES.MICROINSTRUCTION))
        {
        case 0:
            /* code */
            gateSP = Low16bits(CURRENT_LATCHES.REGS[6] - 2);
            break;
        case 1:
            gateSP = CURRENT_LATCHES.SSP;
            break;
        case 2:
            gateSP = CURRENT_LATCHES.USP;
            break;
        case 3:
            gateSP = Low16bits(CURRENT_LATCHES.REGS[6] + 2);
            break;
        }
    }
    else if (GetGATE_PSR(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        gatePSR = CURRENT_LATCHES.PSR;
    }
    else if (GetGate_PTE(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        // take current value of MDR (PTE) and set/clr modified bit, set reference bit
        gatePTE = (CURRENT_LATCHES.MDR & 0xFFFC) + ((CURRENT_LATCHES.PSR & 0x8) >> 3) + 1;
    }
}

void drive_bus()
{

    /*
     * Datapath routine for driving the bus from one of the 5 possible
     * tristate drivers.
     */

    if (GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        BUS = gateMarmux;
    }
    else if (GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        BUS = gatePC;
    }
    else if (GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        BUS = gateALU;
    }
    else if (GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        BUS = gateSHF;
    }
    else if (GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        BUS = gateMDR;
    }
    else if (GetGATE_VEC(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        BUS = gateVEC;
    }
    else if (GetGATE_SP(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        BUS = gateSP;
    }
    else if (GetGATE_PSR(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        BUS = gatePSR;
    }
    else if (GetGate_PTE(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        BUS = gatePTE;
    }
    // else
    // {
    //     // might not be okay
    //     BUS = 0;
    // }
}

void latch_datapath_values()
{

    /*
     * Datapath routine for computing all functions that need to latch
     * values in the data path at the end of this cycle.  Some values
     * require sourcing the bus; therefore, this routine has to come
     * after drive_bus.
     */
    //  AFTER DRIVE_BUS METHOD

    // optimize by checking for a combination of all signals at once

    if (GetLD_MAR(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        NEXT_LATCHES.MAR = BUS;
    }
    if (GetLD_MDR(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        // Assume everything is already on the bus
        if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION))
        {
            // load
            int marValLow = MEMORY[CURRENT_LATCHES.MAR >> 1][0];
            int marValHigh = MEMORY[CURRENT_LATCHES.MAR >> 1][1];

            int store;
            store = Low16bits(marValLow + (marValHigh << 8));
            NEXT_LATCHES.MDR = store;
        }
        else
        {
            NEXT_LATCHES.MDR = BUS;
        }
    }
    if (GetLD_IR(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        NEXT_LATCHES.IR = BUS;
    }
    if (GetLD_BEN(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        NEXT_LATCHES.BEN = (((CURRENT_LATCHES.IR >> 11) & 1) & CURRENT_LATCHES.N) | (((CURRENT_LATCHES.IR >> 10) & 1) & CURRENT_LATCHES.Z) | (((CURRENT_LATCHES.IR >> 9) & 1) & CURRENT_LATCHES.P);
    }
    if (GetLD_PC(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        switch (GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION))
        {
        case 0:
            // increment pc by 2
            NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
            break;
        case 1:
            // Load bus into pc
            NEXT_LATCHES.PC = BUS;
            break;
        case 2:
            // TODO: Create a seperate func for getting addr mux result
            //  Addr adder mux
            //  lshf 1 (addr2mux value) + ADDR1MUX value
            {
                // get addr2mux val
                int IR_holder = Low16bits(CURRENT_LATCHES.IR);
                int addr2MuxVal = -1;
                switch (GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION))
                {
                case 0:
                    // load 0
                    addr2MuxVal = 0;
                    break;
                case 1:
                    // load offset 6 from IR
                    addr2MuxVal = SEXT(IR_holder & 0x3F, 6);
                    break;
                case 2:
                    // load offset 9 from IR
                    addr2MuxVal = SEXT(IR_holder & 0x1FF, 9);
                    break;
                case 3:
                    // load offset 11 from IR
                    addr2MuxVal = SEXT(IR_holder & 0x7FF, 11);
                    break;
                }
                if (GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION))
                {
                    addr2MuxVal = addr2MuxVal << 1;
                }

                // get addr1mux val
                int addr1MuxVal = -1;
                if (GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION))
                {
                    // baseReg
                    // how to read reg file sr1 out?
                    // IR[8:6] ?
                    switch (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION))
                    {
                    case 0:
                        addr1MuxVal = CURRENT_LATCHES.REGS[src119(CURRENT_LATCHES.IR)];
                        break;
                    case 1:
                        addr1MuxVal = CURRENT_LATCHES.REGS[src1BaseReg(CURRENT_LATCHES.IR)]; // reg number
                        break;
                    case 2:
                        addr1MuxVal = CURRENT_LATCHES.REGS[6];
                        break;
                    case 3:
                        // unused
                        break;
                    }
                }
                else
                {
                    // PC
                    addr1MuxVal = CURRENT_LATCHES.PC;
                }
                NEXT_LATCHES.PC = Low16bits(addr1MuxVal + addr2MuxVal);
                break;
            }
        case 3:
            NEXT_LATCHES.PC = CURRENT_LATCHES.PC - 2;
            break;
        }
    }
    if (GetLD_REG(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        int loader;
        // deciding on reg source
        if (GetREGMUX(CURRENT_LATCHES.MICROINSTRUCTION))
        {
            // take from SPMUX
            switch (GetSPMUX(CURRENT_LATCHES.MICROINSTRUCTION))
            {
            case 0:
                /* code */
                loader = Low16bits(CURRENT_LATCHES.REGS[6] - 2);
                break;
            case 1:
                loader = CURRENT_LATCHES.SSP;
                break;
            case 2:
                loader = CURRENT_LATCHES.USP;
                break;
            case 3:
                loader = Low16bits(CURRENT_LATCHES.REGS[6] + 2);
                break;
            }
        }
        else
        {
            // take from bus
            loader = Low16bits(BUS);
        }

        switch (GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION))
        {
        case 0:
            NEXT_LATCHES.REGS[((CURRENT_LATCHES.IR >> 9) & 0x7)] = loader;
            break;
        case 1:
            NEXT_LATCHES.REGS[7] = loader;
            break;
        case 2:
            NEXT_LATCHES.REGS[6] = loader;
            break;
        case 3:
            // unused
            break;
        }
    }
    if (GetLD_CC(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        // if PSR is being gated want to snag cc bits from there
        if (GetLD_PSR(CURRENT_LATCHES.MICROINSTRUCTION) && (GetPSRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2 || GetPSRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0))
        {
            // old psr is being loaded
            // get info from MDR (direct wire)
            int psrCC = Low16bits(CURRENT_LATCHES.MDR);
            switch (GetPSRMUX(CURRENT_LATCHES.MICROINSTRUCTION))
            {
            // 00 - 0x0002
            // 01 - PSR'NZP (for when NZP modifies PSR)
            // 10 - MDR
            // 11 - Unused
            case 0:
                psrCC = 0x0002;
                break;
            case 2:
                psrCC = CURRENT_LATCHES.MDR;
                break;
            default:
                printf("Should not reach this case");
                break;
            }
            PSRCCSet(psrCC);
        }
        else
        {
            // otherwise check the DR
            int drVal = -1;
            switch (GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION))
            {
            case 0:
                drVal = (CURRENT_LATCHES.IR >> 9) & 0x7;
                break;
            case 1:
                drVal = 7;
                break;
            case 2:
                drVal = 6;
                break;
            case 3:
                // unused
                break;
            }
            drVal = NEXT_LATCHES.REGS[drVal];

            int signBit = (drVal >> 15) & 1;
            if (drVal == 0)
            {
                // zero
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 1;
                NEXT_LATCHES.P = 0;
            }
            else if (signBit == 0)
            {
                // pos
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 1;
            }
            else if (signBit == 1)
            {
                // negative
                NEXT_LATCHES.N = 1;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 0;
            }
        }
        // Check the DRMUX to see where it is then cmp DR val to pos neg and zero
    }
    if (GetLD_PSR(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        switch (GetPSRMUX(CURRENT_LATCHES.MICROINSTRUCTION))
        {
        case 0:
            NEXT_LATCHES.PSR = 0x0002;
            break;
        case 1:
            NEXT_LATCHES.PSR = (CURRENT_LATCHES.PSR & 0x8000) | (CURRENT_LATCHES.N << 2) | (CURRENT_LATCHES.Z << 1) | (CURRENT_LATCHES.P);
            break;
        case 2:
            NEXT_LATCHES.PSR = CURRENT_LATCHES.MDR;
            break;
        case 3:
            // Clr write bit in PSR
            NEXT_LATCHES.PSR = CURRENT_LATCHES.PSR & 0xFFF7;
            break;
        }
    }
    if (GetLD_INTV(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        if (INT && GetINT_EN(CURRENT_LATCHES.MICROINSTRUCTION))
        {
            NEXT_LATCHES.INTV = 1;
        }
    }
    if (GetLD_EXCV(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        // VCB has already been set
        // Clear VCB here
        if (CURRENT_LATCHES.VCB & 1)
        {
            // UA
            NEXT_LATCHES.EXCV = 4;
            NEXT_LATCHES.VCB = 0;
        }
        else if ((CURRENT_LATCHES.VCB >> 2) & 1)
        {
            // PA
            NEXT_LATCHES.EXCV = 3;
            NEXT_LATCHES.VCB = 0;
        }
        else if ((CURRENT_LATCHES.VCB >> 3) & 1)
        {
            // PF
            NEXT_LATCHES.EXCV = 2;
            NEXT_LATCHES.VCB = 0;
        }
        else if ((CURRENT_LATCHES.VCB >> 1) & 1)
        {
            // UO
            NEXT_LATCHES.EXCV = 5;
            NEXT_LATCHES.VCB = 0;
        }
    }
    if (GetLD_VCB(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        // check next latches MAR
        int val = 0;
        if (GetPF_EN(CURRENT_LATCHES.MICROINSTRUCTION) && !((CURRENT_LATCHES.MDR >> 2) & 1))
        {
            // page fault err
            val = val | (1 << 3);
        }
        if (GetPA_EN(CURRENT_LATCHES.MICROINSTRUCTION) && !((CURRENT_LATCHES.MDR >> 3) & 1) && ((NEXT_LATCHES.PSR >> 15) & 1))
        {
            // Protection Access Exception
            val = val | (1 << 2);
        }

        if (GetUOP_EN(CURRENT_LATCHES.MICROINSTRUCTION))
        {
            val = val | (1 << 1);
        }
        if (GetUA_EN(CURRENT_LATCHES.MICROINSTRUCTION) && (NEXT_LATCHES.MAR & 1) && ((NEXT_LATCHES.PSR >> 3) & 1))
        {
            val = val | 1;
        }
        NEXT_LATCHES.VCB = val;
    }
    if (GetLD_SSP(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        NEXT_LATCHES.SSP = CURRENT_LATCHES.REGS[6];
    }
    if (GetLD_USP(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        NEXT_LATCHES.USP = CURRENT_LATCHES.REGS[6];
    }
    if (GetLD_VEC(CURRENT_LATCHES.MICROINSTRUCTION))
    {

        int loader;
        if (GetINT_EN(CURRENT_LATCHES.MICROINSTRUCTION) && INT)
        {
            loader = NEXT_LATCHES.INTV;
            INT = 0; // INT has been fully serviced (µsequence and LD_INTV got to it)
            loader = Low16bits((loader << 1) + VecTableAddr);
            NEXT_LATCHES.VEC = loader;
        }
        else if (GetEXC_EN(CURRENT_LATCHES.MICROINSTRUCTION))
        {
            loader = NEXT_LATCHES.EXCV;
            loader = Low16bits((loader << 1) + VecTableAddr);
            NEXT_LATCHES.VEC = loader;
        }
        // hard code beginning of vec table
    }
    if (GetLD_VA(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        NEXT_LATCHES.VA = CURRENT_LATCHES.MAR;
    }
    if (GetLD_RETR(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        switch (CURRENT_LATCHES.STATE_NUMBER)
        {
        case 2:
            NEXT_LATCHES.RETR = 29;
            break;
        case 3:
            NEXT_LATCHES.RETR = 24;
            break;
        case 6:
            NEXT_LATCHES.RETR = 25;
            break;
        case 7:
            NEXT_LATCHES.RETR = 23;
            break;
        case 18:
            NEXT_LATCHES.RETR = 33;
            break;
        case 19:
            NEXT_LATCHES.RETR = 33;
            break;
        default:
            printf("\nUnexpected State loading the return register for VA translation\n");
            break;
        }
    }
    if (GetR_W(CURRENT_LATCHES.MICROINSTRUCTION) && GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        // The write

        int mdrValLow = (CURRENT_LATCHES.MDR & 0xFF);
        int mdrValHigh = ((CURRENT_LATCHES.MDR >> 8) & 0xFF);

        int store;
        if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION))
        {
            // size is word; read from MDR
            MEMORY[CURRENT_LATCHES.MAR >> 1][0] = mdrValLow;
            MEMORY[CURRENT_LATCHES.MAR >> 1][1] = mdrValHigh;
        }
        else
        {
            // size is byte; read from MDR
            if (CURRENT_LATCHES.MAR & 1)
            {
                // use high byte
                MEMORY[CURRENT_LATCHES.MAR >> 1][1] = mdrValLow;
            }
            else
            {
                // use low byte
                MEMORY[CURRENT_LATCHES.MAR >> 1][0] = mdrValLow;
            }
        }
    }
}

void printIns(int toPrint[])
{
    for (int i = 0; i < 35; i++)
    {
        printf("%d", toPrint[i]);
    }
    printf("\n");
}

int SEXT(int value, int bits)
{
    int store = value;
    int sign = ((store & (1 << (bits - 1))) >> (bits - 1));

    int mask = ~(0 << (15 - bits));
    mask = mask << bits;

    if (sign == 1)
    {
        // neg
        store = mask | store;
    }
    else
    {
        // pos
        mask = ~mask;
        store = mask & store;
    }
    store = Low16bits(store);
    return store;
}

void PSRCCSet(int source)
{
    // sets cc based on PSR
    source = source & 0x7;
    NEXT_LATCHES.P = 0;
    NEXT_LATCHES.Z = 0;
    NEXT_LATCHES.N = 0;
    switch (source)
    {
    case 1:
        NEXT_LATCHES.P = 1;
        break;
    case 2:
        NEXT_LATCHES.Z = 1;
        break;
    case 4:
        NEXT_LATCHES.N = 1;
        break;
    default:
        printf("\nPSR has bad CC bits\n");
        // kill pc if PSR loaded incorrectly
        // exit(5);
        break;
    }
}