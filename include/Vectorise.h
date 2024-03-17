#pragma once
#include <thread>
#include <deque>
#include <mutex>
#include <cassert>
#include <atomic>
#include <functional>
#include <condition_variable>
#include <iostream>
#include <vector>

/*
  Stragtegy: 
    SUBMIT STD::FUNCTION OBJECTS TO THE POOL
    PLACE THEM ONTO THE STACK OF EACH THREAD
    WHICH EITHER WAITS IN LOOP or PERFORMS WORK IN THE QUEUE
*/

class PooledThread{
  private:
    std::atomic_bool                     _busy;
    std::atomic_bool                     _shutdown;
    std::mutex                           _mtx;
    std::condition_variable              _cv;

    // Use a Deque so that references are not 
    // invalidated upon insertion/deletion
    std::deque<std::function<void()>>    _workQueue;
    std::thread                          _thread;

  public:
    void submit( std::function<void()>&& work );
    void notify();
    void join();
    void waitLoop();
    PooledThread();
};



class ThreadPool{
  private:
    const int                          nThreads;
    std::vector<PooledThread>          pool;
  
  public:
    ThreadPool(): nThreads(std::thread::hardware_concurrency()), pool( std::vector<PooledThread>(nThreads) ) {std::cout << "initThreadPool\n";}

    void submit( std::function<void()>&& work )
    {
      // use this to pick a thread to hand out to
      static int threadNumber;
      threadNumber = (threadNumber+1) % nThreads;

      // submit this work to the next thread in line
      pool[threadNumber].submit( std::move(work) );
    }

    //! Call this only when we have submitted all the work
    void run_and_wait()
    {
      for (int i=0; i<nThreads; i++)
      {
        // notify to start processing
        pool[i].notify();
        pool[i].join();
      }
    }

};

