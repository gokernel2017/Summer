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
var _CANVAS_;   // <<<<<<<<<<  The Real Canvas  >>>>>>>>>>
var _CONTEXT_;  // The context to real canvas
//
var canvas;   // Off Screen Canvas
var bmp;      // The context of Off Screen Canvas | <<<<<<<<<<  DRAW IN THIS  >>>>>>>>>>

var _fps_ = 0;
var _value_fps_ = 0;
var _count_ = 0;

function main_idle () { }

function gaInit (w,h,idle) {
  isTouch = (('ontouchstart' in window) || (navigator.MaxTouchPoints > 0) || (navigator.msMaxTouchPoints > 0));
  if (document.getElementsByTagName("body").length==0 || document.getElementsByTagName("link").length==0) {
    alert ("INIT ERRO: tag: <body> or tag: <link rel='stylesheet' href='gapplication.css' /> not found");
    return 0;
  }

  _CANVAS_ = document.createElement("canvas"); // append this
  canvas = document.createElement("canvas"); // ... not append ... off screen

	_CANVAS_.width = w;
	_CANVAS_.height = h;
  canvas.width = w;
	canvas.height = h;
  _CANVAS_.style.border = "2px solid orange"; 

  _CONTEXT_ = _CANVAS_.getContext ("2d");
  bmp       = canvas.getContext ("2d"); // draw in this

  bmp.font = "12px sans-serif";
  bmp.fillStyle = "orange";

  document.body.appendChild (_CANVAS_); // add here

  if (idle)
    main_idle = idle;

  setInterval(function () { // setInterval
    _value_fps_ = _fps_;
    _fps_ = 0;
    _count_++;
  }, 1000);

  return 1;

}// gaInit()

function testDrawTriangle() {
  bmp.strokeStyle = 'orange';
  bmp.stroke();

  bmp.beginPath();
  bmp.moveTo(200, 100);
  bmp.lineTo(100, 300);
  bmp.lineTo(300, 300);
  bmp.closePath();
}

function gaRun () {
  main_idle ();
  _fps_++;
  requestAnimationFrame (gaRun);
}

function gaBeginScene(){
  bmp.fillStyle = "#000000"; // black
  bmp.fillRect(0,0,canvas.width,canvas.height);
}
function gaEndScene(){
  _CONTEXT_.drawImage (canvas, 0, 0);
}

function gaFPS () {
  bmp.fillStyle = "orange";
  bmp.fillText ("FPS: "+_value_fps_,7,18);
//  if (_fps_==0) return 1;
//  else          return 0;
}

