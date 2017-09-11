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
TVar  Gvar[255];
int   return_type;

static FUNC   *Gfunc = NULL;  // store the user functions
static ASM    *asm_function = NULL;
static ASM    *asm_include = NULL;
static ARG    argument[20];

static int
    is_function,
    main_variable_type,
    var_type,
    is_recursive,
    argument_count
    ;
static char
    func_name [100]
    ;

extern void g4 (ASM *a, UCHAR c1, UCHAR c2, UCHAR c3, UCHAR c4);
extern void g2 (ASM *a, UCHAR c1, UCHAR c2);

//-------------------------------------------------------------------
//######################  PRIVATE PROTOTYPES  #######################
//-------------------------------------------------------------------
//
static void   word_int      (ASM *a);
static void   word_float    (ASM *a);
static void   word_function (ASM *a);
//
static void   print_var     (int index);
//
static void   expression    (ASM *a);
static void   expr0         (ASM *a);
static void   expr1         (ASM *a);
static void   expr2         (ASM *a);
static void   expr3         (ASM *a);
static int    see           (void);   // get the next char ... no increment ( str )
static void   execute_call  (ASM *a, FUNC *fi);
static FUNC *FuncFind  (char *name);

void lib_info (int arg);
void lib_hello (void);
void lib_arg_float (float arg1, int arg2);

/*
struct FUNC {
    char  *name;
    char  *proto; // prototype
    UCHAR *code;  // the function on JIT MODE | or VM in VM MODE
    int   type;   // FUNC_TYPE_COMPILED = 0, FUNC_TYPE_NATIVE, FUNC_TYPE_VM
    int   len;
    struct FUNC  *next;
};
*/
FUNC stdlib[]={
  //-----------------------------------------------------------------
  // char*        char*   UCHAR*/ASM*             int   int   FUNC*
  // name         proto   code                    type  len   next
  //-----------------------------------------------------------------
  { "info",       "0i",   (UCHAR*)lib_info,       0,    0,    NULL },
  { "hello",      "00",   (UCHAR*)lib_hello,      0,    0,    NULL },
  { "arg_float",  "00",   (UCHAR*)lib_arg_float,  0,    0,    NULL },
  { NULL,         NULL,   NULL,                   0,    0,    NULL }
};

void func_null (void) { printf ("FUNCTION: func_null\n"); }
FUNC func_null_default = { "func_null", "00", (UCHAR*)func_null, 0, 0, NULL };

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
    printf ("Core_Init VM (Virtual Machine)\n\n");
