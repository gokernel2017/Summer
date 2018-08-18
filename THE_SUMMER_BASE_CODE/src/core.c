//-------------------------------------------------------------------
//
// THIS FILE IS PART OF SUMMER LANGUAGE:
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
// Main Core Language:
//
// FILE:
//   core.c
//
// SUMMER LANGUAGE START DATE ( 27/08/2017 - 08:35 ):
//   rewrite: 20/07/2018 - 11:10
//
// BY:  Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "summer.h"
#include "config.h"

//-----------------------------------------------
//------------------  VARIABLES  ----------------
//-----------------------------------------------
//
TVar            Gvar [GVAR_SIZE]; // global:
int             disasm_mode;
//
static ASM    * asm_function;
static char     strErro [STR_ERRO_SIZE];
//
static int
    erro,
    main_variable_type,
    var_type
    ;


//-----------------------------------------------
//-----------------  PROTOTYPES  ----------------
//-----------------------------------------------
//
static void   word_int    (LEXER *l, ASM *a);
static void   word_disasm (LEXER *l, ASM *a);
static void   expression  (LEXER *l, ASM *a);
//--------------  Expression:  --------------
static int    expr0       (LEXER *l, ASM *a);
static void   expr1       (LEXER *l, ASM *a);
static void   expr2       (LEXER *l, ASM *a);
static void   expr3       (LEXER *l, ASM *a);
static void   atom        (LEXER *l, ASM *a);
//-------------------------------------------
static void   do_block    (LEXER *l, ASM *a);
static int    stmt        (LEXER *l, ASM *a);
static int    see         (LEXER *l);


ASM * core_Init (unsigned int size) {
    static int init = 0;
    ASM *a;
    if (!init) {
        init = 1;
        if ((a            = asm_new (size))==NULL) return NULL;
        if ((asm_function = asm_new (size))==NULL) return NULL;
        return a;
    }
    return NULL;
}

void core_Finalize (void) {

}

