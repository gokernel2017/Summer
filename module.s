//-----------------------------------------------
//
// Module example using SDL 1.x
//
//-----------------------------------------------
//
module ("libSDL", "sdl");  // in Linux
module ("SDL",    "sdl");     // in Windows

import  ("sdl", "SDL_Init",   "0i");
import  ("sdl", "SDL_SetVideoMode", "iiiii");
import  ("sdl", "SDL_Delay",  "0i");
import  ("sdl", "SDL_Quit",   "00");

  sdl.SDL_Init (32);

  sdl.SDL_SetVideoMode (800, 600, 16, 0); // color 16

  sdl.SDL_Delay (3000);

  sdl.SDL_Quit ();

//-----------------------------------------------
