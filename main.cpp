#include <iostream>
#include "include/Utils.h"
#include "include/Screen.h"
#include "include/Globals.h"
#include "include/Complex.h"

// init the global copy
Config Config::Instance;

int main(int argc, char* argv[])
{
  StopWatch s("Main");
  Config::Instance.readCommandLineArgs(argc,argv);

  #if defined(SIMD)
  SimdPixels p(Config::Instance.getSW(),Config::Instance.getSH());
  #else
  Pixels     p(Config::Instance.getSW(),Config::Instance.getSH());
  #endif
  p.processPixels();
  
  createScreenAndRender(p.getPixels());
  loopUntilExit();

  return 0;
}