//-------------------------------------------------------------------
//
// Function Arguments Example:
//
//-------------------------------------------------------------------
//
int a = 300, b = 700 , argument1 = 1;

function hello (int arg1, int arg2)
{
    #ifdef USE_JIT
    "\nSummer TAG: USE_JIT\n";
    #endif
    #ifdef USE_VM
    "\nSummer TAG: USE_VM\n";
    #endif

    a = arg1;
    b = arg2;

    somai (arg1, arg2);
    info (arg1);

    "\nFunction hello | set arg1 a = " a;
    "\nFunction hello | set arg2 b = " b;
}

    "\nValue a: " a; // 300
    "Value b: " b; // 700

    hello (argument1, 1000);

    // here a = (arg1) : 1
    // here b = (arg2) : 1000
