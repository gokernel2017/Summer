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

                printf ("\n<<<<<<<  Parse OK  >>>>>>>\n");
            }
            else printf ("\n%s\n", asm_ErroGet());
            #endif

            #ifdef USE_VM
            vm_run (a);
            #endif

            display_var();

        }
        else printf ("\n%s\n", asm_ErroGet());

        free (text);

        // free parse ...

        printf ("\nExiting With Sucess !\n");

    } else {
        printf ("\nUSAGE: %s <file.s>\n", argv[0]);
    }
    return 0;
}
