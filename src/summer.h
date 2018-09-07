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
// The Main Header:
//
// FILE:
//   summer.h
//
// SUMMER LANGUAGE START DATE ( 27/08/2017 - 08:35 ):
//   rewrite: 20/07/2018 - 11:10
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
// FILE SIZE: 9.796
#ifndef _SUMMER_H_
#define _SUMMER_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef WIN32
    #include <windows.h>

    #define USE_APPLICATION

    typedef HWND OBJECT;

#endif
#ifdef __linux__
    #include <unistd.h>
    #include <sys/mman.h> // to: mprotect()
    #include <dlfcn.h>    // to: dlopen(), dlsym(), ... in file: cs_library.c

    #ifdef USE_APPLICATION
    typedef Window OBJECT;
    #endif

#endif

#include "lex.h"
#include "config.h"

#if defined(__x86_64__)
//    #error "ERRO: ##########  This example suport only 32 BITS  ##########"
#endif

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------
//---------------  DEFINE / ENUM  ---------------
//-----------------------------------------------
//
#define SUMMER_VERSION        0
#define SUMMER_VERSION_SUB    11
#define SUMMER_VERSION_PATCH  0
#define SUMMER_VERSION_NAME   "BETA"
//
//------------------------------------------------
//
// !!!!  HELLO !!!! : Defined im file: "lex.h"
//
//#define LIBIMPORT             extern
//
//------------------------------------------------
//
#define ASM_DEFAULT_SIZE      50000
#define UCHAR                 unsigned char
#define STR_ERRO_SIZE         1024
#define GVAR_SIZE             255
#define TYPE_NO_RETURN        100
#define OP_NOP                0x90
//
// pass 5 arguments to function:
//
#define G2_MOV_EAX_EDI    0x89, 0xc7        // 89 c7      : mov   %eax, %edi
#define G2_MOV_EAX_ESI    0x89, 0xc6        // 89 c6      : mov   %eax, %esi
#define G2_MOV_EAX_EDX    0x89, 0xc2        // 89 c2      : mov   %eax, %edx
#define G2_MOV_EAX_ECX    0x89, 0xc1        // 89 c1      : mov   %eax, %ecx
#define G3_MOV_EAX_r8d    0x41, 0x89, 0xc0  // 41 89 c0   : mov %eax, %r8d
#define G3_MOV_EAX_r9d    0x41, 0x89, 0xc1  // 41 89 c1   : mov %eax, %r9d

enum {
    TYPE_INT = 0,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_POINTER,
    TYPE_STRUCT,
    TYPE_PSTRUCT  // struct data *p;
};
enum {
    FUNC_TYPE_NATIVE_C = 0,
    FUNC_TYPE_COMPILED,
    FUNC_TYPE_VM
};
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
enum { // VM opcodes:
    OP_PUSH_INT,
    OP_PUSH_FLOAT,
    OP_PUSH_VAR,
    OP_POP_VAR,

    OP_MUL_INT,
    OP_DIV_INT,
    OP_ADD_INT,
    OP_SUB_INT,

    OP_MUL_FLOAT,
    OP_DIV_FLOAT,
    OP_ADD_FLOAT,
    OP_SUB_FLOAT,


    // compare and jumps:
    OP_CMP_INT,
    OP_JUMP_JMP,
    OP_JUMP_JGE,
    OP_JUMP_JLE,
    OP_JUMP_JE,
    OP_JUMP_JNE,
    OP_JUMP_JG,
    OP_JUMP_JL,

    OP_POP_EAX,
    OP_PRINT_EAX,

    OP_MOV_EAX_VAR,

    OP_PUSH_STRING,

    OP_INC_VAR_INT,
    OP_DEC_VAR_INT,

    OP_CALL_VM,
    OP_CALL,

    OP_HALT
};


//-----------------------------------------------
//------------------  STRUCT  -------------------
//-----------------------------------------------
//
typedef union  VALUE      VALUE;
//
typedef struct ASM        ASM;
typedef struct ASM_label  ASM_label;
typedef struct ASM_jump   ASM_jump;
//
typedef struct TVar       TVar;
typedef struct TFunc      TFunc;
typedef struct F_STRING   F_STRING;
typedef struct MODULE     MODULE;
typedef struct DEFINE     DEFINE;

typedef struct TEvent     TEvent;