#endif
        if ((a            = asm_new (size))==NULL) return NULL;
        if ((asm_function = asm_new (size))==NULL) return NULL;
        if ((asm_include  = asm_new (size))==NULL) return NULL;

        #ifdef USE_JIT
        asm_set_executable (asm_include->code, size - 2);
        #endif

        CreateVarLong  ("0TYPE_LONG", 0);
        CreateVarFloat ("0TYPE_FLOAT", 0);

        if (erro) {
            printf ("\nERRO - core_Init: %s\n", asm_ErroGet());
            return NULL;
        }

        return a;
    }
    return NULL;
}
static int stmt (ASM *a) {
    ASM *a_f;

    if (is_function) a_f = asm_function; else a_f = a;

    tok = lex();

    switch (tok) {
    case '{':
        while (!erro && tok && tok != '}') stmt(a_f);
        tok = 1;
        return 1;
    case TOK_INT:      word_int      (a_f); return 1;
    case TOK_FLOAT:    word_float    (a_f); return 1;
    case TOK_FUNCTION: word_function (a_f); return 1;
    default:           expression    (a_f); return 1;
    case ';':
    case '}':
        return 1;
    case 0: return 0;
    }
    return 1;
}
static void expression (ASM *a) {
    char buf[100] = {0};
    int i;

    if(tok==TOK_NUMBER){ // OK !
        if((i=see())=='*' || i=='/'|| i=='+' || i=='-'){
            main_variable_type = var_type = TYPE_LONG; // 0

            if (strchr(token, '.')) main_variable_type = TYPE_FLOAT;

            expr0(a);

        #ifdef USE_JIT
            if(main_variable_type==TYPE_LONG)
                asm_popl_var(a, &Gvar[TYPE_LONG].value.l); // Gvar[0] : Copia o TOPO DA PILHA ( %ESP ) para a variavel
            if(main_variable_type==TYPE_FLOAT)
                asm_float_fstps (a, &Gvar[TYPE_FLOAT].value.f); // Gvar[1]

            //*** push argument 1 :
            g4 (a,0xc7,0x44,0x24,(char)0); // c7 44 24   04   dc 05 00 00 	  movl    $0x5dc,0x4(%esp)
            *(long*)a->p = main_variable_type;
            a->p += sizeof(long);
            asm_call (a, print_var, 0, 0);
        #endif
        #ifdef USE_VM
            vme_popvar (a, main_variable_type);
            vme_printvar (a, main_variable_type);
            vme_printc (a, 10); // new line
        #endif
        }//: if((i=see())=='*' || i=='/'|| i=='+' || i=='-')
  return;
    }//: if(tok==TOK_NUMBER)

    if (tok==TOK_ID) {
        char _token_[100];
        char *_str_;
        int  _tok_;
        int  _line_;
        FUNC *fi;

        main_variable_type = var_type = TYPE_LONG; // 0

        // call a function without return:
        // function_name (...);
        if ((fi = FuncFind(token)) != NULL) {
            execute_call (a, fi);
      return;
        }

        strcpy (_token_, token); // save
        _str_ = str;
        _tok_ = tok;
        _line_ = line;

        if((i=VarFind(token))!=-1){

            main_variable_type = var_type = Gvar[i].type;

            tok=lex(); // get next token

            if(tok==TOK_PLUS_PLUS){// a++;
                #ifdef USE_JIT
                asm_incl(a,&Gvar[i].value.l);
                #endif
                #ifdef USE_VM
                vme_incvar(a,i);
                #endif
            }
            else if(tok=='='){// i = a * b + c * d;
                tok=lex();
                expr0(a);

                #ifdef USE_JIT
                if(main_variable_type==TYPE_LONG)
                    asm_popl_var(a, &Gvar[i].value.l); // Gvar[0] : Copia o TOPO DA PILHA ( %ESP ) para a variavel
                if(main_variable_type==TYPE_FLOAT)
                    asm_float_fstps (a, &Gvar[i].value.f); // Gvar[1]
                #endif
                #ifdef USE_VM
                vme_popvar (a, i);
                #endif
            } else {
            // expression type, no compatible with C:
            // a * b + c * d;

                strcpy (token, _token_); // restore
                str = _str_;
                tok = _tok_;
                line = _line_;

                // expression type:
                // a * b + c * d;
                expr0(a);

                #ifdef USE_JIT
                if(main_variable_type==TYPE_LONG)
                    asm_popl_var(a, &Gvar[TYPE_LONG].value.l); // Gvar[0] : Copia o TOPO DA PILHA ( %ESP ) para a variavel
                if(main_variable_type==TYPE_FLOAT)
                    asm_float_fstps (a, &Gvar[TYPE_FLOAT].value.f); // Gvar[1]

                //*** push argument 1 :
                g4 (a,0xc7,0x44,0x24,(char)0); // c7 44 24   04   dc 05 00 00 	  movl    $0x5dc,0x4(%esp)
                *(long*)a->p = main_variable_type;
                a->p += sizeof(long);
                asm_call (a, print_var, 0, 0);
                #endif
                #ifdef USE_VM
                vme_popvar (a, main_variable_type);
                vme_printvar (a, main_variable_type);
                vme_printc (a, 10); // new line
                #endif
            }

        } else { //: if ((i=VarFind(token))!=-1)
            sprintf (buf, "EXPRESSION ERRO LINE(%d) - Ilegal Identifier '%s'", line, _token_);
            asm_Erro (buf);
        }
    } else {
        sprintf (buf, "EXPRESSION ERRO LINE(%d) - Ilegar Word (%s)\n", line, token);
        asm_Erro (buf);
    }
}

