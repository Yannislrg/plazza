/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** ConditionVariable unit tests
*/

#include <gtest/gtest.h>
#include <atomic>
#include "concurrency/Thread.hpp"
#include "fixtures/NotifyFixture.hpp"

TEST_F(NotifyFixture, NotifyOneWakesWaiter) {
  std::atomic<bool> notified{false};

  {
    Thread waiter([this, &notified] {
      _mutex.lock();
      while (!_ready) {
        _conditionVariable.wait(_mutex);
      }
      notified = true;
      _mutex.unlock();
    });

    _mutex.lock();
    _ready = true;
    _mutex.unlock();
    _conditionVariable.notifyOne();
  }

  EXPECT_TRUE(notified);
}

TEST_F(NotifyFixture, NotifyAllWakesAllWaiters) {
  constexpr int waiterCount = 3;
  std::atomic<int> notifiedCount{0};

  {
    Thread w1([this, &notifiedCount] {
      _mutex.lock();
      while (!_ready) {
        _conditionVariable.wait(_mutex);
      }
      ++notifiedCount;
      _mutex.unlock();
    });
    Thread w2([this, &notifiedCount] {
      _mutex.lock();
      while (!_ready) {
        _conditionVariable.wait(_mutex);
      }
      ++notifiedCount;
      _mutex.unlock();
    });
    Thread w3([this, &notifiedCount] {
      _mutex.lock();
      while (!_ready) {
        _conditionVariable.wait(_mutex);
      }
      ++notifiedCount;
      _mutex.unlock();
    });

    _mutex.lock();
    _ready = true;
    _mutex.unlock();
    _conditionVariable.notifyAll();
  }

  EXPECT_EQ(notifiedCount, waiterCount);
}
