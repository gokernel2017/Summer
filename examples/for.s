//-----------------------------------------------
//
// Test word ( for ):
//
//-----------------------------------------------
//
int i = 0;

console.log("----------------\n");
#ifdef USE_JIT
    console.log("Tag - USE_JIT:\n");
#endif
#ifdef USE_VM
    console.log("Tag - USE_VM:\n");
#endif
console.log("----------------\n");

function hello () {
    i;
    i = add (i,1);
}

  for (;;) {

      if (i > 20) {
          break;
      }

      hello();

  }


