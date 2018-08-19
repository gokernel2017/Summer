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
#include <stdarg.h>

//-----------------------------------------------
//-----------------  PROTOTYPES  ----------------
//-----------------------------------------------
//
static void   word_int    (LEXER *l, ASM *a);
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
F_STRING    * fs_new      (char *s);
// stdlib:
void  lib_info    (int arg);
int   lib_add     (int a, int b);
int   lib_arg     (int a, int b, int c, int d, int e);
void  lib_printf  (char *format, ...);

//-----------------------------------------------
//------------------  VARIABLES  ----------------
//-----------------------------------------------
//
TVar            Gvar [GVAR_SIZE]; // global:
//
static ASM    * asm_function;
static char     strErro [STR_ERRO_SIZE];
static F_STRING * fs = NULL;

//
static int
    erro,
    main_variable_type,
    var_type
    ;

static TFunc stdlib[]={
  //-----------------------------------------------------------------
  // char*        char*   UCHAR*/ASM*             int   int   FUNC*
  // name         proto   code                    type  len   next
  //-----------------------------------------------------------------
  { "info",       "0i",     (UCHAR*)lib_info,       0,    0,    NULL },
  { "add",        "iii",    (UCHAR*)lib_add,        0,    0,    NULL },
  { "arg",        "iiiiii", (UCHAR*)lib_arg,        0,    0,    NULL },
  { "printf",     "0s",     (UCHAR*)lib_printf,     0,    0,    NULL },
  { NULL,         NULL,     NULL,                   0,    0,    NULL }
};


