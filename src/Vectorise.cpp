#include "include/Vectorise.h"

void PooledThread::waitLoop()
{
  std::function<void()> localFunc;
  while(true)
  {
    // lock the mutex
    std::unique_lock<std::mutex> lock( _mtx );

    // wait until busy == true
    _cv.wait(
      lock,
      [=](){ return (_busy or _shutdown); }
    );

    if (_shutdown) break;

    /*
    If we're here we assume the queue is 
    full and we can bulk-process everything
    */
    for (int i=0; i<_workQueue.size(); i++)
      _workQueue[i]();

    // no longer busy
    _busy = false;

  }
}

void PooledThread::submit( std::function<void()>&& work )
{
  _workQueue.push_back( work );

  _busy = true;
  _cv.notify_one();
}

/**
 * Notifies that work is ready.
 * Run through it as fast as possible
*/
void PooledThread::notify()
{
  _busy = true;
  _cv.notify_one();
}

PooledThread::PooledThread() :
  _busy(false), _shutdown(false) 
{
  _thread = std::thread( &PooledThread::waitLoop, this );
}


void PooledThread::join()
{
  _shutdown = true;
  _busy     = false;
  _cv.notify_one();
  _thread.join();
}