static void expr0 (ASM *a) { // '+' '-' : ADDITION | SUBTRACTION
    int op;
    expr1(a);
    while ((op=tok) == '+' || op == '-') {
        tok=lex();
        expr1(a);
        if (main_variable_type==TYPE_FLOAT) {
            #ifdef USE_JIT
            if (op=='+') asm_float_faddp(a);
            #endif
            #ifdef USE_VM
            if (op=='+') vme_addf(a);
            #endif
        } else { // LONG
            #ifdef USE_JIT
            if (op=='+') asm_add_eax_esp (a);
            #endif
            #ifdef USE_VM
            if (op=='+') vme_addl(a);
            #endif
        }
    }
}
static void expr1 (ASM *a) { // '*' '/' : MULTIPLICATION | DIVISION
    int op;
    expr2(a);
    while ((op=tok) == '*' || op == '/') {
        tok=lex();
        expr2(a);
        if (main_variable_type==TYPE_FLOAT) {
            #ifdef USE_JIT
            if (op=='*') asm_float_fmulp (a);
            #endif
            #ifdef USE_VM
            if (op=='*') vme_mulf (a);
            #endif
        } else { // LONG
            #ifdef USE_JIT
            if (op=='*') asm_imul_eax_esp (a);
            #endif
            #ifdef USE_VM
            if (op=='*') vme_mull (a);
            #endif
        }
    }
}
static void expr2 (ASM *a) { // '('
    if (tok=='(') {
        tok=lex(); expr0(a);
        if (tok != ')') {
            asm_Erro ("ERRO )\n");
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
            var_type = Gvar[i].type;

            tok=lex();

        #ifdef USE_JIT
            if (main_variable_type == TYPE_FLOAT && Gvar[i].type == TYPE_LONG) {
                // db 05    70 40 40 00    	fildl  0x404070
                g2(a,0xdb,0x05); asm_get_addr(a, &Gvar[i].value.l);
            } else {
                if (var_type==TYPE_FLOAT)
                    asm_float_flds (a, &Gvar[i].value.f);
                else
                    asm_pushl_var (a, &Gvar[i].value.l);
            }
        #endif
        #ifdef USE_VM
            vme_pushvar (a, i);
        #endif

        } else {
            char buf[255];
            sprintf(buf, "ERRO LINE(%d expr0()): Var Not Found '%s'", line, token);
            asm_Erro (buf);
        }
    }
    else if (tok==TOK_NUMBER) {
        if (strchr(token, '.'))
            var_type = TYPE_FLOAT;

    #ifdef USE_JIT
        if (var_type==TYPE_FLOAT)
            asm_float_flds_value (a, atof(token));
        else
            asm_push_number (a, atoi(token));
    #endif
    #ifdef USE_VM
        if (var_type==TYPE_FLOAT)
            vme_pushf (a, atof(token));
        else
            vme_pushl (a, atoi(token));
    #endif

        tok=lex();
    }
    else asm_Erro("Expression");
}
//void execute_call (ASM *a, UCHAR *code) {
void execute_call (ASM *a, FUNC *fi) {
    int count=0, pos=0, size = 4;
//    int exist_string = 0;

    //if ((tok=lex())!='(') { lex_erro ("Function need char: '('\n"); return; }

    while (!erro && (tok=lex())) { // pass function arguments ...
        if (tok==TOK_ID || tok==TOK_NUMBER || tok=='(') {
            if (tok==TOK_NUMBER) {
            #ifdef USE_JIT
                if (strchr(token, '.')) { // float
                    // b8   00 00 20 c1       	mov    $0xc1200000,%eax
                    // 89 44 24   08          	mov    %eax,0x8(%esp)
                    g(a,0xb8);
                    *(float*)a->p = atof(token);
                    a->p += sizeof(float);
                    g4(a,0x89,0x44,0x24,(char)pos);
//printf ("ARUMENT FLOAT(%s): (%f)\n", name, atof(token));
                } else {
                    // c7 44 24   04   dc 05 00 00 	  movl    $0x5dc,0x4(%esp)
                    g4(a,0xc7,0x44,0x24,(char)pos);
                    *(long*)a->p = atoi(token);
                    a->p += sizeof(long);
                }
            #endif
            #ifdef USE_VM
                if (strchr(token, '.')) { // float
                    vme_pushf (a, atof(token));
                } else {
                    vme_pushl (a, atoi(token));
                }
            #endif

                count++;
                pos += size;
            }//: if (tok==TOK_NUMBER)

        }//: if (tok==TOK_ID || tok==TOK_NUMBER || tok=='(')

        if (count > 15) break;
        if (tok==')' || tok==';') break;

    }//: while (!erro && (tok=lex()))

    return_type = '0';

    if (fi->proto) return_type = fi->proto[0];

    #ifdef USE_JIT    
    asm_call (a, fi->code, count, (UCHAR)return_type);
    #endif
    #ifdef USE_VM
    if (fi->type == FUNC_TYPE_VM) {
        asm_callvm (a, (ASM*)(fi->code));
    } else {
        asm_call (a, fi->code, count, (UCHAR)return_type);
    }
    #endif
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
        //
        // ... todo ...
        //
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

// int syntax: int i, a = 10, b = 20, c = 3, d = 5;
static void word_int (ASM *a) {
//    int pos = -4;

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
    if (tok != ';') asm_Erro ("ERRO: The word(int) need the char(;) on the end\n");
}
static void word_float (ASM *a) {
    while ((tok=lex())) {
        if (tok == TOK_ID) { // ok
            char  name[255];
            float value = 0.0;

            strcpy (name, token);

            tok=lex();

            if (tok == '=') {
                tok=lex();
                if (tok==TOK_NUMBER) {
                    value = atof (token);
                }
            }
            CreateVarFloat (name, value);
        }
        if (tok == ';') break;
    }
    if (tok != ';') asm_Erro ("ERRO: The word(float) need the char(;) on the end\n");
}
static void word_function (ASM *a) {
    struct FUNC *func;
    ASM *vm;
    char name[255], proto[255]={0};
    int i;

    tok=lex();

    strcpy (name,token);

    // if exist ... return
    //
    if (FuncFind(name)!=NULL) {
        int brace = 0;

        printf ("Function exist: ... REBOBINANDO '%s'\n", name);

        while((tok=lex()) && tok!=')');

        if (see()=='{') { } else asm_Erro ("word(if) need start block: '{'\n");

        while((tok=lex())){
            if (tok == '{') brace++;
            if (tok == '}') brace--;
            if (brace <= 0) break;
        }

  return;
    }

    // PASSA PARAMETROS ... IMPLEMENTADA APENAS ( int ) ... AGUARDE
    //
    // O analizador de expressao vai usar esses depois...
    //
    // VEJA EM ( expr3() ):
    // ---------------------
    // Funcoes usadas:
    //     ArgumentFind();
    //     asm_push_argument();
    // ---------------------
    //
    argument_count = 0;
    while ((tok=lex())) {
/*
        if (tok==TOK_INT) {
            strcpy (argument[argument_count].type, "int");
            tok=lex();
            if (tok==TOK_ID) {
                strcpy (argument[argument_count].name, token);
                argument_count++;
            }
        }
*/
        if (tok==TOK_ID) {
            argument[argument_count].type[0] = 0;
            strcpy (argument[argument_count].name, token);
            strcat (proto, token);
            strcat (proto, ", ");
            argument_count++;
        }
        if (tok=='{') break;

    }
    if (tok=='{') str--; else { asm_Erro("Word Function need char: '{'"); return; }

    is_function = 1;
//    local.count = 0;
    strcpy (func_name, name);

    // compiling to buffer ( f ):
    //
    asm_reset (asm_function);
    #ifdef USE_JIT
    asm_begin(asm_function);
    asm_sub_esp(asm_function,100);
    #endif
    stmt (a); // here start from char: '{'
    asm_end (asm_function);

    if (erro) return;

    if ((i=strlen(proto)) >= 3 )
        proto[i-2]=0;

#ifdef USE_JIT
    // new function:
    //
    func = (struct FUNC*) malloc (sizeof(struct FUNC));
    func->name = strdup (func_name);
    func->proto = strdup (proto);
    func->type = FUNC_TYPE_COMPILED;
    func->len = ASM_LEN (asm_function);//asm_function->len;
    func->code = (UCHAR*) malloc (func->len);
    // NOW: copy the buffer ( f ):
    for (i=0;i<func->len;i++) {
        func->code[i] = asm_function->code[i];
    }
/*
    //-------------------------------------------
    // HACKING ... ;)
    // Resolve Recursive:
    // change 4 bytes ( func_null ) to this
    //-------------------------------------------
    if (is_recursive)
    for (i=0;i<func->len;i++) {
        // b8     7a 13 40 00       mov    $0x40137a,%eax
        //
        if (func->code[i] == 0xb8) { // unsigned char: 184

            i++; // ... HERE: 4 bytes ...

            // ff d0    call   *%eax
            //
            if (*(func->code+i+4) == 0xff && *(func->code+i+5) == 0xd0) {
                if (*(void**)(func->code+i) == func_null) {
                    *(void**)(func->code+i) = func->code; //<<< change here
                    i += 5;
                }
            }
        }
    }
*/

    asm_set_executable (func->code, func->len-1);
#endif
#ifdef USE_VM
    if ((vm = asm_new (ASM_LEN (asm_function)))!=NULL) {
        // new function:
        //
        func = (struct FUNC*) malloc (sizeof(struct FUNC));
        func->name = strdup (func_name);
        func->proto = strdup (proto);
        func->type = FUNC_TYPE_VM;
        func->len = ASM_LEN (asm_function);//asm_function->len;
        // NOW: copy the buffer ( f ):
        for (i=0;i<func->len;i++) {
            vm->code[i] = asm_function->code[i];
        }
        func->code = vm;

/*
    //-------------------------------------------
    // HACKING ... ;)
    // Resolve Recursive:
    // change 4 bytes ( func_null ) to this
    //-------------------------------------------
    if (is_recursive)
*/
    }
    else return;

#endif

    // add on top:
    func->next = Gfunc;
    Gfunc = func;

    is_function = is_recursive = argument_count = *func_name = 0;
}

static int see (void) {
    char *s = str;
    while (*s) {
        if (*s=='\n' || *s==' ' || *s==9 || *s==13) {
            s++;
        } else return *s;
    }
    return 0;
}

void CreateVarLong (char *name, long value) {
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
        v->type = TYPE_LONG;
        v->value.l = value;
        v->info = NULL;
    }
}
void CreateVarFloat (char *name, float value) {
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
        v->type = TYPE_FLOAT;
        v->value.f = value;
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
#ifdef USE_JIT
static void print_var (int index) {
    switch (index) {
    case TYPE_LONG:  printf ("%ld\n", Gvar[TYPE_LONG].value.l);  break;
    case TYPE_FLOAT: printf ("%f\n",  Gvar[TYPE_FLOAT].value.f); break;
    }
}
#endif // #ifdef USE_JIT

//UCHAR *FuncFind (char *name) {
FUNC *FuncFind (char *name) {
    FUNC  *fi;
    FUNC  *lib;
    FUNC  *func;
//    T_import    *imp;

    if (!strcmp(name, func_name)) {
        is_recursive = 1;
        fi = &func_null_default;
        return fi;
//        return (UCHAR*)func_null; // to change, SEE IN ( word_function() )
    }

    // array:
    lib = stdlib;
    while (lib->name) {
        if ((lib->name[0]==name[0]) && !strcmp(lib->name, name))
      return lib;
        lib++;
    }

    // linked list:
    func = Gfunc;
    while(func){
        if ((func->name[0]==name[0]) && !strcmp(func->name, name))
      return func;
        func = func->next;
    }

/*
    // 03: linked list:
    imp = Gimport;
    while (imp) {
        if ((imp->name[0] == name[0]) && !strcmp(imp->name,name)) return imp->code;
        imp = imp->next;
    }
*/
    return NULL;
}

void lib_info (int arg) {
    switch (arg) {
    case 1: {
        TVar *v = Gvar;
        int i = 0;
        printf ("VARIABLES:\n---------------\n");
        while (v->name) {
            if (v->type==TYPE_LONG)  printf ("Gvar[%d](%s) = %ld\n", i, v->name, v->value.l);
            if (v->type==TYPE_FLOAT) printf ("Gvar[%d](%s) = %f\n", i, v->name, v->value.f);
            v++; i++;
        }
        } break;

    case 2: {
        FUNC *fi = stdlib;
        printf ("FUNCTIONS:\n---------------\n");
        while (fi->name) {
            if(fi->proto) printf ("%s ", fi->proto);
            printf ("%s\n", fi->name);
            fi++;
        }
        fi = Gfunc;
        while(fi){
            if(fi->proto) printf ("%s ", fi->proto);
            printf ("%s\n", fi->name);
            fi = fi->next;
        }
        } break;

    default:
        printf ("USAGE: info(1);\n\nInfo Options:\n 1: Variables\n 2: Functions\n 3: Defines\n 4: Words\n");
    }
}
void lib_hello (void) {
    printf ("Function HELLO WORLD\n");
}

void lib_arg_float (float arg1, int arg2) {
    printf ("arg_float: %f %d\n", arg1, arg2);
}
