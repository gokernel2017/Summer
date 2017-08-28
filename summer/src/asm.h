//-------------------------------------------------------------------
//
// Fast JIT (just-in-time):
//
// TANKS TO:
// ----------------------------------------------
//
//   01: God the creator of the heavens and the earth in the name of Jesus Christ.
//
//   02 - Fabrice Bellard: www.bellard.org
//
// ----------------------------------------------
//
// FILE:
//   asm.h
//
// START DATE:
//   23/08/2017 - 11:45
//
// BY Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#ifndef _ASM_H_
#define _ASM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#ifdef WIN32
    #include <windows.h>
#endif
#ifdef __linux__
    #include <unistd.h>
    #include <sys/mman.h> // to: mprotect()
#endif
#if defined(__x86_64__)
    #error "ERRO: ##########  This example suport only 32 BITS  ##########"
#endif

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------
//---------------  DEFINE / ENUM  ---------------
//-----------------------------------------------
//
#define ASM_VERSION         0
#define ASM_VERSION_SUB     1
#define ASM_VERSION_PATCH   0

#define ASM_DEFAULT_SIZE    50000

#define UCHAR               unsigned char
#define ULONG               unsigned long
#define ASM_LEN(arg)        (arg->p - arg->code)
#define OP_NOP              0x90

enum { // jump type for change the labels
    ASM_JUMP_JMP = 1,
    ASM_JUMP_JNE,
    ASM_JUMP_JLE,
    ASM_JUMP_JGE,
    ASM_JUMP_JG,
    ASM_JUMP_JE,
    ASM_JUMP_JL,
    ASM_JUMP_LOOP
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
    ASM_label *label;
    ASM_jump  *jump;
    ULONG     size;
};
struct ASM_label {
    char      *name;
    int       pos;
    ASM_label *next;
};
struct ASM_jump {
    char      *name;
    int       pos;
    int       type;
    ASM_jump  *next;
};

//-------------------------------------------------------------------
//-------------------------  ASM PUBLIC API  ------------------------
//-------------------------------------------------------------------
//
extern ASM  * asm_new             (unsigned long size);
extern void   asm_free            (ASM *a);
extern void   asm_reset           (ASM *a);
extern void   asm_set_executable  (void *ptr, unsigned long len);
extern void   asm_label           (ASM *a, char *name); // create a label

//--------------------------------
// gen / emit:
//--------------------------------
//
extern void   asm_begin           (ASM *a);
extern void   asm_end             (ASM *a);
extern void   asm_get_addr        (ASM *a, void *ptr);

extern void   asm_movl            (ASM *a, long value, void *var);
extern void   asm_incl            (ASM *a, void *var);
extern void   asm_cmpl            (ASM *a, long value, void *var);

// jumps:
extern void   asm_jmp             (ASM *a, char *name); // jump to label
extern void   asm_jg              (ASM *a, char *name);

extern void   asm_call            (ASM *a, void *func);

#ifdef __cplusplus
}
#endif

#endif // ! _ASM_H_
