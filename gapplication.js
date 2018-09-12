//-------------------------------------------------------------------
//
// Graphic Application API:
//
//   A portable graphic API ( C & WEB ) ... OpenGL, DirectX, HTML_Canvas.
//
// FILE:
//   gapplication.js
//
// START DATE: 12/09//2017 - 07:25
//
//-------------------------------------------------------------------
//
var screen; // <<<<<<<<<<  The Canvas  >>>>>>>>>>

function main_idle () { }

function gaInit (w,h,idle) {
  isTouch = (('ontouchstart' in window) || (navigator.MaxTouchPoints > 0) || (navigator.msMaxTouchPoints > 0));
  if (document.getElementsByTagName("body").length==0 || document.getElementsByTagName("link").length==0) {
    alert ("INIT ERRO: tag: <body> or tag: <link rel='stylesheet' href='gapplication.css' /> not found");
    return 0;
  }

  screen=document.createElement("canvas");
  screen.style.width = w+'px';
  screen.style.height = h+'px';
  screen.style.border = "2px solid orange"; 
  document.body.appendChild(screen); // add here

  if (idle)
    main_idle = idle;

  return 1;
}

function gaRun () {
  main_idle ();
  requestAnimationFrame (gaRun);
}
