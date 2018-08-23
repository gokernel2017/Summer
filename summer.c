/*
  JavaScript Object:

var person = {
    firstName: "John",
    lastName : "Doe",
    id       : 5566,
    fullName : function() {
        return this.firstName + " " + this.lastName;
    }
};

name = person.fullName();


const myObj = {
    name: 'Freddy'
    greet: function() {
      console.log(`hello my name is ${this.name}`);
    }
  };
  myObj.greet();

*/

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
//   02: summer file.s
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
#include "src/summer.h"

int main (int argc, char **argv) {
    char  *text;
    ASM   *a;
    LEXER l;
    int i, is_string=0;

    if ((a = core_Init (ASM_DEFAULT_SIZE)) == NULL)
  return -1;

    for (i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-s") && argc > i+1) is_string = i+1;
    }
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
                Run (a);
            }
            else printf ("ERRO:\n%s\n", ErroGet());
            #endif
            #ifdef USE_VM
            Run (a);
            #endif
//            lib_info (1);

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
        printf (" SUMMER Language Version: %d.%d.%d\n\n", SUMMER_VERSION, SUMMER_VERSION_SUB, SUMMER_VERSION_PATCH);
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

//            if (*string==0) strcpy(string, "info(0);");

            if (Parse(&l, a, string, "stdin") == 0) {

                Run (a);

            }
            else printf ("\n%s\n", ErroGet());

        }//: for (;;)
    }

    printf ("\nExiting With Sucess:\n");

    return 0;
}
