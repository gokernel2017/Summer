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
#ifdef USE_JIT
TVar  Gvar[255];
#endif

static FUNC   *Gfunc = NULL;  // store the user functions
static ASM    *asm_function = NULL;
static ASM    *asm_include = NULL;

static int
    is_function = 0,
    is_recursive = 0,
    argument_count
    ;
static char
    func_name [100]
    ;

extern void g4 (ASM *a, UCHAR c1, UCHAR c2, UCHAR c3, UCHAR c4);

//-------------------------------------------------------------------
//######################  PRIVATE PROTOTYPES  #######################
//-------------------------------------------------------------------
//
static void word_int    (ASM *a);
static void print_i     (int i);
//
static void expression  (ASM *a);
static void expr0       (ASM *a);
static void expr1       (ASM *a);
static void expr2       (ASM *a);
static void expr3       (ASM *a);

ASM *core_Init (unsigned long size) {
    static int init = 0;
    ASM *a;

    if (!init) {
        init = 1;

// for debug !
#ifdef USE_JIT
    printf ("Core_Init JIT 32\n\n");
#endif
#ifdef USE_VM
    printf ("Core_Init VM(Virtual Machine)\n\n");
#endif
        if ((a            = asm_new (ASM_DEFAULT_SIZE))==NULL) return NULL;
        if ((asm_function = asm_new (ASM_DEFAULT_SIZE))==NULL) return NULL;
        if ((asm_include  = asm_new (ASM_DEFAULT_SIZE))==NULL) return NULL;

        #ifdef USE_JIT
        asm_set_executable (asm_include->code, ASM_DEFAULT_SIZE - 2);
        #endif

        CreateVarLong ("i", 0);

        if (erro)
      return NULL;

        return a;
    }
    return NULL;
}
static int stmt (ASM *a) {
    ASM *a_f;

    if (is_function) a_f = asm_function; else a_f = a;

    tok = lex();

    switch (tok) {
    case TOK_INT: word_int (a_f); return 1;
    case ';':
        return 1;
    case   0: return 0;
    default: expression (a_f); return 1;
    }
    return 1;
}
static void expression (ASM *a) {
    char buf[100];

    if (tok==TOK_NUMBER) {
        expr0(a);

        #ifdef USE_JIT
        //-----------------------------------
        // push argument 1:
        //-----------------------------------
        //
        asm_popl_var(a, &Gvar[0].value.l); // Copia o TOPO DA PILHA ( %ESP ) para a variavel
        // c7 44 24   04   dc 05 00 00 	  movl    $0x5dc,0x4(%esp)
        g4(a,0xc7,0x44,0x24,(char)0);
        *(long*)a->p = -1;
        a->p += sizeof(long);
        asm_call(a, print_i);
        #endif
        #ifdef USE_VM
        sprintf (buf, "EXPRESSION: ");
        vme_prints (a, strlen(buf), buf);
        vme_popvar (a, 0);
        vme_printvar (a, 0);
        vme_printc (a, 10); // new line
        #endif
  return;
    }

    if (tok==TOK_ID) {
        int i;
        char _token_[100], var_name[100];
        char *_str_ = str;   // save string
        int _tok_ = tok; // save tok

        strcpy (_token_, token); // save token

        if ((i=VarFind(token))!=-1) {

            tok=lex();

            // EXPRESSION: a * b + c * d;
            if (tok == '*' || tok == '/' ||  tok == '+' || tok == '-') {
                // restore
                str = _str_;
                tok = _tok_;
                strcpy (token, _token_);

                expr0(a);

                #ifdef USE_JIT
                //-----------------------------------
                // push argument 1:
                //-----------------------------------
                //
                asm_popl_var(a, &Gvar[0].value.l); // Copia o TOPO DA PILHA ( %ESP ) para a variavel
                // c7 44 24   04   dc 05 00 00 	  movl    $0x5dc,0x4(%esp)
                g4(a,0xc7,0x44,0x24,(char)0);
                *(long*)a->p = -1;
                a->p += sizeof(long);
                asm_call(a, print_i);
                #endif
                #ifdef USE_VM
                sprintf (buf, "EXPRESSION: ");
                vme_prints (a, strlen(buf), buf);
                vme_popvar (a, 0);
                vme_printvar (a, 0);
                vme_printc (a, 10); // new line
                #endif
            return;
            }

            // or a expressin:
            // a * b + c * d;
            //expr0(a);

            // display variable value:
            if (tok==';') {
            #ifdef USE_JIT
            //-----------------------------------
            // push argument 1:
            //-----------------------------------
            //
            // c7 44 24   04   dc 05 00 00 	  movl    $0x5dc,0x4(%esp)
            g4(a,0xc7,0x44,0x24,(char)0);
            *(long*)a->p = i;
            a->p += sizeof(long);
            asm_call(a, print_i);
            #endif
            #ifdef USE_VM
            sprintf (buf, "%s: ", Gvar[i].name);
            vme_prints (a, strlen(buf), buf);
            vme_printvar (a, i);
            vme_printc (a, 10); // new line
            #endif
            return;
            }


            if (tok==TOK_PLUS_PLUS) {
                #ifdef USE_JIT
                asm_incl (a, &Gvar[i].value.l);
                #endif
                #ifdef USE_VM
                vme_incvar (a, i);
                #endif
            return;
            }

            if (tok=='=') {
                tok=lex();

                // ...

                // or a expressin:
                // i = a * b + c * d;
                expr0(a);

                #ifdef USE_JIT
                asm_popl_var(a, &Gvar[i].value.l); // Copia o TOPO DA PILHA ( %ESP ) para a variavel
                #endif
                #ifdef USE_VM
                vme_popvar (a, i);
                #endif
            return;
            }

        } else { //: if ((i=VarFind(token))!=-1)
            sprintf (buf, "ERRO LINE(%d) - Ilegal identifier '%s'", line, _token_);
            asm_Erro (buf);
        }
    } else {
        sprintf (buf, "EXPR ERRO LINE(%d) - Ilegar Word (%s)\n", line, token);
        asm_Erro (buf);
    }
}
// +, - : soma
void expr0 (ASM *a) {
    int op;

    expr1(a);
    while ((op=tok) == '+' || op == '-') {
        tok=lex();
        expr1(a);
        switch (op) {
#ifdef USE_JIT
        case '+': asm_add_eax_esp(a); break;
        case '-': asm_sub_eax_esp(a); break;
#endif
#ifdef USE_VM
        case '+': vme_addl(a); break;
//        case '-': asm_sub_eax_esp(a); break;
#endif
        }
    }
}
// *, / : multiplica
void expr1 (ASM *a) {
    int op;

    expr2(a);
    while ((op=tok) == '*' || op == '/') {
        tok=lex();
        expr2(a);
        switch (op) {
#ifdef USE_JIT
        case '*': asm_imul_eax_esp(a);  break;
        case '/': asm_idivl_eax_esp(a); break;
#endif
#ifdef USE_VM
        case '*': vme_mull(a);  break;
//        case '/': asm_idivl_eax_esp(a); break;
#endif
        }
    }
}
// (
void expr2 (ASM *a) {
    if (tok=='(') {
        tok=lex();
        expr0(a);
        if (tok != ')') {
            printf ("ERRO )\n");
            asm_Erro (" ");
        }
        tok=lex();
    }
    else expr3(a); // atom:
}
// atom:
void expr3 (ASM *a) {
    if (tok==TOK_ID) {
        int i;
/*
        // push a argument function:
        //
        if (is_function && (i=ArgumentFind(token))!=-1) {
            int arg = 8 + (i * 4);

          if (argument[i].type[0] == 0)
                asm_push_argument(a,arg);

            tok=lex();
        }
        else 
*/
        if ((i=VarFind(token))!=-1) {
#ifdef USE_JIT
            asm_pushl_var (a, &Gvar[i].value.l);
#endif
#ifdef USE_VM
            vme_pushvar (a, i);
#endif
            tok=lex();
        } else {
            char buf[255];
            sprintf(buf, "%s: '%s'", "EXPRESSION VAR NOT FOUND:", token);
            asm_Erro (buf);
        }
    }
    else if (tok==TOK_NUMBER) {
#ifdef USE_JIT
        asm_push_number (a, atoi(token));
#endif
#ifdef USE_VM
        vme_pushl (a, atoi(token));
#endif
        tok=lex();
    }
    else asm_Erro("Expression");
}

