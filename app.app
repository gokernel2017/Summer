//-------------------------------------------------------------------
//
// APP Example:
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
var win, button, editor, i = 100;

//$var i=100,bt1,bt2; function call1(e){LOG("i: %d",i); i=i+1; } bt1=app_NewButton(0,0,30,30,"ADD"); app_SetCall(bt1,call1); function call2(msg){LOG("i: %d",i); i = i - 1; } bt2 = app_NewButton(0,0,150,30,"SUB"); app_SetCall(bt2,call2);
//$var i=100,bt1,bt2; function call1(e){LOG("i: %d",i); i=i+1; } bt1=app_NewButton(0,0,30,30,"ADD"); app_SetEvent(bt1,call1,"onmouseup"); function call2(msg){LOG("i: %d",i); i = i - 1; } bt2 = app_NewButton(0,0,150,30,"SUB"); app_SetEvent(bt2,call2,"onmouseup");

function call_button (e) {
  //LOG ("i: %d", i);
    printi (i);
    i = i + 1;
}

if (app_Init(0,0)) {

    button = app_NewButton (0, 0, 20, 20, "Button 1");

//    app_SetCall (button, call_button);
    app_SetEvent (button, call_button, "onmouseup");

    editor = app_NewEditor  (0, 20, 70, "Simple Editor Test\n\n", 5000);
    
    app_SetSize (editor, 700, 500);

    app_Run (0);
}

