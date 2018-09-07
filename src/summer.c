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
// The Main Program.
//
// FILE:
//   summer.c
//
// USAGE:
// ----------------------------------------------
//
//   01: summer
//
//   02: summer file.sum
//
//   03: summer -s "int a=100, b=200; a = b; a;"
//
//   04: summer -s "10 * 20 + 3 * 5"
//
// ----------------------------------------------
//
// START DATE:
//   27/08/2017 - 08:35
//
// BY:  Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "summer.h"

LIBIMPORT int asm_set_executable (ASM *a, unsigned int size);

int main (int argc, char **argv) {
    char  *text;
    ASM   *a;
    LEXER l;
    int i, is_string = 0;

    for (i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-s") && argc > i+1) is_string = i+1;
        if (!strcmp(argv[i], "-asm")) asm_mode = 1;
        if (!strcmp(argv[i], "-h")) {
            printf ("Summer Language Help:\n");
            printf ("OPTIONS:\n");
            printf ("  -h   : Display this help.\n");
            printf ("  -s   : Run a 'string' program.\n");
            printf ("  -asm : JIT MODE - Compiler the code to Assembly ( AT&T Syntax ) x86 32 bits.\n");
      return 0;
        }
    }

    if ((a = core_Init (ASM_DEFAULT_SIZE)) == NULL)
  return -1;

    // execute a string and exit:
    //
    // summer -s "int a=100, b=200; a=b; a;"
    //
    if (is_string) {
        if (Parse(&l, a, argv[is_string], "string") == 0) {
            #ifdef USE_JIT
            if (asm_set_executable(a, asm_get_len(a)) == 0) {
                Run (a);
            }
            else printf ("ERRO:\n%s\n", ErroGet());
            #endif
            #ifdef USE_VM
            Run (a);
            #endif
        }
        else printf ("ERRO:\n%s\n", ErroGet());
        printf ("\nExiting With Sucess(STRING):\n");
  return 0;
    }

    if (argc >= 2 && (text = FileOpen (argv[1])) != NULL) {

        if (Parse(&l, a, text, argv[1]) == 0) {

            #ifdef USE_JIT
            if (asm_set_executable(a, asm_get_len(a)) == 0) {
                #ifdef USE_ASM
                if (asm_mode==0)
                #endif
                    Run (a);
            }
            else printf ("ERRO:\n%s\n", ErroGet());
            #endif
            #ifdef USE_VM
            Run (a);
            #endif
        }
        else printf ("ERRO:\n%s\n", ErroGet());

        free (text);
        core_Finalize ();

    } else {

//-------------------------------------------
// INTERACTIVE MODE:
//-------------------------------------------

        char string [1024];

        #ifdef USE_JIT
        if (asm_set_executable(a, ASM_DEFAULT_SIZE - 2) != 0) {
            printf ("\n%s\n", ErroGet());
      return -1;
        }
        #endif

        printf ("__________________________________________________________________\n\n");
        printf (" SUMMER Language Version: %d.%d.%d - '%s'\n\n", SUMMER_VERSION, SUMMER_VERSION_SUB, SUMMER_VERSION_PATCH, SUMMER_VERSION_NAME);
        printf (" To exit type: 'quit' or 'q'\n");
        printf ("__________________________________________________________________\n\n");

        for (;;) {
            printf ("SUMMER > ");
            gets (string);
            if (!strcmp(string, "quit") || !strcmp(string, "q")) break;

            if (!strcmp(string, "clear") || !strcmp(string, "cls")) {
                #ifdef WIN32
                system("cls");
                #endif
                #ifdef __linux__
                system("clear");
                #endif
                continue;
            }

            if (*string==0) strcpy(string, "info(0);");

            if (Parse(&l, a, string, "stdin") == 0) {

                Run (a);

            }
            else printf ("\n%s\n", ErroGet());

        }//: for (;;)
    }

    printf ("Exiting With Sucess:\n");

    return 0;
}
