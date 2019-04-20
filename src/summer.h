//-------------------------------------------------------------------
//
// SUMMER LANGUAGE:
//
// The Main Header:
//
// FILE:
//   summer.h
//
// SUMMER LANGUAGE START DATE ( 27/08/2017 - 08:35 ):
//   rewrite 01: 20/07/2018 - 11:10
//   rewrite 02: 23/03/2019 - 08:50
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#ifndef _SUMMER_H_
#define _SUMMER_H_

// INFO: Windows X64 BITS functions arguments:
// arg 0 = %ecx
// arg 1 = %edx
// arg 2 = %r8d
// arg 3 = %r9d
//
// INFO: Linux X64 BITS functions arguments:
// arg 0 = %edi
// arg 1 = %esi
// arg 2 = %edx
// arg 3 = %ecx
// arg 4 = %r8
// arg 5 = %r9

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------
//------------------  INCLUDE  ------------------
//-----------------------------------------------
//
#include "asm.h"
#include "lex.h"

#ifdef USE_APPLICATION
    // Application API
    #include "app/app.h"
#endif

#ifdef USE_SG
    // Summer Graphic ( GL / DirectX / HTML Canvas ):
    #include "sg/sg.h"
#endif

//-----------------------------------------------
//---------------  DEFINE / ENUM  ---------------
//-----------------------------------------------
//
#define SUMMER_VERSION            0
#define SUMMER_VERSION_SUB        70
#define SUMMER_VERSION_PATCH      0
#if defined(__x86_64__)
    #define SUMMER_VERSION_NAME   "BETA 64 BITS"
#else
    #define SUMMER_VERSION_NAME   "BETA 32 BITS"
#endif

#define GVAR_SIZE                 255

enum {
    TYPE_LONG = 0,
    TYPE_FLOAT,
    TYPE_FLOATP,  // float *fp;
    TYPE_STRING,
    TYPE_POINTER,
    TYPE_STRUCT,
    TYPE_PSTRUCT,  // struct data *p;
    TYPE_UNKNOW
};

enum {
    FUNC_TYPE_NATIVE_C = 0,
    FUNC_TYPE_COMPILED,
    FUNC_TYPE_VM,
    FUNC_TYPE_MODULE     // .dll | .so
};

//-----------------------------------------------
//-------------------  STRUCT  ------------------
//-----------------------------------------------
//
typedef union  VALUE    VALUE;
typedef struct TVar     TVar;
typedef struct TFunc    TFunc;
typedef struct TFstring	TFstring; // fixed string
typedef struct TDefine	TDefine;
typedef struct TArg     TArg;

union VALUE {
    long    l;  //: type integer
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
    int     sub_esp;  // used only in dynamic modules: .dll | .so
    TFunc   *next;
};
struct TFstring {
    char *s;
    int   i;
    TFstring *next;
}; // fixed string
struct TDefine {
    char    *name;
    int     value;
    TDefine *next;
};
struct TArg {
    // type[0] := '-' ... Is EVENT
    char  type[20]; //  "int", "float", "data_struct"
    char  name[20];
};

//-----------------------------------------------
//--------------  GLOBAL VARIABLE  --------------
//-----------------------------------------------
//
LIBIMPORT TVar Gvar [GVAR_SIZE]; // global:
LIBIMPORT int ifndef_true;

//-----------------------------------------------
//-----------------  PUBLIC API  ----------------
//-----------------------------------------------
//
LIBIMPORT ASM   * core_Init       (unsigned int size);
LIBIMPORT int     core_Parse      (LEXER *l, ASM *a, char *text, char *name);
LIBIMPORT void    core_Finalize   (void);
//
LIBIMPORT void    Disasm          (UCHAR *code, char *name, int len);
LIBIMPORT void    Assemble        (LEXER *l, ASM *a);
//
LIBIMPORT ASM   * GetAsmMain      (void); // in file: "summer.c"
//
LIBIMPORT void    CreateVarLong   (char *name, long l);
LIBIMPORT void    FuncAdd         (TFunc *func);
LIBIMPORT int     ArgumentFind    (char *name);
LIBIMPORT TFunc * FuncFind        (char *name);
LIBIMPORT int     VarFind         (char *name);
LIBIMPORT void    proc_ifdef      (char *name);
LIBIMPORT int     is_defined      (char *name);

LIBIMPORT TFstring *fs_new (char *s);

#ifdef USE_APPLICATION
LIBIMPORT void LOG (char *format, ...);
#endif

#ifdef __cplusplus
}
#endif
#endif // ! _SUMMER_H_

