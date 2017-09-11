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

extern void display_var (void);

int main (int argc, char *argv[]) {
    char *text;
    ASM *a;

    if ((a = core_Init(ASM_DEFAULT_SIZE)) == NULL)
  return -1;

    if (argc >= 2 && (text = core_FileOpen(argv[1])) != NULL) {

        if (!core_Parse (a, text)) {

            #ifdef USE_JIT
            if (asm_set_executable (a->code, ASM_LEN(a)) == 0) {

                ( (void(*)()) a->code ) (); //<<<<<<<  execute JIT here  >>>>>>>

            }
            else printf ("\n%s\n", asm_ErroGet());
            #endif

            #ifdef USE_VM
            vm_run (a);
            #endif
        }
        else printf ("\n%s\n", asm_ErroGet());

        free (text);

    } else {
        char string [1024];

        #ifdef USE_JIT
        if (asm_set_executable (a->code, ASM_DEFAULT_SIZE - 2) != 0) {
            printf ("\n%s\n", asm_ErroGet());
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

            if (*string==0) strcpy(string, "info(0);");

            if (!core_Parse (a, string)) {

                #ifdef USE_JIT
                ( (void(*)()) a->code ) (); // <<<<<<< execute here >>>>>>>
                #endif

                #ifdef USE_VM
                vm_run (a);
                #endif
            }
            else printf ("\n%s\n", asm_ErroGet());
        }
    }

    printf ("\nExiting With Sucess !\n");

    return 0;
}
