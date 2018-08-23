//-------------------------------------------------------------------
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
// THIS FILE IS PART OF SUMMER LANGUAGE:
//
// The back-end | JIT:
//
// FILE:
//   asm.h
//
// SUMMER LANGUAGE START DATE ( 27/08/2017 - 08:35 ):
//   rewrite: 20/07/2018 - 11:10
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#ifndef _ASM_H_
#define _ASM_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef WIN32
    #include <windows.h>
#endif
#ifdef __linux__
    #include <unistd.h>
    #include <sys/mman.h> // to: mprotect()
    #include <dlfcn.h>    // to: dlopen(), dlsym(), ... in file: cs_library.c
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define LIBIMPORT         extern
#define ASM_DEFAULT_SIZE  50000
#define UCHAR             unsigned char

//
// pass 5 arguments to function:
//
#define G2_MOV_EAX_EDI    0x89, 0xc7        // 89 c7      : mov   %eax, %edi
#define G2_MOV_EAX_ESI    0x89, 0xc6        // 89 c6      : mov   %eax, %esi
#define G2_MOV_EAX_EDX    0x89, 0xc2        // 89 c2      : mov   %eax, %edx
#define G2_MOV_EAX_ECX    0x89, 0xc1        // 89 c1      : mov   %eax, %ecx
#define G3_MOV_EAX_r8d    0x41, 0x89, 0xc0  // 41 89 c0   : mov %eax, %r8d

enum {
    EAX = 0,
    ECX,
    EDX,
    EBX,
    ESP,
    EBP,
    ESI,
    EDI
};

//-----------------------------------------------
//------------------  STRUCT  -------------------
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
    int       size;
    int       ip; // not used
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

LIBIMPORT void    Run                 (ASM *a); // back-end in file: vm.c
LIBIMPORT int     asm_set_executable  (ASM *a, unsigned int size);
LIBIMPORT ASM   * asm_new             (unsigned int size);
LIBIMPORT void    asm_reset           (ASM *a);
LIBIMPORT void    asm_begin           (ASM *a); // 32/64 BITS OK
LIBIMPORT void    asm_end             (ASM *a); // 32/64 BITS OK
LIBIMPORT int     asm_get_len         (ASM *a);
LIBIMPORT void    asm_get_addr        (ASM *a, void *ptr); // 32/64 BITS OK
// gen/emit:
LIBIMPORT void    g                   (ASM *a, UCHAR c);
LIBIMPORT void    g2                  (ASM *a, UCHAR c1, UCHAR c2);
LIBIMPORT void    g3                  (ASM *a, UCHAR c1, UCHAR c2, UCHAR c3);
LIBIMPORT void    g4                  (ASM *a, UCHAR c1, UCHAR c2, UCHAR c3, UCHAR c4);
LIBIMPORT void    emit_push_int       (ASM *a, int value);
LIBIMPORT void    emit_push_var       (ASM *a, void *var);
LIBIMPORT void    emit_pop_var        (ASM *a, void *var);

//-----------------------------------------------
// int expression math:
//-----------------------------------------------
#define emit_mul_int  asm_imul_eax_esp
#define emit_div_int  asm_idivl_eax_esp
#define emit_add_int  asm_add_eax_esp
#define emit_sub_int  asm_sub_eax_esp
LIBIMPORT void asm_imul_eax_esp   (ASM *a);
LIBIMPORT void asm_idivl_eax_esp  (ASM *a);
LIBIMPORT void asm_add_eax_esp    (ASM *a);
LIBIMPORT void asm_sub_eax_esp    (ASM *a);
//-----------------------------------------------
LIBIMPORT void emit_call          (ASM *a, void *func, UCHAR arg_count, UCHAR return_type);

LIBIMPORT void emit_pop_eax       (ASM *a);
LIBIMPORT void emit_movl_ESP      (ASM *a, long value, UCHAR index); // movl    $0x5dc,0x4(%esp)
LIBIMPORT void emit_mov_eax_ESP   (ASM *a, UCHAR index); // mov    %eax,0x4(%esp)
LIBIMPORT void emit_mov_var_reg   (ASM *a, void *var, int reg); // move: variable to %register
LIBIMPORT void emit_mov_reg_var   (ASM *a, int reg, void *var); // 32/64 BITS OK - move: %register to variable

#ifdef __cplusplus
}
#endif

#endif // ! _ASM_H
