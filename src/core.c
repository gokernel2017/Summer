//-------------------------------------------------------------------
//
// SUMMER LANGUAGE:
//
// Main Core:
//
// FILE:
//   core.c
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

// INFO: Windows X64 BITS functions arguments:
// arg 0 = %ecx
// arg 1 = %edx
// arg 2 = %r8d
// arg 3 = %r9d
//
// INFO: Linux X64 BITS functions arguments:
// arg 0 = %edi
// arg 1 = %esi
// arg 2 = %edx
// arg 3 = %ecx
// arg 4 = %r8
// arg 5 = %r9


//-----------------------------------------------
//-----------------  PROTOTYPES  ----------------
//-----------------------------------------------
//
static void word_int (LEXER *l, ASM *a);
static void word_var (LEXER *l, ASM *a); // int, float
static void word_asm (LEXER *l, ASM *a);
//
static int 	expr0 (LEXER *l, ASM *a);
static void	expr1 (LEXER *l, ASM *a);
static void expr2 (LEXER *l, ASM *a);
static void expr3	(LEXER *l, ASM *a);
static void atom 	(LEXER *l, ASM *a);
static int	see		(LEXER *l);
//
static TFstring *fs_new (char *s);

static void execute_call (LEXER *l, ASM *a, TFunc *func);
static void DefineAdd (char *name, int value);

void lib_info		(int arg);
void lib_help		(int i);
void lib_disasm (char *name);
int  lib_func_add (int a, int b);
void lib_prints  (char *s);
void lib_printi  (int i);

static TFunc stdlib[]={
  //-----------------------------------------------------------------------------
  // char*        char*   	UCHAR*/ASM*             int   int   int       FUNC*
  // name         proto   	code                    type  len   sub_esp   next
  //-----------------------------------------------------------------------------
  { "info",				"0i",			(UCHAR*)lib_info,       0,    0,  	0,  			NULL },
  { "help",				"0i",			(UCHAR*)lib_help,       0,    0,  	0,  			NULL },
	{ "disasm",			"0s",     (UCHAR*)lib_disasm,     0,    0,  	0,  			NULL },
  { "func_add",		"iii",		(UCHAR*)lib_func_add,   0,    0,  	0,  			NULL },
  { "prints",			"0s",		  (UCHAR*)lib_prints,    	0,    0,  	0,  			NULL },
  { "printi",			"0i",		  (UCHAR*)lib_printi,    	0,    0,  	0,  			NULL },
  { NULL,					NULL,			NULL,                   0,    0, 		0,   			NULL }
};

//-----------------------------------------------
//------------------  VARIABLES  ----------------
//-----------------------------------------------
//
int value;
static TFunc *Gfunc = NULL;
TVar Gvar [GVAR_SIZE]; // global:

ASM *asm_function;
static TFstring *fs = NULL;
static TDefine *Gdefine = NULL;

static char func_name [100];

static int
		is_function,
		main_variable_type, var_type
		;

