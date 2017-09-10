//-------------------------------------------------------------------
//
// SUMMER LANGUAGE:
//
//   Lexical Analyzer.
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
#include <ctype.h>

char  *str;
char  token [TOKEN_SIZE];
int   tok;
int   line;

const UCHAR ctable [] = { // used in function: int lex(void);
  0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                     // 10
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                     // 20
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                     // 30
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                     // 40
  0, 0, 0, 0, 0, 0, 0, '0', '1', '2',               // 50
  '3', '4', '5', '6', '7', '8', '9', 0, 0, 0,       // 60
  0, 0, 0, 0, 'A', 'B', 'C', 'D', 'E', 'F',         // 70
  'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', // 80
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', // 90
  0, 0, 0, 0, '_', 0, 'a', 'b', 'c', 'd',           // 100
  'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', // 110
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', // 120
  'y', 'z', 0, 0, 0, 0, 0
};
const UCHAR table [] = { // used in function: int alex(void);
  0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                     // 10
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                     // 20
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                     // 30
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                     // 40
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                     // 50
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                     // 60
  0, 0, 0, 0, 'A', 'B', 'C', 'D', 'E', 'F',         // 70
  'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', // 80
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', // 90
  0, 0, 0, 0, '_', 0, 'a', 'b', 'c', 'd',           // 100
  'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', // 110
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', // 120
  'y', 'z', 0, 0, 0, 0, 0
};

int lex (void) {
    register char *p = token;
    register int c;

    *p = 0;

top:

    //##############  REMOVE SPACE  #############
    //
    if (*str=='\n' || *str==' ' || *str==9 || *str==13) {
        if (*str++=='\n') line++;
        goto top;
    }

    if ((c = *str)==0) return 0;

    //################  STRING  #################
    //
    if (c == '"') {
        str++; // '"'
        while (*str && *str != '"' && *str != '\r' && *str != '\n')
            *p++ = *str++;
        *p = 0;
        if (*str=='"') str++; else asm_Erro("String erro");

        return TOK_STRING;
    }

    //##########  WORD, IDENTIFIER ...  #########
    //
//    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
    if ( table [ c ] ) {

        //-------------------------------------------------
        // Thanks  to: Fabrice Bellard - www.bellard.org
        // Code Based: TCC_VERSION "0.9.23"
        //   Function: static inline void next_nomacro1 (void);
        //
        // CODE: if (!isidnum_table[c]) break;
        //
        //-------------------------------------------------
        //
        while (ctable[ (UCHAR)(*str) ]) {
            *p++ = *str++;
        }
        *p = 0;

        if (!strcmp(token, "int"))      return TOK_INT;
        if (!strcmp(token, "float"))    return TOK_FLOAT;
        if (!strcmp(token, "function")) return TOK_FUNCTION;

        return TOK_ID;
    }

    //#################  NUMBER  ################
    //
    if (c >= '0' && c <= '9') {
        while ((*str >= '0' && *str <= '9') || *str == '.')
            *p++ = *str++;
        *p = 0;

        return TOK_NUMBER;
    }

    //########  C suported character ...  #######
    //
    switch (c) {
    case '(': case ')': case '{': case '}': case '<': case '>': case '[': case ']':
    case '*': case '/': case '+': case '-':
    case '=':
    case ',':
    case 39 : // '
    case ';':
    case '&':
    case '!':
    case '#':
    case '.':
    case ':':
    case '\\':
    case '|':

        //##########  REMOVE COMMENTS  ##########
        if (*str == '/'){
            if (str[1] == '*') { // comment block
                str += 2;
                do {
                    while (*str && *str != '*') {
                        if (*str == '\n') line++; //<<<<<<<<<<  line++  >>>>>>>>>>
                        str++;
                    }
                    str++;
                } while (*str && *str != '/');
                if (*str=='/') str++;
                else    asm_Erro ("BLOCK COMMENT ERRO: '/'");
                goto top;
            } else if (str[1] == '/') { // comment line
                str += 2;
                while ((*str) && (*str != '\n') && (*str != '\r'))
                    str++;
                goto top;
            }
        }

        *p++ = c;
        *p = 0;

        if (c=='=' && str[1]=='=') { *p++ = str[1]; *p=0; str += 2; return TOK_EQUAL_EQUAL; }
        if (c=='!' && str[1]=='=') { *p++ = str[1]; *p=0; str += 2; return TOK_NOT_EQUAL; }
        if (c=='&' && str[1]=='&') { *p++ = str[1]; *p=0; str += 2; return TOK_AND_AND; }
        if (c=='+' && str[1]=='+') { *p++ = str[1]; *p=0; str += 2; return TOK_PLUS_PLUS; }

        if (c=='-') {

            if (str[1]=='-') { *p++ = str[1]; *p=0; str += 2; return TOK_MINUS_MINUS; }
            if (str[1]=='>') { *p++ = str[1]; *p=0; str += 2; return TOK_PTR; }

            if (isdigit(str[1])) { // number: -100
                str++;
                while ((*str >= '0' && *str <= '9') || *str == '.')
                    *p++ = *str++;
                *p = 0;

                return TOK_NUMBER;
            }
        }

        return *str++;

//    default:
//        printf ("ERRO LINE(%d): Ilegal char: (%c) %d\n", line, *str, *str);
    default:
        {
        char buf[100];
        sprintf (buf, "LEX ERRO LINE(%d): Ilegal char: (%c) %d\n", line, *str, *str);
        asm_Erro (buf);
        }
    }//: switch (c) {
    return 0;
}
#ifdef TEST_LEX_MAIN
int main (int argc, char *argv[]) {

    return 0;
}
#endif
