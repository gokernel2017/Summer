//-------------------------------------------------------------------
//
// VM (Virtual Machine).
//
//   Main Header.
//
// TANKS TO:
// ----------------------------------------------
//
//   01: God the creator of the heavens and the earth in the name of Jesus Christ.
//
// ----------------------------------------------
//
// FILE:
//   vm.h 
//
// START DATE:
//   29/08/2017 - 17:42
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#ifndef _VM_H_
#define _VM_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "def.h"

//-----------------------------------------------
//---------------  DEFINE / ENUM  ---------------
//-----------------------------------------------
//
#define VM_VERSION        0
#define VM_VERSION_SUB    1
#define VM_VERSION_PATCH  0

#define ASM_DEFAULT_SIZE  50000

enum {
    OP_HALT = 0,  // exit of (vm_run) loop

    OP_PUSHL,     // push a long value on stack | sp++.
    OP_PUSHF,     // push a float value on stack | sp++

    OP_PUSHVAR,   // LONG: push a variable long value on stack | sp++
    OP_POPVAR,    // copy the value of (sp) to variable index | sp--.
    OP_INCVAR,

    OP_MULL,
    OP_DIVL,
    OP_ADDL,
    OP_SUBL,

    OP_MULF,
    OP_DIVF,
    OP_ADDF,
    OP_SUBF,

    OP_CMPL,      // compare long | sp--, sp--
    OP_JMP,
    OP_JEQ,       // ==
    OP_JNE,       // !=
    OP_JG,        // <

    OP_PRINTVAR,
    OP_PRINTS,
    OP_PRINTC,

    OP_CALL,      // call a C Function
    OP_CALLVM     // call a VM Function
};

//-----------------------------------------------
//-------------------  STRUCT  ------------------
//-----------------------------------------------
//
typedef struct ASM        ASM;
typedef struct ASM_label  ASM_label;
typedef struct ASM_jump   ASM_jump;

struct ASM {
    UCHAR     *p;
    UCHAR     *code;
    int       ip;
    ASM_label *label;
    ASM_jump  *jump;
};
struct ASM_label {
    char      *name;
    int       pos;
    ASM_label *next;
};
struct ASM_jump {
    char      *name;
    int       pos;
    ASM_jump  *next;
};

//-------------------------------------------------------------------
//-------------------------  GLOBAL VARIABLE  -----------------------
//-------------------------------------------------------------------
//
extern int erro;

//-------------------------------------------------------------------
//-------------------------  VM PUBLIC API  -------------------------
//-------------------------------------------------------------------
//
extern ASM  *asm_new        (unsigned long size);
extern void vm_run          (ASM *vm);
extern void vm_label        (ASM *vm, char *name);
//
// emit:
//
extern void asm_end         (ASM *vm);
extern void vme_pushl       (ASM *vm, long value);
extern void vme_pushf       (ASM *vm, float value);
extern void vme_pushvar     (ASM *vm, UCHAR index);
extern void vme_addl        (ASM *vm);
extern void vme_mull        (ASM *vm);
extern void vme_addf        (ASM *vm);
extern void vme_mulf        (ASM *vm);
extern void vme_popvar      (ASM *vm, UCHAR i);
extern void vme_incvar      (ASM *vm, UCHAR index);
//
// print:
//
extern void vme_printvar    (ASM *vm, UCHAR index);
extern void vme_printc      (ASM *vm, char c);
extern void vme_prints      (ASM *vm, UCHAR size, const char *str);

extern void vme_cmpl        (ASM *vm);
extern void vme_jmp         (ASM *vm, char *name);
extern void vme_jg          (ASM *vm, char *name);

extern void asm_call        (ASM *vm, void *func, UCHAR argc, UCHAR ret);
extern void asm_callvm      (ASM *vm, void *func);
extern void vme_argc        (ASM *vm, UCHAR c);

extern void asm_reset       (ASM *vm);
extern void asm_Erro        (char *s);
extern char *asm_ErroGet    (void);
extern void asm_ErroReset   (void);

#endif // ! _VM_H_