static int expr0 (LEXER *l, ASM *a) {
    if (l->tok == TOK_ID) {
        //---------------------------------------
        //
        // Expression type:
        //
        //   i = a * b + c;
        //
        //---------------------------------------
/*
        if (see(l)=='=') {
						char buf[100];
						strcpy (buf, l->token);
						if (lex(l) == '=') {
								lex(l);
								expr1(l,a);
								emit_mov_reg_var (a, EAX, &value);
								return 1;
						}
				}
*/
        //---------------------------------------
        //
        // Expression type:
        //
        //   i = a * b + c;
        //
        //---------------------------------------
        if (see(l)=='=') {
						int i = 1;
            if ((i=VarFind(l->token)) != -1) {
                lex_save (l); // save the lexer position
                if (lex(l) == '=') {
                    lex(l);
                    expr1(l,a);
                    if(main_variable_type==TYPE_FLOAT) {
                        emit_float_fstps (a, &Gvar[i].value.f); // Gvar[1]
                    } else {
												emit_mov_reg_var (a, EAX, &Gvar[i].value.l);
                    }
              return i;
                } else {
                    lex_restore (l); // restore the lexer position
                }
            }//: if ((i=VarFind(l->token)) != -1)
        }//: if (see(l)=='=')
    }
    expr1 (l, a);
    return -1;
}
static void expr1 (LEXER *l, ASM *a) { // '+' '-' : ADDITION | SUBTRACTION
    int op;
    expr2 (l, a);
    while ((op=l->tok) == '+' || op == '-') {
        lex(l);
        expr2(l,a);
        if (var_type==TYPE_FLOAT) {
            if (op=='+') emit_add_float(a);
            if (op=='-') emit_sub_float(a);
        } else { // TYPE_LONG
						if (op=='+') emit_expression_add_long(a);
						if (op=='-') emit_expression_sub_long(a);
				}
    }
}
static void expr2 (LEXER *l, ASM *a) { // '*' '/' : MULTIPLICATION | DIVISION
    int op;
    expr3(l,a);
    while ((op=l->tok) == '*' || op == '/') {
        lex (l);
        expr3 (l, a);

        if (var_type==TYPE_FLOAT) {
            if (op=='*') emit_mul_float(a);
            if (op=='/') emit_div_float(a);
        } else { // TYPE_LONG
						if (op=='*') emit_expression_mul_long(a);
						if (op=='/') emit_expression_div_long(a);
        }
    }
}
static void expr3 (LEXER *l, ASM *a) { // '('
    if (l->tok=='(') {
        lex(l); expr0(l,a);
        if (l->tok != ')') {
            Erro("ERRO )\n");
        }
        lex(l);
    }
    else atom(l,a); // atom:
}
static void atom (LEXER *l, ASM *a) { // expres
    if (l->tok == TOK_STRING) {
        TFstring *s = fs_new (l->token);
        if (s) {
            emit_expression_push_long (a, (long)(s->s));
//            g(a, 0x68); asm_get_addr(a,s->s);
        }
        lex(l);
  return;
    }

		if (l->tok == TOK_ID) {
				int i;
				if ((i = VarFind(l->token)) != -1) {
            var_type = Gvar[i].type;

//            #ifdef USE_JIT
            if (main_variable_type == TYPE_FLOAT && var_type != TYPE_FLOAT) {
                // db 05    70 40 40 00    	fildl  0x404070
                //g2(a,0xdb,0x05); asm_get_addr(a, &Gvar[i].value.i);
                Erro ("%s: %d: Float and Integer ... Not Permited: '%s' ;)\n", l->name, l->line, Gvar[i].name);
            } else {
                if (var_type == TYPE_FLOAT) {
                    emit_float_flds (a, &Gvar[i].value.f);
                } else {
										emit_expression_push_var (a, &Gvar[i].value.l);
                }
            }
//            #endif
//            #ifdef USE_VM
//            emit_push_var (a,i);
//            #endif

            lex(l);
        }
        else Erro ("%s: %d: - Var Not Found '%s'\n", l->name, l->line, l->token);
		}
		else if (l->tok == TOK_NUMBER) {
        if (strchr(l->token, '.'))
            var_type = TYPE_FLOAT;

        if (var_type==TYPE_FLOAT) {
            emit_push_float(a, atof(l->token));
        } else {
						emit_expression_push_long (a, atol(l->token));
        }
        lex(l);
    }
    else {
				printf ("ATOM ERRO TOKEN(%s) ... SAINDO\n", l->token);
				Erro("%s: %d Expression atom - Ilegal Word (%s)\n", l->line, l->token);
		}
}// atom ()

