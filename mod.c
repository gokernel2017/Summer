//-------------------------------------------------------------------
//
// A tiny module example:
//
// FILE:
//   mod.c
//
// COMPILE IN WINDOWS:
//   gcc -shared mod.c -o mod.dll -O2 -Wall
//
// COMPILE IN LINUX:
//   gcc -shared mod.c -o mod.so -O2 -Wall
//
// NOTE:
//   in linux copy the file ( mod.so ) to ( /usr/lib or /lib )
//
// USAGE:
//   summer test_mod.s
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include <stdio.h>

#ifdef _WIN32
    #define LLIBIMPORT extern __declspec (dllexport)
#endif
#ifdef __linux__
    #define LLIBIMPORT extern
#endif


LLIBIMPORT void mod_hello (void) {

    printf ("\nHello World - From module.function(mod_hello) ...\n\n");

}

