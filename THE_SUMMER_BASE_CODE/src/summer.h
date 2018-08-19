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
//
#ifndef _SUMMER_H
#define _SUMMER_H

#include "lex.h"

#if defined(USE_JIT)
    #include "asm.h"
#endif
#if defined(USE_VM)
    #include "vm.h"
#endif

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
#define SUMMER_VERSION_SUB    1
#define SUMMER_VERSION_PATCH  0

#define STR_ERRO_SIZE         1024
#define GVAR_SIZE             255
#define TYPE_NO_RETURN        100

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

//-----------------------------------------------
//------------------  STRUCT  -------------------
//-----------------------------------------------
//
typedef union  VALUE      VALUE;
typedef struct TVar       TVar;
typedef struct TFunc      TFunc;
typedef struct F_STRING   F_STRING;

union VALUE {
    int     i;  //: type integer
    float   f;  //: type float
    char    *s; //: type pointer of char
    void    *p; //: type pointer
};
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


// global:
LIBIMPORT TVar  Gvar [GVAR_SIZE];

//-------------------------------------------------------------------
//---------------------------  PUBLIC API  --------------------------
//-------------------------------------------------------------------
//
LIBIMPORT void    Run                 (ASM *a); // back-end in file: asm.c | vm.c
//
// FILE: "core.c"
//
LIBIMPORT ASM   * core_Init       (unsigned int size);
LIBIMPORT void    core_Finalize   (void);
LIBIMPORT char  * FileOpen        (const char *FileName);
LIBIMPORT void    CreateVarInt    (char *name, int value);
LIBIMPORT TFunc * FuncFind        (char *name);
LIBIMPORT int     VarFind         (char *name);
LIBIMPORT int     Parse           (LEXER *l, ASM *a, char *text, char *name);
LIBIMPORT void    Erro            (char *s);
LIBIMPORT char  * ErroGet         (void);
LIBIMPORT void    ErroReset       (void);

#ifdef __cplusplus
}
#endif

#endif // ! _SUMMER_H