void expression (LEXER *l, ASM *a) {

    if (l->tok == TOK_STRING) { // OK !
        TFstring *s = fs_new (l->token);
        if (s) {
						emit_print_string (a, s->s);
        }
  return;
    }

    if (l->tok==TOK_ID || l->tok==TOK_NUMBER) {
				TFunc *fi;
				int i, next;

        if (l->tok==TOK_NUMBER && strchr(l->token, '.'))
            main_variable_type = var_type = TYPE_FLOAT;
        else
            main_variable_type = var_type = TYPE_LONG; // 0

				next = see(l);

        //
        // call a function without return:
        //   function_name (...);
        //
        if ((fi = FuncFind(l->token)) != NULL) {
            execute_call (l, a, fi);
      return;
        }

        if ((i = VarFind(l->token)) != -1) {
            main_variable_type = var_type = Gvar[i].type;
						// ...
            if (next=='=') {
                lex_save(l); // save the lexer position
                lex(l); // =
                if (lex(l)==TOK_ID) {

/*
                    if (see(l)=='.' && ModuleIsLib(l->token)) {
                        lex_save(l); // save the lexer position
                        sprintf(buf, "%s", l->token);
                        lex(l); // .
                        if (lex(l)==TOK_ID && (fi = ModuleFind(buf, l->token)) != NULL) { // Module FuncName

                            execute_call(l, a, fi);

                            //
                            // The function return is stored in register %eax ... types ! TYPE_FLOAT
                            //
                            //   HERE: copy the register ( %eax ) to the variable
                            //
                            if (main_variable_type != TYPE_FLOAT) {
                                emit_mov_reg_var(a, EAX, &Gvar[i].value.i);
                            } else {

                            }

                      return;
                        }
                    }
*/
                    //
                    // call a function with return:
                    //   i = function_name (...);
                    //
                    if ((fi = FuncFind(l->token)) != NULL) {
                        execute_call(l, a, fi);

                        //
                        // The function return is stored in register %eax ... types ! TYPE_FLOAT
                        //
                        //   HERE: copy the register ( %eax ) to the variable
                        //
                        // OBS:
                        //   todo retorno eh armazenado em %eax, exceto tipo (float e double).
                        //
                        if (Gvar[i].type==TYPE_FLOAT) {
                            emit_float_fstps(a, &Gvar[i].value.f);
                        } else {
                            emit_mov_reg_var(a, EAX, &Gvar[i].value.l);
                        }

                  return;
                    }//: if ((fi = FuncFind(l->token)) != NULL)

                }
                lex_restore(l); // restore the lexer position

            }// if (next=='=')

				}// if ((i = VarFind(l->token)) != -1)

        //---------------------------------------
        // Expression types:
        //   a * b + c * d;
        //   10 * a + 3 * b;
        //   i;
        //---------------------------------------
        //
				asm_expression_reset(); // reg = 0;
				if (expr0(l,a) == -1) { // Expression: a + b;
						int new_line = 0;
						if (l->tok==';')
								new_line = 1;
						// display the expression result
						emit_pop_print_result (a, main_variable_type, new_line);
				}
    }
    else Erro("%s: %d | Expression ERRO - Ilegar Word (%s)\n", l->name, l->line, l->token);
}

int stmt (LEXER *l, ASM *a) {
		lex (l);

		switch (l->tok) {
		case TOK_INT: word_int (l,a); return 1;
		case TOK_VAR: word_var (l,a); return 1;
		case TOK_ASM: word_asm (l,a); return 1;
    default: expression (l,a); return 1;
		case TOK_NEW_LINE:
    case ';':
    case ',':
				return 1;
		case 0: return 0;
		}
//		printf ("TOKEN(%s)\n", l->token);
		return 1;
}

int core_Parse (LEXER *l, ASM *a, char *text, char *name) {
		lex_set (l, text, name);
		ErroReset ();
		asm_Reset (a);
		emit_begin (a);
		while (!erro && stmt(l,a)) {
				// ... compiling ...
		}
		emit_end (a);
		return erro;
}

