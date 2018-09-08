//-------------------------------------------------------------------
//
// Application API(0.1.0): WEB MODE
//
// START DATE: 26/05/2018 - 10:40
//
//-------------------------------------------------------------------
//
// not used in WEB MODE:
var argc = 0;
var argv = 0;
//
//---------------------------
var object; // Gvar[0] | pointer
var event = 0;    // Gvar[1] | int
var mx = 0;       // Gvar[2] | int : mouse_x
var my = 0;       // Gvar[3] | int : mouse_y
// Event Type Number:
const MOUSEMOVE = 512; // WM_MOUSEMOVE

//---------------------------
//
var main_widget;
var zIndex = 1;
var isTouch = false;
var offset = [0, 0];
var isDown = false; // "mouse_clicked"

function AppInit (ac, av) {
  isTouch = (('ontouchstart' in window) || (navigator.MaxTouchPoints > 0) || (navigator.msMaxTouchPoints > 0));
  if (document.getElementsByTagName("body").length==0 || document.getElementsByTagName("link").length==0) {
    alert ("INIT ERRO: tag: <body> or tag: <link rel='stylesheet' href='garden.css' /> not found");
    return 0;
  }

  if (isTouch==false) {
    // desktop event: no mobile
    document.onmousemove = function(e) {
      //-------------------------------------------
      // isDown == true
      // on click in the window title bar.
      //-------------------------------------------
      if (isDown) {
        e.preventDefault();
        //
        // change widget position ( style value )
        //
        //mx = event.clientX;
        //my = event.clientY;
        main_widget.style.left = (e.clientX + offset[0]) + 'px';
        main_widget.style.top  = (e.clientY + offset[1]) + 'px';
      }//: if (isDown)
    }//: document.onmousemove = function(e)
  }//: if (isTouch==false)

  return 1;
}

function AppNewWindow (parent,x,y,w,h,txt) {
  var o=document.createElement("widget");
  var bar=document.createElement("widget_bar");
  var text=document.createElement("widget_bar_text");

//  o.setAttribute("id", "main_body");

  o.style.left = x+'px';
  o.style.top = y+'px';
  o.style.width = w+'px';
  o.style.height = h+'px';

  text.innerHTML = txt;

  // EVENTS:
  //
  if (isTouch) {
    // ...
  } else {
    //-------------------------------------------
    // DESKTOP MOUSE EVENT:
    //-------------------------------------------
    //
    o.addEventListener('mousedown', function(e) {

      this.style.zIndex = zIndex++; // move element to top:
      main_widget = this;

      mx = e.clientX-this.offsetLeft;
      my = e.clientY-this.offsetTop;

      // click in title_bar:
      //
      if (e.clientY-this.offsetTop < 35) {
        isDown = true; //<<<<<<<  enable here  >>>>>>>
        offset = [
          this.offsetLeft - e.clientX,
          this.offsetTop - e.clientY
        ];
      }

    }, true);

    o.addEventListener('mouseup', function() { isDown = false; }, true);

    o.addEventListener('mousemove', function(e) {
      object = e.target;
      mx = e.clientX-this.offsetLeft;
      my = e.clientY-this.offsetTop;
    }, true);

  }//: events ...

  bar.appendChild(text);
  o.appendChild(bar); // add title_bar
  document.body.appendChild(o); // add here

  return o;
}// AppNewWindow ()

function AppNewButton (parent,x,y,w,h,txt) {
  var o=document.createElement("button");

  o.innerHTML = txt; // button text

  o.style.position = "absolute";
  o.style.left = x+'px';
  o.style.top = y+'px';
  o.style.width = w+'px';
  o.style.height = h+'px';

  parent.appendChild(o); // add element on parent

  return o;
}

function app_new_terminal (parent, x, y, w, h) {
  // main object
  var o = document.createElement("div");
  o.className = "terminal";

  // 01 - history element:
  var history = document.createElement("div");

  //---------------------------------------------

  // 02 - line element:
  var container = document.createElement("div");
  container.className = "line";

  var _spa_ = document.createElement("span");
  _spa_.innerHTML = "c:/&nbsp;>&nbsp;";

  var _inp_ = document.createElement("input");
  _inp_.setAttribute("type", "text");
  _inp_.setAttribute("id", "input");
  _inp_.value = "Terminal BETA";

  container.appendChild(_spa_);
  container.appendChild(_inp_);

  //---------------------------------------------

  o.style.left = x+'px';
  o.style.top = y+'px';
  o.style.width = w+'px';
  o.style.height = h+'px';
  o.style.overflowY = "auto";
//document.getElementById("myDIV").style.overflowY = "scroll"; 

  // click in this ... set focus to element: input
  //
  o.addEventListener('click', function () {
    _inp_.focus();
  });

  _inp_.onkeydown = function(e) {
    if (e.keyCode == 13) {

      history.innerHTML += this.value + '<br/>';
      
      // you can change the path if you want
      // crappy implementation here,  but you get the idea
//      if($(this).val().substring(0, 3) === 'cd '){
//  			$('#path').html($(this).val().substring(3)+'&nbsp;>&nbsp;');
//      }
      
      // clear the input
      this.value = '';
    }
  }

  // 01:
  o.appendChild(history);
  o.appendChild(container);

  parent.appendChild(o); // add element on parent

  return o;
}

function AppSetCall (o,call,type) {
  if (type=="onclick") o.onclick = call;
  if (type=="onmousemove") o.onmousemove = call;
}

function AppRun () {

}

/*
var input = document.createElement("input");
input.setAttribute('type', 'text');

*/
