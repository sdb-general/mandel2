I note that in Vectorise.cpp, we don't need to call ```thread.join()```. We could simply let the threads continue waiting in the condition variable and clear
out the work queues so that they could be reused, meaning we avoid the cost of restarting these threads.
