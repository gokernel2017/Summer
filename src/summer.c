//-------------------------------------------------------------------
//
// SUMMER LANGUAGE:
//
// Main program:
//
// FILE:
//   summer.c
//
// SUMMER LANGUAGE START DATE ( 27/08/2017 - 08:35 ):
//   rewrite 01: 20/07/2018 - 11:10
//   rewrite 02: 23/03/2019 - 08:50
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "summer.h"

#define PROG_SIZE			70000
#define STRING_SIZE		1024

ASM *a;

ASM * GetAsmMain (void) {
		return a;
}

int main (int argc, char *argv[]) {
		LEXER l;
		FILE *fp;

		if ((a = core_Init(ASM_DEFAULT_SIZE)) == NULL)
	return -1;

		if (argc >= 2 && (fp = fopen (argv[1], "r")) != NULL) {
				char prog [PROG_SIZE + 1];
				int c, i = 0;
				while ((c = fgetc(fp)) != EOF) prog[i++] = c;
				prog[i] = 0;
				fclose (fp);
				if (i >= PROG_SIZE) {
						printf ("\nERRO: Big Text File('%s' := size: %d) > PROG_SIZE: %d\n\n", argv[1], i, PROG_SIZE);
						goto label_end;
				}

				if (core_Parse(&l, a, prog, argv[1]) == 0) {
						if (asm_SetExecutable_ASM(a, 0) == 0) {
								asm_Run (a); //<<<<<<<<<<  execute the JIT here  >>>>>>>>>>
//								Disasm(asm_GetCode(a), "main", asm_GetLen(a));
						}
						else printf ("ERRO:\n%s", ErroGet());
				}
				else printf ("ERRO:\n%s", ErroGet());

		} else {
		//-------------------------------------------
		// INTERACTIVE MODE:
		//-------------------------------------------

				char string [STRING_SIZE + 1];

        if (asm_SetExecutable_ASM(a, ASM_DEFAULT_SIZE - 2) != 0) {
            printf ("\n%s\n", ErroGet());
						goto label_end;
        }

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

            //if (*string==0) strcpy(string, "info(0);");

						if (core_Parse(&l, a, string, "stdin") == 0) {
								asm_Run (a); //<<<<<<<<<<  execute the JIT here  >>>>>>>>>>
						}
						else printf ("ERRO:\n%s", ErroGet());
				}
		}

		label_end:
		asm_Free (a);
		core_Finalize ();
		printf ("\nExiting With Sucess:\n");
		return 0;
}