union VALUE {
    int     i;  //: type integer
    float   f;  //: type float
    char    *s; //: type pointer of char
    void    *p; //: type pointer
};
struct ASM {
    UCHAR     *p;
    UCHAR     *code;
    ASM_label *label;
    ASM_jump  *jump;
    int       size;
    //-------------------------------------------
    #ifdef USE_VM
    int       ip;
    VALUE     arg [10];
    #endif
};
//
#ifdef USE_JIT
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
#endif // #ifdef USE_JIT
#ifdef USE_VM
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
#endif // #ifdef USE_VM
//
struct TVar {
    char    *name;
    int     type;
    VALUE   value;
    void    *info;  // any information ... struct type use this
};
struct TFunc {
    char    *name;
    char    *proto; // prototype
    UCHAR   *code;  // the function on JIT MODE | or VM in VM MODE
    int     type;   // FUNC_TYPE_NATIVE_C = 0, FUNC_TYPE_COMPILED, FUNC_TYPE_VM
    int     len;
    TFunc   *next;
};
struct F_STRING {
    char *s;
    int   i;
    F_STRING *next;
}; // fixed string
struct MODULE {
    char    *name;
    void    *lib;
    TFunc   *func;
    MODULE  *next;
};
typedef struct {
    char  type[20]; // "int", "float", "data_struct"
    char  name[20];
}ARG;
struct DEFINE {
    char    *name;
    int     value;
    DEFINE  *next;
};
struct TEvent {
    int   type;
    int   id;
    int   value;  // mouse_button, key
    int   x, y;   // position, size
};

// global:
LIBIMPORT TVar  Gvar [GVAR_SIZE];
LIBIMPORT int   asm_mode;
LIBIMPORT char  write_var_name [100];
LIBIMPORT char  write_func_name [100];

//-------------------------------------------------------------------
//---------------------------  PUBLIC API  --------------------------
//-------------------------------------------------------------------
//
// In File: "application.c"
//
#ifdef USE_APPLICATION
LIBIMPORT int     AppInit           (int argc, char **argv);
LIBIMPORT void    AppRun            (void);
LIBIMPORT OBJECT  AppNewWindow      (OBJECT parent, int x, int y, int w, int h, char *text);
LIBIMPORT OBJECT  AppNewButton      (OBJECT parent, int x, int y, int w, int h, char *text);
LIBIMPORT void    AppSetCall        (OBJECT o, void(*call)(TEvent *evevt));
#endif

LIBIMPORT void    Run               (ASM *a); // back-end in file: asm.c | vm.c
//
// FILE: "core.c"
//
LIBIMPORT ASM   * core_Init         (unsigned int size);
LIBIMPORT void    core_Finalize     (void);
LIBIMPORT char  * FileOpen          (const char *FileName);
LIBIMPORT void    CreateVar         (char *name, char *svalue);
LIBIMPORT void    CreateVarInt      (char *name, int value);
LIBIMPORT TFunc * FuncFind          (char *name);
LIBIMPORT int     VarFind           (char *name);
LIBIMPORT TFunc * ModuleFind        (char *LibName, char *FuncName);
LIBIMPORT int     ModuleIsLib       (char *LibName); // rwturn:1 or 0
LIBIMPORT int     Parse             (LEXER *l, ASM *a, char *text, char *name);
//
LIBIMPORT void    Erro              (char *format, ...);
LIBIMPORT char  * ErroGet           (void);
LIBIMPORT void    ErroReset         (void);
//
//-------------------------------------------------------------------
// BACK-END in file: asm.c | vm.c
//-------------------------------------------------------------------
//
#ifdef USE_JIT
LIBIMPORT void emit_expression_pop_64_int (ASM *a);
LIBIMPORT void emit_expression_pop_64_float (ASM *a);
//
LIBIMPORT int     set_executable    (void *ptr, unsigned int size);
LIBIMPORT void    asm_get_addr      (ASM *a, void *ptr); // 32/64 BITS OK
//-----------------------------------------------
// int expression math:
//-----------------------------------------------
#define emit_mul_int  asm_imul_eax_esp
#define emit_div_int  asm_idivl_eax_esp
#define emit_add_int  asm_add_eax_esp
#define emit_sub_int  asm_sub_eax_esp
LIBIMPORT void    asm_imul_eax_esp  (ASM *a);
LIBIMPORT void    asm_idivl_eax_esp (ASM *a);
LIBIMPORT void    asm_add_eax_esp   (ASM *a);
LIBIMPORT void    asm_sub_eax_esp   (ASM *a);
//-------------------------------------------
#define emit_push_float asm_float_flds_value
#define emit_add_float  asm_float_faddp
#define emit_mul_float  asm_float_fmulp
LIBIMPORT void asm_float_flds_value (ASM *a, float value);
LIBIMPORT void asm_float_flds (ASM *a, void *var);
LIBIMPORT void asm_float_fstps (ASM *a, void *var);
LIBIMPORT void asm_float_fmulp (ASM *a);
LIBIMPORT void asm_float_faddp (ASM *a);
//-------------------------------------------

