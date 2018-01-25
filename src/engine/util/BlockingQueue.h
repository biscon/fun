//
// Created by bison on 25-01-2018.
//

#ifndef GAME_BLOCKINGQUEUE_H
#define GAME_BLOCKINGQUEUE_H

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

// needed because GCC on windows sucks
#ifdef __WIN32__
#include "../mingw.thread.h"
#include "../mingw.condition_variable.h"
#endif

template <typename T>
class Queue
{
 public:

  T pop()
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty())
    {
      cond_.wait(mlock);
    }
    auto val = queue_.front();
    queue_.pop();
    return val;
  }

  void pop(T& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty())
    {
      cond_.wait(mlock);
    }
    item = queue_.front();
    queue_.pop();
  }

  void push(const T& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(item);
    mlock.unlock();
    cond_.notify_one();
  }
  Queue()=default;
  Queue(const Queue&) = delete;            // disable copying
  Queue& operator=(const Queue&) = delete; // disable assignment

 private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable cond_;
};

#endif //GAME_BLOCKINGQUEUE_H
