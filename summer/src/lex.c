//-------------------------------------------------------------------
//
// SUMMER LANGUAGE:
//
//   Lexical Analizer.
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
//   lex.c
//
// START DATE:
//   27/08/2017 - 08:35
//
// BY:  Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "summer.h"

int lex (PARSE *parse) {
    int c;
//    char *p;

    if (parse->text[parse->i]==0) return 0;

    c = parse->text[parse->i++];

    return c;
}
