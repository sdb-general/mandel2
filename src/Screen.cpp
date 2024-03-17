#include "include/Screen.h"
#include "include/Globals.h"
#include "include/Utils.h"
#include <math.h>
#include <vector>

static SDL_Event     event;
static SDL_Window*   window; 
static SDL_Renderer* renderer; 

void createScreenAndRender(const std::vector<int>& pixels)
{
  StopWatch s("Rendering");
  if ( SDL_Init(SDL_INIT_VIDEO) != 0 )
  {
    std::cout << "Failed to init video\n"; std::abort();
  }
  window = SDL_CreateWindow( "Mandelbrot", 
                              SDL_WINDOWPOS_UNDEFINED, 
                              SDL_WINDOWPOS_UNDEFINED,
                              Config::Instance.getSW(),
                              Config::Instance.getSH(),
                              0 );
  assert(window);

  // create renderer
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  assert(renderer);
  SDL_SetRenderDrawColor( renderer, 86, 29, 94, 255 );

  // also check the input pixels are the correct size
  assert( pixels.size() == Config::Instance.getSW() * Config::Instance.getSH() );

  for (int w=0; w<Config::Instance.getSW(); w++)
  for (int h=0; h<Config::Instance.getSH(); h++)
  {
    SDL_SetRenderDrawColor( 
      renderer,
      // indexing might be wrong way round int terms of rows/columns here
      static_cast<int>( 255 * (0.5 * std::sin( 0.1 * pixels[ w*Config::Instance.getSH() + h ]       ) + 0.5) ),
      static_cast<int>( 255 * (0.5 * std::sin( 0.1 * pixels[ w*Config::Instance.getSH() + h ] + 2.1 ) + 0.5) ),
      static_cast<int>( 255 * (0.5 * std::sin( 0.1 * pixels[ w*Config::Instance.getSH() + h ] + 4.2 ) + 0.5) ),
      255
     );
    SDL_RenderDrawPoint( renderer, w, h );
  }
  SDL_RenderPresent(renderer);
} 

void loopUntilExit()
/*
LOOPS UNTIL WINDOW IS CLOSED
*/
{
  bool quit = false;
  while ( not quit )
  {
    while ( SDL_PollEvent( &event ) )
    {
      if ( event.type == SDL_QUIT )
        quit = true;
    }
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
}

