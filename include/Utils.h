#pragma once
#include <chrono>
#include <string>
#include <iostream>

class StopWatch{
  public:
	std::chrono::high_resolution_clock::time_point _startTime;
  std::string _operationName;

   inline StopWatch(std::string operationName);
   StopWatch() = delete;
   inline ~StopWatch();
};

inline StopWatch::StopWatch(std::string operationName)
  : _operationName(operationName)
{
  std::cout << "\n++++++++++++++++++++++++\n"
            << "Starting " << _operationName.c_str() << "\n"
            << "++++++++++++++++++++++++\n\n";
  _startTime = std::chrono::high_resolution_clock::now();
}

inline StopWatch::~StopWatch()
{
  auto now      = std::chrono::high_resolution_clock::now();
  auto duration =  std::chrono::duration_cast<std::chrono::milliseconds>(now - _startTime);
  std::cout << "\n++++++++++++++++++++++++\n"
            << "Finishing " << _operationName.c_str() << "\n"
            << "Time taken " << duration.count() << "ms\n"
            << "++++++++++++++++++++++++\n\n";
}

