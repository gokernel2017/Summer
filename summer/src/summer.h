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
#endif

#if defined(USE_VM)
    #include "vm.h"
#endif


//-------------------------------------------------------------------
//-------------------------  DEFINE / ENUM  -------------------------
//-------------------------------------------------------------------
//
#define TOKEN_SIZE    1024

enum {
    TOK_INT = 255,
    TOK_FLOAT,
    TOK_FUNCTION,

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
enum { // variable type:
    TYPE_LONG = 1,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_POINTER,
    TYPE_STRUCT,
    TYPE_PSTRUCT  // struct data *p;
};

//-------------------------------------------------------------------
//----------------------------  STRUCT  -----------------------------
//-------------------------------------------------------------------
//
typedef struct FUNC     FUNC;
#ifdef USE_JIT
typedef union  TValue   TValue;
typedef struct TVar     TVar;
#endif

struct FUNC {
    char  *name;
    char  *proto; // prototype
    int   type;   // FUNC_TYPE_COMPILED, FUNC_TYPE_NATIVE
    int   len;
    UCHAR *code;
    struct FUNC  *next;
};
#ifdef USE_JIT
union TValue {
    long    l;  //: type long integer
    float   f;  //: type float
    char    *s; //: type pointer of char
    void    *p; //: type pointer
};
struct TVar {
    char    *name;
    int     type;
    TValue  value;
    void    *info;  // any information ... struct type use this
};
#endif

//-------------------------------------------------------------------
//------------------------  GLOBAL VARIABLE  ------------------------
//-------------------------------------------------------------------
//
extern TVar   Gvar[255];

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

extern UCHAR  * core_FuncFind     (char *name);

//---------------------------
// vm.c
//---------------------------
//
extern void     CreateVarLong     (char *name, long value);

//---------------------------
// lex.c
//---------------------------
//
extern int     lex                (void);

#endif // ! _SUMMER_H_