//
// function_name (a, b, c + d);
//
static void execute_call (LEXER *l, ASM *a, TFunc *func) {
    int count = 0, i;
		#if !defined(__x86_64__)
    int pos = 0, size = 4, is_float = 0, is_string = 0; // Used in: JIT 32 bits
		#endif

    // no argument
    if (func->proto && func->proto[1] == '0') {
        while (lex(l))
            if (l->tok == ')' || l->tok == ';') break;
    } else {

        // get next: '('
        if (lex(l)!='(') { Erro ("Function need char: '('\n"); return; }

// INFO: Windows X64 BITS functions arguments:
// arg 0 = %ecx
// arg 1 = %edx
// arg 2 = %r8d
// arg 3 = %r9d
//
// INFO: Linux X64 BITS functions arguments:
// arg 0 = %edi
// arg 1 = %esi
// arg 2 = %edx
// arg 3 = %ecx
// arg 4 = %r8
// arg 5 = %r9

        while (lex(l)) {

            if (l->tok==TOK_ID || l->tok==TOK_NUMBER || l->tok==TOK_STRING || l->tok=='(') {

                main_variable_type = var_type = TYPE_LONG;

								asm_expression_reset(); // reg = 0;
                // The result of expression is store in the register: %eax.
                expr0 (l,a);

                #if defined(__x86_64__)
								// 4 arquments
								if (count <= 3)
										gen(a,PUSH_EAX);
                #else
                //-----------------------------------
                // JIT 32 bits
                if (var_type != TYPE_FLOAT) {
                    emit_mov_eax_ESP (a, pos);
                    size = 4;
                    pos += size;
                } else { // JIT 32 bits: pass function argument float
/*
                    // send the result of (float expression) to: 4(%esp)
                    //
                    // dd 5c 24   04    fstpl  0x4(%esp)
                    // d9 5c 24   04    fstps  0x4(%esp)
                    //
                    if (is_string)
                        g4 (a,0xdd,0x5c,0x24, (char)pos); // fstpl
                    else
                        g4 (a,0xd9,0x5c,0x24, (char)pos); // fstps
                    if (is_string) size = 8;
                    pos += size;
*/
                }
                #endif

                if (count++ > 15) break;
            }
            if (l->tok == ')' || l->tok == ';') break;
        }
    }

    if (count > 6) {
        Erro ("%s:%d: - Call Function(%s) the max arguments is: 5\n", l->name, l->line, func->name);
  return;
    }

		// pass 64 bits arquments:
		#if defined(__x86_64__)
		if (count > 0) {
				for (i = count-1; i >= 0; i--) {
						if (i == 0) { // argument 1
								#ifdef WIN32
								gen(a,POP_ECX);
								#else
								gen(a,POP_EDI);
								#endif
						}
						else if (i == 1) { // argument 2
								#ifdef WIN32
								gen(a,POP_EDX);
								#else
								gen(a,POP_ESI);
								#endif
						}
				}
		}
		#endif

		emit_call (a, func->code, (UCHAR)count, 0);
}

ASM * core_Init (unsigned int size) {
		ASM *a;
		static int init = 0;
		if (!init) {
				init = 1;
        if ((a =            asm_New(size)) == NULL) return NULL;
        if ((asm_function = asm_New(size)) == NULL) return NULL;

				#ifdef WIN32
				DefineAdd("WIN32", 1);
				#endif
				#ifdef __linux__
				DefineAdd("__linux__", 2);
				#endif

				#ifdef __x86_64__
						// 64 bits
						DefineAdd("__x86_64__", 2);
						#ifdef WIN32
						DefineAdd("WINDOWS_64", 1);
						#endif
						#ifdef __linux__
						DefineAdd("LINUX_64", 2);
						#endif
				#else // 64 bits
						// 32 bits
						DefineAdd("__x86_32__", 2);
						#ifdef WIN32
						DefineAdd("WINDOWS_32", 1);
						#endif
						#ifdef __linux__
						DefineAdd("LINUX_32", 2);
						#endif
				#endif // 32 bits
        return a;
		}
		return NULL;
}

