#include "include/Vectorise.h"

void PooledThread::waitLoop()
{
  int                   processedItems = 0;
  std::function<void()> localFunc;
  while(true)
  {
    // lock the mutex
    std::unique_lock<std::mutex> lock( this->_threadMtx );

    // wait until busy == true
    _cv.wait(
      lock,
      [=](){ return (_busy or _shutdown); }
    );

    if (_shutdown) break;

    /*
    Loop again here. Process each element
    in the queue one by one. If we catch
    an out-of-range error, break and wait
    under the cv
    */
    while(true)
    {
      try
      {
        localFunc = _workQueue.at( processedItems );
      }
      catch(...)
      {
        // We processed everything in the workQueue, 
        // and ended up here. Break and wait under
        // the condition variable.
        break;
      }

      // no exception. Run the function
      localFunc();
      processedItems++;
    }

    // no longer busy
    _busy = false;

  }
}

void PooledThread::submit( std::function<void()>&& work )
{
  _workQueue.push_back( work );

  _busy = true;
  this->_cv.notify_one();
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
