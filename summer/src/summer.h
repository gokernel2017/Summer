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

#include "asm.h"

//-------------------------------------------------------------------
//-------------------------  DEFINE / ENUM  -------------------------
//-------------------------------------------------------------------
//
#define TOKEN_SIZE    1024

//-------------------------------------------------------------------
//----------------------------  STRUCT  -----------------------------
//-------------------------------------------------------------------
//
typedef struct PARSE    PARSE;

struct PARSE {
    char  *text;
    char  token [TOKEN_SIZE];
    int   tok;
    int   i;    // index of ( text[] )
    ASM   *a;
};

//-------------------------------------------------------------------
//-----------------------  SUMMER PUBLIC API  -----------------------
//-------------------------------------------------------------------
//

//---------------------------
// core.c
//---------------------------
//
extern char   * coreFileOpen  (const char *FileName);

// create a new parse with ( parse->text = NULL ) ... need setup ( parse->text )
// argument size: set the ASM ( a->code ) buffer.
extern PARSE  * coreNewParse  (unsigned long size);

//---------------------------
// lex.c
//---------------------------
//
extern int      lex           (PARSE *parse);

#endif // ! _SUMMER_H_