void core_Finalize (void) {
    // free Fixed String:
		//printf ("Fixed String FREE ...\n");
    while (fs != NULL) {
        TFstring *temp = fs->next;
        if (fs->s) {
            //printf ("FSTRING(%s)\n", fs->s);
            free(fs->s);
        }
        free(fs);
        fs = temp;
    }
}

static int see (LEXER *l) {
    char *s = l->text+l->pos;
    while (*s) {
        if (*s=='\n' || *s==' ' || *s==9 || *s==13) {
            s++;
        } else {
            if (s[0]=='=' && s[1]=='=') return TOK_EQUAL_EQUAL;
            if (s[0]=='+' && s[1]=='+') return TOK_PLUS_PLUS;
            if (s[0]=='+' && s[1]=='=') return TOK_PLUS_EQUAL;
            if (s[0]=='-' && s[1]=='-') return TOK_MINUS_MINUS;
            return *s;
        }
    }
    return 0;
}

void CreateVar (char *name, char *svalue) {
    TVar *v = Gvar;
    int i = 0;
    while (v->name) {
        if (!strcmp(v->name, name))
      return;
        v++;
        i++;
    }
    if (i < GVAR_SIZE) {
        if (strchr(svalue, '.')) {
            v->type = TYPE_FLOAT;
            v->value.f = atof(svalue);
        } else {
            v->type = TYPE_LONG;
            v->value.l = atol(svalue);
        }
        v->name = strdup(name);
        v->info = NULL;
    }
}

void CreateVarLong (char *name, long l) {
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
        v->value.l = l;
        v->info = NULL;
    }
}

void FuncAdd (TFunc *func) {
		// add on top:
		func->next = Gfunc;
		Gfunc = func;
}

static void DefineAdd (char *name, int value) {
    TDefine *o = Gdefine, *n;
    while (o) {
        if (!strcmp(o->name, name)) return;
        o = o->next;
    }
    if ((n = (TDefine*)malloc(sizeof(TDefine)))==NULL) return;
    n->name = strdup(name);
    n->value = value;
    // add on top
    n->next = Gdefine;
    Gdefine = n;
}

int is_defined (char *name) {
    TDefine *p = Gdefine;
    while (p) {
        if (!strcmp(p->name, name)) {
            return 1;
        }
        p = p->next;
    }
		return 0;
}

void proc_ifdef (char *name) {
    TDefine *p = Gdefine;
    while (p) {
        if (!strcmp(p->name, name)) {
            return ;
        }
        p = p->next;
    }
		ifndef_true = 1;

/*
    TDefine *p = Gdefine;
    while (p) {
        if (!strcmp(p->name, name)) {
						ifdef_inc++;
            return ;
        }
        p = p->next;
    }
		printf("HELLO ---------------------\n");
		ifdef_index++;
		ifdef_array[ifdef_index] = ifdef_index;
		ifndef_true++;
*/
}

