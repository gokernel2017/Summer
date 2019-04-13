//-------------------------------------------------------------------
//
// APP Example:
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
var win, bt1, bt2, editor, i = 100, count = 1;

function call_add (e) {
    //printi (i);
    i = i + 1;
    LOG ("i: %d", i);
}
function call_sub (e) {
    //printi (i);
    i = i - 1;
    LOG ("i: %d", i);
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

