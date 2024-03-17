#pragma once
#include <string>
#include <algorithm>
#include <vector>
#include <iostream>
#include <assert.h>

static constexpr int argc_max = 20;

class Config{
  private:
    std::vector<std::string> clArguments;
  public:
    int    _maxIters;
    double _xCentre;
    double _yCentre;
    double _zoom;

    int _screenWidth;
    int _screenHeight;

    Config() : _maxIters(64), _xCentre(0.0), _yCentre(0.0), _zoom(0.5),
    _screenWidth(1280), _screenHeight(720) {}

    void   setMaxIters(int iters) {_maxIters=iters;};
    int    getMaxIters()    const {return _maxIters;}
    void   setX(double x)         {_xCentre=x;}
    double getX()           const {return _xCentre;}
    void   setY(double y)         {_yCentre=y;}
    double getY()           const {return _yCentre;}
    void   setZoom(double z)      {_zoom=z;}
    double getZoom()        const {return _zoom;}

    int  getSW() const {return _screenWidth;}
    void setSW(int sw) {_screenWidth=sw;}
    int  getSH() const {return _screenHeight;}
    void setSH(int sh) {_screenHeight=sh;}

    std::string getCommandOption(const std::string& option)
    {
      auto iter = std::find(clArguments.begin(), clArguments.end(), option);
      // make sure we aren't at the end, and that there's something to read...
      // mind the sneaky increment in the "if" clause...
      if (iter != clArguments.end() and ++iter != clArguments.end()) {
        return *iter;
      }

      // no luck. return the empty string
      static const std::string emptyString("");
      return emptyString;
    }

    void readCommandLineArgs(int argc, char* argv[])
    {
      // push all the argv into the vector to hold them
      // start at 1; don't care about the binary name
      for (int i = 1; i < std::min(argc, argc_max); i++)
        clArguments.push_back(std::string(argv[i]));

      // get max iters
      std::string lArg;
      lArg = getCommandOption( "--iters" );
      if (lArg.empty()) {
        std::cout << "--iters argument not supplied. Default to " << getMaxIters() << ".\n";
        setMaxIters(64);
      }
      else{
        int iters = std::stoi( lArg );
        assert(iters > 0);
        setMaxIters( std::stoi( lArg ) );
        std::cout << "Running with " << getMaxIters() << " iterations.\n";
      }

      // get x centre
      lArg = getCommandOption( "--x" );
      if (lArg.empty()) {
        std::cout << "--x argument not supplied. Default to " << getX() << ".\n";
      }
      else{
        setX( std::stod( lArg ) );
        std::cout << "Running with centre = " << getX() << ".\n";
      }
      // get xycentre
      
      lArg = getCommandOption( "--y" );
      if (lArg.empty()) {
        std::cout << "--y argument not supplied. Default to " << getY() << ".\n";
      }
      else{
        setY( std::stod( lArg ) );
        std::cout << "Running with centre = " << getY() << ".\n";
      }
      // get zoom
      lArg = getCommandOption( "--zoom" );
      if (lArg.empty()) {
        std::cout << "--zoom argument not supplied. Default to " << getZoom() << ".\n";
      }
      else{
        setZoom( std::stod( lArg ) );
        std::cout << "Running with zoom = " << getZoom() << ". Increase this to get closer in!\n";
      }
    }
    
    std::pair<double, double> getXBounds()
    {
      std::pair<double, double> result;
      result.first = getX() - 0.5 / getZoom();
      result.second= getX() + 0.5 / getZoom();
      return result;
    }
    std::pair<double, double> getYBounds()
    {
      std::pair<double, double> result;
      result.first = getY() - 0.5 / getZoom();
      result.second= getY() + 0.5 / getZoom();
      return result;
    }

    static Config Instance;

};

