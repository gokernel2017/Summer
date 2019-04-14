//-------------------------------------------------------------------
//
// APP Example:
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "hello.sum"

var win, bt1, bt2, editor, i = 100, count = 1, a;

function call_add (int msg) {
    i = i + 1;
    LOG ("i: %d", i);
//    printi (i);
}
function call_sub (int msg) {
    i = i - 1;
    LOG ("i: %d", i);
//    printi (i);
}

if (app_Init(0,0)) {

    //
    // Create only one
    //
    if (!bt1) {
        LOG ("Starting File: 'app.app' ---------- count: %d", count);
        count = count + 1;

        bt1 = app_NewButton (0, 0, 30, 30, "ADD");
        app_SetCall (bt1, call_add);

        bt2 = app_NewButton (0, 0, 150, 30, "SUB");
        app_SetCall (bt2, call_sub);

        app_Run (0);
    }

}

