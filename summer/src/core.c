//-------------------------------------------------------------------
//
// SUMMER LANGUAGE:
//
//   Main Core Language.
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
//   core.c
//
// START DATE:
//   27/08/2017 - 08:35
//
// BY:  Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "summer.h"

#define STR_ERRO_SIZE   1024

// global:
TVar  *Gvar;

static FUNC   *Gfunc = NULL;  // store the user functions
static ASM    *asm_function = NULL;
static ASM    *asm_include = NULL;

static int
    is_function = 0,
    is_recursive = 0,
    argument_count,
    Gvar_len
    ;
static char
    func_name [100]
    ;

ASM *core_Init (unsigned long size) {
    static int init = 0;
    ASM *a;

    if (!init) {
        init = 1;

// for debug !
#ifdef USE_JIT
    printf ("Core_Init JIT 32\n");
#endif
#ifdef USE_VM
    printf ("Core_Init VM(Virtual Machine)\n");
#endif
        if ((a            = asm_new (ASM_DEFAULT_SIZE))==NULL) return NULL;
        if ((asm_function = asm_new (ASM_DEFAULT_SIZE))==NULL) return NULL;
        if ((asm_include  = asm_new (ASM_DEFAULT_SIZE))==NULL) return NULL;

        #ifdef USE_JIT
        asm_set_executable (asm_include->code, ASM_DEFAULT_SIZE - 2);
        #endif

        if (erro)
      return NULL;

        return a;
    }
    return NULL;
}
static int stmt (ASM *a) {

    tok = lex();

    switch (tok) {
    case 0: return 0;
    }
    return 1;
}
int core_Parse (ASM *a, char *text) {
    str = text;

#ifdef USE_JIT
    asm_reset (a);
    asm_begin (a);
    asm_sub_esp (a, 100); //: 100 / 4 ==: MAXIMO DE 25 PARAMETROS, VARIAVEL LOCAL
#endif
    while (!erro && stmt(a)) {

        // ... todo ...

        // for testing only ...
        //
        if (*token) printf ("TOKEN(%s)\n", token);
    }
    asm_end (a);

    return erro;
}

char * core_FileOpen (const char *FileName) {
    FILE *fp;

    if ((fp = fopen (FileName, "rb")) != NULL) {
        char *str;
        int size, i;

        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        str = (char *)malloc (size + 5);
        if(!str){
            fclose (fp);
            return NULL;
        }
        i = fread(str, 1, size, fp);
        fclose(fp);
        str[i] = 0;
        str[i+1] = 0;

        return str;
    }
    else printf ("File Not Found: '%s'\n", FileName);

    return NULL;
}

static void core_VarCreate (TVar *v) {
  Gvar = (TVar*) realloc (Gvar, (Gvar_len+1) * sizeof (TVar));
  Gvar[Gvar_len++] = *v;
}

void core_CreateVarInt (char *name, long value) {
    register int i;
    TVar v;

    for(i=0;i<Gvar_len;i++)
        if(!strcmp(Gvar[i].name, name)){
            if(Gvar[i].type == TYPE_LONG)
                Gvar[i].value.l = value;
            return;
        }

    v.name = strdup (name);
    v.type = TYPE_LONG;
    v.value.l = value;
    v.info = NULL;

    core_VarCreate(&v);
}
int core_VarFind (char *name) { // if not exist return -1
    register int i;
    for (i = 0; i < Gvar_len; i++) if (!strcmp(Gvar[i].name, name)) return i;
    return -1;
}

UCHAR *core_FuncFind (char *name) {
    FUNC  *func;

    func = Gfunc;
    while(func){
        if ((func->name[0]==name[0]) && !strcmp(func->name, name)) return func->code;
        func = func->next;
    }
    return NULL;
}

void display_var (void) {
    int i;
    for(i=0;i<Gvar_len;i++)
        printf ("Gvar[ %d ]( %s ) = %ld\n", i, Gvar[i].name, Gvar[i].value.l);
}