int core_Parse (ASM *a, char *text) {
    str = text;
    line = 1;

    asm_ErroReset();
    asm_reset (a);

    #ifdef USE_JIT
    asm_begin (a);
    asm_sub_esp (a, 100); //: 100 / 4 ==: MAXIMO DE 25 PARAMETROS, VARIAVEL LOCAL
    #endif
    while (!erro && stmt(a)) {

        // ... todo ...

        // for testing only ...
        //
        //if (*token) printf ("TOKEN(%s)\n", token);
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

UCHAR *core_FuncFind (char *name) {
    FUNC  *func;

    func = Gfunc;
    while(func){
        if ((func->name[0]==name[0]) && !strcmp(func->name, name)) return func->code;
        func = func->next;
    }
    return NULL;
}


// int syntax: int i, a = 10, b = 20, c = 3, d = 5;
static void word_int (ASM *a) {
    int pos = -4;

    while ((tok=lex())) {
        if (tok==TOK_ID) {
            char name[255];
            int value = 0;

            strcpy (name, token); // save

            if ((tok=lex())=='=') {
                if ((tok=lex())==TOK_NUMBER)
                    value = atoi (token);
            }
            if (is_function) {
/*
                strcpy (local.name[local.count], name);
                local.value[local.count] = value;
//  c7 45   fc    39 30 00 00 	movl   $0x3039,0xfffffffc(%ebp)
//  c7 45   f8    dc 05 00 00 	movl   $0x5dc,0xfffffff8(%ebp)
                g3(a,0xc7,0x45,(char)pos);
                *(long*)(a->code+a->len) = value;
                a->len += sizeof(long);

                pos -= 4;
                local.count++;
*/
            }
            else CreateVarLong (name, value);
        }
        if (tok==';') break;
    }
    if (tok != ';')
        asm_Erro ("ERRO: The word(int) need the char(;) on the end\n");
}

#ifdef USE_JIT
void CreateVarLong (char *name, long value) {
    TVar *v = Gvar;
    int i = 0;
    while (v->name) {
        if (!strcmp(v->name, name))
      return;
        v++;
        i++;
    }
    if (i < 255) {
        v->name = strdup(name);
        v->type = TYPE_LONG;
        v->value.l = value;
        v->info = NULL;
    }
}
int VarFind (char *name) {
    TVar *v = Gvar;
    int i = 0;
    while(v->name) {
        if (!strcmp(v->name, name))
      return i;
        v++;
        i++;
    }
    return -1;
}
void print_i (int i) {
    if (i == -1) {
        printf ("EXPRESSION: %ld\n", Gvar[0].value.l);
    } else {
        printf ("%s: %ld\n", Gvar[i].name, Gvar[i].value.l);
    }
}

#endif

/*
void display_var (void) {
    TVar *v = Gvar;
    while (v->name) {
        printf ("Gvar(%s) = %ld\n", v->name, v->value.l);
        v++;
    }
}
*/