char * FileOpen (const char *FileName) {
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
void CreateVarInt (char *name, int value) {
    TVar *v = Gvar;
    int i = 0;
    while (v->name) {
        if (!strcmp(v->name, name))
      return;
        v++;
        i++;
    }
    if (i < GVAR_SIZE) {
        v->name = strdup(name);
        v->type = TYPE_INT;
        v->value.i = value;
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

static void word_int (LEXER *l, ASM *a) {
    while (lex(l)) {
        if (l->tok == TOK_ID) {
            char name[255];
            int value = 0;

            strcpy (name, l->token); // save

            if (lex(l) == '=') {
                if (lex(l) == TOK_NUMBER)
                    value = atoi (l->token);
            }
            CreateVarInt (name, value);

        }
        if (l->tok == ';') break;
    }
    if (l->tok != ';') Erro ("ERRO: The word(float) need the char(;) on the end\n");
}
static void word_disasm (LEXER *l, ASM *a) {
    lex(l);
    disasm_mode = atoi (l->token);
}

static void expression (LEXER *l, ASM *a) {
    if (l->tok==TOK_ID || l->tok==TOK_NUMBER) {

        main_variable_type = var_type = TYPE_INT; // 0

        // expression type:
        // 10 * 20 + 3 * 5;
        if (expr0(l,a) == -1) {
            #ifdef USE_JIT
                #if defined(__x86_64__)
                //-----------------------------------
                // JIT 64 bits

                if (main_variable_type == TYPE_INT) {
                    // argument 1:
                    //b8    00 20 40 00       	mov    $0x402000,%eax
                    g(a,0xb8); asm_get_addr(a,"%d\n");
                    g2(a,G2_MOV_EAX_EDI); // 89 c7   : mov   %eax,%edi

                    // argument 2:
                    emit_pop_eax (a); // %eax | eax.i := expression result
                    g2(a,G2_MOV_EAX_ESI); // 89 c6   : mov   %eax,%esi

                } else if (main_variable_type == TYPE_FLOAT) {

                }
                emit_call(a,printf,0,0);   // call the function
                #else
                //-----------------------------------
                // JIT 32 bits

                if (main_variable_type == TYPE_INT) {
                    emit_pop_eax (a); // %eax | eax.i := expression result
                    emit_movl_ESP (a, (long)("%d\n"), 0); // pass argument 1
                    emit_mov_eax_ESP (a, 4); // pass argument 2
                } else if (main_variable_type == TYPE_FLOAT) {

                }
                emit_call(a,printf,0,0);   // call the function
                #endif
            #endif //: #ifdef USE_JIT

            #ifdef USE_VM
            emit_pop_eax (a); // %eax | eax.i := expression result
            emit_print_eax (a,main_variable_type);
            #endif
        }//: if (expr0(l,a) == -1)

    } else {
        char buf[100];
        sprintf (buf, "EXPRESSION ERRO LINE(%d) - Ilegar Word (%s)\n", l->line, l->token);
        Erro (buf);
    }
}
static int expr0 (LEXER *l, ASM *a) {
    if (l->tok == TOK_ID) {
        int i;
        // i = a * b + c;
        if (see(l)=='=') {
            if ((i=VarFind(l->token)) != -1) {
                char token[255];
                sprintf (token, "%s", l->token);
                int pos = l->pos; // save
                int tok = l->tok;
                lex(l);
                if (l->tok == '=') {
                    lex(l);
                    expr1(l,a);
                    #ifdef USE_JIT
                    // Copia o TOPO DA PILHA ( %ESP ) para a variavel ... e estabiliza a PILHA
                    emit_pop_var (a, &Gvar[i].value.i);
                    #endif
                    #ifdef USE_VM
                    // Copia o TOPO DA PILHA ( sp ) para a variavel ... e decrementa sp++.
                    emit_pop_var (a,i);
                    #endif
                    return i;
                } else {
                    sprintf (l->token, "%s", token);
                    l->pos = pos; // restore
                    l->tok = tok;
                }
            }//: if ((i=VarFind(l->token)) != -1)
        }//: if (see(l)=='=')
    }
    expr1(l,a);
    return -1;
}
static void expr1 (LEXER *l, ASM *a) { // '+' '-' : ADDITION | SUBTRACTION
    int op;
    expr2(l,a);
    while ((op=l->tok) == '+' || op == '-') {
        lex(l);
        expr2(l,a);
        if (main_variable_type==TYPE_FLOAT) {
//            if (op=='+') emit_add_float (a);
        } else { // INT
            if (op=='+') emit_add_int (a);
        }

    }
}
static void expr2 (LEXER *l, ASM *a) { // '*' '/' : MULTIPLICATION | DIVISION
    int op;
    expr3(l,a);
    while ((op=l->tok) == '*' || op == '/') {
        lex(l);
        expr3(l,a);
        if (main_variable_type==TYPE_FLOAT) {
//            if (op=='*') emit_mul_float (a);
        } else { // INT
            if (op=='*') emit_mul_int (a);
        }
    }
}
static void expr3 (LEXER *l, ASM *a) { // '('
    if (l->tok=='(') {
        lex(l); expr0(l,a);
        if (l->tok != ')') {
            Erro ("ERRO )\n");
        }
        lex(l);
    }
    else atom(l,a); // atom:
}
static void atom (LEXER *l, ASM *a) { // expres
    if (l->tok==TOK_ID) {
        int i;

        if ((i=VarFind(l->token))!=-1) {
            var_type = Gvar[i].type;

            #ifdef USE_JIT
            emit_push_var (a, &Gvar[i].value.i);
            #endif
            #ifdef USE_VM
            emit_push_var (a,i);
            #endif

            lex(l);

        } else {
            char buf[255];
            sprintf(buf, "ERRO LINE(%d ATOM()): Var Not Found '%s'", l->line, l->token);
            Erro (buf);
        }
    }
    else if (l->tok==TOK_NUMBER) {
        if (strchr(l->token, '.'))
            var_type = TYPE_FLOAT;

        if (var_type==TYPE_FLOAT) {
//            emit_push_float (a, atof(l->token));
        } else {
            emit_push_int (a, atoi(l->token));
        }

        lex(l);
    }
    else { Erro("Expression"); printf ("LINE: %d token(%s)\n", l->line, l->token); }
}//: atom ()


void do_block (LEXER *l, ASM *a) {

    while (!erro && l->tok && l->tok != '}') {
        stmt (l,a);
    }
    l->tok = ';';
}

static int stmt (LEXER *l, ASM *a) {

    lex (l);

    switch (l->tok) {
    case '{':
        //----------------------------------------------------
        do_block (l,a); //<<<<<<<<<<  no recursive  >>>>>>>>>>
        //----------------------------------------------------
        return 1;
    case TOK_INT:       word_int      (l,a); return 1;
    case TOK_DISASM:    word_disasm   (l,a); return 1;
    default:            expression    (l,a); return 1;
    case ';':
    case '}':
        return 1;
    case 0: return 0;
    }
    return 1;
}
static int see (LEXER *l) {
    char *s = l->text+l->pos;
    while (*s) {
        if (*s=='\n' || *s==' ' || *s==9 || *s==13) {
            s++;
        } else {
            if (s[0]=='=' && s[1]=='=') return TOK_EQUAL_EQUAL;
            if (s[0]=='+' && s[1]=='+') return TOK_PLUS_PLUS;
            return *s;
        }
    }
    return 0;
}

int Parse (LEXER *l, ASM *a, char *text, char *name) {
    lex_set (l, text, name);
    ErroReset ();
    asm_reset (a);
    asm_begin (a);
    while (!erro && stmt(l,a)) {
        // ... compiling ...
    }
    asm_end (a);
    return erro;
}

void lib_info (int arg) {
    switch (arg) {
    case 1: {
        TVar *v = Gvar;
        int i = 0;
        printf ("VARIABLES:\n---------------\n");
        while (v->name) {
            if (v->type==TYPE_INT)   printf ("Gvar[%d](%s) = %d\n", i, v->name, v->value.i);
            else
            if (v->type==TYPE_FLOAT) printf ("Gvar[%d](%s) = %f\n", i, v->name, v->value.f);
            v++; i++;
        }
        } break;

    default:
        printf ("USAGE(%d): info(1);\n\nInfo Options:\n 1: Variables\n 2: Functions\n 3: Defines\n 4: Words\n",arg);
    }
}

void Erro (char *s) {
    erro++;
    if ((strlen(strErro) + strlen(s)) < STR_ERRO_SIZE)
        strcat (strErro, s);
}

char *ErroGet (void) {
    if (strErro[0])
        return strErro;
    else
        return NULL;
}

void ErroReset (void) {
    erro = 0;
    strErro[0] = 0;
}