//-----------------------------------------------
//------------------  FUNCTIONS  ----------------
//-----------------------------------------------
//
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
TFunc *FuncFind (char *name) {
    // array:
    TFunc *lib = stdlib;
    while (lib->name) {
        if ((lib->name[0]==name[0]) && !strcmp(lib->name, name))
      return lib;
        lib++;
    }
    return NULL;
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

//
// function_name (a, b, c + d);
//
void execute_call (LEXER *l, ASM *a, TFunc *func) {
    int count = 0;
    int pos = 0, size = 4; // Used in: JIT 32 bits
    int return_type = TYPE_INT;

    // no argument
    if (func->proto && func->proto[1] == '0') {
        while (lex(l))
            if (l->tok == ')' || l->tok == ';') break;
    } else {
        // get next: '('
        if (lex(l)!='(') { Erro ("Function need char: '('\n"); return; }

        while (lex(l)) {

            if (l->tok==TOK_STRING) {
                F_STRING *s = fs_new (l->token);
                if (s) {
                    #ifdef USE_VM
                    emit_push_string (a, s->s);
                    #endif

                    #ifdef USE_JIT
                    #if defined(__x86_64__)
                    //---------------------------
                    // JIT 64 bits
                    //
                    // bf    1c 07 40 00       	mov    $0x40071c,%edi
                    //
                    //-----------------------------------------------
                    // <<<<<<<   ONLY ARGUMENT 1 IMPLEMENTED   >>>>>>
                    //-----------------------------------------------
                    //
                    if (count==0) {
                        g(a,0xbf); asm_get_addr(a,s->s);
                    }
                    //
                    // <<<<<<<<<<<<<<<<<<<<<  Please Wait ... >>>>>>>
                    #else
                    //---------------------------
                    // JIT 32 bits
                    //
                    // movl   $0x403000, 0(%esp)
                    //
                    emit_movl_ESP (a, (long)(s->s), pos); // pass argument 1
                    pos += size;
                    #endif
                    #endif // #ifdef USE_JIT
                    if (count++ > 15) break;
                }
            }//: if (l->tok==TOK_STRING)
            else
            if (l->tok==TOK_ID || l->tok==TOK_NUMBER || l->tok=='(') {

                //
                // The result of expression is store in the "stack".
                //
                expr0 (l,a);

                #ifdef USE_JIT

                emit_pop_eax (a); // ... pop the "stack" and store in %eax ... := expression result

                #if defined(__x86_64__)
                //-----------------------------------
                // JIT 64 bits
                //
                // %edi, %esi, %edx, %ecx, %r8 and %r9
                //
                if (count==0) {     // argument 1:
                    g2(a,G2_MOV_EAX_EDI);
                }
                else if (count==1) { // argument 2
                    g2(a,G2_MOV_EAX_ESI);
                }
                else if (count==2) { // argument 3
                    g2(a,G2_MOV_EAX_EDX);
                }
                else if (count==3) { // argument 4
                    g2(a,G2_MOV_EAX_ECX);
                }
                else if (count==4) { // argument 5
                    g3(a,G3_MOV_EAX_r8d);
                }
                #else
                //-----------------------------------
                // JIT 32 bits
                emit_mov_eax_ESP (a, pos);
                pos += size;
                #endif
                #endif // ! USE_JIT

                if (count++ > 15) break;
            }
            if (l->tok == ')' || l->tok == ';') break;
        }
    }

    if (func->proto) {
        if (func->proto[0] == '0') return_type = TYPE_NO_RETURN;
        if (func->proto[0] == 'f') return_type = TYPE_FLOAT;
    }
    if (func->type == FUNC_TYPE_VM) {
        //
        // THE_SUMMER_BASE_CODE: <<<<<<<<<<  Not implemented  >>>>>>>>>>
        //
        // here: fi->code ==  ASM*
        //
        // emit_call_vm (a, (ASM*)(func->code), (UCHAR)count, return_type);
    } else {
        emit_call (a, (void*)func->code, (UCHAR)count, return_type);
    }
}

static void expression (LEXER *l, ASM *a) {
    if (l->tok==TOK_ID || l->tok==TOK_NUMBER) {
        TFunc *fi;
        int i;

        main_variable_type = var_type = TYPE_INT; // 0

        //
        // call a function without return:
        //   function_name (...);
        //
        if ((fi = FuncFind(l->token)) != NULL) {
            execute_call (l, a, fi);
      return;
        }

        if ((i = VarFind (l->token)) != -1) {

            main_variable_type = var_type = Gvar[i].type;

            if (see(l)=='=') {
                lex_save(l); // save the lexer position
                lex(l); // =
                //
                // call a function with return:
                //   i = function_name (...);
                //
                if (lex(l)==TOK_ID &&(fi = FuncFind(l->token)) != NULL) {

                    execute_call (l, a, fi);

                    #ifdef USE_VM
                    // The function return is stored in variable VALUE( eax ) ... see in file: vm.c
                    emit_mov_eax_var(a,i);
                    #endif

                    #ifdef USE_JIT
                    //
                    // The function return is stored in register %eax ... types ! TYPE_FLOAT
                    //
                    //   HERE: copy the register ( %eax ) to the variable
                    //
                    if (main_variable_type != TYPE_FLOAT) {
                        emit_mov_reg_var (a, EAX, &Gvar[i].value.i);
                    } else {

                    }
                    #endif //: #ifdef USE_JIT
              return;
                }
                lex_restore (l); // restore the lexer position

            }//: if (see(l)=='=')

        }//: if ((i = VarFind (l->token)) != -1)

        //---------------------------------------
        // Expression types:
        //   a * b + c * d;
        //   10 * a + 3 * b;
        //---------------------------------------
        //
        if (expr0(l,a) == -1) {
            #ifdef USE_VM
            emit_pop_eax (a); // %eax | eax.i := expression result
            emit_print_eax (a,main_variable_type);
            #endif

            #ifdef USE_JIT
            #if defined(__x86_64__)
            //
            // JIT 64 bits
            //
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
            //
            // JIT 32 bits
            //

            if (main_variable_type == TYPE_INT) {
                emit_pop_eax (a); // %eax | eax.i := expression result
                emit_movl_ESP (a, (long)("%d\n"), 0); // pass argument 1
                emit_mov_eax_ESP (a, 4);              // pass argument 2
            } else if (main_variable_type == TYPE_FLOAT) {

            }
            emit_call(a,printf,0,0);   // call the function
            #endif
            #endif //: #ifdef USE_JIT
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
        //---------------------------------------
        //
        // Expression type:
        //
        //   i = a * b + c;
        //
        //---------------------------------------
        if (see(l)=='=') {
            if ((i=VarFind(l->token)) != -1) {
                lex_save (l); // save the lexer position
                if (lex(l) == '=') {
                    lex(l);
                    expr1(l,a);
                    #ifdef USE_VM
                    // Copia o TOPO DA PILHA ( sp ) para a variavel ... e decrementa sp++.
                    emit_pop_var (a,i);
                    #endif
                    #ifdef USE_JIT
                    // Copia o TOPO DA PILHA ( %ESP ) para a variavel ... e estabiliza a PILHA
                    emit_pop_var (a, &Gvar[i].value.i);
                    #endif
              return i;
                } else {
                    lex_restore (l); // restore the lexer position
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
            if (op=='-') emit_sub_int (a);
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
            if (op=='/') emit_div_int (a);
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
F_STRING *fs_new (char *s) {
    static int count = 0;
    F_STRING *p = fs, *n;

    while (p) {
        if (!strcmp(p->s,s)) return p;
        p = p->next;
    }

    if ((n = (F_STRING*)malloc(sizeof(F_STRING)))==NULL) return NULL;
    n->s = strdup(s);

//printf ("FIXED: %p\n", &n->s);

    n->i = count++;
    // add on top
    n->next = fs;
    fs = n;

    return n;
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

    case 2: {
        TFunc *fi = stdlib;
        printf ("FUNCTIONS:\n---------------\n");
        while (fi->name) {
            if(fi->proto){
                char *s=fi->proto;
                if (*s=='0') printf ("void  ");
                else
                if (*s=='i') printf ("int   ");
                else
                if (*s=='f') printf ("float ");
                else
                if (*s=='s') printf ("char  *");
                else
                if (*s=='p') printf ("void * ");
                printf ("%s (", fi->name);
                s++;
                while(*s){
                    if (*s=='i') printf ("int");
                    else
                    if (*s=='f') printf ("float");
                    s++;
                    if(*s) printf (", ");
                }
                printf (");\n");
            }
            fi++;
        }
        }
        break;

    default:
        printf ("USAGE(%d): info(1);\n\nInfo Options:\n 1: Variables\n 2: Functions\n 3: Defines\n 4: Words\n",arg);
    }
}
int lib_add (int a, int b) {
    return a + b;
}

int lib_arg (int a, int b, int c, int d, int e) {
  printf ("arg1: %d\n", a);
  printf ("arg2: %d\n", b);
  printf ("arg3: %d\n", c);
  printf ("arg4: %d\n", d);
  printf ("arg5: %d\n", e);
  return a + b + c + d + e;
}
void lib_printf (char *format, ...) {
    char msg[1024] = { 0 };
    register unsigned int i;
    va_list ap;

    va_start (ap,format);
    vsprintf (msg, format, ap);
    va_end (ap);

    for (i = 0; i < strlen(msg); i++) {
        if (msg[i] == '\\' && msg[i+1] == 'n') { // new line
            putc (10, stdout); // new line
            i++;
        }
        else if (msg[i] == '\\' && msg[i+1] == 't') { // tab
            putc ('\t', stdout); // tab
            i++;
        } else {
            putc (msg[i], stdout);
        }
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
