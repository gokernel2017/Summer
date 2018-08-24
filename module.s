//-----------------------------------------------
//
// Module example using SDL 1.x
//
//-----------------------------------------------
//
int data = 500;

module ("libSDL", "sdl"); // in Linux:    "libSDL.so"
module ("SDL",    "sdl"); // in Windows:  "SDL.dll"

import  ("sdl", "SDL_Init",   "0i");
import  ("sdl", "SDL_SetVideoMode", "iiiii");
import  ("sdl", "SDL_Delay",  "0i");
import  ("sdl", "SDL_Quit",   "00");

  sdl.SDL_Init (32);

  data = sdl.SDL_SetVideoMode (800, 600, 16, 0); // color 16

  sdl.SDL_Delay (3000);

  sdl.SDL_Quit ();

  display (data);

//-----------------------------------------------
