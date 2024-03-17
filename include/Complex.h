#pragma once
#include <vector>
class Complex
{

  public:
    double _x; double _y;

    Complex() = default;
    Complex(double x, double y) : _x(x), _y(y) {}
};

/*
Class to hold the integers we wanna pass
to the screen eventually.
Each point represents a pixel. 

Use the conversion routines inside the 
Globals class.

Template args represent number of arguments
in X direction vs Y direction

Make the methods virtual so we can override
them when we do fancier stuff later.

Define your own kernel function in each 
derived class!
*/
class Pixels{
  public:
    Pixels() = delete;
    Pixels(int x, int y) : _xPixels(x), _yPixels(y) {_pixels.resize(_xPixels*_yPixels);}
    virtual void processPixels();
    
    // kernel to return number of iterations
    int kernel(const double, const double);


  protected:
    const int _xPixels;
    const int _yPixels;
    // vector to store results of iters
    std::vector<int> _pixels;

  public:
    // expose results once they're ready
    const std::vector<int>& getPixels() const {return _pixels;};
};

class SimdPixels : public Pixels{
  public:
    SimdPixels() = delete;
    SimdPixels(int x, int y) : Pixels(x,y) {}

    // override the processPixels function
    virtual void processPixels() override;

    // New kernel function. We wanna do the same operation to 
    // four doubles at once. Pass in 1 real and 4 imags
    void kernel(const double x, const double y0, const double y1, const double y2, const double y3, int pixelX, int pixelY);
};