TFunc *FuncFind (char *name) {
    // array:
    TFunc *lib = stdlib;
    while (lib->name) {
        if ((lib->name[0]==name[0]) && !strcmp(lib->name, name))
      return lib;
        lib++;
    }

    // linked list:
    TFunc *func = Gfunc;
    while (func) {
        if ((func->name[0]==name[0]) && !strcmp(func->name, name))
      return func;
        func = func->next;
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

static TFstring *fs_new (char *s) {
    static int count = 0;
    TFstring *p = fs, *n;

    while (p) {
        if (!strcmp(p->s,s)) return p;
        p = p->next;
    }

    if ((n = (TFstring*)malloc(sizeof(TFstring)))==NULL) return NULL;
    n->s = strdup(s);

//printf ("FIXED: %p\n", &n->s);

    n->i = count++;
    // add on top
    n->next = fs;
    fs = n;

    return n;
}

static void word_int (LEXER *l, ASM *a) {
    while (lex(l)) {
        if (l->tok==TOK_ID) {
            char name[255];
            int value = 0;

            strcpy (name, l->token); // save

            if (lex(l) == '=') {
                if (lex(l) == TOK_NUMBER)
                    value = atoi (l->token);
            }
            if (is_function) {
/*
                //---------------------------------------------------
                // this is temporary ...
                // in function(word_function) changes ...
                //---------------------------------------------------
                if (local_count < LOCAL_MAX) {
                    sprintf (local[local_count].name, "%s", name);
                    local[local_count].type = TYPE_LONG;
                    local[local_count].value.l = value;
                    local[local_count].info = NULL;
                    local_count++;
                }
                else Erro ("Variable Local Max %d\n", LOCAL_MAX);
                // ... need implementation ...
*/
            }
            else CreateVarLong (name, value);
        }
        if (l->tok == ';') break;
    }

    if (l->tok != ';') Erro ("ERRO: The word(int) need the char(;) on the end\n");

}// word_int()

static void word_var (LEXER *l, ASM *a) {
    while (lex(l)) {
        if (l->tok == TOK_ID) {
            char name[100];
            char svalue[100] = { '0', 0 };

            strcpy (name, l->token); // save

            if (lex(l) == '=') {
                if (lex(l) == TOK_NUMBER) {
                    sprintf (svalue, "%s", l->token);
                }
            }
            CreateVar (name, svalue);
        }
        if (l->tok == ';') break;
    }
    if (l->tok != ';') Erro ("ERRO: The word(var) need the char(;) on the end\n");

}//: word_var ()

static void word_asm (LEXER *l, ASM *a) {
		Assemble (l,a);
}

void lib_info (int arg) {
    switch (arg) {
    case 1: {
        TVar *v = Gvar;
        int i = 0;
        printf ("VARIABLES:\n---------------\n");
        while (v->name) {
            if (v->type==TYPE_LONG)   printf ("Gvar[%d](%s) = %ld\n", i, v->name, v->value.l);
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
                    if (*s=='0') printf ("void");
										else
                    if (*s=='i') printf ("int");
                    else
                    if (*s=='f') printf ("float");
                    else
                    if (*s=='s') printf ("char *");
                    s++;
                    if(*s) printf (", ");
                }
                printf (");\n");
            }
            fi++;
        }
        fi = Gfunc;
        while(fi){
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
                    if (*s=='0') printf ("void");
										else
                    if (*s=='i') printf ("int");
                    else
                    if (*s=='f') printf ("float");
                    else
                    if (*s=='s') printf ("char *");
                    s++;
                    if(*s) printf (", ");
                }
                printf (");\n");
            }
            fi = fi->next;
        }
				}
        break;

    case 3: {
        TDefine *o = Gdefine;
        printf ("ALL DEFINES:\n---------------\n");
        while (o) {
            printf ("  %s : %d\n", o->name, o->value);
            o = o->next;
        }
        } break;

    default:
        printf ("USAGE(%d): info(1);\n\nInfo Options:\n 1: Variables\n 2: Functions\n 3: Defines\n 4: Words\n",arg);
    }
}

void lib_help (int i) {
		printf ("\nEsta eh uma funcao da STDLIB chamada HELP( %d )\n", i);
}

void lib_disasm (char *name) {
		if (name) {
				if (!strcmp(name, "main")) {
						ASM *_a_ = GetAsmMain();
						Disasm (asm_GetCode(_a_), name, asm_GetLen(_a_));
				} else {
						TFunc *func = FuncFind(name);
						if (func && func->type == FUNC_TYPE_COMPILED) {
								Disasm (func->code, name, func->len);
						}
				}
		}
}

int lib_func_add (int a, int b) {
		printf ("a: %d, b: %d = %d\n", a, b, a+b);
		return a + b;
}

void lib_prints (char *s) {
		printf ("STRING(%s)\n", s);
}
void lib_printi (int i) {
		printf ("%d\n", i);
}

