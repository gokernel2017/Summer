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

#define PROG_SIZE     70000
#define STRING_SIZE   1024

LEXER l;
ASM *a;
char string [STRING_SIZE + 1];

#ifdef USE_APPLICATION
OBJECT *console = NULL;

void LOG (char *format, ...) {
    if (console) {
        char msg[1024] = { 0 };
        va_list ap;

        va_start (ap,format);
        vsprintf (msg, format, ap);
        va_end (ap);
        app_ConsoleAdd(console,msg,COLOR_ORANGE);
//        app_ConsoleTOP (console); // set to display the top
        app_SetState (RET_REDRAW); // force redraw ... on next event
    }
}

void call_console (int msg) {
    if (msg == MSG_KEY) {
        char *text = app_ConsoleGetText (console);
        sprintf (string, "%s", text+1);
        if (core_Parse(&l, a, string, "ERRO") == 0) {
            asm_Run (a); //<<<<<<<<<<  execute the JIT here  >>>>>>>>>>
        } else {
            if (iten_color) iten_color->color = COLOR_ERRO;
            app_ConsoleAdd(console, ErroGet(), COLOR_WHITE);
//printf ("%s\n", ErroGet());
        }
        app_ConsoleSetText(console, "$");
    }
}
#endif

ASM * GetAsmMain (void) {
    return a;
}

int main (int argc, char *argv[]) {
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
            }
            else printf ("ERRO:\n%s", ErroGet());
        }
        else printf ("ERRO:\n%s", ErroGet());

    } else {
    //-------------------------------------------
    // INTERACTIVE MODE:
    //-------------------------------------------

        if (asm_SetExecutable_ASM(a, ASM_DEFAULT_SIZE - 2) != 0) {
            printf ("\n%s\n", ErroGet());
            goto label_end;
        }

#ifdef USE_APPLICATION
        if (app_Init(argc,argv)) {
            if ((console = app_NewConsole(NULL, 0, 50, screen->h-310, "$info(2);")) != NULL) {
                app_SetSize (console, screen->w-100, 0);
                app_SetCall (console, call_console);
                app_SetFocus (console);
                app_Run(NULL);
            }
        }
#else

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
#endif
    }

    label_end:
    ASM_FREE (a);
    ASM_FREE (a);
    core_Finalize ();
    printf ("\nExiting With Sucess:\n");
    return 0;
}

