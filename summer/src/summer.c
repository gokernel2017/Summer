//-------------------------------------------------------------------
//
// SUMMER LANGUAGE:
//
//   The Main Compiler.
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
//   summer.c
//
// START DATE:
//   27/08/2017 - 08:35
//
// BY:  Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "summer.h"

int main (int argc, char *argv[]) {
    char *str;
    PARSE *parse;

    if (argc >= 2 && (str = coreFileOpen(argv[1])) != NULL) {

        if ((parse = coreNewParse (ASM_DEFAULT_SIZE)) != NULL) {
            int i;

            parse->text = str;

            while ((i = lex(parse))) { // testint only ...
                printf ("%c", i);
            }
        }
        free (str);

        // free parse ...

        printf ("Exiting With Sucess !\n");

    } else {
        printf ("\nUSAGE: %s <file.s>\n", argv[0]);
    }
    return 0;
}