LIBIMPORT void    g                 (ASM *a, UCHAR c);
LIBIMPORT void    g2                (ASM *a, UCHAR c1, UCHAR c2);
LIBIMPORT void    g3                (ASM *a, UCHAR c1, UCHAR c2, UCHAR c3);
LIBIMPORT void    g4                (ASM *a, UCHAR c1, UCHAR c2, UCHAR c3, UCHAR c4);
//
LIBIMPORT void    emit_movl_ESP     (ASM *a, long value, UCHAR index); // movl    $0x5dc,0x4(%esp)
LIBIMPORT void    emit_mov_eax_ESP  (ASM *a, UCHAR index); // mov    %eax,0x4(%esp)
LIBIMPORT void    emit_mov_var_reg  (ASM *a, void *var, int reg); // move: variable to %register
LIBIMPORT void    emit_mov_reg_var  (ASM *a, int reg, void *var); // 32/64 BITS OK - move: %register to variable
LIBIMPORT void    emit_cmp_eax_edx  (ASM *a);
LIBIMPORT void    emit_cmp_eax_var  (ASM *a, void *var);
LIBIMPORT void    emit_pop_edx      (ASM *a);
LIBIMPORT void    emit_pop_var      (ASM *a, void *var);  // 32/64 BITS OK
LIBIMPORT void    emit_push_var     (ASM *a, void *var);  // 32/64 BITS OK
LIBIMPORT void    emit_incl         (ASM *a, void *var);
LIBIMPORT void    emit_decl         (ASM *a, void *var);  //: 32/64 BITS OK
LIBIMPORT void    emit_sub_esp      (ASM *a, char c);     // 32/64 BITS OK
LIBIMPORT void    asm_mov_value_eax (ASM *a, long value);
//
// 64 BITS: Function argument:
//
LIBIMPORT void    emit_mov_eax_edi  (ASM *a); // 64 BITS: Function argument 1: %edi
LIBIMPORT void    emit_mov_eax_esi  (ASM *a); // 64 BITS: Function argument 2: %esi
LIBIMPORT void    emit_mov_eax_edx  (ASM *a); // 64 BITS: Function argument 3: %edx
LIBIMPORT void    emit_mov_eax_ecx  (ASM *a); // 64 BITS: Function argument 4: %ecx
LIBIMPORT void    emit_mov_eax_r8d  (ASM *a); // 64 BITS: Function argument 5: %r8d
LIBIMPORT void    emit_mov_eax_r9d  (ASM *a); // 64 BITS: Function argument 6: %r9d
#ifdef USE_ASM
LIBIMPORT void write_asm (char *format, ...);
#endif
//
#endif // #ifdef USE_JIT
#ifdef USE_VM
LIBIMPORT void    emit_mul_int      (ASM *a);
LIBIMPORT void    emit_div_int      (ASM *a);
LIBIMPORT void    emit_add_int      (ASM *a);
LIBIMPORT void    emit_sub_int      (ASM *a);
LIBIMPORT void    emit_add_float    (ASM *a);
LIBIMPORT void    emit_mul_float    (ASM *a);
//
LIBIMPORT void    emit_call_vm      (ASM *a, void *func, UCHAR arg_count, UCHAR return_type);
LIBIMPORT void    emit_cmp_int      (ASM *a);
LIBIMPORT void    emit_mov_eax_var  (ASM *a, UCHAR index);
LIBIMPORT void    emit_print_eax    (ASM *a, UCHAR type);
LIBIMPORT void    emit_pop_var      (ASM *a, UCHAR i);
LIBIMPORT void    emit_push_var     (ASM *a, UCHAR i);
LIBIMPORT void    emit_push_string  (ASM *a, char *s);
LIBIMPORT void    emit_inc_var_int  (ASM *a, UCHAR index);
LIBIMPORT void    emit_dec_var_int  (ASM *a, UCHAR index);
//
#endif // #ifdef USE_VM
//-------------------------------------------------------------------
//
LIBIMPORT ASM   * asm_new           (unsigned int size);
LIBIMPORT void    asm_reset         (ASM *a);
LIBIMPORT void    asm_begin         (ASM *a); // 32/64 BITS OK
LIBIMPORT void    asm_end           (ASM *a); // 32/64 BITS OK
LIBIMPORT int     asm_get_len       (ASM *a);
LIBIMPORT void    asm_label         (ASM *a, char *name);
//
LIBIMPORT void    emit_call         (ASM *a, void *func, UCHAR arg_count, UCHAR return_type);
//
LIBIMPORT void    emit_jump_jmp     (ASM *a, char *name);
LIBIMPORT void    emit_jump_je      (ASM *a, char *name);
LIBIMPORT void    emit_jump_jg      (ASM *a, char *name);
LIBIMPORT void    emit_jump_jl      (ASM *a, char *name);
LIBIMPORT void    emit_jump_jne     (ASM *a, char *name);
LIBIMPORT void    emit_jump_jle     (ASM *a, char *name);
LIBIMPORT void    emit_jump_jge     (ASM *a, char *name);

//
LIBIMPORT void    emit_push_int     (ASM *a, int value);
LIBIMPORT void    emit_push_float   (ASM *a, float value);
LIBIMPORT void    emit_push_eax     (ASM *a);
LIBIMPORT void    emit_pop_eax      (ASM *a);


#ifdef __cplusplus
}
#endif
#endif // ! _SUMMER_H_
