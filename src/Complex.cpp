#include "include/Complex.h"
#include "include/Globals.h"
#include "include/Utils.h"

#include <immintrin.h>
#include <emmintrin.h>

void Pixels::processPixels()
{
  StopWatch s("Process Pixels");
  // get X and Y bounds
  auto xBounds = Config::Instance.getXBounds();
  double xLow  = xBounds.first; 
  double xHigh = xBounds.second; 
  auto yBounds = Config::Instance.getYBounds();
  double yHigh = yBounds.second;
  double yLow  = yBounds.first;

  double dX    = (xHigh - xLow) / _xPixels;
  double dY    = (yHigh - yLow) / _yPixels;

  for (int x=0; x<_xPixels; x++)
  {
    double doubleX = xLow + x*dX;
    for (int y=0; y<_yPixels; y++)
    {
      double doubleY = yLow + y*dY;
      _pixels[ x*_yPixels + y ] = kernel(doubleX, doubleY);
    }
  }
}

int Pixels::kernel(const double real, const double imag)
{
  int iters = 0;
  double zR(0); double zI(0);
  double tempReal;
  while (++iters<Config::Instance.getMaxIters() and (zR*zR+zI*zI<4))
  {
    tempReal = zR;
    zR = zR * zR - zI * zI + real;
    zI = 2*tempReal   * zI + imag;
  }
  return iters;
}


/*=================================================================*/
/*
  SIMD version
*/
/*=================================================================*/

void SimdPixels::processPixels()
{
  StopWatch s("Process Pixels SIMD");
  // get X and Y bounds
  // we assume that the template params actually 
  // match the number of pixels we wanna process

  auto xBounds = Config::Instance.getXBounds();
  double xLow  = xBounds.first; 
  double xHigh = xBounds.second; 
  auto yBounds = Config::Instance.getYBounds();
  double yHigh = yBounds.second;
  double yLow  = yBounds.first;

  double dX    = (xHigh - xLow) / _xPixels;
  double dY    = (yHigh - yLow) / _yPixels;

  for (int x=0; x<_xPixels; x++)
  {
    double doubleX = xLow + x*dX;

    // jump manage 4 doubles at once
    for (int y=0; y<_yPixels; y+=4)
    {
      double doubleY = yLow + y*dY;

      // set the pixel vector inside this function
      kernel( doubleX, doubleY, doubleY+dY, doubleY+2*dY, doubleY+3*dY, x, y );
    }
  }
}

/*
  Return true if any of the bits of a
  are nonzero
*/
inline bool anyContinue( __m256d a )
{
  // static const __m256d zeroMask = _mm256_set_pd(0.,0.,0.,0.);
  bool output = false;
  for (size_t i=0; i<4; i++)
    output |= ( a[i] == 1.0 );
  return output;
}

void SimdPixels::kernel(const double x, const double y0, const double y1, const double y2, const double y3, int pixelX, int pixelY)
{
  // set some registers
  __m256d real( _mm256_set_pd(  x,  x,  x,  x )), imag(_mm256_set_pd(  y0,  y1,  y2,  y3  ));
  static const __m256d two ( _mm256_set_pd(2.0,2.0,2.0,2.0) );
  static const __m256d four( _mm256_set_pd(4.0,4.0,4.0,4.0) );
  __m256d zR, zI, zR2, zI2;
  __m256d twoZRtimesZI;   // intermediate variable for zI calc
  __m256d absValue; // used for checking if our abs value is maxed out yet
  __m256d iters;
  __m256d continueToIter = _mm256_set_pd(1.,1.,1.,1.);
  // __m256d increment;      // use this to 

  static const __m256d maxIters = _mm256_set_pd((double)Config::Instance.getMaxIters(), (double)Config::Instance.getMaxIters(), (double)Config::Instance.getMaxIters(), (double)Config::Instance.getMaxIters() );;
  static const __m256d oneMask  = _mm256_set_pd(1.,1.,1.,1.);

  // set the registers
  zR             = _mm256_set_pd(0.,0.,0.,0.);
  zI             = _mm256_set_pd(0.,0.,0.,0.);
      
  iters          = _mm256_set_pd(1.,1.,1.,1.);
  absValue       = _mm256_set_pd(0.,0.,0.,0.); // check if our abs value has eceed 4
  absValue       = _mm256_set_pd(0.,0.,0.,0.); 

  while( anyContinue(continueToIter) )
  {
    zR2 = _mm256_mul_pd(zR, zR);
    zI2 = _mm256_mul_pd(zI, zI);

    // horrid nesting of multiplications
    // twoZRtimesZI <- 2*zR*zI
    twoZRtimesZI = _mm256_mul_pd( two, _mm256_mul_pd( zR,zI ) );

    // update zR
    zR = _mm256_sub_pd( zR2, zI2 );
    zR = _mm256_add_pd( zR, real );

    // update zI
    zI = _mm256_add_pd( twoZRtimesZI, imag );

    // check if we need to stop due to absValue exceeded
    absValue = _mm256_cmp_pd( _mm256_add_pd( zR2, zI2 ), four, _CMP_LT_OQ );

    // check if we have exceeded max iterations first
    continueToIter = _mm256_cmp_pd( iters , maxIters, _CMP_LT_OQ );

    // perform AND on these two conditinons
    continueToIter = _mm256_and_pd( absValue, continueToIter );

    // finally, AND this with the oneMask, so we don't set any 
    // bits other than the ones we want
    continueToIter = _mm256_and_pd( continueToIter, oneMask );

    // now we can increment
    iters = _mm256_add_pd( iters, continueToIter );
  }

  // extraction
  for (int i=0; i<4; i++)
  {
    _pixels[ pixelX*_yPixels + (pixelY+i) ] = (int)iters[3-i];
  }
  
}
