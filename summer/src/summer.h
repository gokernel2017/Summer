//-------------------------------------------------------------------
//
// SUMMER LANGUAGE:
//
//   Main Header.
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
//   summer.h
//
// START DATE:
//   27/08/2017 - 08:35
//
// BY:  Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#ifndef _SUMMER_H_
#define _SUMMER_H_

// 2 tags defined:
#if (defined(USE_JIT) && defined(USE_VM))
    #error "ERRO: Please define only one( USE_JIT or USE_VM )."
#endif

// no tag defined:
#if (!defined(USE_JIT) && !defined(USE_VM))
    #error "ERRO: TAG NOT DEFINED - Please define( USE_JIT or USE_VM )."
#endif

#if defined(USE_JIT)
    #include "asm.h"
    #include "def.h"
#endif

#if defined(USE_VM)
    #include "vm.h"
#endif

//-------------------------------------------------------------------
//-------------------------  DEFINE / ENUM  -------------------------
//-------------------------------------------------------------------
//
#define SUMMER_VERSION          0
#define SUMMER_VERSION_SUB      1
#define SUMMER_VERSION_PATCH    0

#define TOKEN_SIZE    1024

enum {
    TOK_INT = 255,
    TOK_FLOAT,
    TOK_IF,
    TOK_FUNCTION,

    TOK_IFDEF,        // #ifdef
    TOK_ENDIF,        // #endif

    TOK_ID,
    TOK_NUMBER,
    TOK_STRING,
    //
    TOK_PLUS_PLUS,    // ++
    TOK_MINUS_MINUS,  // --
    TOK_EQUAL_EQUAL,  // ==
    TOK_NOT_EQUAL,    // !=
    TOK_AND_AND,      // &&
    TOK_PTR           // ->
};

enum { FUNC_TYPE_NATIVE_C = 0, FUNC_TYPE_COMPILED, FUNC_TYPE_VM };

//-------------------------------------------------------------------
//----------------------------  STRUCT  -----------------------------
//-------------------------------------------------------------------
//
typedef struct FUNC         FUNC;
typedef struct FUNC_INFO    FUNC_INFO;
typedef struct ARG          ARG;

struct FUNC {
    char  *name;
    char  *proto; // prototype
    UCHAR *code;  // the function on JIT MODE | or VM in VM MODE
    int   type;   // FUNC_TYPE_NATIVE_C = 0, FUNC_TYPE_COMPILED, FUNC_TYPE_VM
    int   len;
    struct FUNC  *next;
};
struct ARG {
    char  type[20]; // "int", "float", "data_struct"
    char  name[20];
};


//-------------------------------------------------------------------
//------------------------  GLOBAL VARIABLE  ------------------------
//-------------------------------------------------------------------
//

// lex.c
extern char   *str;
extern char   token [TOKEN_SIZE];
extern int    tok;
extern int    line;

//-------------------------------------------------------------------
//-----------------------  SUMMER PUBLIC API  -----------------------
//-------------------------------------------------------------------
//

//---------------------------
// core.c
//---------------------------
//
extern ASM    * core_Init         (unsigned long size);

extern int      core_Parse        (ASM *a, char *text);

extern char   * core_FileOpen     (const char *FileName);

extern int      VarFind           (char *name); // if not exist return -1

//extern UCHAR  * core_FuncFind     (char *name);

extern void     core_DefineAdd    (char *name, int value);

extern void     CreateVarLong     (char *name, long value);
extern void     CreateVarFloat    (char *name, float value);

//---------------------------
// lex.c
//---------------------------
//
extern int     lex                (void);

#endif // ! _SUMMER_H_
